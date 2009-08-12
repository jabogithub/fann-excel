#include <fann.h>
#include <stdio.h>
#include "XGetopt.h"
#include <iostream>
#include <string>

void printUsage(const char* prgName)
{
	std::cout << "Usage: " << prgName<< " -n NetFile -d DataTrainFile [-m MaxEpochs -e DesiredError -l LogFile -h]" << std::endl;
}

int
main(int argc, char *argv[])
{
	// Parse the command line
	char c;
	std::string annFile, trainFile;
	std::string logFileS = "FANN-excel.log";
	unsigned int maxEpochs = 100;
	float desiredError = (fann_type)1e-10;

	while ((c = getopt(argc, argv, "hn:d:m:e:l:")) != EOF)
		switch (c)
		{
		case 'n':
			annFile = std::string(optarg);
			break;
		case 'd':
			trainFile = std::string(optarg);
			break;
		case 'm':
			maxEpochs = atoi(optarg);
			break;
		case 'e':
			desiredError = (float)atof(optarg);
			break;
		case 'l':
			logFileS = std::string(optarg);
			break;
		case 'h':
			printUsage(argv[0]);
			return 0;
		case '?':
			std::cout << "ERROR: illegal option " << argv[optind-1] << std::endl;
			printUsage(argv[0]);
			return -1;
		default:
			std::cout << "WARNING: no handler for option " << c << std::endl;
			printUsage(argv[0]);
			return -1;
		}
	// Check for no parameters
	if (argc < 3)
	{
		printUsage(argv[0]);
		return -1;
	}

	const int epochsBetweenReports = maxEpochs / 10;
	FILE* logFile = fopen(logFileS.c_str(),"a");
	
	// Load the network form the file
	fprintf(logFile, "Reading network file %s\n", annFile.c_str());
	struct fann* ann = fann_create_from_file(annFile.c_str());
	if (ann == NULL)
		return -1;

	// Train the network
	fprintf(logFile, "Training from file %s\n", trainFile.c_str());
	fann_train_on_file(ann, trainFile.c_str(), maxEpochs, epochsBetweenReports, desiredError,logFile);
	
	double mse = fann_get_MSE(ann);
	// Save and clean-up
	fann_save(ann, annFile.c_str());

	fprintf(logFile, "MSE=%f", mse);
	fclose(logFile);
	fann_destroy(ann);
	
	return 0;
}