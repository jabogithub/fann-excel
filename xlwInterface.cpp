//// 
//// created by xlwplus
////

#include <xlw/pragmas.h>
#include <xlw/MyContainers.h>
#include <xlw/CellMatrix.h>
#include "Interface.h"
#include <xlw/xlw.h>
#include <xlw/XlFunctionRegistration.h>
#include <stdexcept>
#include <xlw/XlOpenClose.h>
#include <ctime>
#include <xlw/xlarray.h>

namespace {
const char* LibraryName = "NeuralNetworkTools";
};

// dummy function to force linkage
namespace {
void DummyFunction()
{
xlAutoOpen();
xlAutoClose();
}
}

// registrations start here


namespace
{
XLRegistration::Arg
nnInqLibraryVersionArgs[]=
{
 { "","" } 
};
  XLRegistration::XLFunctionRegistrationHelper
registernnInqLibraryVersion("xlnnInqLibraryVersion",
"nnInqLibraryVersion",
" Get library ID. ",
LibraryName,
nnInqLibraryVersionArgs,
""
);
}



extern "C"
{
LPXLOPER EXCEL_EXPORT
xlnnInqLibraryVersion(
)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

std::string result(
	nnInqLibraryVersion());
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
nnSaveFannInputTrainingFileArgs[]=
{
{ "inData"," is input data matrix. Variables are in columns. Each training set in rows "},
{ "outData"," is output data matrix. Variables in columns. Training sets in rows "},
{ "fileName"," is name of the output file "}
};
  XLRegistration::XLFunctionRegistrationHelper
registernnSaveFannInputTrainingFile("xlnnSaveFannInputTrainingFile",
"nnSaveFannInputTrainingFile",
" create training file for FANN network ",
LibraryName,
nnSaveFannInputTrainingFileArgs,
"KKR"
);
}



extern "C"
{
LPXLOPER EXCEL_EXPORT
xlnnSaveFannInputTrainingFile(
LPXLARRAY inDataa,
LPXLARRAY outDataa,
LPXLOPER fileNamea)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

NEMatrix inData(
	GetMatrix(inDataa));

NEMatrix outData(
	GetMatrix(outDataa));

XlfOper fileNameb(
	(fileNamea));
std::string fileName(
	fileNameb.AsString("fileName"));

bool result(
	nnSaveFannInputTrainingFile(
		inData,
		outData,
		fileName)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
nnTrainOnFileArgs[]=
{
{ "dataFile"," is name of the input data file "},
{ "nOfInputNeurons"," is number of neurons in the input layer "},
{ "nOfOutputNeurons"," is number of neuron in the output layer "},
{ "nOfLayers"," is number of layers "},
{ "nOfHiddenNeurons"," is number of eurons in each hidden layer "},
{ "maxEpochs"," maximum number of iterations "}
};
  XLRegistration::XLFunctionRegistrationHelper
registernnTrainOnFile("xlnnTrainOnFile",
"nnTrainOnFile",
" train network on input file ",
LibraryName,
nnTrainOnFileArgs,
"RBBBBB"
);
}



extern "C"
{
LPXLOPER EXCEL_EXPORT
xlnnTrainOnFile(
LPXLOPER dataFilea,
double nOfInputNeuronsa,
double nOfOutputNeuronsa,
double nOfLayersa,
double nOfHiddenNeuronsa,
double maxEpochsa)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

XlfOper dataFileb(
	(dataFilea));
std::string dataFile(
	dataFileb.AsString("dataFile"));

int nOfInputNeurons(
	static_cast<int>(nOfInputNeuronsa));

int nOfOutputNeurons(
	static_cast<int>(nOfOutputNeuronsa));

int nOfLayers(
	static_cast<int>(nOfLayersa));

int nOfHiddenNeurons(
	static_cast<int>(nOfHiddenNeuronsa));

int maxEpochs(
	static_cast<int>(maxEpochsa));

bool result(
	nnTrainOnFile(
		dataFile,
		nOfInputNeurons,
		nOfOutputNeurons,
		nOfLayers,
		nOfHiddenNeurons,
		maxEpochs)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

