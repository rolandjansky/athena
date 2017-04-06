/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #include <iostream>
#include <string>
#include <list>
#include "TROOT.h"
#include "ZmumuValidationExample.cxx"

void run_ZmumuValidationExample(Int_t nIterationsUserInput=20)
{

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


   //test files
   //s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data11_7TeV.periodM2.physics_Muons.PhysCont.DESD_ZMUMU.t0pro09_v01.ZmumuValidationTest");
   //s_fileNames.push_back("/home/stahlman/testarea/AlignmentDev/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/ZmumuValidationOut.root");

   //190608_191239 tag validation
   //string output_tag = "190608_191239"; s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data11_7TeV.190608_191239.physics_Muons.recon.DESD_ZMUMU.f411_m716_f411.ZmumuTuple.v1");

   //189598_189845 tag validation
   //string output_tag = "189598_189845"; s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data11_7TeV.189598_189845.physics_Muons.recon.DESD_ZMUMU.f406_m716_f406.ZmumuTuple");

   //2012 Period A
   string output_tag = "PeriodA";
   //   s_fileNames.push_back("/afs/cern.ch/user/s/sthenkel/work/A20ProjectArea/Validation/Zmumu/local/0/ZmumuValidationOut.root");
   s_fileNames.push_back("/afs/cern.ch/user/s/sthenkel/eos/atlas/user/s/sthenkel/ProcessedOutput/Zmumu_BS_mig/user.sthenkel.t12.valid1.167824.Sherpa_CT10_ZmumuMassiveCBPt280_500_BFilter.ESD.e3099_s1982_s1964_r6006_ZmumuValidation/mergedOutput/MergedZmumuValidation.root");
   /*
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA3.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427142453");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA3.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.12042714251
0");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA3.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427142526");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA4.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427142559");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA4.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427142622");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA4.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427142644");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA4.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427142704");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA4.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427142729");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA4.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427142756");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA4.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427142818");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA5.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427142854");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA6.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427142940");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA7.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427143018");
   s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.data12_8TeV.periodA7.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.ZmumuTuple.v1.120427143034");
   */

   //MC validation study - big ZDeltaPhi
   //string output_tag = "ZDeltaPhi_01";  s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.mc10_7TeV.106047.PythiaZmumu_no_filter.recon.ESD.e574_s933_s946_r2425.ZmumuTuple.ZDeltaPhi_01.v4.120326010304");

   //MC validation study - small ZDeltaPhi
   //string output_tag = "ZDeltaPhi_02";  s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.mc10_7TeV.106047.PythiaZmumu_no_filter.recon.ESD.e574_s933_s946_r2425.ZmumuTuple.ZDeltaPhi_02.v1.120403101909");

   //MC validation study - RDeltaPhi
   //string output_tag = "RDeltaPhi_01";  s_fileNames.push_back("/local_data0/stahlman/data/ZmumuValidation/user.stahlman.mc10_7TeV.106047.PythiaZmumu_no_filter.recon.ESD.e574_s933_s946_r2425.ZmumuTuple.RDeltaPhi_01.v4.120326010436");

   /////////////////////////////////////////////////
   //define job parameters and run
   /////////////////////////////////////////////////
   int nIterations = 5; if (nIterationsUserInput > 0) nIterations = nIterationsUserInput;
   int nEvents = 0;
   bool isMC = false;

   //string tree_name = "DefaultParams";
   string tree_name = "Refit1Params";

   string output_tag_mod = "";
   //string output_tag_mod = "ZScaleUp500.";
   //string output_tag_mod = "ZScaleDn500.";

   string output_file = Form("%sZmumuValidationExampleOutput.%s.%s.%s%s.root", output_dir.c_str(), (isMC ? "MC":"Data"), output_tag.c_str(), output_tag_mod.c_str(), tree_name.c_str());

   ZmumuValidationExample myZmumuValidationExample(s_fileNames, tree_name, output_file, isMC);
   myZmumuValidationExample.SetPrintLevel(0);
   myZmumuValidationExample.SetEtaBins(10);
   myZmumuValidationExample.SetPhiBins(10);
   myZmumuValidationExample.bookHistograms();

   for (int i = 0; i < nIterations; i++) {
     std::cout<< endl << " ==== ZmumuValidation ==== starting iteration "<< i+1 <<" of " << nIterations<< std::endl;
     myZmumuValidationExample.loop(nEvents);
   }

   myZmumuValidationExample.writeToFile(0);
   myZmumuValidationExample.DrawMap();

   std::cout<< "Finished - Output File: " << output_file << std::endl;

   return;
}
