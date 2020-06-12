/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */


#include <xAODEgamma/ElectronContainer.h>

#include "AthenaMonitoring/AthenaMonManager.h"
#include "HIMonitoring/HIMonitoringElectronsTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

#include "LWHists/TH2D_LW.h"

HIMonitoringElectronsTool::
 HIMonitoringElectronsTool(const std::string& type, const std::string& name,
                           const IInterface* parent) : ManagedMonitorToolBase(type, name, parent) {
  m_FCalEt = 0;
  m_FCalEt_A = 0;
  m_FCalEt_C = 0;

  //From Dominik Derendarz - manual cherry-picking of 345b289e
  declareProperty( "FCalEt_nbins", m_FCalEt_nbins=96 );
  declareProperty( "lowFCalEt", m_low_FCalEt=-0.16 );
  declareProperty( "highFCalEt", m_high_FCalEt=0.8 );
  m_FCalEt_oneSide_nbins = m_FCalEt_nbins/2;
  m_low_FCalEt_oneSide = m_low_FCalEt/2.0;
  m_high_FCalEt_oneSide = m_high_FCalEt/2.0;
  //In master, before the cherry picking, was:
  //m_FCalEt_nbins = 95;
  //m_low_FCalEt = -0.15;
  //m_high_FCalEt = 0.8;
  //m_FCalEt_oneSide_nbins = 50;
  //m_low_FCalEt_oneSide = -0.1;
  //m_high_FCalEt_oneSide = 0.4;

  m_eta_nbins = 60;
  m_low_eta = -3.0;
  m_high_eta = 3.0;

  m_phi_nbins = 64;
  m_low_phi = -3.14159;
  m_high_phi = 3.14159;

  m_z0sintheta_nbins = 50;
  m_low_z0sintheta = -10.0;
  m_high_z0sintheta = 10.0;

  m_iso_nbins = 60;
  m_low_iso = -30.0;
  m_high_iso = 30.0;

  m_reta_nbins = 40;
  m_low_reta = 0.7;
  m_high_reta = 1.1;

  m_rphi_nbins = 50;
  m_low_rphi = 0.6;
  m_high_rphi = 1.1;

  m_weta2_nbins = 50;
  m_low_weta2 = 0.005;
  m_high_weta2 = 0.015;

  m_rhad_nbins = 60;
  m_low_rhad = -0.3;
  m_high_rhad = 0.3;

  m_eratio_nbins = 60;
  m_low_eratio = -0.1;
  m_high_eratio = 1.1;

  m_f1_nbins = 65;
  m_low_f1 = -0.05;
  m_high_f1 = 0.6;

  m_f3_nbins = 80;
  m_low_f3 = -0.05;
  m_high_f3 = 0.15;

  m_deltaeta1_nbins = 50;
  m_low_deltaeta1 = -0.05;
  m_high_deltaeta1 = 0.05;

  m_deltaphires_nbins = 50;
  m_low_deltaphires = -0.05;
  m_high_deltaphires = 0.05;

  m_eprobabilityht_nbins = 60;
  m_low_eprobabilityht = -0.1;
  m_high_eprobabilityht = 1.1;

  StatusCode sc;
  //LH-based selection tools
  m_electronLooseLHTool = new AsgElectronLikelihoodTool("ElectronLooseLHTool");
  sc &= m_electronLooseLHTool->setProperty("ConfigFile",
                                           "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodLooseOfflineConfig2016_Smooth.conf");
  if (!m_electronLooseLHTool->initialize().isSuccess()) {
    Fatal("MyFunction", "Failed to initialize ElectronLooseLHTool ");
  }

  m_electronMediumLHTool = new AsgElectronLikelihoodTool("ElectronMediumLHTool");
  sc &= m_electronMediumLHTool->setProperty("ConfigFile",
                                            "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodMediumOfflineConfig2016_Smooth.conf");
  if (!m_electronMediumLHTool->initialize().isSuccess()) {
    Fatal("MyFunction", "Failed to initialize ElectronMediumLHTool ");
  }

  m_electronTightLHTool = new AsgElectronLikelihoodTool("ElectronTightLHTool");
  sc &= m_electronTightLHTool->setProperty("ConfigFile",
                                           "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodTightOfflineConfig2016_Smooth.conf");
  if (!m_electronTightLHTool->initialize().isSuccess()) {
    Fatal("MyFunction", "Failed to initialize ElectronTightLHTool ");
  }

  if (sc.isFailure()) {
    Warning("MyFunction", "Failed to set some properties");
  }
}

HIMonitoringElectronsTool::~HIMonitoringElectronsTool() {
}

// Description: Used for rebooking unmanaged histograms
StatusCode HIMonitoringElectronsTool::bookHistogramsRecurrent( ) {
  return StatusCode::SUCCESS;
}

// Description: Used for re-booking managed histograms
StatusCode HIMonitoringElectronsTool::bookHistograms( ) {
  if (m_environment == AthenaMonManager::online) {
    // book histograms that are only made in the online environment...
  }

  if (m_dataType == AthenaMonManager::cosmics) {
    // book histograms that are only relevant for cosmics data...
  }

  book_hist();


  return StatusCode::SUCCESS;
}

