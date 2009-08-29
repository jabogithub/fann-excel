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
	float dError = (fann_type)1e-10;
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
			dError = (float)atof(optarg);
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
	{
		std::cout << "Cannot open network file " << annFile << std::endl;
		return -1;
	}
	// Set up parameters
	fann_set_callback(ann, cbPrintStatusToStdOut);
	fann_set_training_algorithm(ann, alg);
	std::cout << "Network algorithm: " << fann_get_training_algorithm(ann) << std::endl;
	fann_set_learning_rate(ann, learningRate);
	std::cout << "Learning rate: " << fann_get_learning_rate(ann) << std::endl;
	fann_set_bit_fail_limit(ann, bitFail);
	std::cout << "Bit Fail: " << fann_get_bit_fail_limit(ann) << std::endl;

	// Train the network
	std::string bestTrainNetFile = annFile + ".bestTrain.net";
	std::string bestBitNetFile = annFile + ".bestBit.net";
	// Reset and recalculate MSE for Train and Test data
	double mseTrain = 1e9;
	double mseTrainMin = mseTrain;
	int bitFailMin = 1e9;

	std::cout << "--> fannTrainOnData(" << annFile << ")" << std::endl 
		<< "Max epochs: " << maxEpochs << ".\t"
		<< "Desired error: " << dError << std::endl << std::flush;

	struct fann_train_data* trainData = fann_read_train_from_file(trainFile.c_str());
	if (trainData == NULL)
	{
		std::cout << "Cannot open training file " << trainFile << std::endl;
		return -1;
	}

	for (unsigned int i = 0; i < maxEpochs; ++i)
	{
		mseTrain = fann_train_epoch(ann, trainData);
		// Report to log file + save the best networks if necessary
		if(i % epochsBetweenReports == 0)
		{
			int bitFail = fann_get_bit_fail(ann);
			std::cout << "Epochs: " << i << "\tMSE(train): " << mseTrain << "\t"
				<< "Bit fail: " << bitFail << "\t";
			if (mseTrain < mseTrainMin)
			{
				mseTrainMin = mseTrain;
				fann_save(ann, bestTrainNetFile.c_str());
				std::cout << "saving bestTrain.net\t";
			}
			if (bitFail < bitFailMin)
			{
				bitFailMin = bitFail;
				fann_save(ann, bestBitNetFile.c_str());
				std::cout << "saving bestBit.net";
			}
			std::cout << std::endl << std::flush;
		}
		if (mseTrain < dError)
			break;
	}
		
	// Save the best
	std::cout << "*** Finished *** MSE(lastTrain): " << mseTrain << ", "
		<< "MSE(bestTrain): " << mseTrainMin << std::endl << std::flush; 
	fann_save(ann, annFile.c_str());

	// clean-up
	fann_destroy_train(trainData);
	fann_destroy(ann);
	
	return 0;
}