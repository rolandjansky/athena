/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//Methods to process SCT histograms after merging.
//Author: Masaki ENDO (masaki.endo@cern.ch)
//Date: Aug 2012
/////////////////////////////////////////////////////////////////////

#include "DataQualityUtils/MonitoringFile.h"

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
#include "TF1.h"
#include "TTree.h"
#include "TBranch.h"


namespace dqutils {

  static const bool rno_debug = false;

  void
  MonitoringFile::SCTPostProcess( std::string inFilename, bool /* isIncremental */ )
  {
    if (rno_debug) std::cout << "Start SCT post-processing" <<std::endl;

    TFile *infile = TFile::Open(inFilename.c_str(),"UPDATE");
    if(infile==0 || ! infile->IsOpen()){
      std::cerr << "--> SCTPostProcess: Input file not opened" <<std::endl;
      return;
    }
    if(infile->IsZombie()){
      std::cerr << "--> SCTPostProcess: Input file " << inFilename << " cannot be opened" <<std::endl;
      return;
    }
    if(infile->GetSize()<1000){
      std::cerr << "--> SCTPostProcess: Input file empty " <<std::endl;
      return;
    }

    //start postprocessing
    
    TIter next_run(infile->GetListOfKeys());
    TKey *key_run(0);
    key_run = (TKey*)next_run();
    TDirectory *dir0 = dynamic_cast<TDirectory*> (key_run->ReadObj());
    if (!dir0) return; // should never fail
    dir0->cd();

    TString run_dir;
    int times = 1;
    while (times--) {  // just once
      run_dir = dir0->GetName();
      TString rno_dir = run_dir + "/SCT/GENERAL/RatioNoise/";
      
      TDirectory *dir = infile->GetDirectory(rno_dir);
      if(!dir){
	std::cerr << "--> SCTPostProcess: directory " << rno_dir << " not found " <<std::endl;
	return;
      }
      
      TString noSideHitHist = rno_dir + "h_NZ1_vs_modnum";
      TString oneSideHitHist = rno_dir + "h_N11_vs_modnum";
      
      TH1F *h_nosidehit = (TH1F*)infile->Get(noSideHitHist);
      TH1F *h_onesidehit = (TH1F*)infile->Get(oneSideHitHist);
      
      if(!h_nosidehit || !h_onesidehit){
	std::cerr << "--> SCTPostProcess: no such histogram " <<std::endl;
	return;
      }
      
      
      TH1F *hist_all = new TH1F("h_NO","Noise Occupancy for Barrel and Endcaps",500,0.,100.);
      TH1F *hist_bar = new TH1F("h_NOb","Noise Occupancy for Barrel",500,0.,100.);
      TH1F *hist_end = new TH1F("h_NOEC","Noise Occupancy for Endcaps",500,0.,100.);
      TH1F *hist_endc = new TH1F("hist_endcapC","endcap C",500,0.,100.);
      TH1F *hist_enda = new TH1F("hist_endcapA","endcap A",500,0.,100.);
      
      TH1F *hist_bar_layer[4];
      for(int i=0;i<4;i++){
	hist_bar_layer[i] = new TH1F(Form("h_NOb_layer%d",i),Form("Noise Occupancy Barrel Layer %d",i),500,0.,100.);
      }
      TH1F *hist_endC_disk[9];
      for(int i=0;i<9;i++){
	hist_endC_disk[i] = new TH1F(Form("h_NOECC_disk%d",i),Form("Noise Occupancy Endcap C disk %d",i),500,0.,100.);
      }
      TH1F *hist_endA_disk[9];
      for(int i=0;i<9;i++){
	hist_endA_disk[i] = new TH1F(Form("h_NOECA_disk%d",i),Form("Noise Occupancy Endcap A disk %d",i),500,0.,100.);
      }
      TH1F *hist_inner = new TH1F("h_NOEC_Inner","Noise Occupancy Inner Endcap Modules",500,0.,100.);
      TH1F *hist_middle = new TH1F("h_NOEC_Middle","Noise Occupancy Middle Endcap Modules",500,0.,100.);
      TH1F *hist_outer = new TH1F("h_NOEC_Outer","Noise Occupancy Outer Endcap Modules",500,0.,100.);
      TH1F *hist_smiddle = new TH1F("h_NOEC_ShortMiddle","Noise Occupancy Short Middle Endcap Modules",500,0.,100.);
      
      int nModuleEndCap1[9] = {0, 92, 224, 356, 488, 620, 752, 844, 936};
      int nModuleEndCap2[9] = {92, 224, 356, 488, 620, 752, 844, 936, 988};
      int nModuleBarrel1[4] = {0, 384, 864, 1440};
      int nModuleBarrel2[4] = {384, 864, 1440, 2112};
      
      for(int i=0;i<4088;i++){
	int nosidehit = h_nosidehit->GetBinContent(i+1);
	int onesidehit = h_onesidehit->GetBinContent(i+1);
	if(nosidehit>0){
	  double Rval = (double)onesidehit/(double)nosidehit;
	  double NOval = 1./768.*Rval/(2.+Rval);
	  if(NOval!=0){
	    hist_all->Fill(100000.*NOval);
	    if(i>=0 && i<988){
	      hist_end->Fill(100000.*NOval);
	      hist_endc->Fill(100000.*NOval);
	      for(int j=0;j<9;j++){
		if(i>=nModuleEndCap1[j] && i<nModuleEndCap2[j]){
		  hist_endC_disk[j]->Fill(100000.*NOval);
		  if(i-nModuleEndCap1[j]<52){
		    hist_outer->Fill(100000.*NOval);
		  }else if(i-nModuleEndCap1[j]>=52 && i-nModuleEndCap1[j]<92){
		    if(i<844){
		      hist_middle->Fill(100000.*NOval);
		    }else if(i>=844){
		      hist_smiddle->Fill(100000.*NOval);
		    }
		  }else if(i-nModuleEndCap1[j]>=92 && i-nModuleEndCap1[j]<132){
		    hist_inner->Fill(100000.*NOval);
		  }
		}
	      }
	    }else if(i>=988 && i<3100){
	      hist_bar->Fill(100000.*NOval);
	      for(int j=0;j<4;j++){
		if(i>=nModuleBarrel1[j]+988 && i<nModuleBarrel2[j]+988){
		  hist_bar_layer[j]->Fill(100000*NOval);
		}
	      }
	    }else if(i>=3100 && i<4088){
	      hist_end->Fill(100000.*NOval);
	      hist_enda->Fill(100000.*NOval);
	      for(int j=0;j<9;j++){
		if(i>=nModuleEndCap1[j]+3100 && i<nModuleEndCap2[j]+3100){
		  hist_endA_disk[j]->Fill(100000*NOval);
		  if(i-nModuleEndCap1[j]-3100<52){
		    hist_outer->Fill(100000.*NOval);
		  }else if(i-nModuleEndCap1[j]-3100>=52 && i-nModuleEndCap1[j]-3100<92){
		    if(i-3100<844){
		      hist_middle->Fill(100000.*NOval);
		    }else if(i-3100>=844){
		      hist_smiddle->Fill(100000.*NOval);
		    }
		  }else if(i-nModuleEndCap1[j]-3100>=92 && i-nModuleEndCap1[j]-3100<132){
		    hist_inner->Fill(100000.*NOval);
		  }
		}
	      }
	    }
	  }
	}
      }
      hist_all->SetBinContent(hist_all->GetNbinsX(),hist_all->GetBinContent(hist_all->GetNbinsX() ) + hist_all->GetBinContent(hist_all->GetNbinsX() +1));
      hist_bar->SetBinContent(hist_bar->GetNbinsX(),hist_bar->GetBinContent(hist_bar->GetNbinsX() ) + hist_bar->GetBinContent(hist_bar->GetNbinsX() +1));
      hist_end->SetBinContent(hist_end->GetNbinsX(),hist_end->GetBinContent(hist_end->GetNbinsX() ) + hist_end->GetBinContent(hist_end->GetNbinsX() +1));
      
      dir->cd();
      hist_all->Write("", TObject::kOverwrite);
      hist_bar->Write("", TObject::kOverwrite);
      hist_end->Write("", TObject::kOverwrite);
      for(int i=0;i<4;i++){
	hist_bar_layer[i]->Write("", TObject::kOverwrite);
      }
      for(int i=0;i<9;i++){
	hist_endC_disk[i]->Write("", TObject::kOverwrite);
      }
      for(int i=0;i<9;i++){
	hist_endA_disk[i]->Write("", TObject::kOverwrite);
      }
      hist_inner->Write("", TObject::kOverwrite);
      hist_middle->Write("", TObject::kOverwrite);
      hist_outer->Write("", TObject::kOverwrite);
      hist_smiddle->Write("", TObject::kOverwrite);
      infile->Write();
      
    }//while
  }

}//namespace
