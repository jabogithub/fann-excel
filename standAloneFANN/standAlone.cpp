#include <fann.h>
#include <stdio.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	// Check input consistency
	if (argc < 3)
	{
		printf("Usage: %s ANN_file training_file [maxEpochs [desiredError]]", argv[0]);
		return -1;
	}

	// Read in input parameters
	unsigned int maxEpochs = 100;
	fann_type desiredError = (fann_type)1e-10;
	const char* annFile = argv[1];
	const char* trainFile = argv[2];
	if (argc > 3)
		maxEpochs = atoi(argv[3]);
	if (argc > 4)
		desiredError = (fann_type)atof(argv[4]);
	const int epochsBetweenReports = maxEpochs / 10;
	FILE* logFile = fopen("FANN-excel.log","a");
	
	// Load the network form the file
	fprintf(logFile, "Reading network file %s\n", annFile);
	struct fann* ann = fann_create_from_file(annFile);
	if (ann == NULL)
		return -1;

	// Train the network
	fprintf(logFile, "Training from file %s\n", trainFile);
	fann_train_on_file(ann, trainFile, maxEpochs, epochsBetweenReports, desiredError,logFile);
	
	double mse = fann_get_MSE(ann);
	// Save and clean-up
	fann_save(ann, annFile);

	fprintf(logFile, "MSE=%f", mse);
	fclose(logFile);
	fann_destroy(ann);
	
	return 0;
}