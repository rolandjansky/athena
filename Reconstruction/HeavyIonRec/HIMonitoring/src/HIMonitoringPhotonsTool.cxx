/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */


#include "PATCore/AcceptData.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "HIMonitoring/HIMonitoringPhotonsTool.h"

#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "LWHists/TH1D_LW.h"
#include "LWHists/TH2D_LW.h"

HIMonitoringPhotonsTool::
 HIMonitoringPhotonsTool(const std::string& type, const std::string& name,
                         const IInterface* parent) : ManagedMonitorToolBase(type, name, parent) {
  m_FCalEt = 0;

  //From Dominik Derendarz - manual cherry-picking of 345b289e
  declareProperty( "FCalEt_nbins", m_FCalEt_nbins=95);
  declareProperty( "lowFCalEt", m_FCalEt_low=-0.15);
  declareProperty( "highFCalEt", m_FCalEt_high=0.8);
  declareProperty( "FCalEt_peripheral", m_FCalEt_peripheral=20);
  declareProperty( "FCalEt_central", m_FCalEt_central=50);
  //In master, before the cherry picking, was:
  //m_FCalEt_nbins = 50;
  //m_FCalEt_low = -50;
  //m_FCalEt_high = 200;

  m_PhotonPt_nbins = 50;
  m_PhotonPt_low = 0;
  m_PhotonPt_high = 500;

  m_PhotonEta_nbins = 60;
  m_PhotonEta_low = -3;
  m_PhotonEta_high = +3;

  m_PhotonPhi_nbins = 64;
  m_PhotonPhi_low = -3.14159;
  m_PhotonPhi_high = +3.14159;

  m_IsoWide_nbins = 100;
  m_IsoWide_low = -50.0;
  m_IsoWide_high = 50.0;

  m_IsoNarrow_nbins = 60;
  m_IsoNarrow_low = -30.0;
  m_IsoNarrow_high = 30.0;

  m_reta_nbins = 50;
  m_reta_low = 0.70;
  m_reta_high = 1.1;

  m_rphi_nbins = 50;
  m_rphi_low = 0.60;
  m_rphi_high = 1.1;

  m_weta2_nbins = 50;
  m_weta2_low = 0.005;
  m_weta2_high = 0.015;

  m_rhad_nbins = 50;
  m_rhad_low = -0.3;
  m_rhad_high = +0.3;

  m_wtots1_nbins = 50;
  m_wtots1_low = 0;
  m_wtots1_high = 6;

  m_fracs1_nbins = 50;
  m_fracs1_low = -0.1;
  m_fracs1_high = 1.2;

  m_deltae_nbins = 50;
  m_deltae_low = -0.1;
  m_deltae_high = 1.5;

  m_eratio_nbins = 50;
  m_eratio_low = -0.1;
  m_eratio_high = 1.1;

  m_f1_nbins = 50;
  m_f1_low = -0.05;
  m_f1_high = 0.60;

  StatusCode sc;
  m_photonLooseIsEMSelector = new AsgPhotonIsEMSelector("PhotonLooseIsEMSelector");
  sc &= m_photonLooseIsEMSelector->setProperty("isEMMask", egammaPID::PhotonLoose);
  sc &= m_photonLooseIsEMSelector->setProperty("ConfigFile",
                                               "ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMLooseSelectorCutDefs.conf");
  if (!m_photonLooseIsEMSelector->initialize().isSuccess()) {
    Fatal("MyFunction", "Failed to initialize PhotonLooseIsEMSelector ");
  }

  m_photonTightIsEMSelector = new AsgPhotonIsEMSelector("PhotonTightIsEMSelector");
  sc &= m_photonTightIsEMSelector->setProperty("isEMMask", egammaPID::PhotonTight);
  sc &= m_photonTightIsEMSelector->setProperty("ConfigFile",
                                         "ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMTightSelectorCutDefs.conf");
  if (!m_photonTightIsEMSelector->initialize().isSuccess()) {
    Fatal("MyFunction", "Failed to initialize PhotonTightIsEMSelector ");
  }
  if (sc.isFailure()) {
    Warning("MyFunction", "Failed to set some properties");
  }
}

HIMonitoringPhotonsTool::~HIMonitoringPhotonsTool() {
}

// Description: Used for rebooking unmanaged histograms
StatusCode HIMonitoringPhotonsTool::bookHistogramsRecurrent( ) {
  return StatusCode::SUCCESS;
}

// Description: Used for re-booking managed histograms
StatusCode HIMonitoringPhotonsTool::bookHistograms( ) {
  if (m_environment == AthenaMonManager::online) {
    // book histograms that are only made in the online environment...
  }

  if (m_dataType == AthenaMonManager::cosmics) {
    // book histograms that are only relevant for cosmics data...
  }

  book_hist();


  return StatusCode::SUCCESS;
}

