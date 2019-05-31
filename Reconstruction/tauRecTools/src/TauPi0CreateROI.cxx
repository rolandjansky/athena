/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
//-----------------------------------------------------------------------------
// file:        TauPi0CreateROI.cxx
// package:     Reconstruction/tauEvent
// authors:     Will Davey, Benedict Winter, Stephanie Yuen
// date:        2012-10-09
//-----------------------------------------------------------------------------

#include <TString.h>

//#include "GaudiKernel/IToolSvc.h"

//#include "CaloEvent/CaloCellContainer.h"
//#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloUtils/CaloCellList.h"

//#include "AthContainers/OwnershipPolicy.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "TauPi0CreateROI.h"

#include <boost/scoped_ptr.hpp>

using std::vector;
using std::string;

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

TauPi0CreateROI::TauPi0CreateROI(   const string& name ) :
     TauRecToolBase(name)
{
}
   
//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

TauPi0CreateROI::~TauPi0CreateROI() {
}

StatusCode TauPi0CreateROI::initialize() {
    
    // retrieve tools
    ATH_MSG_DEBUG( "Retrieving tools" );
    
    ATH_CHECK( m_caloCellInputContainer.initialize() );

    return StatusCode::SUCCESS;
}

StatusCode TauPi0CreateROI::eventInitialize() {
    
    const CaloCell_ID* cellID;
    if((detStore()->retrieve(cellID)).isFailure()){
        ATH_MSG_ERROR("Unable to retrieve caloCell_ID helper from DetectorStore");
        return StatusCode::FAILURE;
    }

    // Get hash range
    IdentifierHash hashMax;
    hashMax = cellID->calo_cell_hash_max();
    ATH_MSG_VERBOSE("CaloCell Hash Max: " << hashMax);

    // Reset addedCellsMap
    m_addedCellsMap.clear();
    for (unsigned i = 0; i < hashMax; i++) {
        m_addedCellsMap.push_back(NULL);
    }

    return StatusCode::SUCCESS;
}

StatusCode TauPi0CreateROI::executePi0CreateROI(xAOD::TauJet& pTau, CaloCellContainer& pPi0CellContainer) {

    //---------------------------------------------------------------------
    // only run on 1-5 prong taus 
    //---------------------------------------------------------------------
    if (pTau.nTracks() == 0 || pTau.nTracks() >5 ) {
        return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("new tau. \tpt = " << pTau.pt() << "\teta = " << pTau.eta() << "\tphi = " << pTau.phi() << "\tnprongs = " << pTau.nTracks());

    //---------------------------------------------------------------------
    // retrieve cells around tau 
    //---------------------------------------------------------------------
    // get all calo cell container
    SG::ReadHandle<CaloCellContainer> caloCellInHandle( m_caloCellInputContainer );
    if (!caloCellInHandle.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << caloCellInHandle.key());
      return StatusCode::FAILURE;
    }
    const CaloCellContainer *pCellContainer = NULL;
    pCellContainer = caloCellInHandle.cptr();
    
    // get only EM cells within dR<0.4
    vector<CaloCell_ID::SUBCALO> emSubCaloBlocks;
    emSubCaloBlocks.push_back(CaloCell_ID::LAREM);
    boost::scoped_ptr<CaloCellList> pCells(new CaloCellList(pCellContainer,emSubCaloBlocks)); 
    pCells->select(pTau.eta(), pTau.phi(), 0.4); // TODO: change hardcoded 0.4 to tau cone variable, (or func. from TauJet)?

    //---------------------------------------------------------------------
    // Put Ecal cells in output container
    //---------------------------------------------------------------------

    CaloCellList::list_iterator cellItr(pCells->begin()), cellItrE(pCells->end());
    for(; cellItr != cellItrE; ++cellItr) {
        const CaloCell* cell = (*cellItr);

        // only keep cells that are in Ecal (PS, EM1, EM2 and EM3, both barrel and endcap).
        int samp = cell->caloDDE()->getSampling();
        if(samp>7) continue;

        // Store cell in output container
        storeCell(cell, pPi0CellContainer);
    }

    return StatusCode::SUCCESS;
}

StatusCode TauPi0CreateROI::eventFinalize() {

  return StatusCode::SUCCESS;
}

StatusCode TauPi0CreateROI::finalize() {
    return StatusCode::SUCCESS;
}

void TauPi0CreateROI::storeCell(const CaloCell* cell, CaloCellContainer& cellContainer){
    // Store cell in output container if it is a new cell
    // Produce a copy of the cell, in order to prevent 
    // the energy of the original cell to be changed. 
    // Store unweighted cells, since the cell weights are applied during reclustering
    
    //Ask cell for it's hash
    const IdentifierHash cellHash = cell->caloDDE()->calo_hash();
    //Check if this cell is already part of reducedCellContainer
    bool isNewCell = (m_addedCellsMap.at(cellHash)==NULL);

    if(isNewCell){
        CaloCell* copyCell = cell->clone();
        cellContainer.push_back(copyCell);
        m_addedCellsMap[cellHash] = copyCell;
    }
}

#endif