StatusCode HIMonitoringElectronsTool::fillHistograms() {
  StatusCode sc;

  /// HI event shape
  const xAOD::HIEventShapeContainer* evtShape;

  sc = evtStore()->retrieve(evtShape, "HIEventShape");
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find HIEventShape");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("HIEventShape retrieved from StoreGate");
  }

  int size = evtShape->size();
  for (int i = 0; i < size; i++) {
    const xAOD::HIEventShape* sh = evtShape->at(i);
    if (sh->layer() == 21 || sh->layer() == 22 || sh->layer() == 23) {
      m_FCalEt += sh->et();
      if ((sh->etaMax()) > 0) m_FCalEt_A += sh->et();
      else m_FCalEt_C += sh->et();
    }
  }
  m_FCalEt = m_FCalEt * 1.e-6;
  m_FCalEt_A = m_FCalEt_A * 1.e-6;
  m_FCalEt_C = m_FCalEt_C * 1.e-6;


  const xAOD::ElectronContainer* electrons = 0;
  sc = evtStore()->retrieve(electrons, "Electrons");
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find Electrons");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Electrons retrieved from StoreGate");
  }

  xAOD::ElectronContainer::const_iterator electrons_itr = electrons->begin();
  xAOD::ElectronContainer::const_iterator electrons_end = electrons->end();

  int n_loose_LH = 0;
  int n_medium_LH = 0;
  int n_tight_LH = 0;

  for (; electrons_itr != electrons_end; ++electrons_itr) {
    float pt = (*electrons_itr)->pt() * 1.e-3;
    float eta = (*electrons_itr)->caloCluster()->etaBE(2);

    if (pt < 15 || fabs(eta) > 2.47) continue;

    float phi = (*electrons_itr)->phi();
    m_h_electron_eta_phi->Fill(eta, phi);

    float z0 = (*electrons_itr)->trackParticle()->z0();
    m_h_electron_z0sintheta->Fill(z0 * sin(2 * TMath::ATan(exp(-eta))));

    bool loose_LH = (bool) m_electronLooseLHTool->accept(*electrons_itr);
    bool medium_LH = (bool) m_electronMediumLHTool->accept(*electrons_itr);
    bool tight_LH = (bool) m_electronTightLHTool->accept(*electrons_itr);

    float etcone20 = (*electrons_itr)->auxdata<float>("etcone20") * 1.e-3;

    float reta = (*electrons_itr)->auxdata<float>("Reta");
    float rphi = (*electrons_itr)->auxdata<float>("Rphi");
    float weta2 = (*electrons_itr)->auxdata< float >("weta2");

    float rhad = (*electrons_itr)->auxdata<float>("Rhad");

    float eratio = (*electrons_itr)->auxdata<float>("Eratio");
    float f1 = (*electrons_itr)->auxdata<float>("f1");
    float f3 = (*electrons_itr)->auxdata<float>("f3");

    float deltaeta1;
    (*electrons_itr)->trackCaloMatchValue(deltaeta1, xAOD::EgammaParameters::deltaEta1);
    float deltaphires;
    (*electrons_itr)->trackCaloMatchValue(deltaphires, xAOD::EgammaParameters::deltaPhiRescaled2);
    float TRT_PID;
    (*electrons_itr)->trackParticle()->summaryValue(TRT_PID, xAOD::eProbabilityHT);

    if (loose_LH) {
      n_loose_LH++;

      m_h_electron_fcal_etcone20_looseLH->Fill(m_FCalEt, etcone20);
      m_h_electron_fcalA_etcone20_looseLH->Fill(m_FCalEt_A, etcone20);
      m_h_electron_fcalC_etcone20_looseLH->Fill(m_FCalEt_C, etcone20);

      m_h_electron_fcal_reta_looseLH->Fill(m_FCalEt, reta);
      m_h_electron_fcalA_reta_looseLH->Fill(m_FCalEt_A, reta);
      m_h_electron_fcalC_reta_looseLH->Fill(m_FCalEt_C, reta);
      m_h_electron_fcal_rphi_looseLH->Fill(m_FCalEt, rphi);
      m_h_electron_fcalA_rphi_looseLH->Fill(m_FCalEt_A, rphi);
      m_h_electron_fcalC_rphi_looseLH->Fill(m_FCalEt_C, rphi);
      m_h_electron_fcal_weta2_looseLH->Fill(m_FCalEt, weta2);
      m_h_electron_fcalA_weta2_looseLH->Fill(m_FCalEt_A, weta2);
      m_h_electron_fcalC_weta2_looseLH->Fill(m_FCalEt_C, weta2);

      m_h_electron_fcal_rhad_looseLH->Fill(m_FCalEt, rhad);
      m_h_electron_fcalA_rhad_looseLH->Fill(m_FCalEt_A, rhad);
      m_h_electron_fcalC_rhad_looseLH->Fill(m_FCalEt_C, rhad);

      m_h_electron_fcal_eratio_looseLH->Fill(m_FCalEt, eratio);
      m_h_electron_fcalA_eratio_looseLH->Fill(m_FCalEt_A, eratio);
      m_h_electron_fcalC_eratio_looseLH->Fill(m_FCalEt_C, eratio);
      m_h_electron_fcal_f1_looseLH->Fill(m_FCalEt, f1);
      m_h_electron_fcalA_f1_looseLH->Fill(m_FCalEt_A, f1);
      m_h_electron_fcalC_f1_looseLH->Fill(m_FCalEt_C, f1);
      m_h_electron_fcal_f3_looseLH->Fill(m_FCalEt, f3);
      m_h_electron_fcalA_f3_looseLH->Fill(m_FCalEt_A, f3);
      m_h_electron_fcalC_f3_looseLH->Fill(m_FCalEt_C, f3);

      m_h_electron_fcal_deltaeta1_looseLH->Fill(m_FCalEt, deltaeta1);
      m_h_electron_fcalA_deltaeta1_looseLH->Fill(m_FCalEt_A, deltaeta1);
      m_h_electron_fcalC_deltaeta1_looseLH->Fill(m_FCalEt_C, deltaeta1);
      m_h_electron_fcal_deltaphires_looseLH->Fill(m_FCalEt, deltaphires);
      m_h_electron_fcalA_deltaphires_looseLH->Fill(m_FCalEt_A, deltaphires);
      m_h_electron_fcalC_deltaphires_looseLH->Fill(m_FCalEt_C, deltaphires);
      if (fabs(eta) < 2.01) {
        m_h_electron_fcal_eprobabilityht_looseLH->Fill(m_FCalEt, TRT_PID);
        m_h_electron_fcalA_eprobabilityht_looseLH->Fill(m_FCalEt_A, TRT_PID);
        m_h_electron_fcalC_eprobabilityht_looseLH->Fill(m_FCalEt_C, TRT_PID);
      }
    }

    if (medium_LH) {
      n_medium_LH++;

      m_h_electron_fcal_etcone20_mediumLH->Fill(m_FCalEt, etcone20);
      m_h_electron_fcalA_etcone20_mediumLH->Fill(m_FCalEt_A, etcone20);
      m_h_electron_fcalC_etcone20_mediumLH->Fill(m_FCalEt_C, etcone20);

      m_h_electron_fcal_reta_mediumLH->Fill(m_FCalEt, reta);
      m_h_electron_fcalA_reta_mediumLH->Fill(m_FCalEt_A, reta);
      m_h_electron_fcalC_reta_mediumLH->Fill(m_FCalEt_C, reta);
      m_h_electron_fcal_rphi_mediumLH->Fill(m_FCalEt, rphi);
      m_h_electron_fcalA_rphi_mediumLH->Fill(m_FCalEt_A, rphi);
      m_h_electron_fcalC_rphi_mediumLH->Fill(m_FCalEt_C, rphi);
      m_h_electron_fcal_weta2_mediumLH->Fill(m_FCalEt, weta2);
      m_h_electron_fcalA_weta2_mediumLH->Fill(m_FCalEt_A, weta2);
      m_h_electron_fcalC_weta2_mediumLH->Fill(m_FCalEt_C, weta2);

      m_h_electron_fcal_rhad_mediumLH->Fill(m_FCalEt, rhad);
      m_h_electron_fcalA_rhad_mediumLH->Fill(m_FCalEt_A, rhad);
      m_h_electron_fcalC_rhad_mediumLH->Fill(m_FCalEt_C, rhad);

      m_h_electron_fcal_eratio_mediumLH->Fill(m_FCalEt, eratio);
      m_h_electron_fcalA_eratio_mediumLH->Fill(m_FCalEt_A, eratio);
      m_h_electron_fcalC_eratio_mediumLH->Fill(m_FCalEt_C, eratio);
      m_h_electron_fcal_f1_mediumLH->Fill(m_FCalEt, f1);
      m_h_electron_fcalA_f1_mediumLH->Fill(m_FCalEt_A, f1);
      m_h_electron_fcalC_f1_mediumLH->Fill(m_FCalEt_C, f1);
      m_h_electron_fcal_f3_mediumLH->Fill(m_FCalEt, f3);
      m_h_electron_fcalA_f3_mediumLH->Fill(m_FCalEt_A, f3);
      m_h_electron_fcalC_f3_mediumLH->Fill(m_FCalEt_C, f3);

      m_h_electron_fcal_deltaeta1_mediumLH->Fill(m_FCalEt, deltaeta1);
      m_h_electron_fcalA_deltaeta1_mediumLH->Fill(m_FCalEt_A, deltaeta1);
      m_h_electron_fcalC_deltaeta1_mediumLH->Fill(m_FCalEt_C, deltaeta1);
      m_h_electron_fcal_deltaphires_mediumLH->Fill(m_FCalEt, deltaphires);
      m_h_electron_fcalA_deltaphires_mediumLH->Fill(m_FCalEt_A, deltaphires);
      m_h_electron_fcalC_deltaphires_mediumLH->Fill(m_FCalEt_C, deltaphires);
      if (fabs(eta) < 2.01) {
        m_h_electron_fcal_eprobabilityht_mediumLH->Fill(m_FCalEt, TRT_PID);
        m_h_electron_fcalA_eprobabilityht_mediumLH->Fill(m_FCalEt_A, TRT_PID);
        m_h_electron_fcalC_eprobabilityht_mediumLH->Fill(m_FCalEt_C, TRT_PID);
      }
    }

    if (tight_LH) {
      n_tight_LH++;

      m_h_electron_fcal_etcone20_tightLH->Fill(m_FCalEt, etcone20);
      m_h_electron_fcalA_etcone20_tightLH->Fill(m_FCalEt_A, etcone20);
      m_h_electron_fcalC_etcone20_tightLH->Fill(m_FCalEt_C, etcone20);

      m_h_electron_fcal_reta_tightLH->Fill(m_FCalEt, reta);
      m_h_electron_fcalA_reta_tightLH->Fill(m_FCalEt_A, reta);
      m_h_electron_fcalC_reta_tightLH->Fill(m_FCalEt_C, reta);
      m_h_electron_fcal_rphi_tightLH->Fill(m_FCalEt, rphi);
      m_h_electron_fcalA_rphi_tightLH->Fill(m_FCalEt_A, rphi);
      m_h_electron_fcalC_rphi_tightLH->Fill(m_FCalEt_C, rphi);
      m_h_electron_fcal_weta2_tightLH->Fill(m_FCalEt, weta2);
      m_h_electron_fcalA_weta2_tightLH->Fill(m_FCalEt_A, weta2);
      m_h_electron_fcalC_weta2_tightLH->Fill(m_FCalEt_C, weta2);

      m_h_electron_fcal_rhad_tightLH->Fill(m_FCalEt, rhad);
      m_h_electron_fcalA_rhad_tightLH->Fill(m_FCalEt_A, rhad);
      m_h_electron_fcalC_rhad_tightLH->Fill(m_FCalEt_C, rhad);

      m_h_electron_fcal_eratio_tightLH->Fill(m_FCalEt, eratio);
      m_h_electron_fcalA_eratio_tightLH->Fill(m_FCalEt_A, eratio);
      m_h_electron_fcalC_eratio_tightLH->Fill(m_FCalEt_C, eratio);
      m_h_electron_fcal_f1_tightLH->Fill(m_FCalEt, f1);
      m_h_electron_fcalA_f1_tightLH->Fill(m_FCalEt_A, f1);
      m_h_electron_fcalC_f1_tightLH->Fill(m_FCalEt_C, f1);
      m_h_electron_fcal_f3_tightLH->Fill(m_FCalEt, f3);
      m_h_electron_fcalA_f3_tightLH->Fill(m_FCalEt_A, f3);
      m_h_electron_fcalC_f3_tightLH->Fill(m_FCalEt_C, f3);

      m_h_electron_fcal_deltaeta1_tightLH->Fill(m_FCalEt, deltaeta1);
      m_h_electron_fcalA_deltaeta1_tightLH->Fill(m_FCalEt_A, deltaeta1);
      m_h_electron_fcalC_deltaeta1_tightLH->Fill(m_FCalEt_C, deltaeta1);
      m_h_electron_fcal_deltaphires_tightLH->Fill(m_FCalEt, deltaphires);
      m_h_electron_fcalA_deltaphires_tightLH->Fill(m_FCalEt_A, deltaphires);
      m_h_electron_fcalC_deltaphires_tightLH->Fill(m_FCalEt_C, deltaphires);
      if (fabs(eta) < 2.01) {
        m_h_electron_fcal_eprobabilityht_tightLH->Fill(m_FCalEt, TRT_PID);
        m_h_electron_fcalA_eprobabilityht_tightLH->Fill(m_FCalEt_A, TRT_PID);
        m_h_electron_fcalC_eprobabilityht_tightLH->Fill(m_FCalEt_C, TRT_PID);
      }
    }
  }

  if (n_loose_LH) {
    m_h_FCalEt_looseLH->Fill(m_FCalEt);
    m_h_FCalEt_A_looseLH->Fill(m_FCalEt_A);
    m_h_FCalEt_C_looseLH->Fill(m_FCalEt_C);
  }
  if (n_medium_LH) {
    m_h_FCalEt_mediumLH->Fill(m_FCalEt);
    m_h_FCalEt_A_mediumLH->Fill(m_FCalEt_A);
    m_h_FCalEt_C_mediumLH->Fill(m_FCalEt_C);
  }
  if (n_tight_LH) {
    m_h_FCalEt_tightLH->Fill(m_FCalEt);
    m_h_FCalEt_A_tightLH->Fill(m_FCalEt_A);
    m_h_FCalEt_C_tightLH->Fill(m_FCalEt_C);
  }

  return StatusCode::SUCCESS;
}

