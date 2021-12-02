# include <iostream>
# include <cstdlib>
# include <string>
# include <chrono>
# include <cmath>
# include <vector>
# include <fstream>
# include <mpi.h>
# include <tbb/tbb.h>


/** Une structure complexe est définie pour la bonne raison que la classe
 * complex proposée par g++ est très lente ! Le calcul est bien plus rapide
 * avec la petite structure donnée ci--dessous
 **/
struct Complex
{
    Complex() : real(0.), imag(0.)
    {}
    Complex(double r, double i) : real(r), imag(i)
    {}
    Complex operator + ( const Complex& z )
    {
        return Complex(real + z.real, imag + z.imag );
    }
    Complex operator * ( const Complex& z )
    {
        return Complex(real*z.real-imag*z.imag, real*z.imag+imag*z.real);
    }
    double sqNorm() { return real*real + imag*imag; }
    double real,imag;
};

std::ostream& operator << ( std::ostream& out, const Complex& c )
{
  out << "(" << c.real << "," << c.imag << ")" << std::endl;
  return out;
}

/** Pour un c complexe donné, calcul le nombre d'itérations de mandelbrot
 * nécessaires pour détecter une éventuelle divergence. Si la suite
 * converge, la fonction retourne la valeur maxIter
 **/
int iterMandelbrot( int maxIter, const Complex& c)
{
    Complex z{0.,0.};
    // On vérifie dans un premier temps si le complexe
    // n'appartient pas à une zone de convergence connue :
    // Appartenance aux disques  C0{(0,0),1/4} et C1{(-1,0),1/4}
    if ( c.real*c.real+c.imag*c.imag < 0.0625 )
        return maxIter;
    if ( (c.real+1)*(c.real+1)+c.imag*c.imag < 0.0625 )
        return maxIter;
    // Appartenance à la cardioïde {(1/4,0),1/2(1-cos(theta))}    
    if ((c.real > -0.75) && (c.real < 0.5) ) {
        Complex ct{c.real-0.25,c.imag};
        double ctnrm2 = sqrt(ct.sqNorm());
        if (ctnrm2 < 0.5*(1-ct.real/ctnrm2)) return maxIter;
    }
    int niter = 0;
    while ((z.sqNorm() < 4.) && (niter < maxIter))
    {
        z = z*z + c;
        ++niter;
    }
    return niter;
}

/**
 * On parcourt chaque pixel de l'espace image et on fait correspondre par
 * translation et homothétie une valeur complexe c qui servira pour
 * itérer sur la suite de Mandelbrot. Le nombre d'itérations renvoyé
 * servira pour construire l'image finale.
 
 Sortie : un vecteur de taille W*H avec pour chaque case un nombre d'étape de convergence de 0 à maxIter
 MODIFICATION DE LA FONCTION :
 j'ai supprimé le paramètre W étant donné que maintenant, cette fonction ne prendra plus que des lignes de taille W en argument.
 **/
void 
computeMandelbrotSetRow( int W, int H, int maxIter, int num_ligne, int* pixels)
{
    // Calcul le facteur d'échelle pour rester dans le disque de rayon 2
    // centré en (0,0)
    double scaleX = 3./(W-1);
    double scaleY = 2.25/(H-1.);
    //
    // On parcourt les pixels de l'espace image :
    for ( int j = 0; j < W; ++j ) {
       Complex c{-2.+j*scaleX,-1.125+ num_ligne*scaleY};
       pixels[j] = iterMandelbrot( maxIter, c );
    }
}

