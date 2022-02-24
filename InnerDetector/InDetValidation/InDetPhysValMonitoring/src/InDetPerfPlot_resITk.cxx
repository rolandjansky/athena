/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file InDetPerfPlot_resITk.cxx
 * @author nora pettersson
 **/

#include "InDetPerfPlot_resITk.h"
// #include <iostream>
#include <utility>
#include <cmath>
#include "TFile.h"

InDetPerfPlot_resITk::InDetPerfPlot_resITk(InDetPlotBase* pParent, const std::string& sDir)  : InDetPlotBase(pParent, sDir),
  m_resolutionMethod(IDPVM::ResolutionHelper::iterRMS_convergence),
  m_primTrk(false),
  m_secdTrk(false),
  m_allTrk(false),
  m_fillITkTailsPlots(false),
  m_trkP{},
  m_truetrkP{},
  m_trkErrP{},

  m_resP{},
  m_pullP{},
  m_sigP{},
  m_resITk_pull{},
  m_resITk_res{},
  m_resITk_reco{},
  m_resITk_true{},
  m_resITk_sigma{},

  m_resITk_chargeID{},
  m_resITk_chargeID_vs_eta{},
  m_resITk_chargeID_vs_pt{},
  m_resITk_chargeID_vs_phi{},

  m_resITk_tail_vs_eta{},
  m_resITk_tail_vs_pt{},

  m_resITk_tailfrac_vs_eta{},
  m_resITk_tailfrac_vs_pt{},

  m_eta_tail{},
  m_pT_tail{},

  m_InnermostLayerHits_tail{},
  m_InnermostLayerOutliers_tail{},
  m_InnermostLayerSharedHits_tail{},
  m_InnermostLayerSplitHits_tail{},
  m_NextToInnermostLayerHits_tail{},
  m_NextToInnermostLayerOutliers_tail{},
  m_NextToInnermostLayerSharedHits_tail{},
  m_NextToInnermostLayerSplitHits_tail{},
  m_PixelHits_tail{},
  m_PixelHoles_tail{},
  m_PixelOutliers_tail{},
  m_PixelContribLayers_tail{},
  m_PixelSharedHits_tail{},
  m_PixelSplitHits_tail{},
  m_PixelGangedHits_tail{},
  m_PixelGangedHitsFlaggedFakes_tail{},
  m_SCTHits_tail{},
  m_SCTHoles_tail{},
  m_SCTOutliers_tail{},
  m_SCTDoubleHoles_tail{},
  m_SCTSharedHits_tail{},

  m_resITk_chargeID_chgvschg{},

  m_resITk_resHelpereta{},
  m_resITk_Resolution_vs_eta{},
  m_resITk_ResProjections_vs_eta{},

  m_resITk_resHelpereta_pos{},
  m_resITk_Resolution_vs_eta_pos{},
  m_resITk_resHelpereta_neg{},
  m_resITk_Resolution_vs_eta_neg{},

  m_resITk_resHelperpt{},
  m_resITk_Resolution_vs_pt{},
  m_resITk_ResProjections_vs_pt{},

  m_resITk_resHelperpt_pos{},
  m_resITk_Resolution_vs_pt_pos{},
  m_resITk_resHelperpt_neg{},
  m_resITk_Resolution_vs_pt_neg{},

//  m_resITk_resHelperetapt{},
  m_resITk_pullHelperpt{},
  m_resITk_pullHelpereta{},

  m_resITk_pullResolution_vs_pt{},
  m_resITk_pullResolution_vs_eta{},

  m_resITk_pullProjections_vs_pt{},
  m_resITk_pullProjections_vs_eta{},

  m_resITk_Resolution_vs_pt_EtaBin{},
  m_resITk_Resolution_vs_eta_PtBin{},
  m_resITk_meanProfeta{},
  m_resITk_meanProfpt{},
  m_resITk_sigmaVsEta{},
  m_trk_chi2ndof{},
  m_trk_chi2ndof_vs_eta{},
  m_trk_chi2ndof_vs_totHits{},
  m_trk_chi2ndof_vs_totHits_prob{},

  m_trk_chi2{},
  m_trk_ndof{},

  m_significance_d0{},
  m_significance_z0{},

  m_significance_d0_vs_eta{},
  m_significance_z0_vs_eta{} {
    
  TString tsDir = (TString) sDir;

  if (tsDir.Contains("Primary")) {
    m_primTrk = true; // control if sec/prim from init
  } else if (tsDir.Contains("Secondary")) {
    m_secdTrk = true;
  } else {
    m_allTrk = true;
  }

  // Temporary until method/bins/Etc is established
  // Using globally defined bin limits
  for (int ieta = 0; ieta <= m_nEtaBins; ieta++) {
    m_EtaBins[ieta] = m_etaMin + ((m_etaMax - m_etaMin) / m_nEtaBins) * ieta;
  }
  // Parameter definitions
  m_paramProp[D0].paraName = std::string("d0");
  m_paramProp[D0].paraLabel = std::string("d_{0}");
  m_paramProp[D0].paraUnit = std::string("[mm]");
  m_paramProp[D0].useLogLinBins = true;
  m_paramProp[D0].nBinsRes = 1000;
  m_paramProp[D0].limRes = {
    3e-4, 30
  };
  m_paramProp[D0].nBinsPrp = 200;
  m_paramProp[D0].limPrp = {
    -0.4, 0.4
  };
  m_paramProp[D0].nBinsSig = 1000;
  m_paramProp[D0].limSig = {
    0.005, 0.1
  };
  m_paramProp[D0].resUnit = std::string("[#mum]");

  m_paramProp[Z0].paraName = std::string("z0");
  m_paramProp[Z0].paraLabel = std::string("z_{0}");
  m_paramProp[Z0].paraUnit = std::string("[mm]");
  m_paramProp[Z0].useLogLinBins = true;
  m_paramProp[Z0].nBinsRes = 1000;
  m_paramProp[Z0].limRes = {
    1e-3, 200.0
  };
  m_paramProp[Z0].nBinsPrp = 200;
  m_paramProp[Z0].limPrp = {
    -150.0, 150.0
  };
  m_paramProp[Z0].nBinsSig = 1000;
  m_paramProp[Z0].limSig = {
    0.005, 0.1
  };
  m_paramProp[Z0].resUnit = std::string("[#mum]");

  m_paramProp[Z0SIN].paraName = std::string("z0sin");
  m_paramProp[Z0SIN].paraLabel = std::string("z_{0}#times sin(#theta)");
  m_paramProp[Z0SIN].paraUnit = std::string("[mm]");
  m_paramProp[Z0SIN].nBinsRes = 1000;
  m_paramProp[Z0SIN].useLogLinBins = true;
  m_paramProp[Z0SIN].limRes = {
    5e-4,5
  };
  m_paramProp[Z0SIN].nBinsPrp = 200;
  m_paramProp[Z0SIN].limPrp = {
    -150.0, 150.0
  };
  m_paramProp[Z0SIN].nBinsSig = 200;
  m_paramProp[Z0SIN].limSig = {
    0.005, 0.04
  };
  m_paramProp[Z0SIN].resUnit = std::string("[#mum]");


  m_paramProp[QOVERP].paraName = std::string("qoverp");
  m_paramProp[QOVERP].paraLabel = std::string("(q/p)");
  m_paramProp[QOVERP].paraUnit = std::string("[MeV^{-1}]");
  m_paramProp[QOVERP].nBinsRes = 2000;
  m_paramProp[QOVERP].useLogLinBins = true;
  m_paramProp[QOVERP].limRes = {
    1e-7, 1e-2
  };
  m_paramProp[QOVERP].nBinsPrp = 200;
  m_paramProp[QOVERP].limPrp = {
    -2.5e-5, 2.5e-5
  };
  m_paramProp[QOVERP].nBinsSig = 100;
  m_paramProp[QOVERP].limSig = {
    0.0, 1.0e-6
  };
  m_paramProp[QOVERP].resUnit = std::string("[MeV^{-1}]");

  m_paramProp[QOVERPT].paraName = std::string("qoverpt");
  m_paramProp[QOVERPT].paraLabel = std::string("(1/p_{T})");
  m_paramProp[QOVERPT].paraUnit = std::string("[MeV^{-1}]");
  m_paramProp[QOVERPT].nBinsRes = 2000;
  m_paramProp[QOVERPT].useLogLinBins = true;
  // note that this (and only this!) is measured as a RELATIVE resolution! 
  m_paramProp[QOVERPT].limRes = {
   1e-5,50
  };
  m_paramProp[QOVERPT].nBinsPrp = 200;
  m_paramProp[QOVERPT].limPrp = {
    -1e-2, 1e-2
  };
  m_paramProp[QOVERPT].nBinsSig = 100;
  m_paramProp[QOVERPT].limSig = {
    0.0, 1.0e-6
  };
  m_paramProp[QOVERPT].resUnit = std::string("[MeV^{-1}]");

  m_paramProp[THETA].paraName = std::string("theta");
  m_paramProp[THETA].paraLabel = std::string("#theta");
  m_paramProp[THETA].paraUnit = std::string("[rad]");
  m_paramProp[THETA].nBinsRes = 2000;
  m_paramProp[THETA].useLogLinBins = true;
  m_paramProp[THETA].limRes = {
   1e-7,1e-1
  };
  m_paramProp[THETA].nBinsPrp = 200;
  m_paramProp[THETA].limPrp = {
    0.0, 3.14
  };
  m_paramProp[THETA].nBinsSig = 150;
  m_paramProp[THETA].limSig = {
    0.0, 0.0005
  };
  m_paramProp[THETA].resUnit = std::string("[rad]");

  m_paramProp[PHI].paraName = std::string("phi");
  m_paramProp[PHI].paraLabel = std::string("#phi");
  m_paramProp[PHI].paraUnit = std::string("[rad]");
  m_paramProp[PHI].nBinsRes = 1000;
  m_paramProp[PHI].useLogLinBins = true;
  m_paramProp[PHI].limRes = {
    1e-5,1e-1
  };
  m_paramProp[PHI].nBinsPrp = 60;
  m_paramProp[PHI].limPrp = {
    -3.14, 3.14
  };
  m_paramProp[PHI].nBinsSig = 200;
  m_paramProp[PHI].limSig = {
    0.0, 0.0005
  };
  m_paramProp[PHI].resUnit = std::string("[rad]");

  m_paramProp[PT].paraName = std::string("pt");
  m_paramProp[PT].paraLabel = std::string("p_{T}");
  m_paramProp[PT].paraUnit = std::string("[GeV]");
  m_paramProp[PT].nBinsRes = 2000;
  m_paramProp[PT].useLogLinBins = true;
  m_paramProp[PT].limRes = {
    1e-3,1000
  };
  m_paramProp[PT].nBinsPrp = 200;
  m_paramProp[PT].limPrp = {
    0.0, 110.0
  };
  m_paramProp[PT].nBinsSig = 200;
  m_paramProp[PT].limSig = {
    0.0, 0.1
  };
  m_paramProp[PT].resUnit = std::string("[GeV]");
}

