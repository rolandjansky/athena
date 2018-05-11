/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file InDetPerfPlot_resITk.cxx
 * @author nora pettersson
 **/

#include "InDetPerfPlot_resITk.h"
// #include <iostream>
#include <utility>
#include <cmath>

InDetPerfPlot_resITk::InDetPerfPlot_resITk(InDetPlotBase* pParent, const std::string& sDir)  : InDetPlotBase(pParent, sDir),
  m_meanWidthMethod(IDPVM::GetMeanWidth::iterRMS_convergence),
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
  for (int ieta = 0; ieta <= m_nEtaBins; ieta++) {
    m_EtaBins[ieta] = -4.0 + (8.0 / m_nEtaBins) * ieta;
  }
  // Parameter definitions
  m_paramProp[D0].paraName = std::string("d0");
  m_paramProp[D0].paraLabel = std::string("d_{0}");
  m_paramProp[D0].paraUnit = std::string("[mm]");
  m_paramProp[D0].nBinsRes = 4000;
  m_paramProp[D0].limRes = {
    -30.0, 30.0
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
  m_paramProp[Z0].nBinsRes = 4000;
  m_paramProp[Z0].limRes = {
    -200.0, 200.0
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
  m_paramProp[QOVERPT].nBinsRes = 3000;
  m_paramProp[QOVERPT].limRes = {
    -30.0, 30.0
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
    ptlimits[i] = 1000.0* std::exp((i-49.0)/6.804);
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
      Book2D(tmpName, tmpTitle, 2000, nMinEta, nMaxEta, m_paramProp[iparam].nBinsRes,
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
      tmpName = "PtProjections_pullProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1);
      tmpTitle = tmpName + "; (" + m_paramProp[iparam].paraLabel + "^{reco}-" + m_paramProp[iparam].paraLabel +
                 "^{true})/#sigma_{" + m_paramProp[iparam].paraLabel + "}";
      m_resITk_pullProjections_vs_pt[iparam][ibins] = Book1D(tmpName, tmpTitle, 200, -10.0, 10.0, false);
    }

    for (unsigned int ibins = 0; ibins < m_nEtaBins; ibins++) {
      tmpName = "EtaProjections_pullProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1);
      tmpTitle = tmpName + "; (" + m_paramProp[iparam].paraLabel + "^{reco}-" + m_paramProp[iparam].paraLabel +
                 "^{true})/#sigma_{" + m_paramProp[iparam].paraLabel + "}";
      m_resITk_pullProjections_vs_eta[iparam][ibins] = Book1D(tmpName, tmpTitle, 200, -10.0, 10.0, false);
    }
    for (unsigned int ibins = 0; ibins < m_nPtBins; ibins++) {
      tmpName = "PtProjections_resProjection_" + m_paramProp[iparam].paraName + std::to_string(ibins + 1);
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
  m_trkErrP[Z0SIN] =
    std::sqrt(std::pow(m_trkErrP[THETA] * std::sin(m_trkP[THETA]),
                    2) + std::pow(m_trkP[Z0] * m_trkErrP[THETA] * std::cos(m_trkP[THETA]), 2));

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

}

void
InDetPerfPlot_resITk::getMeanWidthResultsModUnits(TH1* p_input_hist, std::vector<float>& p_result,
						  IDPVM::GetMeanWidth::methods p_method) {
  // LM: reason for using this function:
  //   orignial code was set up to only have resolution resuls in um,
  //   while inputs are in mm. Do not want to do a substantial rewrite.
  double mm2um = 1000.;

  if (!p_result.empty()) {
    p_result.clear();
  }
  m_getMeanWidth.setResults(p_input_hist, p_method);
  for (auto it : m_getMeanWidth.getDebugs())
    ATH_MSG_DEBUG(it);
  for (auto it : m_getMeanWidth.getInfos())
    ATH_MSG_INFO(it);
  for (auto it : m_getMeanWidth.getWarnings())
    ATH_MSG_WARNING(it);
  for (auto it : m_getMeanWidth.getErrors())
    ATH_MSG_ERROR(it);
  p_result.push_back(m_getMeanWidth.getRMS());
  p_result.push_back(m_getMeanWidth.getRMSError());
  p_result.push_back(m_getMeanWidth.getMean());
  p_result.push_back(m_getMeanWidth.getMeanError());
  p_result.push_back(m_getMeanWidth.getFracOut());
  p_result.push_back(m_getMeanWidth.getFracOutUnc());
  p_result.push_back(m_getMeanWidth.getFracUOflow());
  p_result.push_back(m_getMeanWidth.getBinsWithinRes());

  TString vari = p_input_hist->GetName();
  if ( !vari.Contains("pull") &&
       (vari.Contains("d0") || vari.Contains("z0")) ) {
    // the  mean, meanerror, RMS and RMSerror
    // are conventionally first four elements of results vector
    p_result[0]*= mm2um;
    p_result[1]*= mm2um;
    p_result[2]*= mm2um;
    p_result[3]*= mm2um;
  }

}

void
InDetPerfPlot_resITk::makeResolutions(TH2* h, TH1* hres[4]) {

  // Should fix this in a better way
  TString hname = h->GetName();

  // warnings in case input histograms have large % events in under- and over- flow bins 
  std::vector< std::pair<unsigned int,double> > warnUOBinFrac;
  // warnings in case of too few bins within resolution
  std::vector< std::pair<unsigned int,double> > warnBinsWithinRes;

  if (hname.Contains("Helpereta")) {
    for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ieta + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), h->GetXaxis()->FindBin(m_EtaBins[ieta]),
                                         h->GetXaxis()->FindBin(m_EtaBins[ieta + 1]));
      if (tmp->Integral() < 1) {
        continue;
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result, m_meanWidthMethod);
      hres[0]->SetBinContent(ieta + 1, result.at(0));
      hres[0]->SetBinError(ieta + 1, result.at(1));
      hres[1]->SetBinContent(ieta + 1, result.at(2));
      hres[1]->SetBinError(ieta + 1, result.at(3));
      if (result.size()>6 && result.at(6)>0.)
	warnUOBinFrac.push_back(std::make_pair(ieta + 1,result.at(6)));
      if (result.size()>7 && result.at(7)>0.)
	warnBinsWithinRes.push_back(std::make_pair(ieta + 1,result.at(7)));
      result.clear();
      /*	  
	      std::vector<float> result;
	      IDPVM::GetMeanWidth::methods altMeanWidthMethod = Gauss_fit;
              getMeanWidthResultsModUnits(tmp, result, altMeanWidthMethod);
              hres[2]->SetBinContent(ieta+1,result.at(0));
              hres[2]->SetBinError(ieta+1,result.at(1));
              hres[3]->SetBinContent(ieta+1,result.at(2));
              hres[3]->SetBinError(ieta+1,result.at(3));*/
      delete tmp;
    }
    if (!warnUOBinFrac.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres[0]->GetName(),warnUOBinFrac));
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres[1]->GetName(),warnUOBinFrac));
    }
    if (!warnBinsWithinRes.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportBinsWithinRes(hres[0]->GetName(),warnBinsWithinRes));
      ATH_MSG_WARNING(m_getMeanWidth.reportBinsWithinRes(hres[1]->GetName(),warnBinsWithinRes));
    }
  }
  else if (hname.Contains("Helperpt")) {
    if (!warnUOBinFrac.empty())
      warnUOBinFrac.clear();
    for (unsigned int ipt = 0; ipt < m_nPtBins; ipt++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ipt + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), h->GetXaxis()->FindBin(m_PtBins[ipt]),
                                         h->GetXaxis()->FindBin(m_PtBins[ipt + 1]));
      if (tmp->Integral() < 1) {
        continue;
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result, m_meanWidthMethod);
      hres[0]->SetBinContent(ipt + 1, result.at(0));
      hres[0]->SetBinError(ipt + 1, result.at(1));
      hres[1]->SetBinContent(ipt + 1, result.at(2));
      hres[1]->SetBinError(ipt + 1, result.at(3));
      if (result.size()>6 && result.at(6)>0.) 
	warnUOBinFrac.push_back(std::make_pair(ipt + 1,result.at(6)));
      if (result.size()>7 && result.at(7)>0.)
	warnBinsWithinRes.push_back(std::make_pair(ipt + 1,result.at(7)));
      result.clear();
      
