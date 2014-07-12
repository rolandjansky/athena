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
#include "TGraphAsymmErrors.h"


namespace dqutils {

  static const bool fdbg = false;
  // static const bool fdbg = true;

  void
  MonitoringFile::HLTMuonHistogramDivision(std::string inFilename, TString& run_dir)
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
    /* TIter next_run (mf.GetListOfKeys());
       TKey* key_run(0);
       while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) { //== the while commented out at the end
      TObject* obj_run = key_run->ReadObj();
      TDirectory* tdir_run = dynamic_cast<TDirectory*>( obj_run );
      if (tdir_run ==0 ) {
	delete obj_run;
	continue;
      } // no deleting
      run_dir = tdir_run->GetName();
      if (!run_dir.Contains("run") )  {
	delete obj_run;
	continue;
	} // no deleting */

    {
      run_dir = dir0->GetName();
      if (fdbg) {
	std::cout << "HLTMuon: run directory is " << run_dir << std::endl;
      }
      std::string run_dir2 = run_dir.Data();
      int run_number = atoi( (run_dir2.substr(4, run_dir2.size()-4 )).c_str() );
      run_number=run_number;

      //===HLTMuon
      TString muon_dir = run_dir + "/HLT/MuonMon/";

      TString cm_dir = muon_dir + "Common/";
      TString mf_dir = muon_dir + "muFast/";
      TString mc_dir = muon_dir + "muComb/";
      TString mi_dir = muon_dir + "muIso/";
      TString tm_dir = muon_dir + "TileMu/";
      TString ef_dir = muon_dir + "MuonEF/";
      TString mg_dir = muon_dir + "MuGirl/";
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

      TH1F* h1eff(0);
      TH1F* h1num(0);
      TH1F* h1den(0);
      TH1F* h1sumeff(0); // new YY
      TGraphAsymmErrors* geff(0);

      //==Efficiency
      //  muFast efficiency
      TDirectory* dir = mf.GetDirectory(eff_dir);
      if(!dir){
        std::cerr<< "HLTMuonHistogramDivision: directory "<<eff_dir<<" not found"<<std::endl;
        return;
      }

      std::vector<TString> effnames;
      effnames.push_back("muFast_effi_toRecMuonCB_pt");
      effnames.push_back("muFast_effi_toRecMuonCB_pt_barrel");
      effnames.push_back("muFast_effi_toRecMuonCB_pt_endcap");
      effnames.push_back("muFast_effi_toRecMuonCB_eta");
      effnames.push_back("muFast_effi_toRecMuonCB_phi");

      for( std::vector<TString>::iterator it = effnames.begin(); it != effnames.end(); it++ ){
        seff = eff_dir + (*it);
        snum = mf_dir + (*it) + "_numer";
        sden = mf_dir + (*it) + "_denom";
	
	h1eff = 0;
	mf.get(seff, h1eff);
	if (!h1eff) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	  }
	  continue;
	}
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
	