StatusCode HIMonitoringElectronsTool::procHistograms( ) {
  //if( endOfLowStatFlag() || endOfLumiBlockFlag() ) {	}


  if (endOfRunFlag()) {
    if (m_h_FCalEt_looseLH->GetEntries() > 0) m_h_FCalEt_looseLH->Scale(1. / m_h_FCalEt_looseLH->GetEntries());
    if (m_h_FCalEt_mediumLH->GetEntries() > 0) m_h_FCalEt_mediumLH->Scale(1. / m_h_FCalEt_mediumLH->GetEntries());
    if (m_h_FCalEt_tightLH->GetEntries() > 0) m_h_FCalEt_tightLH->Scale(1. / m_h_FCalEt_tightLH->GetEntries());
    if (m_h_FCalEt_A_looseLH->GetEntries() > 0) m_h_FCalEt_A_looseLH->Scale(1. / m_h_FCalEt_A_looseLH->GetEntries());
    if (m_h_FCalEt_A_mediumLH->GetEntries() > 0) m_h_FCalEt_A_mediumLH->Scale(1. / m_h_FCalEt_A_mediumLH->GetEntries());

    if (m_h_FCalEt_A_tightLH->GetEntries() > 0) m_h_FCalEt_A_tightLH->Scale(1. / m_h_FCalEt_A_tightLH->GetEntries());
    if (m_h_FCalEt_C_looseLH->GetEntries() > 0) m_h_FCalEt_C_looseLH->Scale(1. / m_h_FCalEt_C_looseLH->GetEntries());
    if (m_h_FCalEt_C_mediumLH->GetEntries() > 0) m_h_FCalEt_C_mediumLH->Scale(1. / m_h_FCalEt_C_mediumLH->GetEntries());

    if (m_h_FCalEt_C_tightLH->GetEntries() > 0) m_h_FCalEt_C_tightLH->Scale(1. / m_h_FCalEt_C_tightLH->GetEntries());
    if (m_h_electron_z0sintheta->GetEntries() > 0) m_h_electron_z0sintheta->Scale(
        1. / m_h_electron_z0sintheta->GetEntries());
  }

  return StatusCode::SUCCESS;
}