/*		  
	    std::vector<float> result;
	    IDPVM::GetMeanWidth::methods altMeanWidthMethod = Gauss_fit;
            getMeanWidthResultsModUnits(tmp, result, altMeanWidthMethod);
            hres[2]->SetBinContent(ipt+1,result.at(0));
            hres[2]->SetBinError(ipt+1,result.at(1));
            hres[3]->SetBinContent(ipt+1,result.at(2));
            hres[3]->SetBinError(ipt+1,result.at(3));*/
      delete tmp;
    }
    if (!warnUOBinFrac.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres[0]->GetName(),warnUOBinFrac));
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres[1]->GetName(),warnUOBinFrac));
    }
    if (!warnBinsWithinRes.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportBinsWithinRes(hres[0]->GetName(),warnBinsWithinRes));
      ATH_MSG_WARNING(m_getMeanWidth.reportBinsWithinRes(hres[1]->GetName(),warnBinsWithinRes));
    }
    
  }// end of "Helperpt"

} // end of makeResolutions

void
InDetPerfPlot_resITk::makeResolutions(TH2* h, TH1* hres[4], TH1* hproj[m_nEtaBins], bool save) {
  TString hname = h->GetName();
  // warnings in case input histograms have large % events in under- and over- flow bins 
  std::vector< std::pair<unsigned int,double> > warnUOBinFrac;
  // warnings in case of too few bins within resolution
  std::vector< std::pair<unsigned int,double> > warnBinsWithinRes;

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
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result, m_meanWidthMethod);
      hres[0]->SetBinContent(ieta + 1, result.at(0));
      hres[0]->SetBinError(ieta + 1, result.at(1));
      hres[1]->SetBinContent(ieta + 1, result.at(2));
      hres[1]->SetBinError(ieta + 1, result.at(3));
      if (result.size()>6 && result.at(6)>0.) 
	warnUOBinFrac.push_back(std::make_pair(ieta + 1,result.at(6)));
      if (result.size()>7 && result.at(7)>0.)
	warnBinsWithinRes.push_back(std::make_pair(ieta + 1,result.at(7)));
      result.clear();
      /*	  
	      std::vector<float> result;
	      IDPVM::GetMeanWidth::methods altMeanWidthMethod = Gauss_fit;
              getMeanWidthResultsModUnits(hproj[ieta], result, altMeanWidthMethod);	  
              hres[2]->SetBinContent(ieta+1,result.at(0));
              hres[2]->SetBinError(ieta+1,result.at(1));
              hres[3]->SetBinContent(ieta+1,result.at(2));
              hres[3]->SetBinError(ieta+1,result.at(3));*/
      delete tmp;
    }
    if (!warnUOBinFrac.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres[0]->GetName(),warnUOBinFrac));
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres[1]->GetName(),warnUOBinFrac));
    }
    if (!warnBinsWithinRes.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportBinsWithinRes(hres[0]->GetName(),warnBinsWithinRes));
      ATH_MSG_WARNING(m_getMeanWidth.reportBinsWithinRes(hres[1]->GetName(),warnBinsWithinRes));
    }
  } else if (hname.Contains("Helperpt")) {
    if (!warnUOBinFrac.empty())
      warnUOBinFrac.clear();
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
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result, m_meanWidthMethod);
      hres[0]->SetBinContent(ipt + 1, result.at(0));
      hres[0]->SetBinError(ipt + 1, result.at(1));
      hres[1]->SetBinContent(ipt + 1, result.at(2));
      hres[1]->SetBinError(ipt + 1, result.at(3));
      if (result.size()>6 && result.at(6)>0.)
	warnUOBinFrac.push_back(std::make_pair(ipt + 1,result.at(6)));
      if (result.size()>7 && result.at(7)>0.)
	warnBinsWithinRes.push_back(std::make_pair(ipt + 1,result.at(7)));
      result.clear();
      /*  std::vector<float> result;
          IDPVM::GetMeanWidth::methods altMeanWidthMethod = Gauss_fit;
          getMeanWidthResultsModUnits((hproj[ipt], result, altMeanWidthMethod);
          hres[2]->SetBinContent(ipt+1,result.at(0));
          hres[2]->SetBinError(ipt+1,result.at(1));
          hres[3]->SetBinContent(ipt+1,result.at(2));
          hres[3]->SetBinError(ipt+1,result.at(3));*/
      delete tmp;
    }
    if (!warnUOBinFrac.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres[0]->GetName(),warnUOBinFrac));
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres[1]->GetName(),warnUOBinFrac));
    }
    if (!warnBinsWithinRes.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportBinsWithinRes(hres[0]->GetName(),warnBinsWithinRes));
      ATH_MSG_WARNING(m_getMeanWidth.reportBinsWithinRes(hres[1]->GetName(),warnBinsWithinRes));
    }
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

