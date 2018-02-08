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
#include "TProfile.h"
#include "TProfile2D.h"


namespace dqutils {

  static const bool rno_debug = false;

  void
  MonitoringFile::SCTPostProcess( std::string inFilename, bool /* isIncremental */ )
  {
    if (rno_debug) std::cout << "Start SCT post-processing" <<std::endl;

    TH1::SetDefaultSumw2();
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

    // start postprocessing

    TIter next_run(infile->GetListOfKeys());
    TKey *key_run(0);
    key_run = (TKey*)next_run();
    TDirectory *dir0 = dynamic_cast<TDirectory*> (key_run->ReadObj());
    if(!dir0) return; // should never fail
    dir0->cd();

    TString run_dir;
    const int N_dir=4;
    const int N_BARREL=4, N_DISKS=9;
    TString subDetName[N_dir] = {"/GENERAL", "/SCTEC", "/SCTB", "/SCTEA"};

    int times = 1;
    while (times--) {  // just once
      run_dir = dir0->GetName();
      bool do_rno = true;
      bool do_no = true;
      bool do_conf = true;
      bool do_eff = true;

      // setup directories

      TString rno_dir[N_dir];
      TDirectory *rnodir[N_dir];
      for(int i=0;i<N_dir;i++){
	rno_dir[i] = run_dir + "/SCT" + subDetName[i] + "/RatioNoise/";
	rnodir[i] = (TDirectory*)infile->GetDirectory(rno_dir[i]);
	if(!rnodir[i]){
	  std::cerr << "--> SCTPostProcess: directory " << rno_dir[i] << " not found " <<std::endl;
	  do_rno = false;
	  //return;
	}
      }

      TString no_dir[N_dir];
      TDirectory *nodir[N_dir];
      for(int i=0;i<N_dir;i++){
	if(i==0)no_dir[i] = run_dir + "/SCT" + subDetName[i] + "/noise/";
	else no_dir[i] = run_dir + "/SCT" + subDetName[i] + "/Noise/";
	nodir[i] = (TDirectory*)infile->GetDirectory(no_dir[i]);
	if(!nodir[i]){
	  std::cerr << "--> SCTPostProcess: directory " << no_dir[i] << " not found " <<std::endl;
	  do_no = false;
	  //return;
	}
      }

      TString conf_dir[N_dir];
      TDirectory *confdir[N_dir];
      for(int i=0;i<N_dir;i++){
	conf_dir[i] = run_dir + "/SCT" + subDetName[i] + "/Conf/";
	confdir[i] = (TDirectory*)infile->GetDirectory(conf_dir[i]);
	if(!confdir[i]){
	  std::cerr << "--> SCTPostProcess: directory " << conf_dir[i] << " not found " <<std::endl;
	  do_conf = false;
	  //return;
	}
      }

      TString eff_dir[N_dir];
      TDirectory *effdir[N_dir];
      for(int i=0;i<N_dir;i++){
	eff_dir[i] = run_dir + "/SCT" + subDetName[i] + "/eff/";
	effdir[i] = (TDirectory*)infile->GetDirectory(eff_dir[i]);
	if(!effdir[i]){
	  std::cerr << "--> SCTPostProcess: directory " << eff_dir[i] << " not found " <<std::endl;
	  do_eff = false;
	  //return;
	}
      }

      int layers[3] = {N_DISKS, N_BARREL, N_DISKS};
      if (rno_debug) std::cout << "SCT post-processing: efficiency" <<std::endl;
      // efficiency
      if(do_eff){
	TString posi_eff[3] = {"m_eff_", "eff_", "p_eff_"};

	effdir[0]->cd();
	TH1F *h_EffDist = new TH1F("SctEffDistribution", "SCT Efficiency Distribution", 500, 0, 1);
	h_EffDist->GetXaxis()->SetTitle("Efficiency");
	h_EffDist->GetYaxis()->SetTitle("Links");

	TString mapName = "";
	TProfile2D *tp2dMap = 0;

	for(Long_t i=0;i<3;i++){
	  effdir[i+1]->cd();
	  for(Long_t j=0;j<layers[i];j++){
	    for(Long_t k=0;k<2;k++){
	      mapName = eff_dir[i+1] + posi_eff[i]+j+"_"+k;
	      tp2dMap = (TProfile2D*) infile->Get(mapName);
	      if(!tp2dMap){
		std::cerr << "--> SCTPostProcess: no such histogram: "<<mapName<<std::endl;
		continue;
	      }
	      for(int xbin=1;xbin<tp2dMap->GetNbinsX()+1;xbin++){
		for(int ybin=1;ybin<tp2dMap->GetNbinsY()+1;ybin++){
		  if(tp2dMap->GetBinEntries(tp2dMap->GetBin(xbin,ybin))==0) continue;
		  h_EffDist->Fill(tp2dMap->GetBinContent(xbin,ybin));
		}
	      }
	    }
	  }
	}
	effdir[0]->cd();
	h_EffDist->Write("", TObject::kOverwrite);
      }

      if (rno_debug) std::cout << "SCT post-processing: ratio noise" <<std::endl;
      // ratio noise
      if(do_rno){
	int nModuleEndCap1[9] = {0, 92, 224, 356, 488, 620, 752, 844, 936};
	int nModuleEndCap2[9] = {92, 224, 356, 488, 620, 752, 844, 936, 988};
	int nModuleBarrel1[4] = {0, 384, 864, 1440};
	int nModuleBarrel2[4] = {384, 864, 1440, 2112};

	rnodir[0]->cd();
	TH1F *hist_all = new TH1F("h_NO","Noise Occupancy for Barrel and Endcaps",500,0.,100.);
	TH1F *hist_bar = new TH1F("h_NOb","Noise Occupancy for Barrel",500,0.,100.);
	TH1F *hist_end = new TH1F("h_NOEC","Noise Occupancy for Endcaps",500,0.,100.);
	TH1F *hist_endc = new TH1F("hist_endcapC","endcap C",500,0.,100.);
	TH1F *hist_enda = new TH1F("hist_endcapA","endcap A",500,0.,100.);

	TH1F *hist_inner = new TH1F("h_NOEC_Inner","Noise Occupancy Inner Endcap Modules",500,0.,100.);
	TH1F *hist_middle = new TH1F("h_NOEC_Middle","Noise Occupancy Middle Endcap Modules",500,0.,100.);
	TH1F *hist_outer = new TH1F("h_NOEC_Outer","Noise Occupancy Outer Endcap Modules",500,0.,100.);
	TH1F *hist_smiddle = new TH1F("h_NOEC_ShortMiddle","Noise Occupancy Short Middle Endcap Modules",500,0.,100.);

	TH1F *hist_bar_layer[4];
	for(int i=0;i<4;i++){
	  rnodir[2]->cd();
	  hist_bar_layer[i] = new TH1F(Form("h_NOb_layer%d",i),Form("Noise Occupancy Barrel Layer %d",i),500,0.,100.);
	}
	TH1F *hist_endC_disk[9];
	for(int i=0;i<9;i++){
	  rnodir[1]->cd();
	  hist_endC_disk[i] = new TH1F(Form("h_NOECC_disk%d",i),Form("Noise Occupancy Endcap C disk %d",i),500,0.,100.);
	}
	TH1F *hist_endA_disk[9];
	for(int i=0;i<9;i++){
	  rnodir[3]->cd();
	  hist_endA_disk[i] = new TH1F(Form("h_NOECA_disk%d",i),Form("Noise Occupancy Endcap A disk %d",i),500,0.,100.);
	}

	rnodir[0]->cd();
	TString noSideHitHist = rno_dir[0] + "h_NZ1_vs_modnum";
	TString oneSideHitHist = rno_dir[0] + "h_N11_vs_modnum";
	TH1F *h_nosidehit = (TH1F*)infile->Get(noSideHitHist);
	TH1F *h_onesidehit = (TH1F*)infile->Get(oneSideHitHist);
	if(!h_nosidehit || !h_onesidehit){
	  std::cerr << "--> SCTPostProcess: no such histograms " << noSideHitHist << "and/or" << oneSideHitHist <<std::endl;
	}

	for(int i=0;i<4088;i++){
	  int nosidehit = h_nosidehit ? h_nosidehit->GetBinContent(i+1): -999;
	  int onesidehit = h_onesidehit ? h_onesidehit->GetBinContent(i+1): -999;
	  if(nosidehit>0 && onesidehit>-999){
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

	rnodir[0]->cd();
	hist_all->Write("", TObject::kOverwrite);
	hist_bar->Write("", TObject::kOverwrite);
	hist_end->Write("", TObject::kOverwrite);
	hist_inner->Write("", TObject::kOverwrite);
	hist_middle->Write("", TObject::kOverwrite);
	hist_outer->Write("", TObject::kOverwrite);
	hist_smiddle->Write("", TObject::kOverwrite);
	for(int i=0;i<9;i++){
	  rnodir[1]->cd();
	  hist_endC_disk[i]->Write("", TObject::kOverwrite);
	}
	for(int i=0;i<4;i++){
	  rnodir[2]->cd();
	  hist_bar_layer[i]->Write("", TObject::kOverwrite);
	}
	for(int i=0;i<9;i++){
	  rnodir[3]->cd();
	  hist_endA_disk[i]->Write("", TObject::kOverwrite);
	}
      }

      if (rno_debug) std::cout << "SCT post-processing: noise occupancy" <<std::endl;
      // noise occupancy
      if(do_no){
	TString posi_notrg[3] = {"/noiseoccupancymaptriggerECm_", "/noiseoccupancymaptrigger_", "/noiseoccupancymaptriggerECp_"};
	TString posi_no[3] = {"/noiseoccupancymapECm_", "/noiseoccupancymap_", "/noiseoccupancymapECp_"};
	TString posi_hotrg[3] = {"/hitoccupancymaptriggerECm_", "/hitoccupancymaptrigger_", "/hitoccupancymaptriggerECp_"};
	TString posi_ho[3] = {"/hitoccupancymapECm_", "/hitoccupancymap_", "/hitoccupancymapECp_"};

	TString tmpName = no_dir[0]+"SCTNOdistributionTrigger";
	TString NO_title = "";
	nodir[0]->cd();
	TH1F* h_SCTNOTrigger = 0;
	if(!infile->Get(tmpName)){
	  NO_title = "NO Distribution for the SCT for EMPTY trigger";
	}
	else{
	  NO_title = ((TH1F*)infile->Get(tmpName))->GetTitle();
	}
	h_SCTNOTrigger = new TH1F("SCTNOdistributionTrigger", NO_title, 8000,1e-1 ,20000);
	h_SCTNOTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
	h_SCTNOTrigger->GetYaxis()->SetTitle("Modules");

	TH1F* h_SCTNO = new TH1F("SCTNOdistribution", "NO Distribution for the SCT", 8000,1e-1 ,20000);
	h_SCTNO->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
	h_SCTNO->GetYaxis()->SetTitle("Modules");

	NO_title.ReplaceAll("SCT", "Barrel");
	TH1F* h_barrelNOTrigger = new TH1F("barrelNOdistributionTrigger", NO_title, 8000,1e-1 ,20000);
	h_barrelNOTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
	h_barrelNOTrigger->GetYaxis()->SetTitle("Modules");
	TH1F* h_barrelNO = new TH1F("barrelNOdistribution", "NO Distribution for the Barrel", 8000,1e-1 ,20000);
	h_barrelNO->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
	h_barrelNO->GetYaxis()->SetTitle("Modules");

	NO_title.ReplaceAll("Barrel", "EndCap A");
	TH1F* h_ECANOTrigger = new TH1F("ECANOdistributionTrigger", NO_title, 8000,1e-1 ,20000);
	h_ECANOTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
	h_ECANOTrigger->GetYaxis()->SetTitle("Modules");
	TH1F* h_ECANO = new TH1F("ECANOdistribution", "NO Distribution for the EndCap A", 8000,1e-1 ,20000);
	h_ECANO->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
	h_ECANO->GetYaxis()->SetTitle("Modules");

	NO_title.ReplaceAll("EndCap A", "EndCap C");
	TH1F* h_ECCNOTrigger = new TH1F("ECCNOdistributionTrigger", NO_title, 8000,1e-1 ,20000);
	h_ECCNOTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
	h_ECCNOTrigger->GetYaxis()->SetTitle("Modules");
	TH1F* h_ECCNO = new TH1F("ECCNOdistribution", "NO Distribution for the EndCap C", 8000,1e-1 ,20000);
	h_ECCNO->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
	h_ECCNO->GetYaxis()->SetTitle("Modules");

	tmpName = no_dir[0]+"SCTHOdistributionTrigger";
	TString HO_title = "";
	nodir[0]->cd();
	TH1F* h_SCTHOTrigger = 0;
	if(!infile->Get(tmpName)){
	  HO_title = "HO Distribution for the SCT for EMPTY trigger";
	}
	else{
	  HO_title = ((TH1F*)infile->Get(tmpName))->GetTitle();
	}
	h_SCTHOTrigger = new TH1F("SCTHOdistributionTrigger", HO_title, 8000,1e-1 ,20000);
	h_SCTHOTrigger->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
	h_SCTHOTrigger->GetYaxis()->SetTitle("Modules");

	TH1F* h_SCTHO = new TH1F("SCTHOdistribution", "HO Distribution for the SCT", 8000,1e-1 ,20000);
	h_SCTHO->GetXaxis()->SetTitle("Noise Occupancy [10^{-5}]");
	h_SCTHO->GetYaxis()->SetTitle("Modules");

	HO_title.ReplaceAll("SCT", "Barrel");
	TH1F* h_barrelHOTrigger = new TH1F("barrelHOdistributionTrigger", HO_title, 8000,1e-1 ,20000);
	h_barrelHOTrigger->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
	h_barrelHOTrigger->GetYaxis()->SetTitle("Modules");
	TH1F* h_barrelHO = new TH1F("barrelHOdistribution", "HO Distribution for the Barrel", 8000,1e-1 ,20000);
	h_barrelHO->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
	h_barrelHO->GetYaxis()->SetTitle("Modules");

	HO_title.ReplaceAll("Barrel", "EndCap A");
	TH1F* h_ECAHOTrigger = new TH1F("ECAHOdistributionTrigger", HO_title, 8000,1e-1 ,20000);
	h_ECAHOTrigger->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
	h_ECAHOTrigger->GetYaxis()->SetTitle("Modules");
	TH1F* h_ECAHO = new TH1F("ECAHOdistribution", "HO Distribution for the EndCap A", 8000,1e-1 ,20000);
	h_ECAHO->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
	h_ECAHO->GetYaxis()->SetTitle("Modules");

	HO_title.ReplaceAll("EndCap A", "EndCap C");
	TH1F* h_ECCHOTrigger = new TH1F("ECCHOdistributionTrigger", HO_title, 8000,1e-1 ,20000);
	h_ECCHOTrigger->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
	h_ECCHOTrigger->GetYaxis()->SetTitle("Modules");
	TH1F* h_ECCHO = new TH1F("ECCHOdistribution", "HO Distribution for the EndCap C", 8000,1e-1 ,20000);
	h_ECCHO->GetXaxis()->SetTitle("Hit Occupancy [10^{-5}]");
	h_ECCHO->GetYaxis()->SetTitle("Modules");

	TString mapName = "";
	TProfile2D *tp2dMap = 0;

	for(Long_t i=0;i<3;i++){
	  nodir[i+1]->cd();
	  for(Long_t j=0;j<layers[i];j++){
	    for(Long_t k=0;k<2;k++){
	      mapName = no_dir[i+1] + posi_no[i]+j+"_"+k;
	      tp2dMap = (TProfile2D*) infile->Get(mapName);
	      if(!tp2dMap){
		std::cerr << "--> SCTPostProcess: no such histogram: "<<mapName<<std::endl;
		continue;
	      }
	      for(int xbin=1;xbin<tp2dMap->GetNbinsX()+1;xbin++){
		for(int ybin=1;ybin<tp2dMap->GetNbinsY()+1;ybin++){
		  if(tp2dMap->GetBinEntries(tp2dMap->GetBin(xbin,ybin))==0) continue;
		  h_SCTNO->Fill(tp2dMap->GetBinContent(xbin,ybin));
		  if(i==0) h_ECCNO->Fill(tp2dMap->GetBinContent(xbin,ybin));
		  if(i==1) h_barrelNO->Fill(tp2dMap->GetBinContent(xbin,ybin));
		  if(i==2) h_ECANO->Fill(tp2dMap->GetBinContent(xbin,ybin));
		}
	      }
	    }
	    for(Long_t k=0;k<2;k++){
	      mapName = no_dir[i+1] + posi_notrg[i]+j+"_"+k;
	      tp2dMap = (TProfile2D*) infile->Get(mapName);
	      if(!tp2dMap){
		std::cerr << "--> SCTPostProcess: no such histogram: "<<mapName<<std::endl;
		continue;
	      }
	      for(int xbin=1;xbin<tp2dMap->GetNbinsX()+1;xbin++){
		for(int ybin=1;ybin<tp2dMap->GetNbinsY()+1;ybin++){
		  if(tp2dMap->GetBinEntries(tp2dMap->GetBin(xbin,ybin))==0) continue;
		  h_SCTNOTrigger->Fill(tp2dMap->GetBinContent(xbin,ybin));
		  if(i==0) h_ECCNOTrigger->Fill(tp2dMap->GetBinContent(xbin,ybin));
		  if(i==1) h_barrelNOTrigger->Fill(tp2dMap->GetBinContent(xbin,ybin));
		  if(i==2) h_ECANOTrigger->Fill(tp2dMap->GetBinContent(xbin,ybin));
		}
	      }
	    }
	    for(Long_t k=0;k<2;k++){
	      mapName = no_dir[i+1] + posi_ho[i]+j+"_"+k;
	      tp2dMap = (TProfile2D*) infile->Get(mapName);
	      if(!tp2dMap){
		std::cerr << "--> SCTPostProcess: no such histogram: "<<mapName<<std::endl;
		continue;
	      }
	      for(int xbin=1;xbin<tp2dMap->GetNbinsX()+1;xbin++){
		for(int ybin=1;ybin<tp2dMap->GetNbinsY()+1;ybin++){
		  if(tp2dMap->GetBinEntries(tp2dMap->GetBin(xbin,ybin))==0) continue;
		  h_SCTHO->Fill(tp2dMap->GetBinContent(xbin,ybin));
		  if(i==0) h_ECCHO->Fill(tp2dMap->GetBinContent(xbin,ybin));
		  if(i==1) h_barrelHO->Fill(tp2dMap->GetBinContent(xbin,ybin));
		  if(i==2) h_ECAHO->Fill(tp2dMap->GetBinContent(xbin,ybin));
		}
	      }
	    }
	    for(Long_t k=0;k<2;k++){
	      mapName = no_dir[i+1] + posi_hotrg[i]+j+"_"+k;
	      tp2dMap = (TProfile2D*) infile->Get(mapName);
	      if(!tp2dMap){
		std::cerr << "--> SCTPostProcess: no such histogram: "<<mapName<<std::endl;
		continue;
	      }
	      for(int xbin=1;xbin<tp2dMap->GetNbinsX()+1;xbin++){
		for(int ybin=1;ybin<tp2dMap->GetNbinsY()+1;ybin++){
		  if(tp2dMap->GetBinEntries(tp2dMap->GetBin(xbin,ybin))==0) continue;
		  h_SCTHOTrigger->Fill(tp2dMap->GetBinContent(xbin,ybin));
		  if(i==0) h_ECCHOTrigger->Fill(tp2dMap->GetBinContent(xbin,ybin));
		  if(i==1) h_barrelHOTrigger->Fill(tp2dMap->GetBinContent(xbin,ybin));
		  if(i==2) h_ECAHOTrigger->Fill(tp2dMap->GetBinContent(xbin,ybin));
		}
	      }
	    }
	  }
	}

	nodir[0]->cd();

	h_SCTNOTrigger->Write("", TObject::kOverwrite);
	h_SCTNO->Write("", TObject::kOverwrite);
	h_barrelNOTrigger->Write("", TObject::kOverwrite);
	h_barrelNO->Write("", TObject::kOverwrite);
	h_ECCNOTrigger->Write("", TObject::kOverwrite);
	h_ECCNO->Write("", TObject::kOverwrite);
	h_ECANOTrigger->Write("", TObject::kOverwrite);
	h_ECANO->Write("", TObject::kOverwrite);

	h_SCTHOTrigger->Write("", TObject::kOverwrite);
	h_SCTHO->Write("", TObject::kOverwrite);
	h_barrelHOTrigger->Write("", TObject::kOverwrite);
	h_barrelHO->Write("", TObject::kOverwrite);
	h_ECCHOTrigger->Write("", TObject::kOverwrite);
	h_ECCHO->Write("", TObject::kOverwrite);
	h_ECAHOTrigger->Write("", TObject::kOverwrite);
	h_ECAHO->Write("", TObject::kOverwrite);
      }

      if (rno_debug) std::cout << "SCT post-processing: Configuration" <<std::endl;
      // Configuration
      if(do_conf){
	const int N_hist = 3;
	double Threshold[N_hist] = {0.7,0.1,150};
	TString name_hist[N_hist][N_dir] = {
	  {"","/errors/Errors/SCT_NumberOfErrorsEC_", "/errors/Errors/SCT_NumberOfErrorsB_", "/errors/Errors/SCT_NumberOfErrorsEA_"},
	  {"","/eff/ineffm_", "/eff/ineff_", "/eff/ineffp_"},
	  {"","/Noise/noiseoccupancymaptriggerECm_", "/Noise/noiseoccupancymaptrigger_", "/Noise/noiseoccupancymaptriggerECp_"}
	};
	int countM[N_hist][N_dir];
	for (int ibc = 0; ibc != N_dir; ++ibc) { // region
	  for(int ih = 0; ih != N_hist; ++ih) { // hist
	    countM[ih][ibc]=0;
	  }
	}

	TH2F* hist0 = 0;
	TH2F* hist1 = 0;
	TString mapName = "";

	confdir[0]->cd();
	for (int ibc = 1; ibc != N_dir; ++ibc) { // region(B=1,ECA=2,ECC=3)
	  for (int lyr = 0; lyr != layers[ibc-1]; ++lyr) { // layer
	    for (int ih = 0; ih!= N_hist; ++ih) { // hist
	      hist0 = 0; hist1 = 0;
	      mapName =  run_dir + "/SCT" + subDetName[ibc] + name_hist[ih][ibc] + TString::Itoa(lyr,10) + "_0";
	      hist0 = (TH2F*)infile->Get(mapName);
	      mapName =  run_dir + "/SCT" + subDetName[ibc] + name_hist[ih][ibc] + TString::Itoa(lyr,10) + "_1";
	      hist1 = (TH2F*)infile->Get(mapName);
	      if(!hist0 || !hist1){
		std::cerr << "--> SCTPostProcess: no such histogram: " << mapName + " or _0" <<std::endl;
		continue;
	      }
	      int xbins = hist0->GetNbinsX() + 1;
	      int ybins = hist0->GetNbinsY() + 1;
	      for (int xb = 1; xb != xbins; ++xb) {
		for (int yb = 1; yb != ybins; ++yb) {
		  if (hist0->GetBinContent(xb,yb) > Threshold[ih] || hist1->GetBinContent(xb,yb) > Threshold[ih]){
		    countM[ih][0]++;
		    countM[ih][ibc]++;
		  }
		}
	      }
	    }
	  }
	}

	dir0->cd();
	TString conftitleNew[N_dir] = {
	  "SCTConf", "SCTConfEndcapC", "SCTConfBarrel", "SCTConfEndcapA"
	};
	TString conflabel[N_dir] = {
	  "Num of Problematic Module in All Region", "Num of Problematic Module in EndcapC",
	  "Num of Problematic Module in Barrel", "Num of Problematic Module in EndcapA"
	};
	TH1F *ConfNew[N_dir];
	for(int i=0;i<N_dir;i++){
	  TH1F* h_tmp = (TH1F*)infile->Get(conf_dir[i] + conftitleNew[i]);
	  confdir[i]->cd();
	  ConfNew[i] = new TH1F(conftitleNew[i]+"New", conflabel[i], 5, -0.5, 4.5);
	  ConfNew[i]->GetXaxis()->SetBinLabel(1,"Flagged Links");
	  if(h_tmp) ConfNew[i]->SetBinContent(1,h_tmp->GetBinContent(2));
	  if(h_tmp) ConfNew[i]->SetBinError(1,TMath::Sqrt(h_tmp->GetBinContent(2)));
	  ConfNew[i]->GetXaxis()->SetBinLabel(2,"Masked Links");
	  if(h_tmp) ConfNew[i]->SetBinContent(2,h_tmp->GetBinContent(3));
	  if(h_tmp) ConfNew[i]->SetBinError(2,TMath::Sqrt(h_tmp->GetBinContent(3)));
	  ConfNew[i]->GetXaxis()->SetBinLabel(3,"Errors");
	  ConfNew[i]->SetBinContent(3,countM[0][i]);
	  ConfNew[i]->SetBinError(3,TMath::Sqrt(countM[0][i]));
	  ConfNew[i]->GetXaxis()->SetBinLabel(4,"Inefficient");
	  ConfNew[i]->SetBinContent(4,countM[1][i]);
	  ConfNew[i]->SetBinError(4,TMath::Sqrt(countM[1][i]));
	  ConfNew[i]->GetXaxis()->SetBinLabel(5,"Noisy");
	  ConfNew[i]->SetBinContent(5,countM[2][i]);
	  ConfNew[i]->SetBinError(5,TMath::Sqrt(countM[2][i]));
	  ConfNew[i]->Write("", TObject::kOverwrite);
	}
      }

      if (rno_debug) std::cout<<"Writing Histograms for SCT..."<<std::endl;
      infile->Write();

    }//while
    //infile->Close();
  }

}//namespace
