/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_resITk.cxx
 * @author nora pettersson
 **/

#include "InDetPerfPlot_resITk.h"
// #include <iostream>
#include <map>

InDetPerfPlot_resITk::InDetPerfPlot_resITk(PlotBase* pParent, const std::string& sDir)  : PlotBase(pParent, sDir),
  m_primTrk(false),
  m_secdTrk(false),
  m_allTrk(false),
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

  m_resITk_momTail{},
  m_resITk_momTail_vs_eta{},
  m_resITk_momTail_vs_pt{},
  m_resITk_momTail_vs_phi{},
  m_resITk_momTail_Frac{},
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
  m_DEBUG_D0dep{},
  m_DEBUG_FirstHitR_d0{},
  m_DEBUG_NOBREM_d0{},
  m_DEBUG_BREM_d0{},

  m_trk_chi2ndof{},
  m_trk_chi2ndof_vs_eta{},
  m_trk_chi2ndof_vs_totHits{},
  m_trk_chi2ndof_vs_totHits_prob{},

  m_trk_chi2{},
  m_trk_ndof{},

  m_significance_d0{},
  m_significance_z0{},

  m_significance_d0_vs_eta{},
  m_significance_z0_vs_eta{},

  m_fix_qoverpt_res{},
  m_fix_qoverptresolutionRMS_vs_eta{},
  m_fix_d0_res{},
  m_fix_d0resolutionRMS_vs_eta{},
  m_fix_z0_res{},
  m_fix_z0resolutionRMS_vs_eta{} {
  TString tsDir = (TString) sDir;

  if (tsDir.Contains("Primary")) {
    m_primTrk = true; // control if sec/prim from init
  } else if (tsDir.Contains("Secondary")) {
    m_secdTrk = true;
  } else {
    m_allTrk = true;
  }

  // Temporary until method/bins/Etc is established
  for (int ieta = 0; ieta <= m_nEtaBins; ieta++) {
    m_EtaBins[ieta] = -4.0 + (8.0 / m_nEtaBins) * ieta;
  }
  // Parameter definitions
  m_paramProp[D0].paraName = std::string("d0");
  m_paramProp[D0].paraLabel = std::string("d_{0}");
  m_paramProp[D0].paraUnit = std::string("[mm]");
  m_paramProp[D0].nBinsRes = 1000;
  m_paramProp[D0].limRes = {
    -1.5, 1.5
  };
  m_paramProp[D0].nBinsPrp = 200;
  m_paramProp[D0].limPrp = {
    -0.4, 0.4
  };
  m_paramProp[D0].nBinsSig = 200;
  m_paramProp[D0].limSig = {
    0.005, 0.04
  };
  m_paramProp[D0].resUnit = std::string("[#mum]");

  m_paramProp[Z0].paraName = std::string("z0");
  m_paramProp[Z0].paraLabel = std::string("z_{0}");
  m_paramProp[Z0].paraUnit = std::string("[mm]");
  m_paramProp[Z0].nBinsRes = 2000;
  m_paramProp[Z0].limRes = {
    -10.0, 10.0
  };
  m_paramProp[Z0].nBinsPrp = 200;
  m_paramProp[Z0].limPrp = {
    -150.0, 150.0
  };
  m_paramProp[Z0].nBinsSig = 200;
  m_paramProp[Z0].limSig = {
    0.005, 0.04
  };
  m_paramProp[Z0].resUnit = std::string("[#mum]");

  m_paramProp[Z0SIN].paraName = std::string("z0sin");
  m_paramProp[Z0SIN].paraLabel = std::string("z_{0}#times sin(#theta)");
  m_paramProp[Z0SIN].paraUnit = std::string("[mm]");
  m_paramProp[Z0SIN].nBinsRes = 1000;
  m_paramProp[Z0SIN].limRes = {
    -0.2, 0.2
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
  m_paramProp[QOVERP].limRes = {
    -2.5e-5, 2.5e-5
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
  m_paramProp[QOVERPT].limRes = {
    -15.0, 15.0
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
  m_paramProp[THETA].nBinsRes = 1000;
  m_paramProp[THETA].limRes = {
    -0.01, 0.01
  };
  m_paramProp[THETA].nBinsPrp = 200;
  m_paramProp[THETA].limPrp = {
    0.0, 3.14
  };
  m_paramProp[THETA].nBinsSig = 150;
  m_paramProp[THETA].limSig = {
    0.0, 0.0005
  };
  m_paramProp[THETA].resUnit = std::string("[mrad]");

  m_paramProp[PHI].paraName = std::string("phi");
  m_paramProp[PHI].paraLabel = std::string("#phi");
  m_paramProp[PHI].paraUnit = std::string("[rad]");
  m_paramProp[PHI].nBinsRes = 1000;
  m_paramProp[PHI].limRes = {
    -0.01, 0.01
  };
  m_paramProp[PHI].nBinsPrp = 60;
  m_paramProp[PHI].limPrp = {
    -3.14, 3.14
  };
  m_paramProp[PHI].nBinsSig = 200;
  m_paramProp[PHI].limSig = {
    0.0, 0.0005
  };
  m_paramProp[PHI].resUnit = std::string("[mrad]");

  m_paramProp[PT].paraName = std::string("pt");
  m_paramProp[PT].paraLabel = std::string("p_{T}");
  m_paramProp[PT].paraUnit = std::string("[GeV]");
  m_paramProp[PT].nBinsRes = 1000;
  m_paramProp[PT].limRes = {
    -100.0, 100.0
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
InDetPerfPlot_resITk::initializePlots() {
  // Bins for resolutions
  int nBinsEta = 50.0;
  float nMinEta = -5.0;
  float nMaxEta = fabs(nMinEta);
  int nBinsPt = 50.0;
  float nMinPt = 0.0;
  float nMaxPt = 500.0;

  Float_t ptlimits[50];
  for (int i=0; i<50; ++i) {
    ptlimits[i] = 1000.0* TMath::Exp((i-49.0)/6.804);
  }

  for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {
    ///pull
    std::string tmpName = "pull_" + m_paramProp[iparam].paraName;
    std::string tmpTitle = tmpName + "; (" + m_paramProp[iparam].paraLabel + "^{reco}-" +
                           m_paramProp[iparam].paraLabel +
                           "^{true})/#sigma_{" + m_paramProp[iparam].paraLabel + "}";
    m_resITk_pull[iparam] = Book1D(tmpName, tmpTitle, 200, -5.0, 5.0, false);
    // res
    tmpName = "res_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; " + m_paramProp[iparam].paraLabel + "^{reco}-" + m_paramProp[iparam].paraLabel +
               "^{true} / " + m_paramProp[iparam].paraLabel + "^{true}";
    //       paramProp[iparam].paraUnit;
    m_resITk_res[iparam] = Book1D(tmpName, tmpTitle, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(
                                    0), m_paramProp[iparam].limRes.at(1), false);
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
    // res versus eta
    tmpName = "resHelpereta_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track #eta; " + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;
    m_resITk_resHelpereta[iparam] =
      Book2D(tmpName, tmpTitle, nBinsEta, nMinEta, nMaxEta, m_paramProp[iparam].nBinsRes,
             m_paramProp[iparam].limRes.at(0), m_paramProp[iparam].limRes.at(1), false);
    for (unsigned int ibins = 0; ibins < m_nEtaBins; ibins++) {
      tmpName = "EtaProjections_resProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1);
      tmpTitle = "resProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1) + "; " +
                 m_paramProp[iparam].paraLabel + "^{reco}-" + m_paramProp[iparam].paraLabel + "^{true} " +
                 m_paramProp[iparam].paraUnit;
      m_resITk_ResProjections_vs_eta[iparam][ibins] =
        Book1D(tmpName, tmpTitle, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(0),
               m_paramProp[iparam].limRes.at(1), false);
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
    m_resITk_resHelpereta_pos[iparam] = Book2D(tmpName, tmpTitle, nBinsEta, nMinEta, nMaxEta,
                                               m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(0),
                                               m_paramProp[iparam].limRes.at(1), false);
    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_eta_pos";
      tmpTitle = tmpName + "; true track #eta; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                 m_paramProp[iparam].resUnit;
      m_resITk_Resolution_vs_eta_pos[iparam][ires] = Book1D(tmpName, tmpTitle, m_nEtaBins, -4.0, 4.0, false);
    }
    tmpName = "resHelpereta_neg" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track #eta; " + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;
    m_resITk_resHelpereta_neg[iparam] = Book2D(tmpName, tmpTitle, nBinsEta, nMinEta, nMaxEta,
                                               m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(0),
                                               m_paramProp[iparam].limRes.at(1), false);
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
    m_resITk_resHelperpt[iparam] =
      Book2D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(
               0), m_paramProp[iparam].limRes.at(1), false);
    m_resITk_resHelperpt[iparam]->GetXaxis()->Set(49,ptlimits);
    tmpName = "pullHelperpt_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track p_{T} [GeV]; (" + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true})/#sigma_{" + m_paramProp[iparam].paraLabel + "}";
    m_resITk_pullHelperpt[iparam] = Book2D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, 200, -10.0, 10.0, false);
    m_resITk_pullHelperpt[iparam]->GetXaxis()->Set(49,ptlimits);
    tmpName = "pullHelpereta_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track #eta; (" + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true})/#sigma_{" + m_paramProp[iparam].paraLabel + "}";
    m_resITk_pullHelpereta[iparam] = Book2D(tmpName, tmpTitle, nBinsEta, nMinEta, nMaxEta, 200, -10.0, 10.0, false);


    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + "pull" + m_resHisto[ires] + "_vs_pt";
      tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + ytitlePull[ires];
      m_resITk_pullResolution_vs_pt[iparam][ires] = Book1D(tmpName, tmpTitle, m_nPtBins, 0., 50.0, false);
      m_resITk_pullResolution_vs_pt[iparam][ires]->GetXaxis()->Set(m_nPtBins, m_PtBins);
    }

    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + "pull" + m_resHisto[ires] + "_vs_eta";
      tmpTitle = tmpName + "; true track #eta; " + m_paramProp[iparam].paraLabel + ytitlePull[ires];
      m_resITk_pullResolution_vs_eta[iparam][ires] = Book1D(tmpName, tmpTitle, m_nEtaBins, 0., 50.0, false);
      m_resITk_pullResolution_vs_eta[iparam][ires]->GetXaxis()->Set(m_nEtaBins, m_EtaBins);
    }


    for (unsigned int ibins = 0; ibins < m_nPtBins; ibins++) {
      tmpName = "PtPullProjections/pullProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1);
      tmpTitle = tmpName + "; (" + m_paramProp[iparam].paraLabel + "^{reco}-" + m_paramProp[iparam].paraLabel +
                 "^{true})/#sigma_{" + m_paramProp[iparam].paraLabel + "}";
      m_resITk_pullProjections_vs_pt[iparam][ibins] = Book1D(tmpName, tmpTitle, 200, -10.0, 10.0, false);
    }

    for (unsigned int ibins = 0; ibins < m_nEtaBins; ibins++) {
      tmpName = "EtaPullProjections/pullProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1);
      tmpTitle = tmpName + "; (" + m_paramProp[iparam].paraLabel + "^{reco}-" + m_paramProp[iparam].paraLabel +
                 "^{true})/#sigma_{" + m_paramProp[iparam].paraLabel + "}";
      m_resITk_pullProjections_vs_eta[iparam][ibins] = Book1D(tmpName, tmpTitle, 200, -10.0, 10.0, false);
    }
    for (unsigned int ibins = 0; ibins < m_nPtBins; ibins++) {
      tmpName = "PtProjections/resProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1);
      tmpTitle = tmpName + "; " + m_paramProp[iparam].paraLabel + "^{reco}-" + m_paramProp[iparam].paraLabel +
                 "^{true} " +
                 m_paramProp[iparam].paraUnit;
      m_resITk_ResProjections_vs_pt[iparam][ibins] =
        Book1D(tmpName, tmpTitle, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(0),
               m_paramProp[iparam].limRes.at(1), false);
    }
    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_pt";
      tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                 m_paramProp[iparam].resUnit;
      m_resITk_Resolution_vs_pt[iparam][ires] = Book1D(tmpName, tmpTitle, m_nPtBins, 0., 50.0, false);
      m_resITk_Resolution_vs_pt[iparam][ires]->GetXaxis()->Set(m_nPtBins, m_PtBins);
    }
    tmpName = "resHelperpt_pos" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;
    m_resITk_resHelperpt_pos[iparam] =
      Book2D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(
               0), m_paramProp[iparam].limRes.at(1), false);
    m_resITk_resHelperpt_pos[iparam]->GetXaxis()->Set(49,ptlimits);
    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_pt_pos";
      tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                 m_paramProp[iparam].resUnit;
      m_resITk_Resolution_vs_pt_pos[iparam][ires] = Book1D(tmpName, tmpTitle, m_nPtBins, 0., 50.0, false);
      m_resITk_Resolution_vs_pt_pos[iparam][ires]->GetXaxis()->Set(m_nPtBins, m_PtBins);
    }
    tmpName = "resHelperpt_neg" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;
    m_resITk_resHelperpt_neg[iparam] =
      Book2D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(
               0), m_paramProp[iparam].limRes.at(1), false);
    m_resITk_resHelperpt_neg[iparam]->GetXaxis()->Set(49,ptlimits);
    for (unsigned int ires = 0; ires < m_nResHist; ires++) {
      tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_pt_neg";
      tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                 m_paramProp[iparam].resUnit;
      m_resITk_Resolution_vs_pt_neg[iparam][ires] = Book1D(tmpName, tmpTitle, m_nPtBins, 0., 50.0, false);
      m_resITk_Resolution_vs_pt_neg[iparam][ires]->GetXaxis()->Set(m_nPtBins, m_PtBins);
    }
    // res versus eta pt
    tmpName = "resHelperetapt_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; true track p_{T}; true track #eta; " + m_paramProp[iparam].paraLabel + "^{reco}-" +
               m_paramProp[iparam].paraLabel + "^{true} " + m_paramProp[iparam].paraUnit;