void
InDetPerfPlot_resITk::fillITkTailsPlots(bool filltail) {
  m_fillITkTailsPlots = filltail;
}

void
InDetPerfPlot_resITk::inResoFileForTailPlots(std::string resoFileName) {
  m_resoFileName = resoFileName;

  if (m_fillITkTailsPlots) {
    m_resoFile = std::shared_ptr<TFile> (new TFile(m_resoFileName.c_str(), "READ"));
  }
}

void
InDetPerfPlot_resITk::initializePlots() {
  
  // Bins for resolutions
  // If different from "m_EtaBins" leads to incorrect mapping in eta bins giving asymmetric resolutions
  int nBinsEta = m_nEtaBins;
  float nMinEta = m_etaMin;
  float nMaxEta = m_etaMax;

  int nBinsPt = m_nPtBins;
  float nMinPt = 0.;
  float nMaxPt = 50.0;

  Float_t* ptlimits = m_PtBins.data();

  for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {
    ///pull
    std::string tmpName = "pull_" + m_paramProp[iparam].paraName;
    std::string tmpTitle = tmpName + "; (" + m_paramProp[iparam].paraLabel + "^{reco}-" +
                           m_paramProp[iparam].paraLabel +
                           "^{true})/#sigma_{" + m_paramProp[iparam].paraLabel + "}";
    m_resITk_pull[iparam] = Book1D(tmpName, tmpTitle, 200, -5.0, 5.0, false);
    // res
    // setup in case we want a log-linear reso binning
    std::vector<Double_t> aux_loglin_x;
    if (m_paramProp[iparam].useLogLinBins) {
      ATH_MSG_DEBUG("Booking a loglinear resolution binning for "<<m_paramProp[iparam].paraName);
      aux_loglin_x = populateLogLinearBinning(m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(0), m_paramProp[iparam].limRes.at(1),true);
    }

    tmpName = "res_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; " + m_paramProp[iparam].paraLabel + "^{reco}-" + m_paramProp[iparam].paraLabel +
               "^{true} / " + m_paramProp[iparam].paraLabel + "^{true}";
    //       paramProp[iparam].paraUnit;
   if (m_paramProp[iparam].useLogLinBins) {
      m_resITk_res[iparam] = Book1D(tmpName, tmpTitle, aux_loglin_x, false);
   }
   else {
      m_resITk_res[iparam] = Book1D(tmpName, tmpTitle, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(
                                    0), m_paramProp[iparam].limRes.at(1), false);
    }
    // reco
    tmpName = "reco_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; " + m_paramProp[iparam].paraLabel + "^{reco} " + m_paramProp[iparam].paraUnit;
    m_resITk_reco[iparam] = Book1D(tmpName, tmpTitle, m_paramProp[iparam].nBinsPrp, m_paramProp[iparam].limPrp.at(
                                     0), m_paramProp[iparam].limPrp.at(1), false);
    // true
    tmpName = "true_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; " + m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;
    m_resITk_true[iparam] = Book1D(tmpName, tmpTitle, m_paramProp[iparam].nBinsPrp, m_paramProp[iparam].limPrp.at(
                                     0), m_paramProp[iparam].limPrp.at(1), false);
    // sigma
    tmpName = "sigma_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; #sigma_{" + m_paramProp[iparam].paraLabel + "} " + m_paramProp[iparam].paraUnit;
    m_resITk_sigma[iparam] = Book1D(tmpName, tmpTitle, m_paramProp[iparam].nBinsSig, m_paramProp[iparam].limSig.at(
                                      0), m_paramProp[iparam].limSig.at(1), false);

    tmpName = "tail_vs_eta_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track #eta;";
    m_resITk_tail_vs_eta[iparam] = Book2D(tmpName, tmpTitle, m_nEtaBins, nMinEta, nMaxEta, 2, 0., 2., false);
    m_resITk_tail_vs_eta[iparam]->GetYaxis()->SetBinLabel(1, "Core");
    m_resITk_tail_vs_eta[iparam]->GetYaxis()->SetBinLabel(2, "Tail");

    tmpName = "tail_vs_pt_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track p_{T} [GeV];";
    m_resITk_tail_vs_pt[iparam] = Book2D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, 2, 0., 2., false);
    m_resITk_tail_vs_pt[iparam]->GetXaxis()->Set(nBinsPt,ptlimits);
    m_resITk_tail_vs_pt[iparam]->GetYaxis()->SetBinLabel(1, "Core");
    m_resITk_tail_vs_pt[iparam]->GetYaxis()->SetBinLabel(2, "Tail");

    tmpName = "tailfrac_vs_eta_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track #eta; " + "N_{Tails}/N_{Total}";
    m_resITk_tailfrac_vs_eta[iparam] = BookTEfficiency(tmpName, tmpTitle, m_nEtaBins, nMinEta, nMaxEta, false);

    for (int nptbins = 0; nptbins <= m_nPtBins; nptbins++) {
      m_PtBins_eff[nptbins] = m_PtBins[nptbins];
    }
    tmpName = "tailfrac_vs_pt_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track p_{T} [GeV];";
    m_resITk_tailfrac_vs_pt[iparam] = BookTEfficiency(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, false);
    m_resITk_tailfrac_vs_pt[iparam]->SetBins(nBinsPt,m_PtBins_eff.data());

    if (m_fillITkTailsPlots) {
      tmpName = "eta_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; true track #eta;";
      m_eta_tail[iparam] = Book1D(tmpName, tmpTitle, m_nEtaBins, nMinEta, nMaxEta, false);
      
      tmpName = "pt_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; true track p_{T} [GeV];";
      m_pT_tail[iparam] = Book1D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, false);
      m_pT_tail[iparam]->SetBins(nBinsPt,m_PtBins_eff.data());

      tmpName = "HitContent_NInnermostLayerHits_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Innermost Layer Clusters";
      m_InnermostLayerHits_tail[iparam] = Book1D(tmpName, tmpTitle, 21, -0.5, 20.5, false);
      
      tmpName = "HitContent_NInnermostLayerOutliers_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Innermost Layer Outliers";
      m_InnermostLayerOutliers_tail[iparam] = Book1D(tmpName, tmpTitle, 21, -0.5, 20.5, false);
      
      tmpName = "HitContent_NInnermostLayerSharedHits_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Innermost Layer Shared Clusters";
      m_InnermostLayerSharedHits_tail[iparam] = Book1D(tmpName, tmpTitle, 21, -0.5, 20.5, false);
      
      tmpName = "HitContent_NInnermostLayerSplitHits_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Innermost Layer Split Clusters";
      m_InnermostLayerSplitHits_tail[iparam] = Book1D(tmpName, tmpTitle, 21, -0.5, 20.5, false);
      
      tmpName = "HitContent_NNextToInnermostLayerHits_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Next-To-Innermost Layer clusters";
      m_NextToInnermostLayerHits_tail[iparam] = Book1D(tmpName, tmpTitle, 21, -0.5, 20.5, false);
      
      tmpName = "HitContent_NNextToInnermostLayerOutliers_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Next-To-Innermost Layer Outliers";
      m_NextToInnermostLayerOutliers_tail[iparam] = Book1D(tmpName, tmpTitle, 21, -0.5, 20.5, false);
      
      tmpName = "HitContent_NNextToInnermostLayerSharedHits_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Next-To-Innermost Layer Shared Clusters";
      m_NextToInnermostLayerSharedHits_tail[iparam] = Book1D(tmpName, tmpTitle, 21, -0.5, 20.5, false);
      
      tmpName = "HitContent_NNextToInnermostLayerSplitHits_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Next-To-Innermost Layer Split Clusters";
      m_NextToInnermostLayerSplitHits_tail[iparam] = Book1D(tmpName, tmpTitle, 21, -0.5, 20.5, false);
      
      tmpName = "HitContent_NPixelHits_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Clusters";
      m_PixelHits_tail[iparam] = Book1D(tmpName, tmpTitle, 50, -0.5, 49.5, false);
      
      tmpName = "HitContent_NPixelHoles_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Holes";
      m_PixelHoles_tail[iparam] = Book1D(tmpName, tmpTitle, 10, -0.5, 9.5, false);
      
      tmpName = "HitContent_NPixelOutliers_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Outliers";
      m_PixelOutliers_tail[iparam] = Book1D(tmpName, tmpTitle, 10, -0.5, 9.5, false);
      
      tmpName = "HitContent_NPixelContribLayers_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Layers";
      m_PixelContribLayers_tail[iparam] = Book1D(tmpName, tmpTitle, 10, -0.5, 9.5, false);
      
      tmpName = "HitContent_NPixelSharedHits_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Shared Clusters";
      m_PixelSharedHits_tail[iparam] = Book1D(tmpName, tmpTitle, 10, -0.5, 9.5, false);
      
      tmpName = "HitContent_NPixelSplitHits_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Split Clusters";
      m_PixelSplitHits_tail[iparam] = Book1D(tmpName, tmpTitle, 5, -0.5, 4.5, false);
      
      tmpName = "HitContent_NPixelGangedHits_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Clusters";
      m_PixelGangedHits_tail[iparam] = Book1D(tmpName, tmpTitle, 10, -0.5, 9.5, false);
      
      tmpName = "HitContent_NPixelGangedHitsFlaggedFakes_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; #eta";
      m_PixelGangedHitsFlaggedFakes_tail[iparam] = Book1D(tmpName, tmpTitle, 80, -4, 4, false);
      
      tmpName = "HitContent_NSCTHits_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Clusters";
      m_SCTHits_tail[iparam] = Book1D(tmpName, tmpTitle, 41, -0.5, 40.5, false);
      
      tmpName = "HitContent_NSCTHoles_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Holes";
      m_SCTHoles_tail[iparam] = Book1D(tmpName, tmpTitle, 21, -0.5, 20.5, false);
      
      tmpName = "HitContent_NSCTOutliers_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Outliers";
      m_SCTOutliers_tail[iparam] = Book1D(tmpName, tmpTitle, 21, -0.5, 20.5, false);
      
      tmpName = "HitContent_NSCTDoubleHoles_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Double Holes";
      m_SCTDoubleHoles_tail[iparam] = Book1D(tmpName, tmpTitle, 21, -0.5, 20.5, false);
      
      tmpName = "HitContent_NSCTSharedHits_tail_" + m_paramProp[iparam].paraName;
      tmpTitle = tmpName + "; Number of Shared Clusters";
      m_SCTSharedHits_tail[iparam] = Book1D(tmpName, tmpTitle, 21, -0.5, 20.5, false);
    }

    // res versus eta
    tmpName = "resHelpereta_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track #eta; " + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;

    if (m_paramProp[iparam].useLogLinBins) {   
      m_resITk_resHelpereta[iparam] = Book2D(tmpName, tmpTitle, m_nEtaBins, nMinEta, nMaxEta, aux_loglin_x, false); 
    }
    else {
    m_resITk_resHelpereta[iparam] = Book2D(tmpName, tmpTitle, m_nEtaBins, nMinEta, nMaxEta, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(0), m_paramProp[iparam].limRes.at(1), false);
    }

    for (unsigned int ibins = 0; ibins < m_nEtaBins; ibins++) {
      tmpName = "EtaProjections_resProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1);
      tmpTitle = "resProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1) + "; " +
                 m_paramProp[iparam].paraLabel + "^{reco}-" + m_paramProp[iparam].paraLabel + "^{true} " +
                 m_paramProp[iparam].paraUnit;

    if (m_paramProp[iparam].useLogLinBins) { 
      m_resITk_ResProjections_vs_eta[iparam][ibins] =
        Book1D(tmpName, tmpTitle, aux_loglin_x, false);
    }
    else {
      m_resITk_ResProjections_vs_eta[iparam][ibins] =
        Book1D(tmpName, tmpTitle, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(0),
               m_paramProp[iparam].limRes.at(1), false);
      }
    }
    std::string ytitle[4] = {
      " resolution ", " mean ", " resolution ", " mean "
    };
    std::string ytitlePull[4] = {
      " pull width ", " pull mean ", " pull width ", " pull mean "
    };

    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_eta";
      tmpTitle = tmpName + "; true track #eta; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                 m_paramProp[iparam].resUnit;
      m_resITk_Resolution_vs_eta[iparam][ires] = Book1D(tmpName, tmpTitle, m_nEtaBins, -4.0, 4.0, false);
    }


    tmpName = "resHelpereta_pos" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track #eta; " + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;
    

    if (m_paramProp[iparam].useLogLinBins) { 
      m_resITk_resHelpereta_pos[iparam] = Book2D(tmpName, tmpTitle, nBinsEta, nMinEta, nMaxEta,
                                               aux_loglin_x, false);
    }
    else {
      m_resITk_resHelpereta_pos[iparam] = Book2D(tmpName, tmpTitle, nBinsEta, nMinEta, nMaxEta,
                                               m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(0),
                                               m_paramProp[iparam].limRes.at(1), false);
    }
    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_eta_pos";
      tmpTitle = tmpName + "; true track #eta; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                 m_paramProp[iparam].resUnit;
      m_resITk_Resolution_vs_eta_pos[iparam][ires] = Book1D(tmpName, tmpTitle, m_nEtaBins, -4.0, 4.0, false);
    }
    tmpName = "resHelpereta_neg" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track #eta; " + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;
    if (m_paramProp[iparam].useLogLinBins) { 
      m_resITk_resHelpereta_neg[iparam] = Book2D(tmpName, tmpTitle, nBinsEta, nMinEta, nMaxEta,
                                                 aux_loglin_x, false);
    }
    else { 
      m_resITk_resHelpereta_neg[iparam] = Book2D(tmpName, tmpTitle, nBinsEta, nMinEta, nMaxEta,
                                                 m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(0),
                                                 m_paramProp[iparam].limRes.at(1), false);
    }
    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_eta_neg";
      tmpTitle = tmpName + "; true track #eta; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                 m_paramProp[iparam].resUnit;
      m_resITk_Resolution_vs_eta_neg[iparam][ires] = Book1D(tmpName, tmpTitle, m_nEtaBins, -4.0, 4.0, false);
    }

    // res versus pt
    tmpName = "resHelperpt_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;

    if (m_paramProp[iparam].useLogLinBins) {  
      m_resITk_resHelperpt[iparam] =
        Book2D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, aux_loglin_x, false);
    }
    else {
      m_resITk_resHelperpt[iparam] =
        Book2D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(
                 0), m_paramProp[iparam].limRes.at(1), false);
    }
    m_resITk_resHelperpt[iparam]->GetXaxis()->Set(nBinsPt,ptlimits);


    std::vector<double> binsForPull = populateLogLinearBinning(1000,2e-2,100,true);
    tmpName = "pullHelperpt_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track p_{T} [GeV]; (" + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true})/#sigma_{" + m_paramProp[iparam].paraLabel + "}";
    m_resITk_pullHelperpt[iparam] = Book2D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, binsForPull, false);
    m_resITk_pullHelperpt[iparam]->GetXaxis()->Set(nBinsPt,ptlimits);

    tmpName = "pullHelpereta_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track #eta; (" + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true})/#sigma_{" + m_paramProp[iparam].paraLabel + "}";
    m_resITk_pullHelpereta[iparam] = Book2D(tmpName, tmpTitle, nBinsEta, nMinEta, nMaxEta, binsForPull, false);


    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + "pull" + m_resHisto[ires] + "_vs_pt";
      tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + ytitlePull[ires];
      m_resITk_pullResolution_vs_pt[iparam][ires] = Book1D(tmpName, tmpTitle, m_nPtBins, 0., 50.0, false);
      m_resITk_pullResolution_vs_pt[iparam][ires]->GetXaxis()->Set(m_nPtBins, m_PtBins.data());
    }

    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + "pull" + m_resHisto[ires] + "_vs_eta";
      tmpTitle = tmpName + "; true track #eta; " + m_paramProp[iparam].paraLabel + ytitlePull[ires];
      m_resITk_pullResolution_vs_eta[iparam][ires] = Book1D(tmpName, tmpTitle, m_nEtaBins, 0., 50.0, false);
      m_resITk_pullResolution_vs_eta[iparam][ires]->GetXaxis()->Set(m_nEtaBins, m_EtaBins.data());
    }


    for (unsigned int ibins = 0; ibins < m_nPtBins; ibins++) {
      tmpName = "PtProjections_pullProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1);
      tmpTitle = tmpName + "; (" + m_paramProp[iparam].paraLabel + "^{reco}-" + m_paramProp[iparam].paraLabel +
                 "^{true})/#sigma_{" + m_paramProp[iparam].paraLabel + "}";
      m_resITk_pullProjections_vs_pt[iparam][ibins] = Book1D(tmpName, tmpTitle, binsForPull, false);
    }

    for (unsigned int ibins = 0; ibins < m_nEtaBins; ibins++) {
      tmpName = "EtaProjections_pullProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1);
      tmpTitle = tmpName + "; (" + m_paramProp[iparam].paraLabel + "^{reco}-" + m_paramProp[iparam].paraLabel +
                 "^{true})/#sigma_{" + m_paramProp[iparam].paraLabel + "}";
      m_resITk_pullProjections_vs_eta[iparam][ibins] = Book1D(tmpName, tmpTitle, binsForPull, false);
    }
    for (unsigned int ibins = 0; ibins < m_nPtBins; ibins++) {
      tmpName = "PtProjections_resProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1);
      tmpTitle = tmpName + "; " + m_paramProp[iparam].paraLabel + "^{reco}-" + m_paramProp[iparam].paraLabel +
                 "^{true} " +
                 m_paramProp[iparam].paraUnit;
      if (m_paramProp[iparam].useLogLinBins){
        m_resITk_ResProjections_vs_pt[iparam][ibins] =
          Book1D(tmpName, tmpTitle, aux_loglin_x, false);
      }
      else {
        m_resITk_ResProjections_vs_pt[iparam][ibins] =
          Book1D(tmpName, tmpTitle, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(0),
                 m_paramProp[iparam].limRes.at(1), false);
      }
    }
    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_pt";
      tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                 m_paramProp[iparam].resUnit;
      m_resITk_Resolution_vs_pt[iparam][ires] = Book1D(tmpName, tmpTitle, m_nPtBins, 0., 50.0, false);
      m_resITk_Resolution_vs_pt[iparam][ires]->GetXaxis()->Set(m_nPtBins, m_PtBins.data());
    }
    tmpName = "resHelperpt_pos" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;
    if (m_paramProp[iparam].useLogLinBins){
      m_resITk_resHelperpt_pos[iparam] =
        Book2D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, aux_loglin_x, false);
    }
    else {
      m_resITk_resHelperpt_pos[iparam] =
        Book2D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(
                 0), m_paramProp[iparam].limRes.at(1), false);
      }
    m_resITk_resHelperpt_pos[iparam]->GetXaxis()->Set(nBinsPt,ptlimits);
    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_pt_pos";
      tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                 m_paramProp[iparam].resUnit;
      m_resITk_Resolution_vs_pt_pos[iparam][ires] = Book1D(tmpName, tmpTitle, m_nPtBins, 0., 50.0, false);
      m_resITk_Resolution_vs_pt_pos[iparam][ires]->GetXaxis()->Set(m_nPtBins, m_PtBins.data());
    }
    tmpName = "resHelperpt_neg" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;

    if (m_paramProp[iparam].useLogLinBins){
      m_resITk_resHelperpt_neg[iparam] =
        Book2D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, aux_loglin_x, false);
    }
    else {
      m_resITk_resHelperpt_neg[iparam] =
        Book2D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(
                 0), m_paramProp[iparam].limRes.at(1), false);
    }
    m_resITk_resHelperpt_neg[iparam]->GetXaxis()->Set(nBinsPt,ptlimits);
    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_pt_neg";
      tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                 m_paramProp[iparam].resUnit;
      m_resITk_Resolution_vs_pt_neg[iparam][ires] = Book1D(tmpName, tmpTitle, m_nPtBins, 0., 50.0, false);
      m_resITk_Resolution_vs_pt_neg[iparam][ires]->GetXaxis()->Set(m_nPtBins, m_PtBins.data());
    }
    // res versus eta pt
    tmpName = "resHelperetapt_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track p_{T}; true track #eta; " + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;

    for (unsigned int ires = 0; ires < 2; ires++) {
      for (int ibin = 0; ibin < 4; ibin++) {
        int tmpInt = ibin + 1;
        tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_pt_EtaBin" + std::to_string(tmpInt);
        tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                   m_paramProp[iparam].resUnit;
        m_resITk_Resolution_vs_pt_EtaBin[iparam][ibin][ires] = Book1D(tmpName, tmpTitle, m_nPtBins, 0., 50.0, false);
	m_resITk_Resolution_vs_pt_EtaBin[iparam][ibin][ires]->GetXaxis()->Set(m_nPtBins, m_PtBins.data());
      }
      for (int iPtBin = 0; iPtBin < 4; iPtBin++) {
        tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_eta_PtBin" + std::to_string(iPtBin + 1);
        tmpTitle = tmpName + "; true track #eta; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                   m_paramProp[iparam].resUnit;
        m_resITk_Resolution_vs_eta_PtBin[iparam][iPtBin][ires] =
          Book1D(tmpName, tmpTitle, m_nEtaBins, -4.0, 4.0, false);
      }
    }

    tmpName = "meanProfeta_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track #eta; #sigma_{" + m_paramProp[iparam].paraLabel + "} " +
               m_paramProp[iparam].paraUnit;
    m_resITk_meanProfeta[iparam] = BookTProfile(tmpName, tmpTitle, nBinsEta, nMinEta, nMaxEta, -1.0, 1.0, false);

    tmpName = "SigmaVsEta_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track #eta; #sigma_{" + m_paramProp[iparam].paraLabel + "} " +
               m_paramProp[iparam].paraUnit;
    m_resITk_sigmaVsEta[iparam] = Book2D(tmpName, tmpTitle, nBinsEta, nMinEta, nMaxEta, m_paramProp[iparam].nBinsSig,
                                         -1.0, 1.0, false);

    tmpName = "meanProfpt_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track p_{T}; #sigma_{" + m_paramProp[iparam].paraLabel + "} " +
               m_paramProp[iparam].paraUnit;
    m_resITk_meanProfpt[iparam] = BookTProfile(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, -1.0, 1.0, false);
    m_resITk_meanProfpt[iparam]->GetXaxis()->Set(nBinsPt,ptlimits);

  }

  m_resITk_chargeID = Book1D("chargeID", "chargeID", 2, 0., 2., false);
  m_resITk_chargeID->GetXaxis()->SetBinLabel(1, "ID");
  m_resITk_chargeID->GetXaxis()->SetBinLabel(2, "misID");
  m_resITk_chargeID_chgvschg = Book2D("chargeID_chgvschg", "chargeID_chgvschg; charge_{reco}; charge_{true}", 5, -2.0,
                                      2.0, 5, -2.0, 2.0, false);

  m_resITk_chargeID_vs_eta = Book1D("chargeID_vs_eta", "chargeID_vs_eta; truth track #eta; Fraction of misID [%]",
                                    nBinsEta, nMinEta, nMaxEta, false);
  m_resITk_chargeID_vs_pt = Book1D("chargeID_vs_pt", "chargeID_vs_pt; truth track p_{T} [GeV]; Fraction of misID [%]",
                                   nBinsPt, nMaxPt, nMaxPt, false);
  m_resITk_chargeID_vs_pt->GetXaxis()->Set(nBinsPt,ptlimits);

  m_trk_chi2ndof = Book1D("trk_chi2ndof", "trk_chi2ndof; #chi_{0}/ndof", 200, 0., 20, false);
  m_trk_chi2 = Book1D("trk_chi2", "trk_chi2; #chi^{2}/ndof", 200, 0., 200, false);
  m_trk_ndof = Book1D("trk_ndof", "trk_ndof; #chi^{2}/ndof", 200, 0., 200, false);
  m_trk_chi2ndof_vs_eta = BookTProfile("trk_chi2ndof_vs_eta", "trk_chi2ndof_vs_eta; #eta; #chi^{2}/ndof", nBinsEta,
                                       nMinEta, nMaxEta, -1.0, 1.0, false);
  m_trk_chi2ndof_vs_totHits = BookTProfile("trk_chi2ndof_vs_totHits",
                                           "trk_chi2ndof_vs_totHits; number of Total Hits; #chi^{2}/ndof", 30, 0, 30,
                                           -1.0, 1.0, false);
  m_trk_chi2ndof_vs_totHits_prob = BookTProfile("trk_chi2ndof_vs_totHits_prob",
                                                "trk_chi2ndof_vs_totHits_prob; number of Total Hits; #chi^{2}/ndof", 30,
                                                0, 30, -1.0, 1.0, false);

  m_significance_d0 = Book2D("significance_d0", "d0 significance w.r.t pv; #eta; d_{0}/#sigma_{d_{0}}", 320, -4.0, 4.0,
                             150, -5.0, 5.0, false);
  m_significance_z0 = Book2D("significance_z0", "z0 significance w.r.t pv; #eta; z_{0}/#sigma_{z_{0}}", 320, -4.0, 4.0,
                             150, -5.0, 5.0, false);

  m_significance_d0_vs_eta = Book1D("significance_d0_vs_eta", "d0 significance with w.r.t pv; #eta; d_{0} sign width",
                                    m_nEtaBins, -4.0, 4.0, false);
  m_significance_z0_vs_eta = Book1D("significance_z0_vs_eta", "z0 significance with w.r.t pv; #eta; z_{0} sign width",
                                    m_nEtaBins, -4.0, 4.0, false);

}

