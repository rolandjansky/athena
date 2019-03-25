/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPOVERLAY_CXX
#define PILEUPOVERLAY_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"
#include "TFile.h"

namespace Upgrade {

bool UpgradePerformanceFunctions::getPileupTrackConfSetting() {
  return m_bUseTrackConf;
}

float UpgradePerformanceFunctions::getPileupJetPtThresholdMeV() {
  return m_fPileupJetThresholdMeV;
}

// Loads the pileup jets
std::string UpgradePerformanceFunctions::setPileupTemplatesPath() {
  if ( std::abs(m_avgMu - 140) < 0.1 )
    return m_puPath + "/PULibrary140_1.root";
  else if ( std::abs(m_avgMu - 200) < 0.1 )
    return m_puPath + "/PULibrary200_2.root";
  else
    return "";
}

StatusCode UpgradePerformanceFunctions::initializePileupTemplates() {
  if ( !(m_avgMu == 140 || m_avgMu == 200) ){ 
    ATH_MSG_FATAL("initializePileupTemplates: Pileup library only implemented for mu=140 and 200 only!");
    return StatusCode::FAILURE;
  }

  std::string pileupTemplateFilename = setPileupTemplatesPath();

  if (m_fPileupJetThresholdMeV < 20000.) {
    ATH_MSG_WARNING("initializePileupTemplates: Lowest threshold you can get is 20 GeV and that will be automatically set now");
    m_fPileupJetThresholdMeV = 20000.;
  }

  ATH_MSG_INFO("========");
  ATH_MSG_INFO("initializePileupTemplates:");
  ATH_MSG_INFO("Pileup jet pT treshold " << m_fPileupJetThresholdMeV / 1000);
  ATH_MSG_INFO("Avg int per x-ing " << m_avgMu);
  ATH_MSG_INFO("PU jet library " << pileupTemplateFilename);

  //m_fPileupTCMaxPtMeV = 70000.; // not used. Now hardcoded to 100 GeV

  TFile *ftemplate = new TFile(pileupTemplateFilename.c_str(), "READ");
  if (ftemplate == nullptr)
    ATH_MSG_FATAL("Cannot open " + pileupTemplateFilename);
  TString treeName = "PUEvtTree30";
  TString treePt = "m30_pt", treeEta = "m30_eta", treePhi = "m30_phi", treeE = "m30_E";

  ATH_MSG_INFO("Tree name " << treeName.Data());
  ATH_MSG_INFO("pT-eta-phi-E branches " << treePt.Data() << " " << treeEta.Data() << " " << treePhi.Data() << " " << treeE.Data());
  ATH_MSG_INFO("========");

  m_pileupTree30.reset( (TTree*)ftemplate->Get(treeName) );
  if (m_pileupTree30 == nullptr)
    ATH_MSG_FATAL("Cannot access TTree " + treeName + " in " + pileupTemplateFilename);

  ATH_MSG_INFO("UpgradePerformanceFunctions::initializePileupTemplates::Number of events in the Pileup Overlay library: " << m_pileupTree30->GetEntries());

  m_pileupPt30  = 0;
  m_pileupEta30 = 0;
  m_pileupPhi30 = 0;
  m_pileupE30   = 0;
  m_pileupTree30->SetBranchAddress(treePt, &m_pileupPt30);
  m_pileupTree30->SetBranchAddress(treeEta, &m_pileupEta30);
  m_pileupTree30->SetBranchAddress(treePhi, &m_pileupPhi30);
  m_pileupTree30->SetBranchAddress(treeE, &m_pileupE30);

  /*
    The next 320 lines initiate the track confirmation functions:
      func_TC_lt50, func_TC_gt50, func_IH_lt50, func_IH_gt50
    Could be slashed to ~40 lines
  */

  // initialize TC
  if (m_bUseTrackConf) ATH_MSG_INFO("initializePileupTemplates: You are using Tracking Confirmation");
  else ATH_MSG_WARNING("initializePileupTemplates: You are NOT using Tracking Confirmation");

  // version from Marianna Testa from 17.7.2015
  // 6 scenarios for Mu200, central region (= bronze)

  float fHS70Nominal200[m_nPUPtBins] = {0.00655021, 0.00657876, 0.00578142, 0.00670391};
  float fHS80Nominal200[m_nPUPtBins] = {0.0128369,  0.0132928, 0.0100504, 0.0121145};
  float fHS90Nominal200[m_nPUPtBins] = {0.0366681,  0.0383563, 0.0180194, 0.0167449};

  float fPU02Nominal200[m_nPUPtBins] = {0.853741, 0.844738, 0.911594, 0.965879};
  float fPU05Nominal200[m_nPUPtBins] = {0.922095, 0.916188, 0.966736, 0.989596};
  float fPU10Nominal200[m_nPUPtBins] = {0.948818, 0.950567, 0.98141, 0.993905};

  // 6 scenarios for Mu200, medium region (= silver)

  float fHS70Medium200[m_nPUPtBins] = {0.0179052, 0.0181347, 0.00919735, 0.00586602};
  float fHS80Medium200[m_nPUPtBins] = {0.0335118, 0.0347988, 0.0173066, 0.00759878};
  float fHS90Medium200[m_nPUPtBins] = {0.0779999, 0.0811618, 0.0449882, 0.0178825};

  float fPU02Medium200[m_nPUPtBins] = {0.709526, 0.710547, 0.81575, 0.917082};
  float fPU05Medium200[m_nPUPtBins] = {0.857033, 0.851397, 0.906623, 0.972068};
  float fPU10Medium200[m_nPUPtBins] = {0.919615, 0.914685, 0.941338, 0.984482};

  // 6 scenarios for Mu200, extended region (= gold)

  float fHS70Extended200[m_nPUPtBins] = {0.0200319, 0.0199557, 0.0110303, 0.0121212};
  float fHS80Extended200[m_nPUPtBins] = {0.0411415, 0.0429245, 0.0194322, 0.0151515};
  float fHS90Extended200[m_nPUPtBins] = {0.0888737, 0.0940972, 0.0466814, 0.0181818};

  float fPU02Extended200[m_nPUPtBins] = {0.699877, 0.700218, 0.809446, 0.931242};
  float fPU05Extended200[m_nPUPtBins] = {0.850517, 0.835516, 0.904409, 0.977778};
  float fPU10Extended200[m_nPUPtBins] = {0.910625, 0.905812, 0.933026, 0.986765};

  // TO BE UPDATED
  // 6 scenarios for Mu140, central region (= bronze)

  float fHS70Nominal140[m_nPUPtBins] = {0.00655021, 0.00657876, 0.00578142, 0.00670391};
  float fHS80Nominal140[m_nPUPtBins] = {0.0128369,  0.0132928, 0.0100504, 0.0121145};
  float fHS90Nominal140[m_nPUPtBins] = {0.0366681,  0.0383563, 0.0180194, 0.0167449};

  float fPU02Nominal140[m_nPUPtBins] = {0.853741, 0.844738, 0.911594, 0.965879};
  float fPU05Nominal140[m_nPUPtBins] = {0.922095, 0.916188, 0.966736, 0.989596};
  float fPU10Nominal140[m_nPUPtBins] = {0.948818, 0.950567, 0.98141, 0.993905};

  // 6 scenarios for Mu140, medium region (= silver)

  float fHS70Medium140[m_nPUPtBins] = {0.0179052, 0.0181347, 0.00919735, 0.00586602};
  float fHS80Medium140[m_nPUPtBins] = {0.0335118, 0.0347988, 0.0173066, 0.00759878};
  float fHS90Medium140[m_nPUPtBins] = {0.0779999, 0.0811618, 0.0449882, 0.0178825};

  float fPU02Medium140[m_nPUPtBins] = {0.709526, 0.710547, 0.81575, 0.917082};
  float fPU05Medium140[m_nPUPtBins] = {0.857033, 0.851397, 0.906623, 0.972068};
  float fPU10Medium140[m_nPUPtBins] = {0.919615, 0.914685, 0.941338, 0.984482};

  // 6 scenarios for Mu140, extended region (= gold)

  float fHS70Extended140[m_nPUPtBins] = {0.0200319, 0.0199557, 0.0110303, 0.0121212};
  float fHS80Extended140[m_nPUPtBins] = {0.0411415, 0.0429245, 0.0194322, 0.0151515};
  float fHS90Extended140[m_nPUPtBins] = {0.0888737, 0.0940972, 0.0466814, 0.0181818};

  float fPU02Extended140[m_nPUPtBins] = {0.699877, 0.700218, 0.809446, 0.931242};
  float fPU05Extended140[m_nPUPtBins] = {0.850517, 0.835516, 0.904409, 0.977778};
  float fPU10Extended140[m_nPUPtBins] = {0.910625, 0.905812, 0.933026, 0.986765};

  // central region

  // mu 200

  if ( !(m_layout == Step1p6 || m_layout == run2) ) {


    if (m_pueff == HS && fabs(m_fEff - 0.7) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffNominalArray[i] = fHS70Nominal200[i];
    }
    if (m_pueff == HS && fabs(m_fEff - 0.8) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffNominalArray[i] = fHS80Nominal200[i];
    }
    if (m_pueff == HS && fabs(m_fEff - 0.9) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffNominalArray[i] = fHS90Nominal200[i];
    }

    if (m_pueff == PU && fabs(m_fEff - 0.02) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffNominalArray[i] = fPU02Nominal200[i];
    }
    else if (m_pueff == PU && fabs(m_fEff - 0.05) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffNominalArray[i] = fPU05Nominal200[i];
    }
    else if (m_pueff == PU && fabs(m_fEff - 0.1) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffNominalArray[i] = fPU10Nominal200[i];
    } else {
      ATH_MSG_WARNING("initializePileupTemplates:: You haven't provided working point consistent with PU::Gold");
      return StatusCode::FAILURE;
    }

    // mu 140

    if (m_pueff == HS && fabs(m_fEff - 0.7) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffNominalArray[i] = fHS70Nominal140[i];
    }
    if (m_pueff == HS && fabs(m_fEff - 0.8) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffNominalArray[i] = fHS80Nominal140[i];
    }
    if (m_pueff == HS && fabs(m_fEff - 0.9) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffNominalArray[i] = fHS90Nominal140[i];
    }

    if (m_pueff == PU && fabs(m_fEff - 0.02) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffNominalArray[i] = fPU02Nominal140[i];
    }
    if (m_pueff == PU && fabs(m_fEff - 0.05) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffNominalArray[i] = fPU05Nominal140[i];
    }
    if (m_pueff == PU && fabs(m_fEff - 0.1) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffNominalArray[i] = fPU10Nominal140[i];
    }

    // medium region

    // mu 200

    if (m_pueff == HS && fabs(m_fEff - 0.7) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffMediumArray[i] = fHS70Medium200[i];
    }
    if (m_pueff == HS && fabs(m_fEff - 0.8) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffMediumArray[i] = fHS80Medium200[i];
    }
    if (m_pueff == HS && fabs(m_fEff - 0.9) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffMediumArray[i] = fHS90Medium200[i];
    }

    if (m_pueff == PU && fabs(m_fEff - 0.02) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffMediumArray[i] = fPU02Medium200[i];
    }
    else if (m_pueff == PU && fabs(m_fEff - 0.05) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffMediumArray[i] = fPU05Medium200[i];
    }
    else if (m_pueff == PU && fabs(m_fEff - 0.1) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffMediumArray[i] = fPU10Medium200[i];
    } else {
      ATH_MSG_WARNING("initializePileupTemplates: You haven't provided working point consistent with PU::Gold");
      return StatusCode::FAILURE;
    }

    // mu 140

    if (m_pueff == HS && fabs(m_fEff - 0.7) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffMediumArray[i] = fHS70Medium140[i];
    }
    if (m_pueff == HS && fabs(m_fEff - 0.8) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffMediumArray[i] = fHS80Medium140[i];
    }
    if (m_pueff == HS && fabs(m_fEff - 0.9) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffMediumArray[i] = fHS90Medium140[i];
    }

    if (m_pueff == PU && fabs(m_fEff - 0.02) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffMediumArray[i] = fPU02Medium140[i];
    }
    if (m_pueff == PU && fabs(m_fEff - 0.05) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffMediumArray[i] = fPU05Medium140[i];
    }
    if (m_pueff == PU && fabs(m_fEff - 0.1) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffMediumArray[i] = fPU10Medium140[i];
    }

    // extended region

    // mu 200

    if (m_pueff == HS && fabs(m_fEff - 0.7) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffExtendedArray[i] = fHS70Extended200[i];
    }
    if (m_pueff == HS && fabs(m_fEff - 0.8) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffExtendedArray[i] = fHS80Extended200[i];
    }
    if (m_pueff == HS && fabs(m_fEff - 0.9) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffExtendedArray[i] = fHS90Extended200[i];
    }

    if (m_pueff == PU && fabs(m_fEff - 0.02) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffExtendedArray[i] = fPU02Extended200[i];
    }
    else if (m_pueff == PU && fabs(m_fEff - 0.05) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffExtendedArray[i] = fPU05Extended200[i];
    }
    else if (m_pueff == PU && fabs(m_fEff - 0.1) < 0.001 && fabs( m_avgMu - 200)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffExtendedArray[i] = fPU10Extended200[i];
    } else {
      ATH_MSG_WARNING("initializePileupTemplates: You haven't provided working point consistent with PU::Gold ");
      return StatusCode::FAILURE;
    }

    // mu 140

    if (m_pueff == HS && fabs(m_fEff - 0.7) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffExtendedArray[i] = fHS70Extended140[i];
    }
    if (m_pueff == HS && fabs(m_fEff - 0.8) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffExtendedArray[i] = fHS80Extended140[i];
    }
    if (m_pueff == HS && fabs(m_fEff - 0.9) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffExtendedArray[i] = fHS90Extended140[i];
    }

    if (m_pueff == PU && fabs(m_fEff - 0.02) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffExtendedArray[i] = fPU02Extended140[i];
    }
    if (m_pueff == PU && fabs(m_fEff - 0.05) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffExtendedArray[i] = fPU05Extended140[i];
    }
    if (m_pueff == PU && fabs(m_fEff - 0.1) < 0.001 && fabs( m_avgMu - 140)  < 0.1) {
      for (int i = 0; i < m_nPUPtBins; i++)
        fEffExtendedArray[i] = fPU10Extended140[i];
    }

  }

  // as of ITk Step 1.6 we have different parametrisation: pol3 below and above 50 GeV

  // 30 < pt < 50 GeV, |eta| < 2.3
  float pars_ITK_20_lt50_central[7] = {0.921339, 0.122825, -0.22213, 0.124209, 0.00550693, -0.0245591, 0.00536658}; // ITk 2% PU
  float pars_ITKHGTD_20_lt50_central[7] = {0.921171, 0.124265, -0.225279, 0.126476, 0.00553336, -0.0250627, 0.00548019}; // ITk+HGTD 2% PU
  float pars_ITK_05_lt50_central[7] = {0.85327, 0.131332, -0.194897, 0.070671, 0.0157236, -0.0106902, 0.000405723};      // ITk 0.5% PU
  float pars_ITKHGTD_05_lt50_central[7] = {0.852851, 0.134451, -0.200315, 0.0734677, 0.0160129, -0.0112085, 0.000486897};     // ITk+HGTD 0.5% PU

  // 30 < pt < 50 GeV, |eta| > 2.3
  float pars_ITK_20_lt50_forward[7] = {2.7154, -1.7423, 0.365471, 0.109652, -0.0472849, 0.00403016, 0.0}; // ITk 2% PU
  float pars_ITKHGTD_20_lt50_forward[7] = {-0.294663, 1.26458, -0.340713, -0.0659547, 0.0459632, -0.0060219, 0.0}; // ITk+HGTD 2% PU
  float pars_ITK_05_lt50_forward[7] = {15.7242, -15.8733, 4.16912, 0.793916, -0.487652, 0.05429, 0.0};      // ITk 0.5% PU
  float pars_ITKHGTD_05_lt50_forward[7] = {3.21686, -2.62274, 0.665918, 0.14365, -0.0761096, 0.00733407, 0.0};     // ITk+HGTD 0.5% PU

  // pt > 50 GeV, |eta| < 2.3
  float pars_ITK_20_gt50_central[7] = {0.982908, -0.0145804, 0.0286217, -0.0232102, 0.000496221, 0.00534865, -0.00143827}; // ITk 2% PU
  float pars_ITKHGTD_20_gt50_central[7] = {0.983195, -0.0164301, 0.0326577, -0.0262776, 0.000616215, 0.00608644, -0.00165304}; // ITk+HGTD 2% PU
  float pars_ITK_05_gt50_central[7] = {0.846853, 0.533312, -1.0802, 0.763498, -0.0174135, -0.169933, 0.0450262};      // ITk 0.5% PU
  float pars_ITKHGTD_05_gt50_central[7] = {0.844652, 0.5523, -1.12607, 0.804256, -0.024922, -0.17656, 0.0474512};     // ITk+HGTD 0.5% PU

  // pt > 50 GeV, |eta| > 2.3
  float pars_ITK_20_gt50_forward[7] = {-0.75594, 1.97061, -0.534466, -0.138732, 0.0848717, -0.0104199, 0.0}; // ITk 2% PU
  float pars_ITKHGTD_20_gt50_forward[7] = {-4.26236, 5.65654, -1.44938,-0.364698,0.210134,-0.0242407, 0.0}; // ITk+HGTD 2% PU
  float pars_ITK_05_gt50_forward[7] = {13.0335, -12.587, 3.06589, 0.748741, -0.407662, 0.0442132, 0.0};      // ITk 0.5% PU
  float pars_ITKHGTD_05_gt50_forward[7] = {-4.26236, 5.65654, -1.44938,-0.364698,0.210134,-0.0242407, 0.0};     // ITk+HGTD 0.5% PU

  // 1% working point from previous studies (w/o HGTD) may be slightly inconsistent
  float pars_ITK_10_gt50[7] = {0.917016, -0.0202391, 0.0501683, -0.0201117, 0.0, 0.0, 0.0};    // ITk 1% PU
  float pars_ITK_10_lt50[7] = {0.977724, -0.310333, 0.160202, -0.0318497, 0.0, 0.0, 0.0}; 
  float pars_ITKHGTD_10_lt50[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};    // ITk+HGTD 1% PU - not defined
  float pars_ITKHGTD_10_gt50[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};    // ITk+HGTD 1% PU - not defined

  func_TC_lt50_central.reset( new TF1("func_TC_lt50_central", "pol6", 0.2, 2.3) );
  func_TC_gt50_central.reset( new TF1("func_TC_gt50_central", "pol6", 0.2, 2.3) );
  func_TC_lt50_forward.reset( new TF1("func_TC_lt50_forward", "pol6", 2.3, 4.0) );
  func_TC_gt50_forward.reset( new TF1("func_TC_gt50_forward", "pol6", 2.3, 4.0) );
  func_IH_lt50_central.reset( new TF1("func_IH_lt50_central", "pol6", 0.2, 2.3) );
  func_IH_gt50_central.reset( new TF1("func_IH_gt50_central", "pol6", 0.2, 2.3) );
  func_IH_lt50_forward.reset( new TF1("func_IH_lt50_forward", "pol6", 2.3, 4.0) );
  func_IH_gt50_forward.reset( new TF1("func_IH_gt50_forward", "pol6", 2.3, 4.0) );

  if ( m_layout == Step1p6 ) {
    if (m_pueff == PU && fabs(m_fEff - 0.02) < 0.001 && fabs( m_avgMu - 200)  < 0.1) { // "standard setting" (PU 2%)
      for (int i = 0; i < 7; i++) {
        func_TC_lt50_central->SetParameter(i, pars_ITK_20_lt50_central[i]);
        func_TC_gt50_central->SetParameter(i, pars_ITK_20_gt50_central[i]);
        func_TC_lt50_forward->SetParameter(i, pars_ITK_20_lt50_forward[i]);
        func_TC_gt50_forward->SetParameter(i, pars_ITK_20_gt50_forward[i]);
        func_IH_lt50_central->SetParameter(i, pars_ITKHGTD_20_lt50_central[i]);
        func_IH_gt50_central->SetParameter(i, pars_ITKHGTD_20_gt50_central[i]);
        func_IH_lt50_forward->SetParameter(i, pars_ITKHGTD_20_lt50_forward[i]);
        func_IH_gt50_forward->SetParameter(i, pars_ITKHGTD_20_gt50_forward[i]);
      }
    }
    else if (m_pueff == PU && fabs(m_fEff - 0.01) < 0.001 && fabs( m_avgMu - 200)  < 0.1) { // PU 1% ITk only
      for (int i = 0; i < 7; i++) {
        func_TC_lt50_central->SetParameter(i, pars_ITK_10_lt50[i]);
        func_TC_gt50_central->SetParameter(i, pars_ITK_10_gt50[i]);
        func_TC_lt50_forward->SetParameter(i, pars_ITK_10_lt50[i]);
        func_TC_gt50_forward->SetParameter(i, pars_ITK_10_gt50[i]);
        func_IH_lt50_central->SetParameter(i, pars_ITKHGTD_10_lt50[i]);
        func_IH_gt50_central->SetParameter(i, pars_ITKHGTD_10_gt50[i]);
        func_IH_lt50_forward->SetParameter(i, pars_ITKHGTD_10_lt50[i]);
        func_IH_gt50_forward->SetParameter(i, pars_ITKHGTD_10_gt50[i]);
      }
    }
    else if (m_pueff == PU && fabs(m_fEff - 0.005) < 0.0001 && fabs( m_avgMu - 200)  < 0.1) { // PU 0.5% ITk only
      for (int i = 0; i < 7; i++) {
        func_TC_lt50_central->SetParameter(i, pars_ITK_05_lt50_central[i]);
        func_TC_gt50_central->SetParameter(i, pars_ITK_05_gt50_central[i]);
        func_TC_lt50_forward->SetParameter(i, pars_ITK_05_lt50_forward[i]);
        func_TC_gt50_forward->SetParameter(i, pars_ITK_05_gt50_forward[i]);
        func_IH_lt50_central->SetParameter(i, pars_ITKHGTD_05_lt50_central[i]);
        func_IH_gt50_central->SetParameter(i, pars_ITKHGTD_05_gt50_central[i]);
        func_IH_lt50_forward->SetParameter(i, pars_ITKHGTD_05_lt50_forward[i]);
        func_IH_gt50_forward->SetParameter(i, pars_ITKHGTD_05_gt50_forward[i]);
      }
    }
    else
      ATH_MSG_FATAL(Form("UpgradePerformanceFunctions::initializePileupTemplates:: ITk 1.6 with mu=%.0f, JVT scheme: %s, JVT eff: %.3f not supported.", m_avgMu, m_pueff == PU ? "PU" : "HS", m_fEff));

  } else {
    // in older than Step 1.6 version you won't use the new parametrisation however the function needs to be initialised -> dummy
    for (int i = 0; i < 7; i++) {
      func_TC_lt50_central->SetParameter(i, 100.);
      func_TC_gt50_central->SetParameter(i, 100.);
      func_TC_lt50_forward->SetParameter(i, 100.);
      func_TC_gt50_forward->SetParameter(i, 100.);
      func_IH_lt50_central->SetParameter(i, 100.);
      func_IH_gt50_central->SetParameter(i, 100.);
      func_IH_lt50_forward->SetParameter(i, 100.);
      func_IH_gt50_forward->SetParameter(i, 100.);
    }
  }

  return StatusCode::SUCCESS;
}

std::vector<TLorentzVector> UpgradePerformanceFunctions::getPileupJets() {
  std::vector<TLorentzVector> pujets;

  static int iNevts = m_pileupTree30->GetEntries();
  int indx = m_pileupRandom.Integer(iNevts);
  m_pileupTree30->GetEntry(indx);
  int nj = m_pileupEta30->size();

  // loop over number of actual PU jets in the event
  if (nj > 0) {
    for (int ia = 0; ia < nj; ia++) {
      if ( m_pileupPt30->at(ia) < getPileupJetPtThresholdMeV() ) continue;
      TLorentzVector jet;
      jet.SetPtEtaPhiE(m_pileupPt30->at(ia), m_pileupEta30->at(ia),
                       m_pileupPhi30->at(ia), m_pileupE30->at(ia));
      pujets.push_back(jet);
    }
  }
  return pujets;
}

double UpgradePerformanceFunctions::getJVTeff(const Upgrade::Jet jet) {
  // Jet is either HS or PU
  if (jet.truthIsHS)
    return getJVTeff_HSjet(jet.Pt(), jet.Eta());
  return getJVTeff_PUjet(jet.Pt(), jet.Eta());
}

double UpgradePerformanceFunctions::getJVTeff_HSjet(double ptMeV, double eta) {
  static TString method = "UpgradePerformanceFunctions::getJVTeff_HSjet_GeV";
  static int nwarn = 0;
  if ( m_layout != Step1p6 && ++nwarn < 5 ) ATH_MSG_WARNING(method << " method is only implemented for Step 1.6 layout");
  if ( m_pueff != PU ) ATH_MSG_FATAL(method + " only supports the PU JVT scheme (flat PU-jet efficiency, variable HS efficiency)");

  // if >100 GeV, no JVT cut is applied. Nor for pT < 30 GeV.
  if ( ptMeV > 100e3 ) return 1.0;
  if ( ptMeV <  30e3 ) return 1.0;

  double absEta = std::abs(eta);
  // No pileup tagging above maximal eta
  if ( absEta > m_GoldEtaMax ) return 1.0;

  TF1 *eff_gt50 = 0;
  TF1 *eff_lt50 = 0;
  if (m_bUseHGTD0 || m_bUseHGTD1) {
    //HGTD configurations
    if (absEta < 2.3) {
      eff_lt50 = func_IH_lt50_central.get();
      eff_gt50 = func_IH_gt50_central.get();
    } else {
      eff_lt50 = func_IH_lt50_forward.get();
      eff_gt50 = func_IH_gt50_forward.get();
    }
  } else {
    //Itk only configuration
    if (absEta < 2.3) {
      eff_lt50 = func_TC_lt50_central.get();
      eff_gt50 = func_TC_gt50_central.get();
    } else {
      eff_lt50 = func_TC_lt50_forward.get();
      eff_gt50 = func_TC_gt50_forward.get();
    }
  }
  if ( (eff_lt50 == 0) || (eff_gt50 == 0) ) {
    //should never happen
    ATH_MSG_FATAL("Unexpected jet property for track confirmation. Please report.");   
  }

  // eff measured in two bins: 20-50 GeV and 50-100 GeV
  // to make a smooth pT dependance, interpolate from bin centres: 35-75
  // (pT-35)/40 becomes 0 @ 35 GeV and 1 @ 75 GeV
  double pTfrac = (TMath::Range(35e3, 75e3, ptMeV) - 35e3) / 40e3;
  absEta = TMath::Range(0.2, m_GoldEtaMax, absEta);
  return (1.0 - pTfrac) * eff_lt50->Eval(absEta) + pTfrac * eff_gt50->Eval(absEta);
}

double UpgradePerformanceFunctions::getJVTeff_PUjet(double ptMeV, double eta) {
  static TString method = "UpgradePerformanceFunctions::getJVTeff_PUjet";
  static int nwarn = 0;
  if ( m_layout != Step1p6 && ++nwarn < 5 ) ATH_MSG_WARNING(method << " method is only implemented for Step 1.6 layout");
  if ( m_pueff  != PU      ) ATH_MSG_FATAL(method + " only supports the PU JVT scheme (flat PU-jet efficiency, variable HS efficiency)");

  // No pileup suppression assumed outside the ITk acceptance of |eta|>3.8 nor below pT=20 GeV
  if (std::abs(eta) > m_GoldEtaMax) return 1.0;
  if (ptMeV < 20e3) return 1.0;
  if (ptMeV > 100e3) return 1.0;
  return m_fEff;
}

/*
 *  Equivalent to getJVTeff methods for step 1.6 layout
 */
float UpgradePerformanceFunctions::getTrackJetConfirmEff(float ptMeV, float eta, TString jetType) {
  float ptGeV = ptMeV / 1000.;
  //  float m_fPileupTCMaxPtGeV = m_fPileupTCMaxPtMeV/1000.;
  float tceff = 1.0; // jet survival probability

  // if TC is OFF, return 1.00 for everything

  if (!m_bUseTrackConf) {
    tceff = 1.0;
    return tceff;
  }

  if (!jetType.Contains("HS") && !jetType.Contains("PU")) {
    ATH_MSG_WARNING("jet type " << jetType << " not supported.  Allowed types are HS and PU.");
    tceff = 1.0;
  }

  if (ptGeV < 20.) {
    ATH_MSG_WARNING("track-jet confirmation not implemented for pT<20 GeV !");
    tceff = 1.0;
  }

  // completely new approach

  // estimate the pT bin
  int nPt = -1;
  if ( 20. <= ptGeV && ptGeV < 40. )
    nPt = 0;
  if ( 40. <= ptGeV && ptGeV < 50. )
    nPt = 1;
  if ( 50. <= ptGeV && ptGeV < 70. )
    nPt = 2;
  if ( 70. <= ptGeV && ptGeV <= 100. )
    nPt = 3;

  bool bHS = false;

  if ( jetType.Contains("HS") ) // jet type, not Efficiency Scheme!!
    bHS = true;
  if ( jetType.Contains("PU") ) // jet type, not Efficiency Scheme!!
    bHS = false;

  // apply only in correct pt range

  // now implement correctly the Step 1.6

  if (m_layout != Step1p6) {

    // std::cout << " Entering !Step1p6 " << std::endl;

    if (nPt > -1) {

      // now clearly distinguish all three scenarios

      // ----------- Bronze ------------

      if ( m_layout == bronze ) {

        if (fabs(eta) > m_BronzeEtaMax)
          tceff = 1.00; // nothing changes above 2.4
        else {

          if (m_pueff == HS) {      // HS Efficiency Scheme
            if (bHS) { // HS jet
              tceff = m_fEff;
            } else { // PU jet
              tceff = fEffNominalArray[nPt];
            }
          }

          if (m_pueff == PU) {      // PU Efficiency Scheme
            if (bHS) { // HS jet
              tceff = fEffNominalArray[nPt];
            } else { // PU jet
              tceff = m_fEff;
            }
          }

        }
      }
      // ----------- Silver ------------

      if ( m_layout == silver ) {
        if (fabs(eta) > m_SilverEtaMax)
          tceff = 1.00; // nothing changes above 3.2
        else if (fabs(eta) > m_BronzeEtaMax)  { // the actual silver part

          if (m_pueff == HS) {      // HS Efficiency Scheme
            if (bHS) { // HS jet
              tceff = m_fEff;
            } else { // PU jet
              tceff = fEffMediumArray[nPt];
            }
          }

          if (m_pueff == PU) {      // PU Efficiency Scheme
            if (bHS) { // HS jet
              tceff = fEffMediumArray[nPt];
            } else { // PU jet
              tceff = m_fEff;
            }
          }
        } else { // nominal part stays the same

          if (m_pueff == HS) {      // HS Efficiency Scheme
            if (bHS) { // HS jet
              tceff = m_fEff;
            } else { // PU jet
              tceff = fEffNominalArray[nPt];
            }
          }

          if (m_pueff == PU) {      // PU Efficiency Scheme
            if (bHS) { // HS jet
              tceff = fEffNominalArray[nPt];
            } else { // PU jet
              tceff = m_fEff;
            }
          }
        }
      }

      // ----------- Gold ------------

      if ( m_layout == gold ) {
        if (fabs(eta) > m_GoldEtaMax)
          tceff = 1.00; // nothing changes above 3.8

        else if (fabs(eta) > m_SilverEtaMax)  { // the actual gold part

          if (m_pueff == HS) {      // HS Efficiency Scheme
            if (bHS) { // HS jet
              tceff = m_fEff;
            } else { // PU jet
              tceff = fEffExtendedArray[nPt];
            }
          }

          if (m_pueff == PU) {      // PU Efficiency Scheme
            if (bHS) { // HS jet
              tceff = fEffExtendedArray[nPt];
            } else { // PU jet
              tceff = m_fEff;
            }
          }
        }

        else if (fabs(eta) > m_BronzeEtaMax)  { // medium part stays the same

          if (m_pueff == HS) {      // HS Efficiency Scheme
            if (bHS) { // HS jet
              tceff = m_fEff;
            } else { // PU jet
              tceff = fEffMediumArray[nPt];
            }
          }

          if (m_pueff == PU) {      // PU Efficiency Scheme
            if (bHS) { // HS jet
              tceff = fEffMediumArray[nPt];
            } else { // PU jet
              tceff = m_fEff;
            }
          }
        } else { // nominal part stays the same

          if (m_pueff == HS) {      // HS Efficiency Scheme
            if (bHS) { // HS jet
              tceff = m_fEff;
            } else { // PU jet
              tceff = fEffNominalArray[nPt];
            }
          }

          if (m_pueff == PU) {      // PU Efficiency Scheme
            if (bHS) { // HS jet
              tceff = fEffNominalArray[nPt];
            } else { // PU jet
              tceff = m_fEff;
            }
          }
        }
      }
    }

    //  std::cout  << " m_bUseHGTD0: " << m_bUseHGTD0 << " m_bUseHGTD1: " << m_bUseHGTD1 << std::endl;
    // HGTD add-on two scenarios which surely could be done nicer
    if  (m_bUseHGTD0 || m_bUseHGTD1) {

      //std::cout << "UpgradePerformanceFunctions::getTrackJetConfirmEff::Warning You are using an obsolete HGTD implementation, for correct one, initialise with layout=Step1p6" << std::endl;      

      float minHGTD = 0.;
      if (m_bUseHGTD0)
        minHGTD =  m_HGTD0min;
      if (m_bUseHGTD1)
        minHGTD =  m_HGTD1min;

      if (fabs(eta) > m_HGTDmax) {
        tceff = 1.00; // nothing changes above 4.3
      }
      else if ( fabs(eta) > minHGTD ) {
        // simple scenario for the start - flat in pt (20-100 GeV) and eta, only affecting the HS efficiency --> assumes 2%PU scenario
        //      if (ptGeV > 30. && ptGeV < 100.){
        if (ptGeV > 20. && ptGeV < 100.) {
          float tceff1 = tceff;
          if (!bHS) {
            if (fabs(eta) > 2.4)
              tceff = tceff1 * 0.5; //????
            else
              tceff = tceff1;
          }
        }
      }
      // else we keep what is in the central region
    }
  } else { //step1p6 layout

    // new implementation of the track confirmation

    // PU scheme only - 2%, 1%, 0.5%

    if (m_pueff == PU) {      // PU Efficiency Scheme

      if (!bHS && ptGeV < 100.) // flat efficiency for PU
        tceff = m_fEff;
      else { // HS jet
	tceff = getJVTeff_HSjet(ptGeV*1000., eta);
      }
    }
    else {
      ATH_MSG_ERROR("UpgradePerformanceFunctions::getTrackJetConfirmEff::Warning You are using an unsupported HGTD implementation (HS scheme)!");
      ATH_MSG_ERROR(" -> for correct one, initialise with  m_upgrade->setPileupEfficiencyScheme(UpgradePerformanceFunctions::PileupEff::PU) ");
      tceff = 1.0;
    }
  } // end of Step 1.6 branch

  //  std::cout << "UpgradePerformanceFunctions::getTrackJetConfirmEff:: bHS: " << bHS <<" pt: " << ptGeV << " eta: " << eta << " tc: " << tceff << std::endl;

  return tceff;
}

void UpgradePerformanceFunctions::extendJetTrigger(bool exttrig) {
  m_ExtHGTDTrig = exttrig;
}

float UpgradePerformanceFunctions::getHGTDMinEta() {
  if (m_bUseHGTD0) return m_HGTD0min;
  if (m_bUseHGTD1) return m_HGTD1min;
  return -1000;
}

float UpgradePerformanceFunctions::getHGTDMaxEta() {
  return m_HGTDmax;
}

}

#endif