/*    m_resITk_resHelperetapt[iparam] = Book3D(tmpName, tmpTitle, nBinsPt, nMinPt, nMaxPt, nBinsEta, nMinEta, nMaxEta,
                                             m_paramProp[iparam].nBinsRes, m_paramProp[iparam].limRes.at(0),
                                             m_paramProp[iparam].limRes.at(1), false);
*/
    for (unsigned int ires = 0; ires < 2; ires++) {
      for (int ibin = 0; ibin < 4; ibin++) {
        int tmpInt = ibin + 1;
        tmpName = m_paramProp[iparam].paraName + m_resHisto[ires] + "_vs_pt_EtaBin" + std::to_string(tmpInt);
        tmpTitle = tmpName + "; true track p_{T} [GeV]; " + m_paramProp[iparam].paraLabel + ytitle[ires] +
                   m_paramProp[iparam].resUnit;
        m_resITk_Resolution_vs_pt_EtaBin[iparam][ibin][ires] = Book1D(tmpName, tmpTitle, m_nPtBins, 0., 50.0, false);
        m_resITk_Resolution_vs_pt_EtaBin[iparam][ibin][ires]->GetXaxis()->Set(m_nPtBins, m_PtBins);
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
    m_resITk_meanProfpt[iparam]->GetXaxis()->Set(49,ptlimits);
//    m_resITk_meanProfpt[iparam]->GetXaxis()->Set(m_nPtBins, m_PtBins);

    tmpName = "DEBUG_D0dep_" + m_paramProp[iparam].paraName;
    tmpTitle = tmpName + "; " + m_paramProp[iparam].paraLabel + "^{reco} " + m_paramProp[iparam].paraUnit +
               "; d_{0} [mm]";
    m_DEBUG_D0dep[iparam] = BookTProfile(tmpName, tmpTitle, m_paramProp[iparam].nBinsPrp, m_paramProp[iparam].limPrp.at(
                                           0), m_paramProp[iparam].limPrp.at(1), -0.1, 0.1, false);
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
    m_resITk_chargeID_vs_pt->GetXaxis()->Set(49,ptlimits);
//  m_resITk_chargeID_vs_pt->GetXaxis()->Set(m_nPtBins, m_PtBins);

  m_resITk_momTail = Book1D("momTail", "momTail", 2, 0., 2., false);
  m_resITk_momTail->GetXaxis()->SetBinLabel(1, "Core");
  m_resITk_momTail->GetXaxis()->SetBinLabel(2, "Tail");

  m_resITk_momTail_vs_eta = Book1D("momTail_vs_eta", "momTail_vs_eta; truth track #eta; Fraction of p_{T}-tail [%]",
                                   nBinsEta, nMinEta, nMaxEta, false);
  m_resITk_momTail_vs_pt = Book1D("momTail_vs_pt", "momTail_vs_pt; truth track p_{T} [GeV]; Fraction of p_{T}-tail [%]",
                                  nBinsPt, nMinPt, nMaxPt, false);
    m_resITk_momTail_vs_pt->GetXaxis()->Set(49,ptlimits);
  m_resITk_momTail_vs_pt->GetXaxis()->Set(m_nPtBins, m_PtBins);
  m_resITk_momTail_vs_phi = Book1D("momTail_vs_phi", "momTail_vs_phi; truth track #phi; Fraction of p_{T}-tail [%]",
                                   nBinsEta, nMinEta, nMaxEta, false);

  m_resITk_momTail_Frac = Book1D("momTail_Frac", "momTail_Frac; (p^{reco}_{T}-p^{true}_{T})/p^{true}_{T}", 100, 0.,
                                 100., false);

  m_DEBUG_BREM_d0 = Book1D("DEBUG_BREM_d0", "DEBUG_BREM_d0; d_{0} [mm]", m_paramProp[D0].nBinsPrp, m_paramProp[D0].limPrp.at(
                             0), m_paramProp[D0].limPrp.at(1), false);
  m_DEBUG_NOBREM_d0 = Book1D("DEBUG_NOBREM_d0", "DEBUG_NOBREM_d0; d_{0} [mm]", m_paramProp[D0].nBinsPrp, m_paramProp[D0].limPrp.at(
                               0), m_paramProp[D0].limPrp.at(1), false);

  m_DEBUG_FirstHitR_d0 =
    BookTProfile("DEBUG_FirstHitR_d0", "DEBUG_FirstHitR_d0; R [mm]", 150, 0., 10., -1.0, 1.0, false);

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

// tmp fix for pT resolution
// 16 eta bins
  std::string fixName = "fix_qoverpt_res1";
  m_fix_qoverpt_res[0] = Book1D(fixName, fixName, 600, -15.0, 15.0, false);
  fixName = "fix_qoverpt_res2";
  m_fix_qoverpt_res[1] = Book1D(fixName, fixName, 600, -5.0, 5.0, false);
  fixName = "fix_qoverpt_res3";
  m_fix_qoverpt_res[2] = Book1D(fixName, fixName, 600, -2.0, 2.0, false);
  fixName = "fix_qoverpt_res4";
  m_fix_qoverpt_res[3] = Book1D(fixName, fixName, 600, -1.0, 1.0, false);
  fixName = "fix_qoverpt_res5";
  m_fix_qoverpt_res[4] = Book1D(fixName, fixName, 600, -1.0, 1.0, false);
  fixName = "fix_qoverpt_res6";
  m_fix_qoverpt_res[5] = Book1D(fixName, fixName, 600, -0.5, 0.5, false);
  fixName = "fix_qoverpt_res7";
  m_fix_qoverpt_res[6] = Book1D(fixName, fixName, 600, -0.2, 0.2, false);
  fixName = "fix_qoverpt_res8";
  m_fix_qoverpt_res[7] = Book1D(fixName, fixName, 600, -0.2, 0.2, false);

  fixName = "fix_qoverpt_res16";
  m_fix_qoverpt_res[15] = Book1D(fixName, fixName, 600, -15.0, 15.0, false);
  fixName = "fix_qoverpt_res15";
  m_fix_qoverpt_res[14] = Book1D(fixName, fixName, 600, -5.0, 5.0, false);
  fixName = "fix_qoverpt_res14";
  m_fix_qoverpt_res[13] = Book1D(fixName, fixName, 600, -2.0, 2.0, false);
  fixName = "fix_qoverpt_res13";
  m_fix_qoverpt_res[12] = Book1D(fixName, fixName, 600, -1.0, 1.0, false);
  fixName = "fix_qoverpt_res12";
  m_fix_qoverpt_res[11] = Book1D(fixName, fixName, 600, -1.0, 1.0, false);
  fixName = "fix_qoverpt_res11";
  m_fix_qoverpt_res[10] = Book1D(fixName, fixName, 600, -0.5, 0.5, false);
  fixName = "fix_qoverpt_res10";
  m_fix_qoverpt_res[9] = Book1D(fixName, fixName, 600, -0.2, 0.2, false);
  fixName = "fix_qoverpt_res9";
  m_fix_qoverpt_res[8] = Book1D(fixName, fixName, 600, -0.2, 0.2, false);

  m_fix_qoverptresolutionRMS_vs_eta = Book1D("fix_qoverptresolutionRMS_vs_eta",
                                             "fix_qoverptresolutionRMS_vseta; true particle #eta; p_{T} #times #sigma(1/p_{T})", m_nEtaBins, -4.0, 4.0,
                                             false);



  float rangesd0[16] = {
    1.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.5
  };

  float rangesz0[16] = {
    10.0, 10.0, 8.0, 6.0, 6.0, 4.0, 3.0, 2.0,
    2.0, 3.0, 4.0, 6.0, 6.0, 8.0, 10.0, 10.0
  };

  for (int ieta = 0; ieta < 16; ieta++) {
    fixName = "fix_d0_res" + std::to_string(ieta + 1);
    m_fix_d0_res[ieta] = Book1D(fixName, fixName, 600, -rangesd0[ieta], rangesd0[ieta], false);
  }
  m_fix_d0resolutionRMS_vs_eta = Book1D("fix_d0resolutionRMS_vs_eta",
                                        "fix_d0resolutionRMS_vs_eta; true particle #eta; #sigma(d_{0}) [#mum]",
                                        m_nEtaBins, -4.0, 4.0, false);


  for (int ieta = 0; ieta < 16; ieta++) {
    fixName = "fix_z0_res" + std::to_string(ieta + 1);
    m_fix_z0_res[ieta] = Book1D(fixName, fixName, 600, -rangesz0[ieta], rangesz0[ieta], false);
  }
  m_fix_z0resolutionRMS_vs_eta = Book1D("fix_z0resolutionRMS_vs_eta",
                                        "fix_z0resolutionRMS_vs_eta; true particle #eta; #sigma(z_{0}) [#mum]",
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
}

void
InDetPerfPlot_resITk::getPlots() {
  float eta = -TMath::Log(TMath::Tan(m_truetrkP[THETA] / 2));

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
    // Look at PT tails
    if (iparam == PT) {
      m_resITk_momTail_Frac->Fill((m_trkP[PT] - m_truetrkP[PT]) / m_truetrkP[PT]);
      if ((m_trkP[PT] - m_truetrkP[PT]) / m_truetrkP[PT] > 0.5 && m_truetrkP[PT] > 0.0) {
        m_resITk_momTail->Fill(1.);
        m_resITk_momTail_vs_phi->Fill(m_truetrkP[PHI]);
        m_resITk_momTail_vs_eta->Fill(eta);
        m_resITk_momTail_vs_pt->Fill(m_truetrkP[PT]);
      } else {
        m_resITk_momTail->Fill(0.);
      }
    }

    m_DEBUG_D0dep[iparam]->Fill(m_trkP[iparam], m_trkP[D0]);
  }
  for (int ieta = 0; ieta < m_nEtaBins; ieta++) {
    // std::cout << eta << " " << m_EtaBins[ieta+1] << " " << m_EtaBins[ieta] << std::endl;
    if (eta < m_EtaBins[ieta + 1] && eta > m_EtaBins[ieta]) {
      m_fix_qoverpt_res[ieta]->Fill(m_resP[QOVERPT]);
      m_fix_d0_res[ieta]->Fill(m_resP[D0]);
      m_fix_z0_res[ieta]->Fill(m_resP[Z0]);
      // std::cout << ieta << std::endl;
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
//  std::cout << m_resP[QOVERPT] << std::endl;
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
  m_trkP[QOVERPT] = trkprt.qOverP() * (1 / TMath::Sin(trkprt.theta()));
  m_trkP[THETA] = trkprt.theta();
  m_trkP[PHI] = trkprt.phi0();
  m_trkP[PT] = trkprt.pt() / 1000.;
  m_trkP[Z0SIN] = trkprt.z0() * TMath::Sin(trkprt.theta());


  m_DEBUG_FirstHitR_d0->Fill(trkprt.radiusOfFirstHit(), m_trkP[D0]);
  if (trkprt.track()) {
    if (trkprt.track()->info().trackProperties(Trk::TrackInfo::BremFit) &&
        trkprt.track()->info().trackProperties(Trk::TrackInfo::BremFitSuccessful)) {
      m_DEBUG_BREM_d0->Fill(m_trkP[D0]);
    } else {
      m_DEBUG_NOBREM_d0->Fill(m_trkP[D0]);
    }
  }
  // Track fit errors
  m_trkErrP[D0] = TMath::Sqrt(trkprt.definingParametersCovMatrix()(0, 0));
  m_trkErrP[Z0] = TMath::Sqrt(trkprt.definingParametersCovMatrix()(1, 1));
  m_trkErrP[PHI] = TMath::Sqrt(trkprt.definingParametersCovMatrix()(2, 2));
  m_trkErrP[THETA] = TMath::Sqrt(trkprt.definingParametersCovMatrix()(3, 3));
  m_trkErrP[QOVERP] = TMath::Sqrt(trkprt.definingParametersCovMatrix()(4, 4));
  m_trkErrP[QOVERPT] = m_trkErrP[QOVERP] * (1 / TMath::Sin(trkprt.theta()));
  m_trkErrP[Z0SIN] =
    TMath::Sqrt(pow(m_trkErrP[THETA] * TMath::Sin(m_trkP[THETA]),
                    2) + pow(m_trkP[Z0] * m_trkErrP[THETA] * TMath::Cos(m_trkP[THETA]), 2));

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
                                                                   TMath::Sin(truthprt.auxdata<float>("theta"))) :
                                                                  m_truetrkP[QOVERPT] = -9999.;


  m_truetrkP[PT] = truthprt.pt() / 1000.;
  (truthprt.isAvailable<float>("z0") &&
   truthprt.isAvailable<float>("theta")) ? m_truetrkP[Z0SIN] = m_truetrkP[Z0] *
                                                               TMath::Sin(m_truetrkP[THETA]) : m_truetrkP[Z0SIN] =
                                                                 -9999.;
}

void
InDetPerfPlot_resITk::finalizePlots() {
  for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {
    makeResolutions(m_resITk_resHelpereta[iparam], m_resITk_Resolution_vs_eta[iparam],
                    m_resITk_ResProjections_vs_eta[iparam], true);
    makeResolutions(m_resITk_resHelperpt[iparam], m_resITk_Resolution_vs_pt[iparam],
                    m_resITk_ResProjections_vs_pt[iparam], true);
    makeResolutions(m_resITk_resHelperpt_pos[iparam], m_resITk_Resolution_vs_pt_pos[iparam]);
    makeResolutions(m_resITk_resHelperpt_neg[iparam], m_resITk_Resolution_vs_pt_neg[iparam]);
    makeResolutions(m_resITk_resHelpereta_pos[iparam], m_resITk_Resolution_vs_eta_pos[iparam]);
    makeResolutions(m_resITk_resHelpereta_neg[iparam], m_resITk_Resolution_vs_eta_neg[iparam]);
/*    makeResolutions(m_resITk_resHelperetapt[iparam], m_resITk_Resolution_vs_pt_EtaBin[iparam],
                    m_resITk_Resolution_vs_eta_PtBin[iparam]);
*/
    // add for the pull vs pT
    makeResolutions(m_resITk_pullHelperpt[iparam], m_resITk_pullResolution_vs_pt[iparam],
                    m_resITk_pullProjections_vs_pt[iparam], true);
    makeResolutions(m_resITk_pullHelpereta[iparam], m_resITk_pullResolution_vs_eta[iparam],
                    m_resITk_pullProjections_vs_eta[iparam], true);
  }

  // Get total fraction of miss ID and Momentum tails
  m_resITk_chargeID_vs_pt->Scale(1 / m_resITk_chargeID->GetEntries());
  m_resITk_chargeID_vs_eta->Scale(1 / m_resITk_chargeID->GetEntries());

  m_resITk_momTail_vs_pt->Scale(1 / m_resITk_momTail->GetEntries());
  m_resITk_momTail_vs_phi->Scale(1 / m_resITk_momTail->GetEntries());
  m_resITk_momTail_vs_eta->Scale(1 / m_resITk_momTail->GetEntries());

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
  // m_fix_qoverpt_res[8] = Book1D(fixName,fixName,600,-0.2,0.2,false);

  for (int ieta = 0; ieta < m_nEtaBins; ieta++) {
    std::vector<float> result = getResolution(m_fix_qoverpt_res[ieta], std::string("RMS"));
    m_fix_qoverptresolutionRMS_vs_eta->SetBinContent(ieta + 1, result.at(0));
    m_fix_qoverptresolutionRMS_vs_eta->SetBinError(ieta + 1, result.at(1));
  }
  for (int ieta = 0; ieta < m_nEtaBins; ieta++) {
    std::vector<float> result = getResolution(m_fix_d0_res[ieta], std::string("RMS"));
    m_fix_d0resolutionRMS_vs_eta->SetBinContent(ieta + 1, result.at(0));
    m_fix_d0resolutionRMS_vs_eta->SetBinError(ieta + 1, result.at(1));
  }
  for (int ieta = 0; ieta < m_nEtaBins; ieta++) {
    std::vector<float> result = getResolution(m_fix_z0_res[ieta], std::string("RMS"));
    m_fix_z0resolutionRMS_vs_eta->SetBinContent(ieta + 1, result.at(0));
    m_fix_z0resolutionRMS_vs_eta->SetBinError(ieta + 1, result.at(1));
  }
}

void
InDetPerfPlot_resITk::makeResolutions(TH2* h, TH1* hres[4]) {
  // Should fix this in a better way
  TString hname = h->GetName();

  if (hname.Contains("Helpereta")) {
    for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ieta + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), h->GetXaxis()->FindBin(m_EtaBins[ieta]),
                                         h->GetXaxis()->FindBin(m_EtaBins[ieta + 1]));
      if (tmp->Integral() < 1) {
        continue;
      }
      std::vector<float> result = getResolution(tmp, std::string("RMS"));
      hres[0]->SetBinContent(ieta + 1, result.at(0));
      hres[0]->SetBinError(ieta + 1, result.at(1));
      hres[1]->SetBinContent(ieta + 1, result.at(2));
      hres[1]->SetBinError(ieta + 1, result.at(3));
      result.clear();
      /*	  result = getResolution(tmp,std::string("GAUS"));
              hres[2]->SetBinContent(ieta+1,result.at(0));
              hres[2]->SetBinError(ieta+1,result.at(1));
              hres[3]->SetBinContent(ieta+1,result.at(2));
              hres[3]->SetBinError(ieta+1,result.at(3));*/
      delete tmp;
    }
  } else if (hname.Contains("Helperpt")) {
    for (unsigned int ipt = 0; ipt < m_nPtBins; ipt++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ipt + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), h->GetXaxis()->FindBin(m_PtBins[ipt]),
                                         h->GetXaxis()->FindBin(m_PtBins[ipt + 1]));
      if (tmp->Integral() < 1) {
        continue;
      }
      std::vector<float> result = getResolution(tmp, std::string("RMS"));
      hres[0]->SetBinContent(ipt + 1, result.at(0));
      hres[0]->SetBinError(ipt + 1, result.at(1));
      hres[1]->SetBinContent(ipt + 1, result.at(2));
      hres[1]->SetBinError(ipt + 1, result.at(3));
      result.clear();
