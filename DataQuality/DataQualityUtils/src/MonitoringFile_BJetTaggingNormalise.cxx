/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Methods to perform post-processing on "run_nnnnnn/JetTagging/ *2D" histograms
 * Mainly to normalise the z axis range to the total number of tracks/jets.
 *
 *
 *
 * Based on the HLTJetCalcEfficiencyAndRate code of Venkatesh Kaushik (venkat.kaushik@cern.ch).
 *
 * Author   : Manuel Neumann (mneumann@cern.ch)
 * Date     : Aug 2011
 */

#include "DataQualityUtils/MonitoringFile.h"

#include <vector>

#include "TH2F.h"
#include "TFile.h"
#include "TKey.h"

namespace dqutils{

  void MonitoringFile::BJetTaggingNormalise(TFile* f) {

    int debugLevel = MonitoringFile::getDebugLevel();

    if (debugLevel > 1) {
      std::cout << "--> BJetTaggingNormalise: Adjusting ranges of d0, z0 and d0Sig, z0Sig"
		<< std::endl;
    }

    f->cd("/");
    // get run directory name
    TIter nextcd0(gDirectory->GetListOfKeys());
    TKey *key0 = (TKey*) nextcd0();

    TDirectory *dir0 = dynamic_cast<TDirectory*> (key0->ReadObj());
    if (dir0 != 0) {

      dir0->cd();

      TIter next_run(f->GetListOfKeys());
      TKey* key_run(0);

      // loop through the runs
      while ((key_run = dynamic_cast<TKey*> (next_run())) != 0) {
	if (debugLevel > 1) {
	  std::cout << "--> BJetTaggingNormalise: Getting run " << key_run << std::endl;
	}
	TObject* obj_run = key_run->ReadObj();
	TDirectory* tdir_run = dynamic_cast<TDirectory*> (obj_run);

	// Some checks of the run number
	if (tdir_run != 0) {
	  //std::cerr << "--> BJetTaggingNormalise: directory " << tdir_run << " not found."
	  //		<< std::endl;
	  //return;
	  //}

	  TString run_dir = tdir_run->GetName();
	  if (!run_dir.Contains("run")) {

	    std::cerr << "--> BJetTaggingNormalise: no run found" << std::endl;
	    return;

	  }
	  if (debugLevel > 1) {
	    std::cout << "--> BJetTaggingNormalise: Getting run no. " << run_dir << std::endl;
	  }

	  // Setting the branch for JetTagging
	  TString diag_dir = run_dir + "/JetTagging";

	  TDirectory* dir(0);
	  if (debugLevel > 1) {
	    std::cout << "--> BJetTaggingNormalise: directory " << diag_dir << std::endl;
	  }
	  if (!(dir = f->GetDirectory(diag_dir))) {
	    std::cerr << "--> BJetTaggingNormalise: directory " << diag_dir << " not found."
		      << std::endl;
	    return;
	  }
	  if (debugLevel > 1) {
	    std::cout << "--> BJetTaggingNormalise: Setting the to process histgrams"
		      << std::endl;
	  }

	  // 2D histograms

	  std::vector < TString > nomHistosNames;
	  std::vector < TString > denHistosNames;

	  nomHistosNames.push_back("track_selector_eff");
	  nomHistosNames.push_back("track_selector_eff_LS"); // added by SARA
	  //nomHistosNames.push_back("jet_2D_kinematic"); // removed by SARA
	  nomHistosNames.push_back("ip3d_tag_def_rate_2D");
	  //nomHistosNames.push_back("ip2d_tag_pos_rate_2D");
	  nomHistosNames.push_back("ip3d_tag_neg_rate_2D");
	  nomHistosNames.push_back("ip3d_tag_pos_rate_2D");
	  //nomHistosNames.push_back("sv1_tag_neg_rate_2D");
	  //nomHistosNames.push_back("sv1_tag_pos_rate_2D");
	  //nomHistosNames.push_back("sv2_tag_neg_rate_2D");
	  //nomHistosNames.push_back("sv2_tag_pos_rate_2D");
	  nomHistosNames.push_back("tracks_pTMin_2D");
	  nomHistosNames.push_back("tracks_d0Max_2D");
	  nomHistosNames.push_back("tracks_z0Max_2D");
	  nomHistosNames.push_back("tracks_sigd0Max_2D");
	  nomHistosNames.push_back("tracks_sigz0Max_2D");
	  nomHistosNames.push_back("tracks_etaMax_2D");
	  nomHistosNames.push_back("tracks_nHitBLayer_2D");
	  nomHistosNames.push_back("tracks_deadBLayer_2D");
	  nomHistosNames.push_back("tracks_nHitPix_2D");
	  nomHistosNames.push_back("tracks_nHitSct_2D");
	  nomHistosNames.push_back("tracks_nHitSi_2D");
	  nomHistosNames.push_back("tracks_nHitTrt_2D");
	  nomHistosNames.push_back("tracks_nHitTrtHighE_2D");
	  nomHistosNames.push_back("tracks_fitChi2_2D");
	  nomHistosNames.push_back("tracks_fitProb_2D");
	  nomHistosNames.push_back("tracks_fitChi2OnNdfMax_2D");

	  denHistosNames.push_back("track_selector_all");
	  denHistosNames.push_back("track_selector_all_LS"); // added by SARA
	  //denHistosNames.push_back("jet_2D_all"); // removed by SARA
          denHistosNames.push_back("jet_2D_all");
          denHistosNames.push_back("jet_2D_all");
          denHistosNames.push_back("jet_2D_all");
	  //denHistosNames.push_back("track_selector_all");
	  //denHistosNames.push_back("track_selector_all");
	  //denHistosNames.push_back("track_selector_all");
	  //denHistosNames.push_back("track_selector_all");
	  //denHistosNames.push_back("track_selector_all");
	  //denHistosNames.push_back("track_selector_all");
	  //denHistosNames.push_back("track_selector_all");
	  //denHistosNames.push_back("track_selector_all");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  denHistosNames.push_back("tracks_all_2D");
	  //denHistosNames.push_back("tracks_all_2D"); // removed by SARA (was orphen)

	  TH2F* workingHistogramNom(0);
	  TH2F* workingHistogramDen(0);

	  TString nomHistos, workingHistogramNameNom;
	  TString denHistos, workingHistogramNameDen;

	  for (unsigned int i = 0; i < nomHistosNames.size(); i++) {

	    workingHistogramNameNom = (nomHistosNames[i]);
	    workingHistogramNameDen = (denHistosNames[i]);
	    nomHistos = diag_dir + "/" + workingHistogramNameNom;
	    denHistos = diag_dir + "/" + workingHistogramNameDen;
	    //std::cout << "--> BJetTaggingNormalise: histogram " << nomHistos << std::endl;


	    //	 std::cout << "--> BJetTaggingNormalise: Doing the normalisation" << std::endl;
	    if (!f->Get(nomHistos)) {
	      if (debugLevel > 0) {
		std::cerr << "--> BBJetTaggingNormalise: no such histogram " << nomHistos
			  << std::endl;
	      }
	      continue;
	    }
	    if (!f->Get(denHistos)) {
	      if (debugLevel > 0) {
		std::cerr << "--> BJetTaggingNormalise: no such histogram " << denHistos
			  << std::endl;
	      }
	      continue;

	    }

	    workingHistogramNom = dynamic_cast<TH2F*> (f->Get(nomHistos));
	    workingHistogramDen = dynamic_cast<TH2F*> (f->Get(denHistos));

	    if (workingHistogramNom == 0 || workingHistogramDen == 0) {
	      continue;
	    }

	    /*
	     * Here the bins are initialised and the upper and lower end from the histogram data
	     * are used as new borders of the updated histogram.
	     * */

	    {
	      if (debugLevel > 1) {
		std::cout << nomHistos << "/" << denHistos << " integral before "
			  << workingHistogramNom->Integral() << std::endl;
	      }
	      workingHistogramNom->Divide(workingHistogramDen);

	      if (debugLevel > 1) {
		std::cout << "integral after " << workingHistogramNom->Integral() << std::endl;
	      }
	    }
	    dir->cd();
	    workingHistogramNom->Write("", TObject::kOverwrite);

	    // for eta/Pt range
	  } // end loop over 2D histograms

	  // 1D histograms

	  std::vector < TString > nom1DHistosNames; // added by SARA
	  std::vector < TString > den1DHistosNames; // added by SARA
	  std::vector < TString > eff1DHistosNames; // added by SARA

	  nom1DHistosNames.push_back("jet_top_tagged"); // added by SARA
	  nom1DHistosNames.push_back("jet_pt_top_tagged"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_phi_sum60OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_phi_sum70OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_phi_sum77OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_phi_sum85OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_eta_sum60OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_eta_sum70OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_eta_sum77OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_eta_sum85OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_sj_phi_sum60OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_sj_phi_sum70OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_sj_phi_sum77OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_sj_phi_sum85OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_sj_eta_sum60OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_sj_eta_sum70OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_sj_eta_sum77OP"); // added by SARA
	  nom1DHistosNames.push_back("tag_MV_w_sj_eta_sum85OP"); // added by SARA

	  den1DHistosNames.push_back("jet_top"); // added by SARA
	  den1DHistosNames.push_back("jet_pt_top"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_phi_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_phi_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_phi_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_phi_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_eta_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_eta_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_eta_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_eta_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_sj_phi_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_sj_phi_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_sj_phi_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_sj_phi_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_sj_eta_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_sj_eta_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_sj_eta_sumAll"); // added by SARA
	  den1DHistosNames.push_back("tag_MV_w_sj_eta_sumAll"); // added by SARA

	  eff1DHistosNames.push_back("jet_top_eff"); // added by SARA
	  eff1DHistosNames.push_back("jet_pt_top_eff"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_phi_frac60OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_phi_frac70OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_phi_frac77OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_phi_frac85OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_eta_frac60OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_eta_frac70OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_eta_frac77OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_eta_frac85OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_sj_phi_frac60OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_sj_phi_frac70OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_sj_phi_frac77OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_sj_phi_frac85OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_sj_eta_frac60OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_sj_eta_frac70OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_sj_eta_frac77OP"); // added by SARA
	  eff1DHistosNames.push_back("tag_MV_w_sj_eta_frac85OP"); // added by SARA

	  TH1F* working1DHistogramNom(0);
	  TH1F* working1DHistogramDen(0);
	  TH1F* working1DHistogramEff(0);

	  TString nom1DHistos, working1DHistogramNameNom;
	  TString den1DHistos, working1DHistogramNameDen;
	  TString eff1DHistos, working1DHistogramNameEff;

	  for (unsigned int i = 0; i < nom1DHistosNames.size(); i++) {

	    working1DHistogramNameNom = (nom1DHistosNames[i]);
	    working1DHistogramNameDen = (den1DHistosNames[i]);
	    working1DHistogramNameEff = (eff1DHistosNames[i]);
	    nom1DHistos = diag_dir + "/" + working1DHistogramNameNom;
	    den1DHistos = diag_dir + "/" + working1DHistogramNameDen;
	    eff1DHistos = diag_dir + "/" + working1DHistogramNameEff;
	    //std::cout << "--> BJetTaggingNormalise: 1D histogram " << nom1DHistos << std::endl;


	    //	 std::cout << "--> BJetTaggingNormalise: Doing the 1D normalisation" << std::endl;
	    if (!f->Get(nom1DHistos)) {
	      if (debugLevel > 0) {
		std::cerr << "--> BBJetTaggingNormalise: no such 1D histogram " << nom1DHistos
			  << std::endl;
	      }
	      continue;
	    }
	    if (!f->Get(den1DHistos)) {
	      if (debugLevel > 0) {
		std::cerr << "--> BJetTaggingNormalise: no such 1D histogram " << den1DHistos
			  << std::endl;
	      }
	      continue;
	    }
	    if (!f->Get(eff1DHistos)) {
	      if (debugLevel > 0) {
		std::cerr << "--> BJetTaggingNormalise: no such 1D histogram " << eff1DHistos
			  << std::endl;
	      }
	      continue;
	    }

	    working1DHistogramNom = dynamic_cast<TH1F*> (f->Get(nom1DHistos));
	    working1DHistogramDen = dynamic_cast<TH1F*> (f->Get(den1DHistos));
	    working1DHistogramEff = dynamic_cast<TH1F*> (f->Get(eff1DHistos));

	    if (working1DHistogramNom == 0 || working1DHistogramDen == 0) {
	      continue;
	    }

	    /*
	     * Here the bins are initialised and the upper and lower end from the histogram data
	     * are used as new borders of the updated histogram.
	     * */

	    {
	      if (debugLevel > 1) {
		std::cout << nom1DHistos << "/" << den1DHistos << " integral before "
			  << working1DHistogramNom->Integral() << std::endl;
	      }
	      working1DHistogramEff->Divide(working1DHistogramNom,working1DHistogramDen,1.,1.,"B");

	      if (debugLevel > 1) {
		std::cout << "integral after " << working1DHistogramEff->Integral() << std::endl;
	      }
	    }
	    dir->cd();
	    working1DHistogramEff->Write("", TObject::kOverwrite);

	    // for eta/Pt range
	  } // end loop over 1D histograms

	  if (debugLevel > 1) {
	    std::cout << "--> BJetTaggingNormalise: Finished" << std::endl;
	  }
	}//tdir_run != 0
      }//while
    }//MonitoringFile::BJetTaggingNormalise
  }
  
}//namespace
