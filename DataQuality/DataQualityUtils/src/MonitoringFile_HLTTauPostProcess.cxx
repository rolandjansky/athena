/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// HLT Tau post processing jobs
// 01/03/2011: update by ccuenca
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
  // main process
  //--------------------------------------------------------------------------------
  void MonitoringFile::HLTTauPostProcess( const std::string & inFilename, bool /* isIncremental */ ){
    //std::cout << "--> HLTTauPostProcess: Begin HLT Tau post-processing" << std::endl;
  
    //open root file
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
  
    //check files are loaded.  
    if (f == 0 || ! f->IsOpen() ) {
      //std::cerr << "--> HLTTauPostProcess: Input file not opened" << std::endl;
      delete f;
      return;
    }

    //zombie files?
    if(f->IsZombie()) {
      //std::cerr << "--> HLTTauPostProcess: Input file " << inFilename << " cannot be opened. " << std::endl;
      delete f;
      return;
    }
  
    //check file size is not too small.
    if(f->GetSize()<1000.) {
      //std::cerr << "--> HLTTauPostProcess: Input file empty" << std::endl;
      f->Close();
      delete f;
      return; 
    }
  

    //making sure we are in the right directory
    //   f->cd("/");
    //   f->pwd();
    //   f->ls();

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
    
      if (runDirName.find("run") == std::string::npos ) {
	delete obj_run;
	continue; 
      }

      //find tauMon dir
      TString taumonDirName = runDirName + "/HLT/TauMon";
      TDirectory* taumonDir(0);
      if( !(taumonDir = f->GetDirectory(taumonDirName)) ) {
	//std::cerr << "--> HLTTauPostProcess: directory " << taumonDirName << " not found." << std::endl;
	return;
      }


      std::vector<TString> varName;
      varName.push_back("Et");
      varName.push_back("Eta");
      varName.push_back("Phi");
      
      std::vector<TString> lvlName;
      lvlName.push_back("L1");
      lvlName.push_back("L2");
      lvlName.push_back("EF");
      
      std::vector< std::pair< int, int > > ratioIndex;
      ratioIndex.push_back( std::make_pair(2,0) ); // EF vs L1
      ratioIndex.push_back( std::make_pair(1,0) ); // L2 vs L1
      ratioIndex.push_back( std::make_pair(2,1) ); // EF vs L2
      
      
      
      std::vector<TString> varName0;
      varName0.push_back("Pt");
      varName0.push_back("Eta");
      varName0.push_back("Phi");
      varName0.push_back("Nvtx");
      
      std::vector<TString> lvlNameO;
      lvlNameO.push_back("L1");
      lvlNameO.push_back("L2");
      lvlNameO.push_back("EF");
      lvlNameO.push_back("");//offline
      
      std::vector< std::pair< int, int > > ratioIndexO;
      ratioIndexO.push_back( std::make_pair(0,3) ); // L1 vs off
      ratioIndexO.push_back( std::make_pair(1,3) ); // L2 vs off
      ratioIndexO.push_back( std::make_pair(2,3) ); // EF vs off

    
      //loop over directories
      TIter next_trig ( taumonDir->GetListOfKeys() );
      TKey* key_trig(0);
      while ( (key_trig = dynamic_cast<TKey*>( next_trig() )) != 0 ) {
	TObject*    obj_trig = key_trig->ReadObj();
	TDirectory* dir_trig = dynamic_cast<TDirectory*>( obj_trig );
        if (!dir_trig) continue;
	//std::cout<<"--> HLTTauPostProcess: calling functions for " <<  dir_trig->GetName() << endl;
	// "<<runDirName<<" and trigger item="<<dir_trig->GetName()<<endl;
      
	HLTTauPostProcess(f,dir_trig,"/RelativeEfficiency","/RelativeEfficiency/Efficiency",
			  lvlName,varName,ratioIndex, 1);//name style : 1 (relative eff)
	HLTTauPostProcess(f,dir_trig,"/OfflineRatio","/OfflineRatio/Ratio",
			  lvlNameO,varName0,ratioIndexO, 2);//name style : 2 (offline eff)
	HLTTauPostProcess(f,dir_trig,"/OfflineRatio/BDTMedium","/OfflineRatio/Ratio/BDTMedium",
			  lvlNameO,varName0,ratioIndexO, 2, "BDTMedium");//name style : 2 (offline eff)
      


      }
    }
  
    f->Close();
    delete f;
    //std::cout << "--> HLTTauPostProcess: finished HLT Tau post-processing"<<std::endl;
  }
  
  
  void MonitoringFile::HLTTauPostProcess( TFile* f, TDirectory* dir, 
					  TString pathApp, TString pathAppEff, 
					  std::vector<TString> lvlN, std::vector<TString> varN,
					  std::vector< std::pair< int, int > > ratioIndex, int nameStyle,
                                          TString nameApp){

    std::string path = getPath(dir);
  
    //cout<<"HLTTauPostProcess in "<< path
//         <<" pathApp="<<pathApp
//         <<" pathAppEff="<<pathAppEff
//         <<" nameStyle="<<nameStyle<<endl;

    TString basePath = path + pathApp + "/";
    if(  f->cd(basePath.Data())==0 ) {
      //cout<<"basePath isn't there!"<<endl;
      return;
    }
  
    //TH1F* hRoI[lvlN.size()][varN.size()];
    TH1F* hRoI[100][100];
    for (unsigned int iLvl = 0; iLvl < lvlN.size(); iLvl ++) {
      for (unsigned int iVar = 0; iVar < varN.size(); iVar ++) {
	TString hName;
	if (nameStyle == 1) hName = basePath + "h" + lvlN[iLvl] + "RoI" + varN[iVar] + (iLvl == 0 ? "Denom" : "Num") + nameApp;
	if (nameStyle == 2) hName = basePath + "hTau" + varN[iVar] + lvlN[iLvl] + nameApp;
	if ( ! CheckHistogram(f, hName.Data()) ) {
          //cout<<" histo "<<hName<<" is not in f "<<f->GetName()<<endl;
          return;
        }
	hRoI[iLvl][iVar] = (TH1F*) ( f->Get( hName.Data() ) ) ->Clone();
      }
    }


    basePath += path + pathAppEff + "/";
    f->cd(basePath.Data());
    TH1F* hEff[100][100];
    for (unsigned int iVar = 0; iVar < varN.size(); iVar ++) {
      for (unsigned int iRatio = 0 ; iRatio < ratioIndex.size() ; iRatio ++ ) {
      
	TString hName;
	if (nameStyle == 1) 
          hName = basePath + "h" + 
	    lvlN[ ratioIndex[ iRatio ].first ] + "vs" + lvlN[ ratioIndex[ iRatio ].second ] + 
	    varN[iVar] + "Efficiency" + nameApp;

	if (nameStyle == 2)
          hName = basePath + "h" + 
	    lvlN[ ratioIndex[ iRatio ].first ] + "OfflineRatio" +
	    varN[iVar] + nameApp;
      
	if ( ! CheckHistogram(f, hName.Data()) ) return;
	hEff[iRatio][iVar] = (TH1F*) ( f->Get(hName.Data()) );
	hEff[iRatio][iVar] -> Divide ( hRoI[ ratioIndex[ iRatio ].first  ][iVar], 
				       hRoI[ ratioIndex[ iRatio ].second ][iVar], 
				       1.0, 1.0, "b" );
      
	hEff[iRatio][iVar] -> Write ( "" , TObject::kOverwrite );
      }
    }
  
    f->Write();
  }
 
}