/*		  result = getResolution(tmp,std::string("GAUS"));
            hres[2]->SetBinContent(ipt+1,result.at(0));
            hres[2]->SetBinError(ipt+1,result.at(1));
            hres[3]->SetBinContent(ipt+1,result.at(2));
            hres[3]->SetBinError(ipt+1,result.at(3));*/
      delete tmp;
    }
  }
}

void
InDetPerfPlot_resITk::makeResolutions(TH2* h, TH1* hres[4], TH1* hproj[m_nEtaBins], bool save) {
  TString hname = h->GetName();

  if (hname.Contains("Helpereta")) {
    for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ieta + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), h->GetXaxis()->FindBin(m_EtaBins[ieta]),
                                         h->GetXaxis()->FindBin(m_EtaBins[ieta + 1]-0.05));
      if (tmp->Integral() < 1) {
        continue;
      }
      if (save) {
        cloneHistogram(tmp, hproj[ieta]);
      }
      std::vector<float> result = getResolution(tmp, std::string("RMS"));
      hres[0]->SetBinContent(ieta + 1, result.at(0));
      hres[0]->SetBinError(ieta + 1, result.at(1));
      hres[1]->SetBinContent(ieta + 1, result.at(2));
      hres[1]->SetBinError(ieta + 1, result.at(3));
      result.clear();
      /*	  result = getResolution(hproj[ieta],std::string("GAUS"));
              hres[2]->SetBinContent(ieta+1,result.at(0));
              hres[2]->SetBinError(ieta+1,result.at(1));
              hres[3]->SetBinContent(ieta+1,result.at(2));
              hres[3]->SetBinError(ieta+1,result.at(3));*/
      delete tmp;
    }
  } else if (hname.Contains("Helperpt")) {
    for (unsigned int ipt = 0; ipt < m_nPtBins; ipt++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ipt + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), h->GetXaxis()->FindBin(m_PtBins[ipt]),
                                         h->GetXaxis()->FindBin(m_PtBins[ipt + 1]));
      if (tmp->Integral() < 1) {
        continue;
      }
      if (save) {
        cloneHistogram(tmp, hproj[ipt]);
      }
      std::vector<float> result = getResolution(tmp, std::string("RMS"));
      hres[0]->SetBinContent(ipt + 1, result.at(0));
      hres[0]->SetBinError(ipt + 1, result.at(1));
      hres[1]->SetBinContent(ipt + 1, result.at(2));
      hres[1]->SetBinError(ipt + 1, result.at(3));
      result.clear();
      /*  result = getResolution(hproj[ipt],std::string("GAUS"));
          hres[2]->SetBinContent(ipt+1,result.at(0));
          hres[2]->SetBinError(ipt+1,result.at(1));
          hres[3]->SetBinContent(ipt+1,result.at(2));
          hres[3]->SetBinError(ipt+1,result.at(3));*/
      delete tmp;
    }
  }
}