void
InDetPerfPlot_resITk::fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt) {
  // Check whether the track is primary or secondary
  int trueBC = -9999;

  trueBC = truthprt.barcode();
  int isPrimTrk = 0;
  int isSecdTrk = 0;
  if ((trueBC > 0) && (trueBC < 200000)) {
    isPrimTrk = 1;
  }
  if ((trueBC > 200000)) {
    isSecdTrk = 1;
  }

  // Move on to the next track incase the wrong track category
  if (!isPrimTrk && !isSecdTrk) {
    return;
  }
  if (!isPrimTrk && m_primTrk) {
    return;
  }
  if (!isSecdTrk && m_secdTrk) {
    return;
  }

  // Get track paramters for truth and reco tracks for easy access
  // store all in track parameter maps trkP/truetrkP
  getTrackParameters(trkprt);
  getTrackParameters(truthprt);
  getPlotParameters();
  getPlots();
  uint8_t iPixHits, iSCTHits;

  int pixHits = 0;
  int sctHits = 0;
  if (trkprt.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
    pixHits = iPixHits;
  }
  if (trkprt.summaryValue(iSCTHits, xAOD::numberOfSCTHits)) {
    sctHits = iSCTHits;
  }
  float chi2ndof = 1e-7;
  if (trkprt.chiSquared() / trkprt.numberDoF() > chi2ndof) {
    chi2ndof = trkprt.chiSquared() / trkprt.numberDoF();
  }
  m_trk_chi2ndof->Fill(chi2ndof);
  m_trk_chi2ndof_vs_eta->Fill(trkprt.eta(), chi2ndof);
  m_trk_chi2ndof_vs_totHits->Fill(pixHits + sctHits, chi2ndof);
  m_trk_chi2ndof_vs_totHits_prob->Fill(pixHits + sctHits, TMath::Prob(trkprt.chiSquared(), trkprt.numberDoF()));

  m_trk_chi2->Fill(trkprt.chiSquared());
  m_trk_ndof->Fill(trkprt.numberDoF());

  if (m_fillITkTailsPlots) {
    uint8_t iInnermostHits, iInnermostOutliers, iInnermostShared, iInnermostSplit, iNextToInnermostHits, iNextToInnermostOutliers, iNextToInnermostShared, iNextToInnermostSplit, iPixHoles, iPixelOutliers, iPixelContribLayers, iPixelShared, iPixelSplit, iPixelGanged, iPixelGangedFF, iSCTHoles, iSCTOutliers, iSCTDoubleHoles, iSCTShared;
    
    float eta = truthprt.eta();
    
    for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {
      std::string hReso_name = std::string("IDPerformanceMon/Tracks/SelectedMatchedTracks/Primary/") + m_paramProp[iparam].paraName + std::string("resolutionRMS_vs_eta");
      
      std::shared_ptr<TH1D> hReso {static_cast<TH1D*>(m_resoFile->Get(hReso_name.c_str()))};
      
      for (unsigned int ieta = 1; ieta <= m_nEtaBins; ieta++) {	
        if (eta >= m_EtaBins[ieta-1] && eta < m_EtaBins[ieta]) {
	  double param_trkReso_eta;

	  if (iparam == D0 || iparam == Z0 || iparam == Z0SIN) {
	    param_trkReso_eta = hReso->GetBinContent(ieta)/1000.; //Conversion factor between resolutions and m_resITk_resHelper
	  }
	  else {
	    param_trkReso_eta = hReso->GetBinContent(ieta);
	  }
	  
	  double cutoff = 3.0 * param_trkReso_eta;
	  if (fabs(m_resP[iparam]) > cutoff) {
	    m_eta_tail[iparam]->Fill(eta);
	    m_pT_tail[iparam]->Fill(m_truetrkP[PT]);
	    
            if (trkprt.summaryValue(iInnermostHits, xAOD::numberOfInnermostPixelLayerHits)) {
              m_InnermostLayerHits_tail[iparam]->Fill(iInnermostHits);
            }
	    if (trkprt.summaryValue(iInnermostOutliers, xAOD::numberOfInnermostPixelLayerOutliers)) {
	      m_InnermostLayerOutliers_tail[iparam]->Fill(iInnermostOutliers);
	    }
	    if (trkprt.summaryValue(iInnermostShared, xAOD::numberOfInnermostPixelLayerSharedHits)) {
	      m_InnermostLayerSharedHits_tail[iparam]->Fill(iInnermostShared);
	    }
	    if (trkprt.summaryValue(iInnermostSplit, xAOD::numberOfInnermostPixelLayerSplitHits)) {
	      m_InnermostLayerSplitHits_tail[iparam]->Fill(iInnermostSplit);
	    }
	    if (trkprt.summaryValue(iNextToInnermostHits, xAOD::numberOfNextToInnermostPixelLayerHits)) {
	      m_NextToInnermostLayerHits_tail[iparam]->Fill(iNextToInnermostHits);
	    }
	    if (trkprt.summaryValue(iNextToInnermostOutliers, xAOD::numberOfNextToInnermostPixelLayerOutliers)) {
	      m_NextToInnermostLayerOutliers_tail[iparam]->Fill(iNextToInnermostOutliers);
	    }
	    if (trkprt.summaryValue(iNextToInnermostShared, xAOD::numberOfNextToInnermostPixelLayerSharedHits)) {
	      m_NextToInnermostLayerSharedHits_tail[iparam]->Fill(iNextToInnermostShared);
	    }
	    if (trkprt.summaryValue(iNextToInnermostSplit, xAOD::numberOfNextToInnermostPixelLayerSplitHits)) {
	      m_NextToInnermostLayerSplitHits_tail[iparam]->Fill(iNextToInnermostSplit);
	    }
	    if (trkprt.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
	      m_PixelHits_tail[iparam]->Fill(iPixHits);
	    }
	    if (trkprt.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
	      m_PixelHoles_tail[iparam]->Fill(iPixHoles);
	    }
	    if (trkprt.summaryValue(iPixelOutliers, xAOD::numberOfPixelOutliers)) {
	      m_PixelOutliers_tail[iparam]->Fill(iPixelOutliers);
	    }
	    if (trkprt.summaryValue(iPixelContribLayers, xAOD::numberOfContribPixelLayers)) {
	      m_PixelContribLayers_tail[iparam]->Fill(iPixelContribLayers);
	    }
	    if (trkprt.summaryValue(iPixelShared, xAOD::numberOfPixelSharedHits)) {
	      m_PixelSharedHits_tail[iparam]->Fill(iPixelShared);
	    }
	    if (trkprt.summaryValue(iPixelSplit, xAOD::numberOfPixelSplitHits)) {
	      m_PixelSplitHits_tail[iparam]->Fill(iPixelSplit);
	    }
	    if (trkprt.summaryValue(iPixelGanged, xAOD::numberOfGangedPixels)) {
	      m_PixelGangedHits_tail[iparam]->Fill(iPixelGanged);
	    }
	    if (trkprt.summaryValue(iPixelGangedFF, xAOD::numberOfGangedFlaggedFakes)) {
	      m_PixelGangedHitsFlaggedFakes_tail[iparam]->Fill(iPixelGangedFF);
	    }
	    if (trkprt.summaryValue(iSCTHits, xAOD::numberOfSCTHits)) {
	      m_SCTHits_tail[iparam]->Fill(iSCTHits);
	    }
	    if (trkprt.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
	      m_SCTHoles_tail[iparam]->Fill(iSCTHoles);
	    }
	    if (trkprt.summaryValue(iSCTOutliers, xAOD::numberOfSCTOutliers)) {
	      m_SCTOutliers_tail[iparam]->Fill(iSCTOutliers);
	    }
	    if (trkprt.summaryValue(iSCTDoubleHoles, xAOD::numberOfSCTDoubleHoles)) {
	      m_SCTDoubleHoles_tail[iparam]->Fill(iSCTDoubleHoles);
	    }
	    if (trkprt.summaryValue(iSCTShared, xAOD::numberOfSCTSharedHits)) {
	      m_SCTSharedHits_tail[iparam]->Fill(iSCTShared);
	    }	  
	  }
	}
      }
    }
  }
}

