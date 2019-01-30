/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
//Methods to Divide HLTMuon Histograms to calculate efficiency and occupancy after merging.
//Author: Akimasa Ishikawa (akimasa.ishikawa@cern.ch)
//Date:   Aug 2009
////////////////////////////////////////////////////////////////////////////////////////////////////

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
#include "TGraph.h"
#include "TString.h"
#include "TGraphAsymmErrors.h"


namespace dqutils {

  //   static const bool fdbg = false;
  static const bool fdbg = true;
  bool HI_pp_key=false;//true::HI false::pp

  void MonitoringFile::HLTMuonHistogramDivision(std::string inFilename, TString& run_dir)
  {

    if (fdbg) std::cout << "  Start to Divide HLTMuon Histograms for Efficiency and Rate Ratio" << std::endl;
    PostProcessorFileWrapper mf( inFilename , "HLT Histogram Division");
    if (!mf.IsOpen()) {
      std::cerr << "HLTMuonPostProcess(): "
	<< "Input file not opened \n";
      return;
    }
    if(mf.GetSize()<1000.) {
      std::cerr << "HLTMuonPostProcess(): "
	<< "Input file empty \n";
      return; 
    }
    // get run directory name
    //Seemingly unnecessary lines are necessary
    TIter nextcd0(gDirectory->GetListOfKeys());
    TKey *key0 = (TKey*)nextcd0();
    if (key0 == 0) return;
    TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
    if (dir0 == 0) return;
    dir0->cd();
    ///

    // 110728: removing the iteration of searching run directory according to the new MDT code
    // 150621: reenable ...
    TIter next_run (mf.GetListOfKeys());
    TKey* key_run(0);
    while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) { //== the while commented out at the end
      if (!key_run->IsFolder()) continue;
      run_dir = key_run->GetName();
      if (!run_dir.Contains("run") )  { continue;}
      break;
    }
    if (! run_dir.Contains("run") ) {
      std::cerr << "HLTMuon: unable to find run directory ..." << std::endl;
      return;
    }
    {
      //run_dir = dir0->GetName();
      if (fdbg) {
	std::cout << "HLTMuon: run directory is " << run_dir << std::endl;
      }
      std::string run_dir2 = run_dir.Data();
      //int run_number = atoi( (run_dir2.substr(4, run_dir2.size()-4 )).c_str() );
      //run_number=run_number;

      //===HLTMuon

      TH1F* hHI_PP_Flag(0);
      TString hi_pp_flag = run_dir + "/HLT/MuonMon/Common/HI_PP_Flag";
      mf.get(hi_pp_flag, hHI_PP_Flag);
      if (!hHI_PP_Flag) {
	std::cerr<<"HLTMuon:unable to find hist : HI_PP_Flag"<<std::endl;	
      }else{
	//std::cout<<"HI_PP_Flag :found "<<std::endl;
	//std::cout<<"bin content "<<hHI_PP_Flag->GetBinContent(0)<<"/"<<hHI_PP_Flag->GetBinContent(1)<<"/"<<hHI_PP_Flag->GetBinContent(2)<<std::endl;
	if(hHI_PP_Flag->GetBinContent(1) > 0){
	  HI_pp_key = true;
	}else{
	  HI_pp_key = false;
	}
      }

      TString muon_dir = run_dir + "/HLT/MuonMon/";

      TString cm_dir = muon_dir + "Common/";
      TString mf_dir = muon_dir + "L2MuonSA/";
      TString mc_dir = muon_dir + "muComb/";
      TString mi_dir = muon_dir + "muIso/";
      TString tm_dir = muon_dir + "TileMu/";
      TString ef_dir = muon_dir + "MuonEF/";
      //TString mg_dir = muon_dir + "MuGirl/";  // removed, Yuan 
      // YY added
      TString ztp_dir = muon_dir + "MuZTP/";

      TString eff_dir  = muon_dir + "Efficiency/";
      TString nd_dir  = muon_dir + "Efficiency/NumDenom/";
      TString rate_dir = muon_dir + "Rate/";
      TString rr_dir   = muon_dir + "Rate/Ratio/";

      TString seff;
      TString seffg; // YY added
      TString snum;
      TString sden;
      TString stmp;
      TString stmpg;

      TH1F* h1tmp(0);
      TH1F* h1eff(0);
      TH1F* h1num(0);
      TH1F* h1den(0);
      TH1F* h1sumeff(0); // new YY
      TH1F *h1effsum(nullptr);
      TGraphAsymmErrors* h1tmpg;

      //==Efficiency
      //  L2MuonSA efficiency
      TDirectory* dir = mf.GetDirectory(eff_dir);
      if(!dir){
	std::cerr<< "HLTMuonHistogramDivision: directory "<<eff_dir<<" not found"<<std::endl;
	return;
      }

      std::vector<TString> effnames;
      effnames.push_back("L2MuonSA_effi_toRecMuonCB_pt");
      effnames.push_back("L2MuonSA_effi_toRecMuonCB_pt_barrel");
      effnames.push_back("L2MuonSA_effi_toRecMuonCB_pt_endcap");
      effnames.push_back("L2MuonSA_effi_toRecMuonCB_eta");
      effnames.push_back("L2MuonSA_effi_toRecMuonCB_phi");

      for( std::vector<TString>::iterator it = effnames.begin(); it != effnames.end(); it++ ){
	seff = eff_dir + (*it);
	snum = mf_dir + (*it) + "_numer";
	sden = mf_dir + (*it) + "_denom";
	stmp = (*it);

	h1num = 0;
	mf.get(snum, h1num);
	if (!h1num) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	  }
	  continue;
	}
	h1den = 0;
	mf.get(sden, h1den);
	if (!h1den) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	  }
	  continue;
	}

	if(h1num && h1den){
	  h1tmp = (TH1F*)h1den->Clone();
	  h1tmp->SetName(stmp);
	  std::string stcar = h1tmp->GetTitle();
	  stcar.replace(stcar.end()-5,stcar.end(),"");
	  h1tmp->SetTitle(stcar.c_str());
	  h1tmp->GetYaxis()->SetTitle("Efficiency");
	  h1tmp->Reset();
	  h1tmp->Divide(h1num, h1den, 1., 1., "B");
	  dir->cd();
	  h1tmp->Write();                                                           	
	}

      }//effnames
      mf.Write();

      //  muComb efficiency
      effnames.clear();
      effnames.push_back("muComb_effi_toOffl_pt");
      effnames.push_back("muComb_effi_toOffl_pt_barrel");
      effnames.push_back("muComb_effi_toOffl_pt_endcap");
      effnames.push_back("muComb_effi_toOffl_eta");
      effnames.push_back("muComb_effi_toOffl_phi");

      for( std::vector<TString>::iterator it = effnames.begin(); it != effnames.end(); it++ ){
	seff = eff_dir + (*it);
	snum = mc_dir + (*it) + "_numer";
	sden = mc_dir + (*it) + "_denom";
	stmp = (*it);

	h1num = 0;
	mf.get(snum, h1num);
	if (!h1num) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	  }
	  continue;
	}
	h1den = 0;
	mf.get(sden, h1den);
	if (!h1den) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	  }
	  continue;
	}

	if(h1num && h1den){
	  h1tmp = (TH1F*)h1den->Clone();
	  h1tmp->SetName(stmp);
	  std::string stcar = h1tmp->GetTitle();
	  stcar.replace(stcar.end()-5,stcar.end(),"");
	  h1tmp->SetTitle(stcar.c_str());
	  h1tmp->GetYaxis()->SetTitle("Efficiency");
	  h1tmp->Reset();
	  h1tmp->Divide(h1num, h1den, 1., 1., "B");
	  dir->cd();
	  h1tmp->Write();
	}

      }//effnames
      mf.Write();

      //  muIso efficiency
      effnames.clear();
      effnames.push_back("muIso_effi_toOffl_pt");

      for( std::vector<TString>::iterator it = effnames.begin(); it != effnames.end(); it++ ){
	seff = eff_dir + (*it);
	snum = mi_dir + (*it) + "_numer";
	sden = mi_dir + (*it) + "_denom";
	stmp = (*it);

	h1num = 0;
	mf.get(snum, h1num);
	if (!h1num) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	  }
	  continue;
	}
	h1den = 0;
	mf.get(sden, h1den);
	if (!h1den) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	  }
	  continue;
	}

	if(h1num && h1den){
	  h1tmp = (TH1F*)h1den->Clone();
	  h1tmp->SetName(stmp);
	  std::string stcar = h1tmp->GetTitle();
	  stcar.replace(stcar.end()-5,stcar.end(),"");
	  h1tmp->SetTitle(stcar.c_str());
	  h1tmp->GetYaxis()->SetTitle("Efficiency");
	  h1tmp->Reset();
	  h1tmp->Divide(h1num, h1den, 1., 1., "B");
	  dir->cd();
	  h1tmp->Write();
	}
      }//effnames
      mf.Write();

      //  TileMu efficiency
      effnames.clear();
      std::vector<TString> numnames;
      std::vector<TString> dennames;
      effnames.push_back  ("TileMu_RecCBMuon_EffEta");
      numnames.push_back  ("Rec_Eta_Num");
      dennames.push_back("Rec_Eta");
      effnames.push_back  ("TileMu_RecCBMuon_EffPhi");
      numnames.push_back  ("Rec_Phi_Num");
      dennames.push_back("Rec_Phi");
      effnames.push_back  ("TileTrackMu_RecCBMuon_EffEta");
      numnames.push_back  ("TileTrackMu_Eta");
      dennames.push_back("Rec_Eta");
      effnames.push_back  ("TileTrackMu_RecCBMuon_EffPhi");
      numnames.push_back  ("TileTrackMu_Phi");
      dennames.push_back("Rec_Phi");
      effnames.push_back  ("TileTrackMu_RecCBMuon_EffPt");
      numnames.push_back  ("TileTrackMu_Pt");
      dennames.push_back("Rec_Pt");

      for( unsigned int i=0; i < effnames.size() ; i++ ){
	seff   = eff_dir + effnames.at(i);
	snum   = tm_dir + numnames.at(i);
	sden   = tm_dir + dennames.at(i);
	stmp = effnames.at(i);

	h1num = 0;
	mf.get(snum, h1num);
	if (!h1num) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	  }
	  continue;
	}
	h1den = 0;
	mf.get(sden, h1den);
	if (!h1den) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	  }
	  continue;
	}

	if( h1num && h1den){
	  h1tmp = (TH1F*)h1den->Clone();
	  h1tmp->SetName(stmp);
	  std::istringstream iss(stmp.Data());
	  std::string token;
	  int ili=0;
	  while (std::getline(iss, token, '_'))
	  {
	    stmp = token + "Feature wrt Offline)";
	    ili++;
	    if(ili>0)break;
	  }

	  std::string stcar = h1tmp->GetTitle();
	  if(stcar.find("Pt") != std::string::npos){
	    stmp = "Efficiency on p_{T} (" + stmp;
	    h1tmp->GetXaxis()->SetTitle("p_{T}");
	  } 
	  if(stcar.find("Eta") != std::string::npos)stmp = "Efficiency on #eta (" + stmp;
	  if(stcar.find("Phi") != std::string::npos)stmp = "Efficiency on #phi (" + stmp;
	  h1tmp->SetTitle(stmp);
	  h1tmp->GetYaxis()->SetTitle("Efficiency");
	  h1tmp->Reset();
	  h1tmp->Divide(h1num, h1den, 1., 1., "B");
	  dir->cd();
	  h1tmp->Write();
	}
      }//effnames
      mf.Write();

      //  MuonEF efficiency
      effnames.clear();
      numnames.clear();
      dennames.clear();
      effnames.push_back("EFMS_effi_toOffl_pt");
      effnames.push_back("EFMS_effi_toOffl_eta");
      effnames.push_back("EFMS_effi_toOffl_phi");
      effnames.push_back("EFSA_effi_toOffl_pt");
      effnames.push_back("EFCB_effi_toOffl_pt");

      for( std::vector<TString>::iterator it = effnames.begin(); it != effnames.end(); it++ ){
	seff = eff_dir + (*it);
	snum = ef_dir + (*it) + "_numer";
	sden = ef_dir + (*it) + "_denom";
	stmp = (*it);

	h1num = 0;
	mf.get(snum, h1num);
	if (!h1num) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	  }
	  continue;
	}
	h1den = 0;
	mf.get(sden, h1den);
	if (!h1den) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	  }
	  continue;
	}

	if(h1num && h1den){
	  h1tmp = (TH1F*)h1den->Clone();
	  h1tmp->SetName(stmp);
	  std::string stcar = h1tmp->GetTitle();
	  stcar.replace(stcar.end()-5,stcar.end(),"");
	  h1tmp->SetTitle(stcar.c_str());
	  h1tmp->GetYaxis()->SetTitle("Efficiency");
	  h1tmp->Reset();
	  h1tmp->Divide(h1num, h1den, 1., 1., "B");                                 	
	  dir->cd();                                                               	
	  h1tmp->Write();                                                           	
	}
      }//effnames
      mf.Write();

      //==Turn on
      std::vector<TString> chainsMSonly;  
      std::vector<TString> chainsStandard;
      std::vector<TString> chainsMG;
      std::vector<TString> chainsMI;
      std::vector<TString> chainsGeneric;
      std::vector<TString> chainsEFiso;
      std::vector<TString> chainsEFFS;

      // Generic (EFsuper etc.)
      // chainsGeneric.push_back("mu36_tight");             // v4 primary
      //chainsGeneric.push_back("mu4_cosmic_L1MU4_EMPTY");   // LS1
      //chainsGeneric.push_back("mu4_cosmic_L1MU11_EMPTY");  // LS1
      chainsGeneric.push_back("muChain1");   // MAM 
      chainsGeneric.push_back("muChain2");  // MAM

      // Generic (Isolated muons)
      // chainsEFiso.push_back("mu24i_tight")  ;            // v4 primary
      chainsEFiso.push_back("muChainEFiso1")  ;            // MAM 
      chainsEFiso.push_back("muChainEFiso2")  ;            // MAM 

      // MSonly
      // chainsMSonly.push_back("mu50_MSonly_barrel_tight");    // v4 primary
      //chainsMSonly.push_back("mu4_msonly_cosmic_L1MU11_EMPTY");    // LS1
      chainsMSonly.push_back("muChainMSonly1");    // MAM 
      chainsMSonly.push_back("muChainMSonly2");    // MAM 

      // EFFS triggers (L. Yuan)
      // chainsEFFS.push_back("mu18_tight_mu8_EFFS");
      chainsEFFS.push_back("muChainEFFS");  // MAM

      enum indexINDEP { INDORTH, INDEGAMMA, INDMET, INDJET, INDTAU, INDMBIAS };
      TString trigger[INDMBIAS + 1];

      trigger[INDORTH] = "Orthog";//EGamma + Tau + Jet + MET
      trigger[INDEGAMMA] = "EGamma";
      trigger[INDMET] = "MET";
      trigger[INDJET] = "Jet";

      int maxindep = 0; // YY 20.01.2012

      // YY added:
      TString ESchainName = "_ES";

      TString bestr[2] = {"_Barrel", "_Endcap"};

      // made it as enum: 20.1.2012
      std::string triggerES[7] = {"_ESstd", "_EStag", "_ESid", "_ESindep", "_ESHIL1", "_ESHIid", "_ESHIindep"};
      enum indexES { ESSTD, ESTAG, ESID, ESINDEP, ESHIL1, ESHIID, ESHIINDEP };

      int maxESbr = ESINDEP;

      bool CB_mon_ESbr[ESHIINDEP + 1];
      bool MS_mon_ESbr[ESHIINDEP + 1];

      CB_mon_ESbr[ESSTD] = 0;
      CB_mon_ESbr[ESTAG] = 0;
      CB_mon_ESbr[ESID] = 1;
      CB_mon_ESbr[ESINDEP] = 0;
      CB_mon_ESbr[ESHIL1] = 0;
      CB_mon_ESbr[ESHIID] = 0;
      CB_mon_ESbr[ESHIINDEP] = 0;

      MS_mon_ESbr[ESSTD] = 0;
      MS_mon_ESbr[ESTAG] = 1;
      MS_mon_ESbr[ESID] = 0;
      MS_mon_ESbr[ESINDEP] = 0;
      MS_mon_ESbr[ESHIL1] = 0;
      MS_mon_ESbr[ESHIID] = 0;
      MS_mon_ESbr[ESHIINDEP] = 0;

      std::vector<std::string> vectkwd;

      vectkwd.push_back(triggerES[ESTAG]);
      vectkwd.push_back(triggerES[ESID]);
      vectkwd.push_back("_Jet");
      vectkwd.push_back("_all");

      TString hptName = "_hpt";
      TString MSchainName = "_MSb";

      // YY: pt range.
      int iSTDL;
      int iSTDH;
      if(HI_pp_key){
	iSTDL = 45;  // 12 GeV
	//iSTDL = 54;  // 15 GeV
	iSTDH = 75; // 25 GeV
      }else{
	iSTDL = 105;  // 60 GeV
	//iSTDL = 91;  // 40 GeV
	iSTDH = 120; // 100 GeV
	//iSTDH = 120; // 100 GeV
      }
      int iMSL = 105;  // 60 GeV
      int iMSH = 120;  // 100 GeV

      if(HI_pp_key){
	iMSL=54;//15GeV
	iMSH=75;//25GeV
      }
      // YY added:
      enum ieffAlgo {
	iL2MuonSA = 0,   // StdAlgo
	iMuComb = 1,   // StdAlgo
	iEFCB   = 2,   // StdAlgo
	iMuGirl = 3,   // StdAlgo

	iEFSA   = 1    // MSAlgo
      };

      //  Standard Chains
      //TString alg[5] = {"_L2MuonSA", "_MuComb", "_MuonEFMS", "_MuonEFSA", "_MuonEFCB"};
      //TString wrtalg[5] = {"_L1", "_L2MuonSA", "_MuComb", "_MuComb", "_MuComb"};

      // ******************************************************//
      // start the code add by Yuan //
      //TString FS_pre_trigger = "mu18it_tight";
      TString FS_pre_trigger = "EFFSpre";
      for(unsigned int i=0; i<chainsEFFS.size(); i++){
	TString chainName = chainsEFFS.at(i);

	TString hists_str[9] = {chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB",
	  chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB_Barrel",
	  chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB_Endcap",
	  chainName + "_tagEFFSpre_mu0_15" + "_Turn_On_Curve_wrt_probe_MuidCB",
	  chainName + "_tagEFFSpre_mu15_20" + "_Turn_On_Curve_wrt_probe_MuidCB",
	  chainName + "_tagEFFSpre_mu20" + "_Turn_On_Curve_wrt_probe_MuidCB",
	  chainName + "_Turn_On_Curve_wrt_subleading_MuidCB",
	  FS_pre_trigger + "_dimuonTP" + "_Turn_On_Curve_wrt_probe_MuidCB",
	  FS_pre_trigger + "_dimuonTP" + "_Turn_On_Curve_wrt_L1_probe_MuidCB",
	};

	bool for_mydebug = false;
	for(int iROI = 0; iROI < 9; iROI++){
	  sden = nd_dir + hists_str[iROI] + "_Denominator";
	  snum = nd_dir + hists_str[iROI] + "_Numerator";
	  seff = eff_dir + hists_str[iROI];
	  seffg = seff + "_Fit";

	  stmp  = hists_str[iROI];
	  stmpg = hists_str[iROI] + "_Fit";
	  h1num = 0;
	  mf.get(snum, h1num);
	  if (!h1num) {
	    if (for_mydebug) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	    }
	    continue;
	  }
	  h1den = 0;
	  mf.get(sden, h1den);
	  if (!h1den) {
	    if (for_mydebug) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	    }
	    continue;
	  }

	  if(h1num && h1den){
	    h1tmp = (TH1F*)h1den->Clone();
	    h1tmp->SetName(stmp);                          						
	    h1tmp->SetTitle(stmp);                         						  
	    h1tmp->GetYaxis()->SetTitle("Efficiency");     						  
	    h1tmp->Reset();                                						  
	    h1tmp->Divide(h1num, h1den, 1., 1., "B");      						  
	    dir->cd();
	    h1tmp->Write(); 
	    h1tmpg = new TGraphAsymmErrors();
	    h1tmpg->SetMarkerStyle(20);
	    h1tmpg->SetMinimum(0.0);
	    h1tmpg->SetMaximum(1.05);
	    h1tmpg->BayesDivide(h1num, h1den);
	    h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
	    h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
	    h1tmpg->SetName(stmpg);
	    h1tmpg->Write();
	    delete h1tmpg;
	  }

	} // end the loop on individual turn-on curves

	TString L1_TP_str = FS_pre_trigger + "_dimuonTP_L1" + "_Turn_On_Curve_wrt_probe_MuidCB";
	sden = nd_dir + FS_pre_trigger + "_dimuonTP" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";
	snum = nd_dir + FS_pre_trigger + "_dimuonTP" + "_Turn_On_Curve_wrt_L1_probe_MuidCB" + "_Denominator";
	seff = eff_dir + L1_TP_str;
	seffg = seff + "_Fit";
	stmp = L1_TP_str;
	stmpg=L1_TP_str + "_Fit";

	h1num = 0;
	mf.get(snum, h1num);
	if (!h1num) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	  }
	  continue;
	}
	h1den = 0;
	mf.get(sden, h1den);
	if (!h1den) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	  }
	  continue;
	}

	if( h1num && h1den){
	  h1tmp = (TH1F*)h1den->Clone();
	  h1tmp->SetName(stmp);                          				
	  h1tmp->SetTitle(stmp);                         			  
	  h1tmp->GetYaxis()->SetTitle("Efficiency");     				  
	  h1tmp->Reset();                                				  
	  h1tmp->Divide(h1num, h1den, 1., 1., "B");      				  
	  dir->cd();                                    				  
	  h1tmp->Write();                                				  
	  h1tmpg = new TGraphAsymmErrors();
	  h1tmpg->SetMarkerStyle(20);
	  h1tmpg->SetMinimum(0.0);
	  h1tmpg->SetMaximum(1.05);
	  h1tmpg->BayesDivide(h1num, h1den);
	  h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
	  h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
	  dir->cd();
	  h1tmpg->SetName(stmpg);
	  h1tmpg->Write();
	  delete h1tmpg;
	}

	//**** summargy plot **********//

	TString histNumB = nd_dir + chainName +"_tagEFFSpre_Turn_On_Curve_wrt_probe_MuidCB_Barrel_Numerator";
	TString histDenB = nd_dir + chainName +"_tagEFFSpre_Turn_On_Curve_wrt_probe_MuidCB_Barrel_Denominator"; 
	TString histNumE = nd_dir + chainName +"_tagEFFSpre_Turn_On_Curve_wrt_probe_MuidCB_Endcap_Numerator"; 
	TString histDenE = nd_dir + chainName +"_tagEFFSpre_Turn_On_Curve_wrt_probe_MuidCB_Endcap_Denominator"; 
	TString histL1sum = eff_dir + chainName + "_EFplateau_wrtOffline";

	TH1F *h1numb = 0; mf.get(histNumB, h1numb);
	if (!h1numb) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histNumB << std::endl;
	  }
	  continue;
	}
	TH1F *h1nume = 0; mf.get(histNumE, h1nume);
	if (!h1nume) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histNumE << std::endl;
	  }
	  continue;
	}
	TH1F *h1denb = 0; mf.get(histDenB, h1denb);
	if (!h1denb) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histDenB << std::endl;
	  }
	  continue;
	}
	TH1F *h1dene = 0; mf.get(histDenE, h1dene);
	if (!h1dene) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histDenE << std::endl;
	  }
	  continue;
	}
	TH1F *h1sumL = 0; mf.get(histL1sum, h1sumL);
	if (!h1sumL) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histL1sum << std::endl;
	  }
	  continue;
	}

	int iSTDL = 75;//25GeV
	int iSTDH = 120;
	if(HI_pp_key){//HI run 4-25GeV
	  iSTDL = 17;
	  iSTDH = 75;				
	}
	double sumeff, sumerr;
	double sumn = h1numb->Integral(iSTDL, iSTDH); // 60-100 GeV
	double sumd = h1denb->Integral(iSTDL, iSTDH);
	if (sumd == 0.) {
	  sumeff = 0.;
	  sumerr = 0.;
	} else {
	  sumeff = (double)sumn / (double) sumd;
	  sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	}

	h1sumL->SetBinContent(1, sumeff);
	h1sumL->SetBinError(1, sumerr);

	sumn = h1nume->Integral(iSTDL, iSTDH);
	sumd = h1dene->Integral(iSTDL, iSTDH);
	if (sumd == 0.) {
	  sumeff = 0.;
	  sumerr = 0.;
	} else {
	  sumeff = (double)sumn / (double) sumd;
	  sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	}
	h1sumL->GetYaxis()->SetTitle("Efficiency");     				  
	h1sumL->SetBinContent(2, sumeff);
	h1sumL->SetBinError(2, sumerr);
        h1sumL->SetMinimum(0.0);
        h1sumL->SetMaximum(1.05);

	dir->cd();
	h1sumL->Write("",TObject::kOverwrite);

	TString histNum_mu0_15 = nd_dir + chainName +"_tagEFFSpre_mu0_15_Turn_On_Curve_wrt_probe_MuidCB_Numerator";
	TString histDen_mu0_15 = nd_dir + chainName +"_tagEFFSpre_mu0_15_Turn_On_Curve_wrt_probe_MuidCB_Denominator"; 
	TString histNum_mu15_20 = nd_dir + chainName +"_tagEFFSpre_mu15_20_Turn_On_Curve_wrt_probe_MuidCB_Numerator"; 
	TString histDen_mu15_20 = nd_dir + chainName +"_tagEFFSpre_mu15_20_Turn_On_Curve_wrt_probe_MuidCB_Denominator"; 
	TString histNum_mu20 = nd_dir + chainName +"_tagEFFSpre_mu20_Turn_On_Curve_wrt_probe_MuidCB_Numerator"; 
	TString histDen_mu20 = nd_dir + chainName +"_tagEFFSpre_mu20_Turn_On_Curve_wrt_probe_MuidCB_Denominator"; 
	TString histEFsum_mu = eff_dir + chainName + "_EFplateau_wrtOffline_mu_dependence";

	TH1F *h1num_mu0_15 = 0; mf.get(histNum_mu0_15, h1num_mu0_15);
	if (!h1num_mu0_15) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histNum_mu0_15 << std::endl;
	  }
	  continue;
	}

	TH1F *h1num_mu15_20 = 0; mf.get(histNum_mu15_20, h1num_mu15_20);
	if (!h1num_mu15_20) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histNum_mu15_20 << std::endl;
	  }
	  continue;
	}

	TH1F *h1num_mu20 = 0; mf.get(histNum_mu20, h1num_mu20);
	if (!h1num_mu20) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histNum_mu20 << std::endl;
	  }
	  continue;
	}

	TH1F *h1den_mu0_15 = 0; mf.get(histDen_mu0_15, h1den_mu0_15);
	if (!h1den_mu0_15) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histDen_mu0_15 << std::endl;
	  }
	  continue;
	}

	TH1F *h1den_mu15_20 = 0; mf.get(histDen_mu15_20, h1den_mu15_20);
	if (!h1den_mu15_20) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histDen_mu15_20 << std::endl;
	  }
	  continue;
	}

	TH1F *h1den_mu20 = 0; mf.get(histDen_mu20, h1den_mu20);
	if (!h1den_mu20) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histDen_mu20 << std::endl;
	  }
	  continue;
	}

	TH1F *h1sum_mu = 0; mf.get(histEFsum_mu, h1sum_mu);
	if (!h1sum_mu) {
	  if (for_mydebug) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histEFsum_mu << std::endl;
	  }
	  continue;
	}

	sumn = h1num_mu0_15->Integral(iSTDL, iSTDH); // 25-100 GeV
	sumd = h1den_mu0_15->Integral(iSTDL, iSTDH);
	if (sumd == 0.) {
	  sumeff = 0.;
	  sumerr = 0.;
	} else {
	  sumeff = (double)sumn / (double) sumd;
	  sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	}

	h1sum_mu->SetBinContent(1, sumeff);
	h1sum_mu->SetBinError(1, sumerr);

	sumn = h1num_mu15_20->Integral(iSTDL, iSTDH);
	sumd = h1den_mu15_20->Integral(iSTDL, iSTDH);
	if (sumd == 0.) {
	  sumeff = 0.;
	  sumerr = 0.;
	} else {
	  sumeff = (double)sumn / (double) sumd;
	  sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	}
	h1sum_mu->SetBinContent(2, sumeff);
	h1sum_mu->SetBinError(2, sumerr);

	sumn = h1num_mu20->Integral(iSTDL, iSTDH);
	sumd = h1den_mu20->Integral(iSTDL, iSTDH);
	if (sumd == 0.) {
	  sumeff = 0.;
	  sumerr = 0.;
	} else {
	  sumeff = (double)sumn / (double) sumd;
	  sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	}
	h1sum_mu->GetYaxis()->SetTitle("Efficiency");     				  
	h1sum_mu->SetBinContent(3, sumeff);
	h1sum_mu->SetBinError(3, sumerr);
	h1sum_mu->SetMaximum(1.05);
	h1sum_mu->SetMinimum(0.0);
	dir->cd();
	h1sum_mu->Write("",TObject::kOverwrite);
	mf.Write();
      }
      //  end of the code add by Yuan //
      // ******************************************************//
      TString alg2[3] = {"_L2MuonSA", "_MuonEFMS", "_MuonEFSA"};
      TString wrtalg2[3] = {"_L1", "_L2MuonSA", "_L2MuonSA"};

      // ******************************************************//
      // ******************  MSonly Chains ********************//
      // ******************************************************//
      for( unsigned int i=0 ; i < chainsMSonly.size() ; i++ ){
	TString chainName = chainsMSonly.at(i);

	for( int trg = 0 ; trg < maxindep ; trg++ ){
	  sden = nd_dir + chainName + "_Turn_On_Curve_wrt_MuidSA" + trigger[trg] + "_Triggered_Denominator";

	  for(int alg = 0 ; alg < 3 ; alg++ ){
	    snum = nd_dir  + chainName + alg2[alg] + "_Turn_On_Curve_wrt_MuidSA" + trigger[trg] + "_Triggered_Numerator";
	    seff = eff_dir + chainName + alg2[alg] + "_Turn_On_Curve_wrt_MuidSA" + trigger[trg] + "_Triggered";
	    seffg = seff + "_Fit"; // YY added 20.04.10
	    stmp = chainName + alg2[alg] + "_Turn_On_Curve_wrt_MuidSA" + trigger[trg] +"_Triggered";
	    stmpg = chainName + alg2[alg] + "_Turn_On_Curve_wrt_MuidSA" + trigger[trg] +"_Triggered" + "_Fit";

	    h1num = 0;
	    mf.get(snum, h1num);
	    if (!h1num) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	      }
	      continue;
	    }
	    h1den = 0;
	    mf.get(sden, h1den);
	    if (!h1den) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	      }
	      continue;
	    }

	    if( h1num && h1den){
	      h1tmp = (TH1F*)h1den->Clone();
	      h1tmp->SetName(stmp);                          				
	      h1tmp->SetTitle(stmp);                         			  
	      h1tmp->GetYaxis()->SetTitle("Efficiency");     				  
	      h1tmp->Reset();                                				  
	      h1tmp->Divide(h1num, h1den, 1., 1., "B");      				  
	      dir->cd();                                    				  
	      h1tmp->Write();                                				  
	      h1tmpg = new TGraphAsymmErrors();
	      h1tmpg->SetName(stmpg);
	      h1tmpg->SetMarkerStyle(20);
	      h1tmpg->SetMinimum(0.0);
	      h1tmpg->SetMaximum(1.05);
	      h1tmpg->BayesDivide(h1num, h1den);
	      h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
	      h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
	      dir->cd();
	      h1tmpg->Write();
	      delete h1tmpg;

	      // summary for jet trigger
	      if (0 == alg || 2 == alg) {
		if (3 == trg) { // jet
		  double sumeff, sumerr;
		  double sumn = h1num->Integral(iMSL, iMSH);
		  double sumd = h1den->Integral(iMSL, iMSH);
		  if (sumd == 0.) {
		    sumeff = 0.;
		    sumerr = 0.;
		  } else {
		    sumeff = (double)sumn / (double)sumd;
		    sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		  }
		  int iholx = -1;
		  if (0 == alg) {
		    iholx = static_cast<int>(iL2MuonSA);
		  } else if (2 == alg) {
		    iholx = static_cast<int>(iEFSA);
		  }

		  if (iholx >= 0) {
		    TString s = eff_dir + chainName + "_highpt_effsummary_by" + vectkwd.at(2);
		    // std::cerr << "hist summary: " << s << " n: " << sumn << " d: " << sumd << " eff: " << sumeff << " err: " << sumerr << std::endl;
		    mf.get(s, h1sumeff);
		    if (!h1sumeff) {
		      if (fdbg) {
			std::cerr << "HLTMuon PostProcessing: no such histogram!! " << s << std::endl;
		      }
		      continue;
		    }
		    h1sumeff->SetBinContent(iholx+1, sumeff);
		    h1sumeff->SetBinError(iholx+1, sumerr);
                    h1sumeff->GetYaxis()->SetTitleOffset(1.3);
                    h1sumeff->SetMinimum(0.0);
	            h1sumeff->SetMaximum(1.05);
		    // saving
		    dir->cd();
		    h1sumeff->Write("", TObject::kOverwrite);
		  }
		}
	      }
	    }

	  }//alg
	}//trg
	mf.Write();

	for( int alg = 0 ; alg < 3 ; alg++ ){
	  //wrt MuidSA
	  sden  = nd_dir  + chainName + "_Turn_On_Curve_wrt_MuidSA_Denominator";
	  snum  = nd_dir  + chainName + alg2[alg] + "_Turn_On_Curve_Numerator";
	  seff  = eff_dir + chainName + alg2[alg] + "_Turn_On_Curve_wrt_MuidSA";
	  seffg = seff + "_Fit"; // YY added 20.04.10
	  stmp = chainName + alg2[alg] + "_Turn_On_Curve_wrt_MuidSA";
	  stmpg = chainName + alg2[alg] + "_Turn_On_Curve_wrt_MuidSA"+"_Fit";

	  h1num = 0;
	  mf.get(snum, h1num);
	  if (!h1num) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	    }
	    continue;
	  }
	  h1den = 0;
	  mf.get(sden, h1den);
	  if (!h1den) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	    }
	    continue;
	  }

	  if(h1num && h1den){
	    h1tmp = (TH1F*)h1den->Clone();
	    h1tmp->SetName(stmp);                          				
	    h1tmp->SetTitle(stmp);                         			  
	    h1tmp->GetYaxis()->SetTitle("Efficiency");     				  
	    h1tmp->Reset();                                				  
	    h1tmp->Divide(h1num, h1den, 1., 1., "B");      				  
	    dir->cd();                                    				  
	    h1tmp->Write(); 
	    h1tmpg = new TGraphAsymmErrors();
	    h1tmpg->SetName(stmpg);
	    h1tmpg->SetMarkerStyle(20);
	    h1tmpg->SetMinimum(0.0);
	    h1tmpg->SetMaximum(1.05);
	    h1tmpg->BayesDivide(h1num, h1den);
	    h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
	    h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
	    dir->cd();
	    h1tmpg->Write();
	    delete h1tmpg;

	    if (0 == alg || 2 == alg) { // no condition on ES bits = all events
	      double sumeff, sumerr;
	      double sumn = h1num->Integral(iMSL, iMSH);
	      double sumd = h1den->Integral(iMSL, iMSH);
	      if (sumd == 0.) {
		sumeff = 0.;
		sumerr = 0.;
	      } else {
		sumeff = (double)sumn / (double)sumd;
		sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	      }
	      int iholx = -1;
	      if (0 == alg) {
		iholx = static_cast<int>(iL2MuonSA);
	      } else if (2 == alg) {
		iholx = static_cast<int>(iEFSA);
	      }

	      if (iholx >= 0) {
		TString s = eff_dir + chainName + "_highpt_effsummary_by" + vectkwd.at(3);
		// std::cerr << "hist summary: " << s << " n: " << sumn << " d: " << sumd << " eff: " << sumeff << " err: " << sumerr << std::endl;
		mf.get(s, h1sumeff);
		if (!h1sumeff) {
		  if (fdbg) {
		    std::cerr << "HLTMuon PostProcessing: no such histogram!! " << s << std::endl;
		  }
		  continue;
		}
                h1sumeff->GetYaxis()->SetTitleOffset(1.3);
		h1sumeff->SetBinContent(iholx+1, sumeff);
		h1sumeff->SetBinError(iholx+1, sumerr);
  	        h1sumeff->SetMinimum(0.0);
	        h1sumeff->SetMaximum(1.05);
		// saving
		dir->cd();
		h1sumeff->Write("", TObject::kOverwrite);
	      }
	    }
	  }
	  //wrt MuidSA

	  // for ES ----------------------------------------------------------------
	  for (int ies = 0; ies <= maxESbr; ies++) {

	    if(!MS_mon_ESbr[ies])continue;
	    // for ES, L1 ------------------------------------------------------------
	    if (0 == alg) {
	      sden = nd_dir + chainName + triggerES[ies] + "_Turn_On_Curve_wrt_MuidSA_Denominator";
	      snum = nd_dir + chainName + triggerES[ies] + "_L2MuonSA" + "_Turn_On_Curve_wrt" + "_L1" + "_Denominator";
	      seff = eff_dir + chainName + triggerES[ies] + "_L1" + "_Turn_On_Curve_wrt_MuidSA";
	      seffg = seff + "_Fit";
	      stmp = chainName + triggerES[alg] + "_L1"+"_Turn_On_Curve_wrt_MuidSA";
	      stmpg= chainName + triggerES[alg] + "_L1"+"_Turn_On_Curve_wrt_MuidSA" + "_Fit";

	      h1num = 0;
	      mf.get(snum, h1num);
	      if (!h1num) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
		}
		continue;
	      }
	      h1den = 0;
	      mf.get(sden, h1den);
	      if (!h1den) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		}
		continue;
	      }

	      if( h1num && h1den){
		h1tmp = (TH1F*)h1den->Clone();
		h1tmp->SetName(stmp);                          				
		h1tmp->SetTitle(stmp);                         			  
		h1tmp->GetYaxis()->SetTitle("Efficiency");     				  
		h1tmp->Reset();                                				  
		h1tmp->Divide(h1num, h1den, 1., 1., "B");      				  
		dir->cd();                                    				  
		h1tmp->Write();                                				  
		h1tmpg = new TGraphAsymmErrors();
		h1tmpg->SetName(stmpg);
		h1tmpg->SetMarkerStyle(20);
		h1tmpg->SetMinimum(0.0);
		h1tmpg->SetMaximum(1.05);
		h1tmpg->BayesDivide(h1num, h1den);
		h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
		h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
		dir->cd();
		h1tmpg->Write();
		delete h1tmpg;
	      }

	      for (int be = 0; be < 2; be++) {
		sden = nd_dir + chainName + triggerES[ies] + "_Turn_On_Curve_wrt_MuidSA" + bestr[be] + "_Denominator";
		snum = nd_dir + chainName + triggerES[ies] + "_L2MuonSA" + "_Turn_On_Curve_wrt" + "_L1" + bestr[be] + "_Denominator";
		seff  = eff_dir + chainName + triggerES[ies] + "_L1" + bestr[be] + "_Turn_On_Curve_wrt_MuidSA";
		seffg = seff + "_Fit";
		stmp = chainName + triggerES[ies] + "_L1" + bestr[be] + "_Turn_On_Curve_wrt_MuidSA";
		stmpg = chainName + triggerES[ies] + "_L1" + bestr[be] + "_Turn_On_Curve_wrt_MuidSA"+"_Fit";

		h1num = 0;
		mf.get(snum, h1num);
		if (!h1num) {
		  if (fdbg) {
		    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
		  }
		  continue;
		}
		h1den = 0;
		mf.get(sden, h1den);
		if (!h1den) {
		  if (fdbg) {
		    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		  }
		  continue;
		}

		if( h1num && h1den){
		  h1tmp = (TH1F*)h1den->Clone();                                            		
		  h1tmp->SetName(stmp);                          						
		  h1tmp->SetTitle(stmp);                         			   		
		  h1tmp->GetYaxis()->SetTitle("Efficiency");     				  		
		  h1tmp->Reset();                                				  		
		  h1tmp->Divide(h1num, h1den, 1., 1., "B");      				  		
		  dir->cd();                                    				  		
		  h1tmp->Write();                                				  		
		  h1tmpg = new TGraphAsymmErrors();
		  h1tmpg->SetName(stmpg);
		  h1tmpg->SetMarkerStyle(20);                                               		
		  h1tmpg->SetMinimum(0.0);
		  h1tmpg->SetMaximum(1.05);
		  h1tmpg->BayesDivide(h1num, h1den);                                        		
		  h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
		  h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
		  dir->cd();                                                                		
		  h1tmpg->Write();                                                          		
		  delete h1tmpg;
		}
	      }
	    }

	    // for ES, L1 end ------------------------------------------------------------
	    sden  = nd_dir  + chainName + triggerES[ies] + "_Turn_On_Curve_wrt_MuidSA_Denominator";
	    snum  = nd_dir  + chainName + triggerES[ies] + alg2[alg] + "_Turn_On_Curve_Numerator";
	    seff  = eff_dir + chainName + triggerES[ies] + alg2[alg] + "_Turn_On_Curve_wrt_MuidSA";
	    seffg = seff + "_Fit"; // YY added 20.04.10
	    stmp = chainName + triggerES[ies] + alg2[alg] + "_Turn_On_Curve_wrt_MuidSA";
	    stmpg = chainName + triggerES[ies] + alg2[alg] + "_Turn_On_Curve_wrt_MuidSA" +"_Fit";

	    h1num = 0;
	    mf.get(snum, h1num);
	    if (!h1num) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	      }
	      continue;
	    }
	    h1den = 0;
	    mf.get(sden, h1den);
	    if (!h1den) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	      }
	      continue;
	    }

	    if(h1num && h1den){
	      h1tmp = (TH1F*)h1den->Clone();
	      h1tmp->SetName(stmp);                          											
	      h1tmp->SetTitle(stmp);                         			   							  
	      h1tmp->GetYaxis()->SetTitle("Efficiency");     				  							  
	      h1tmp->Reset();                                				  							  
	      h1tmp->Divide(h1num, h1den, 1., 1., "B");      				  							  
	      dir->cd();                                    				  							  
	      h1tmp->Write();                                				  							  
	      h1tmpg = new TGraphAsymmErrors();
	      h1tmpg->SetName(stmpg);
	      h1tmpg->SetMarkerStyle(20);                                               							  
	      h1tmpg->SetMinimum(0.0);
	      h1tmpg->SetMaximum(1.05);
	      h1tmpg->BayesDivide(h1num, h1den);                                        							  
	      h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
	      h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
	      dir->cd();                                                                							  
	      h1tmpg->Write();                                                          							  
	      delete h1tmpg;

	      if (0 == alg || 2 == alg) {
		if (ESTAG == ies || ESINDEP == ies) {
		  double sumeff, sumerr;
		  double sumn = h1num->Integral(iMSL, iMSH);
		  double sumd = h1den->Integral(iMSL, iMSH);
		  if (sumd == 0.) {
		    sumeff = 0.;
		    sumerr = 0.;
		  } else {
		    sumeff = (double)sumn / (double)sumd;
		    sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		  }
		  int iholx = -1;
		  if (0 == alg) {
		    iholx = static_cast<int>(iL2MuonSA);
		  } else if (2 == alg) {
		    iholx = static_cast<int>(iEFSA);
		  }

		  if (iholx >= 0) {
		    TString s = eff_dir + chainName + "_highpt_effsummary_by" + triggerES[ies];
		    // std::cerr << "hist summary: " << s << " n: " << sumn << " d: " << sumd << " eff: " << sumeff << " err: " << sumerr << std::endl;
		    mf.get(s, h1sumeff);
		    if (!h1sumeff) {
		      if (fdbg) {
			std::cerr << "HLTMuon PostProcessing: no such histogram!! " << s << std::endl;
		      }
		      continue;
		    }
                    h1sumeff->GetYaxis()->SetTitleOffset(1.3);
		    h1sumeff->SetBinContent(iholx+1, sumeff);
		    h1sumeff->SetBinError(iholx+1, sumerr);
                    h1sumeff->SetMinimum(0.0);
	            h1sumeff->SetMaximum(1.05);
		    // saving
		    dir->cd();
		    h1sumeff->Write("", TObject::kOverwrite);
		  }
		}
	      }
	    }
	  }
	  // for ES: end ----------------------------------------------------------------

	  if (0 == alg || 2 == alg) {
	    for (int be = 0; be < 2; be++) {
	      //wrt MuidSA
	      sden  = nd_dir  + chainName + "_Turn_On_Curve_wrt_MuidSA" + bestr[be] + "_Denominator";
	      snum  = nd_dir  + chainName + alg2[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	      seff  = eff_dir + chainName + alg2[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidSA";
	      seffg = seff + "_Fit"; // YY added 20.04.10
	      stmp = chainName + alg2[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidSA";
	      stmpg = chainName + alg2[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidSA" + "_Fit";

	      h1num = 0;
	      mf.get(snum, h1num);
	      if (!h1num) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
		}
		continue;
	      }
	      h1den = 0;
	      mf.get(sden, h1den);
	      if (!h1den) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		}
		continue;
	      }

	      if( h1num && h1den){
		h1tmp = (TH1F*)h1den->Clone();
		h1tmp->SetName(stmp);                          				
		h1tmp->SetTitle(stmp);                         			  
		h1tmp->GetYaxis()->SetTitle("Efficiency");     				  
		h1tmp->Reset();                                				  
		h1tmp->Divide(h1num, h1den, 1., 1., "B");      				  
		dir->cd();                                    				  
		h1tmp->Write();                                				  
		h1tmpg = new TGraphAsymmErrors();
		h1tmpg->SetName(stmpg);
		h1tmpg->SetMarkerStyle(20);
		h1tmpg->SetMinimum(0.0);
		h1tmpg->SetMaximum(1.05);
		h1tmpg->BayesDivide(h1num, h1den);
		h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
		h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
		dir->cd();
		h1tmpg->Write();
		delete h1tmpg;

	      }
	    }
	  }

	  //wrt upstream
	  sden  = nd_dir  + chainName + alg2[alg] + "_Turn_On_Curve_wrt" + wrtalg2[alg] + "_Denominator";
	  snum  = nd_dir  + chainName + alg2[alg] + "_Turn_On_Curve_Numerator";
	  seff  = eff_dir + chainName + alg2[alg] + "_Turn_On_Curve_wrt" + wrtalg2[alg];
	  seffg = seff + "_Fit"; // YY added 20.04.10
	  stmp =  chainName + alg2[alg] + "_Turn_On_Curve_wrt" + wrtalg2[alg];
	  stmpg =  chainName + alg2[alg] + "_Turn_On_Curve_wrt" + wrtalg2[alg]+"_Fit";

	  h1num = 0;
	  mf.get(snum, h1num);
	  if (!h1num) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	    }
	    continue;
	  }
	  h1den = 0;
	  mf.get(sden, h1den);
	  if (!h1den) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	    }
	    continue;
	  }

	  if(h1num && h1den){
	    h1tmp = (TH1F*)h1den->Clone();
	    h1tmp->SetName(stmp);                          				
	    h1tmp->SetTitle(stmp);                         			  
	    h1tmp->GetYaxis()->SetTitle("Efficiency");     				  
	    h1tmp->Reset();                                				  
	    h1tmp->Divide(h1num, h1den, 1., 1., "B");      				  
	    dir->cd();                                    				  
	    h1tmp->Write(); 
	    h1tmpg = new TGraphAsymmErrors();
	    h1tmpg->SetName(stmpg);
	    h1tmpg->SetMarkerStyle(20);
	    h1tmpg->SetMinimum(0.0);
	    h1tmpg->SetMaximum(1.05);
	    h1tmpg->BayesDivide(h1num, h1den);
	    h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
	    h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
	    dir->cd();
	    h1tmpg->Write();
	    delete h1tmpg;
	  }

	  //wrt upstream
	  // for ES --------------------------------------------------------------------
	  for (int ies = 0; ies <= maxESbr; ies++) {
	    if(!MS_mon_ESbr[ies])continue; 
	    sden  = nd_dir  + chainName + triggerES[ies] + alg2[alg] + "_Turn_On_Curve_wrt" + wrtalg2[alg] + "_Denominator";
	    snum  = nd_dir  + chainName + triggerES[ies] + alg2[alg] + "_Turn_On_Curve_Numerator";
	    seff  = eff_dir + chainName + triggerES[ies] + alg2[alg] + "_Turn_On_Curve_wrt" + wrtalg2[alg];
	    seffg = seff + "_Fit"; // YY added 20.04.10
	    stmp = chainName + triggerES[ies] + alg2[alg] + "_Turn_On_Curve_wrt" + wrtalg2[alg];
	    stmpg = chainName + triggerES[ies] + alg2[alg] + "_Turn_On_Curve_wrt" + wrtalg2[alg]+"_Fit";

	    h1num = 0;
	    mf.get(snum, h1num);
	    if (!h1num) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	      }
	      continue;
	    }
	    h1den = 0;
	    mf.get(sden, h1den);
	    if (!h1den) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	      }
	      continue;
	    }

	    if(h1num && h1den){
	      h1tmp = (TH1F*)h1den->Clone();
	      h1tmp->SetName(stmp);                          				
	      h1tmp->SetTitle(stmp);                         			  
	      h1tmp->GetYaxis()->SetTitle("Efficiency");     				  
	      h1tmp->Reset();                                				  
	      h1tmp->Divide(h1num, h1den, 1., 1., "B");      				  
	      dir->cd();                                    				  
	      h1tmp->Write();    
	      h1tmpg = new TGraphAsymmErrors();
	      h1tmpg->SetName(stmpg);
	      h1tmpg->SetMarkerStyle(20);
	      h1tmpg->SetMinimum(0.0);
	      h1tmpg->SetMaximum(1.05);
	      h1tmpg->BayesDivide(h1num, h1den);
	      h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
	      h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
	      dir->cd();
	      h1tmpg->Write();
	      delete h1tmpg;
	    }
	  }
	  // for ES: end --------------------------------------------------------------------

	  if (0 == alg || 2 == alg) {
	    for (int be = 0; be < 2; be++) {
	      //wrt upstream
	      sden  = nd_dir  + chainName + alg2[alg] + "_Turn_On_Curve_wrt" + wrtalg2[alg] + bestr[be] + "_Denominator";
	      snum  = nd_dir  + chainName + alg2[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	      seff  = eff_dir + chainName + alg2[alg] + "_Turn_On_Curve_wrt" + wrtalg2[alg] + bestr[be];
	      seffg = seff + "_Fit"; // YY added 20.04.10
	      stmp =  chainName + alg2[alg] + "_Turn_On_Curve_wrt" + wrtalg2[alg] + bestr[be];
	      stmpg =  chainName + alg2[alg] + "_Turn_On_Curve_wrt" + wrtalg2[alg] + bestr[be]+"_Fit";

	      h1num = 0;
	      mf.get(snum, h1num);
	      if (!h1num) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
		}
		continue;
	      }
	      h1den = 0;
	      mf.get(sden, h1den);
	      if (!h1den) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		}
		continue;
	      }

	      if( h1num && h1den){
		h1tmp = (TH1F*)h1den->Clone();
		h1tmp->SetName(stmp);                          				
		h1tmp->SetTitle(stmp);                         			  
		h1tmp->GetYaxis()->SetTitle("Efficiency");     				  
		h1tmp->Reset();                                				  
		h1tmp->Divide(h1num, h1den, 1., 1., "B");      				  
		dir->cd();                                    				  
		h1tmp->Write();                                				  
		h1tmpg = new TGraphAsymmErrors();
		h1tmpg->SetName(stmpg);
		h1tmpg->SetMarkerStyle(20);
		h1tmpg->SetMinimum(0.0);
		h1tmpg->SetMaximum(1.05);
		h1tmpg->BayesDivide(h1num, h1den);
		h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
		h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
		dir->cd();
		h1tmpg->Write();
		delete h1tmpg;
	      }
	    }
	  }
	}//alg
	mf.Write();
      }//i

      //==Rate
      dir = mf.GetDirectory(rr_dir);
      if(!dir){
	if (fdbg) {
	  std::cerr<< "HLTMuonHistogramDivision: directory "<<rr_dir<<" not found"<<std::endl;
	}
	return;
      }

      //  EF rate / offline rate
      std::string type[3] = {"MS_", "SA_", "CB_"};
      std::string off[3] = {"Moore_MS_", "Moore_SA_", "Muid_"};
      std::string cut[2] = {"4", "10"};

      for( int itype = 0 ; itype < 3 ; itype++ ){
	for( int icut = 0 ; icut < 2 ; icut++ ){
	  seff   = rr_dir + "EF_" + type[itype] + "Over_" + off[itype] + cut[icut] + "GeV_Cut";
	  snum   = rate_dir + "Number_Of_EF_" + type[itype] + "Muons_" + cut[icut] + "GeV_Cut";
	  sden = rate_dir + "Number_Of_" + off[itype] + "Muons_" + cut[icut] + "GeV_Cut";
	  stmp = "EF_" + type[itype] + "Over_" + off[itype] + cut[icut] + "GeV_Cut";
	  stmpg = "EF_" + type[itype] + "Over_" + off[itype] + cut[icut] + "GeV_Cut"+"_Fit";

	  h1num = 0;
	  mf.get(snum, h1num);
	  if (!h1num) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	    }
	    continue;
	  }
	  h1den = 0;
	  mf.get(sden, h1den);
	  if (!h1den) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	    }
	    continue;
	  }

	  if( h1num && h1den){
	    h1tmp = (TH1F*)h1den->Clone();
	    h1tmp->SetName(stmp);                          				
	    h1tmp->SetTitle(stmp);                         			  
	    h1tmp->GetYaxis()->SetTitle("Efficiency");     				  
	    h1tmp->Reset();                                				  
	    h1tmp->Divide(h1num, h1den, 1., 1., "B");      				  
	    dir->cd();                                    				  
	    h1tmp->Write();                                				  
	  }
	}//icut
      }//itype
      mf.Write();

      // Triggers / Event
      std::vector<TString> chains;
      std::vector<TString> chains2;

      {
	std::map<std::string, std::string> ztpmap;

	//// cosmic menu items for ZTP
	//ztpmap["mu4_cosmic_L1MU4_EMPTY"] = "L1_MU4";  
	//ztpmap["mu24_imedium"] = "L1_MU15";
	ztpmap["muChain1"] = "L1_MU15";
	ztpmap["muChain2"] = "L1_MU15";
	ztpmap["muChainEFiso1"] = "L1_MU15";
	ztpmap["muChainEFiso2"] = "L1_MU15";
	ztpmap["muChainMSonly1"] = "L1_MU15";
	ztpmap["muChainMSonly2"] = "L1_MU15";

	std::map<std::string, int> ztp_isomap;
	//ztp_isomap["mu4_cosmic_L1MU4_EMPTY"] = 0;
	//ztp_isomap["mu24_imedium"] = 1;
	ztp_isomap["muChain1"] = 0;
	ztp_isomap["muChain2"] = 0;
	ztp_isomap["muChainEFiso1"] = 1;
	ztp_isomap["muChainEFiso2"] = 1;
	ztp_isomap["muChainMSonly1"] = 0;
	ztp_isomap["muChainMSonly2"] = 0;

	//// pp_v4 menu items for ZTP
	//ztpmap["mu36_tight"] = "L1_MU15";
	//ztpmap["mu24i_tight"] = "L1_MU15";
	//ztpmap["mu50_MSonly_barrel_tight"] = "L1_MU15";

	//std::map<std::string, int> ztp_isomap;
	//ztp_isomap["mu36_tight"] = 0;
	//ztp_isomap["mu24i_tight"] = 1;
	//ztp_isomap["mu50_MSonly_barrel_tight"] = 0;

	// old menu pp_v3 for ZTP
	// ztpmap["mu15"]="L1_MU10";
	// ztpmap["mu15i"]="L1_MU10";
	// ztpmap["mu20i_medium"]="L1_MU11";
	// ztpmap["mu20"]="L1_MU10";
	// ztpmap["mu20_MG"]="L1_MU10";
	// ztpmap["mu22_medium"]="L1_MU11"; // YY
	// ztpmap["mu22_MG_medium"]="L1_MU11";
	// ztpmap["mu40_MSonly_barrel"]="L1_MU10";
	// ztpmap["mu40_MSonly_barrel_medium"]="L1_MU11";
	// ztpmap["mu40_MSonly_tight"]="L1_MU10";
	// ztpmap["mu40_MSonly_tighter"]="L1_MU10";

	for(std::map<std::string, std::string>::iterator itmap=ztpmap.begin();itmap!=ztpmap.end();++itmap){
	  TString histdirmuztp = run_dir + "/HLT/MuonMon/MuZTP/"+itmap->first;
	  TDirectory* ztpdir = mf.GetDirectory(histdirmuztp);
	  bool  isefisochain = ztp_isomap[itmap->first] > 0; 

	  //efficiency histograms
	  std::vector<std::string> var;
	  var.push_back("_Pt_");
	  var.push_back("_Pt_EC_");
	  var.push_back("_Pt_B_");
	  var.push_back("_Pt_4bins_");
	  var.push_back("_Pt_B_4bins_");
	  var.push_back("_Eta_");
	  var.push_back("_Phi_");
	  var.push_back("_Phi_EC_");
	  var.push_back("_Phi_B_");
	  for(unsigned int k=0;k<var.size();k++){
	    std::vector<std::string> level;
	    level.push_back("L1");
	    level.push_back("L2");
	    level.push_back("EF");
	    if(isefisochain) level.push_back("EFIso");
	    for(unsigned int j=0;j<level.size();j++){
	      //ABSOLUTE
	      seffg = histdirmuztp + "/muZTP_eff_"+level[j]+var[k]+itmap->first;
	      snum = histdirmuztp + "/muZTP"+var[k]+level[j]+"fired_"+itmap->first;
	      sden = histdirmuztp + "/muZTP"+var[k]+itmap->first;
	      stmp = "muZTP_eff_"+level[j]+var[k] + itmap->first;
	      stmpg = "muZTP_eff_"+level[j]+var[k] + itmap->first;

	      // for debugging
	      h1num = 0;
	      mf.get(snum, h1num);
	      if (!h1num) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
		}
		continue;
	      }
	      h1den = 0;
	      mf.get(sden, h1den);
	      if (!h1den) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		}
		continue;
	      }

	      if(h1num && h1den){
		h1tmpg = new TGraphAsymmErrors();
		h1tmpg->SetName(stmpg);
		h1tmpg->SetMarkerStyle(20);
		h1tmpg->SetMinimum(0.0);
		h1tmpg->SetMaximum(1.05);
	        h1tmpg->BayesDivide(h1num, h1den);
		h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
		h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
		ztpdir->cd();
		h1tmpg->Write();
		delete h1tmpg;
	      }
	      mf.Write();

	      seffg = histdirmuztp + "/muZTP_eff_"+level[j]+"_Eta_1bin_"+itmap->first;
	      snum = histdirmuztp + "/muZTP_Eta_1bin_"+level[j]+"fired_"+itmap->first;
	      sden = histdirmuztp + "/muZTP_Eta_1bin_"+itmap->first;
	      stmpg= "muZTP_eff_"+level[j]+"_Eta_1bin_"+itmap->first;

	      h1num = 0;
	      mf.get(snum, h1num);
	      if (!h1num) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
		}
		continue;
	      }
	      h1den = 0;
	      mf.get(sden, h1den);
	      if (!h1den) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		}
		continue;
	      }

	      if(h1num && h1den){
		h1tmpg = new TGraphAsymmErrors();
		h1tmpg->SetName(stmpg);
		h1tmpg->SetMarkerStyle(20);
		h1tmpg->SetMinimum(0.0);
		h1tmpg->SetMaximum(1.05);
		h1tmpg->BayesDivide(h1num, h1den);
		h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
		h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
		ztpdir->cd();
		h1tmpg->Write();
		delete h1tmpg;
	      }
	      mf.Write();

	      seffg = histdirmuztp + "/muZTP_eff_"+level[j]+"_Eta_2bins_"+itmap->first;
	      snum = histdirmuztp + "/muZTP_Eta_2bins_"+level[j]+"fired_"+itmap->first;
	      sden = histdirmuztp + "/muZTP_Eta_2bins_"+itmap->first;
	      stmpg = "muZTP_eff_" + level[j] +"_Eta_2bins_"+itmap->first;

	      h1num = 0;
	      mf.get(snum, h1num);
	      if (!h1num) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
		}
		continue;
	      }
	      h1den = 0;
	      mf.get(sden, h1den);
	      if (!h1den) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		}
		continue;
	      }

	      if(h1num && h1den){
		h1tmpg = new TGraphAsymmErrors();
		h1tmpg->SetName(stmp);
		h1tmpg->SetMarkerStyle(20);
		h1tmpg->SetMinimum(0.0);
		h1tmpg->SetMaximum(1.05);
		h1tmpg->BayesDivide(h1num, h1den);
		h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
		h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
		ztpdir->cd();
		h1tmpg->Write();
		delete h1tmpg;
	      }
	      mf.Write();

	      seffg = histdirmuztp + "/muZTP_eff_"+level[j]+"_Eta_1bin_cut_"+itmap->first;
	      snum = histdirmuztp + "/muZTP_Eta_1bin_cut_"+level[j]+"fired_"+itmap->first;
	      sden = histdirmuztp + "/muZTP_Eta_1bin_cut_"+itmap->first;
	      stmpg = "muZTP_eff_"+level[j]+"_Eta_1bin_cut_"+itmap->first;

	      h1num = 0;
	      mf.get(snum, h1num);
	      if (!h1num) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
		}
		continue;
	      }
	      h1den = 0;
	      mf.get(sden, h1den);
	      if (!h1den) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		}
		continue;
	      }

	      if(h1num && h1den){
		h1tmpg = new TGraphAsymmErrors();
		h1tmpg->SetName(stmpg);
		h1tmpg->SetMarkerStyle(20);
		h1tmpg->SetMinimum(0.0);
		h1tmpg->SetMaximum(1.05);
		h1tmpg->BayesDivide(h1num, h1den);
		h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
		h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
		ztpdir->cd();
		h1tmpg->Write();
		delete h1tmpg;
	      }
	      mf.Write();

	      seffg = histdirmuztp + "/muZTP_eff_"+level[j]+"_Eta_2bins_cut_"+itmap->first;
	      snum = histdirmuztp + "/muZTP_Eta_2bins_cut_"+level[j]+"fired_"+itmap->first;
	      sden = histdirmuztp + "/muZTP_Eta_2bins_cut_"+itmap->first;
	      stmpg = "muZTP_eff_"+level[j]+"_Eta_2bins_cut_"+itmap->first;

	      h1num = 0;
	      mf.get(snum, h1num);
	      if (!h1num) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
		}
		continue;
	      }
	      h1den = 0;
	      mf.get(sden, h1den);
	      if (!h1den) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		}
		continue;
	      }

	      if(h1num && h1den){
		h1tmpg = new TGraphAsymmErrors();
		h1tmpg->SetName(stmpg);
		h1tmpg->SetMarkerStyle(20);
		h1tmpg->SetMinimum(0.0);
		h1tmpg->SetMaximum(1.05);
		h1tmpg->BayesDivide(h1num, h1den);
		h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
		h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
		ztpdir->cd();
		h1tmpg->Write();
		delete h1tmpg;
	      }
	      mf.Write();

	      //2D ETA_PHI
	      seff = histdirmuztp + "/muZTP_eff_EtaPhi_"+level[j]+"_" + itmap->first;
	      snum = histdirmuztp + "/muZTP_EtaPhi_"+level[j]+"_" + itmap->first;
	      sden = histdirmuztp + "/muZTP_EtaPhi_all_"+ itmap->first;
	      stmp = "muZTP_eff_EtaPhi_"+level[j]+"_"+itmap->first;

	      TH2F *h2num(0);
	      TH2F *h2den(0);
	      TH2F *h2tmp(0);

	      h2num = 0;
	      mf.get(snum, h2num);
	      if(!h2num){
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
		}
		continue;
	      }
	      h2den = 0;
	      mf.get(sden, h2den);
	      if(!h2den) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		}
		continue;
	      }

	      if( h2num && h2den){
		h2tmp = (TH2F*)h1den->Clone();
		h2tmp->SetName(stmp);
		h2tmp->SetTitle(stmp);
		h2tmp->GetYaxis()->SetTitle("Efficiency");
		h2tmp->Reset();
		h2tmp->Divide(h1num, h1den, 1., 1., "B");
		dir->cd();
		h2tmp->Write();
	      }
	      mf.Write();
	    }//level

	    //RELATIVE
	    seffg = histdirmuztp + "/muZTP_eff_EFwrtL2"+var[k]+itmap->first;
	    snum = histdirmuztp + "/muZTP"+var[k]+"EFL2fired_"+itmap->first;
	    sden = histdirmuztp + "/muZTP"+var[k]+"L2fired_"+itmap->first;
	    stmpg =  "muZTP_eff_EFwrtL2" + var[k]+itmap->first;

	    h1num = 0;
	    mf.get(snum, h1num);
	    if (!h1num) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	      }
	      continue;
	    }
	    h1den = 0;
	    mf.get(sden, h1den);
	    if (!h1den) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	      }
	      continue;
	    }

	    if(h1num && h1den){
	     // h1tmp = (TH1F*)h1den->Clone();
	     // h1tmp->SetName(stmp);                          				
	     // h1tmp->SetTitle(stmp);                         			  
	    //  h1tmp->GetYaxis()->SetTitle("Efficiency");     				  
	    //  h1tmp->Reset();                                				  
	    //  h1tmp->Divide(h1num, h1den, 1., 1., "B");      				  
	    //  dir->cd();                                    				  
	    //  h1tmp->Write();
	      h1tmpg = new TGraphAsymmErrors();
	      h1tmpg->SetName(stmpg);
	      h1tmpg->SetMarkerStyle(20);
	      h1tmpg->SetMinimum(0.0);
	      h1tmpg->SetMaximum(1.05);
	      h1tmpg->BayesDivide(h1num, h1den);
	      h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
	      h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
	      ztpdir->cd();
	      h1tmpg->Write();
	      delete h1tmpg;
	    }
	    mf.Write();

	    seffg = histdirmuztp + "/muZTP_eff_EFwrtL1"+var[k]+itmap->first;
	    snum = histdirmuztp + "/muZTP"+var[k]+"EFfired_"+itmap->first;
	    sden = histdirmuztp + "/muZTP"+var[k]+"L1fired_"+itmap->first;
	    stmpg = "muZTP_eff_EFwrtL1"+var[k]+itmap->first;

	    h1num = 0;
	    mf.get(snum, h1num);
	    if (!h1num) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	      }
	      continue;
	    }
	    h1den = 0;
	    mf.get(sden, h1den);
	    if (!h1den) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	      }
	      continue;
	    }

	    if(h1num && h1den){
	      h1tmpg = new TGraphAsymmErrors();
	      h1tmpg->SetName(stmpg);
	      h1tmpg->SetMarkerStyle(20);
	      h1tmpg->SetMinimum(0.0);
	      h1tmpg->SetMaximum(1.05);
	      h1tmpg->BayesDivide(h1num, h1den);
	      h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
	      h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
	      ztpdir->cd();
	      h1tmpg->Write();
	      delete h1tmpg;
	    }
	    mf.Write();

	    seffg = histdirmuztp + "/muZTP_eff_L2wrtL1"+var[k]+itmap->first;
	    snum = histdirmuztp + "/muZTP"+var[k]+"L2fired_"+itmap->first;
	    sden = histdirmuztp + "/muZTP"+var[k]+"L1fired_"+itmap->first;
	    stmpg = "muZTP_eff_L2wrtL1"+var[k]+itmap->first;

	    h1num = 0;
	    mf.get(snum, h1num);
	    if (!h1num) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	      }
	      continue;
	    }
	    h1den = 0;
	    mf.get(sden, h1den);
	    if (!h1den) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	      }
	      continue;
	    }

	    if(h1num && h1den){
	      h1tmpg = new TGraphAsymmErrors();
	      h1tmpg->SetName(stmpg);
	      h1tmpg->SetMarkerStyle(20);
	      h1tmpg->SetMinimum(0.0);
	      h1tmpg->SetMaximum(1.05);
	      h1tmpg->BayesDivide(h1num, h1den);
	      h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
	      h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
	      ztpdir->cd();
	      h1tmpg->Write();
	      delete h1tmpg;
	    }
	    mf.Write();

	    if(isefisochain){
	      seffg = histdirmuztp + "/muZTP_eff_EFIsowrtEF"+var[k]+itmap->first;
	      snum = histdirmuztp + "/muZTP"+var[k]+"EFIsofired_"+itmap->first;
	      sden = histdirmuztp + "/muZTP"+var[k]+"EFfired_"+itmap->first;
	      stmpg = "muZTP_eff_EFIsowrtEF"+var[k]+itmap->first;

	      h1num = 0;
	      mf.get(snum, h1num);
	      if (!h1num) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
		}
		continue;
	      }
	      h1den = 0;
	      mf.get(sden, h1den);
	      if (!h1den) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		}
		continue;
	      }

	      if(h1num && h1den){
		h1tmpg = new TGraphAsymmErrors();
		h1tmpg->SetName(stmpg);
		h1tmpg->SetMarkerStyle(20);
		h1tmpg->SetMinimum(0.0);
		h1tmpg->SetMaximum(1.05);
		h1tmpg->BayesDivide(h1num, h1den);
		h1tmpg->GetYaxis()->SetTitle("Efficiency");     				  
		h1tmpg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
		ztpdir->cd();
		h1tmpg->Write();
		delete h1tmpg;
	      }
	      mf.Write();
	    }
	  }//var
	}
	// mf.Write();
      } // procMuZTP


      {  
	TDirectory* efdir = mf.GetDirectory(eff_dir);
	if(!efdir){
	  if (fdbg) {
	    std::cerr<< "HLTMuonHistogramDivision: directory "<<eff_dir<<" not found"<<std::endl;
	  }
	  return;
	}

	// procChainDQA_HighPt
	// pp_v4
	//const int MAXARR = 3;
	//std::string charr[MAXARR] = {"mu36_tight", "mu24i_tight", "mu50_MSonly_barrel_tight"};
	//std::string monarr[MAXARR] = {"_EFmuon", "_EFmuon", "_MuonEFSA"};
	//std::string monL2arr[MAXARR] = {"_L2MuonSA", "_L2MuonSA", "_L2MuonSA"};
	//bool isBarrelMon[MAXARR] = {false, false, true}; // enable MSonly
	//bool isMSbMon[MAXARR] = {true, false, false}; // Skip isol and MSonly
	//bool monL1[MAXARR] = {true, true, false}; // Skip MSonly
	//bool isefIsolation[MAXARR] = {false, true, false}; // EF isolation  add by Yuan

	// MAM 
	const int MAXARR = 6;
	std::string charr[MAXARR] = {"muChain1", "muChain2", "muChainEFiso1", "muChainEFiso2","muChainMSonly1","muChainMSonly2"};
	std::string monarr[MAXARR] = {"_EFmuon", "_EFmuon", "_EFmuon", "_EFmuon", "_MuonEFSA", "_MuonEFSA"};
	std::string monL2arr[MAXARR] = {"_L2MuonSA", "_L2MuonSA", "_L2MuonSA", "_L2MuonSA", "_L2MuonSA", "_L2MuonSA"};
	bool isBarrelMon[MAXARR] = {false, false, false, false, true, true}; // enable MSonly
	bool isMSbMon[MAXARR] = {true, true,false, false,false, false}; // Skip isol and MSonly
	bool monL1[MAXARR] = {true, true, true, true, false, false}; // Skip MSonly
	bool isefIsolation[MAXARR] = {false, false, true, true, false, false}; // EF isolation  add by Yuan

	// I need to prepare maps ... mu20_MG -> MuGirlEF etc.
	for (int ialg = 0; ialg < MAXARR; ialg++) {
	  std::string chainName = charr[ialg];
	  std::string MoniAlg = monarr[ialg];
	  std::string MoniL2Alg = monL2arr[ialg];

	  /* 1. Picking up ztp graph */
	  TString hdirztp = muon_dir + "MuZTP/" + chainName + "/";
	  TString histZtpNum, histZtpDen;
	  if (!isBarrelMon[ialg]) {
	    histZtpNum = hdirztp + "muZTP_Pt_4bins_EFfired_" + chainName;
	    histZtpDen = hdirztp + "muZTP_Pt_4bins_L1fired_" + chainName;
	  } else {
	    histZtpNum = hdirztp + "muZTP_Pt_B_4bins_EFfired_" + chainName;
	    histZtpDen = hdirztp + "muZTP_Pt_B_4bins_L1fired_" + chainName;
	  }
	  if(isefIsolation[ialg]) histZtpNum = hdirztp + "muZTP_Pt_4bins_EFIsofired_" + chainName;  // add by Yuan
	  TString histZtpEff = eff_dir + chainName + "_highpt3bins_effwrtL1";

	  h1num = 0; mf.get(histZtpNum, h1num);
	  if (!h1num) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histZtpNum << std::endl;
	    }
	    continue;
	  }
	  h1den = 0; mf.get(histZtpDen, h1den);
	  if (!h1den) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histZtpDen << std::endl;
	    }
	    continue;
	  }
	  h1eff = 0; mf.get(histZtpEff, h1eff);
	  if (!h1eff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histZtpEff << std::endl;
	    }
	    continue;
	  }

	  /* 2. Filling summary histogram from ZTP values */
	  if (h1num && h1den && h1eff) {
	    for (int ibin = 2; ibin <= 3; ibin++) {
	      // at the moment it is not correct if we run the algorithm # 4: mu40_MSonly_barrel .
	      double sumeff, sumerr;
	      double sumn = h1num->Integral(ibin, ibin); ////
	      if(isBarrelMon[ialg] || isMSbMon[ialg]) sumn = h1num->Integral(ibin+1, ibin+1);
	      double sumd = h1den->Integral(ibin, ibin); ////
	      if(isBarrelMon[ialg] || isMSbMon[ialg]) sumd = h1den->Integral(ibin+1, ibin+1);
	      if (sumd == 0.) {
		sumeff = 0.;
		sumerr = 0.;
	      } else {
		sumeff = (double)sumn / (double) sumd;
		sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	      }
	      h1eff->GetYaxis()->SetTitle("Efficiency");     				  
	      h1eff->SetBinContent(ibin-1, sumeff);  ////
	      h1eff->SetBinError(ibin-1, sumerr);    ////
	      h1eff->SetMinimum(0.0);
	      h1eff->SetMaximum(1.05);
	    }
	  }

	  /* 3. Picking up chainDQ MSonly graph   abandoned !!!*/
	  /* EF efficiency wrt L1, as for the ztp graph = overall HLT efficiency wrt L1: not possible, wrt offline 
	     if (isMSbMon[ialg]) {  // skip muIso and MSonly !!!
	     TString histChNum = nd_dir + chainName + MSchainName + MoniAlg + "_Turn_On_Curve_Numerator";
	     TString histChDen = nd_dir + chainName + MSchainName + MoniL2Alg + "_Turn_On_Curve_wrt_L1_Denominator";

	     h1num = 0; mf.get(histChNum, h1num);
	     if (!h1num) {
	     if (fdbg) {
	     std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histZtpNum << std::endl;
	     }
	     continue;
	     }
	     h1den = 0; mf.get(histChDen, h1den);
	     if (!h1den) {
	     if (fdbg) {
	     std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histZtpDen << std::endl;
	     }
	     continue;
	     }

	     if (h1num && h1den) {
	  // Integrate 100-300 GeV
	  double sumeff, sumerr;
	  double sumn = h1num->Integral(21, 28);  ////
	  double sumd = h1den->Integral(21, 28);  ////
	  if (sumd == 0.) {
	  sumeff = 0.;
	  sumerr = 0.;
	  } else {
	  sumeff = (double)sumn / (double) sumd;
	  sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	  }
	  h1eff->SetBinContent(3, sumeff);   ////
	  h1eff->SetBinError(3, sumerr);     ////
	  }
	  }
	  */
	  efdir->cd();
	  h1eff->Write("",TObject::kOverwrite);

	  if (monL1[ialg]) { // MUST skip muGirl, muIso and MSonly as histograms are not defined!!!
	    TString hdirztp = muon_dir + "MuZTP/" + chainName + "/";
	    TString histZtpNumB = hdirztp + "muZTP_Pt_B_L1fired_" + chainName;
	    TString histZtpDenB = hdirztp + "muZTP_Pt_B_" + chainName;
	    TString histZtpNumE = hdirztp + "muZTP_Pt_EC_L1fired_" + chainName;
	    TString histZtpDenE = hdirztp + "muZTP_Pt_EC_" + chainName;
	    TString histZtpL1sum = eff_dir + chainName + "_highptL1plateau_wrtOffline";

	    TH1F *h1numb = 0; mf.get(histZtpNumB, h1numb);
	    if (!h1numb) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histZtpNumB << std::endl;
	      }
	      continue;
	    }
	    TH1F *h1nume = 0; mf.get(histZtpNumE, h1nume);
	    if (!h1nume) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histZtpNumE << std::endl;
	      }
	      continue;
	    }
	    TH1F *h1denb = 0; mf.get(histZtpDenB, h1denb);
	    if (!h1denb) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histZtpDenB << std::endl;
	      }
	      continue;
	    }
	    TH1F *h1dene = 0; mf.get(histZtpDenE, h1dene);
	    if (!h1dene) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histZtpDenE << std::endl;
	      }
	      continue;
	    }
	    TH1F *h1sumL = 0; mf.get(histZtpL1sum, h1sumL);
	    if (!h1sumL) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histZtpL1sum << std::endl;
	      }
	      continue;
	    }

	    double sumeff, sumerr;
	    double sumn = h1numb->Integral(13, 25); // 12-25 GeV
	    if(HI_pp_key)sumn = h1numb->Integral(13, 20); // 60-100 GeV
	    //if(HI_pp_key)sumn = h1numb->Integral(7, 10); // 30-50 GeV
	    double sumd = h1denb->Integral(13, 25);
	    if(HI_pp_key)sumd = h1denb->Integral(13, 20);
	    //if(HI_pp_key)sumd = h1denb->Integral(7, 10);
	    if (sumd == 0.) {
	      sumeff = 0.;
	      sumerr = 0.;
	    } else {
	      sumeff = (double)sumn / (double) sumd;
	      sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	    }
	    h1sumL->SetBinContent(1, sumeff);
	    h1sumL->SetBinError(1, sumerr);

	    sumn = h1nume->Integral(13, 25);
	    if(HI_pp_key)sumn = h1numb->Integral(13, 20); // 60-100 GeV
	    //if(HI_pp_key)sumn = h1numb->Integral(7, 10); // 30-50 GeV
	    sumd = h1dene->Integral(13, 25);
	    if(HI_pp_key)sumd = h1denb->Integral(13, 20);
	    //if(HI_pp_key)sumd = h1denb->Integral(7, 10);
	    if (sumd == 0.) {
	      sumeff = 0.;
	      sumerr = 0.;
	    } else {
	      sumeff = (double)sumn / (double) sumd;
	      sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	    }
	    h1sumL->GetYaxis()->SetTitle("Efficiency");     				  
	    h1sumL->SetBinContent(2, sumeff);
	    h1sumL->SetBinError(2, sumerr);
	    h1sumL->SetMinimum(0.0);
	    h1sumL->SetMaximum(1.05);
	    efdir->cd();
	    h1sumL->Write("",TObject::kOverwrite);
	  }
	}
	mf.Write();
      } //procChainDQA_HighPt
      //===End HLTMuonw

      // ******************************************************//
      // *********************  generic ***********************//
      // ******************************************************//
      TString monalg[3]={"_L2MuonSA", "_MuComb", "_EFmuon"};
      TString wrtalg[3]={"_L1", "_L2MuonSA", "_MuComb"};
      TString numer, denom, effi;
      TString histdireff = eff_dir;

      TDirectory* efdir = mf.GetDirectory(eff_dir);
      if(!efdir){
	if (fdbg) {
	  std::cerr<< "HLTMuonHistogramDivision: directory "<<eff_dir<<" not found"<<std::endl;
	}
	return;
      }

      // processing Iso histograms in the same manner
      chainsGeneric.insert(chainsGeneric.end(), chainsEFiso.begin(), chainsEFiso.end());  

      for( unsigned int i=0 ; i < chainsGeneric.size() ; i++ ){
	TString chainName = chainsGeneric.at(i);
	if (fdbg) {
	  std::cout << "proc generic " << chainName << std::endl;
	}

	//wrt CB muon && Upstream trigger
	for( int alg = 0 ; alg < 3 ; alg++ ){
	  denom = chainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	  numer = chainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	  effi  = chainName + monalg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	  HLTMuonHDiv(mf, histdireff, numer, denom, effi, "_Fit");

	  denom = chainName + MSchainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	  numer = chainName + MSchainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	  effi  = chainName + MSchainName + monalg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	  HLTMuonHDiv(mf, histdireff, numer, denom, effi, "_Fit");

	  // Summary all - removed

	  denom = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	  numer = chainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	  effi = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	  HLTMuonHDiv(mf, histdireff, numer, denom, effi, "_Fit");


	  denom = chainName + MSchainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	  numer = chainName + MSchainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	  effi = chainName + MSchainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	  HLTMuonHDiv(mf, histdireff, numer, denom, effi, "_Fit");

	  //  for ES ------------------------------------------------------------------------------------
	  for (int i = 0; i <= maxESbr; i++) {

	    if(!CB_mon_ESbr[i]) continue;
	    if (0 == alg) {
	      // L1 efficiency: new for 2011 HI runs and afterward
	      // only division once since it is "the zero-th" algorithm
	      denom = chainName + triggerES[i] + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	      numer = chainName + triggerES[i] + "_L2MuonSA" + "_Turn_On_Curve_wrt" + "_L1" + "_Denominator";
	      effi  = chainName + triggerES[i] + "_L1" + "_Turn_On_Curve_wrt_MuidCB";
	      HLTMuonHDiv(mf, histdireff, numer, denom, effi, "_Fit");

	      // Need to implement barrel and endcap ...
	      for (int be = 0; be < 2; be++) {
		denom = chainName + triggerES[i] + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
		numer = chainName + triggerES[i] + "_L2MuonSA" + "_Turn_On_Curve_wrt" + "_L1" + bestr[be] + "_Denominator";
		effi  = chainName + triggerES[i] + "_L1" + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
		HLTMuonHDiv(mf, histdireff, numer, denom, effi, "_Fit");

		h1num = 0;
		mf.get(nd_dir + numer, h1num);
		if (!h1num) {
		  if (fdbg) {
		    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
		  }
		  continue;
		}
		h1den = 0;
		mf.get(nd_dir + denom, h1den);
		if (!h1den) {
		  if (fdbg) {
		    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		  }
		  continue;
		}

		if (ESINDEP == i) {
		  // integrating over and fill in a summary histogram
		  double sumeff, sumerr;
		  double sumn = h1num->Integral(iSTDL, iSTDH); // 60-100 GeV
		  double sumd = h1den->Integral(iSTDL, iSTDH);
		  if (sumd == 0.) {
		    sumeff = 0.;
		    sumerr = 0.;
		  } else {
		    sumeff = (double)sumn / (double)sumd;
		    sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		  }

		  TString s = histdireff + chainName + "_L1plateau_wrtOffline_by_ESindep";
		  TH1F *h1effL1 = 0;
		  mf.get(s, h1effL1);
		  if (!h1effL1) {
		    if (fdbg) {
		      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		    }
		    continue;
		  }
		  h1effL1->SetBinContent(be+1, sumeff);
		  h1effL1->SetBinError(be+1, sumerr);
		  h1eff->SetMinimum(0.0);
		  h1eff->SetMaximum(1.05);
		  efdir->cd();
		  h1effL1->Write("",TObject::kOverwrite);
		}
	      }	  
	    }

	    denom = chainName + triggerES[i] + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	    numer = chainName + triggerES[i] + monalg[alg] + "_Turn_On_Curve_Numerator";
	    effi  = chainName + triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	    HLTMuonHDiv(mf, histdireff, numer, denom, effi, "_Fit");

	    // Summary ESid and ESindep
	    h1num = 0;
	    mf.get(nd_dir + numer, h1num);
	    if (!h1num) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	      }
	      continue;
	    }
	    h1den = 0;
	    mf.get(nd_dir + denom, h1den);
	    if (!h1den) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	      }
	      continue;
	    }
	    if (0 == alg || 1 == alg || 2 == alg) {
	      // if (ESID == i || ESINDEP == i) {
	      double sumd, sumn, sumeff, sumerr;
	      sumn = h1num->Integral(iSTDL, iSTDH);
	      sumd = h1den->Integral(iSTDL, iSTDH);
	      if (sumd == 0.) {
		sumeff = 0.;
		sumerr = 0.;
	      } else {
		sumeff = (double)sumn / (double)sumd;
		sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	      }
	      int iholx = -1;
	      if (0 == alg) {
		iholx = static_cast<int>(iL2MuonSA);
	      } else if (1 == alg) {
		iholx = static_cast<int>(iMuComb);
	      } else if (2 == alg) {
		iholx = static_cast<int>(iEFCB);
	      }

	      TString s = histdireff + chainName + "_highpt_effsummary_by" + triggerES[i];
	      mf.get(s, h1effsum);
	      if (!h1effsum) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< s << std::endl;
		}
		continue;
	      }
              h1effsum->GetYaxis()->SetTitleOffset(1.3);
	      h1effsum->SetBinContent(iholx+1, sumeff);
	      h1effsum->SetBinError(iholx+1, sumerr);
	      h1effsum->SetMinimum(0.0);
	      h1effsum->SetMaximum(1.05);
	      efdir->cd();
	      h1effsum->Write("",TObject::kOverwrite);
	    }

	    denom = chainName + triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	    numer = chainName + triggerES[i] + monalg[alg] + "_Turn_On_Curve_Numerator";
	    effi = chainName + triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	    HLTMuonHDiv(mf, histdireff, numer, denom, effi, "_Fit");

	    }
	    //  for ES : END ------------------------------------------------------------------------------

	    // Barrel/Endcap
	    if (0 == alg || 1 == alg || 2 == alg) {
	      for (int be = 0; be < 2; be++) {
		denom = chainName + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
		numer = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
		effi  = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
		HLTMuonHDiv(mf, histdireff, numer, denom, effi, "_Fit");
	      }

	      for (int be = 0; be < 2; be++) {
		denom = chainName + MSchainName + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
		numer = chainName + MSchainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
		effi  = chainName + MSchainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
		HLTMuonHDiv(mf, histdireff, numer, denom, effi, "_Fit");
	      }

	      for (int be = 0; be < 2; be++) {
		denom = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be] + "_Denominator";
		numer = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
		effi = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be];
		HLTMuonHDiv(mf, histdireff, numer, denom, effi, "_Fit");
	      }


	      for (int be = 0; be < 2; be++) {
		denom = chainName + MSchainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be] + "_Denominator";
		numer = chainName + MSchainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
		effi = chainName + MSchainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be];
		HLTMuonHDiv(mf, histdireff, numer, denom, effi, "_Fit");
	      }

	    }

	  }
	}      
      }//while (now just open bracket: 110728
    }//MonitoringFile::HLTMuonHistogramDivision

    void MonitoringFile::HLTMuonHDiv(PostProcessorFileWrapper& mf,
	TString sdir, TString snum, TString sden, TString seff, TString seffg)
    {
      TH1F* h1tmpf(0);
      TH1F* h1num(0);
      TH1F* h1den(0);
      TGraphAsymmErrors* h1tmpfg = new TGraphAsymmErrors();;
      TString stmp = seff + seffg;
      h1num = 0;
      mf.get(sdir + "NumDenom/" + snum, h1num);
      if (!h1num) {
	if (fdbg) {
	  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< snum << std::endl;
	}
	return;
      }
      h1den = 0;
      mf.get(sdir + "NumDenom/" + sden, h1den);
      if (!h1den) {
	if (fdbg) {
	  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
	}
	return;
      }

      TDirectory* dir = mf.GetDirectory(sdir);

      if(h1num && h1den){
	h1tmpf = (TH1F*)h1den->Clone();
	h1tmpf->SetName(stmp);                          				
	h1tmpf->SetTitle(stmp);                         			  
	h1tmpf->GetYaxis()->SetTitle("Efficiency");     				  
	h1tmpf->Reset();                                				  
	h1tmpf->Divide(h1num, h1den, 1., 1., "B");      				  
	h1tmpf->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
	h1tmpf->SetMinimum(0.0);
	h1tmpfg->SetMaximum(1.05);
	h1tmpf->SetName(stmp);
	dir->cd();                                    				  
	h1tmpf->Write();                                				  
	h1tmpfg->SetMarkerStyle(20);
	h1tmpfg->SetMinimum(0.0);
	h1tmpfg->SetMaximum(1.05);
	h1tmpfg->BayesDivide(h1num, h1den);
	h1tmpfg->GetYaxis()->SetTitle("Efficiency");     				  
	h1tmpfg->GetXaxis()->SetTitle(h1den->GetXaxis()->GetTitle());     				  
	dir->cd();
	h1tmpfg->SetName(stmp);
        h1tmpfg->Write();
	delete h1tmpfg;
      }
    }

  }//namespace
