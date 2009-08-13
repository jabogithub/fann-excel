#include <fann.h>
#include "XGetopt.h"
#include <iostream>
#include <string>

void printUsage(const char* prgName)
{
	std::cout << "Usage: " << prgName << std::endl
		<< "\t-n NetFile" << std::endl
		<< "\t-d DataTrainFile" << std::endl
		<< "\t[-m MaxEpochs]" << std::endl
		<< "\t[-e DesiredError]" << std::endl
		<< "\t[-f ReportFrequency]" << std::endl
		<< "\t[-a Algorithm]\t0:INCREMENTAL, 1:BATCH, 2:RPROP, 3:QUICKPROP" << std::endl
		<< "\t[-l LearningRate]" << std::endl
		<< "\t[-b BitFailLimit]" << std::endl
		<< "\t[-h]\t\tHelp" << std::endl;
}

int FANN_API
cbPrintStatusToStdOut(struct fann *ann, struct fann_train_data *train,
					  unsigned int max_epochs, unsigned int epochs_between_reports,
					  float desired_error, unsigned int epochs)
{
	std::cout << "Epochs: " << epochs << ".\t" 
		<< "MSE: " << fann_get_MSE(ann) << ".\t"
		<<  "bit_fail: " << fann_get_bit_fail(ann) << std::endl << std::flush;
	return 0;
}

int
main(int argc, char *argv[])
{
	// Parse the command line
	char c;
	std::string annFile, trainFile;
	unsigned int maxEpochs = 100;
	float desiredError = (fann_type)1e-10;
	int reportFrequency = 10;
	fann_train_enum alg = FANN_TRAIN_RPROP;
	float learningRate = (float)0.7;
	float bitFail = (float)0.35;

	while ((c = getopt(argc, argv, "hn:d:m:e:f:a:l:b:")) != EOF)
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
		case 'f':
			reportFrequency = atoi(optarg);
			break;
		case 'a':
			switch(atoi(optarg))
			{
			case 0:	alg = FANN_TRAIN_INCREMENTAL; 
				std::cout << "Setting algorithm to INCREMENTAL" << std::endl;
				break;
			case 1: alg = FANN_TRAIN_BATCH; 
				std::cout << "Setting algorithm to BATCH" << std::endl;
				break;
			case 2: alg = FANN_TRAIN_RPROP; 
				std::cout << "Setting algorithm to RPROP" << std::endl;
				break;
			case 3: alg = FANN_TRAIN_QUICKPROP; 
				std::cout << "Setting algorithm to QUICKPROP" << std::endl;
				break;
			}
			break;
		case 'l':
			learningRate = (float)atof(optarg);
			break;
		case 'b':
			bitFail = (float)atof(optarg);
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

	const int epochsBetweenReports = maxEpochs / reportFrequency;
	
	// Load the network form the file
	std::cout << "Reading network file " << annFile << std::endl << std::flush;
	struct fann* ann = fann_create_from_file(annFile.c_str());
	if (ann == NULL)
		return -1;
	fann_set_callback(ann, cbPrintStatusToStdOut);
	fann_set_training_algorithm(ann, alg);
	std::cout << "Network algorithm: " << fann_get_training_algorithm(ann) << std::endl;
	fann_set_learning_rate(ann, learningRate);
	std::cout << "Learning rate: " << fann_get_learning_rate(ann) << std::endl;
	fann_set_bit_fail_limit(ann, bitFail);
	std::cout << "Bit Fail: " << fann_get_bit_fail_limit(ann) << std::endl;

	// Train the network
	std::cout << "Training from file " << trainFile << std::endl << std::flush;
	fann_train_on_file(ann, trainFile.c_str(), maxEpochs, epochsBetweenReports, desiredError);
	
	double mse = fann_get_MSE(ann);
	std::cout << "Finished MSE=" << mse << std::endl;

	// Save and clean-up
	fann_save(ann, annFile.c_str());
	fann_destroy(ann);
	
	return 0;
}