void
InDetPerfPlot_resITk::getPlots() {
  float eta = -std::log(std::tan(m_truetrkP[THETA] / 2));

  for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {
    m_resITk_pull[iparam]->Fill(m_pullP[iparam]);
    m_resITk_res[iparam]->Fill(m_resP[iparam]);
    m_resITk_reco[iparam]->Fill(m_trkP[iparam]);
    m_resITk_true[iparam]->Fill(m_truetrkP[iparam]);
    m_resITk_sigma[iparam]->Fill(m_sigP[iparam]);

    m_resITk_meanProfeta[iparam]->Fill(eta, m_sigP[iparam]);
    m_resITk_sigmaVsEta[iparam]->Fill(eta, m_sigP[iparam]);

    m_resITk_meanProfpt[iparam]->Fill(m_truetrkP[PT], m_sigP[iparam]);
    m_resITk_resHelpereta[iparam]->Fill(eta, m_resP[iparam]);
    m_resITk_resHelperpt[iparam]->Fill(m_truetrkP[PT], m_resP[iparam]);
//    m_resITk_resHelperetapt[iparam]->Fill(m_truetrkP[PT], eta, m_resP[iparam]);

    m_resITk_pullHelperpt[iparam]->Fill(m_truetrkP[PT], m_pullP[iparam]);
    m_resITk_pullHelpereta[iparam]->Fill(eta, m_pullP[iparam]);

    if (m_trkP[QOVERP] >= 0.) {
      m_resITk_resHelpereta_pos[iparam]->Fill(eta, m_resP[iparam]);
      m_resITk_resHelperpt_pos[iparam]->Fill(m_truetrkP[PT], m_resP[iparam]);
    }
    if (m_trkP[QOVERP] < 0.) {
      m_resITk_resHelpereta_neg[iparam]->Fill(eta, m_resP[iparam]);
      m_resITk_resHelperpt_neg[iparam]->Fill(m_truetrkP[PT], m_resP[iparam]);
    }

    if (iparam == QOVERP) {
      if (m_trkP[QOVERP] / m_truetrkP[QOVERP] > 0.) {
        m_resITk_chargeID->Fill(0.);
      }
      if (m_trkP[QOVERP] / m_truetrkP[QOVERP] < 0.) {
        m_resITk_chargeID->Fill(1.);
        m_resITk_chargeID_vs_eta->Fill(eta);
        m_resITk_chargeID_vs_pt->Fill(m_truetrkP[PT]);

        m_resITk_chargeID_chgvschg->Fill(m_trkP[QOVERP] / fabs(m_trkP[QOVERP]),
                                         m_truetrkP[QOVERP] / fabs(m_truetrkP[QOVERP]));
      }
    }

  }

  m_significance_d0->Fill(eta, m_trkP[D0]);
  m_significance_z0->Fill(eta, m_trkP[Z0]);
}

