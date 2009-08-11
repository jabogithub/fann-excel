#include "Interface.h"

#include <sstream>
#include <fstream>
#include <stdio.h>

#include <fann.h>
#include <xlw/xlw.h>

static const char* logFileName = "FANN-excel.log";	// global logfile
static const unsigned int reportingFrequency = 10;	// number of reporting lines during one training
static const double defaultError = 1e-3;			// default MSE

std::string
fannInqLibraryVersion()
{ 
	std::stringstream ss; 
	ss << "FANNExcel 0.03; " << __TIMESTAMP__ << ", xlw: " << XLW_VERSION << ", fann: 2.1.0";
	return ss.str();
}


bool // create training file for FANN network
fannCreateTrainingFile(const NEMatrix& inData // is input data matrix. Variables are in columns. Each training set in rows
					,  const NEMatrix& outData // is output data matrix. Variables in columns. Training sets in rows
					,  const std::string& trainFileName // is name of the output file
					)
{
	if (inData.rows() != outData.rows())
		throw "Inconsitent in- and out- data. Number of row differs";

	std::ofstream out(trainFileName.c_str());
	// Description header: # of rows, # of in-variables, # of out-variables
	out << inData.rows() << "\t" << inData.columns() << "\t" << outData.columns() << std::endl;
	for (unsigned int i = 0; i<inData.rows(); ++i)
	{	
		// In Data row
		for (unsigned int j=0; j<inData.columns(); ++j)
		{
			out << inData(i,j);
			if (j!=inData.columns()-1)
				out << "\t";
			else
				out << std::endl;
		}
		// Out data row
		for (unsigned int j=0; j<outData.columns(); ++j)
		{
			out << outData(i,j);
			if (j!=outData.columns()-1)
				out << "\t";
			else
				out << std::endl;
		}
	}
	out.close();
	return true;
}

bool	// create a standard fully connected backpropagation neural network and save it into file. 
fannCreateStandardArray(int nOfLayers			// is number of layers
					,	const MyArray&	neurons	// vector with number of neurons in each layer (including input, hiddenand output layers)
					,	const std::string& netFileName	// is the name of the created network file (including path)
					)
{
	unsigned int* layers = new unsigned int[neurons.size()];
	for(unsigned int i = 0; i < neurons.size(); ++i)
		layers[i] = (unsigned int)neurons[i];

	struct fann* ann = fann_create_standard_array(nOfLayers, layers);
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_save(ann, netFileName.c_str());
	return true;
}

struct fann* openAnnFile(const std::string& netFile)
{
	struct fann* ann = fann_create_from_file(netFile.c_str());
	if (ann == NULL)
		throw "Cannot open network file";
	return ann;
}

double	// train network on input file
fannTrainOnFile(const std::string& netFile			// is the ANN file
				,	const std::string& trainFile	// is name of the input training data file
				,	int maxEpochs					// maximum number of epochs,
				,	DoubleOrNothing desiredError	// desired error (MSE)
			  )
{
	// Load the network form the file
	struct fann* ann = openAnnFile(netFile);

	// Check consistency between thenetwork and the training file
	std::ifstream in(trainFile.c_str());
	int nOfRows, nOfInputs, nOfOutputs;
	in >> nOfRows >> nOfInputs >> nOfOutputs;
	in.close();

	if (nOfInputs != fann_get_num_input(ann))
	{
		std::stringstream ss;
		ss << "number of input neurons in the training file is " << nOfInputs << ", which differs from number of input neurons in the network " << fann_get_num_input(ann);
		throw ss.str();
	}
	if (nOfOutputs != fann_get_num_output(ann))
	{
		std::stringstream ss;
		ss << "number of output neurons in the training file is " << nOfOutputs << ", which differs from number of output neurons in the network " << fann_get_num_output(ann);
		throw ss.str();
	}

	// Train the network
	const float dError =(float) desiredError.GetValueOrDefault(defaultError);
	const int epochsBetweenReports = (int)maxEpochs / reportingFrequency;	
	FILE* logFile = fopen(logFileName, "a");
	fann_train_on_file(ann, trainFile.c_str(), maxEpochs, epochsBetweenReports, dError, logFile);
	fclose(logFile);
	double mse = fann_get_MSE(ann);

	// Save the trained network to the ANN file
	fann_save(ann, netFile.c_str());
	fann_destroy(ann);
	
	return mse;
}

