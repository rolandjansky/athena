/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/


#include "DiTauRec/IDVarCalculator.h"
#include "DiTauRec/DiTauToolBase.h"

#include "DiTauRec/DiTauCandidateData.h"

#include "fastjet/PseudoJet.hh"
#include "tauRecTools/KineUtils.h"


//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

IDVarCalculator::IDVarCalculator(const std::string& type,
    const std::string& name,
    const IInterface * parent) :
    DiTauToolBase(type, name, parent),
    m_useCells(true)
{
    declareInterface<DiTauToolBase > (this);
    declareProperty("useCells", m_useCells);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

IDVarCalculator::~IDVarCalculator() {
}

//-------------------------------------------------------------------------
// initialize
//-------------------------------------------------------------------------

StatusCode IDVarCalculator::initialize() {

    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// execute
//-------------------------------------------------------------------------

StatusCode IDVarCalculator::execute(DiTauCandidateData * data,
                                    const EventContext& /*ctx*/) const {

    ATH_MSG_DEBUG("execute IDVarCalculator...");

    // ----------------------------------------------------------------------------
    // get ditau elements
    // ----------------------------------------------------------------------------

    // ditau
    xAOD::DiTauJet* pDiTau = data->xAODDiTau;
    if (!pDiTau) {
        ATH_MSG_ERROR("no di-tau candidate given");
        return StatusCode::FAILURE;
    }

    // seed jet
    const xAOD::Jet* pSeed = data->seed;
    if (!pSeed) {
        ATH_MSG_WARNING("No jet seed given.");
        return StatusCode::FAILURE;
    }

    // subjets
    std::vector<fastjet::PseudoJet> vSubjets = data->subjets;
    if (vSubjets.size()==0) {
        ATH_MSG_WARNING("No subjets given. Continue without ID variable calculation.");
        return StatusCode::SUCCESS;
    }

    // cells if available 
    bool useCells = m_useCells;;
    std::vector<const CaloCell*> vSubjetCells = data->subjetCells;
    if (vSubjetCells.size()==0) {
        ATH_MSG_DEBUG("No cell information available.");
        useCells = false; 
    } 


    // ----------------------------------------------------------------------------
    // write subjets
    // ----------------------------------------------------------------------------
    for (unsigned int i = 0; i < vSubjets.size(); i++) {
        fastjet::PseudoJet subjet = vSubjets.at(i);
        pDiTau->setSubjetPtEtaPhiE(i, subjet.pt(), subjet.eta(), subjet.phi_std(), subjet.e());
        ATH_MSG_DEBUG("subjet " << i << " pt: " << subjet.pt() << " eta: " << subjet.eta() << " phi: " << subjet.phi_std() << " e: " << subjet.e());
    }


    // ----------------------------------------------------------------------------
    // write f_core
    // ----------------------------------------------------------------------------
    if (useCells == false) {
        ATH_MSG_DEBUG("no cells are used for ID variable calculation. Continue.");
        return StatusCode::SUCCESS;
    }


    float Rcore = data->Rcore;
    float Rsubjet = data->Rsubjet;
    float f_core;
    float dR;
    for (unsigned int i = 0; i < vSubjets.size(); i++) {
        fastjet::PseudoJet subjet = vSubjets.at(i);
        float ptAll = 0;
        float ptCore = 0;

        for (const auto& cc : vSubjetCells) {
            dR = Tau1P3PKineUtils::deltaR(subjet.eta(), subjet.phi_std(), cc->eta(), cc->phi());

            if (dR < Rsubjet) {
                ptAll += cc->pt();
            }
            if (dR < Rcore) {
                ptCore += cc->pt();
            }
        }

        if (ptAll != 0 && ptCore != 0)
            f_core = ptCore/ptAll;
        else 
            f_core = -999;
        ATH_MSG_DEBUG("subjet "<< i << ": f_core=" << f_core);
        pDiTau->setfCore(i, f_core);
    }



    return StatusCode::SUCCESS;
}


