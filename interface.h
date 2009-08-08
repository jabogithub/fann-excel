#pragma once

#include <xlw/MyContainers.h>

//<xlw:libraryname=FANNExcel

std::string // Get library ID.
nnInqLibraryVersion();

bool // create training file for FANN network
nnSaveFannInputTrainingFile(const NEMatrix& inData // is input data matrix. Variables are in columns. Each training set in rows
					,  const NEMatrix& outData // is output data matrix. Variables in columns. Training sets in rows
					,  std::string fileName // is name of the output file
					);

bool	// train network on input file
nnTrainOnFile(const std::string dataFile	// is name of the input data file
			  ,	int nOfInputNeurons		// is number of neurons in the input layer
			  , int nOfOutputNeurons	// is number of neuron in the output layer
			  , int nOfLayers			// is number of layers
			  , int nOfHiddenNeurons	// is number of eurons in each hidden layer
			  , int maxEpochs			// maximum number of iterations
			  );
			