// ---- CallBack --------
static NEMatrix&	inDataGlobal = NEMatrix();	// Ugly solution: Static valrable to get data into the callback...
static NEMatrix&	outDataGlobal = NEMatrix();
static void __stdcall cbTrainOnData(unsigned int rowNo, unsigned int nIn, unsigned int nOut, fann_type* inRow, fann_type* outRow)
{
	for (unsigned int i = 0; i < nIn; ++i)
		inRow[i] = (fann_type) inDataGlobal(rowNo,i);
	for (unsigned int j = 0; j < nOut; ++j)
		outRow[j] = (fann_type) outDataGlobal(rowNo, j);
}

// create fann_train_data struct
struct fann_train_data*
prepareTrainData(struct fann* ann, const NEMatrix& inData, const NEMatrix& outData)
{	
	// Check input/output layer consistency with data
	unsigned int nOfRows = inData.rows();
	unsigned int nOfInputs = inData.columns();
	unsigned int nOfOutputs= outData.columns();
	if (nOfInputs != fann_get_num_input(ann))
	{
		std::stringstream ss;
		ss << "number of input neurons in the training file is " << nOfInputs << ", which differs from number of input neurons in the network " << fann_get_num_input(ann);
		throw ss.str();
	}

	if (nOfOutputs != fann_get_num_output(ann))
	{
		std::stringstream ss;
		ss << "number of output neurons in the training file is " << nOfOutputs << ", which differs from number of output neurons in the network " << fann_get_num_output(ann);
		throw ss.str();
	}
	if (inData.rows() != outData.rows())
	{
		std::stringstream ss;
		ss << "number of inData rows " << inData.rows() << " differs from number of out-data rows " << outData.rows();
		throw ss.str();
	}

	// Create train_data struct
	inDataGlobal = inData;
	outDataGlobal = outData;
	struct fann_train_data* data = fann_create_train_from_callback(nOfRows, nOfInputs, nOfOutputs, cbTrainOnData); 
	return data;
}

double
fannTrainOnData(const std::string& netFile	// is the ANN file
				,	const NEMatrix& inData	// is input data matrix. Variables are in columns. Training sets in rows
				,	const NEMatrix& outData	// is output data matrix. Variables in columns. Training sets in rows
				,	int maxEpochs					// is maximum number of epochs,
				,	DoubleOrNothing desiredError	// is desired error (MSE)
				)
{
	// create ANN from file
	struct fann* ann = openAnnFile(netFile);
	// create train data
	struct fann_train_data* data = prepareTrainData(ann, inData, outData);

	// Train the network
	FILE* logFile = fopen(logFileName, "a");
	const float dError =(float) desiredError.GetValueOrDefault(defaultError);
	const int epochsBetweenReports = (int)maxEpochs / reportingFrequency;
	fann_train_on_data(ann, data, maxEpochs, epochsBetweenReports, dError, logFile);	
	fclose(logFile);

	fann_destroy_train(data);
	double mse = fann_get_MSE(ann);

	// Save and destroy
	fann_save(ann, netFile.c_str());
	fann_destroy(ann);
	
	return mse;
}

double	// test network on set of known in- and out-data withou modifying hte network. Return MSE
fannTestOnData(const std::string& netFile	// is the network definition ANN file
			   ,	const NEMatrix& inData	// is input data matrix. Variables are in columns. Training sets in rows
			   ,	const NEMatrix& outData	// is output data matrix. Variables in columns. Training sets in rows
				)
{
	// create ANN from file
	struct fann* ann = openAnnFile(netFile);
	// create train data
	struct fann_train_data* data = prepareTrainData(ann, inData, outData);
	// test + report MSE	
	double mse = fann_test_data(ann, data);
	// Clean-up
	fann_destroy_train(data);
	fann_destroy(ann);
	return mse;
}