void
InDetPerfPlot_resITk::cloneHistogram(TH1D* h, TH1* hcopy) {
  unsigned int nbin = h->GetNbinsX();

  for (unsigned int ibin = 0; ibin < nbin; ibin++) {
    float binContent = h->GetBinContent(ibin + 1);
    float binError = h->GetBinError(ibin + 1);
    hcopy->SetBinContent(ibin + 1, binContent);
    hcopy->SetBinError(ibin + 1, binError);
  }
}

void
InDetPerfPlot_resITk::makeResolutions(TH3* h, TH1* hres_eta[4][4], TH1* hres_pt[4][4]) {
  float BinEta[5] = {
    0.0, 1.0, 1.5, 2.7, 5.0
  };
  float BinPt[5] = {
    0.0, 1.0, 2.0, 5.0, 500.0
  };

  for (unsigned int ieta = 0; ieta < 4; ieta++) {
    for (unsigned int ipt = 0; ipt < m_nPtBins; ipt++) {
      std::string tmpName = h->GetName() + std::string("pz_binPt") + std::to_string(ipt + 1) +
                            std::string("pz_binEta") + std::to_string(ieta + 1);
      std::string tmpName1 = tmpName + std::string("_neg");
      std::string tmpName2 = tmpName + std::string("_pos");
      TH1D* tmp1 = (TH1D*) h->ProjectionZ(tmpName1.c_str(), h->GetXaxis()->FindBin(m_PtBins[ipt]),
                                          h->GetXaxis()->FindBin(m_PtBins[ipt + 1]),
                                          h->GetYaxis()->FindBin(-BinEta[ieta + 1]),
                                          h->GetYaxis()->FindBin(-BinEta[ieta]));
      TH1D* tmp2 = (TH1D*) h->ProjectionZ(tmpName2.c_str(), h->GetXaxis()->FindBin(m_PtBins[ipt]),
                                          h->GetXaxis()->FindBin(m_PtBins[ipt + 1]),
                                          h->GetYaxis()->FindBin(BinEta[ieta]),
                                          h->GetYaxis()->FindBin(BinEta[ieta + 1]));
      tmp1->Add(tmp2);
      if (tmp1->Integral() < 1) {
        continue;
      }
      std::vector<float> result = getResolution(tmp1, std::string("RMS"));
      hres_eta[ieta][0]->SetBinContent(ipt + 1, result.at(0));
      hres_eta[ieta][0]->SetBinError(ipt + 1, result.at(1));
      hres_eta[ieta][1]->SetBinContent(ipt + 1, result.at(2));
      hres_eta[ieta][1]->SetBinError(ipt + 1, result.at(3));
      delete tmp1;
      delete tmp2;
    }
  }

  for (unsigned int ipt = 0; ipt < 4; ipt++) {
    for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
      std::string tmpName = h->GetName() + std::string("pz_binPt") + std::to_string(ipt + 1) +
                            std::string("pz_binEta") + std::to_string(ieta + 1);
      TH1D* tmp =
        (TH1D*) h->ProjectionZ(tmpName.c_str(), h->GetXaxis()->FindBin(BinPt[ipt]), h->GetXaxis()->FindBin(
                                 BinPt[ipt + 1]), h->GetYaxis()->FindBin(m_EtaBins[ieta]),
                               h->GetYaxis()->FindBin(m_EtaBins[ieta + 1]));
      if (tmp->Integral() < 1) {
        continue;
      }
      std::vector<float> result = getResolution(tmp, std::string("RMS"));
      hres_pt[ipt][0]->SetBinContent(ieta + 1, result.at(0));
      hres_pt[ipt][0]->SetBinError(ieta + 1, result.at(1));
      hres_pt[ipt][1]->SetBinContent(ieta + 1, result.at(2));
      hres_pt[ipt][1]->SetBinError(ieta + 1, result.at(3));
      delete tmp;
    }
  }
}