std::vector<int>
computeMandelbrotSet( int W, int H, int maxIter )
{
    std::chrono::time_point<std::chrono::system_clock> start, end;

    int nbp;
    MPI_Comm_size(MPI_COMM_WORLD, &nbp);

    int nt = 60;

    std::vector<int> pixels(W*nt);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Barrier(MPI_COMM_WORLD);

    start = std::chrono::system_clock::now();
    // On parcourt les pixels de l'espace image :
    if (rank == 0) {
        std::vector<int> result(W);
        pixels.resize(W*H);
        

        MPI_Status status;
        if (nbp > 1) {
            int count_line = 0, lines_received = 0;
            for(int i = 1; i < nbp; i++) {
                MPI_Send(&count_line, 1, MPI_INT, i, count_line, MPI_COMM_WORLD);
                count_line+= nt;
            }
            
            while(lines_received < H) {
                MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                MPI_Recv(pixels.data() + W*status.MPI_TAG, W*nt, MPI_INT, status.MPI_SOURCE, 
                    status.MPI_TAG, MPI_COMM_WORLD, &status);

                lines_received+= nt;
                if (count_line < H) {
                    MPI_Send(&count_line, 1, MPI_INT, status.MPI_SOURCE, count_line, MPI_COMM_WORLD);
                    count_line+=nt;
                }
                else {
                    int stop_val = -1;
                    MPI_Send(&stop_val, 1, MPI_INT, status.MPI_SOURCE, count_line, MPI_COMM_WORLD);
                }
                
            }
        }
        else {
            // #pragma omp parallel for schedule(dynamic) num_threads(H)
            // for ( int i = 0; i < H; ++i ) {
            //     computeMandelbrotSetRow(W, H, maxIter, i, pixels.data() + W*(H - i - 1));
            // }

            auto partitioner = tbb::simple_partitioner();
            tbb::parallel_for(
                tbb::blocked_range<unsigned>(0, H, nt),
                [&] (tbb::blocked_range<unsigned> const& r) {
                    for ( int i = r.begin(); i != r.end(); ++i ) {
                        computeMandelbrotSetRow(W, H, maxIter, i, pixels.data() + W*(H - i - 1));
                    }
                },
                partitioner
            );
        }
        
    }
    else {
        int line_to_work;
        MPI_Status status;
        MPI_Recv(&line_to_work, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        while (line_to_work != -1) {
            #pragma omp parallel for schedule(dynamic) num_threads(nt)
            for(int i = 0; i < nt; i++) {
                computeMandelbrotSetRow(W, H, maxIter, line_to_work + i, pixels.data() + i*W);
            }
            
            MPI_Send(pixels.data(), W*nt, MPI_INT, 0, line_to_work, MPI_COMM_WORLD);
            MPI_Recv(&line_to_work, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }
        
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Temps calcul ensemble mandelbrot : " << elapsed_seconds.count() 
              << " rank: " << rank << std::endl;

    return pixels;
}

/** Construit et sauvegarde l'image finale **/
void savePicture( const std::string& filename, int W, int H, const std::vector<int>& nbIters, int maxIter )
{
    double scaleCol = 1./maxIter;//16777216
    std::ofstream ofs( filename.c_str(), std::ios::out | std::ios::binary );
    ofs << "P6\n"
        << W << " " << H << "\n255\n";
    for ( int i = 0; i < W * H; ++i ) {
        double iter = scaleCol*nbIters[i];
        unsigned char r = (unsigned char)(256 - (unsigned (iter*256.) & 0xFF));
        unsigned char b = (unsigned char)(256 - (unsigned (iter*65536) & 0xFF));
        unsigned char g = (unsigned char)(256 - (unsigned( iter*16777216) & 0xFF));
        ofs << r << g << b;
    }
    ofs.close();
}

int main(int argc, char *argv[] ) 
 { 
    const int W = 800;
    const int H = 600;
    // Normalement, pour un bon rendu, il faudrait le nombre d'itérations
    // ci--dessous :
    //const int maxIter = 16777216;
    const int maxIter = 8*65536;

    //Initialization MPI
    MPI_Init(&argc, &argv);

    int nbp;
    MPI_Comm_size(MPI_COMM_WORLD, &nbp);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    auto iters = computeMandelbrotSet( W, H, maxIter );

    if(rank == 0) {
      savePicture("mandelbrot2.tga", W, H, iters, maxIter);
    }
    
    MPI_Finalize();
    return EXIT_SUCCESS;
 }
    
