/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TStopwatch.h"

#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#endif

using namespace TMVA;

void myTMVARegressionApplication(TTree* theTree, int neurons, int layer) 
{
 
   TString myMethodList = "";
  
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
   // ---------------------------------------------------------------
   
   std::cout << std::endl;
   std::cout << "==> Start TMVARegressionApplication" << std::endl;
   
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
   
   // --- Create the Reader object
   TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );    
   
   // Create a set of variables and declare them to the reader
   Float_t x,y;
   reader->AddVariable( Form("y_%i",layer), &y );
   
   TString dir    = Form("weights_layer%i_neurons%i/",layer,neurons);
   TString prefix = "TMVARegression";
   
   // Book method(s)
   for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
      if (it->second) {
         TString methodName = it->first + " method";
         TString weightfile = dir + prefix + "_" + TString(it->first) + ".weights.xml";
         reader->BookMVA( methodName, weightfile ); 
      }
   }
   
   // Book output histograms
   /*
   TH1* hists[100];
   Int_t nhists = -1;
   for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++)
   {
      TH1* h = new TH1F( it->first.c_str(), TString(it->first) + " method", 100, 0,1);
      if (it->second) hists[++nhists] = h;
   }
   nhists++;
   */
   std::cout << "--- TMVARegressionApp "<< std::endl;
   
   // --- Event loop
   theTree->SetBranchAddress( "y", &y );
   
   std::cout << "--- Processing: " << theTree->GetEntries() << " events" << std::endl;
   TStopwatch sw;
   sw.Start();
   
   //***************************************
   int N=theTree->GetEntries();
   TH1D* hist=new TH1D("MLP","MLP",100, 0,1);
   for (Long64_t ievt=0; ievt<N;ievt++)
   {
    
    theTree->GetEntry(ievt);
    if(ievt%5000==0) cout<<"already "<<ievt<<" from "<<N<<" done "<<endl;
    
    // Retrieve the MVA target values (regression outputs) and fill into histograms
    // NOTE: EvaluateRegression(..) returns a vector for multi-target regression
    Float_t val = (reader->EvaluateRegression( "MLP method" ))[0];
    hist->Fill( val );
   }
   
   sw.Stop();
   std::cout << "--- End of event loop: "; sw.Print();
   

   // --- Write histograms
   TFile *target  = new TFile(Form("TMVARegApp_layer%i_neurons%i.root",layer,neurons),"RECREATE" );
   hist->Write();
   target->Close();

   delete reader;
    
   std::cout << "==> TMVARegressionApplication is done!" << std::endl << std::endl;
   std::cout << "Output:" << target->GetName()<< std::endl;
}


