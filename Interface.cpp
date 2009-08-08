#include "Interface.h"

#include "fann.h"
#include <sstream>
#include <fstream>

std::string
nnInqLibraryVersion()
{ 
	std::stringstream ss; 
	ss << "Neural Network Tools v 0.01 " << __TIMESTAMP__ ;
	return ss.str();
}


bool // create training file for FANN network
nnSaveFannInputTrainingFile(const NEMatrix& inData // is input data matrix. Variables are in columns. Each training set in rows
					,  const NEMatrix& outData // is output data matrix. Variables in columns. Training sets in rows
					,  std::string fileName // is name of the output file
					)
{
	if (inData.rows() != outData.rows())
		throw "Inconsitent in- and out- data. Number of row differs";

	std::ofstream out(fileName.c_str());
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


bool	// train network on input file
nnTrainOnFile(const std::string dataFile	// is name of the input data file
			  ,	int nOfInputNeurons		// is number of neurons in the input layer
			  , int nOfOutputNeurons	// is number of neuron in the output layer
			  , int nOfLayers			// is number of layers
			  , int nOfHiddenNeurons	// is number of eurons in each hidden layer
			  , int maxEpochs			// maximum number of iterations
			  )
{
	const float desiredError = (float)0.001;
	const int epochsBetweenReports = (int)maxEpochs/5;

	struct fann* ann = fann_create_standard(nOfLayers, nOfInputNeurons, nOfHiddenNeurons, nOfOutputNeurons);

    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_train_on_file(ann, dataFile.c_str(), maxEpochs, epochsBetweenReports, desiredError);

	std::string outNetFileName = dataFile + ".net";
	fann_save(ann, outNetFileName.c_str());
	fann_destroy(ann);
	
	return true;
}