StatusCode HIMonitoringPhotonsTool::fillHistograms() {
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
    }
  }

  m_FCalEt = m_FCalEt * 1e-3;


  const xAOD::PhotonContainer* photons = 0;
  sc = evtStore()->retrieve(photons, "Photons");
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find Photons");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Photons retrieved from StoreGate");
  }

  xAOD::PhotonContainer::const_iterator photons_itr = photons->begin();
  xAOD::PhotonContainer::const_iterator photons_end = photons->end();

  for (; photons_itr != photons_end; ++photons_itr) {
    float pt = (*photons_itr)->pt() * 1e-3;
    float eta = (*photons_itr)->caloCluster()->etaBE(2);
    float phi = (*photons_itr)->phi();

    m_h_photon_pt_eta->Fill(pt, eta);
    m_h_photon_pt_phi->Fill(pt, phi);
    m_h_photon_pt_fcal->Fill(pt, phi);

    bool loose_MC15 = (bool) m_photonLooseIsEMSelector->accept(*photons_itr);
    bool tight_MC15 = (bool) m_photonTightIsEMSelector->accept(*photons_itr);

    if (tight_MC15) {
      m_h_photon_pt_eta_tight->Fill(pt, eta);
      m_h_photon_pt_phi_tight->Fill(pt, phi);
      m_h_photon_pt_fcal_tight->Fill(pt, phi);
    }

    if (pt < 20) continue;

    m_h_photon_eta_phi_ptCut->Fill(eta, phi);

    if (tight_MC15) {
      m_h_photon_eta_phi_ptCut_tight->Fill(eta, phi);
    }

    float etcone20 = (*photons_itr)->auxdata< float >("etcone20") * 1e-3;
    float etcone30 = (*photons_itr)->auxdata< float >("etcone30") * 1e-3;
    float etcone40 = (*photons_itr)->auxdata< float >("etcone40") * 1e-3;

    m_h_photon_fcal_etcone20_ptCut->Fill(m_FCalEt, etcone20);
    m_h_photon_fcal_etcone30_ptCut->Fill(m_FCalEt, etcone30);
    m_h_photon_fcal_etcone40_ptCut->Fill(m_FCalEt, etcone40);

    //In master, before the cherry picking, was:
    //if (m_FCalEt < 20) {
    if (m_FCalEt < m_FCalEt_peripheral) {
      m_h_photon_pt_etcone40_fcal0->Fill(pt, etcone40);

      m_h_photon_etcone20_ptCut_fcal0->Fill(etcone20);
      m_h_photon_etcone30_ptCut_fcal0->Fill(etcone30);
      m_h_photon_etcone40_ptCut_fcal0->Fill(etcone40);

      m_h_photon_etcone30_ptCut_fcal0_all->Fill(etcone30);
      if (loose_MC15) m_h_photon_etcone30_ptCut_fcal0_loose->Fill(etcone30);
      if (tight_MC15) m_h_photon_etcone30_ptCut_fcal0_tight->Fill(etcone30);
    }
    //if (m_FCalEt > 20 && m_FCalEt < 50) {
    if (m_FCalEt > m_FCalEt_peripheral && m_FCalEt < m_FCalEt_central) {
      m_h_photon_pt_etcone40_fcal1->Fill(pt, etcone40);

      m_h_photon_etcone20_ptCut_fcal1->Fill(etcone20);
      m_h_photon_etcone30_ptCut_fcal1->Fill(etcone30);
      m_h_photon_etcone40_ptCut_fcal1->Fill(etcone40);

      m_h_photon_etcone30_ptCut_fcal1_all->Fill(etcone30);
      if (loose_MC15) m_h_photon_etcone30_ptCut_fcal1_loose->Fill(etcone30);
      if (tight_MC15) m_h_photon_etcone30_ptCut_fcal1_tight->Fill(etcone30);
    }
    //if (m_FCalEt > 50) {
    if (m_FCalEt > m_FCalEt_central) {
      m_h_photon_pt_etcone40_fcal2->Fill(pt, etcone40);

      m_h_photon_etcone20_ptCut_fcal2->Fill(etcone20);
      m_h_photon_etcone30_ptCut_fcal2->Fill(etcone30);
      m_h_photon_etcone40_ptCut_fcal2->Fill(etcone40);

      m_h_photon_etcone30_ptCut_fcal2_all->Fill(etcone30);
      if (loose_MC15) m_h_photon_etcone30_ptCut_fcal2_loose->Fill(etcone30);
      if (tight_MC15) m_h_photon_etcone30_ptCut_fcal2_tight->Fill(etcone30);
    }

    float reta = (*photons_itr)->auxdata< float >("Reta");
    float rphi = (*photons_itr)->auxdata< float >("Rphi");
    float weta2 = (*photons_itr)->auxdata< float >("weta2");

    float rhad = (*photons_itr)->auxdata< float >("Rhad");
    float rhad1 = (*photons_itr)->auxdata< float >("Rhad1");
    float wtots1 = (*photons_itr)->auxdata< float >("wtots1");
    float fracs1 = (*photons_itr)->auxdata< float >("fracs1");

    float deltae = (*photons_itr)->auxdata< float >("DeltaE") * 1e-3;
    float eratio = (*photons_itr)->auxdata< float >("Eratio");
    float f1 = (*photons_itr)->auxdata< float >("f1");

    if (fabs(eta) < 1.37) {
      m_h_photon_reta_fcal_ptCut_eta0->Fill(m_FCalEt, reta);
      m_h_photon_rphi_fcal_ptCut_eta0->Fill(m_FCalEt, rphi);
      m_h_photon_weta2_fcal_ptCut_eta0->Fill(m_FCalEt, weta2);
      m_h_photon_rhad_fcal_ptCut_eta0->Fill(m_FCalEt, rhad);
      m_h_photon_wtots1_fcal_ptCut_eta0->Fill(m_FCalEt, wtots1);
      m_h_photon_fracs1_fcal_ptCut_eta0->Fill(m_FCalEt, fracs1);
      m_h_photon_deltae_fcal_ptCut_eta0->Fill(m_FCalEt, deltae);
      m_h_photon_eratio_fcal_ptCut_eta0->Fill(m_FCalEt, eratio);
      m_h_photon_f1_fcal_ptCut_eta0->Fill(m_FCalEt, f1);
    }
    if (fabs(eta) > 1.52) {
      m_h_photon_reta_fcal_ptCut_eta1->Fill(m_FCalEt, reta);
      m_h_photon_rphi_fcal_ptCut_eta1->Fill(m_FCalEt, rphi);
      m_h_photon_weta2_fcal_ptCut_eta1->Fill(m_FCalEt, weta2);
      m_h_photon_rhad_fcal_ptCut_eta1->Fill(m_FCalEt, rhad1);
      m_h_photon_wtots1_fcal_ptCut_eta1->Fill(m_FCalEt, wtots1);
      m_h_photon_fracs1_fcal_ptCut_eta1->Fill(m_FCalEt, fracs1);
      m_h_photon_deltae_fcal_ptCut_eta1->Fill(m_FCalEt, deltae);
      m_h_photon_eratio_fcal_ptCut_eta1->Fill(m_FCalEt, eratio);
      m_h_photon_f1_fcal_ptCut_eta1->Fill(m_FCalEt, f1);
    }

    m_h_photon_reta_ptCut_all->Fill(reta);
    m_h_photon_rphi_ptCut_all->Fill(rphi);
    m_h_photon_weta2_ptCut_all->Fill(weta2);
    m_h_photon_rhad_ptCut_all->Fill(rhad);
    m_h_photon_wtots1_ptCut_all->Fill(wtots1);
    m_h_photon_fracs1_ptCut_all->Fill(fracs1);
    m_h_photon_deltae_ptCut_all->Fill(deltae);
    m_h_photon_eratio_ptCut_all->Fill(eratio);
    m_h_photon_f1_ptCut_all->Fill(f1);
    if (loose_MC15) {
      m_h_photon_reta_ptCut_loose->Fill(reta);
      m_h_photon_rphi_ptCut_loose->Fill(rphi);
      m_h_photon_weta2_ptCut_loose->Fill(weta2);
      m_h_photon_rhad_ptCut_loose->Fill(rhad);
      m_h_photon_wtots1_ptCut_loose->Fill(wtots1);
      m_h_photon_fracs1_ptCut_loose->Fill(fracs1);
      m_h_photon_deltae_ptCut_loose->Fill(deltae);
      m_h_photon_eratio_ptCut_loose->Fill(eratio);
      m_h_photon_f1_ptCut_loose->Fill(f1);
    }
    if (tight_MC15) {
      m_h_photon_reta_ptCut_tight->Fill(reta);
      m_h_photon_rphi_ptCut_tight->Fill(rphi);
      m_h_photon_weta2_ptCut_tight->Fill(weta2);
      m_h_photon_rhad_ptCut_tight->Fill(rhad);
      m_h_photon_wtots1_ptCut_tight->Fill(wtots1);
      m_h_photon_fracs1_ptCut_tight->Fill(fracs1);
      m_h_photon_deltae_ptCut_tight->Fill(deltae);
      m_h_photon_eratio_ptCut_tight->Fill(eratio);
      m_h_photon_f1_ptCut_tight->Fill(f1);
    }

    m_h_photon_reta_eta_ptCut->Fill(eta, reta);
    m_h_photon_rphi_eta_ptCut->Fill(eta, rphi);
    m_h_photon_weta2_eta_ptCut->Fill(eta, weta2);

    if (fabs(eta) < 1.52) m_h_photon_rhad_eta_ptCut->Fill(eta, rhad);
    else m_h_photon_rhad_eta_ptCut->Fill(eta, rhad1);
    m_h_photon_wtots1_eta_ptCut->Fill(eta, wtots1);
    m_h_photon_fracs1_eta_ptCut->Fill(eta, fracs1);

    m_h_photon_deltae_eta_ptCut->Fill(eta, deltae);
    m_h_photon_eratio_eta_ptCut->Fill(eta, eratio);
    m_h_photon_f1_eta_ptCut->Fill(eta, f1);
  }

  return StatusCode::SUCCESS;
}