void
InDetPerfPlot_resITk::getPlotParameters() {
  for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {
    m_resP[iparam] = m_trkP[iparam] - m_truetrkP[iparam];
    // if(iparam == PT) m_resP[iparam] = (trkP[iparam] - m_truetrkP[iparam]);
    m_sigP[iparam] = m_trkErrP[iparam];
    (m_sigP[iparam] != 0) ? m_pullP[iparam] = m_resP[iparam] / m_sigP[iparam] : m_pullP[iparam] = -9999.;
  }
  m_resP[QOVERPT] = (m_trkP[QOVERPT] - m_truetrkP[QOVERPT]) * (1 / m_truetrkP[QOVERPT]);
}

void
InDetPerfPlot_resITk::getTrackParameters(const xAOD::TrackParticle& trkprt) {
  // std::cout << "---------------------------" << std::endl;
  // std::cout << trkprt.vx() << "	" << trkprt.vy() << "	" << trkprt.vz() << std::endl;
  // std::cout << trkprt.beamlineTiltX() << "	" << trkprt.beamlineTiltY() << "	" << trkprt.beamlineTiltX()<<
  // std::endl;
  // std::cout << "---------------------------" << std::endl;

  // std::cout << trkprt.patternRecoInfo() << std::endl;
  // std::cout << trkprt.trackFitter() << std::endl;
  // std::cout << trkprt.track()->info().dumpInfo() << std::endl;
  // Track perigee, BS?
  // std::cout << trkprt.track()->info().trackProperties(Trk::TrackInfo::BremFit) << std::endl;
  // std::cout << trkprt.trackProperties(xAOD::BremFit) << std::endl;

  m_trkP[D0] = trkprt.d0();
  m_trkP[Z0] = trkprt.z0();
  m_trkP[QOVERP] = trkprt.qOverP();
  m_trkP[QOVERPT] = trkprt.qOverP() * (1 / std::sin(trkprt.theta()));
  m_trkP[THETA] = trkprt.theta();
  m_trkP[PHI] = trkprt.phi0();
  m_trkP[PT] = trkprt.pt() / 1000.;
  m_trkP[Z0SIN] = trkprt.z0() * std::sin(trkprt.theta());


  // Track fit errors
  m_trkErrP[D0] = std::sqrt(trkprt.definingParametersCovMatrix()(0, 0));
  m_trkErrP[Z0] = std::sqrt(trkprt.definingParametersCovMatrix()(1, 1));
  m_trkErrP[PHI] = std::sqrt(trkprt.definingParametersCovMatrix()(2, 2));
  m_trkErrP[THETA] = std::sqrt(trkprt.definingParametersCovMatrix()(3, 3));
  m_trkErrP[QOVERP] = std::sqrt(trkprt.definingParametersCovMatrix()(4, 4));
  m_trkErrP[QOVERPT] = m_trkErrP[QOVERP] * (1 / std::sin(trkprt.theta()));
  m_trkErrP[Z0SIN] = std::sqrt(std::pow(m_trkErrP[Z0] * std::sin(m_trkP[THETA]), 2) + 
                               std::pow(m_trkP[Z0] * m_trkErrP[THETA] * std::cos(m_trkP[THETA]), 2) + 
                               2 * m_trkP[Z0] * std::sin(m_trkP[THETA]) * std::cos(m_trkP[THETA]) * trkprt.definingParametersCovMatrix()(1, 3));  // Fixing incorrect formula for z0sin error

  // Get error on pT, taken from xAOD::TrackingHelpers.pTErr() but this function only works on a pointer input...
  if (trkprt.definingParametersCovMatrixVec().size() < 15) {
    throw std::runtime_error(
            "TrackParticle without covariance matrix for defining parameters or the covariance matrix is wrong dimensionality.");
  }
  if (std::abs(trkprt.qOverP()) < 0) {
    m_trkErrP[PT] = 0.0;
    throw std::runtime_error("q/p is zero");
  } else {
    double pt = trkprt.pt();
    double diff_qp = -pt / std::abs(trkprt.qOverP());
    double diff_theta = pt / tan(trkprt.theta());
    const std::vector<float>& cov = trkprt.definingParametersCovMatrixVec();
    double pt_err2 = diff_qp * (diff_qp * cov[14] + diff_theta * cov[13]) + diff_theta * diff_theta * cov[9];
    m_trkErrP[PT] = sqrt(pt_err2) / 1000.;
  }
}

