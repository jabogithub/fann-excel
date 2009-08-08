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
const char* LibraryName = "FANNExcel";
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
fannInqLibraryVersionArgs[]=
{
 { "","" } 
};
  XLRegistration::XLFunctionRegistrationHelper
registerfannInqLibraryVersion("xlfannInqLibraryVersion",
"fannInqLibraryVersion",
" Get library ID. ",
LibraryName,
fannInqLibraryVersionArgs,
""
);
}



extern "C"
{
LPXLOPER EXCEL_EXPORT
xlfannInqLibraryVersion(
)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

std::string result(
	fannInqLibraryVersion());
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
fannCreateTrainingFileArgs[]=
{
{ "inData"," is input data matrix. Variables are in columns. Training sets in rows "},
{ "outData"," is output data matrix. Variables in columns. Training sets in rows "},
{ "trainFileName"," is name of the training file to be created "}
};
  XLRegistration::XLFunctionRegistrationHelper
registerfannCreateTrainingFile("xlfannCreateTrainingFile",
"fannCreateTrainingFile",
" create training file for FANN network ",
LibraryName,
fannCreateTrainingFileArgs,
"KKR"
);
}



extern "C"
{
LPXLOPER EXCEL_EXPORT
xlfannCreateTrainingFile(
LPXLARRAY inDataa,
LPXLARRAY outDataa,
LPXLOPER trainFileNamea)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

NEMatrix inData(
	GetMatrix(inDataa));

NEMatrix outData(
	GetMatrix(outDataa));

XlfOper trainFileNameb(
	(trainFileNamea));
std::string trainFileName(
	trainFileNameb.AsString("trainFileName"));

bool result(
	fannCreateTrainingFile(
		inData,
		outData,
		trainFileName)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
fannCreateStandardArrayArgs[]=
{
{ "nOfLayers"," is number of layers "},
{ "neurons"," vector with number of neurons in each layer (including input, hiddenand output layers) "},
{ "netFileName"," is the name of the created ANN file "}
};
  XLRegistration::XLFunctionRegistrationHelper
registerfannCreateStandardArray("xlfannCreateStandardArray",
"fannCreateStandardArray",
" create a standard fully connected backpropagation neural network and save it into file. ",
LibraryName,
fannCreateStandardArrayArgs,
"BRR"
);
}



extern "C"
{
LPXLOPER EXCEL_EXPORT
xlfannCreateStandardArray(
double nOfLayersa,
LPXLOPER neuronsa,
LPXLOPER netFileNamea)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

int nOfLayers(
	static_cast<int>(nOfLayersa));

XlfOper neuronsb(
	(neuronsa));
MyArray neurons(
	neuronsb.AsArray("neurons"));

XlfOper netFileNameb(
	(netFileNamea));
std::string netFileName(
	netFileNameb.AsString("netFileName"));

bool result(
	fannCreateStandardArray(
		nOfLayers,
		neurons,
		netFileName)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

namespace
{
XLRegistration::Arg
fannTrainOnFileArgs[]=
{
{ "netFile"," is the ANN file "},
{ "trainFile"," is name of the input training data file "},
{ "maxEpochs"," maximum number of epochs, "},
{ "desiredError"," desired error (MSE) "}
};
  XLRegistration::XLFunctionRegistrationHelper
registerfannTrainOnFile("xlfannTrainOnFile",
"fannTrainOnFile",
" train network on train file ",
LibraryName,
fannTrainOnFileArgs,
"RRBP"
);
}



extern "C"
{
LPXLOPER EXCEL_EXPORT
xlfannTrainOnFile(
LPXLOPER netFilea,
LPXLOPER trainFilea,
double maxEpochsa,
LPXLOPER desiredErrora)
{
EXCEL_BEGIN;

	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

XlfOper netFileb(
	(netFilea));
std::string netFile(
	netFileb.AsString("netFile"));

XlfOper trainFileb(
	(trainFilea));
std::string trainFile(
	trainFileb.AsString("trainFile"));

int maxEpochs(
	static_cast<int>(maxEpochsa));

XlfOper desiredErrorb(
	(desiredErrora));
CellMatrix desiredErrorc(
	desiredErrorb.AsCellMatrix("desiredErrorc"));
DoubleOrNothing desiredError(
	DoubleOrNothing(desiredErrorc,"desiredError"));

bool result(
	fannTrainOnFile(
		netFile,
		trainFile,
		maxEpochs,
		desiredError)
	);
return XlfOper(result);
EXCEL_END
}
}



//////////////////////////

