/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

using namespace std;

#include "ISF_FastCaloSimParametrization/TFCS2DFunctionRegression.h"

#include "TMVA/Config.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/Factory.h"
#include "TRandom3.h"
#include "TFile.h"
#include "TString.h"
#include "TMVA/IMethod.h"
#include "TMVA/MethodMLP.h"
#define private public
#define protected public
#include "TMath.h"


//=============================================
//======= TFCS2DFunctionRegression =========
//=============================================

TFCS2DFunctionRegression::TFCS2DFunctionRegression()
{
}

TFCS2DFunctionRegression::TFCS2DFunctionRegression(TH1* hist)
{
	
  Initialize(hist);
  
}


void TFCS2DFunctionRegression::Initialize(TH1* /*hist*/)
{
 //not needed
}

void TFCS2DFunctionRegression::storeRegression(string weightfilename,int mycase)
{
  
  get_weights(weightfilename,mycase);
  
  //for testing - DOES NOT WORK FOR TRANSFORMED VARARIABLES (VarTransform=Norm)
  //validate(10,weightfilename,mycase);
  
}


void TFCS2DFunctionRegression::validate(int Ntoys,string weightfilename,int mycase)
{
 
 TRandom* myRandom1=new TRandom3();
 TRandom* myRandom2=new TRandom3();

 myRandom1->SetSeed( 0 );
 myRandom2->SetSeed( 1000 );


 //calculate regression from the weights and compare to TMVA value:
 cout<<endl;
 cout<<"--- Validating the regression value:"<<endl;
 for(int i=0;i<Ntoys;i++)
 {
   std::vector<double> vuniform;
   float random1 = myRandom1->Uniform(1);
   float random2 = myRandom2->Uniform(1);
   vuniform.push_back(random1);
   vuniform.push_back(random2);
   
   if(mycase==1) cout <<"myvalue "<< regression_value(vuniform, mycase) <<" TMVA value "<<(tmvaregression_application(random1,random2,weightfilename,mycase)).at(0)<<endl;
   if(mycase==2) cout <<"myvalue "<< regression_value(vuniform, mycase) <<" TMVA value "<<(tmvaregression_application(random1,random2,weightfilename,mycase)).at(0)<< "  " << (tmvaregression_application(random1,random2,weightfilename,mycase)).at(1) << endl;
 }
 
}


void TFCS2DFunctionRegression::get_weights(string /*weightfile*/,int /*mycase*/)
{

/*
 
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
 
 Float_t r=0.5; //just a dummy
 Float_t alpha=0.5;
 Float_t probr=0.5;
 Float_t probalpha=0.5;
 if(mycase==1){
   reader->AddVariable( "alpha", &alpha );
   reader->AddVariable( "r", &r );
 } 
 if(mycase==2){
   reader->AddVariable( "probalpha", &probalpha );
   reader->AddVariable( "probr", &probr );
 }

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
 TObjArray* Network=mlp->fNetwork;
 //mlp->MakeClass("mlpcode.C");

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
 */

}

double TFCS2DFunctionRegression::regression_value(vector<double> vuniform, int mycase)
{

 int n_neurons=m_fWeightMatrix0to1.size();
 
 int fLayers=3;
 vector<int> fLayerSize;
 fLayerSize.push_back(3);
 fLayerSize.push_back(n_neurons);
 fLayerSize.push_back(mycase);
 

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

 for (int i=0; i<fLayerSize[0]-1; i++){
  fWeights[0][i]=vuniform[i];
 }

 // layer 0 to 1
 for (int o=0; o<fLayerSize[1]-1; o++)
 {
  for (int i=0; i<fLayerSize[0]; i++)
  {
   fWeights[1][o] += m_fWeightMatrix0to1[o][i] * fWeights[0][i];
  }
  fWeights[1][o] = TMath::TanH(fWeights[1][o]);
 }
 
 // layer 1 to 2
 for (int o=0; o<fLayerSize[2]; o++)
 {
  for (int i=0; i<fLayerSize[1]; i++)
  {
   fWeights[2][o] += m_fWeightMatrix1to2[o][i] * fWeights[1][i];
  }
 }
 
 return fWeights[2][0];
 
}


double TFCS2DFunctionRegression::rnd_to_fct(std::vector<double> vrdn, int mycase)
{
  
  double value=regression_value(vrdn,mycase);
  return value;
  
}


//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCS2DFunctionRegression)

