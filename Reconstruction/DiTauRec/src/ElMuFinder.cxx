/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "DiTauRec/ElMuFinder.h"
#include "DiTauRec/DiTauToolBase.h"

#include "DiTauRec/DiTauCandidateData.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

#include "tauRecTools/KineUtils.h"

#include "StoreGate/ReadHandle.h"


ElMuFinder::ElMuFinder(const std::string& type,
		       const std::string& name,
		       const IInterface * parent) :
  DiTauToolBase(type, name, parent),
  m_elMinPt(7000),
  m_elMaxEta(2.47),
  m_muMinPt(7000),
  m_muMaxEta(2.47),
  m_muQual(2)
{
  declareInterface<DiTauToolBase > (this);
  declareProperty("ElectronMinPt", m_elMinPt);
  declareProperty("ElectronMaxEta", m_elMaxEta);
  declareProperty("MuonMinPt", m_muMinPt);
  declareProperty("MuonMaxEta", m_muMaxEta);
  declareProperty("MuonQuality", m_muQual);
}


ElMuFinder::~ElMuFinder() {
}


StatusCode ElMuFinder::initialize() {

  ATH_CHECK( m_elContName.initialize() );
  ATH_CHECK( m_muContName.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode ElMuFinder::execute(DiTauCandidateData * data,
                               const EventContext& ctx) const {

  ATH_MSG_DEBUG("execute ElMuFinder...");

  xAOD::DiTauJet *pDiTau = data->xAODDiTau;

  if (pDiTau == nullptr) {
    ATH_MSG_ERROR("no di-tau candidate given");
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::ElectronContainer> pElCont (m_elContName, ctx);
  SG::ReadHandle<xAOD::MuonContainer> pMuCont (m_muContName, ctx);

  // select electrons
  float dR;
  data->electrons.clear();
  if (pElCont.isValid()) {
    for (const auto el : *pElCont ) {
      ATH_MSG_DEBUG("electron pt:" << el->pt() << " eta:" << el->eta() << " phi:" << el->phi());
      if ( !el->author(xAOD::EgammaParameters::AuthorElectron) && 
	   !el->author(xAOD::EgammaParameters::AuthorAmbiguous) )
	continue;    
      ATH_MSG_DEBUG("Electron passes author selection.");
    
      if (el->pt() < m_elMinPt || std::abs(el->eta()) > m_elMaxEta)
	continue;
      ATH_MSG_DEBUG("Electron passes basic kinematic selection");
    
      // electron inside seed jet area?
      dR = Tau1P3PKineUtils::deltaR(data->seed->eta(), data->seed->phi(), el->eta(), el->phi());
      if (dR > data->Rjet)
	continue;
    
      data->electrons.push_back(el);
    }
    ATH_MSG_DEBUG("Number of good electrons found: " << data->electrons.size() );
  }
  else {
    ATH_MSG_WARNING("Can not obtain ElectronContainer with key " << m_elContName);
  }

  // select muons
  data->muons.clear();
  if (pMuCont.isValid()) {
    for (const auto mu : *pMuCont) {
      ATH_MSG_DEBUG("muon pt:" << mu->pt() << " eta:" << mu->eta() << " ");
      xAOD::Muon::Quality muonQuality = mu->quality();
      // FIXME: to be checked
      if (muonQuality >= m_muQual && std::abs(mu->eta()) >= m_muMaxEta) continue;
    
      // muon inside seed jet area?
      dR = Tau1P3PKineUtils::deltaR(data->seed->eta(), data->seed->phi(), mu->eta(), mu->phi());
      if (dR > data->Rjet)
	continue;
      data->muons.push_back(mu);
    }
    ATH_MSG_DEBUG("Number of good muons found: " << data->muons.size() );
  }
  else {
    ATH_MSG_WARNING("Can not obtain MuonContainer with key " << m_muContName);
  }

  return StatusCode::SUCCESS;
}