StatusCode HIMonitoringPhotonsTool::procHistograms( ) {
  //if( endOfLowStatFlag() || endOfLumiBlockFlag() ) { }


  if (endOfRunFlag()) {
    if (m_h_photon_reta_ptCut_all->GetEntries() > 0) m_h_photon_reta_ptCut_all->Scale(
        1. / m_h_photon_reta_ptCut_all->GetEntries());
    if (m_h_photon_reta_ptCut_loose->GetEntries() > 0) m_h_photon_reta_ptCut_loose->Scale(
        1. / m_h_photon_reta_ptCut_loose->GetEntries());
    if (m_h_photon_reta_ptCut_tight->GetEntries() > 0) m_h_photon_reta_ptCut_tight->Scale(
        1. / m_h_photon_reta_ptCut_tight->GetEntries());
    if (m_h_photon_rphi_ptCut_all->GetEntries() > 0) m_h_photon_rphi_ptCut_all->Scale(1. / m_h_photon_rphi_ptCut_all->GetEntries());
    if (m_h_photon_rphi_ptCut_loose->GetEntries() > 0) m_h_photon_rphi_ptCut_loose->Scale(1. / m_h_photon_rphi_ptCut_loose->GetEntries());
    if (m_h_photon_rphi_ptCut_tight->GetEntries() > 0) m_h_photon_rphi_ptCut_tight->Scale(1. / m_h_photon_rphi_ptCut_tight->GetEntries());
    if (m_h_photon_weta2_ptCut_all->GetEntries() > 0) m_h_photon_weta2_ptCut_all->Scale(1. / m_h_photon_weta2_ptCut_all->GetEntries());
    if (m_h_photon_weta2_ptCut_loose->GetEntries() > 0) m_h_photon_weta2_ptCut_loose->Scale(1. / m_h_photon_weta2_ptCut_loose->GetEntries());
    if (m_h_photon_weta2_ptCut_tight->GetEntries() > 0) m_h_photon_weta2_ptCut_tight->Scale(1. / m_h_photon_weta2_ptCut_tight->GetEntries());
    if (m_h_photon_rhad_ptCut_all->GetEntries() > 0) m_h_photon_rhad_ptCut_all->Scale(1. / m_h_photon_rhad_ptCut_all->GetEntries());
    if (m_h_photon_rhad_ptCut_loose->GetEntries() > 0) m_h_photon_rhad_ptCut_loose->Scale(1. / m_h_photon_rhad_ptCut_loose->GetEntries());
    if (m_h_photon_rhad_ptCut_tight->GetEntries() > 0) m_h_photon_rhad_ptCut_tight->Scale(1. / m_h_photon_rhad_ptCut_tight->GetEntries());
    if (m_h_photon_wtots1_ptCut_all->GetEntries() > 0) m_h_photon_wtots1_ptCut_all->Scale(1. / m_h_photon_wtots1_ptCut_all->GetEntries());
    if (m_h_photon_wtots1_ptCut_loose->GetEntries() > 0) m_h_photon_wtots1_ptCut_loose->Scale(1. / m_h_photon_wtots1_ptCut_loose->GetEntries());
    if (m_h_photon_wtots1_ptCut_tight->GetEntries() > 0) m_h_photon_wtots1_ptCut_tight->Scale(1. / m_h_photon_wtots1_ptCut_tight->GetEntries());
    if (m_h_photon_fracs1_ptCut_all->GetEntries() > 0) m_h_photon_fracs1_ptCut_all->Scale(1. / m_h_photon_fracs1_ptCut_all->GetEntries());
    if (m_h_photon_fracs1_ptCut_loose->GetEntries() > 0) m_h_photon_fracs1_ptCut_loose->Scale(1. / m_h_photon_fracs1_ptCut_loose->GetEntries());
    if (m_h_photon_fracs1_ptCut_tight->GetEntries() > 0) m_h_photon_fracs1_ptCut_tight->Scale(1. / m_h_photon_fracs1_ptCut_tight->GetEntries());
    if (m_h_photon_deltae_ptCut_all->GetEntries() > 0) m_h_photon_deltae_ptCut_all->Scale(1. / m_h_photon_deltae_ptCut_all->GetEntries());
    if (m_h_photon_deltae_ptCut_loose->GetEntries() > 0) m_h_photon_deltae_ptCut_loose->Scale(1. / m_h_photon_deltae_ptCut_loose->GetEntries());
    if (m_h_photon_deltae_ptCut_tight->GetEntries() > 0) m_h_photon_deltae_ptCut_tight->Scale(1. / m_h_photon_deltae_ptCut_tight->GetEntries());
    if (m_h_photon_eratio_ptCut_all->GetEntries() > 0) m_h_photon_eratio_ptCut_all->Scale(1. / m_h_photon_eratio_ptCut_all->GetEntries());
    if (m_h_photon_eratio_ptCut_loose->GetEntries() > 0) m_h_photon_eratio_ptCut_loose->Scale(1. / m_h_photon_eratio_ptCut_loose->GetEntries());
    if (m_h_photon_eratio_ptCut_tight->GetEntries() > 0) m_h_photon_eratio_ptCut_tight->Scale(1. / m_h_photon_eratio_ptCut_tight->GetEntries());
    if (m_h_photon_f1_ptCut_all->GetEntries() > 0) m_h_photon_f1_ptCut_all->Scale(1. / m_h_photon_f1_ptCut_all->GetEntries());
    if (m_h_photon_f1_ptCut_loose->GetEntries() > 0) m_h_photon_f1_ptCut_loose->Scale(1. / m_h_photon_f1_ptCut_loose->GetEntries());
    if (m_h_photon_f1_ptCut_tight->GetEntries() > 0) m_h_photon_f1_ptCut_tight->Scale(1. / m_h_photon_f1_ptCut_tight->GetEntries());
    if (m_h_photon_etcone20_ptCut_fcal0->GetEntries() > 0) m_h_photon_etcone20_ptCut_fcal0->Scale(1. / m_h_photon_etcone20_ptCut_fcal0->GetEntries());
    if (m_h_photon_etcone20_ptCut_fcal1->GetEntries() > 0) m_h_photon_etcone20_ptCut_fcal1->Scale(1. / m_h_photon_etcone20_ptCut_fcal1->GetEntries());
    if (m_h_photon_etcone20_ptCut_fcal2->GetEntries() > 0) m_h_photon_etcone20_ptCut_fcal2->Scale(1. / m_h_photon_etcone20_ptCut_fcal2->GetEntries());
    if (m_h_photon_etcone30_ptCut_fcal0->GetEntries() > 0) m_h_photon_etcone30_ptCut_fcal0->Scale(1. / m_h_photon_etcone30_ptCut_fcal0->GetEntries());
    if (m_h_photon_etcone30_ptCut_fcal1->GetEntries() > 0) m_h_photon_etcone30_ptCut_fcal1->Scale(1. / m_h_photon_etcone30_ptCut_fcal1->GetEntries());
    if (m_h_photon_etcone30_ptCut_fcal2->GetEntries() > 0) m_h_photon_etcone30_ptCut_fcal2->Scale(1. / m_h_photon_etcone30_ptCut_fcal2->GetEntries());
    if (m_h_photon_etcone40_ptCut_fcal0->GetEntries() > 0) m_h_photon_etcone40_ptCut_fcal0->Scale(1. / m_h_photon_etcone40_ptCut_fcal0->GetEntries());
    if (m_h_photon_etcone40_ptCut_fcal1->GetEntries() > 0) m_h_photon_etcone40_ptCut_fcal1->Scale(1. / m_h_photon_etcone40_ptCut_fcal1->GetEntries());
    if (m_h_photon_etcone40_ptCut_fcal2->GetEntries() > 0) m_h_photon_etcone40_ptCut_fcal2->Scale(1. / m_h_photon_etcone40_ptCut_fcal2->GetEntries());
    if (m_h_photon_etcone30_ptCut_fcal0_all->GetEntries() > 0) m_h_photon_etcone30_ptCut_fcal0_all->Scale(1. / m_h_photon_etcone30_ptCut_fcal0_all->GetEntries());
    if (m_h_photon_etcone30_ptCut_fcal0_loose->GetEntries() > 0) m_h_photon_etcone30_ptCut_fcal0_loose->Scale(1. / m_h_photon_etcone30_ptCut_fcal0_loose->GetEntries());
    if (m_h_photon_etcone30_ptCut_fcal0_tight->GetEntries() > 0) m_h_photon_etcone30_ptCut_fcal0_tight->Scale(1. / m_h_photon_etcone30_ptCut_fcal0_tight->GetEntries());
    if (m_h_photon_etcone30_ptCut_fcal1_all->GetEntries() > 0) m_h_photon_etcone30_ptCut_fcal1_all->Scale(1. / m_h_photon_etcone30_ptCut_fcal1_all->GetEntries());
    if (m_h_photon_etcone30_ptCut_fcal1_loose->GetEntries() > 0) m_h_photon_etcone30_ptCut_fcal1_loose->Scale(1. / m_h_photon_etcone30_ptCut_fcal1_loose->GetEntries());
    if (m_h_photon_etcone30_ptCut_fcal1_tight->GetEntries() > 0) m_h_photon_etcone30_ptCut_fcal1_tight->Scale(1. / m_h_photon_etcone30_ptCut_fcal1_tight->GetEntries());
    if (m_h_photon_etcone30_ptCut_fcal2_all->GetEntries() > 0) m_h_photon_etcone30_ptCut_fcal2_all->Scale(1. / m_h_photon_etcone30_ptCut_fcal2_all->GetEntries());
    if (m_h_photon_etcone30_ptCut_fcal2_loose->GetEntries() > 0) m_h_photon_etcone30_ptCut_fcal2_loose->Scale(1. / m_h_photon_etcone30_ptCut_fcal2_loose->GetEntries());
    if (m_h_photon_etcone30_ptCut_fcal2_tight->GetEntries() > 0) m_h_photon_etcone30_ptCut_fcal2_tight->Scale(1. / m_h_photon_etcone30_ptCut_fcal2_tight->GetEntries());
  }

  return StatusCode::SUCCESS;
}