void HIMonitoringElectronsTool::book_hist() {
  std::string path = "HeavyIon/Electrons";

  //FCal sum Et
  m_h_FCalEt_looseLH = new TH1D("h_FCalEt_looseLH", "; FCal #Sigma E_{T} [TeV]; entries", m_FCalEt_nbins, m_low_FCalEt,
                                m_high_FCalEt);
  regHist(m_h_FCalEt_looseLH, path, run).ignore();
  m_h_FCalEt_mediumLH = new TH1D("h_FCalEt_mediumLH", "; FCal #Sigma E_{T} [TeV]; entries", m_FCalEt_nbins,
                                 m_low_FCalEt, m_high_FCalEt);
  regHist(m_h_FCalEt_mediumLH, path, run).ignore();
  m_h_FCalEt_tightLH = new TH1D("h_FCalEt_tightLH", "; FCal #Sigma E_{T} [TeV]; entries", m_FCalEt_nbins, m_low_FCalEt,
                                m_high_FCalEt);
  regHist(m_h_FCalEt_tightLH, path, run).ignore();

  //FCal sum Et, side A
  m_h_FCalEt_A_looseLH = new TH1D("h_FCalEt_A_looseLH", "; FCal A #Sigma E_{T} [TeV]; entries", m_FCalEt_oneSide_nbins,
                                  m_low_FCalEt_oneSide, m_high_FCalEt_oneSide);
  regHist(m_h_FCalEt_A_looseLH, path, run).ignore();
  m_h_FCalEt_A_mediumLH = new TH1D("h_FCalEt_A_mediumLH", "; FCal A #Sigma E_{T} [TeV]; entries",
                                   m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide);
  regHist(m_h_FCalEt_A_mediumLH, path, run).ignore();
  m_h_FCalEt_A_tightLH = new TH1D("h_FCalEt_A_tightLH", "; FCal A #Sigma E_{T} [TeV]; entries", m_FCalEt_oneSide_nbins,
                                  m_low_FCalEt_oneSide, m_high_FCalEt_oneSide);
  regHist(m_h_FCalEt_A_tightLH, path, run).ignore();

  //FCal sum Et, side C
  m_h_FCalEt_C_looseLH = new TH1D("h_FCalEt_C_looseLH", "; FCal C #Sigma E_{T} [TeV]; entries", m_FCalEt_oneSide_nbins,
                                  m_low_FCalEt_oneSide, m_high_FCalEt_oneSide);
  regHist(m_h_FCalEt_C_looseLH, path, run).ignore();
  m_h_FCalEt_C_mediumLH = new TH1D("h_FCalEt_C_mediumLH", "; FCal C #Sigma E_{T} [TeV]; entries",
                                   m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide);
  regHist(m_h_FCalEt_C_mediumLH, path, run).ignore();
  m_h_FCalEt_C_tightLH = new TH1D("h_FCalEt_C_tightLH", "; FCal C #Sigma E_{T} [TeV]; entries", m_FCalEt_oneSide_nbins,
                                  m_low_FCalEt_oneSide, m_high_FCalEt_oneSide);
  regHist(m_h_FCalEt_C_tightLH, path, run).ignore();

  //eta vs phi
  m_h_electron_eta_phi = TH2D_LW::create("h_electron_eta_phi", "; #eta; #phi", m_eta_nbins, m_low_eta, m_high_eta,
                                         m_phi_nbins, m_low_phi, m_high_phi);
  regHist(m_h_electron_eta_phi, path, run).ignore();

  //GSF track z0*sin(theta)
  m_h_electron_z0sintheta = new TH1D("h_electron_z0sintheta", "; z0 sin#theta [mm]; entries", m_z0sintheta_nbins,
                                     m_low_z0sintheta, m_high_z0sintheta);
  regHist(m_h_electron_z0sintheta, path, run).ignore();

  //isolation vs centrality
  m_h_electron_fcal_etcone20_looseLH = TH2D_LW::create("h_electron_fcal_etcone20_looseLH",
                                                       "; FCal #Sigma E_{T} [TeV]; electron etcone20 [GeV]",
                                                       m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_iso_nbins,
                                                       m_low_iso, m_high_iso);
  regHist(m_h_electron_fcal_etcone20_looseLH, path, run).ignore();
  m_h_electron_fcal_etcone20_mediumLH = TH2D_LW::create("h_electron_fcal_etcone20_mediumLH",
                                                        "; FCal #Sigma E_{T} [TeV]; electron etcone20 [GeV]",
                                                        m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_iso_nbins,
                                                        m_low_iso, m_high_iso);
  regHist(m_h_electron_fcal_etcone20_mediumLH, path, run).ignore();
  m_h_electron_fcal_etcone20_tightLH = TH2D_LW::create("h_electron_fcal_etcone20_tightLH",
                                                       "; FCal #Sigma E_{T} [TeV]; electron etcone20 [GeV]",
                                                       m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_iso_nbins,
                                                       m_low_iso, m_high_iso);
  regHist(m_h_electron_fcal_etcone20_tightLH, path, run).ignore();

  //isolation vs centrality, side A
  m_h_electron_fcalA_etcone20_looseLH = TH2D_LW::create("h_electron_fcalA_etcone20_looseLH",
                                                        "; FCal A #Sigma E_{T} [TeV]; electron etcone20 [GeV]",
                                                        m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                        m_high_FCalEt_oneSide, m_iso_nbins, m_low_iso, m_high_iso);
  regHist(m_h_electron_fcalA_etcone20_looseLH, path, run).ignore();
  m_h_electron_fcalA_etcone20_mediumLH = TH2D_LW::create("h_electron_fcalA_etcone20_mediumLH",
                                                         "; FCal A #Sigma E_{T} [TeV]; electron etcone20 [GeV]",
                                                         m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                         m_high_FCalEt_oneSide, m_iso_nbins, m_low_iso, m_high_iso);
  regHist(m_h_electron_fcalA_etcone20_mediumLH, path, run).ignore();
  m_h_electron_fcalA_etcone20_tightLH = TH2D_LW::create("h_electron_fcalA_etcone20_tightLH",
                                                        "; FCal A #Sigma E_{T} [TeV]; electron etcone20 [GeV]",
                                                        m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                        m_high_FCalEt_oneSide, m_iso_nbins, m_low_iso, m_high_iso);
  regHist(m_h_electron_fcalA_etcone20_tightLH, path, run).ignore();

  //isolation vs centrality, side C
  m_h_electron_fcalC_etcone20_looseLH = TH2D_LW::create("h_electron_fcalC_etcone20_looseLH",
                                                        "; FCal C #Sigma E_{T} [TeV]; electron etcone20 [GeV]",
                                                        m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                        m_high_FCalEt_oneSide, m_iso_nbins, m_low_iso, m_high_iso);
  regHist(m_h_electron_fcalC_etcone20_looseLH, path, run).ignore();
  m_h_electron_fcalC_etcone20_mediumLH = TH2D_LW::create("h_electron_fcalC_etcone20_mediumLH",
                                                         "; FCal C #Sigma E_{T} [TeV]; electron etcone20 [GeV]",
                                                         m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                         m_high_FCalEt_oneSide, m_iso_nbins, m_low_iso, m_high_iso);
  regHist(m_h_electron_fcalC_etcone20_mediumLH, path, run).ignore();
  m_h_electron_fcalC_etcone20_tightLH = TH2D_LW::create("h_electron_fcalC_etcone20_tightLH",
                                                        "; FCal C #Sigma E_{T} [TeV]; electron etcone20 [GeV]",
                                                        m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                        m_high_FCalEt_oneSide, m_iso_nbins, m_low_iso, m_high_iso);
  regHist(m_h_electron_fcalC_etcone20_tightLH, path, run).ignore();

  //R_eta
  m_h_electron_fcal_reta_looseLH = TH2D_LW::create("h_electron_fcal_reta_looseLH",
                                                   "; FCal #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_nbins, m_low_FCalEt,
                                                   m_high_FCalEt, m_reta_nbins, m_low_reta, m_high_reta);
  regHist(m_h_electron_fcal_reta_looseLH, path, run).ignore();
  m_h_electron_fcal_reta_mediumLH = TH2D_LW::create("h_electron_fcal_reta_mediumLH",
                                                    "; FCal #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_nbins, m_low_FCalEt,
                                                    m_high_FCalEt, m_reta_nbins, m_low_reta, m_high_reta);
  regHist(m_h_electron_fcal_reta_mediumLH, path, run).ignore();
  m_h_electron_fcal_reta_tightLH = TH2D_LW::create("h_electron_fcal_reta_tightLH",
                                                   "; FCal #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_nbins, m_low_FCalEt,
                                                   m_high_FCalEt, m_reta_nbins, m_low_reta, m_high_reta);
  regHist(m_h_electron_fcal_reta_tightLH, path, run).ignore();

  //R_eta, side A centrality
  m_h_electron_fcalA_reta_looseLH = TH2D_LW::create("h_electron_fcalA_reta_looseLH",
                                                    "; FCal A #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_oneSide_nbins,
                                                    m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_reta_nbins,
                                                    m_low_reta, m_high_reta);
  regHist(m_h_electron_fcalA_reta_looseLH, path, run).ignore();
  m_h_electron_fcalA_reta_mediumLH = TH2D_LW::create("h_electron_fcalA_reta_mediumLH",
                                                     "; FCal A #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_oneSide_nbins,
                                                     m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_reta_nbins,
                                                     m_low_reta, m_high_reta);
  regHist(m_h_electron_fcalA_reta_mediumLH, path, run).ignore();
  m_h_electron_fcalA_reta_tightLH = TH2D_LW::create("h_electron_fcalA_reta_tightLH",
                                                    "; FCal A #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_oneSide_nbins,
                                                    m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_reta_nbins,
                                                    m_low_reta, m_high_reta);
  regHist(m_h_electron_fcalA_reta_tightLH, path, run).ignore();

  //R_eta, side C centrality
  m_h_electron_fcalC_reta_looseLH = TH2D_LW::create("h_electron_fcalC_reta_looseLH",
                                                    "; FCal C #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_oneSide_nbins,
                                                    m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_reta_nbins,
                                                    m_low_reta, m_high_reta);
  regHist(m_h_electron_fcalC_reta_looseLH, path, run).ignore();
  m_h_electron_fcalC_reta_mediumLH = TH2D_LW::create("h_electron_fcalC_reta_mediumLH",
                                                     "; FCal C #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_oneSide_nbins,
                                                     m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_reta_nbins,
                                                     m_low_reta, m_high_reta);
  regHist(m_h_electron_fcalC_reta_mediumLH, path, run).ignore();
  m_h_electron_fcalC_reta_tightLH = TH2D_LW::create("h_electron_fcalC_reta_tightLH",
                                                    "; FCal C #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_oneSide_nbins,
                                                    m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_reta_nbins,
                                                    m_low_reta, m_high_reta);
  regHist(m_h_electron_fcalC_reta_tightLH, path, run).ignore();

  //R_phi
  m_h_electron_fcal_rphi_looseLH = TH2D_LW::create("h_electron_fcal_rphi_looseLH",
                                                   "; FCal #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_nbins, m_low_FCalEt,
                                                   m_high_FCalEt, m_rphi_nbins, m_low_rphi, m_high_rphi);
  regHist(m_h_electron_fcal_rphi_looseLH, path, run).ignore();
  m_h_electron_fcal_rphi_mediumLH = TH2D_LW::create("h_electron_fcal_rphi_mediumLH",
                                                    "; FCal #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_nbins, m_low_FCalEt,
                                                    m_high_FCalEt, m_rphi_nbins, m_low_rphi, m_high_rphi);
  regHist(m_h_electron_fcal_rphi_mediumLH, path, run).ignore();
  m_h_electron_fcal_rphi_tightLH = TH2D_LW::create("h_electron_fcal_rphi_tightLH",
                                                   "; FCal #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_nbins, m_low_FCalEt,
                                                   m_high_FCalEt, m_rphi_nbins, m_low_rphi, m_high_rphi);
  regHist(m_h_electron_fcal_rphi_tightLH, path, run).ignore();

  //R_phi, side A centrality
  m_h_electron_fcalA_rphi_looseLH = TH2D_LW::create("h_electron_fcalA_rphi_looseLH",
                                                    "; FCal A #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_oneSide_nbins,
                                                    m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_rphi_nbins,
                                                    m_low_rphi, m_high_rphi);
  regHist(m_h_electron_fcalA_rphi_looseLH, path, run).ignore();
  m_h_electron_fcalA_rphi_mediumLH = TH2D_LW::create("h_electron_fcalA_rphi_mediumLH",
                                                     "; FCal A #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_oneSide_nbins,
                                                     m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_rphi_nbins,
                                                     m_low_rphi, m_high_rphi);
  regHist(m_h_electron_fcalA_rphi_mediumLH, path, run).ignore();
  m_h_electron_fcalA_rphi_tightLH = TH2D_LW::create("h_electron_fcalA_rphi_tightLH",
                                                    "; FCal A #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_oneSide_nbins,
                                                    m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_rphi_nbins,
                                                    m_low_rphi, m_high_rphi);
  regHist(m_h_electron_fcalA_rphi_tightLH, path, run).ignore();

  //R_phi, side C centrality
  m_h_electron_fcalC_rphi_looseLH = TH2D_LW::create("h_electron_fcalC_rphi_looseLH",
                                                    "; FCal C #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_oneSide_nbins,
                                                    m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_rphi_nbins,
                                                    m_low_rphi, m_high_rphi);
  regHist(m_h_electron_fcalC_rphi_looseLH, path, run).ignore();
  m_h_electron_fcalC_rphi_mediumLH = TH2D_LW::create("h_electron_fcalC_rphi_mediumLH",
                                                     "; FCal C #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_oneSide_nbins,
                                                     m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_rphi_nbins,
                                                     m_low_rphi, m_high_rphi);
  regHist(m_h_electron_fcalC_rphi_mediumLH, path, run).ignore();
  m_h_electron_fcalC_rphi_tightLH = TH2D_LW::create("h_electron_fcalC_rphi_tightLH",
                                                    "; FCal C #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_oneSide_nbins,
                                                    m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_rphi_nbins,
                                                    m_low_rphi, m_high_rphi);
  regHist(m_h_electron_fcalC_rphi_tightLH, path, run).ignore();

  //W_eta2
  m_h_electron_fcal_weta2_looseLH = TH2D_LW::create("h_electron_fcal_weta2_looseLH",
                                                    "; FCal #Sigma E_{T} [TeV]; W_{#eta^{2}}", m_FCalEt_nbins,
                                                    m_low_FCalEt, m_high_FCalEt, m_weta2_nbins, m_low_weta2,
                                                    m_high_weta2);
  regHist(m_h_electron_fcal_weta2_looseLH, path, run).ignore();
  m_h_electron_fcal_weta2_mediumLH = TH2D_LW::create("h_electron_fcal_weta2_mediumLH",
                                                     "; FCal #Sigma E_{T} [TeV]; W_{#eta^{2}}", m_FCalEt_nbins,
                                                     m_low_FCalEt, m_high_FCalEt, m_weta2_nbins, m_low_weta2,
                                                     m_high_weta2);
  regHist(m_h_electron_fcal_weta2_mediumLH, path, run).ignore();
  m_h_electron_fcal_weta2_tightLH = TH2D_LW::create("h_electron_fcal_weta2_tightLH",
                                                    "; FCal #Sigma E_{T} [TeV]; W_{#eta^{2}}", m_FCalEt_nbins,
                                                    m_low_FCalEt, m_high_FCalEt, m_weta2_nbins, m_low_weta2,
                                                    m_high_weta2);
  regHist(m_h_electron_fcal_weta2_tightLH, path, run).ignore();

  //W_eta2, side A centrality
  m_h_electron_fcalA_weta2_looseLH = TH2D_LW::create("h_electron_fcalA_weta2_looseLH",
                                                     "; FCal A #Sigma E_{T} [TeV]; W_{#eta^{2}}",
                                                     m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                     m_high_FCalEt_oneSide, m_weta2_nbins, m_low_weta2, m_high_weta2);
  regHist(m_h_electron_fcalA_weta2_looseLH, path, run).ignore();
  m_h_electron_fcalA_weta2_mediumLH = TH2D_LW::create("h_electron_fcalA_weta2_mediumLH",
                                                      "; FCal A #Sigma E_{T} [TeV]; W_{#eta^{2}}",
                                                      m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                      m_high_FCalEt_oneSide, m_weta2_nbins, m_low_weta2, m_high_weta2);
  regHist(m_h_electron_fcalA_weta2_mediumLH, path, run).ignore();
  m_h_electron_fcalA_weta2_tightLH = TH2D_LW::create("h_electron_fcalA_weta2_tightLH",
                                                     "; FCal A #Sigma E_{T} [TeV]; W_{#eta^{2}}",
                                                     m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                     m_high_FCalEt_oneSide, m_weta2_nbins, m_low_weta2, m_high_weta2);
  regHist(m_h_electron_fcalA_weta2_tightLH, path, run).ignore();

  //W_eta2, side C centrality
  m_h_electron_fcalC_weta2_looseLH = TH2D_LW::create("h_electron_fcalC_weta2_looseLH",
                                                     "; FCal C #Sigma E_{T} [TeV]; W_{#eta^{2}}",
                                                     m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                     m_high_FCalEt_oneSide, m_weta2_nbins, m_low_weta2, m_high_weta2);
  regHist(m_h_electron_fcalC_weta2_looseLH, path, run).ignore();
  m_h_electron_fcalC_weta2_mediumLH = TH2D_LW::create("h_electron_fcalC_weta2_mediumLH",
                                                      "; FCal C #Sigma E_{T} [TeV]; W_{#eta^{2}}",
                                                      m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                      m_high_FCalEt_oneSide, m_weta2_nbins, m_low_weta2, m_high_weta2);
  regHist(m_h_electron_fcalC_weta2_mediumLH, path, run).ignore();
  m_h_electron_fcalC_weta2_tightLH = TH2D_LW::create("h_electron_fcalC_weta2_tightLH",
                                                     "; FCal C #Sigma E_{T} [TeV]; W_{#eta^{2}}",
                                                     m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                     m_high_FCalEt_oneSide, m_weta2_nbins, m_low_weta2, m_high_weta2);
  regHist(m_h_electron_fcalC_weta2_tightLH, path, run).ignore();

  //R_had
  m_h_electron_fcal_rhad_looseLH = TH2D_LW::create("h_electron_fcal_rhad_looseLH", "; FCal #Sigma E_{T} [TeV]; R_{had}",
                                                   m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_rhad_nbins,
                                                   m_low_rhad, m_high_rhad);
  regHist(m_h_electron_fcal_rhad_looseLH, path, run).ignore();
  m_h_electron_fcal_rhad_mediumLH = TH2D_LW::create("h_electron_fcal_rhad_mediumLH",
                                                    "; FCal #Sigma E_{T} [TeV]; R_{had}", m_FCalEt_nbins, m_low_FCalEt,
                                                    m_high_FCalEt, m_rhad_nbins, m_low_rhad, m_high_rhad);
  regHist(m_h_electron_fcal_rhad_mediumLH, path, run).ignore();
  m_h_electron_fcal_rhad_tightLH = TH2D_LW::create("h_electron_fcal_rhad_tightLH", "; FCal #Sigma E_{T} [TeV]; R_{had}",
                                                   m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_rhad_nbins,
                                                   m_low_rhad, m_high_rhad);
  regHist(m_h_electron_fcal_rhad_tightLH, path, run).ignore();

  //R_had, side A centrality
  m_h_electron_fcalA_rhad_looseLH = TH2D_LW::create("h_electron_fcalA_rhad_looseLH",
                                                    "; FCal A #Sigma E_{T} [TeV]; R_{had}", m_FCalEt_oneSide_nbins,
                                                    m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_rhad_nbins,
                                                    m_low_rhad, m_high_rhad);
  regHist(m_h_electron_fcalA_rhad_looseLH, path, run).ignore();
  m_h_electron_fcalA_rhad_mediumLH = TH2D_LW::create("h_electron_fcalA_rhad_mediumLH",
                                                     "; FCal A #Sigma E_{T} [TeV]; R_{had}", m_FCalEt_oneSide_nbins,
                                                     m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_rhad_nbins,
                                                     m_low_rhad, m_high_rhad);
  regHist(m_h_electron_fcalA_rhad_mediumLH, path, run).ignore();
  m_h_electron_fcalA_rhad_tightLH = TH2D_LW::create("h_electron_fcalA_rhad_tightLH",
                                                    "; FCal A #Sigma E_{T} [TeV]; R_{had}", m_FCalEt_oneSide_nbins,
                                                    m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_rhad_nbins,
                                                    m_low_rhad, m_high_rhad);
  regHist(m_h_electron_fcalA_rhad_tightLH, path, run).ignore();

  //R_had, side C centrality
  m_h_electron_fcalC_rhad_looseLH = TH2D_LW::create("h_electron_fcalC_rhad_looseLH",
                                                    "; FCal C #Sigma E_{T} [TeV]; R_{had}", m_FCalEt_oneSide_nbins,
                                                    m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_rhad_nbins,
                                                    m_low_rhad, m_high_rhad);
  regHist(m_h_electron_fcalC_rhad_looseLH, path, run).ignore();
  m_h_electron_fcalC_rhad_mediumLH = TH2D_LW::create("h_electron_fcalC_rhad_mediumLH",
                                                     "; FCal C #Sigma E_{T} [TeV]; R_{had}", m_FCalEt_oneSide_nbins,
                                                     m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_rhad_nbins,
                                                     m_low_rhad, m_high_rhad);
  regHist(m_h_electron_fcalC_rhad_mediumLH, path, run).ignore();
  m_h_electron_fcalC_rhad_tightLH = TH2D_LW::create("h_electron_fcalC_rhad_tightLH",
                                                    "; FCal C #Sigma E_{T} [TeV]; R_{had}", m_FCalEt_oneSide_nbins,
                                                    m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_rhad_nbins,
                                                    m_low_rhad, m_high_rhad);
  regHist(m_h_electron_fcalC_rhad_tightLH, path, run).ignore();

  //E_ratio
  m_h_electron_fcal_eratio_looseLH = TH2D_LW::create("h_electron_fcal_eratio_looseLH",
                                                     "; FCal #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_nbins,
                                                     m_low_FCalEt, m_high_FCalEt, m_eratio_nbins, m_low_eratio,
                                                     m_high_eratio);
  regHist(m_h_electron_fcal_eratio_looseLH, path, run).ignore();
  m_h_electron_fcal_eratio_mediumLH = TH2D_LW::create("h_electron_fcal_eratio_mediumLH",
                                                      "; FCal #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_nbins,
                                                      m_low_FCalEt, m_high_FCalEt, m_eratio_nbins, m_low_eratio,
                                                      m_high_eratio);
  regHist(m_h_electron_fcal_eratio_mediumLH, path, run).ignore();
  m_h_electron_fcal_eratio_tightLH = TH2D_LW::create("h_electron_fcal_eratio_tightLH",
                                                     "; FCal #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_nbins,
                                                     m_low_FCalEt, m_high_FCalEt, m_eratio_nbins, m_low_eratio,
                                                     m_high_eratio);
  regHist(m_h_electron_fcal_eratio_tightLH, path, run).ignore();

  //E_ratio, side A centrality
  m_h_electron_fcalA_eratio_looseLH = TH2D_LW::create("h_electron_fcalA_eratio_looseLH",
                                                      "; FCal A #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_oneSide_nbins,
                                                      m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_eratio_nbins,
                                                      m_low_eratio, m_high_eratio);
  regHist(m_h_electron_fcalA_eratio_looseLH, path, run).ignore();
  m_h_electron_fcalA_eratio_mediumLH = TH2D_LW::create("h_electron_fcalA_eratio_mediumLH",
                                                       "; FCal A #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_oneSide_nbins,
                                                       m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_eratio_nbins,
                                                       m_low_eratio, m_high_eratio);
  regHist(m_h_electron_fcalA_eratio_mediumLH, path, run).ignore();
  m_h_electron_fcalA_eratio_tightLH = TH2D_LW::create("h_electron_fcalA_eratio_tightLH",
                                                      "; FCal A #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_oneSide_nbins,
                                                      m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_eratio_nbins,
                                                      m_low_eratio, m_high_eratio);
  regHist(m_h_electron_fcalA_eratio_tightLH, path, run).ignore();

  //E_ratio, side C centrality
  m_h_electron_fcalC_eratio_looseLH = TH2D_LW::create("h_electron_fcalC_eratio_looseLH",
                                                      "; FCal C #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_oneSide_nbins,
                                                      m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_eratio_nbins,
                                                      m_low_eratio, m_high_eratio);
  regHist(m_h_electron_fcalC_eratio_looseLH, path, run).ignore();
  m_h_electron_fcalC_eratio_mediumLH = TH2D_LW::create("h_electron_fcalC_eratio_mediumLH",
                                                       "; FCal C #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_oneSide_nbins,
                                                       m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_eratio_nbins,
                                                       m_low_eratio, m_high_eratio);
  regHist(m_h_electron_fcalC_eratio_mediumLH, path, run).ignore();
  m_h_electron_fcalC_eratio_tightLH = TH2D_LW::create("h_electron_fcalC_eratio_tightLH",
                                                      "; FCal C #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_oneSide_nbins,
                                                      m_low_FCalEt_oneSide, m_high_FCalEt_oneSide, m_eratio_nbins,
                                                      m_low_eratio, m_high_eratio);
  regHist(m_h_electron_fcalC_eratio_tightLH, path, run).ignore();

  //f_1
  m_h_electron_fcal_f1_looseLH = TH2D_LW::create("h_electron_fcal_f1_looseLH", "; FCal #Sigma E_{T} [TeV]; f_{1}",
                                                 m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f1_nbins, m_low_f1,
                                                 m_high_f1);
  regHist(m_h_electron_fcal_f1_looseLH, path, run).ignore();
  m_h_electron_fcal_f1_mediumLH = TH2D_LW::create("h_electron_fcal_f1_mediumLH", "; FCal #Sigma E_{T} [TeV]; f_{1}",
                                                  m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f1_nbins, m_low_f1,
                                                  m_high_f1);
  regHist(m_h_electron_fcal_f1_mediumLH, path, run).ignore();
  m_h_electron_fcal_f1_tightLH = TH2D_LW::create("h_electron_fcal_f1_tightLH", "; FCal #Sigma E_{T} [TeV]; f_{1}",
                                                 m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f1_nbins, m_low_f1,
                                                 m_high_f1);
  regHist(m_h_electron_fcal_f1_tightLH, path, run).ignore();

  //f_1, side A centrality
  m_h_electron_fcalA_f1_looseLH = TH2D_LW::create("h_electron_fcalA_f1_looseLH", "; FCal A #Sigma E_{T} [TeV]; f_{1}",
                                                  m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide,
                                                  m_f1_nbins, m_low_f1, m_high_f1);
  regHist(m_h_electron_fcalA_f1_looseLH, path, run).ignore();
  m_h_electron_fcalA_f1_mediumLH = TH2D_LW::create("h_electron_fcalA_f1_mediumLH", "; FCal A #Sigma E_{T} [TeV]; f_{1}",
                                                   m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide,
                                                   m_f1_nbins, m_low_f1, m_high_f1);
  regHist(m_h_electron_fcalA_f1_mediumLH, path, run).ignore();
  m_h_electron_fcalA_f1_tightLH = TH2D_LW::create("h_electron_fcalA_f1_tightLH", "; FCal A #Sigma E_{T} [TeV]; f_{1}",
                                                  m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide,
                                                  m_f1_nbins, m_low_f1, m_high_f1);
  regHist(m_h_electron_fcalA_f1_tightLH, path, run).ignore();

  //f_1, side C centrality
  m_h_electron_fcalC_f1_looseLH = TH2D_LW::create("h_electron_fcalC_f1_looseLH", "; FCal C #Sigma E_{T} [TeV]; f_{1}",
                                                  m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide,
                                                  m_f1_nbins, m_low_f1, m_high_f1);
  regHist(m_h_electron_fcalC_f1_looseLH, path, run).ignore();
  m_h_electron_fcalC_f1_mediumLH = TH2D_LW::create("h_electron_fcalC_f1_mediumLH", "; FCal C #Sigma E_{T} [TeV]; f_{1}",
                                                   m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide,
                                                   m_f1_nbins, m_low_f1, m_high_f1);
  regHist(m_h_electron_fcalC_f1_mediumLH, path, run).ignore();
  m_h_electron_fcalC_f1_tightLH = TH2D_LW::create("h_electron_fcalC_f1_tightLH", "; FCal C #Sigma E_{T} [TeV]; f_{1}",
                                                  m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide,
                                                  m_f1_nbins, m_low_f1, m_high_f1);
  regHist(m_h_electron_fcalC_f1_tightLH, path, run).ignore();

  //f_3
  m_h_electron_fcal_f3_looseLH = TH2D_LW::create("h_electron_fcal_f3_looseLH", "; FCal #Sigma E_{T} [TeV]; f_{3}",
                                                 m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f3_nbins, m_low_f3,
                                                 m_high_f3);
  regHist(m_h_electron_fcal_f3_looseLH, path, run).ignore();
  m_h_electron_fcal_f3_mediumLH = TH2D_LW::create("h_electron_fcal_f3_mediumLH", "; FCal #Sigma E_{T} [TeV]; f_{3}",
                                                  m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f3_nbins, m_low_f3,
                                                  m_high_f3);
  regHist(m_h_electron_fcal_f3_mediumLH, path, run).ignore();
  m_h_electron_fcal_f3_tightLH = TH2D_LW::create("h_electron_fcal_f3_tightLH", "; FCal #Sigma E_{T} [TeV]; f_{3}",
                                                 m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f3_nbins, m_low_f3,
                                                 m_high_f3);
  regHist(m_h_electron_fcal_f3_tightLH, path, run).ignore();

  //f_3, side A centrality
  m_h_electron_fcalA_f3_looseLH = TH2D_LW::create("h_electron_fcalA_f3_looseLH", "; FCal A #Sigma E_{T} [TeV]; f_{3}",
                                                  m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide,
                                                  m_f3_nbins, m_low_f3, m_high_f3);
  regHist(m_h_electron_fcalA_f3_looseLH, path, run).ignore();
  m_h_electron_fcalA_f3_mediumLH = TH2D_LW::create("h_electron_fcalA_f3_mediumLH", "; FCal A #Sigma E_{T} [TeV]; f_{3}",
                                                   m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide,
                                                   m_f3_nbins, m_low_f3, m_high_f3);
  regHist(m_h_electron_fcalA_f3_mediumLH, path, run).ignore();
  m_h_electron_fcalA_f3_tightLH = TH2D_LW::create("h_electron_fcalA_f3_tightLH", "; FCal A #Sigma E_{T} [TeV]; f_{3}",
                                                  m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide,
                                                  m_f3_nbins, m_low_f3, m_high_f3);
  regHist(m_h_electron_fcalA_f3_tightLH, path, run).ignore();

  //f_3, side C centrality
  m_h_electron_fcalC_f3_looseLH = TH2D_LW::create("h_electron_fcalC_f3_looseLH", "; FCal C #Sigma E_{T} [TeV]; f_{3}",
                                                  m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide,
                                                  m_f3_nbins, m_low_f3, m_high_f3);
  regHist(m_h_electron_fcalC_f3_looseLH, path, run).ignore();
  m_h_electron_fcalC_f3_mediumLH = TH2D_LW::create("h_electron_fcalC_f3_mediumLH", "; FCal C #Sigma E_{T} [TeV]; f_{3}",
                                                   m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide,
                                                   m_f3_nbins, m_low_f3, m_high_f3);
  regHist(m_h_electron_fcalC_f3_mediumLH, path, run).ignore();
  m_h_electron_fcalC_f3_tightLH = TH2D_LW::create("h_electron_fcalC_f3_tightLH", "; FCal C #Sigma E_{T} [TeV]; f_{3}",
                                                  m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide, m_high_FCalEt_oneSide,
                                                  m_f3_nbins, m_low_f3, m_high_f3);
  regHist(m_h_electron_fcalC_f3_tightLH, path, run).ignore();

  //delta_eta1
  m_h_electron_fcal_deltaeta1_looseLH = TH2D_LW::create("h_electron_fcal_deltaeta1_looseLH",
                                                        "; FCal #Sigma E_{T} [TeV]; #Delta#eta", m_FCalEt_nbins,
                                                        m_low_FCalEt, m_high_FCalEt, m_deltaeta1_nbins, m_low_deltaeta1,
                                                        m_high_deltaeta1);
  regHist(m_h_electron_fcal_deltaeta1_looseLH, path, run).ignore();
  m_h_electron_fcal_deltaeta1_mediumLH = TH2D_LW::create("h_electron_fcal_deltaeta1_mediumLH",
                                                         "; FCal #Sigma E_{T} [TeV]; #Delta#eta", m_FCalEt_nbins,
                                                         m_low_FCalEt, m_high_FCalEt, m_deltaeta1_nbins,
                                                         m_low_deltaeta1, m_high_deltaeta1);
  regHist(m_h_electron_fcal_deltaeta1_mediumLH, path, run).ignore();
  m_h_electron_fcal_deltaeta1_tightLH = TH2D_LW::create("h_electron_fcal_deltaeta1_tightLH",
                                                        "; FCal #Sigma E_{T} [TeV]; #Delta#eta", m_FCalEt_nbins,
                                                        m_low_FCalEt, m_high_FCalEt, m_deltaeta1_nbins, m_low_deltaeta1,
                                                        m_high_deltaeta1);
  regHist(m_h_electron_fcal_deltaeta1_tightLH, path, run).ignore();

  //delta_eta1, side A centrality
  m_h_electron_fcalA_deltaeta1_looseLH = TH2D_LW::create("h_electron_fcalA_deltaeta1_looseLH",
                                                         "; FCal A #Sigma E_{T} [TeV]; #Delta#eta",
                                                         m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                         m_high_FCalEt_oneSide, m_deltaeta1_nbins, m_low_deltaeta1,
                                                         m_high_deltaeta1);
  regHist(m_h_electron_fcalA_deltaeta1_looseLH, path, run).ignore();
  m_h_electron_fcalA_deltaeta1_mediumLH = TH2D_LW::create("h_electron_fcalA_deltaeta1_mediumLH",
                                                          "; FCal A #Sigma E_{T} [TeV]; #Delta#eta",
                                                          m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                          m_high_FCalEt_oneSide, m_deltaeta1_nbins, m_low_deltaeta1,
                                                          m_high_deltaeta1);
  regHist(m_h_electron_fcalA_deltaeta1_mediumLH, path, run).ignore();
  m_h_electron_fcalA_deltaeta1_tightLH = TH2D_LW::create("h_electron_fcalA_deltaeta1_tightLH",
                                                         "; FCal A #Sigma E_{T} [TeV]; #Delta#eta",
                                                         m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                         m_high_FCalEt_oneSide, m_deltaeta1_nbins, m_low_deltaeta1,
                                                         m_high_deltaeta1);
  regHist(m_h_electron_fcalA_deltaeta1_tightLH, path, run).ignore();

  //delta_eta1, side C centrality
  m_h_electron_fcalC_deltaeta1_looseLH = TH2D_LW::create("h_electron_fcalC_deltaeta1_looseLH",
                                                         "; FCal C #Sigma E_{T} [TeV]; #Delta#eta",
                                                         m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                         m_high_FCalEt_oneSide, m_deltaeta1_nbins, m_low_deltaeta1,
                                                         m_high_deltaeta1);
  regHist(m_h_electron_fcalC_deltaeta1_looseLH, path, run).ignore();
  m_h_electron_fcalC_deltaeta1_mediumLH = TH2D_LW::create("h_electron_fcalC_deltaeta1_mediumLH",
                                                          "; FCal C #Sigma E_{T} [TeV]; #Delta#eta",
                                                          m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                          m_high_FCalEt_oneSide, m_deltaeta1_nbins, m_low_deltaeta1,
                                                          m_high_deltaeta1);
  regHist(m_h_electron_fcalC_deltaeta1_mediumLH, path, run).ignore();
  m_h_electron_fcalC_deltaeta1_tightLH = TH2D_LW::create("h_electron_fcalC_deltaeta1_tightLH",
                                                         "; FCal C #Sigma E_{T} [TeV]; #Delta#eta",
                                                         m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                         m_high_FCalEt_oneSide, m_deltaeta1_nbins, m_low_deltaeta1,
                                                         m_high_deltaeta1);
  regHist(m_h_electron_fcalC_deltaeta1_tightLH, path, run).ignore();

  //delta_phi_rescaled
  m_h_electron_fcal_deltaphires_looseLH = TH2D_LW::create("h_electron_fcal_deltaphires_looseLH",
                                                          "; FCal #Sigma E_{T} [TeV]; #Delta#phi rescaled",
                                                          m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt,
                                                          m_deltaphires_nbins, m_low_deltaphires, m_high_deltaphires);
  regHist(m_h_electron_fcal_deltaphires_looseLH, path, run).ignore();
  m_h_electron_fcal_deltaphires_mediumLH = TH2D_LW::create("h_electron_fcal_deltaphires_mediumLH",
                                                           "; FCal #Sigma E_{T} [TeV]; #Delta#phi rescaled",
                                                           m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt,
                                                           m_deltaphires_nbins, m_low_deltaphires, m_high_deltaphires);
  regHist(m_h_electron_fcal_deltaphires_mediumLH, path, run).ignore();
  m_h_electron_fcal_deltaphires_tightLH = TH2D_LW::create("h_electron_fcal_deltaphires_tightLH",
                                                          "; FCal #Sigma E_{T} [TeV]; #Delta#phi rescaled",
                                                          m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt,
                                                          m_deltaphires_nbins, m_low_deltaphires, m_high_deltaphires);
  regHist(m_h_electron_fcal_deltaphires_tightLH, path, run).ignore();

  //delta_phi_rescaled, side A centrality
  m_h_electron_fcalA_deltaphires_looseLH = TH2D_LW::create("h_electron_fcalA_deltaphires_looseLH",
                                                           "; FCal A #Sigma E_{T} [TeV]; #Delta#phi rescaled",
                                                           m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                           m_high_FCalEt_oneSide, m_deltaphires_nbins,
                                                           m_low_deltaphires, m_high_deltaphires);
  regHist(m_h_electron_fcalA_deltaphires_looseLH, path, run).ignore();
  m_h_electron_fcalA_deltaphires_mediumLH = TH2D_LW::create("h_electron_fcalA_deltaphires_mediumLH",
                                                            "; FCal A #Sigma E_{T} [TeV]; #Delta#phi rescaled",
                                                            m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                            m_high_FCalEt_oneSide, m_deltaphires_nbins,
                                                            m_low_deltaphires, m_high_deltaphires);
  regHist(m_h_electron_fcalA_deltaphires_mediumLH, path, run).ignore();
  m_h_electron_fcalA_deltaphires_tightLH = TH2D_LW::create("h_electron_fcalA_deltaphires_tightLH",
                                                           "; FCal A #Sigma E_{T} [TeV]; #Delta#phi rescaled",
                                                           m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                           m_high_FCalEt_oneSide, m_deltaphires_nbins,
                                                           m_low_deltaphires, m_high_deltaphires);
  regHist(m_h_electron_fcalA_deltaphires_tightLH, path, run).ignore();

  //delta_phi_rescaled, side C centrality
  m_h_electron_fcalC_deltaphires_looseLH = TH2D_LW::create("h_electron_fcalC_deltaphires_looseLH",
                                                           "; FCal C #Sigma E_{T} [TeV]; #Delta#phi rescaled",
                                                           m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                           m_high_FCalEt_oneSide, m_deltaphires_nbins,
                                                           m_low_deltaphires, m_high_deltaphires);
  regHist(m_h_electron_fcalC_deltaphires_looseLH, path, run).ignore();
  m_h_electron_fcalC_deltaphires_mediumLH = TH2D_LW::create("h_electron_fcalC_deltaphires_mediumLH",
                                                            "; FCal C #Sigma E_{T} [TeV]; #Delta#phi rescaled",
                                                            m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                            m_high_FCalEt_oneSide, m_deltaphires_nbins,
                                                            m_low_deltaphires, m_high_deltaphires);
  regHist(m_h_electron_fcalC_deltaphires_mediumLH, path, run).ignore();
  m_h_electron_fcalC_deltaphires_tightLH = TH2D_LW::create("h_electron_fcalC_deltaphires_tightLH",
                                                           "; FCal C #Sigma E_{T} [TeV]; #Delta#phi rescaled",
                                                           m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                           m_high_FCalEt_oneSide, m_deltaphires_nbins,
                                                           m_low_deltaphires, m_high_deltaphires);
  regHist(m_h_electron_fcalC_deltaphires_tightLH, path, run).ignore();

  //e_probability_HT
  m_h_electron_fcal_eprobabilityht_looseLH = TH2D_LW::create("h_electron_fcal_eprobabilityht_looseLH",
                                                             "; FCal #Sigma E_{T} [TeV]; eProbabilityHT",
                                                             m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt,
                                                             m_eprobabilityht_nbins, m_low_eprobabilityht,
                                                             m_high_eprobabilityht);
  regHist(m_h_electron_fcal_eprobabilityht_looseLH, path, run).ignore();
  m_h_electron_fcal_eprobabilityht_mediumLH = TH2D_LW::create("h_electron_fcal_eprobabilityht_mediumLH",
                                                              "; FCal #Sigma E_{T} [TeV]; eProbabilityHT",
                                                              m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt,
                                                              m_eprobabilityht_nbins, m_low_eprobabilityht,
                                                              m_high_eprobabilityht);
  regHist(m_h_electron_fcal_eprobabilityht_mediumLH, path, run).ignore();
  m_h_electron_fcal_eprobabilityht_tightLH = TH2D_LW::create("h_electron_fcal_eprobabilityht_tightLH",
                                                             "; FCal #Sigma E_{T} [TeV]; eProbabilityHT",
                                                             m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt,
                                                             m_eprobabilityht_nbins, m_low_eprobabilityht,
                                                             m_high_eprobabilityht);
  regHist(m_h_electron_fcal_eprobabilityht_tightLH, path, run).ignore();

  //e_probability_HT, side A centrality
  m_h_electron_fcalA_eprobabilityht_looseLH = TH2D_LW::create("h_electron_fcalA_eprobabilityht_looseLH",
                                                              "; FCal A #Sigma E_{T} [TeV]; eProbabilityHT",
                                                              m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                              m_high_FCalEt_oneSide, m_eprobabilityht_nbins,
                                                              m_low_eprobabilityht, m_high_eprobabilityht);
  regHist(m_h_electron_fcalA_eprobabilityht_looseLH, path, run).ignore();
  m_h_electron_fcalA_eprobabilityht_mediumLH = TH2D_LW::create("h_electron_fcalA_eprobabilityht_mediumLH",
                                                               "; FCal A #Sigma E_{T} [TeV]; eProbabilityHT",
                                                               m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                               m_high_FCalEt_oneSide, m_eprobabilityht_nbins,
                                                               m_low_eprobabilityht, m_high_eprobabilityht);
  regHist(m_h_electron_fcalA_eprobabilityht_mediumLH, path, run).ignore();
  m_h_electron_fcalA_eprobabilityht_tightLH = TH2D_LW::create("h_electron_fcalA_eprobabilityht_tightLH",
                                                              "; FCal A #Sigma E_{T} [TeV]; eProbabilityHT",
                                                              m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                              m_high_FCalEt_oneSide, m_eprobabilityht_nbins,
                                                              m_low_eprobabilityht, m_high_eprobabilityht);
  regHist(m_h_electron_fcalA_eprobabilityht_tightLH, path, run).ignore();

  //e_probability_HT, side C centrality
  m_h_electron_fcalC_eprobabilityht_looseLH = TH2D_LW::create("h_electron_fcalC_eprobabilityht_looseLH",
                                                              "; FCal C #Sigma E_{T} [TeV]; eProbabilityHT",
                                                              m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                              m_high_FCalEt_oneSide, m_eprobabilityht_nbins,
                                                              m_low_eprobabilityht, m_high_eprobabilityht);
  regHist(m_h_electron_fcalC_eprobabilityht_looseLH, path, run).ignore();
  m_h_electron_fcalC_eprobabilityht_mediumLH = TH2D_LW::create("h_electron_fcalC_eprobabilityht_mediumLH",
                                                               "; FCal C #Sigma E_{T} [TeV]; eProbabilityHT",
                                                               m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                               m_high_FCalEt_oneSide, m_eprobabilityht_nbins,
                                                               m_low_eprobabilityht, m_high_eprobabilityht);
  regHist(m_h_electron_fcalC_eprobabilityht_mediumLH, path, run).ignore();
  m_h_electron_fcalC_eprobabilityht_tightLH = TH2D_LW::create("h_electron_fcalC_eprobabilityht_tightLH",
                                                              "; FCal C #Sigma E_{T} [TeV]; eProbabilityHT",
                                                              m_FCalEt_oneSide_nbins, m_low_FCalEt_oneSide,
                                                              m_high_FCalEt_oneSide, m_eprobabilityht_nbins,
                                                              m_low_eprobabilityht, m_high_eprobabilityht);
  regHist(m_h_electron_fcalC_eprobabilityht_tightLH, path, run).ignore();
}
