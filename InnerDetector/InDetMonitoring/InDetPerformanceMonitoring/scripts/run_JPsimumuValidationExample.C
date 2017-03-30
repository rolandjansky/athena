 #include <iostream>
#include <string>
#include <list>
#include "TROOT.h"
#include "JpsimumuValidationExample.cxx"

bool run_JPsimumuValidationExample(Int_t nIterationsUserInput=5) 
{

   //////////////////////////////////
   // compile JpsimumuValidationExample
   //////////////////////////////////
   // gROOT->ProcessLine(".L JpsimumuValidationExample.cxx+");   

   /////////////////////////////////////////
   // define list of files to be analysed
   /////////////////////////////////////////
   std::list <std::string > s_fileNames;
   
   string output_dir = "output/";
   string output_file_name = "JpsimumuValidationExampleOutput.root";
      
 
   string output_tag = "";
   s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/user.martis/user.martis.7544035.ZmumuValidation._000001.root");
   //s_fileNames.push_back("/afs/cern.ch/work/m/martis/public/athena/JPsiNtuples/user.martis/user.martis.7544035.ZmumuValidation._000002.root");
   
   
   /////////////////////////////////////////////////
   //define job parameters and run
   /////////////////////////////////////////////////  
   int nIterations = 1; if (nIterationsUserInput > 0) nIterations = nIterationsUserInput;
   int nEvents = 200000;
   
   //string tree_name = "DefaultParams";
   string tree_name = "Refit1Params";
   
   string output_tag_mod = "";
   //string output_tag_mod = "ZScaleUp500.";
   //string output_tag_mod = "ZScaleDn500.";
      
   string output_file = Form("%s%s", output_dir.c_str(),output_file_name.c_str());
   
   JpsimumuValidationExample myJpsimumuValidationExample(s_fileNames, tree_name, output_file);
   myJpsimumuValidationExample.SetPrintLevel(1);
   myJpsimumuValidationExample.SetEtaBins(10);
   myJpsimumuValidationExample.SetPhiBins(1); //I think JpsimumuValidationExample is prepared just for one bin in phi. It Could be extended in the future.
   myJpsimumuValidationExample.bookHistograms();

   myJpsimumuValidationExample.JpsiMassForFitLoop(nEvents);

   for (int i = 0; i < nIterations; i++) {
     std::cout<< endl << " ==== JpsimumuValidation ==== starting iteration "<< i+1 <<" of " << nIterations<< std::endl;
     myJpsimumuValidationExample.loop(nEvents);
   }
   
   myJpsimumuValidationExample.writeToFile(0); 

   std::cout<< "Finished - Output File: " << output_file << std::endl;
 
   return true;    
}
