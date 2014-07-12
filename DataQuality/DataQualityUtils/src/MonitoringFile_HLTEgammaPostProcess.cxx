/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// HLT Egamma post processing jobs
// 15/03/2012 initial file by Yan-Jie Schnellbach, based on HLT Tau
// **********************************************************************

#include "DataQualityUtils/MonitoringFile.h"

#include <cmath>
#include <vector>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TKey.h>
#include <TMath.h>
#include <TProfile.h>

namespace dqutils {


  //--------------------------------------------------------------------------------
  // main post processing function (opens file, detects trigger chains, calls
  // individual efficiency calculations
  //--------------------------------------------------------------------------------
  void MonitoringFile::HLTEgammaPostProcess(const std::string &inFilename, bool /* isIncremental */ ){
    // std::cout << "--> HLTEgammaPostProcess: Begin HLT Egamma post-processing" << std::endl;

    // ----- getting file -----

    //open root file
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
  
    //check files are loaded.  
    if (f == 0 || ! f->IsOpen() ) {
      std::cerr << "--> HLTEgammaPostProcess: Input file not opened" << std::endl;
      delete f;
      return;
    }

    //zombie files?
    if(f->IsZombie()) {
      std::cerr << "--> HLTEgammaPostProcess: Input file " << inFilename << " cannot be opened. " << std::endl;
      delete f;
      return;
    }
  
    //check file size is not too small.
    if(f->GetSize()<1000.) {
      std::cerr << "--> HLTEgammaPostProcess: Input file empty" << std::endl;
      f->Close();
      delete f;
      return; 
    }
  
    //build iterator
    TIter next_run ( f->GetListOfKeys() );
    TKey* key_run(0);

    //loop over keys in root directory
    while ((key_run = dynamic_cast<TKey*>( next_run() )) !=0 ) {
      TObject* obj_run = key_run->ReadObj();
      TDirectory* tdir_run = dynamic_cast<TDirectory*>( obj_run );
    
      //check we have a valid pointer
      if (tdir_run ==0 ) {
	delete obj_run;
	continue; 
      }

      //check name
      std::string runDirName( tdir_run->GetName() );
      //std::cout<<"Run_directory: "<<runDirName<<std::endl;
    
      // check run directory 
      //(comment this out for local testing with the TrigEgammaMonitoring recipe)
      if (runDirName.find("run") == std::string::npos ) {
	delete obj_run;
	continue; 
      }
 
      // ----- find egamma monitoring directory (EgammaSigTE) ------
      TString egammamonDirName = runDirName + "/HLT/EgammaSigTE";
      TDirectory* egammamonDir(0);
      if( !(egammamonDir = f->GetDirectory(egammamonDirName)) ) {
	std::cout << "--> HLTEgammaPostProcess: directory " << egammamonDirName << " not found." << std::endl;
	delete obj_run;
	return;
      }

      // ----- get handles for offline egamma object directories -----
      TString offgammaDirName = egammamonDirName + "/OfflinePhoton"; 
      TString offeleDirName = egammamonDirName + "/OfflineElectron"; 
      TDirectory* offgammaDir(0); TDirectory* offeleDir(0);
      if( !(offgammaDir = f->GetDirectory(offgammaDirName))){
	std::cout << "--> HLTEgammaPostProcess: offline directory " << offgammaDirName << " not found, will abort.\n";
	delete obj_run;
	return;
      }
      if( !(offeleDir = f->GetDirectory(offeleDirName))){ 
	std::cout << "--> HLTEgammaPostProcess: offline directory " << offeleDirName << " not found, will abort.\n"; 
	delete obj_run;
        return; 
      } 

      // ----- list the histograms for offline efficiencies -----
      // word of warning: hardcoded maximum of 3 in the efficiency functions 
      //                  if you add stages, increase the size of histo_matrix 
      //                  or histo_tensor respectively! 
      // note: offline effs use "egEt", "egEta" etc., this is hardcoded in the
      //       variable name formation in the offl. eff function.
      // finally: use std::vector<TString> varname = {"Et", "Eta", "Phi"}
      //          if we're ever going to use C++11 around this place...

      std::vector<TString> varName;
      varName.push_back("Et");
      varName.push_back("Eta");
      varName.push_back("Phi");

      // ----- chain stages for relative effs -----
      // word of warning: hardcoded maximum of 5 in the efficiency functions
      //                  if you add stages, increase the size of histo_matrix
      //                  or histo_tensor respectively!
      // finally: use std::vector<TString> phoStage = {"L1", "L2Calo", "EFCalo"}
      //          and std::vector<TString> phoStage = {"L1", "L2Calo", "L2ID", "EFCalo", "EFID"}
      //          if we're ever going to use C++11 around this place...

      //photons
      std::vector<TString> phoStage;
      phoStage.push_back("L1");
      phoStage.push_back("L2Calo");
      phoStage.push_back("EFCalo");

      //electrons
      std::vector<TString> eleStage;
      eleStage.push_back("L1");
      eleStage.push_back("L2Calo");
      eleStage.push_back("L2ID");
      eleStage.push_back("EFCalo");
      eleStage.push_back("EFID");

      // ----- loop over directories (i.e. trigger chains) ------
      TIter next_trig ( egammamonDir->GetListOfKeys() );
      TKey* key_trig(0);
      while ( (key_trig = dynamic_cast<TKey*>( next_trig() )) != 0 ) {

	//store chain specific info (and resets them)
	bool photon_chain(false);
	bool electron_chain(false);
	TObject*    obj_trig = key_trig->ReadObj();
	TDirectory* dir_trig = dynamic_cast<TDirectory*>( obj_trig );

        //check we have a valid pointer
        if (dir_trig == 0) {
          delete obj_trig;
          continue;
        }

	//get name of the current trigger chain
	TString trigger_name = dir_trig->GetName();

	//bow out of offline containers here (we don't monitor them, they're for bootstrapping)
	if (trigger_name.Contains("Offline")) {
          delete obj_trig;
	  continue;
	}

	//detect photon/electron chains (checks chain name, skips unrecognised chains)
	if(trigger_name.BeginsWith("g") || trigger_name.BeginsWith("2g")) photon_chain = true;
	else if(trigger_name.BeginsWith("e") || trigger_name.BeginsWith("2e")) electron_chain = true;
	else{
	  std::cout << "--> HLTEgammaPostProcess: Non-electron/photon chain detected, will be skipped!\n";
          delete obj_trig;
	  continue;
	}
	
	//call photon post processing routine (no track directories, only Calo)
	if(photon_chain){
	  //std::cout<<"--> HLTEgammaPostProcess: calling photon functions for "<<egammamonDirName<<" and trigger item="<<dir_trig->GetName()<<" in directory: "<<getPath(dir_trig)<<endl;
	  
	  //calling boostrap efficiency calcs
	  HLTEgammaEfficiencyOff(f,dir_trig, offgammaDir, "L1/MatchedOffPho/", "L1/EffPho/", varName);
	  HLTEgammaEfficiencyOff(f,dir_trig, offgammaDir, "L2Calo/MatchedOffPho/", "L2Calo/EffPho/", varName);
	  HLTEgammaEfficiencyOff(f,dir_trig, offgammaDir, "EFCalo/MatchedOffPho/", "EFCalo/EffPho/", varName);
	  HLTEgammaEfficiencyOff(f,dir_trig, offgammaDir, "EFeg/MatchedOffPho/", "EFeg/EffPho/", varName);
	  
	  //calling relative efficiency calcs
	  HLTEgammaEfficiencyRel(f,dir_trig, "PreRejection/", "Rejection/", phoStage, varName);
	}
	
	//call electron post processing (the full thing)
	if(electron_chain){ 
	  //std::cout<<"--> HLTEgammaPostProcess: calling electron functions for "<<egammamonDirName<<" and trigger item="<<dir_trig->GetName()<<" in directory: "<<getPath(dir_trig)<<endl; 

	  //calling bootstrap efficiency calcs
	  HLTEgammaEfficiencyOff(f,dir_trig, offeleDir, "L1/MatchedOffEle/", "L1/EffEle/", varName);
	  HLTEgammaEfficiencyOff(f,dir_trig, offeleDir, "L2Calo/MatchedOffEle/", "L2Calo/EffEle/", varName);
	  HLTEgammaEfficiencyOff(f,dir_trig, offeleDir, "L2ID/MatchedOffEle/", "L2ID/EffEle/", varName);
	  HLTEgammaEfficiencyOff(f,dir_trig, offeleDir, "EFCalo/MatchedOffEle/", "EFCalo/EffEle/", varName);
	  HLTEgammaEfficiencyOff(f,dir_trig, offeleDir, "EFID/MatchedOffEle/", "EFID/EffEle/", varName);
	  HLTEgammaEfficiencyOff(f,dir_trig, offeleDir, "EFeg/MatchedOffEle/", "EFeg/EffEle/", varName);
	  
	  //calling relative efficiency calcs
	  HLTEgammaEfficiencyRel(f,dir_trig, "PreRejection/", "Rejection/", eleStage, varName);
	} 
	
	
      }
      
    }
    
    //clean up after ourselves...
    f->Close();
    delete f;
    //std::cout << "--> HLTEgammaPostProcess: finished HLT Egamma post-processing"<<std::endl;
  }
  
}
