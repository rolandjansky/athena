/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DiTauRec/ElMuFinder.h"
#include "DiTauRec/DiTauToolBase.h"

#include "DiTauRec/DiTauCandidateData.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

// #include "MuonSelectorTools/errorcheck.h"
#include "MuonSelectorTools/MuonSelectionTool.h"

#include "tauRecTools/KineUtils.h"
//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

ElMuFinder::ElMuFinder(const std::string& type,
    const std::string& name,
    const IInterface * parent) :
    DiTauToolBase(type, name, parent),
    m_elContName("Electrons"),
    m_elMinPt(7000),
    m_elMaxEta(2.47),
    m_muContName("Muons"),
    m_muMinPt(7000),
    m_muMaxEta(2.47),
    m_muQual(2)
{
    declareInterface<DiTauToolBase > (this);
    declareProperty("ElectronContainer", m_elContName);
    declareProperty("ElectronMinPt", m_elMinPt);
    declareProperty("ElectronMaxEta", m_elMaxEta);
    declareProperty("MuonContainer", m_muContName);
    declareProperty("MuonMinPt", m_muMinPt);
    declareProperty("MuonMaxEta", m_muMaxEta);
    declareProperty("MuonQuality", m_muQual);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

ElMuFinder::~ElMuFinder() {
}

//-------------------------------------------------------------------------
// initialize
//-------------------------------------------------------------------------

StatusCode ElMuFinder::initialize() {

    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// Event Finalize
//-------------------------------------------------------------------------

StatusCode ElMuFinder::eventFinalize(DiTauCandidateData * ) {

    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// execute
//-------------------------------------------------------------------------

StatusCode ElMuFinder::execute(DiTauCandidateData * data) {

    ATH_MSG_DEBUG("execute ElMuFinder...");

    xAOD::DiTauJet *pDiTau = data->xAODDiTau;

    if (pDiTau == NULL) {
        ATH_MSG_ERROR("no di-tau candidate given");
        return StatusCode::FAILURE;
    }

    StatusCode sc;

    const xAOD::ElectronContainer* pElCont = 0;
    sc = evtStore()->retrieve(pElCont, m_elContName);
    if (sc.isFailure() || !pElCont) {
        ATH_MSG_WARNING("could not find electrons with key:" << m_elContName <<
                        " Continue without electron and muon finding.");
        return StatusCode::SUCCESS;
    }

    const xAOD::MuonContainer* pMuCont = 0;
    sc = evtStore()->retrieve(pMuCont, m_muContName);
    if (sc.isFailure() || !pMuCont) {        
        ATH_MSG_WARNING("could not find muons with key:" << m_muContName <<
                        " Continue without electron and muon finding.");
        return StatusCode::SUCCESS;
    }


    // select electrons
    data->electrons.clear();
    float dR;
    for (const auto& el : *pElCont ) {
        ATH_MSG_DEBUG("electron pt:" << el->pt() << " eta:" << el->eta() << " phi:" << el->phi());
        if ( !el->author(xAOD::EgammaParameters::AuthorElectron) && 
             !el->author(xAOD::EgammaParameters::AuthorAmbiguous) )
            continue;    
        ATH_MSG_DEBUG("Electron passes author selection.");

        if (el->pt() < m_elMinPt || fabs(el->eta()) > m_elMaxEta)
            continue;
        ATH_MSG_DEBUG("Electron passes basic kinematic selection");

        // electron inside seed jet area?
        dR = Tau1P3PKineUtils::deltaR(data->seed->eta(), data->seed->phi(), el->eta(), el->phi());
        if (dR > data->Rjet)
            continue;

        data->electrons.push_back(el);
    }
    ATH_MSG_DEBUG("Number of good electrons found: " << data->electrons.size() );

    // select muons
    data->muons.clear();
    for (const auto& mu : *pMuCont) {
        ATH_MSG_DEBUG("muon pt:" << mu->pt() << " eta:" << mu->eta() << " ");
	    xAOD::Muon::Quality muonQuality = mu->quality();
	    if (muonQuality >= m_muQual && std::abs(mu->eta()) >= m_muMaxEta) continue;

        // electron inside seed jet area?
        dR = Tau1P3PKineUtils::deltaR(data->seed->eta(), data->seed->phi(), mu->eta(), mu->phi());
        if (dR > data->Rjet)
            continue;
        data->muons.push_back(mu);
    }
    ATH_MSG_DEBUG("Number of good muons found: " << data->muons.size() );

    return StatusCode::SUCCESS;
}
