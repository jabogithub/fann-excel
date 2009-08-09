#pragma once

#include <xlw/MyContainers.h>
#include <xlw/DoubleOrNothing.h>

//<xlw:libraryname=FANNExcel

std::string // Get library ID.
fannInqLibraryVersion();

bool	// create training file for FANN network
fannCreateTrainingFile(const NEMatrix& inData	// is input data matrix. Variables are in columns. Training sets in rows
					,  const NEMatrix& outData	// is output data matrix. Variables in columns. Training sets in rows
					,  const std::string& trainFileName // is name of the training file to be created
					);

bool	// create a standard fully connected backpropagation neural network and save it into file. 
fannCreateStandardArray(		int nOfLayers			// is number of layers
				    ,	const MyArray&	neurons	// vector with number of neurons in each layer (including input, hiddenand output layers)
					,	const std::string& netFileName	// is the name of the created ANN file
					);

double	// train network on train file. Return MSE 
fannTrainOnFile(const std::string& netFile // is the ANN file
				,	const std::string& trainFile	// is name of the input training data file
				,	int maxEpochs					// is maximum number of epochs,
				,	DoubleOrNothing desiredError	// is desired error (MSE)
				);

double	// train network on in- and out-data. Return MSE
fannTrainOnData(const std::string& netFile	// is the network defition ANN file
				,	const NEMatrix& inData	// is input data matrix. Variables are in columns. Training sets in rows
				,	const NEMatrix& outData	// is output data matrix. Variables in columns. Training sets in rows
				,	int maxEpochs					// is maximum number of epochs,
				,	DoubleOrNothing desiredError	// is desired error (MSE)
				);

double	// test network on set of known in- and out-data withou modifying hte network. Return MSE
fannTestOnData(const std::string& netFile	// is the network definition ANN file
			   ,	const NEMatrix& inData	// is input data matrix. Variables are in columns. Training sets in rows
			   ,	const NEMatrix& outData	// is output data matrix. Variables in columns. Training sets in rows
				);