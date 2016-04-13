/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCS1DFunctionRegression.h"

#include "TMVA/Config.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/Factory.h"
#include "TRandom3.h"
#include "TFile.h"
#include "TString.h"
#include "TMVA/IMethod.h"
#include "TMVA/MethodMLP.h"
#include "TMath.h"


//=============================================
//======= TFCS1DFunctionRegression =========
//=============================================

using namespace std;

namespace {
class MethodMLPNetworkGetter
  : public TMVA::MethodMLP
{
public:
  using TMVA::MethodMLP::fNetwork;
};
}

TFCS1DFunctionRegression::TFCS1DFunctionRegression()
{
}

TFCS1DFunctionRegression::TFCS1DFunctionRegression(TH1* hist)
{
	
	cout<<"IN TFCS1DFunctionRegression"<<endl;
  Initialize(hist);
  
}

TFCS1DFunctionRegression::TFCS1DFunctionRegression(string weightfilename)
{
	
	cout<<"IN TFCS1DFunctionRegression"<<endl;
  Initialize(weightfilename);
  
}

void TFCS1DFunctionRegression::Initialize(TH1* /*hist*/)
{
 //not needed
}

void TFCS1DFunctionRegression::Initialize(string weightfilename)
{
  
	get_weights(weightfilename);
 	
  //for testing:
  validate(10,weightfilename);
 
}


void TFCS1DFunctionRegression::validate(int Ntoys,string weightfilename)
{
 
 TRandom* myRandom=new TRandom3(); myRandom->SetSeed(0);
 
 //calculate regression from the weights and compare to TMVA value:
 cout<<"Validating the regression value:"<<endl;
 for(int i=0;i<Ntoys;i++)
 {
  double random=myRandom->Uniform(1);
  cout<<"myvalue "<<regression_value(random)<<" TMVA value "<<tmvaregression_application(random,weightfilename)<<endl;
 }
 
}




