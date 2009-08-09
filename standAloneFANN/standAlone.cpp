#include <fann.h>
#include <stdio.h>

int
main()
{
	int maxEpochs = 100;
	const int epochsBetweenReports = (int)maxEpochs/5;
	
	// Load the network form the file
	struct fann* ann = fann_create_from_file("TEST-deleteme.net");

	// Train the network
	FILE* logFile = fopen("FANN-excel.log","a");
	fann_train_on_file(ann, "TEST-deleteme.dat", maxEpochs, epochsBetweenReports, 0.00000000001,logFile);
	double mse = fann_get_MSE(ann);

	fann_destroy(ann);
	
	return 0;
}