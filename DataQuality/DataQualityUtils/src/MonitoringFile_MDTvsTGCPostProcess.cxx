/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
//Methods to Process TGC Histograms after merging.
//Author: Akimasa Ishikawa (akimasa.ishikawa@cern.ch)
//Date:   Aug 2009
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataQualityUtils/MonitoringFile.h"
#include "DataQualityUtils/CoolTgc.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <utility>
#include <map>
#include <string>

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TMath.h"
#include "TTree.h"
#include "TBranch.h"

namespace dqutils {
  
  void
  MonitoringFile::MDTvsTGCPostProcess( const std::string & inFilename, bool /* isIncremental */ )
 { 
    //start postprocessing
    MDTvsTGCEfficiency(inFilename);
  }
  
  void
  MonitoringFile::MDTvsTGCEfficiency( const std::string & inFilename )
  {
    PostProcessorFileWrapper mf(inFilename, "MDTvsTGC Efficiency");
    
    if(!mf.IsOpen()){
      //std::cerr << "MDTvsTGCEfficiency(): "
      //	<< "Input file not opened \n";
      return;
    }
    if(mf.GetSize()<1000.){
      //std::cerr << "MDTvsTGCEfficiency(): "
      //	<< "Input file empty \n";
      return;
    }
    
    std::stringstream ss;
    TString sac[2]    ={"A","C"};
    TString sws[2]    ={"Wire","Strip"};
    TString smethod[3]={"SegmTrack","MidOnly",""};
    
    TIter next_run ( mf.GetListOfKeys() );
    TKey* key_run(0);
    while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
      if (!key_run->IsFolder()) continue;
      TString run_dir = key_run->GetName();
      if (!run_dir.Contains("run") )   continue;
      
      TString seff;
      TString snum;
      TString sden;
      TString serr;

      TH2F *h2eff(0);
      TH2F *h2num(0);
      TH2F *h2den(0);
      TH2F *h2err(0);
      
      TString mdtvstgc_dir = run_dir + "/Muon/MuonRawDataMonitoring/MDTvsTGC/";
      
      TString mdtvstgc_sub_dir[2] = {mdtvstgc_dir + "TGCEA/", mdtvstgc_dir + "TGCEC/"};
      
      if(mf.cd(mdtvstgc_dir)) mf.cd(mdtvstgc_dir);
      else {
	//std::cerr << "No MDTvsTGC Directory! " << std::endl; mf.error();
	return;
      }
     
      for(int ac=0;ac<2;ac++){
	TString eff_dir = mdtvstgc_sub_dir[ac];
	TDirectory* dir = mf.GetDirectory(eff_dir);
	if(!dir){
	  //std::cerr<< "TGCHistogramDivision: directory "<<eff_dir<<" not found"<<std::endl;
	  return;
	}
	
	for(int ws=0;ws<2;ws++){
	  // Merge SegmTrack and Midstation-only method results together to make total histograms
	  bool oktomerge = true;
	  
	  TList *nummergelist = new TList();
	  for(int imethod=0;imethod<2;imethod++){
	    ss << sws[ws] << "_EfficiencyAgainstMDT_Map" << smethod[imethod] << "num_" << sac[ac];
	    snum   = ss.str();
	    ss.str("");
	    h2num = 0;
	    mf.get(snum, h2num, dir);
	    if(!h2num){
	      //std::cerr <<"MDTvsTGC PostProcessing: no such histogram!! "<< snum << std::endl;
	      oktomerge = false;
	    }
	    nummergelist->Add(h2num);
	  }
	  ss << sws[ws] << "_EfficiencyAgainstMDT_Map" << smethod[2] << "num_" << sac[ac];
	  snum   = ss.str();
	  ss.str("");
	  h2num = 0;
	  mf.get(snum, h2num, dir);
	  if(!h2num){
	    //std::cerr <<"MDTvsTGC PostProcessing: no such histogram!! "<< snum << std::endl;
	    oktomerge = false;
	  }

	  TList *denmergelist = new TList();
	  for(int imethod=0;imethod<2;imethod++){
	    ss << sws[ws] << "_EfficiencyAgainstMDT_Map" << smethod[imethod] << "denom_" << sac[ac];
	    sden   = ss.str();
	    ss.str("");
	    h2den = 0;
	    mf.get(sden, h2den, dir);
	    if(!h2den){
	      //std::cerr <<"MDTvsTGC PostProcessing: no such histogram!! "<< sden << std::endl;
	      oktomerge = false;
	    }
	    denmergelist->Add(h2den);
	  }
	  ss << sws[ws] << "_EfficiencyAgainstMDT_Map" << smethod[2] << "denom_" << sac[ac];
	  sden   = ss.str();
	  ss.str("");
	  h2den = 0;
	  mf.get(sden, h2den, dir);
	  if(!h2den){
	    //std::cerr <<"MDTvsTGC PostProcessing: no such histogram!! "<< sden << std::endl;
	    oktomerge = false;
	  }
	  
	  if(oktomerge){
	    MDTvsTGCResetContents(h2num);
	    MDTvsTGCResetContents(h2den);
	    h2num->Merge(nummergelist);
	    h2den->Merge(denmergelist);
            
            dir->cd();
            h2num->Write("",TObject::kOverwrite);
            h2den->Write("",TObject::kOverwrite);
	  }
	    
	  // Postprocess all methods
	  for(int imethod=0;imethod<3;imethod++){
	    ss.str("");
	    ss << sws[ws] << "_EfficiencyAgainstMDT_Map" << smethod[imethod] << "_" << sac[ac];
	    seff   = ss.str();
	    ss.str("");
	    ss << sws[ws] << "_EfficiencyAgainstMDT_Map" << smethod[imethod] << "num_" << sac[ac];
	    snum   = ss.str();
	    ss.str("");
	    ss << sws[ws] << "_EfficiencyAgainstMDT_Map" << smethod[imethod] << "denom_" << sac[ac];
	    sden = ss.str();
	    ss.str("");
	    ss << sws[ws] << "_EfficiencyAgainstMDT_Map" << smethod[imethod] << "error_" << sac[ac];
	    serr = ss.str();
	    ss.str("");

	    h2eff = 0;
	    mf.get(seff, h2eff, dir);
	    h2num = 0;
	    mf.get(snum, h2num, dir);
	    h2den = 0;
	    mf.get(sden, h2den, dir);
	    h2err = 0;
	    mf.get(serr, h2err, dir);
	    
	    if( h2eff && h2num && h2den){
	      MDTvsTGCResetContents(h2eff);
	      h2eff->Divide(h2num, h2den, 1., 1., "B");
	      
	      // Make Error Histograms
	      int nX=h2eff->GetNbinsX();int nY=h2eff->GetNbinsY();
	      int beff, bdenom, berror;
	      float feff, fdenom, ferror;
	      for(int x=1;x<=nX;x++){
		for(int y=1;y<=nY;y++){
		  beff=h2eff->GetBin(x,y);        bdenom=h2den->GetBin(x,y);
		  feff=h2eff->GetBinContent(beff);fdenom=h2den->GetBinContent(bdenom);
		  ferror=sqrt(feff*(1-feff)/fdenom);
		  
		  if(h2err){
		    berror=h2err->GetBin(x,y);
		    h2err->SetBinContent(berror,ferror);
		  }
		  h2eff->SetBinError(beff,ferror);
		}
	      }
	    }
	    else{
	      //std::cerr <<"TGC PostProcessing: no such histogram (or num/denom)!! "<< seff << std::endl;
	      continue;
	    }
            // save histograms
            dir->cd();
            h2eff->Write("",TObject::kOverwrite);
            if(h2err)h2err->Write("",TObject::kOverwrite);
	  }// method
	}// wirestrip
        mf.Write();
      }// ac
    }// run_dir
    mf.Write();
  }
  
  void
  MonitoringFile::MDTvsTGCResetContents(TH1* h) {    
  // copy all functions of histogram before resetting
    TList* h_funcs; 
    h_funcs = dynamic_cast<TList*> (h->GetListOfFunctions()->Clone());
    h->Reset(); 
    //now insert these functions back into the hist
    TIter iterE(h_funcs);
    while ( (iterE()) ){h->GetListOfFunctions()->Add(*iterE);}
    delete h_funcs; 
    // are the functions still valid after list deletion?
    //http://root.cern.ch/root/html/TList#TList:_TList  should be!
    //  TIter iterTest(EffBCap->GetListOfFunctions());
    //  while( iterTest() ) std::cout << (*iterTest)->GetName() << std::endl;    
  }
  
}
