/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// HLT MinBias post processing jobs
// 09/06/2015: update by Andrzej Zemla <azemla@cern.ch>
// **********************************************************************

#include "DataQualityUtils/MonitoringFile.h"

#include <cmath>
#include <vector>

#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TKey.h>
#include <TMath.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TString.h>

namespace dqutils {
  
  
  //--------------------------------------------------------------------------------
  // main process
  //--------------------------------------------------------------------------------
  void MonitoringFile::HLTMinBiasMonPostProcess( const std::string & inFilename, bool /* isIncremental */ ){
    //     std::cout << "--> HLTMinBiasMonPostProcess: Begin HLT MinBias    post-processing" << std::endl;
    
    //open root file
    TFile* f = TFile::Open(inFilename.c_str(),"READ");
    
    //check files are loaded.  
    if (f == 0 || ! f->IsOpen() ) {
      //       std::cerr << "--> HLTMinBiasMonPostProcess: Input file not opened" << std::endl;
      delete f;
      return;
    }
    
    //zombie files?
    if(f->IsZombie()) {
      //       std::cerr << "--> HLTMinBiasMonPostProcess: Input file " << inFilename << " cannot be opened. " << std::endl;
      delete f;
      return;
    }
    
    //check file size is not too small.
    if(f->GetSize()<1000.) {
      //       std::cerr << "--> HLTMinBiasMonPostProcess: Input file empty" << std::endl;
      f->Close();
      delete f;
      return; 
    }
    //build iterator
    TIter next_run ( f->GetListOfKeys() );
    TKey* key_run(0);
    TString minbiasmonDirName;
    
    std::vector< std::pair<TString,TString> > *v_targetNames = new std::vector< std::pair<TString,TString> >(0);

    
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
      //       std::cout<<"Run_directory: "<<runDirName<<std::endl;
      
      if (runDirName.find("run") == std::string::npos ) {
	delete obj_run;
	continue; 
      }
      
      //find MinBiasMon dir
      minbiasmonDirName = runDirName + "/HLT/MinBiasMon";
      TDirectory* minbiasmonDir(0);
      if( !(minbiasmonDir = f->GetDirectory(minbiasmonDirName)) ) {
// 	std::cerr << "--> HLTMinBiasMonPostProcess: directory " << minbiasmonDirName << " not found." << std::endl;
	delete v_targetNames;
	return;
      }
      HLTMinBiasMonGetTargetHistos(gDirectory, *v_targetNames);
    }
    
    f->Close();
    f=0;
    
    f = TFile::Open(inFilename.c_str(),"UPDATE");
    
    const TString repPath = TString(f->GetPath());
    
    //first update global efficiency
    f->cd(minbiasmonDirName);
    
    TH1F* h_triggEffic 	    = dynamic_cast<TH1F*>(gDirectory->Get("TriggerEfficiencies"));
    TH1F* h_triggEfficPass  = dynamic_cast<TH1F*>(gDirectory->Get("TriggerEfficienciesPassed"));
    TH1F* h_triggEfficAll   = dynamic_cast<TH1F*>(gDirectory->Get("TriggerEfficienciesAll"));
    
    h_triggEffic->Divide(h_triggEfficPass,h_triggEfficAll,1.,1.,"B");
    h_triggEffic->GetYaxis()->SetRangeUser(0.,1.2);
    h_triggEffic->Write("",TObject::kOverwrite);
    
    for(uint k =0; k < v_targetNames->size();k++){
      v_targetNames->at(k).first.ReplaceAll(repPath,"");
      
      f->cd(v_targetNames->at(k).first);
      
      TH1F* h_target = dynamic_cast<TH1F*>(gDirectory->Get(v_targetNames->at(k).second));
      TH1F* h_num  = dynamic_cast<TH1F*>(gDirectory->Get(v_targetNames->at(k).second + "Passed"));
      TH1F* h_den  = dynamic_cast<TH1F*>(gDirectory->Get(v_targetNames->at(k).second + "All")); 
      
      if(h_target != 0 && h_num !=0 && h_den != 0){
	h_target->Divide(h_num,h_den,1.,1.,"B");
	h_target->GetYaxis()->SetRangeUser(0.,1.2);
	h_target->Write("",TObject::kOverwrite);
      }
    }
    v_targetNames->clear();
    delete v_targetNames;
    v_targetNames=0;

    f->Close();
    delete f;
  }
  
  void MonitoringFile::HLTMinBiasMonGetTargetHistos(TDirectory *source,std::vector< std::pair< TString ,TString > >& targetNames)
  {
    TKey *key;
    TIter nextkey(source->GetListOfKeys());
    while ((key = (TKey*)nextkey())) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom(TDirectory::Class())) {
	
	TString kname = key->GetName();
	
	source->cd(key->GetName());
	TDirectory *nextdir = gDirectory;
	HLTMinBiasMonGetTargetHistos(nextdir,targetNames);
      }
      if (cl->InheritsFrom(TH1F::Class())) {
	TString cRatio=TString(key->GetName());
	if(cRatio=="Efficiency" || 
	  cRatio=="Purity" || 
	  cRatio=="EfficienciesTrigger" || 
	  cRatio=="EfficiencyTracks" ||
    cRatio=="TriggerPurities") 
	    targetNames.push_back( std::pair<TString,TString>(TString(gDirectory->GetPath()),TString(key->GetName())));
	
      }
    }
  }
}