std::vector<float>
InDetPerfPlot_resITk::getResolution(TH1* h, std::string s) {
  std::vector<float> result;

  if (h->GetEntries() == 0.0) {// || h->Integral() < 100.0) {
    result.push_back(0.0);
    result.push_back(0.0);
    result.push_back(0.0);
    result.push_back(0.0);
    result.push_back(0.0);
    result.push_back(0.0);
    return result;
  }
  float mean = h->GetMean();
  float meanErr = h->GetMeanError();
  float rms = h->GetRMS();
  float rmsErr = h->GetRMSError();

//	std::cout << "RMSRMS" << std::endl;
//	std::cout << mean << " " << rms << std::endl;
  float nTot = h->Integral(0., h->GetNbinsX() + 1);
  float nSig = 0.0;
  float itr_rms = 0.0;
  // Stolen from Max
  itr_rms = rms + 1.0;
  if (s == "RMS") {
    int tries = 0;
    while (fabs(itr_rms - rms) > 0.001 && tries < 10) {
      rms = h->GetRMS();
      double min = -3.0 * rms + mean;
      double max = 3.0 * rms + mean;
      if (min < h->GetBinLowEdge(1)) {
        min = h->GetBinLowEdge(1);
      }
      if (max > h->GetBinCenter(h->GetNbinsX())) {
        max = h->GetBinCenter(h->GetNbinsX());
      }
      h->SetAxisRange(min, max);
      mean = h->GetMean();
      itr_rms = h->GetRMS();
      nSig = h->Integral(h->GetXaxis()->FindBin(min), h->GetXaxis()->FindBin(max));
    }
    rms = itr_rms;
    meanErr = h->GetMeanError();
    rmsErr = h->GetRMSError();
    tries++;
  } else if (s == "GAUS") {
    int fitStatus = h->Fit("gaus", "QS0");
    TFitResultPtr r = h->Fit("gaus", "QS0");
    if (fitStatus) {
      mean = r->Parameter(1);
      meanErr = r->ParError(1);
      rms = r->Parameter(2);
      rmsErr = r->ParError(2);
      nSig = h->Integral(h->GetXaxis()->FindBin(-3.0 * rms), h->GetXaxis()->FindBin(3.0 * rms));
    }
  }

  TString vari = h->GetName();
  if (!vari.Contains("pull") && (vari.Contains("d0") || vari.Contains("z0"))) {
    rms = rms * 1000.0;
    rmsErr = rmsErr * 1000.0;
    mean = mean * 1000.0;
    meanErr = meanErr * 1000.0;
  }
  float nOut = nTot - nSig;

  result.push_back(rms);
  result.push_back(rmsErr);
  result.push_back(mean);
  result.push_back(meanErr);

  if (nTot > 0.0) {
    if (nOut / nTot > 0.9) {
      nOut = 0.;
    }
    result.push_back(nOut / nTot);
    if (nOut > 0.0) {
      result.push_back((nOut / nTot) * TMath::Sqrt(1 / nOut + 1 / nTot));
    } else {
      result.push_back(0.0);
    }
  } else {
    result.push_back(0.);
    result.push_back(0.);
  }


  return result;
}
