/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <cstdlib>
#include <iostream> 
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TMVA/Config.h"

//#include "TMVARegGui.C"

#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Factory.h"
#endif

using namespace TMVA;
   
void myTMVARegression(TTree *regTree, int neurons, int layer) 
{

   TString myMethodList = "" ;
   
   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();
   
   std::map<std::string,int> Use;
   Use["PDERS"]           = 0;
   Use["PDEFoam"]         = 0; 
   Use["KNN"]             = 0;
   Use["LD"]		          = 0;
   Use["FDA_GA"]          = 0;
   Use["FDA_MC"]          = 0;
   Use["FDA_MT"]          = 0;
   Use["FDA_GAMT"]        = 0;
   Use["MLP"]             = 1; 
   Use["SVM"]             = 0;
   Use["BDT"]             = 0;
   Use["BDTG"]            = 0;
   
   std::cout << std::endl;
   std::cout << "==> Start TMVARegression" << std::endl;
   
   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
            std::cout << std::endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------

   // --- Here the preparation phase begins

   // Create a new root output file
   TString outfileName( "TMVAReg.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

   // Create the factory object
   TMVA::Factory *factory = new TMVA::Factory( "TMVARegression", outputFile,"!V:!Silent:Color:DrawProgressBar");
   
   //set the weight directory:
   TString dirname=Form("weights_layer%i_neurons%i/",layer,neurons);
   (TMVA::gConfig().GetIONames()).fWeightFileDir = dirname;
   //(TMVA::Config::Config()).fIONames.fWeightFileDir="test";
   
   // Define the input variables that shall be used for the MVA training
   factory->AddVariable( Form("y_%i",layer), "y", 'F' );
   
   // Add the variable carrying the regression target
   factory->AddTarget( Form("x_%i",layer) );
   
   // load the signal and background event samples from ROOT trees
   /*
   TFile *input(0);
   TString fname = "PCAOutput.root";
   
   if (!gSystem->AccessPathName( fname )) 
      input = TFile::Open( fname ); // check if file in local directory exists
   
   if (!input) {
      std::cout << "ERROR: could not open data file" << std::endl;
      exit(1);
   }
   std::cout << "--- TMVARegression           : Using input file: " << input->GetName() << std::endl;
   */
   // --- Register the regression tree
   //TTree *regTree = (TTree*)input->Get("sorted");
   
   // global event weights per tree (see below for setting event-wise weights)
   Double_t regWeight  = 1.0;   
   
   // You can add an arbitrary number of regression trees
   factory->AddRegressionTree( regTree, regWeight );
   
   // Apply additional cuts on the signal and background samples (can be different)
   TCut mycut = ""; // for example: TCut mycut = "abs(var1)<0.5 && abs(var2-0.5)<1";
   
   // tell the factory to use all remaining events in the trees after training for testing:
   factory->PrepareTrainingAndTestTree( mycut,"nTrain_Regression=0:nTest_Regression=0:SplitMode=Random:NormMode=NumEvents:!V" );
   
   // ---- Book MVA methods
   // Neural network (MLP)
   if (Use["MLP"])
      factory->BookMethod( TMVA::Types::kMLP, "MLP", Form("!H:!V:VarTransform=Norm:NeuronType=tanh:NCycles=20000:HiddenLayers=%i:TestRate=6:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15:!UseRegulator",neurons) );

   // --------------------------------------------------------------------------------------------------

   // Train MVAs using the set of training events
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();

   // --------------------------------------------------------------
   
   // Save the output
   outputFile->Close();
   
   std::cout << "==> TMVARegression is done!" << std::endl;      
   std::cout<<"The weights for layer "<<layer<<" with "<<neurons<<" neurons are here: "<<dirname<<std::endl;

   delete factory;
   
}

