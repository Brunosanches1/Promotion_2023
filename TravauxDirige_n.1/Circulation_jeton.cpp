# include <iostream>
# include <cstdlib>
# include <mpi.h>

int main( int nargs, char* argv[] )
{

	MPI_Init( &nargs, &argv );

	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);

	int nbp;
	MPI_Comm_size(globComm, &nbp);

	int rank;
	MPI_Comm_rank(globComm, &rank);

	// int jeton, received;
	// MPI_Status status;
	// if(rank == 0) {
	// 	jeton = 5;

	// 	std::cout << "Processus " << rank 
	// 				<< " envoie le jeton " << jeton 
	// 				<< std::endl;

	// 	MPI_Send(&jeton, 1, MPI_INT, (rank+1)%nbp, 0, globComm);
	// 	MPI_Recv(&received, 1, MPI_INT, nbp-1, MPI_ANY_TAG, globComm, &status);

	// 	std::cout << "Processus " << rank 
	// 				<< " reçoi le jeton " << received 
	// 				<< std::endl;

	// }
	// else {
	// 	MPI_Recv(&received, 1, MPI_INT, rank-1, MPI_ANY_TAG, globComm, &status);

	// 	std::cout << "Processus " << rank 
	// 				<< " reçoit le jeton " << received 
	// 				<< std::endl;

	// 	jeton = received + 1;

	// 	std::cout << "Processus " << rank 
	// 				<< " envoie le jeton " << jeton 
	// 				<< std::endl;

	// 	MPI_Send(&jeton, 1, MPI_INT, (rank+1)%nbp, 0, globComm);
	// }

	int jeton, received;
	MPI_Status status;
	MPI_Request request;

	jeton = rank * 9 + 4;

	std::cout << "Processus " << rank 
				<< " envoie le jeton " << jeton 
				<< std::endl;

	MPI_Isend(&jeton, 1, MPI_INT, (rank+1)%nbp, 0, globComm, &request);
	MPI_Recv(&received, 1, MPI_INT, (rank-1 < 0 ? nbp-1 : rank-1), MPI_ANY_TAG, globComm, &status);

	std::cout << "Processus " << rank 
				<< " reçoi le jeton " << received 
				<< std::endl;

	MPI_Wait(&request, &status);

	MPI_Finalize();
	return EXIT_SUCCESS;
}
