#pragma once

#include <xlw/MyContainers.h>
#include <xlw/DoubleOrNothing.h>

//<xlw:libraryname=FANNExcel

std::string // Get library ID.
fannInqLibraryVersion();

bool	// create training file for FANN network
fannCreateTrainingFile(const NEMatrix& inData // is input data matrix. Variables are in columns. Training sets in rows
					,  const NEMatrix& outData // is output data matrix. Variables in columns. Training sets in rows
					,  std::string trainFileName // is name of the training file to be created
					);

bool	// create a standard fully connected backpropagation neural network and save it into file. 
fannCreateStandardArray(		int nOfLayers			// is number of layers
				    ,	const MyArray&	neurons	// vector with number of neurons in each layer (including input, hiddenand output layers)
					,	std::string netFileName	// is the name of the created ANN file
					);

bool	// train network on train file
fannTrainOnFile(const std::string& netFile // is the ANN file
				,	const std::string& trainFile	// is name of the input training data file
				,	int maxEpochs					// maximum number of epochs,
				,	DoubleOrNothing desiredError	// desired error (MSE)
				);