void TFCS1DFunctionRegression::get_weights(string weightfile)
{
 using namespace TMVA;
 int debug=1;
 
 TString myMethodList = "" ;
 TMVA::Tools::Instance();
 
 std::map<std::string,int> Use;

 // --- Mutidimensional likelihood and Nearest-Neighbour methods
 Use["PDERS"]           = 0;   Use["PDEFoam"]         = 0;   Use["KNN"]            = 0;
 Use["LD"]		          = 0;   Use["FDA_GA"]          = 0;   Use["FDA_MC"]          = 0;
 Use["FDA_MT"]          = 0;   Use["FDA_GAMT"]        = 0;   Use["MLP"]             = 1; 
 Use["SVM"]             = 0;   Use["BDT"]             = 0;   Use["BDTG"]            = 0;
 // ---------------------------------------------------------------

 // Select methods (don't look at this code - not of interest)
 if (myMethodList != "")
 {
  for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;
  std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
  for (UInt_t i=0; i<mlist.size(); i++)
  {
   std::string regMethod(mlist[i]);
   if (Use.find(regMethod) == Use.end())
   {
    std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
    for(std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
    std::cout << std::endl;
   }
   Use[regMethod] = 1;
  }
 }
 
 // --------------------------------------------------------------------------------------------------
 
 TMVA::Reader *reader = new TMVA::Reader( "!Color:Silent");    
 
 Float_t y=0.5; //just a dummy
 reader->AddVariable( "y", &y );
 
 TString dir   = Form("%s/",weightfile.c_str());
 TString prefix = "TMVARegression";
 
 for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++)
 {
  if (it->second)
  {
   TString methodName = it->first + " method";
   TString weightfile = dir + prefix + "_" + TString(it->first) + ".weights.xml";
   reader->BookMVA( methodName, weightfile ); 
  }
 }
 
 TMVA::IMethod* m=reader->FindMVA("MLP method");
 TMVA::MethodMLP *mlp = dynamic_cast<TMVA::MethodMLP*>(m);
 TObjArray* Network=reinterpret_cast<MethodMLPNetworkGetter*>(mlp)->fNetwork;
 
 int num_neurons_input=((TObjArray*)Network->At(1))->GetEntriesFast()-1;
 if(debug) cout<<"num_neurons_input "<<num_neurons_input<<endl;
 
 int fLayers = Network->GetEntriesFast();
 if(fLayers!=3) cout<<"!!!!!!!!!! fLayers is not 3 !!!!!!!!!"<<endl;
 
 for(int a=0;a<((TObjArray*)Network->At(1))->GetEntriesFast();a++)
 {
  vector<double> thisvector;
  for(int b=0;b<((TObjArray*)Network->At(0))->GetEntriesFast();b++)
   thisvector.push_back(0);
  m_fWeightMatrix0to1.push_back(thisvector);
 }
 
 for(int a=0;a<((TObjArray*)Network->At(2))->GetEntriesFast();a++)
 {
  vector<double> thisvector;
  for(int b=0;b<((TObjArray*)Network->At(1))->GetEntriesFast();b++)
   thisvector.push_back(0);
  m_fWeightMatrix1to2.push_back(thisvector);
 }
 
 
 TObjArray* curLayer= (TObjArray*)Network->At(0);
 Int_t numNeurons = curLayer->GetEntriesFast();
 for (Int_t n = 0; n < numNeurons; n++)
 {  
  TNeuron* neuron = (TNeuron*)curLayer->At(n);
  int numSynapses = neuron->NumPostLinks();
  for (int s = 0; s < numSynapses; s++)
  {
   TSynapse* synapse = neuron->PostLinkAt(s);  
   m_fWeightMatrix0to1[s][n]=synapse->GetWeight();
   if(debug) cout<<"fWeightMatrix0to1["<<s<<"]["<<n<<"] "<<synapse->GetWeight()<<endl;
  }
 }
 
 curLayer= (TObjArray*)Network->At(1);
 numNeurons = curLayer->GetEntriesFast();
 for (Int_t n = 0; n < numNeurons; n++)
 {  
  TNeuron* neuron = (TNeuron*)curLayer->At(n);
  int numSynapses = neuron->NumPostLinks();
  for (int s = 0; s < numSynapses; s++)
  {
   TSynapse* synapse = neuron->PostLinkAt(s);  
   m_fWeightMatrix1to2[s][n]=synapse->GetWeight();
   if(debug) cout<<"fWeightMatrix1to2["<<s<<"]["<<n<<"] "<<synapse->GetWeight()<<endl;
  }
 }
 
 delete reader;
 
}

double TFCS1DFunctionRegression::regression_value(double uniform)
{
 double myresult=-1;
 
 int n_neurons=m_fWeightMatrix0to1.size()-1;
 
 int fLayers=3;
 vector<int> fLayerSize;
 fLayerSize.push_back(2);
 fLayerSize.push_back(n_neurons+1);
 fLayerSize.push_back(1);
 
 vector<vector<double> > fWeights;
 for(int l=0;l<fLayers;l++)
 {
  vector<double> thisvector;
  for(int i=0;i<fLayerSize[l];i++)
   thisvector.push_back(0); //placeholder
  fWeights.push_back(thisvector);
 }
 
 for (int l=0; l<fLayers; l++)
  for (int i=0; i<fLayerSize[l]; i++)
   fWeights[l][i]=0;

 for (int l=0; l<fLayers-1; l++)
  fWeights[l][fLayerSize[l]-1]=1;

 for (int i=0; i<fLayerSize[0]-1; i++)
  fWeights[0][i]=uniform;

 // layer 0 to 1
 for (int o=0; o<fLayerSize[1]-1; o++)
 {
  for (int i=0; i<fLayerSize[0]; i++)
  {
   fWeights[1][o] += m_fWeightMatrix0to1[o][i] * fWeights[0][i];
  }
  fWeights[1][o] = tanh(fWeights[1][o]);
 }
 
 // layer 1 to 2
 for (int o=0; o<fLayerSize[2]; o++)
 {
  for (int i=0; i<fLayerSize[1]; i++)
  {
   fWeights[2][o] += m_fWeightMatrix1to2[o][i] * fWeights[1][i];
  }
 }
 
 myresult=fWeights[2][0];
 
 return myresult;
 
}


double TFCS1DFunctionRegression::rnd_to_fct(double rnd)
{
  
  double value=regression_value(rnd); 
  return value;
  
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCS1DFunctionRegression)