double // train on in/outTrainData and report MSE on both train and test data.
fannTrainOnDataAndTest(const std::string& netFile	// is the network defition ANN file
				,	const NEMatrix& inTrainData		// is input train data matrix. Variables are in columns. Training sets in rows
				,	const NEMatrix& outTrainData	// is output train data matrix. Variables in columns. Training sets in rows
				,	int maxEpochs					// is maximum number of epochs,
				,	const NEMatrix& inTestData		// is input test data matrix. 
				,	const NEMatrix& outTestData		// is output test data matrix
				,	DoubleOrNothing desiredError	// is desired error (MSE)
				)
{
	// create ANN from file
	struct fann* ann = openAnnFile(netFile);
	// create train/test data
	struct fann_train_data* trainData	= prepareTrainData(ann, inTrainData, outTrainData);
	struct fann_train_data* testData	= prepareTrainData(ann, inTestData, outTestData);

	// -- Train --
	double dError = desiredError.GetValueOrDefault(defaultError);
	int epochsBetweenPeriods = maxEpochs / reportingFrequency;
	std::string bestTrainNetFile = netFile + ".bestTrain.net";
	std::string bestTestNetFile = netFile + ".bestTest.net";
	FILE* logFile = fopen(logFileName,"a");
	// Reset and recalculate MSE for Train and Test data
	double mseTrain = 1e9;
	double mseTrainMin = mseTrain;
	double mseTestMin = 1e9;
	fprintf(logFile, "--> fannTrainOnData(%s)\n",netFile.c_str());
	fprintf(logFile, "Max epochs %8d. Desired error: %.10f.\n", maxEpochs, dError);

	for (int i = 0; i < maxEpochs; ++i)
	{
		mseTrain = fann_train_epoch(ann, trainData);
		// Report to log file + save the best networks if necessary
		if(i % epochsBetweenPeriods == 0)
		{
			double mseTest = fann_test_data(ann, testData);
			fprintf(logFile, "Epochs     %8d. MSE(train)=%.10f. Bit fail %d. MSE(test)=%f\t", 
				i, mseTrain, fann_get_bit_fail(ann), mseTest);
			if (mseTrain < mseTrainMin)
			{
				mseTrainMin = mseTrain;
				fann_save(ann, bestTrainNetFile.c_str());
				fprintf(logFile, "saving bestTrain.net\t");
			}
			if (mseTest < mseTestMin)
			{
				mseTestMin = mseTest;
				fann_save(ann, bestTestNetFile.c_str());
				fprintf(logFile, "saving bestTest.net");
			}
			fprintf(logFile, "\n");
			fflush(logFile);
		}
		if (mseTrain < dError)
			break;
	}
		
	// Save the best
	fprintf(logFile, "*** Finished *** MSE(lastTrain)=%.10f, MSE(bestTrain)=%.10f, MSE(bestTest)=%.10f\n", 
		mseTrain, mseTrainMin, mseTestMin);
	fann_save(ann, netFile.c_str());

	// clean-up
	fclose(logFile);
	fann_destroy_train(trainData);
	fann_destroy_train(testData);
	fann_destroy(ann);

	return mseTrain;
}

NEMatrix	// run input through the neural network, returning an array of outputs, The variables are incolumns (equal to # of neurons inoutput layer), sets are in rows (equal to # of rows of the input data)
fannRun(const std::string& netFile	// is the network definition ANN file
		,	const NEMatrix& inData	// is input data matrix. Variables are in columns. Sets is in rows
		)
{
	// create ANN from file
	struct fann* ann = openAnnFile(netFile);
	
	// prepare variables
	unsigned int nOfRows = inData.rows();
	unsigned int nOfInput = inData.columns();
	unsigned int nOfOutput = fann_get_num_output(ann);
	NEMatrix outData(nOfRows, nOfOutput);

	fann_type* input = new fann_type[nOfInput];

	// run neural network fit
	for (unsigned int i = 0; i < nOfRows; ++i)
	{	
		for (unsigned int j = 0; j < nOfInput; ++j)
			input[j] = (fann_type)inData(i,j);

		fann_type* output = fann_run(ann, input);
		for (unsigned int j = 0; j < nOfOutput; ++j)
			outData(i,j) = output[j];
	}
	// clean up
	delete input;
	fann_destroy(ann);

	return outData;
}