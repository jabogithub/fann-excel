#include "Interface.h"

#include <sstream>
#include <fstream>

#include <fann.h>
#include <xlw/xlw.h>

std::string
fannInqLibraryVersion()
{ 
	std::stringstream ss; 
	ss << "FANNExcel 0.02; " << __TIMESTAMP__ << ", xlw: " << XLW_VERSION << ", fann: 2.1.0";
	return ss.str();
}


bool // create training file for FANN network
fannCreateTrainingFile(const NEMatrix& inData // is input data matrix. Variables are in columns. Each training set in rows
					,  const NEMatrix& outData // is output data matrix. Variables in columns. Training sets in rows
					,  std::string trainFileName // is name of the output file
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
					,	std::string netFileName	// is the name of the created network file (including path)
					)
{
	unsigned int* layers = new unsigned int[neurons.size()];
	for(unsigned int i = 0; i < neurons.size(); ++i)
		layers[i] = (unsigned int)neurons[i];

	struct fann* ann = fann_create_standard_array(nOfLayers, layers);
	
	fann_save(ann, netFileName.c_str());
	return true;
}

bool	// train network on input file
fannTrainOnFile(const std::string& netFile			// is the ANN file
				,	const std::string& trainFile	// is name of the input training data file
				,	int maxEpochs					// maximum number of epochs,
				,	DoubleOrNothing desiredError	// desired error (MSE)
			  )
{
	const float dError =(float) desiredError.GetValueOrDefault(0.001);
	const int epochsBetweenReports = (int)maxEpochs/5;

	struct fann* ann = fann_create_from_file(netFile.c_str());

    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_train_on_file(ann, trainFile.c_str(), maxEpochs, epochsBetweenReports, dError);

	fann_save(ann, netFile.c_str());
	fann_destroy(ann);
	
	return true;
}