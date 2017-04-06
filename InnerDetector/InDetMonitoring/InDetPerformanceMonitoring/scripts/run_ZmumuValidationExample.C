#include <iostream>
#include <string>
#include <list>
#include "TROOT.h"
#include "TStyle.h"
#include "ZmumuValidationExample.cxx"
#include "userFiles.C"

void run_ZmumuValidationExample(Int_t nIterationsUserInput=1, Int_t sampletype = ZmumuValidationExample::ZSAMPLE, Int_t analysismode = ZmumuValidationExample::ANALYSIS_SAGITTADIST)
{
  gStyle->SetPalette(1);

  //////////////////////////////////
  // compile ZmumuValidationExample
  //////////////////////////////////
  //  gROOT->ProcessLine(".L ZmumuValidationExample.cxx+");
  
  /////////////////////////////////////////
  // define list of files to be analysed
  /////////////////////////////////////////
  std::list <std::string > s_fileNames;
  
  string output_dir = "./";
  string output_file_name = "ZmumuValidationExampleOutput.root";

  string output_tag = "initial_00302872"; 
  string output_tag_mod = "initial_00302872";
  string tree_name = "Default_InDetTrackParticle";
  //string tree_name = "TruthParams";
  //string tree_name = "Refit1_SiAndTRT";
  //string tree_name = "Refit2_SiOnly";
  //string tree_name = "Refit1Params"; 
  //string tree_name = "Refit2Params"; // Silicon only tracks (PIX+SCT)
  //string tree_name = "Default_InDetTrackParticle"; // 

  
   s_fileNames = userFiles();

   //MC
   //   string output_tag = "TrackingCP.nom.r6588.";  s_fileNames.push_back("../nomZmumuMC/r6588/MergedZmumuValidation_nom.root");
   //   string output_tag = "TrackingCP.nom.r6939.";  s_fileNames.push_back("../nomZmumuMC/r6939/MergedZmumuValidation_nom.root");


   // string output_tag = "GRL_v69_All_Good_subset_FromPeriodG"; s_fileNames.push_back("../NomWithGRL_FromPeriodG/MergedZmumuValidation.root");
   //      string output_tag = "GRL_v69_All_Good_subset_FromPeriodG_WithAlignConstEP"; s_fileNames.push_back("../NomWithGRL_WithAlignConstEP_FromPeriodG/MergedZmumuValidation.root");
   //     string output_tag = "ZmumuMC15a"; s_fileNames.push_back("../NomZmumuMC/MergedZmumuValidation_nom.root");

   /////////////////////////////////////////////////
   //define job parameters and run
   /////////////////////////////////////////////////
   int nIterations = 1; if (nIterationsUserInput > 0) nIterations = nIterationsUserInput;
   int nEvents = 100000000;
   //nEvents = 10000;
   bool isMC = false; //for IPResolStudies this option must be set hard-coded in the IPResolStudies.C for the time being, leaving this flag to false even if it is MC in that case
      
   std::vector <double> Jpsi1SMCbias (1); //makes a 1-component vector initialized with zero value at component zero, the mc bias will be filled from the first component on with push_back method

   // string tree_name = "IDPerfMon/Kshort/NoTriggerSelection/DefaultParams";
   //string tree_name = "DefaultParams"; // combined tracks
   //string tree_name = "Refit1Params"; // ID tracks (PIX+SCT+TRT)
   //string tree_name = "Refit2Params"; // Silicon only tracks (PIX+SCT)


   //string output_tag_mod = "epsilonm001_6_eta_3_phi_30iter";
   //string output_tag_mod = "upsilonmc_1eta_1phi";
   //string output_tag_mod = "ZScaleUp500.";
   //string output_tag_mod = "ZScaleDn500.";

   string output_file = "ZmumuValidationExampleOutput_test.root";
   //string output_file = Form("%sZmumuValidationExampleOutput.%s.%s.%s%s.root", output_dir.c_str(), (isMC ? "MC":"Data"), output_tag.c_str(), output_tag_mod.c_str(), tree_name.c_str());

   //output_file = Form("%sZmumuValidationExampleOutput_Zmumu_2016_10x9.root",output_dir.c_str());
   //   output_file = Form("%sZmumuValidationExampleOutput_jpsitest40x40.root",output_dir.c_str());

   ZmumuValidationExample myZmumuValidationExample(s_fileNames, tree_name, output_file, isMC);
   myZmumuValidationExample.SetSampleType(sampletype);
   myZmumuValidationExample.SetAnalysisType(analysismode);
   myZmumuValidationExample.SetPrintLevel(0);
   myZmumuValidationExample.SetEtaBins(6); 
   myZmumuValidationExample.SetPhiBins(6);
   myZmumuValidationExample.SetUserIteration(nIterations);
   // detector deformations / alignment weak modes
   myZmumuValidationExample.ApplyBFieldRotationTest(false);
   myZmumuValidationExample.ApplyEndcapExpansion(true);
   myZmumuValidationExample.ApplyRadialDeformation(false);
   myZmumuValidationExample.ApplySagittaHistogramTest(false);
   myZmumuValidationExample.ApplyTelescopeDeformation(false);
   // invariant mass fit
   myZmumuValidationExample.FitPositiveAndNegativeMuon(false);
   // delta sagitta comparing pt spectrum of mu+ and mu-
   myZmumuValidationExample.RunDeltaSagittaFitter(false);
   // Impact parameter resolution studies
   myZmumuValidationExample.RunIPResolStudies(false);
  
    
   if (sampletype == ZmumuValidationExample::JPSISAMPLE) {
     myZmumuValidationExample.SetEtaBins(12);
     myZmumuValidationExample.SetPhiBins(8); 
     Jpsi1SMCbias.push_back(0);  //eta 1, phi 1
     myZmumuValidationExample.SetMCbias(Jpsi1SMCbias);
     myZmumuValidationExample.SetMCbias(sampletype); // in MeV
   }
   if (sampletype == ZmumuValidationExample::UPSILONSAMPLE) {
     myZmumuValidationExample.SetEtaBins(3);
     myZmumuValidationExample.SetPhiBins(2);
     Jpsi1SMCbias.push_back(0);  //eta 1, phi 1
     myZmumuValidationExample.SetMCbias(sampletype); // in MeV
   }
   if (sampletype == ZmumuValidationExample::DIMUONSAMPLE) {
     myZmumuValidationExample.SetEtaBins(1);
     myZmumuValidationExample.SetPhiBins(1);
   }
   if (sampletype == ZmumuValidationExample::KSHORTSAMPLE) {
     myZmumuValidationExample.SetEtaBins(1);
     myZmumuValidationExample.SetPhiBins(1);
   }

   myZmumuValidationExample.bookHistograms();
   myZmumuValidationExample.configurationReport();


   for (int i = 0; i < nIterations; i++) {
     std::cout<< endl << " ==== ZmumuValidation ==== starting iteration "<< i+1 <<" of " << nIterations<< std::endl;
     myZmumuValidationExample.loop(nEvents);
   }
   
   myZmumuValidationExample.writeToFile(0);
   myZmumuValidationExample.DrawMap();

   std::cout<< "Finished - Output File: " << output_file << std::endl;

   return;
}