/// *** FCal sum Et *** ///
void HIMonitoringPhotonsTool::book_hist() {
  std::string path0 = "HeavyIon/Photons/Basic";

  m_h_photon_pt_eta = TH2D_LW::create("h_photon_pt_eta", "; photon pT; photon eta", m_PhotonPt_nbins, m_PhotonPt_low, m_PhotonPt_high, m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high);
  regHist(m_h_photon_pt_eta, path0, run).ignore();

  m_h_photon_pt_phi = TH2D_LW::create("h_photon_pt_phi", "; photon pT; photon phi", m_PhotonPt_nbins, m_PhotonPt_low, m_PhotonPt_high, m_PhotonPhi_nbins, m_PhotonPhi_low, m_PhotonPhi_high);
  regHist(m_h_photon_pt_phi, path0, run).ignore();

  m_h_photon_pt_fcal = TH2D_LW::create("h_photon_pt_fcal", "; photon pT; FCal SumET", m_PhotonPt_nbins, m_PhotonPt_low, m_PhotonPt_high, m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high);
  regHist(m_h_photon_pt_fcal, path0, run).ignore();

  m_h_photon_eta_phi_ptCut = TH2D_LW::create("h_photon_eta_phi_ptCut", "; photon eta; photon phi", m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high, m_PhotonPhi_nbins, m_PhotonPhi_low, m_PhotonPhi_high);
  regHist(m_h_photon_eta_phi_ptCut, path0, run).ignore();

  // tight versions
  m_h_photon_pt_eta_tight = TH2D_LW::create("h_photon_pt_eta_tight", "; photon pT; photon eta", m_PhotonPt_nbins, m_PhotonPt_low, m_PhotonPt_high, m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high);
  regHist(m_h_photon_pt_eta_tight, path0, run).ignore();

  m_h_photon_pt_phi_tight = TH2D_LW::create("h_photon_pt_phi_tight", "; photon pT; photon phi", m_PhotonPt_nbins, m_PhotonPt_low, m_PhotonPt_high, m_PhotonPhi_nbins, m_PhotonPhi_low, m_PhotonPhi_high);
  regHist(m_h_photon_pt_phi_tight, path0, run).ignore();

  m_h_photon_pt_fcal_tight = TH2D_LW::create("h_photon_pt_fcal_tight", "; photon pT; FCal SumET", m_PhotonPt_nbins, m_PhotonPt_low, m_PhotonPt_high, m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high);
  regHist(m_h_photon_pt_fcal_tight, path0, run).ignore();

  m_h_photon_eta_phi_ptCut_tight = TH2D_LW::create("h_photon_eta_phi_ptCut_tight", "; photon eta; photon phi", m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high, m_PhotonPhi_nbins, m_PhotonPhi_low, m_PhotonPhi_high);
  regHist(m_h_photon_eta_phi_ptCut_tight, path0, run).ignore();

  std::string path1 = "HeavyIon/Photons/ShowerShapes";

  // 1-D vs. ID cuts

  m_h_photon_reta_ptCut_all = new TH1D("h_photon_reta_ptCut_all", "; photon reta ; ", m_reta_nbins, m_reta_low, m_reta_high);
  regHist(m_h_photon_reta_ptCut_all, path1, run).ignore();
  m_h_photon_reta_ptCut_loose = new TH1D("h_photon_reta_ptCut_loose", "; photon reta ; ", m_reta_nbins, m_reta_low, m_reta_high);
  regHist(m_h_photon_reta_ptCut_loose, path1, run).ignore();
  m_h_photon_reta_ptCut_tight = new TH1D("h_photon_reta_ptCut_tight", "; photon reta ; ", m_reta_nbins, m_reta_low, m_reta_high);
  regHist(m_h_photon_reta_ptCut_tight, path1, run).ignore();

  m_h_photon_rphi_ptCut_all = new TH1D("h_photon_rphi_ptCut_all", "; photon rphi ; ", m_rphi_nbins, m_rphi_low, m_rphi_high);
  regHist(m_h_photon_rphi_ptCut_all, path1, run).ignore();
  m_h_photon_rphi_ptCut_loose = new TH1D("h_photon_rphi_ptCut_loose", "; photon rphi ; ", m_rphi_nbins, m_rphi_low, m_rphi_high);
  regHist(m_h_photon_rphi_ptCut_loose, path1, run).ignore();
  m_h_photon_rphi_ptCut_tight = new TH1D("h_photon_rphi_ptCut_tight", "; photon rphi ; ", m_rphi_nbins, m_rphi_low, m_rphi_high);
  regHist(m_h_photon_rphi_ptCut_tight, path1, run).ignore();

  m_h_photon_weta2_ptCut_all = new TH1D("h_photon_weta2_ptCut_all", "; photon weta2 ; ", m_weta2_nbins, m_weta2_low, m_weta2_high);
  regHist(m_h_photon_weta2_ptCut_all, path1, run).ignore();
  m_h_photon_weta2_ptCut_loose = new TH1D("h_photon_weta2_ptCut_loose", "; photon weta2 ; ", m_weta2_nbins, m_weta2_low, m_weta2_high);
  regHist(m_h_photon_weta2_ptCut_loose, path1, run).ignore();
  m_h_photon_weta2_ptCut_tight = new TH1D("h_photon_weta2_ptCut_tight", "; photon weta2 ; ", m_weta2_nbins, m_weta2_low, m_weta2_high);
  regHist(m_h_photon_weta2_ptCut_tight, path1, run).ignore();

  m_h_photon_rhad_ptCut_all = new TH1D("h_photon_rhad_ptCut_all", "; photon rhad ; ", m_rhad_nbins, m_rhad_low, m_rhad_high);
  regHist(m_h_photon_rhad_ptCut_all, path1, run).ignore();
  m_h_photon_rhad_ptCut_loose = new TH1D("h_photon_rhad_ptCut_loose", "; photon rhad ; ", m_rhad_nbins, m_rhad_low, m_rhad_high);
  regHist(m_h_photon_rhad_ptCut_loose, path1, run).ignore();
  m_h_photon_rhad_ptCut_tight = new TH1D("h_photon_rhad_ptCut_tight", "; photon rhad ; ", m_rhad_nbins, m_rhad_low, m_rhad_high);
  regHist(m_h_photon_rhad_ptCut_tight, path1, run).ignore();

  m_h_photon_wtots1_ptCut_all = new TH1D("h_photon_wtots1_ptCut_all", "; photon wtots1 ; ", m_wtots1_nbins, m_wtots1_low, m_wtots1_high);
  regHist(m_h_photon_wtots1_ptCut_all, path1, run).ignore();
  m_h_photon_wtots1_ptCut_loose = new TH1D("h_photon_wtots1_ptCut_loose", "; photon wtots1 ; ", m_wtots1_nbins, m_wtots1_low, m_wtots1_high);
  regHist(m_h_photon_wtots1_ptCut_loose, path1, run).ignore();
  m_h_photon_wtots1_ptCut_tight = new TH1D("h_photon_wtots1_ptCut_tight", "; photon wtots1 ; ", m_wtots1_nbins, m_wtots1_low, m_wtots1_high);
  regHist(m_h_photon_wtots1_ptCut_tight, path1, run).ignore();

  m_h_photon_fracs1_ptCut_all = new TH1D("h_photon_fracs1_ptCut_all", "; photon fracs1 ; ", m_fracs1_nbins, m_fracs1_low, m_fracs1_high);
  regHist(m_h_photon_fracs1_ptCut_all, path1, run).ignore();
  m_h_photon_fracs1_ptCut_loose = new TH1D("h_photon_fracs1_ptCut_loose", "; photon fracs1 ; ", m_fracs1_nbins, m_fracs1_low, m_fracs1_high);
  regHist(m_h_photon_fracs1_ptCut_loose, path1, run).ignore();
  m_h_photon_fracs1_ptCut_tight = new TH1D("h_photon_fracs1_ptCut_tight", "; photon fracs1 ; ", m_fracs1_nbins, m_fracs1_low, m_fracs1_high);
  regHist(m_h_photon_fracs1_ptCut_tight, path1, run).ignore();

  m_h_photon_deltae_ptCut_all = new TH1D("h_photon_deltae_ptCut_all", "; photon deltae ; ", m_deltae_nbins, m_deltae_low, m_deltae_high);
  regHist(m_h_photon_deltae_ptCut_all, path1, run).ignore();
  m_h_photon_deltae_ptCut_loose = new TH1D("h_photon_deltae_ptCut_loose", "; photon deltae ; ", m_deltae_nbins, m_deltae_low, m_deltae_high);
  regHist(m_h_photon_deltae_ptCut_loose, path1, run).ignore();
  m_h_photon_deltae_ptCut_tight = new TH1D("h_photon_deltae_ptCut_tight", "; photon deltae ; ", m_deltae_nbins, m_deltae_low, m_deltae_high);
  regHist(m_h_photon_deltae_ptCut_tight, path1, run).ignore();

  m_h_photon_eratio_ptCut_all = new TH1D("h_photon_eratio_ptCut_all", "; photon eratio ; ", m_eratio_nbins, m_eratio_low, m_eratio_high);
  regHist(m_h_photon_eratio_ptCut_all, path1, run).ignore();
  m_h_photon_eratio_ptCut_loose = new TH1D("h_photon_eratio_ptCut_loose", "; photon eratio ; ", m_eratio_nbins, m_eratio_low, m_eratio_high);
  regHist(m_h_photon_eratio_ptCut_loose, path1, run).ignore();
  m_h_photon_eratio_ptCut_tight = new TH1D("h_photon_eratio_ptCut_tight", "; photon eratio ; ", m_eratio_nbins, m_eratio_low, m_eratio_high);
  regHist(m_h_photon_eratio_ptCut_tight, path1, run).ignore();

  m_h_photon_f1_ptCut_all = new TH1D("h_photon_f1_ptCut_all", "; photon f1 ; ", m_f1_nbins, m_f1_low, m_f1_high);
  regHist(m_h_photon_f1_ptCut_all, path1, run).ignore();
  m_h_photon_f1_ptCut_loose = new TH1D("h_photon_f1_ptCut_loose", "; photon f1 ; ", m_f1_nbins, m_f1_low, m_f1_high);
  regHist(m_h_photon_f1_ptCut_loose, path1, run).ignore();
  m_h_photon_f1_ptCut_tight = new TH1D("h_photon_f1_ptCut_tight", "; photon f1 ; ", m_f1_nbins, m_f1_low, m_f1_high);
  regHist(m_h_photon_f1_ptCut_tight, path1, run).ignore();

  // vs. eta
  m_h_photon_reta_eta_ptCut = TH2D_LW::create("h_photon_reta_eta_ptCut", "; photon eta; photon reta", m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high, m_reta_nbins, m_reta_low, m_reta_high);
  regHist(m_h_photon_reta_eta_ptCut, path1, run).ignore();
  m_h_photon_rphi_eta_ptCut = TH2D_LW::create("h_photon_rphi_eta_ptCut", "; photon eta; photon rphi", m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high, m_rphi_nbins, m_rphi_low, m_rphi_high);
  regHist(m_h_photon_rphi_eta_ptCut, path1, run).ignore();
  m_h_photon_weta2_eta_ptCut = TH2D_LW::create("h_photon_weta2_eta_ptCut", "; photon eta; photon weta2", m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high, m_weta2_nbins, m_weta2_low, m_weta2_high);
  regHist(m_h_photon_weta2_eta_ptCut, path1, run).ignore();

  m_h_photon_rhad_eta_ptCut = TH2D_LW::create("h_photon_rhad_eta_ptCut", "; photon eta; photon rhad", m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high, m_rhad_nbins, m_rhad_low, m_rhad_high);
  regHist(m_h_photon_rhad_eta_ptCut, path1, run).ignore();
  m_h_photon_wtots1_eta_ptCut = TH2D_LW::create("h_photon_wtots1_eta_ptCut", "; photon eta; photon wtots1", m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high, m_wtots1_nbins, m_wtots1_low, m_wtots1_high);
  regHist(m_h_photon_wtots1_eta_ptCut, path1, run).ignore();
  m_h_photon_fracs1_eta_ptCut = TH2D_LW::create("h_photon_fracs1_eta_ptCut", "; photon eta; photon fracs1", m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high, m_fracs1_nbins, m_fracs1_low, m_fracs1_high);
  regHist(m_h_photon_fracs1_eta_ptCut, path1, run).ignore();

  m_h_photon_deltae_eta_ptCut = TH2D_LW::create("h_photon_deltae_eta_ptCut", "; photon eta; photon deltae", m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high, m_deltae_nbins, m_deltae_low, m_deltae_high);
  regHist(m_h_photon_deltae_eta_ptCut, path1, run).ignore();
  m_h_photon_eratio_eta_ptCut = TH2D_LW::create("h_photon_eratio_eta_ptCut", "; photon eta; photon eratio", m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high, m_eratio_nbins, m_eratio_low, m_eratio_high);
  regHist(m_h_photon_eratio_eta_ptCut, path1, run).ignore();
  m_h_photon_f1_eta_ptCut = TH2D_LW::create("h_photon_f1_eta_ptCut", "; photon eta; photon f1", m_PhotonEta_nbins, m_PhotonEta_low, m_PhotonEta_high, m_f1_nbins, m_f1_low, m_f1_high);
  regHist(m_h_photon_f1_eta_ptCut, path1, run).ignore();


  // vs. FCal ET

  m_h_photon_reta_fcal_ptCut_eta0 = TH2D_LW::create("h_photon_reta_fcal_ptCut_eta0", "; FCal Sum ET; photon reta", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_reta_nbins, m_reta_low, m_reta_high);
  regHist(m_h_photon_reta_fcal_ptCut_eta0, path1, run).ignore();
  m_h_photon_reta_fcal_ptCut_eta1 = TH2D_LW::create("h_photon_reta_fcal_ptCut_eta1", "; FCal Sum ET; photon reta", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_reta_nbins, m_reta_low, m_reta_high);
  regHist(m_h_photon_reta_fcal_ptCut_eta1, path1, run).ignore();

  m_h_photon_rphi_fcal_ptCut_eta0 = TH2D_LW::create("h_photon_rphi_fcal_ptCut_eta0", "; FCal Sum ET; photon rphi", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_rphi_nbins, m_rphi_low, m_rphi_high);
  regHist(m_h_photon_rphi_fcal_ptCut_eta0, path1, run).ignore();
  m_h_photon_rphi_fcal_ptCut_eta1 = TH2D_LW::create("h_photon_rphi_fcal_ptCut_eta1", "; FCal Sum ET; photon rphi", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_rphi_nbins, m_rphi_low, m_rphi_high);
  regHist(m_h_photon_rphi_fcal_ptCut_eta1, path1, run).ignore();

  m_h_photon_weta2_fcal_ptCut_eta0 = TH2D_LW::create("h_photon_weta2_fcal_ptCut_eta0", "; FCal Sum ET; photon weta2", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_weta2_nbins, m_weta2_low, m_weta2_high);
  regHist(m_h_photon_weta2_fcal_ptCut_eta0, path1, run).ignore();
  m_h_photon_weta2_fcal_ptCut_eta1 = TH2D_LW::create("h_photon_weta2_fcal_ptCut_eta1", "; FCal Sum ET; photon weta2", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_weta2_nbins, m_weta2_low, m_weta2_high);
  regHist(m_h_photon_weta2_fcal_ptCut_eta1, path1, run).ignore();


  m_h_photon_rhad_fcal_ptCut_eta0 = TH2D_LW::create("h_photon_rhad_fcal_ptCut_eta0", "; FCal Sum ET; photon rhad", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_rhad_nbins, m_rhad_low, m_rhad_high);
  regHist(m_h_photon_rhad_fcal_ptCut_eta0, path1, run).ignore();
  m_h_photon_rhad_fcal_ptCut_eta1 = TH2D_LW::create("h_photon_rhad_fcal_ptCut_eta1", "; FCal Sum ET; photon rhad", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_rhad_nbins, m_rhad_low, m_rhad_high);
  regHist(m_h_photon_rhad_fcal_ptCut_eta1, path1, run).ignore();

  m_h_photon_wtots1_fcal_ptCut_eta0 = TH2D_LW::create("h_photon_wtots1_fcal_ptCut_eta0", "; FCal Sum ET; photon wtots1", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_wtots1_nbins, m_wtots1_low, m_wtots1_high);
  regHist(m_h_photon_wtots1_fcal_ptCut_eta0, path1, run).ignore();
  m_h_photon_wtots1_fcal_ptCut_eta1 = TH2D_LW::create("h_photon_wtots1_fcal_ptCut_eta1", "; FCal Sum ET; photon wtots1", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_wtots1_nbins, m_wtots1_low, m_wtots1_high);
  regHist(m_h_photon_wtots1_fcal_ptCut_eta1, path1, run).ignore();

  m_h_photon_fracs1_fcal_ptCut_eta0 = TH2D_LW::create("h_photon_fracs1_fcal_ptCut_eta0", "; FCal Sum ET; photon fracs1", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_fracs1_nbins, m_fracs1_low, m_fracs1_high);
  regHist(m_h_photon_fracs1_fcal_ptCut_eta0, path1, run).ignore();
  m_h_photon_fracs1_fcal_ptCut_eta1 = TH2D_LW::create("h_photon_fracs1_fcal_ptCut_eta1", "; FCal Sum ET; photon fracs1", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_fracs1_nbins, m_fracs1_low, m_fracs1_high);
  regHist(m_h_photon_fracs1_fcal_ptCut_eta1, path1, run).ignore();


  m_h_photon_deltae_fcal_ptCut_eta0 = TH2D_LW::create("h_photon_deltae_fcal_ptCut_eta0", "; FCal Sum ET; photon deltae", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_deltae_nbins, m_deltae_low, m_deltae_high);
  regHist(m_h_photon_deltae_fcal_ptCut_eta0, path1, run).ignore();
  m_h_photon_deltae_fcal_ptCut_eta1 = TH2D_LW::create("h_photon_deltae_fcal_ptCut_eta1", "; FCal Sum ET; photon deltae", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_deltae_nbins, m_deltae_low, m_deltae_high);
  regHist(m_h_photon_deltae_fcal_ptCut_eta1, path1, run).ignore();

  m_h_photon_eratio_fcal_ptCut_eta0 = TH2D_LW::create("h_photon_eratio_fcal_ptCut_eta0", "; FCal Sum ET; photon eratio", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_eratio_nbins, m_eratio_low, m_eratio_high);
  regHist(m_h_photon_eratio_fcal_ptCut_eta0, path1, run).ignore();
  m_h_photon_eratio_fcal_ptCut_eta1 = TH2D_LW::create("h_photon_eratio_fcal_ptCut_eta1", "; FCal Sum ET; photon eratio", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_eratio_nbins, m_eratio_low, m_eratio_high);
  regHist(m_h_photon_eratio_fcal_ptCut_eta1, path1, run).ignore();

  m_h_photon_f1_fcal_ptCut_eta0 = TH2D_LW::create("h_photon_f1_fcal_ptCut_eta0", "; FCal Sum ET; photon f1", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_f1_nbins, m_f1_low, m_f1_high);
  regHist(m_h_photon_f1_fcal_ptCut_eta0, path1, run).ignore();
  m_h_photon_f1_fcal_ptCut_eta1 = TH2D_LW::create("h_photon_f1_fcal_ptCut_eta1", "; FCal Sum ET; photon f1", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_f1_nbins, m_f1_low, m_f1_high);
  regHist(m_h_photon_f1_fcal_ptCut_eta1, path1, run).ignore();

  std::string path2 = "HeavyIon/Photons/Isolation";

  m_h_photon_fcal_etcone20_ptCut = TH2D_LW::create("h_photon_fcal_etcone20_ptCut", "; FCal Sum ET; photon etcone20", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_IsoWide_nbins, m_IsoWide_low, m_IsoWide_high);
  regHist(m_h_photon_fcal_etcone20_ptCut, path2, run).ignore();

  m_h_photon_fcal_etcone30_ptCut = TH2D_LW::create("h_photon_fcal_etcone30_ptCut", "; FCal Sum ET; photon etcone30", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_IsoWide_nbins, m_IsoWide_low, m_IsoWide_high);
  regHist(m_h_photon_fcal_etcone30_ptCut, path2, run).ignore();

  m_h_photon_fcal_etcone40_ptCut = TH2D_LW::create("h_photon_fcal_etcone40_ptCut", "; FCal Sum ET; photon etcone40", m_FCalEt_nbins, m_FCalEt_low, m_FCalEt_high, m_IsoWide_nbins, m_IsoWide_low, m_IsoWide_high);
  regHist(m_h_photon_fcal_etcone40_ptCut, path2, run).ignore();

  m_h_photon_pt_etcone40_fcal0 = TH2D_LW::create("h_photon_pt_etcone40_fcal0", "; photon pT; photon etcone40", m_PhotonPt_nbins, m_PhotonPt_low, m_PhotonPt_high, m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_pt_etcone40_fcal0, path2, run).ignore();

  m_h_photon_pt_etcone40_fcal1 = TH2D_LW::create("h_photon_pt_etcone40_fcal1", "; photon pT; photon etcone40", m_PhotonPt_nbins, m_PhotonPt_low, m_PhotonPt_high, m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_pt_etcone40_fcal1, path2, run).ignore();

  m_h_photon_pt_etcone40_fcal2 = TH2D_LW::create("h_photon_pt_etcone40_fcal2", "; photon pT; photon etcone40", m_PhotonPt_nbins, m_PhotonPt_low, m_PhotonPt_high, m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_pt_etcone40_fcal2, path2, run).ignore();

  // 1-D vs. FCal ET
  m_h_photon_etcone20_ptCut_fcal0 = new TH1D("h_photon_etcone20_ptCut_fcal0", "; photon etcone20", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone20_ptCut_fcal0, path2, run).ignore();

  m_h_photon_etcone20_ptCut_fcal1 = new TH1D("h_photon_etcone20_ptCut_fcal1", "; photon etcone20", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone20_ptCut_fcal1, path2, run).ignore();

  m_h_photon_etcone20_ptCut_fcal2 = new TH1D("h_photon_etcone20_ptCut_fcal2", "; photon etcone20", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone20_ptCut_fcal2, path2, run).ignore();

  m_h_photon_etcone30_ptCut_fcal0 = new TH1D("h_photon_etcone30_ptCut_fcal0", "; photon etcone30", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone30_ptCut_fcal0, path2, run).ignore();

  m_h_photon_etcone30_ptCut_fcal1 = new TH1D("h_photon_etcone30_ptCut_fcal1", "; photon etcone30", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone30_ptCut_fcal1, path2, run).ignore();

  m_h_photon_etcone30_ptCut_fcal2 = new TH1D("h_photon_etcone30_ptCut_fcal2", "; photon etcone30", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone30_ptCut_fcal2, path2, run).ignore();

  m_h_photon_etcone40_ptCut_fcal0 = new TH1D("h_photon_etcone40_ptCut_fcal0", "; photon etcone40", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone40_ptCut_fcal0, path2, run).ignore();

  m_h_photon_etcone40_ptCut_fcal1 = new TH1D("h_photon_etcone40_ptCut_fcal1", "; photon etcone40", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone40_ptCut_fcal1, path2, run).ignore();

  m_h_photon_etcone40_ptCut_fcal2 = new TH1D("h_photon_etcone40_ptCut_fcal2", "; photon etcone40", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone40_ptCut_fcal2, path2, run).ignore();

  // 1-D vs. ID

  m_h_photon_etcone30_ptCut_fcal0_all = new TH1D("h_photon_etcone30_ptCut_fcal0_all", "; photon etcone30", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone30_ptCut_fcal0_all, path2, run).ignore();
  m_h_photon_etcone30_ptCut_fcal0_loose = new TH1D("h_photon_etcone30_ptCut_fcal0_loose", "; photon etcone30", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone30_ptCut_fcal0_loose, path2, run).ignore();
  m_h_photon_etcone30_ptCut_fcal0_tight = new TH1D("h_photon_etcone30_ptCut_fcal0_tight", "; photon etcone30", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone30_ptCut_fcal0_tight, path2, run).ignore();

  m_h_photon_etcone30_ptCut_fcal1_all = new TH1D("h_photon_etcone30_ptCut_fcal1_all", "; photon etcone30", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone30_ptCut_fcal1_all, path2, run).ignore();
  m_h_photon_etcone30_ptCut_fcal1_loose = new TH1D("h_photon_etcone30_ptCut_fcal1_loose", "; photon etcone30", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone30_ptCut_fcal1_loose, path2, run).ignore();
  m_h_photon_etcone30_ptCut_fcal1_tight = new TH1D("h_photon_etcone30_ptCut_fcal1_tight", "; photon etcone30", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone30_ptCut_fcal1_tight, path2, run).ignore();

  m_h_photon_etcone30_ptCut_fcal2_all = new TH1D("h_photon_etcone30_ptCut_fcal2_all", "; photon etcone30", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone30_ptCut_fcal2_all, path2, run).ignore();
  m_h_photon_etcone30_ptCut_fcal2_loose = new TH1D("h_photon_etcone30_ptCut_fcal2_loose", "; photon etcone30", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone30_ptCut_fcal2_loose, path2, run).ignore();
  m_h_photon_etcone30_ptCut_fcal2_tight = new TH1D("h_photon_etcone30_ptCut_fcal2_tight", "; photon etcone30", m_IsoNarrow_nbins, m_IsoNarrow_low, m_IsoNarrow_high);
  regHist(m_h_photon_etcone30_ptCut_fcal2_tight, path2, run).ignore();
}