        if( h1eff && h1num && h1den){
          h1eff->Reset();
          h1eff->Divide(h1num, h1den, 1., 1., "B");

          dir->cd();
          h1eff->Write("",TObject::kOverwrite);
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

	h1eff = 0;
	mf.get(seff, h1eff);
	if (!h1eff) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	  }
	  continue;
	}
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
	
        if( h1eff && h1num && h1den){
          h1eff->Reset();
          h1eff->Divide(h1num, h1den, 1., 1., "B");

          dir->cd();
          h1eff->Write("",TObject::kOverwrite);
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

	h1eff = 0;
	mf.get(seff, h1eff);
	if (!h1eff) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	  }
	  continue;
	}
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
	
        if( h1eff && h1num && h1den){
	  h1eff->Reset();
          h1eff->Divide(h1num, h1den, 1., 1., "B");

          dir->cd();
          h1eff->Write("",TObject::kOverwrite);
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

	h1eff = 0;
	mf.get(seff, h1eff);
	if (!h1eff) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	  }
	  continue;
	}
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
	
        if( h1eff && h1num && h1den){
          h1eff->Reset();
          h1eff->Divide(h1num, h1den, 1., 1., "B");

          dir->cd();
          h1eff->Write("",TObject::kOverwrite);
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

	h1eff = 0;
	mf.get(seff, h1eff);
	if (!h1eff) {
	  if (fdbg) {
	    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	  }
	  continue;
	}
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
	
        if( h1eff && h1num && h1den){
          h1eff->Reset();
          h1eff->Divide(h1num, h1den, 1., 1., "B");

          dir->cd();
          h1eff->Write("",TObject::kOverwrite);
        }

      }//effnames
      mf.Write();

      //  MuGirl efficiency
      //==Turn on
      std::vector<TString> m_chainsMSonly;
      std::vector<TString> m_chainsStandard;
      std::vector<TString> m_chainsMG;
      std::vector<TString> m_chainsMI;
      std::vector<TString> m_chainsGeneric;
      std::vector<TString> m_chainsEFiso;
      std::vector<TString> m_chainsEFFS;

      // Generic (EFsuper etc.)
      m_chainsGeneric.push_back("mu36_tight");             // v4 primary
      
      // Generic (Isolated muons)
      m_chainsEFiso.push_back("mu24i_tight")  ;            // v4 primary
      
      // MSonly
      m_chainsMSonly.push_back("mu50_MSonly_barrel_tight");    // v4 primary

      // EFFS triggers (L. Yuan)
      m_chainsEFFS.push_back("mu18_tight_mu8_EFFS");

      enum indexINDEP { INDORTH, INDEGAMMA, INDMET, INDJET, INDTAU, INDMBIAS };
      TString m_trigger[INDMBIAS + 1];

      m_trigger[INDORTH] = "Orthog";//EGamma + Tau + Jet + MET
      m_trigger[INDEGAMMA] = "EGamma";
      m_trigger[INDMET] = "MET";
      m_trigger[INDJET] = "Jet";

      int m_maxindep = 0; // YY 20.01.2012

      // YY added:
      TString m_ESchainName = "_ES";

      TString bestr[2] = {"_Barrel", "_Endcap"};

      // made it as enum: 20.1.2012
      std::string m_triggerES[7] = {"_ESstd", "_EStag", "_ESid", "_ESindep", "_ESHIL1", "_ESHIid", "_ESHIindep"};
      enum indexES { ESSTD, ESTAG, ESID, ESINDEP, ESHIL1, ESHIID, ESHIINDEP };

      int m_maxESbr = ESINDEP;

      bool CB_mon_ESbr[ESHIINDEP + 1];
      bool MS_mon_ESbr[ESHIINDEP + 1];

      CB_mon_ESbr[ESSTD] = 0;
      CB_mon_ESbr[ESTAG] = 0;
      CB_mon_ESbr[ESID] = 1;
      CB_mon_ESbr[ESINDEP] = 1;
      CB_mon_ESbr[ESHIL1] = 0;
      CB_mon_ESbr[ESHIID] = 0;
      CB_mon_ESbr[ESHIINDEP] = 0;

      MS_mon_ESbr[ESSTD] = 0;
      MS_mon_ESbr[ESTAG] = 1;
      MS_mon_ESbr[ESID] = 0;
      MS_mon_ESbr[ESINDEP] = 1;
      MS_mon_ESbr[ESHIL1] = 0;
      MS_mon_ESbr[ESHIID] = 0;
      MS_mon_ESbr[ESHIINDEP] = 0;

      std::vector<std::string> m_vectkwd;
      
      m_vectkwd.push_back(m_triggerES[ESTAG]);
      m_vectkwd.push_back(m_triggerES[ESID]);
      m_vectkwd.push_back("_Jet");
      m_vectkwd.push_back("_all");

      TString m_hptName = "_hpt";
      TString m_MSchainName = "_MSb";

      // YY: pt range.
      int iSTDL = 91;  // 40 GeV
      int iSTDH = 112; // 80 GeV
      int iMSL = 105;  // 60 GeV
      int iMSH = 120;  // 100 GeV

      // YY added:
      enum ieffAlgo {
        iMuFast = 0,   // StdAlgo
        iMuComb = 1,   // StdAlgo
        iEFCB   = 2,   // StdAlgo
        iMuGirl = 3,   // StdAlgo

        iEFSA   = 1    // MSAlgo
      };

      //  Standard Chains
      //TString m_alg[5] = {"_MuFast", "_MuComb", "_MuonEFMS", "_MuonEFSA", "_MuonEFCB"};
      //TString m_wrtalg[5] = {"_L1", "_MuFast", "_MuComb", "_MuComb", "_MuComb"};


	// ******************************************************//
	// start the code add by Yuan //
      TString FS_pre_trigger = "mu18it_tight";
      for(unsigned int i=0; i<m_chainsEFFS.size(); i++){
	TString chainName = m_chainsEFFS.at(i);
	
	TString hists_str[9] = {chainName + "_tagMu18it" + "_Turn_On_Curve_wrt_probe_MuidCB",
				chainName + "_tagMu18it" + "_Turn_On_Curve_wrt_probe_MuidCB_Barrel",
				chainName + "_tagMu18it" + "_Turn_On_Curve_wrt_probe_MuidCB_Endcap",
				chainName + "_tagMu18it_mu0_15" + "_Turn_On_Curve_wrt_probe_MuidCB",
				chainName + "_tagMu18it_mu15_20" + "_Turn_On_Curve_wrt_probe_MuidCB",
				chainName + "_tagMu18it_mu20" + "_Turn_On_Curve_wrt_probe_MuidCB",
				chainName + "_Turn_On_Curve_wrt_subleading_MuidCB",
				FS_pre_trigger + "_dimuonTP" + "_Turn_On_Curve_wrt_probe_MuidCB",
				FS_pre_trigger + "_dimuonTP" + "_Turn_On_Curve_wrt_L1_probe_MuidCB",
				};


	bool for_mydebug = true;
	for(int iROI = 0; iROI < 9; iROI++){
	    sden = nd_dir + hists_str[iROI] + "_Denominator";
	    snum = nd_dir + hists_str[iROI] + "_Numerator";
	    seff = eff_dir + hists_str[iROI];
	    seffg = seff + "_Fit";

	    h1eff = 0;
	    mf.get(seff, h1eff);
	    if (!h1eff) {
	      if (for_mydebug) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	      }
	      continue;
	    }
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
            geff = (TGraphAsymmErrors*) mf.Get(seffg);
            if (!geff) {
              if (for_mydebug) {
                std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
              }
              continue;
            }


	    if(h1eff && h1num && h1den){
	      h1eff->Reset();
	      h1eff->Divide(h1num, h1den, 1., 1., "B");
	      
	      dir->cd();
	      h1eff->Write("",TObject::kOverwrite);
	      
              if (geff) {  // YY added 20.04.10
                geff->SetMarkerStyle(20);
                geff->BayesDivide(h1num, h1den);

                // saving summary TGraph
                dir->cd();
                geff->Write("", TObject::kOverwrite);
              }
	    }

	} // end the loop on individual turn-on curves

  	    TString L1_TP_str = FS_pre_trigger + "_dimuonTP_L1" + "_Turn_On_Curve_wrt_probe_MuidCB";
	    sden = nd_dir + FS_pre_trigger + "_dimuonTP" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";
	    snum = nd_dir + FS_pre_trigger + "_dimuonTP" + "_Turn_On_Curve_wrt_L1_probe_MuidCB" + "_Denominator";
	    seff = eff_dir + L1_TP_str;
	    seffg = seff + "_Fit";

	    h1eff = 0;
	    mf.get(seff, h1eff);
	    if (!h1eff) {
	      if (for_mydebug) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	      }
	      continue;
	    }
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
            geff = (TGraphAsymmErrors*) mf.Get(seffg);
            if (!geff) {
              if (for_mydebug) {
                std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
              }
              continue;
            }


	    if(h1eff && h1num && h1den){
	      h1eff->Reset();
	      h1eff->Divide(h1num, h1den, 1., 1., "B");
	      
	      dir->cd();
	      h1eff->Write("",TObject::kOverwrite);
	      
              if (geff) {  // YY added 20.04.10
                geff->SetMarkerStyle(20);
                geff->BayesDivide(h1num, h1den);

                // saving summary TGraph
                dir->cd();
                geff->Write("", TObject::kOverwrite);
              }
	    }

	//**** summargy plot **********//

	      TString histNumB = nd_dir + chainName +"_tagMu18it_Turn_On_Curve_wrt_probe_MuidCB_Barrel_Numerator";
	      TString histDenB = nd_dir + chainName +"_tagMu18it_Turn_On_Curve_wrt_probe_MuidCB_Barrel_Denominator"; 
	      TString histNumE = nd_dir + chainName +"_tagMu18it_Turn_On_Curve_wrt_probe_MuidCB_Endcap_Numerator"; 
	      TString histDenE = nd_dir + chainName +"_tagMu18it_Turn_On_Curve_wrt_probe_MuidCB_Endcap_Denominator"; 
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
	      if (!for_mydebug) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histL1sum << std::endl;
		}
		continue;
	      }

	      int m_iSTDL = 39;
	      int m_iSTDH = 120;
	      double sumeff, sumerr;
	      double sumn = h1numb->Integral(m_iSTDL, m_iSTDH); // 10-100 GeV
	      double sumd = h1denb->Integral(m_iSTDL, m_iSTDH);
	      if (sumd == 0.) {
		sumeff = 0.;
		sumerr = 0.;
	      } else {
		sumeff = (double)sumn / (double) sumd;
                sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
              }

              h1sumL->SetBinContent(1, sumeff);
              h1sumL->SetBinError(1, sumerr);

              sumn = h1nume->Integral(m_iSTDL, m_iSTDH);
              sumd = h1dene->Integral(m_iSTDL, m_iSTDH);
              if (sumd == 0.) {
                sumeff = 0.;
                sumerr = 0.;
              } else {
                sumeff = (double)sumn / (double) sumd;
                sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
              }
              h1sumL->SetBinContent(2, sumeff);
              h1sumL->SetBinError(2, sumerr);

	      dir->cd();
              h1sumL->Write("",TObject::kOverwrite);

	      TString histNum_mu0_15 = nd_dir + chainName +"_tagMu18it_mu0_15_Turn_On_Curve_wrt_probe_MuidCB_Numerator";
	      TString histDen_mu0_15 = nd_dir + chainName +"_tagMu18it_mu0_15_Turn_On_Curve_wrt_probe_MuidCB_Denominator"; 
	      TString histNum_mu15_20 = nd_dir + chainName +"_tagMu18it_mu15_20_Turn_On_Curve_wrt_probe_MuidCB_Numerator"; 
	      TString histDen_mu15_20 = nd_dir + chainName +"_tagMu18it_mu15_20_Turn_On_Curve_wrt_probe_MuidCB_Denominator"; 
	      TString histNum_mu20 = nd_dir + chainName +"_tagMu18it_mu20_Turn_On_Curve_wrt_probe_MuidCB_Numerator"; 
	      TString histDen_mu20 = nd_dir + chainName +"_tagMu18it_mu20_Turn_On_Curve_wrt_probe_MuidCB_Denominator"; 
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
	      if (!for_mydebug) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< histEFsum_mu << std::endl;
		}
		continue;
	      }

	      sumn = h1num_mu0_15->Integral(m_iSTDL, m_iSTDH); // 10-100 GeV
	      sumd = h1den_mu0_15->Integral(m_iSTDL, m_iSTDH);
	      if (sumd == 0.) {
		sumeff = 0.;
		sumerr = 0.;
	      } else {
		sumeff = (double)sumn / (double) sumd;
                sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
              }

              h1sum_mu->SetBinContent(1, sumeff);
              h1sum_mu->SetBinError(1, sumerr);

              sumn = h1num_mu15_20->Integral(m_iSTDL, m_iSTDH);
              sumd = h1den_mu15_20->Integral(m_iSTDL, m_iSTDH);
              if (sumd == 0.) {
                sumeff = 0.;
                sumerr = 0.;
              } else {
                sumeff = (double)sumn / (double) sumd;
                sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
              }
              h1sum_mu->SetBinContent(2, sumeff);
              h1sum_mu->SetBinError(2, sumerr);

              sumn = h1num_mu20->Integral(m_iSTDL, m_iSTDH);
              sumd = h1den_mu20->Integral(m_iSTDL, m_iSTDH);
              if (sumd == 0.) {
                sumeff = 0.;
                sumerr = 0.;
              } else {
                sumeff = (double)sumn / (double) sumd;
                sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
              }
              h1sum_mu->SetBinContent(3, sumeff);
              h1sum_mu->SetBinError(3, sumerr);

	      dir->cd();
              h1sum_mu->Write("",TObject::kOverwrite);


	    mf.Write();
	}
	//  end of the code add by Yuan //
	// ******************************************************//


      // ******************************************************//
      // *********************  MuGirl  ***********************//
      // ******************************************************//
      for (unsigned int i=0 ; i < m_chainsMG.size() ; i++) {
        TString chainName = m_chainsMG.at(i);
	
	std::string chainName_std = (std::string)chainName;
	int nnn = chainName_std.find("_MG");
	if (0 == nnn) {
	  if (fdbg) {
	    std::cerr << "ChainName does not look like muGirl chain" << chainName << std::endl;
	  }
	} else {
	  chainName_std.replace(nnn, 3, "");
	}

	// MuGirl: begin
	TString m_alg[2] = {"_MuGirlL2", "_MuGirlEF"};
	TString m_wrtalg[2] = {"_L1", "_MuGirlL2"};

	// wrt CB Muon && independent trigger
	for( int trg = 0 ; trg < m_maxindep ; trg++ ){
	  sden = nd_dir + chainName + "_Turn_On_Curve_wrt_MuidCB" + m_trigger[trg] + "_Triggered_Denominator";
	  
	  for( int alg=0; alg < 2; alg++){
	    snum = nd_dir + chainName + m_alg[alg] + "_Turn_On_Curve_wrt_MuidCB" + m_trigger[trg] + "_Triggered_Numerator";
	    seff = eff_dir + chainName + m_alg[alg] + "_Turn_On_Curve_wrt_MuidCB" + m_trigger[trg] + "_Triggered";
	    seffg = seff + "_Fit"; // YY added 20.04.10
	    
	    h1eff = 0;
	    mf.get(seff, h1eff);
	    if (!h1eff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	      }
	      continue;
	    }
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
	    geff = (TGraphAsymmErrors*) mf.Get(seffg);
	    if (!geff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	      }
	      continue;
	    }
	    
	    if( h1eff && h1num && h1den){
	      h1eff->Reset();
	      h1eff->Divide(h1num, h1den, 1., 1., "B");

	      dir->cd();
	      h1eff->Write("",TObject::kOverwrite);

	      if (geff) {  // YY added 20.04.10
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);

		// saving summary TGraph
		dir->cd();
		geff->Write("", TObject::kOverwrite);
	      }
	      if (1 == alg) {  // MuGirlEF
		if (3 == trg) { // jet
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
		  int iholx = static_cast<int>(iMuGirl);
		    
		    
		  TString s = eff_dir + chainName_std + "_highpt_effsummary_by" + m_vectkwd.at(2);
		  // std::cout << "hist summary: " << s << " n: " << sumn << " d: " << sumd << " eff: " << sumeff << " err: " << sumerr << std::endl;
		  mf.get(s, h1sumeff);
		  if (!h1sumeff) {
		    if (fdbg) {
		      std::cerr << "HLTMuon PostProcessing: no such histogram!! " << s << std::endl;
		    }
		    continue;
		  }
		  h1sumeff->SetBinContent(iholx+1, sumeff);
		  h1sumeff->SetBinError(iholx+1, sumerr);
		  // saving
		  dir->cd();
		  h1sumeff->Write("", TObject::kOverwrite);
		}
	      }
	    }

	  }//alg
	}//trg
	mf.Write();

	for (int alg = 0; alg < 2; alg++){
	  //wrt MuidCB
	  sden = nd_dir + chainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	  snum = nd_dir + chainName + m_alg[alg] + "_Turn_On_Curve_Numerator";
	  seff = eff_dir + chainName + m_alg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	  seffg = seff + "_Fit"; // YY added 20.04.10

	  h1eff = 0;
	  mf.get(seff, h1eff);
	  if (!h1eff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	    }
	    continue;
	  }
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
	  geff = (TGraphAsymmErrors*) mf.Get(seffg);
	  if (!geff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	    }
	    continue;
	  }

	  if(h1eff && h1num && h1den){
	    h1eff->Reset();
	    h1eff->Divide(h1num, h1den, 1., 1., "B");

	    dir->cd();
	    h1eff->Write("",TObject::kOverwrite);

	    if (geff) {  // YY added 20.04.10
	      geff->SetMarkerStyle(20);
	      geff->BayesDivide(h1num, h1den);
	      
	      // saving summary TGraph
	      dir->cd();
	      geff->Write("", TObject::kOverwrite);
	    }

	    if (1 == alg) {  // MuGirlEF, all events
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
	      int iholx = static_cast<int>(iMuGirl);
		
	      TString s = eff_dir + chainName_std + "_highpt_effsummary_by" + m_vectkwd.at(3);
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
	      // saving
	      dir->cd();
	      h1sumeff->Write("", TObject::kOverwrite);
	    }
	  }

	  //wrt MuidCB + MSonly-based chain
	  sden = nd_dir + chainName + m_MSchainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	  snum = nd_dir + chainName + m_MSchainName + m_alg[alg] + "_Turn_On_Curve_Numerator";
	  seff = eff_dir + chainName + m_MSchainName + m_alg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	  seffg = seff + "_Fit"; // YY added 20.04.10

	  h1eff = 0;
	  mf.get(seff, h1eff);
	  if (!h1eff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	    }
	    continue;
	  }
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
	  geff = (TGraphAsymmErrors*) mf.Get(seffg);
	  if (!geff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	    }
	    continue;
	  }

	  if(h1eff && h1num && h1den){
	    h1eff->Reset();
	    h1eff->Divide(h1num, h1den, 1., 1., "B");

	    dir->cd();
	    h1eff->Write("",TObject::kOverwrite);

	    if (geff) {  // YY added 20.04.10
	      geff->SetMarkerStyle(20);
	      geff->BayesDivide(h1num, h1den);
	      
	      // saving summary TGraph
	      dir->cd();
	      geff->Write("", TObject::kOverwrite);
	    }
	  }

	  // for ES ---------------------------------------------------------------------
	  for (int ies = 0; ies <= m_maxESbr; ies++) {
	    sden = nd_dir + chainName + m_triggerES[ies] + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	    snum = nd_dir + chainName + m_triggerES[ies] + m_alg[alg] + "_Turn_On_Curve_Numerator";
	    seff = eff_dir + chainName + m_triggerES[ies] + m_alg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	    seffg = seff + "_Fit"; // YY added 20.04.10

	    h1eff = 0;
	    mf.get(seff, h1eff);
	    if (!h1eff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	      }
	      continue;
	    }
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
	    geff = (TGraphAsymmErrors*) mf.Get(seffg);
	    if (!geff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	      }
	      continue;
	    }

	    if(h1eff && h1num && h1den){
	      h1eff->Reset();
	      h1eff->Divide(h1num, h1den, 1., 1., "B");

	      dir->cd();
	      h1eff->Write("",TObject::kOverwrite);

	      if (geff) {  // YY added 20.04.10
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);
	      
		// saving summary TGraph
		dir->cd();
		geff->Write("", TObject::kOverwrite);
	      }
	      // Summary ESid
	      if (1 == alg) {  // MuGirlEF
		if (ESID == ies || ESINDEP == ies) {
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
		  int iholx = static_cast<int>(iMuGirl);
		    
		  TString s = eff_dir + chainName_std + "_highpt_effsummary_by" + m_triggerES[ies];
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
		  // saving
		  dir->cd();
		  h1sumeff->Write("", TObject::kOverwrite);
		}
	      }

	    }
	  }
	  // for ES: end ---------------------------------------------------------------------

	  if (0 == alg || 1 == alg) {
	    for (int be = 0; be < 2; be++) {
	      sden = nd_dir + chainName + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
	      snum = nd_dir + chainName + m_alg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	      seff = eff_dir + chainName + m_alg[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
	      seffg = seff + "_Fit"; // YY added 20.04.10
	      
	      h1eff = 0;
	      mf.get(seff, h1eff);
	      if (!h1eff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
		}
		continue;
	      }
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }
	      
	      if(h1eff && h1num && h1den){
		h1eff->Reset();
		h1eff->Divide(h1num, h1den, 1., 1., "B");
		
		dir->cd();
		h1eff->Write("",TObject::kOverwrite);
		
		if (geff) {  // YY added 20.04.10
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
		  
		  // saving summary TGraph
		  dir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }
	      // MSonly-based monitor
	      sden = nd_dir + chainName + m_MSchainName + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
	      snum = nd_dir + chainName + m_MSchainName + m_alg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	      seff = eff_dir + chainName + m_MSchainName + m_alg[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
	      seffg = seff + "_Fit"; // YY added 20.04.10
	      
	      h1eff = 0;
	      mf.get(seff, h1eff);
	      if (!h1eff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
		}
		continue;
	      }
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }
	      
	      if(h1eff && h1num && h1den){
		h1eff->Reset();
		h1eff->Divide(h1num, h1den, 1., 1., "B");
		
		dir->cd();
		h1eff->Write("",TObject::kOverwrite);
		
		if (geff) {  // YY added 20.04.10
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
		  
		  // saving summary TGraph
		  dir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }
	    }
	  }
	  
	  //wrt Upstream
	  sden = nd_dir + chainName + m_alg[alg] + "_Turn_On_Curve_wrt"+ m_wrtalg[alg] + "_Denominator";
	  snum = nd_dir + chainName + m_alg[alg] + "_Turn_On_Curve_Numerator";
	  seff = eff_dir + chainName + m_alg[alg] + "_Turn_On_Curve_wrt" + m_wrtalg[alg];
	  seffg = seff + "_Fit"; // YY added 20.04.10

	  h1eff = 0;
	  mf.get(seff, h1eff);
	  if (!h1eff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	    }
	    continue;
	  }
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
	  geff = (TGraphAsymmErrors*) mf.Get(seffg);
	  if (!geff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	    }
	    continue;
	  }

	  if(h1eff && h1num && h1den){
	    h1eff->Reset();
	    h1eff->Divide(h1num, h1den, 1., 1., "B");

	    dir->cd();
	    h1eff->Write("",TObject::kOverwrite);

	    if (geff) {  // YY added 20.04.10
	      geff->SetMarkerStyle(20);
	      geff->BayesDivide(h1num, h1den);
	      
	      // saving summary TGraph
	      dir->cd();
	      geff->Write("", TObject::kOverwrite);
	    }
	  }


	  //wrt Upstream - MSonly-based
	  sden = nd_dir + chainName + m_MSchainName + m_alg[alg] + "_Turn_On_Curve_wrt"+ m_wrtalg[alg] + "_Denominator";
	  snum = nd_dir + chainName + m_MSchainName + m_alg[alg] + "_Turn_On_Curve_Numerator";
	  seff = eff_dir + chainName + m_MSchainName + m_alg[alg] + "_Turn_On_Curve_wrt" + m_wrtalg[alg];
	  seffg = seff + "_Fit"; // YY added 20.04.10

	  h1eff = 0;
	  mf.get(seff, h1eff);
	  if (!h1eff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	    }
	    continue;
	  }
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
	  geff = (TGraphAsymmErrors*) mf.Get(seffg);
	  if (!geff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	    }
	    continue;
	  }

	  if(h1eff && h1num && h1den){
	    h1eff->Reset();
	    h1eff->Divide(h1num, h1den, 1., 1., "B");

	    dir->cd();
	    h1eff->Write("",TObject::kOverwrite);

	    if (geff) {  // YY added 20.04.10
	      geff->SetMarkerStyle(20);
	      geff->BayesDivide(h1num, h1den);
	      
	      // saving summary TGraph
	      dir->cd();
	      geff->Write("", TObject::kOverwrite);
	    }
	  }

	  // For ES --------------------------------------------------------------------------
	  //wrt Upstream
	  for (int ies = 0; ies <= m_maxESbr; ies++) {

	    sden = nd_dir + chainName + m_triggerES[ies] + m_alg[alg] + "_Turn_On_Curve_wrt"+ m_wrtalg[alg] + "_Denominator";
	    snum = nd_dir + chainName + m_triggerES[ies] + m_alg[alg] + "_Turn_On_Curve_Numerator";
	    seff = eff_dir + chainName + m_triggerES[ies] + m_alg[alg] + "_Turn_On_Curve_wrt" + m_wrtalg[alg];
	    seffg = seff + "_Fit"; // YY added 20.04.10
	      
	    h1eff = 0;
	    mf.get(seff, h1eff);
	    if (!h1eff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	      }
	      continue;
	    }
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
	    geff = (TGraphAsymmErrors*) mf.Get(seffg);
	    if (!geff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	      }
	      continue;
	    }

	    if(h1eff && h1num && h1den){
	      h1eff->Reset();
	      h1eff->Divide(h1num, h1den, 1., 1., "B");
		
	      dir->cd();
	      h1eff->Write("",TObject::kOverwrite);
		
	      if (geff) {  // YY added 20.04.10
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);
		  
		// saving summary TGraph
		dir->cd();
		geff->Write("", TObject::kOverwrite);
	      }
	    }
	  }
	  // For ES --------------------------------------------------------------------------

	  if (0 == alg || 1 == alg) {
	    for (int be = 0; be < 2; be++) {
	      //wrt Upstream
	      sden = nd_dir + chainName + m_alg[alg] + "_Turn_On_Curve_wrt"+ m_wrtalg[alg] + bestr[be] + "_Denominator";
	      snum = nd_dir + chainName + m_alg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	      seff = eff_dir + chainName + m_alg[alg] + "_Turn_On_Curve_wrt" + m_wrtalg[alg] + bestr[be];
	      seffg = seff + "_Fit"; // YY added 20.04.10

	      h1eff = 0;
	      mf.get(seff, h1eff);
	      if (!h1eff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
		}
		continue;
	      }
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }
	      
	      if(h1eff && h1num && h1den){
		h1eff->Reset();
		h1eff->Divide(h1num, h1den, 1., 1., "B");
		
		dir->cd();
		h1eff->Write("",TObject::kOverwrite);
		
		if (geff) {  // YY added 20.04.10
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
		  
		  // saving summary TGraph
		  dir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }
	      sden = nd_dir + chainName + m_MSchainName + m_alg[alg] + "_Turn_On_Curve_wrt"+ m_wrtalg[alg] + bestr[be] + "_Denominator";
	      snum = nd_dir + chainName + m_MSchainName + m_alg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	      seff = eff_dir + chainName + m_MSchainName + m_alg[alg] + "_Turn_On_Curve_wrt" + m_wrtalg[alg] + bestr[be];
	      seffg = seff + "_Fit"; // YY added 20.04.10

	      h1eff = 0;
	      mf.get(seff, h1eff);
	      if (!h1eff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
		}
		continue;
	      }
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }
	      
	      if(h1eff && h1num && h1den){
		h1eff->Reset();
		h1eff->Divide(h1num, h1den, 1., 1., "B");
		
		dir->cd();
		h1eff->Write("",TObject::kOverwrite);
		
		if (geff) {  // YY added 20.04.10
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
		  
		  // saving summary TGraph
		  dir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }

	    }
	  }//alg
	}//i
	mf.Write();
      }// MuGirl: end

      // ******************************************************//
      // *********** MuIso: no extention to high pt ***********//
      // ******************************************************//
      for (unsigned int i=0 ; i < m_chainsMI.size() ; i++) {
	TString chainName = m_chainsMI.at(i);
	
	// MuIso: begin
	TString m_alg[2] = {"_MuIso", "_MuonEFCB"};
	TString m_wrtalg[2] = {"_MuComb", "_MuIso"};

	for( unsigned int i=0 ; i < m_chainsMI.size() ; i++ ){
	  TString chainName = m_chainsMI.at(i);

	  // wrt CB Muon && independent trigger
	  for( int trg = 0 ; trg < m_maxindep ; trg++ ){
	    sden = nd_dir + chainName + "_Turn_On_Curve_wrt_MuidCB" + m_trigger[trg] + "_Triggered_Denominator";

	    for( int alg=0; alg < 2; alg++){
	      snum = nd_dir + chainName + m_alg[alg] + "_Turn_On_Curve_wrt_MuidCB" + m_trigger[trg] + "_Triggered_Numerator";
	      seff = eff_dir + chainName + m_alg[alg] + "_Turn_On_Curve_wrt_MuidCB" + m_trigger[trg] + "_Triggered";
	      seffg = seff + "_Fit"; // YY added 20.04.10
	    
	      h1eff = 0;
	      mf.get(seff, h1eff);
	      if (!h1eff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
		}
		continue;
	      }
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }
	    
	      if( h1eff && h1num && h1den){
		h1eff->Reset();
		h1eff->Divide(h1num, h1den, 1., 1., "B");

		dir->cd();
		h1eff->Write("",TObject::kOverwrite);

		if (geff) {  // YY added 20.04.10
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);

		  // saving summary TGraph
		  dir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }

	    }//alg
	  }//trg
	  mf.Write();

	  for (int alg = 0; alg < 2; alg++){
	    //wrt MuidCB
	    sden = nd_dir + chainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	    snum = nd_dir + chainName + m_alg[alg] + "_Turn_On_Curve_Numerator";
	    seff = eff_dir + chainName + m_alg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	    seffg = seff + "_Fit"; // YY added 20.04.10

	    h1eff = 0;
	    mf.get(seff, h1eff);
	    if (!h1eff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	      }
	      continue;
	    }
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
	    geff = (TGraphAsymmErrors*) mf.Get(seffg);
	    if (!geff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	      }
	      continue;
	    }

	    if(h1eff && h1num && h1den){
	      h1eff->Reset();
	      h1eff->Divide(h1num, h1den, 1., 1., "B");

	      dir->cd();
	      h1eff->Write("",TObject::kOverwrite);

	      if (geff) {  // YY added 20.04.10
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);
	      
		// saving summary TGraph
		dir->cd();
		geff->Write("", TObject::kOverwrite);
	      }
	    }
	    
	    //  for ES ------------------------------------------------------------------------------------
	    for (int ies = 0; ies <= m_maxESbr; ies++) {
	      sden = nd_dir + chainName + m_triggerES[ies] + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	      snum = nd_dir + chainName + m_triggerES[ies] + m_alg[alg] + "_Turn_On_Curve_Numerator";
	      seff = eff_dir + chainName + m_triggerES[ies] + m_alg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	      seffg = seff + "_Fit"; // YY added 20.04.10
	      
	      h1eff = 0;
	      mf.get(seff, h1eff);
	      if (!h1eff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
		}
		continue;
	      }
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }
	      
	      if(h1eff && h1num && h1den){
		h1eff->Reset();
		h1eff->Divide(h1num, h1den, 1., 1., "B");
		
		dir->cd();
		h1eff->Write("",TObject::kOverwrite);
		
		if (geff) {  // YY added 20.04.10
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
		  
		  // saving summary TGraph
		  dir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }
	    }
	    //  for ES : END ------------------------------------------------------------------------------------
	    
	    sden = nd_dir + chainName + "_Efficiency_Curve_wrt_MuidCB_Denominator_vs_etcone40";
	    snum = nd_dir + chainName + m_alg[alg] + "_Efficiency_Curve_Numerator_vs_etcone40";
	    seff = eff_dir + chainName + m_alg[alg] + "_Efficiency_Curve_wrt_MuidCB_vs_etcone40";
	    seffg = seff + "_Fit"; // YY added 20.04.10

	    h1eff = 0;
	    mf.get(seff, h1eff);
	    if (!h1eff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	      }
	      continue;
	    }
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
	    geff = (TGraphAsymmErrors*) mf.Get(seffg);
	    if (!geff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	      }
	      continue;
	    }

	    if(h1eff && h1num && h1den){
	      h1eff->Reset();
	      h1eff->Divide(h1num, h1den, 1., 1., "B");

	      dir->cd();
	      h1eff->Write("",TObject::kOverwrite);

	      if (geff) {  // YY added 20.04.10
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);
	      
		// saving summary TGraph
		dir->cd();
		geff->Write("", TObject::kOverwrite);
	      }
	    }
	  
	    sden = nd_dir + chainName + "_Efficiency_Curve_wrt_MuidCB_Denominator_vs_ptcone40";
	    snum = nd_dir + chainName + m_alg[alg] + "_Efficiency_Curve_Numerator_vs_ptcone40";
	    seff = eff_dir + chainName + m_alg[alg] + "_Efficiency_Curve_wrt_MuidCB_vs_ptcone40";
	    seffg = seff + "_Fit"; // YY added 20.04.10

	    h1eff = 0;
	    mf.get(seff, h1eff);
	    if (!h1eff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	      }
	      continue;
	    }
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
	    geff = (TGraphAsymmErrors*) mf.Get(seffg);
	    if (!geff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	      }
	      continue;
	    }

	    if(h1eff && h1num && h1den){
	      h1eff->Reset();
	      h1eff->Divide(h1num, h1den, 1., 1., "B");

	      dir->cd();
	      h1eff->Write("",TObject::kOverwrite);

	      if (geff) {  // YY added 20.04.10
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);
	      
		// saving summary TGraph
		dir->cd();
		geff->Write("", TObject::kOverwrite);
	      }
	    }

	    //wrt Upstream
	    sden = nd_dir + chainName + m_alg[alg] + "_Turn_On_Curve_wrt"+ m_wrtalg[alg] + "_Denominator";
	    snum = nd_dir + chainName + m_alg[alg] + "_Turn_On_Curve_Numerator";
	    seff = eff_dir + chainName + m_alg[alg] + "_Turn_On_Curve_wrt" + m_wrtalg[alg];
	    seffg = seff + "_Fit"; // YY added 20.04.10

	    h1eff = 0;
	    mf.get(seff, h1eff);
	    if (!h1eff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	      }
	      continue;
	    }
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
	    geff = (TGraphAsymmErrors*) mf.Get(seffg);
	    if (!geff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	      }
	      continue;
	    }

	    if(h1eff && h1num && h1den){
	      h1eff->Reset();
	      h1eff->Divide(h1num, h1den, 1., 1., "B");

	      dir->cd();
	      h1eff->Write("",TObject::kOverwrite);

	      if (geff) {  // YY added 20.04.10
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);
	      
		// saving summary TGraph
		dir->cd();
		geff->Write("", TObject::kOverwrite);
	      }
	    }
	    
	    //  for ES ------------------------------------------------------------------------------------
	    for (int ies = 0; ies <= m_maxESbr; ies++) {
	      sden = nd_dir + chainName + m_triggerES[ies] + m_alg[alg] + "_Turn_On_Curve_wrt"+ m_wrtalg[alg] + "_Denominator";
	      snum = nd_dir + chainName + m_triggerES[ies] + m_alg[alg] + "_Turn_On_Curve_Numerator";
	      seff = eff_dir + chainName + m_triggerES[ies] + m_alg[alg] + "_Turn_On_Curve_wrt" + m_wrtalg[alg];
	      seffg = seff + "_Fit"; // YY added 20.04.10
	      
	      h1eff = 0;
	      mf.get(seff, h1eff);
	      if (!h1eff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
		}
		continue;
	      }
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }
	      
	      if(h1eff && h1num && h1den){
		h1eff->Reset();
		h1eff->Divide(h1num, h1den, 1., 1., "B");
		
		dir->cd();
		h1eff->Write("",TObject::kOverwrite);
		
		if (geff) {  // YY added 20.04.10
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
		  
		  // saving summary TGraph
		  dir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }
	    }

	  }//alg
	  mf.Write();
	}//i
      }// MuIso: end
      
      TString m_alg2[3] = {"_MuFast", "_MuonEFMS", "_MuonEFSA"};
      TString m_wrtalg2[3] = {"_L1", "_MuFast", "_MuFast"};

      // ******************************************************//
      // ******************  MSonly Chains ********************//
      // ******************************************************//
      for( unsigned int i=0 ; i < m_chainsMSonly.size() ; i++ ){
        TString chainName = m_chainsMSonly.at(i);

        for( int trg = 0 ; trg < m_maxindep ; trg++ ){
          sden = nd_dir + chainName + "_Turn_On_Curve_wrt_MuidSA" + m_trigger[trg] + "_Triggered_Denominator";

          for(int alg = 0 ; alg < 3 ; alg++ ){
            snum = nd_dir  + chainName + m_alg2[alg] + "_Turn_On_Curve_wrt_MuidSA" + m_trigger[trg] + "_Triggered_Numerator";
            seff = eff_dir + chainName + m_alg2[alg] + "_Turn_On_Curve_wrt_MuidSA" + m_trigger[trg] + "_Triggered";
	    seffg = seff + "_Fit"; // YY added 20.04.10
	    
	    h1eff = 0;
	    mf.get(seff, h1eff);
	    if (!h1eff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	      }
	      continue;
	    }
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
	    geff = (TGraphAsymmErrors*) mf.Get(seffg);
	    if (!geff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	      }
	      continue;
	    }
	    
	    if(h1eff && h1num && h1den){
              h1eff->Reset();
              h1eff->Divide(h1num, h1den, 1., 1., "B");
	      
              dir->cd();
              h1eff->Write("",TObject::kOverwrite);

	      if (geff) {  // YY added 20.04.10
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);
		
		// saving summary TGraph
		dir->cd();
		geff->Write("", TObject::kOverwrite);
	      }
	      
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
		    iholx = static_cast<int>(iMuFast);
		  } else if (2 == alg) {
		    iholx = static_cast<int>(iEFSA);
		  }
		  
		  if (iholx >= 0) {
		    TString s = eff_dir + chainName + "_highpt_effsummary_by" + m_vectkwd.at(2);
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
          snum  = nd_dir  + chainName + m_alg2[alg] + "_Turn_On_Curve_Numerator";
          seff  = eff_dir + chainName + m_alg2[alg] + "_Turn_On_Curve_wrt_MuidSA";
	  seffg = seff + "_Fit"; // YY added 20.04.10

	  h1eff = 0;
	  mf.get(seff, h1eff);
	  if (!h1eff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	    }
	    continue;
	  }
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
	  geff = (TGraphAsymmErrors*) mf.Get(seffg);
	  if (!geff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	    }
	    continue;
	  }

          if(h1eff && h1num && h1den){
            h1eff->Reset();
            h1eff->Divide(h1num, h1den, 1., 1., "B");

            dir->cd();
            h1eff->Write("",TObject::kOverwrite);
	    if (geff) {  // YY added 20.04.10
	      geff->SetMarkerStyle(20);
	      geff->BayesDivide(h1num, h1den);
	      
	      // saving summary TGraph
	      dir->cd();
	      geff->Write("", TObject::kOverwrite);
	    }
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
		iholx = static_cast<int>(iMuFast);
	      } else if (2 == alg) {
		iholx = static_cast<int>(iEFSA);
	      }
	      
	      if (iholx >= 0) {
		TString s = eff_dir + chainName + "_highpt_effsummary_by" + m_vectkwd.at(3);
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
		// saving
		dir->cd();
		h1sumeff->Write("", TObject::kOverwrite);
	      }
	    }
          }
          //wrt MuidSA

	  // for ES ----------------------------------------------------------------
	  for (int ies = 0; ies <= m_maxESbr; ies++) {

	    if(!MS_mon_ESbr[ies])continue;
	    // for ES, L1 ------------------------------------------------------------
	    if (0 == alg) {
	      sden = nd_dir + chainName + m_triggerES[ies] + "_Turn_On_Curve_wrt_MuidSA_Denominator";
	      snum = nd_dir + chainName + m_triggerES[ies] + "_MuFast" + "_Turn_On_Curve_wrt" + "_L1" + "_Denominator";
	      seff = eff_dir + chainName + m_triggerES[ies] + "_L1" + "_Turn_On_Curve_wrt_MuidSA";
	      seffg = seff + "_Fit";
	      // hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
	      h1eff = 0;
	      mf.get(seff, h1eff);
	      if (!h1eff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
		}
		continue;
	      }
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }
	    
	      if(h1eff && h1num && h1den){
		h1eff->Reset();
		h1eff->Divide(h1num, h1den, 1., 1., "B");

		dir->cd();
		h1eff->Write("",TObject::kOverwrite);
		if (geff) {  // YY added 20.04.10
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
		
		  // saving summary TGraph
		  dir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }
	    
	      for (int be = 0; be < 2; be++) {
		sden = nd_dir + chainName + m_triggerES[ies] + "_Turn_On_Curve_wrt_MuidSA" + bestr[be] + "_Denominator";
		snum = nd_dir + chainName + m_triggerES[ies] + "_MuFast" + "_Turn_On_Curve_wrt" + "_L1" + bestr[be] + "_Denominator";
		seff  = eff_dir + chainName + m_triggerES[ies] + "_L1" + bestr[be] + "_Turn_On_Curve_wrt_MuidSA";
		seffg = seff + "_Fit";
		// hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
		h1eff = 0;
		mf.get(seff, h1eff);
		if (!h1eff) {
		  if (fdbg) {
		    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
		  }
		  continue;
		}
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
		geff = (TGraphAsymmErrors*) mf.Get(seffg);
		if (!geff) {
		  if (fdbg) {
		    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		  }
		  continue;
		}
	      
		if(h1eff && h1num && h1den){
		  h1eff->Reset();
		  h1eff->Divide(h1num, h1den, 1., 1., "B");
		
		  dir->cd();
		  h1eff->Write("",TObject::kOverwrite);
		  if (geff) {  // YY added 20.04.10
		    geff->SetMarkerStyle(20);
		    geff->BayesDivide(h1num, h1den);
		  
		    // saving summary TGraph
		    dir->cd();
		    geff->Write("", TObject::kOverwrite);
		  }
		}
	      }
	    
	    }
	    // for ES, L1 end ------------------------------------------------------------


	    sden  = nd_dir  + chainName + m_triggerES[ies] + "_Turn_On_Curve_wrt_MuidSA_Denominator";
	    snum  = nd_dir  + chainName + m_triggerES[ies] + m_alg2[alg] + "_Turn_On_Curve_Numerator";
	    seff  = eff_dir + chainName + m_triggerES[ies] + m_alg2[alg] + "_Turn_On_Curve_wrt_MuidSA";
	    seffg = seff + "_Fit"; // YY added 20.04.10
	    
	    h1eff = 0;
	    mf.get(seff, h1eff);
	    if (!h1eff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	      }
	      continue;
	    }
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
	    geff = (TGraphAsymmErrors*) mf.Get(seffg);
	    if (!geff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	      }
	      continue;
	    }
	    
	    if(h1eff && h1num && h1den){
	      h1eff->Reset();
	      h1eff->Divide(h1num, h1den, 1., 1., "B");

	      dir->cd();
	      h1eff->Write("",TObject::kOverwrite);
	      if (geff) {  // YY added 20.04.10
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);
		
		// saving summary TGraph
		dir->cd();
		geff->Write("", TObject::kOverwrite);
	      }

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
		    iholx = static_cast<int>(iMuFast);
		  } else if (2 == alg) {
		    iholx = static_cast<int>(iEFSA);
		  }
		  
		  if (iholx >= 0) {
		    TString s = eff_dir + chainName + "_highpt_effsummary_by" + m_triggerES[ies];
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
	      snum  = nd_dir  + chainName + m_alg2[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	      seff  = eff_dir + chainName + m_alg2[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidSA";
	      seffg = seff + "_Fit"; // YY added 20.04.10

	      h1eff = 0;
	      mf.get(seff, h1eff);
	      if (!h1eff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
		}
		continue;
	      }
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }

	      if(h1eff && h1num && h1den){
		h1eff->Reset();
		h1eff->Divide(h1num, h1den, 1., 1., "B");

		dir->cd();
		h1eff->Write("",TObject::kOverwrite);
		if (geff) {  // YY added 20.04.10
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
	      
		  // saving summary TGraph
		  dir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }
	    }
	  }
	  
          //wrt upstream
          sden  = nd_dir  + chainName + m_alg2[alg] + "_Turn_On_Curve_wrt" + m_wrtalg2[alg] + "_Denominator";
          snum  = nd_dir  + chainName + m_alg2[alg] + "_Turn_On_Curve_Numerator";
          seff  = eff_dir + chainName + m_alg2[alg] + "_Turn_On_Curve_wrt" + m_wrtalg2[alg];
	  seffg = seff + "_Fit"; // YY added 20.04.10

	  h1eff = 0;
	  mf.get(seff, h1eff);
	  if (!h1eff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	    }
	    continue;
	  }
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
	  geff = (TGraphAsymmErrors*) mf.Get(seffg);
	  if (!geff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	    }
	    continue;
	  }

          if(h1eff && h1num && h1den){
            h1eff->Reset();
            h1eff->Divide(h1num, h1den, 1., 1., "B");

            dir->cd();
            h1eff->Write("",TObject::kOverwrite);
	    if (geff) {  // YY added 20.04.10
	      geff->SetMarkerStyle(20);
	      geff->BayesDivide(h1num, h1den);
	      
	      // saving summary TGraph
	      dir->cd();
	      geff->Write("", TObject::kOverwrite);
	    }
          }

          //wrt upstream
	  // for ES --------------------------------------------------------------------
	  for (int ies = 0; ies <= m_maxESbr; ies++) {
	    if(!MS_mon_ESbr[ies])continue; 
	    sden  = nd_dir  + chainName + m_triggerES[ies] + m_alg2[alg] + "_Turn_On_Curve_wrt" + m_wrtalg2[alg] + "_Denominator";
	    snum  = nd_dir  + chainName + m_triggerES[ies] + m_alg2[alg] + "_Turn_On_Curve_Numerator";
	    seff  = eff_dir + chainName + m_triggerES[ies] + m_alg2[alg] + "_Turn_On_Curve_wrt" + m_wrtalg2[alg];
	    seffg = seff + "_Fit"; // YY added 20.04.10
	    
	    h1eff = 0;
	    mf.get(seff, h1eff);
	    if (!h1eff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	      }
	      continue;
	    }
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
	    geff = (TGraphAsymmErrors*) mf.Get(seffg);
	    if (!geff) {
	      if (fdbg) {
		std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	      }
	      continue;
	    }
	    
	    if(h1eff && h1num && h1den){
	      h1eff->Reset();
	      h1eff->Divide(h1num, h1den, 1., 1., "B");
	      
	      dir->cd();
	      h1eff->Write("",TObject::kOverwrite);
	      if (geff) {  // YY added 20.04.10
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);
		
		// saving summary TGraph
		dir->cd();
		geff->Write("", TObject::kOverwrite);
	      }
	    }
	  }
	  // for ES: end --------------------------------------------------------------------

	  
	  if (0 == alg || 2 == alg) {
	    for (int be = 0; be < 2; be++) {
	      //wrt upstream
	      sden  = nd_dir  + chainName + m_alg2[alg] + "_Turn_On_Curve_wrt" + m_wrtalg2[alg] + bestr[be] + "_Denominator";
	      snum  = nd_dir  + chainName + m_alg2[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	      seff  = eff_dir + chainName + m_alg2[alg] + "_Turn_On_Curve_wrt" + m_wrtalg2[alg] + bestr[be];
	      seffg = seff + "_Fit"; // YY added 20.04.10

	      h1eff = 0;
	      mf.get(seff, h1eff);
	      if (!h1eff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
		}
		continue;
	      }
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }

	      if(h1eff && h1num && h1den){
		h1eff->Reset();
		h1eff->Divide(h1num, h1den, 1., 1., "B");

		dir->cd();
		h1eff->Write("",TObject::kOverwrite);
		if (geff) {  // YY added 20.04.10
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
	      
		  // saving summary TGraph
		  dir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
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

	  h1eff = 0;
	  mf.get(seff, h1eff);
	  if (!h1eff) {
	    if (fdbg) {
	      std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
	    }
	    continue;
	  }
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

          if(h1eff && h1num && h1den){
            h1eff->Reset();
            h1eff->Divide(h1num, h1den);

            dir->cd();
            h1eff->Write("",TObject::kOverwrite);
          }

        }//icut
      }//itype
      mf.Write();

      // Triggers / Event
      std::vector<TString> m_chains;
      std::vector<TString> m_chains2;

      {
	std::map<std::string, std::string> m_ztpmap;
	// pp_v4 menu items for ZTP
	m_ztpmap["mu36_tight"] = "L1_MU15";
	m_ztpmap["mu24i_tight"] = "L1_MU15";
	m_ztpmap["mu50_MSonly_barrel_tight"] = "L1_MU15";

	std::map<std::string, int> m_ztp_isomap;
	m_ztp_isomap["mu36_tight"] = 0;
	m_ztp_isomap["mu24i_tight"] = 1;
	m_ztp_isomap["mu50_MSonly_barrel_tight"] = 0;

	// old menu pp_v3 for ZTP
	// m_ztpmap["mu15"]="L1_MU10";
	// m_ztpmap["mu15i"]="L1_MU10";
	// m_ztpmap["mu20i_medium"]="L1_MU11";
	// m_ztpmap["mu20"]="L1_MU10";
	// m_ztpmap["mu20_MG"]="L1_MU10";
	// m_ztpmap["mu22_medium"]="L1_MU11"; // YY
	// m_ztpmap["mu22_MG_medium"]="L1_MU11";
	// m_ztpmap["mu40_MSonly_barrel"]="L1_MU10";
	// m_ztpmap["mu40_MSonly_barrel_medium"]="L1_MU11";
	// m_ztpmap["mu40_MSonly_tight"]="L1_MU10";
	// m_ztpmap["mu40_MSonly_tighter"]="L1_MU10";

	for(std::map<std::string, std::string>::iterator itmap=m_ztpmap.begin();itmap!=m_ztpmap.end();++itmap){
	  TString histdirmuztp = run_dir + "/HLT/MuonMon/MuZTP/"+itmap->first;
	  TDirectory* ztpdir = mf.GetDirectory(histdirmuztp);
	  bool  isefisochain = m_ztp_isomap[itmap->first] > 0; 
	  
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
	      // dynamic_cast<TGraphAsymmErrors*>( graph(("muZTP_eff_"+level[j]+var[k]+itmap->first).c_str(), histdirmuztp))->BayesDivide(hist(("muZTP"+var[k]+level[j]+"fired_"+itmap->first).c_str(), histdirmuztp), hist(("muZTP"+var[k]+itmap->first).c_str(), histdirmuztp));
	      
	      seffg = histdirmuztp + "/muZTP_eff_"+level[j]+var[k]+itmap->first;
	      snum = histdirmuztp + "/muZTP"+var[k]+level[j]+"fired_"+itmap->first;
	      sden = histdirmuztp + "/muZTP"+var[k]+itmap->first;
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
		geff = (TGraphAsymmErrors*) mf.Get(seffg);
		if (!geff) {
		  if (fdbg) {
		    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		  }
		  continue;
		}
		if (geff) {
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
		  
		  // saving summary TGraph
		  ztpdir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }
	      mf.Write();

	      // dynamic_cast<TGraphAsymmErrors*>( graph(("muZTP_eff_"+level[j]+"_Eta_1bin_"+itmap->first).c_str(), histdirmuztp))->BayesDivide(hist(("muZTP_Eta_1bin_"+level[j]+"fired_"+itmap->first).c_str(), histdirmuztp), hist(("muZTP_Eta_1bin_"+itmap->first).c_str(), histdirmuztp));

	      seffg = histdirmuztp + "/muZTP_eff_"+level[j]+"_Eta_1bin_"+itmap->first;
	      snum = histdirmuztp + "/muZTP_Eta_1bin_"+level[j]+"fired_"+itmap->first;
	      sden = histdirmuztp + "/muZTP_Eta_1bin_"+itmap->first;
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
		geff = (TGraphAsymmErrors*) mf.Get(seffg);
		if (!geff) {
		  if (fdbg) {
		    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		  }
		  continue;
		}
		if (geff) {
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
		  
		  // saving summary TGraph
		  ztpdir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }
	      mf.Write();
	      
	      // dynamic_cast<TGraphAsymmErrors*>( graph(("muZTP_eff_"+level[j]+"_Eta_2bins_"+itmap->first).c_str(), histdirmuztp))->BayesDivide(hist(("muZTP_Eta_2bins_"+level[j]+"fired_"+itmap->first).c_str(), histdirmuztp), hist(("muZTP_Eta_2bins_"+itmap->first).c_str(), histdirmuztp));
	      seffg = histdirmuztp + "/muZTP_eff_"+level[j]+"_Eta_2bins_"+itmap->first;
	      snum = histdirmuztp + "/muZTP_Eta_2bins_"+level[j]+"fired_"+itmap->first;
	      sden = histdirmuztp + "/muZTP_Eta_2bins_"+itmap->first;
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
		geff = (TGraphAsymmErrors*) mf.Get(seffg);
		if (!geff) {
		  if (fdbg) {
		    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		  }
		  continue;
		}
		if (geff) {
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
		  
		  // saving summary TGraph
		  ztpdir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }
	      mf.Write();

	      
	      // dynamic_cast<TGraphAsymmErrors*>( graph(("muZTP_eff_"+level[j]+"_Eta_1bin_cut_"+itmap->first).c_str(), histdirmuztp))->BayesDivide(hist(("muZTP_Eta_1bin_cut_"+level[j]+"fired_"+itmap->first).c_str(), histdirmuztp), hist(("muZTP_Eta_1bin_cut_"+itmap->first).c_str(), histdirmuztp));
	      seffg = histdirmuztp + "/muZTP_eff_"+level[j]+"_Eta_1bin_cut_"+itmap->first;
	      snum = histdirmuztp + "/muZTP_Eta_1bin_cut_"+level[j]+"fired_"+itmap->first;
	      sden = histdirmuztp + "/muZTP_Eta_1bin_cut_"+itmap->first;

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
		geff = (TGraphAsymmErrors*) mf.Get(seffg);
		if (!geff) {
		  if (fdbg) {
		    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		  }
		  continue;
		}
		if (geff) {
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
		  
		  // saving summary TGraph
		  ztpdir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }
	      mf.Write();

	      // dynamic_cast<TGraphAsymmErrors*>( graph(("muZTP_eff_"+level[j]+"_Eta_2bins_cut_"+itmap->first).c_str(), histdirmuztp))->BayesDivide(hist(("muZTP_Eta_2bins_cut_"+level[j]+"fired_"+itmap->first).c_str(), histdirmuztp), hist(("muZTP_Eta_2bins_cut_"+itmap->first).c_str(), histdirmuztp));
	      seffg = histdirmuztp + "/muZTP_eff_"+level[j]+"_Eta_2bins_cut_"+itmap->first;
	      snum = histdirmuztp + "/muZTP_Eta_2bins_cut_"+level[j]+"fired_"+itmap->first;
	      sden = histdirmuztp + "/muZTP_Eta_2bins_cut_"+itmap->first;
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
		geff = (TGraphAsymmErrors*) mf.Get(seffg);
		if (!geff) {
		  if (fdbg) {
		    std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		  }
		  continue;
		}
		if (geff) {
		  geff->SetMarkerStyle(20);
		  geff->BayesDivide(h1num, h1den);
		  
		  // saving summary TGraph
		  ztpdir->cd();
		  geff->Write("", TObject::kOverwrite);
		}
	      }
	      mf.Write();
	      
	      //2D ETA_PHI
	      seff = histdirmuztp + "/muZTP_eff_EtaPhi_"+level[j]+"_" + itmap->first;
	      snum = histdirmuztp + "/muZTP_EtaPhi_"+level[j]+"_" + itmap->first;
	      sden = histdirmuztp + "/muZTP_EtaPhi_all_"+ itmap->first;

	      TH2F *h2eff(0);
	      TH2F *h2num(0);
	      TH2F *h2den(0);
	      
	      h2eff = 0;
	      mf.get(seff, h2eff);
	      if(!h2eff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
		}
		continue;
	      }
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
	      
	      if(h2eff && h2num && h2den){
		h2eff->Reset();
		h2eff->Divide(h2num, h2den, 1., 1., "B");
		
		ztpdir->cd();
		h2eff->Write("",TObject::kOverwrite);
	      }
	      mf.Write();
	    

	    }//level
	    
	    //RELATIVE
	    // dynamic_cast<TGraphAsymmErrors*>( graph(("muZTP_eff_EFwrtL2"+var[k]+itmap->first).c_str(), histdirmuztp))->BayesDivide(hist(("muZTP"+var[k]+"EFfired_"+itmap->first).c_str(), histdirmuztp), hist(("muZTP"+var[k]+"L2fired_"+itmap->first).c_str(), histdirmuztp));
	    seffg = histdirmuztp + "/muZTP_eff_EFwrtL2"+var[k]+itmap->first;
	    snum = histdirmuztp + "/muZTP"+var[k]+"EFL2fired_"+itmap->first;
	    sden = histdirmuztp + "/muZTP"+var[k]+"L2fired_"+itmap->first;
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }
	      if (geff) {
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);
		
		// saving summary TGraph
		ztpdir->cd();
		geff->Write("", TObject::kOverwrite);
	      }
	    }
	    mf.Write();
	    
	    // dynamic_cast<TGraphAsymmErrors*>( graph(("muZTP_eff_EFwrtL1"+var[k]+itmap->first).c_str(), histdirmuztp))->BayesDivide(hist(("muZTP"+var[k]+"EFfired_"+itmap->first).c_str(), histdirmuztp), hist(("muZTP"+var[k]+"L1fired_"+itmap->first).c_str(), histdirmuztp));
	    seffg = histdirmuztp + "/muZTP_eff_EFwrtL1"+var[k]+itmap->first;
	    snum = histdirmuztp + "/muZTP"+var[k]+"EFfired_"+itmap->first;
	    sden = histdirmuztp + "/muZTP"+var[k]+"L1fired_"+itmap->first;
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }
	      if (geff) {
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);
		
		// saving summary TGraph
		ztpdir->cd();
		geff->Write("", TObject::kOverwrite);
	      }
	    }
	    mf.Write();
	    
	    // dynamic_cast<TGraphAsymmErrors*>( graph(("muZTP_eff_L2wrtL1"+var[k]+itmap->first).c_str(), histdirmuztp))->BayesDivide(hist(("muZTP"+var[k]+"L2fired_"+itmap->first).c_str(), histdirmuztp), hist(("muZTP"+var[k]+"L1fired_"+itmap->first).c_str(), histdirmuztp));
	    seffg = histdirmuztp + "/muZTP_eff_L2wrtL1"+var[k]+itmap->first;
	    snum = histdirmuztp + "/muZTP"+var[k]+"L2fired_"+itmap->first;
	    sden = histdirmuztp + "/muZTP"+var[k]+"L1fired_"+itmap->first;
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
	      geff = (TGraphAsymmErrors*) mf.Get(seffg);
	      if (!geff) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
		}
		continue;
	      }
	      if (geff) {
		geff->SetMarkerStyle(20);
		geff->BayesDivide(h1num, h1den);
		
		// saving summary TGraph
		ztpdir->cd();
		geff->Write("", TObject::kOverwrite);
	      }
	    }
	    mf.Write();

	   if(isefisochain){
	       seffg = histdirmuztp + "/muZTP_eff_EFIsowrtEF"+var[k]+itmap->first;
	       snum = histdirmuztp + "/muZTP"+var[k]+"EFIsofired_"+itmap->first;
	       sden = histdirmuztp + "/muZTP"+var[k]+"EFfired_"+itmap->first;
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
	         geff = (TGraphAsymmErrors*) mf.Get(seffg);
	         if (!geff) {
	           if (fdbg) {
	             std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
	           }
	           continue;
	         }
	         if (geff) {
	           geff->SetMarkerStyle(20);
	           geff->BayesDivide(h1num, h1den);
	           
	           // saving summary TGraph
	           ztpdir->cd();
	           geff->Write("", TObject::kOverwrite);
	         }
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
	const int MAXARR = 3;
	std::string charr[MAXARR] = {"mu36_tight", "mu24i_tight", "mu50_MSonly_barrel_tight"};
	std::string monarr[MAXARR] = {"_EFmuon", "_EFmuon", "_MuonEFSA"};
	std::string monL2arr[MAXARR] = {"_MuFast", "_MuFast", "_MuFast"};
	bool isBarrelMon[MAXARR] = {false, false, true}; // enable MSonly
	bool isMSbMon[MAXARR] = {true, false, false}; // Skip isol and MSonly
	bool monL1[MAXARR] = {true, true, false}; // Skip MSonly
	bool isefIsolation[MAXARR] = {false, true, false}; // EF isolation  add by Yuan

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
	      double sumd = h1den->Integral(ibin, ibin); ////
	      if (sumd == 0.) {
		sumeff = 0.;
		sumerr = 0.;
	      } else {
		sumeff = (double)sumn / (double) sumd;
		sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	      }
	      h1eff->SetBinContent(ibin-1, sumeff);  ////
	      h1eff->SetBinError(ibin-1, sumerr);    ////
	    }
	  }
	  
	    /* 3. Picking up chainDQ MSonly graph */
	    /* EF efficiency wrt L1, as for the ztp graph = overall HLT efficiency wrt L1: not possible, wrt offline */
	  if (isMSbMon[ialg]) {  // skip muIso and MSonly !!!
	    TString histChNum = nd_dir + chainName + m_MSchainName + MoniAlg + "_Turn_On_Curve_Numerator";
	    TString histChDen = nd_dir + chainName + m_MSchainName + MoniL2Alg + "_Turn_On_Curve_wrt_L1_Denominator";
	    
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
	    double sumn = h1numb->Integral(7, 10); // 30-50 GeV
	    double sumd = h1denb->Integral(7, 10);
	    if (sumd == 0.) {
	      sumeff = 0.;
	      sumerr = 0.;
	    } else {
	      sumeff = (double)sumn / (double) sumd;
	      sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	    }
	    h1sumL->SetBinContent(1, sumeff);
	    h1sumL->SetBinError(1, sumerr);
	    
	    sumn = h1nume->Integral(7, 10);
	    sumd = h1dene->Integral(7, 10);
	    if (sumd == 0.) {
	      sumeff = 0.;
	      sumerr = 0.;
	    } else {
	      sumeff = (double)sumn / (double) sumd;
	      sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
	    }
	    h1sumL->SetBinContent(2, sumeff);
	    h1sumL->SetBinError(2, sumerr);
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
      TString monalg[3]={"_MuFast", "_MuComb", "_EFmuon"};
      TString wrtalg[3]={"_L1", "_MuFast", "_MuComb"};
      
      TString numer, denom, effi;
      TString histdireff = eff_dir;

      TDirectory* efdir = mf.GetDirectory(eff_dir);
      if(!efdir){
	if (fdbg) {
	  std::cerr<< "HLTMuonHistogramDivision: directory "<<eff_dir<<" not found"<<std::endl;
	}
	return;
      }

      // processing muIso histograms in the same manner
      m_chainsGeneric.insert(m_chainsGeneric.end(), m_chainsEFiso.begin(), m_chainsEFiso.end());
      
      for( unsigned int i=0 ; i < m_chainsGeneric.size() ; i++ ){
	TString chainName = m_chainsGeneric.at(i);
	if (fdbg) {
	  std::cout << "proc generic " << chainName << std::endl;
	}

	//wrt CB muon && Upstream trigger
	for( int alg = 0 ; alg < 3 ; alg++ ){
	  denom = chainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	  numer = chainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	  effi  = chainName + monalg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	  // hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
	  // dynamic_cast<TGraphAsymmErrors*>( graph(effi + "_Fit", histdireff ) )->BayesDivide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom) );
	  HLTMuonHDiv(mf, histdireff, numer, denom, effi, effi + "_Fit");
	
	  denom = chainName + m_MSchainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	  numer = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	  effi  = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	  // hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
	  // dynamic_cast<TGraphAsymmErrors*>( graph(effi + "_Fit", histdireff ) )->BayesDivide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom) );
	  HLTMuonHDiv(mf, histdireff, numer, denom, effi, effi + "_Fit");
      
	  // Summary all - removed
      
	  denom = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	  numer = chainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	  effi = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	  // hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
	  // dynamic_cast<TGraphAsymmErrors*>( graph(effi + "_Fit", histdireff ) )->BayesDivide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom) );
	  HLTMuonHDiv(mf, histdireff, numer, denom, effi, effi + "_Fit");


	  denom = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	  numer = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	  effi = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	  // hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
	  // dynamic_cast<TGraphAsymmErrors*>( graph(effi + "_Fit", histdireff ) )->BayesDivide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom) );
	  HLTMuonHDiv(mf, histdireff, numer, denom, effi, effi + "_Fit");

	  //  for ES ------------------------------------------------------------------------------------
	  for (int i = 0; i <= m_maxESbr; i++) {

	    if(!CB_mon_ESbr[i]) continue;
	    if (0 == alg) {
	      // L1 efficiency: new for 2011 HI runs and afterward
	      // only division once since it is "the zero-th" algorithm
	      denom = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	      numer = chainName + m_triggerES[i] + "_MuFast" + "_Turn_On_Curve_wrt" + "_L1" + "_Denominator";
	      effi  = chainName + m_triggerES[i] + "_L1" + "_Turn_On_Curve_wrt_MuidCB";
	      // hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
	      // dynamic_cast<TGraphAsymmErrors*>( graph(effi + "_Fit", histdireff ) )->BayesDivide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom) );
	      HLTMuonHDiv(mf, histdireff, numer, denom, effi, effi + "_Fit");

	      // Need to implement barrel and endcap ...
	      for (int be = 0; be < 2; be++) {
		denom = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
		numer = chainName + m_triggerES[i] + "_MuFast" + "_Turn_On_Curve_wrt" + "_L1" + bestr[be] + "_Denominator";
		effi  = chainName + m_triggerES[i] + "_L1" + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
		// hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
		// dynamic_cast<TGraphAsymmErrors*>( graph(effi + "_Fit", histdireff ) )->BayesDivide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom) );
		HLTMuonHDiv(mf, histdireff, numer, denom, effi, effi + "_Fit");

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
		  double sumn = h1num->Integral(iSTDL, iSTDH); // 40-80 GeV
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
		  efdir->cd();
		  h1effL1->Write("",TObject::kOverwrite);
		}
	      }	  
	    }
	
	    denom = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	    numer = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_Numerator";
	    effi  = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	    // hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
	    // dynamic_cast<TGraphAsymmErrors*>( graph(effi + "_Fit", histdireff ) )->BayesDivide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom) );
	    HLTMuonHDiv(mf, histdireff, numer, denom, effi, effi + "_Fit");

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
		iholx = static_cast<int>(iMuFast);
	      } else if (1 == alg) {
		iholx = static_cast<int>(iMuComb);
	      } else if (2 == alg) {
		iholx = static_cast<int>(iEFCB);
	      }
	  
	      TString s = histdireff + chainName + "_highpt_effsummary_by" + m_triggerES[i];
	      TH1F *h1effsum = 0;
	      mf.get(s, h1effsum);
	      if (!h1effsum) {
		if (fdbg) {
		  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sden << std::endl;
		}
		continue;
	      }
	      h1effsum->SetBinContent(iholx+1, sumeff);
	      h1effsum->SetBinError(iholx+1, sumerr);
	      efdir->cd();
	      h1effsum->Write("",TObject::kOverwrite);
	    }
	
	    denom = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	    numer = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_Numerator";
	    effi = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	    // hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
	    // dynamic_cast<TGraphAsymmErrors*>( graph(effi + "_Fit", histdireff ) )->BayesDivide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom) );
	    HLTMuonHDiv(mf, histdireff, numer, denom, effi, effi + "_Fit");

	  }
	  //  for ES : END ------------------------------------------------------------------------------

	  // Barrel/Endcap
	  if (0 == alg || 1 == alg || 2 == alg) {
	    for (int be = 0; be < 2; be++) {
	      denom = chainName + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
	      numer = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	      effi  = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
	      // hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
	      // dynamic_cast<TGraphAsymmErrors*>( graph(effi + "_Fit", histdireff ) )->BayesDivide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom) );
	      HLTMuonHDiv(mf, histdireff, numer, denom, effi, effi + "_Fit");
	    }

	    for (int be = 0; be < 2; be++) {
	      denom = chainName + m_MSchainName + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
	      numer = chainName + m_MSchainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	      effi  = chainName + m_MSchainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
	      // hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
	      // dynamic_cast<TGraphAsymmErrors*>( graph(effi + "_Fit", histdireff ) )->BayesDivide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom) );
	      HLTMuonHDiv(mf, histdireff, numer, denom, effi, effi + "_Fit");
	    }

	    for (int be = 0; be < 2; be++) {
	      denom = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be] + "_Denominator";
	      numer = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	      effi = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be];
	      // hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
	      // dynamic_cast<TGraphAsymmErrors*>( graph(effi + "_Fit", histdireff ) )->BayesDivide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom) );
	      HLTMuonHDiv(mf, histdireff, numer, denom, effi, effi + "_Fit");
	    }


	    for (int be = 0; be < 2; be++) {
	      denom = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be] + "_Denominator";
	      numer = chainName + m_MSchainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	      effi = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be];
	      // hist(effi, histdireff)->Divide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom), 1, 1, "B" );
	      // dynamic_cast<TGraphAsymmErrors*>( graph(effi + "_Fit", histdireff ) )->BayesDivide( hist(numer, histdireffnumdenom), hist(denom, histdireffnumdenom) );
	      HLTMuonHDiv(mf, histdireff, numer, denom, effi, effi + "_Fit");
	    }

	  }

	}
      }      
    }//while (now just open bracket: 110728
  }//MonitoringFile::HLTMuonHistogramDivision

  void MonitoringFile::HLTMuonHDiv(PostProcessorFileWrapper& mf,
				   TString sdir, TString snum, TString sden, TString seff, TString seffg)
  {
    TH1F* h1eff(0);
    TH1F* h1num(0);
    TH1F* h1den(0);
    TGraphAsymmErrors* geff(0);
	    
    h1eff = 0;
    mf.get(sdir + seff, h1eff);
    if (!h1eff) {
      if (fdbg) {
	std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seff << std::endl;
      }
      return;
    }
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
    geff = (TGraphAsymmErrors*) mf.Get(sdir + seffg);
    if (!geff) {
      if (fdbg) {
	std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< seffg << std::endl;
      }
      return;
    }

    TDirectory* dir = mf.GetDirectory(sdir);
    
    if( h1eff && h1num && h1den){
      h1eff->Reset();
      h1eff->Divide(h1num, h1den, 1., 1., "B");
      
      dir->cd();
      h1eff->Write("",TObject::kOverwrite);
      
      if (geff) {  // YY added 20.04.10
	geff->SetMarkerStyle(20);
	geff->BayesDivide(h1num, h1den);
	
	// saving summary TGraph
	dir->cd();
	geff->Write("", TObject::kOverwrite);
      }
    }
  }

  
}//namespace