void
InDetPerfPlot_resITk::makeResolutions(TH3* h, TH1* hres_eta[4][4], TH1* hres_pt[4][4]) {

  // warnings in case input histograms have large % events in under- and over- flow bins
  std::vector< std::pair<unsigned int,double> > warnUOBinFrac;
  // warnings in case of too few bins within resolution
  std::vector< std::pair<unsigned int,double> > warnBinsWithinRes;

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
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp1, result, m_meanWidthMethod);
      hres_eta[ieta][0]->SetBinContent(ipt + 1, result.at(0));
      hres_eta[ieta][0]->SetBinError(ipt + 1, result.at(1));
      hres_eta[ieta][1]->SetBinContent(ipt + 1, result.at(2));
      hres_eta[ieta][1]->SetBinError(ipt + 1, result.at(3));
      if (result.size()>6 && result.at(6)>0.)
	warnUOBinFrac.push_back(std::make_pair(ieta + 1,result.at(6)));
      if (result.size()>7 && result.at(7)>0.)
	warnBinsWithinRes.push_back(std::make_pair(ieta + 1,result.at(7)));
      result.clear();
      delete tmp1;
      delete tmp2;
    }
    if (!warnUOBinFrac.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres_eta[ieta][0]->GetName(),warnUOBinFrac));
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres_eta[ieta][1]->GetName(),warnUOBinFrac));
    }
    if (!warnBinsWithinRes.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportBinsWithinRes(hres_eta[ieta][0]->GetName(),warnBinsWithinRes));
      ATH_MSG_WARNING(m_getMeanWidth.reportBinsWithinRes(hres_eta[ieta][1]->GetName(),warnBinsWithinRes));
    }
  }

  for (unsigned int ipt = 0; ipt < 4; ipt++) {
    if (!warnUOBinFrac.empty())
      warnUOBinFrac.clear();
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
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result,m_meanWidthMethod);
      hres_pt[ipt][0]->SetBinContent(ieta + 1, result.at(0));
      hres_pt[ipt][0]->SetBinError(ieta + 1, result.at(1));
      hres_pt[ipt][1]->SetBinContent(ieta + 1, result.at(2));
      hres_pt[ipt][1]->SetBinError(ieta + 1, result.at(3));
      if (result.size()>6 && result.at(6)>0.)
	warnUOBinFrac.push_back(std::make_pair(ieta + 1,result.at(6)));
      if (result.size()>7 && result.at(7)>0.)
	warnBinsWithinRes.push_back(std::make_pair(ipt + 1,result.at(7)));
      result.clear();
      delete tmp;
    }
    if (!warnUOBinFrac.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres_pt[ipt][0]->GetName(),warnUOBinFrac));
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres_pt[ipt][1]->GetName(),warnUOBinFrac));
    }
    if (!warnBinsWithinRes.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportBinsWithinRes(hres_pt[ipt][0]->GetName(),warnBinsWithinRes));
      ATH_MSG_WARNING(m_getMeanWidth.reportBinsWithinRes(hres_pt[ipt][1]->GetName(),warnBinsWithinRes));
    }
  }// end of loop over ipt bins
}// end of makeResolution