void
InDetPerfPlot_resITk::getTrackParameters(const xAOD::TruthParticle& truthprt) {
  // "d0", "z0", "phi", "theta" , "qOverP"
  // Perigee for truth particles are in aux container
  int nParams = 6;

  for (int iParams = 0; iParams < nParams; iParams++) {
    m_truetrkP[iParams] = -9999.;
    if (truthprt.isAvailable<float>(m_paramProp[iParams].paraName)) {
      m_truetrkP[iParams] = (truthprt.auxdata<float>(m_paramProp[iParams].paraName));
    }
  }

  (truthprt.isAvailable<float>("d0")) ? m_truetrkP[D0] = truthprt.auxdata<float>("d0") : m_truetrkP[D0] = -9999.;
  (truthprt.isAvailable<float>("z0")) ? m_truetrkP[Z0] = truthprt.auxdata<float>("z0") : m_truetrkP[Z0] = -9999.;
  (truthprt.isAvailable<float>("qOverP")) ? m_truetrkP[QOVERP] =
    truthprt.auxdata<float>("qOverP") : m_truetrkP[QOVERP] =
      -9999.;
  (truthprt.isAvailable<float>("theta")) ? m_truetrkP[THETA] = truthprt.auxdata<float>("theta") : m_truetrkP[THETA] =
                                                                 -9999.;
  (truthprt.isAvailable<float>("phi")) ? m_truetrkP[PHI] = truthprt.auxdata<float>("phi") : m_truetrkP[PHI] = -9999.;
  (truthprt.isAvailable<float>("theta") &&
   truthprt.isAvailable<float>("qOverP")) ? m_truetrkP[QOVERPT] = truthprt.auxdata<float>("qOverP") *
                                                                  (1 /
                                                                   std::sin(truthprt.auxdata<float>("theta"))) :
                                                                  m_truetrkP[QOVERPT] = -9999.;


  m_truetrkP[PT] = truthprt.pt() / 1000.;
  (truthprt.isAvailable<float>("z0") &&
   truthprt.isAvailable<float>("theta")) ? m_truetrkP[Z0SIN] = m_truetrkP[Z0] *
                                                               std::sin(m_truetrkP[THETA]) : m_truetrkP[Z0SIN] =
                                                                 -9999.;
}

