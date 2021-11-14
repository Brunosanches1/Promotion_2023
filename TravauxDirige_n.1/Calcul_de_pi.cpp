# include <chrono>
# include <random>
# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <mpi.h>
# include <vector>
# include <array>

// Attention , ne marche qu'en C++ 11 ou supérieur :
double approximate_pi(int nbp, unsigned long nbSamples ) 
{
    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    myclock::duration d = beginning.time_since_epoch();
    unsigned seed = d.count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution <double> distribution ( -1.0 ,1.0);
    unsigned long nbDarts = 0;
    // Throw nbSamples darts in the unit square [-1 :1] x [-1 :1]
    for ( unsigned sample = 0 ; sample < nbSamples / nbp ; ++ sample ) {
        double x = distribution(generator);
        double y = distribution(generator);
        // Test if the dart is in the unit disk
        if ( x*x+y*y<=1 ) nbDarts ++;
    }
    // Number of nbDarts throwed in the unit disk
    double ratio = double(nbDarts)/double(nbSamples);
    return 4*ratio;
}

int main( int nargs, char* argv[] )
{
	// On initialise le contexte MPI qui va s'occuper :
	//    1. Créer un communicateur global, COMM_WORLD qui permet de gérer
	//       et assurer la cohésion de l'ensemble des processus créés par MPI;
	//    2. d'attribuer à chaque processus un identifiant ( entier ) unique pour
	//       le communicateur COMM_WORLD
	//    3. etc...
	MPI_Init( &nargs, &argv );
	// Pour des raisons de portabilité qui débordent largement du cadre
	// de ce cours, on préfère toujours cloner le communicateur global
	// MPI_COMM_WORLD qui gère l'ensemble des processus lancés par MPI.
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	// On interroge le communicateur global pour connaître le nombre de processus
	// qui ont été lancés par l'utilisateur :
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	// On interroge le communicateur global pour connaître l'identifiant qui
	// m'a été attribué ( en tant que processus ). Cet identifiant est compris
	// entre 0 et nbp-1 ( nbp étant le nombre de processus qui ont été lancés par
	// l'utilisateur )
	int rank;
	MPI_Comm_rank(globComm, &rank);
	// Création d'un fichier pour ma propre sortie en écriture :
	std::stringstream fileName;
	fileName << "Output" << std::setfill('0') << std::setw(5) << rank << ".txt";
	std::ofstream output( fileName.str().c_str() );

	int nbSamples = 100000000;
	if (rank == 0) {
		double pi;
		if(nbp == 1) {
			pi = approximate_pi(nbp, nbSamples);
			
		}
		else {
            // MPI_Status status;
            // double ratio;
            // pi = 0;
            // for(int i = 1; i < nbp; i++) {
            //     MPI_Recv(&ratio, 1, MPI_DOUBLE, i, MPI_ANY_TAG, globComm, &status);
            //     pi += ratio;
            // }

            std::vector<MPI_Request> requests(nbp-1);
            std::vector<MPI_Status> status(nbp-1);

            std::vector<double> ratios(nbp-1);
            pi = 0;
            for(int i = 1; i < nbp; i++) {
                MPI_Irecv(&ratios[i-1], 1, MPI_DOUBLE, i, MPI_ANY_TAG, globComm, &requests[i-1]);
            }
            MPI_Request *request_array = &requests[0];
            MPI_Status *status_array = &status[0];

            MPI_Waitall(nbp-1, request_array, status_array);
            for(int i = 1; i < nbp; i++) {
                pi += ratios[i-1];
            }
		}
		output << "Calculated pi = " << pi << std::endl;
	}
	else {
		double ratio;
		// Faire l'appelle avec nbp-1, seulement nbp-1 processus vont realiser le calcul
		ratio = approximate_pi(nbp-1, nbSamples);
		output << "Calculated Ratio = " << ratio << std::endl;
		MPI_Send(&ratio, 1, MPI_DOUBLE, 0, 1234, globComm);
	}

	output.close();
	// A la fin du programme, on doit synchroniser une dernière fois tous les processus
	// afin qu'aucun processus ne se termine pendant que d'autres processus continue à
	// tourner. Si on oublie cet instruction, on aura une plantage assuré des processus
	// qui ne seront pas encore terminés.
	MPI_Finalize();
	return EXIT_SUCCESS;
}
