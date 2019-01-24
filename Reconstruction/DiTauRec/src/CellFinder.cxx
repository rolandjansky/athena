/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "DiTauRec/CellFinder.h"
#include "DiTauRec/DiTauToolBase.h"

#include "DiTauRec/DiTauCandidateData.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "fastjet/PseudoJet.hh"

#include "tauRecTools/KineUtils.h"



//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

CellFinder::CellFinder(const std::string& type,
    const std::string& name,
    const IInterface * parent) :
    DiTauToolBase(type, name, parent),
    m_bWriteJetCells(false),
    m_bWriteSubjetCells(false),
    m_ClusterContainerName("CaloCalTopoClusters"),
    m_CellContainerName("AllCalo"),
    m_Rsubjet(0.2)
{
    declareInterface<DiTauToolBase > (this);
    declareProperty("writeJetCells", m_bWriteJetCells);
    declareProperty("writeSubjetCells", m_bWriteSubjetCells);
    declareProperty("ClusterContainer", m_ClusterContainerName);
    declareProperty("CellContainer", m_CellContainerName);
    declareProperty("Rsubjet", m_Rsubjet);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

CellFinder::~CellFinder() {
}

//-------------------------------------------------------------------------
// initialize
//-------------------------------------------------------------------------

StatusCode CellFinder::initialize() {

    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// execute
//-------------------------------------------------------------------------

StatusCode CellFinder::execute(DiTauCandidateData * data,
                               const EventContext& /*ctx*/) const {

    ATH_MSG_DEBUG("execute CellFinder...");

    // ----------------------------------------------------------------------------
    // get ditau and its seed jet
    // ----------------------------------------------------------------------------

    xAOD::DiTauJet* pDiTau = data->xAODDiTau;
    if (!pDiTau) {
        ATH_MSG_ERROR("no di-tau candidate given");
        return StatusCode::FAILURE;
    }

    const xAOD::Jet* pSeed = data->seed;
    if (!pSeed) {
        ATH_MSG_WARNING("No jet seed given.");
        return StatusCode::FAILURE;
    }

    std::vector<fastjet::PseudoJet> vSubjets = data->subjets;
    if (vSubjets.size()==0) {
        ATH_MSG_WARNING("No subjets given. Continue without cell information.");
        return StatusCode::SUCCESS;
    }

    // ----------------------------------------------------------------------------
    // get clusters linked to the seed jet. Loop over clusters to get linked cells
    // ----------------------------------------------------------------------------
    std::bitset<200000> cellSeen;
    std::vector<const CaloCell*> subjetCells;

    // loop over seed jet constituents
    for (const auto& seedConst: pSeed->getConstituents()) {
        // cast jet constituent to cluster object
        const xAOD::CaloCluster* cluster = dynamic_cast<const xAOD::CaloCluster*>( seedConst->rawConstituent() );

        // loop over cells which are linked to the cluster
        for (const auto& cc : *(cluster->getCellLinks())) {
            // skip if pt<0 or cell already encountered
            if (cc->pt() < 0) continue;
            if (cellSeen.test(cc->caloDDE()->calo_hash())) continue;
            // register cell hash as already seen
            cellSeen.set(cc->caloDDE()->calo_hash());
            

            // check if cell is in one of the subjets cones
            double dR;
            for (const auto& subjet : vSubjets) {
                dR = Tau1P3PKineUtils::deltaR(subjet.eta(), subjet.phi_std(), cc->eta(), cc->phi());
                if (dR < m_Rsubjet) {
                    subjetCells.push_back(cc);
                }
            }
        }
    }

    ATH_MSG_DEBUG("subjetCells.size()=" << subjetCells.size());
    data->subjetCells = subjetCells;


    return StatusCode::SUCCESS;
}