void
InDetPerfPlot_resITk::finalizePlots() {
  for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {    
    m_resolutionHelper.makeResolutions(m_resITk_resHelpereta[iparam], m_resITk_Resolution_vs_eta[iparam][0], m_resITk_Resolution_vs_eta[iparam][1],
				       m_resITk_ResProjections_vs_eta[iparam], true, m_resolutionMethod);
    m_resolutionHelper.makeResolutions(m_resITk_resHelperpt[iparam], m_resITk_Resolution_vs_pt[iparam][0], m_resITk_Resolution_vs_pt[iparam][1],
				       m_resITk_ResProjections_vs_pt[iparam], true, m_resolutionMethod);
    m_resolutionHelper.makeResolutions(m_resITk_resHelperpt_pos[iparam], m_resITk_Resolution_vs_pt_pos[iparam][0], m_resITk_Resolution_vs_pt_pos[iparam][1], m_resolutionMethod);
    m_resolutionHelper.makeResolutions(m_resITk_resHelperpt_neg[iparam], m_resITk_Resolution_vs_pt_neg[iparam][0], m_resITk_Resolution_vs_pt_neg[iparam][1], m_resolutionMethod);
    m_resolutionHelper.makeResolutions(m_resITk_resHelpereta_pos[iparam], m_resITk_Resolution_vs_eta_pos[iparam][0], m_resITk_Resolution_vs_eta_pos[iparam][1], m_resolutionMethod);
    m_resolutionHelper.makeResolutions(m_resITk_resHelpereta_neg[iparam], m_resITk_Resolution_vs_eta_neg[iparam][0], m_resITk_Resolution_vs_eta_neg[iparam][1], m_resolutionMethod);
    
    // add for the pull vs pT
    m_resolutionHelper.makeResolutions(m_resITk_pullHelperpt[iparam], m_resITk_pullResolution_vs_pt[iparam][0], m_resITk_pullResolution_vs_pt[iparam][1],
				       m_resITk_pullProjections_vs_pt[iparam], true, m_resolutionMethod);
    m_resolutionHelper.makeResolutions(m_resITk_pullHelpereta[iparam], m_resITk_pullResolution_vs_eta[iparam][0], m_resITk_pullResolution_vs_eta[iparam][1],
				       m_resITk_pullProjections_vs_eta[iparam], true, m_resolutionMethod);
  }

  for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
    TH1D* tmp = (TH1D*) m_significance_d0->ProjectionY("tmp_py", m_significance_d0->GetXaxis()->FindBin(
                                                         m_EtaBins[ieta]),
                                                       m_significance_d0->GetXaxis()->FindBin(m_EtaBins[ieta + 1]));
    m_significance_d0_vs_eta->SetBinContent(ieta + 1, tmp->GetRMS());
    m_significance_d0_vs_eta->SetBinError(ieta + 1, tmp->GetRMSError());
  }
  for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
    TH1D* tmp = (TH1D*) m_significance_z0->ProjectionY("tmp_py", m_significance_z0->GetXaxis()->FindBin(
                                                         m_EtaBins[ieta]),
                                                       m_significance_z0->GetXaxis()->FindBin(m_EtaBins[ieta + 1]));
    m_significance_z0_vs_eta->SetBinContent(ieta + 1, tmp->GetRMS());
    m_significance_z0_vs_eta->SetBinError(ieta + 1, tmp->GetRMSError());
  }

  for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {
    for (unsigned int ieta = 1; ieta <= m_nEtaBins; ieta++) {
      double param_reso_eta;
      if (iparam == D0 || iparam == Z0 || iparam == Z0SIN) {
	param_reso_eta = m_resITk_Resolution_vs_eta[iparam][0]->GetBinContent(ieta)/1000.; //Conversion factor between resolutions and m_resITk_resHelper
      }
      else {
	param_reso_eta = m_resITk_Resolution_vs_eta[iparam][0]->GetBinContent(ieta);
      }
      
      std::shared_ptr<TH1D> h_py_eta {m_resITk_resHelpereta[iparam]->ProjectionY("h_py_eta", ieta, ieta)};
      
      double cutoff_eta = 3.0 * param_reso_eta;
      int min_bin_eta = h_py_eta->FindBin(-cutoff_eta);
      int max_bin_eta = h_py_eta->FindBin(cutoff_eta);
      
      double Error_nentries_core_eta = 0;
      double nentries_core_eta = h_py_eta->IntegralAndError(min_bin_eta, max_bin_eta, Error_nentries_core_eta);
      m_resITk_tail_vs_eta[iparam]->SetBinContent(ieta+1,1,nentries_core_eta);
      m_resITk_tail_vs_eta[iparam]->SetBinError(ieta+1,1,Error_nentries_core_eta);
      
      double Error_nentries_tail_eta = 0;
      double nentries_tail_eta = h_py_eta->IntegralAndError(1, min_bin_eta, Error_nentries_tail_eta) + h_py_eta->IntegralAndError(max_bin_eta, h_py_eta->GetNbinsX(), Error_nentries_tail_eta);
      m_resITk_tail_vs_eta[iparam]->SetBinContent(ieta+1,2,nentries_tail_eta);
      m_resITk_tail_vs_eta[iparam]->SetBinError(ieta+1,2,Error_nentries_tail_eta);
    }
    
    for (unsigned int ipt = 1; ipt <= m_nPtBins; ipt++) {
      double param_reso_pt;
      if (iparam == D0 || iparam == Z0 || iparam == Z0SIN) {
	param_reso_pt = m_resITk_Resolution_vs_pt[iparam][0]->GetBinContent(ipt)/1000.; //Conversion factor between resolutions and m_resITk_resHelper
      }
      else {
	param_reso_pt = m_resITk_Resolution_vs_pt[iparam][0]->GetBinContent(ipt);
      }
      
      std::shared_ptr<TH1D> h_py_pt {m_resITk_resHelperpt[iparam]->ProjectionY("h_py_pt", ipt, ipt)};
      
      double cutoff_pt = 3.0 * param_reso_pt;
      int min_bin_pt = h_py_pt->FindBin(-cutoff_pt);
      int max_bin_pt = h_py_pt->FindBin(cutoff_pt);
      
      double Error_nentries_core_pt = 0;
      double nentries_core_pt = h_py_pt->IntegralAndError(min_bin_pt, max_bin_pt, Error_nentries_core_pt);
      m_resITk_tail_vs_pt[iparam]->SetBinContent(ipt+1,1,nentries_core_pt);
      m_resITk_tail_vs_pt[iparam]->SetBinError(ipt+1,1,Error_nentries_core_pt);
      
      double Error_nentries_tail_pt = 0;
      double nentries_tail_pt = h_py_pt->IntegralAndError(1, min_bin_pt, Error_nentries_tail_pt) + h_py_pt->IntegralAndError(max_bin_pt, h_py_pt->GetNbinsX(), Error_nentries_tail_pt);
      m_resITk_tail_vs_pt[iparam]->SetBinContent(ipt+1,2,nentries_tail_pt);
      m_resITk_tail_vs_pt[iparam]->SetBinError(ipt+1,2,Error_nentries_tail_pt);
    }
    
    std::shared_ptr<TH1D> hTotal_eta {m_resITk_tail_vs_eta[iparam]->ProjectionX("hTotal_eta", 1, 2)};
    std::shared_ptr<TH1D> hTail_eta {m_resITk_tail_vs_eta[iparam]->ProjectionX("hTail_eta", 2, 2)};
    
    m_resITk_tailfrac_vs_eta[iparam]->SetTotalHistogram(*hTotal_eta, "");
    m_resITk_tailfrac_vs_eta[iparam]->SetPassedHistogram(*hTail_eta, "");
    
    std::string eta_eff_title0 = std::string("tailfrac_vs_eta_") + m_paramProp[iparam].paraName + std::string("; true track #eta; #epsilon = N_{Tail} / N_{Total}");
    const char* eta_eff_title = eta_eff_title0.c_str();
    m_resITk_tailfrac_vs_eta[iparam]->SetTitle(eta_eff_title);
    
    std::shared_ptr<TH1D> hTotal_pt {m_resITk_tail_vs_pt[iparam]->ProjectionX("hTotal_pt", 1, 2)};
    std::shared_ptr<TH1D> hTail_pt {m_resITk_tail_vs_pt[iparam]->ProjectionX("hTail_pt", 2, 2)};
    
    m_resITk_tailfrac_vs_pt[iparam]->SetTotalHistogram(*hTotal_pt, "");
    m_resITk_tailfrac_vs_pt[iparam]->SetPassedHistogram(*hTail_pt, "");
    
    m_resITk_tailfrac_vs_pt[iparam]->SetStatisticOption(TEfficiency::kFCP);
    
    std::string pt_eff_title0 = std::string("tailfrac_vs_pt_") + m_paramProp[iparam].paraName + std::string("; true track p_{T}[GeV]; #epsilon = N_{Tail} / N_{Total}");
    const char* pt_eff_title = pt_eff_title0.c_str();
    m_resITk_tailfrac_vs_pt[iparam]->SetTitle(pt_eff_title);
    
  }

}

void
InDetPerfPlot_resITk::cloneHistogram(TH1D* h, TH1* hcopy) {
  unsigned int nbin = h->GetNbinsX();

  for (unsigned int ibin = 0; ibin <= nbin+1; ++ibin) {
    float binContent = h->GetBinContent(ibin);
    float binError = h->GetBinError(ibin);
    hcopy->SetBinContent(ibin, binContent);
    hcopy->SetBinError(ibin, binError);
  }
}
