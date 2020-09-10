/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
//-----------------------------------------------------------------------------
// file:        TauPi0CreateROI.cxx
// package:     Reconstruction/tauEvent
// authors:     Will Davey, Benedict Winter, Stephanie Yuen
// date:        2012-10-09
//-----------------------------------------------------------------------------

#include "CaloUtils/CaloCellList.h"
#include "TauPi0CreateROI.h"

#include <boost/scoped_ptr.hpp>

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

TauPi0CreateROI::TauPi0CreateROI(const std::string& name) :
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

//______________________________________________________________________________
StatusCode TauPi0CreateROI::executePi0CreateROI(xAOD::TauJet& pTau, CaloCellContainer& pPi0CellContainer, std::vector<CaloCell*>& addedCellsMap) {

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
    std::vector<CaloCell_ID::SUBCALO> emSubCaloBlocks;
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
        const IdentifierHash cellHash = cell->caloDDE()->calo_hash();
        bool isNewCell = (addedCellsMap.at(cellHash)==NULL);

        if(isNewCell){
            CaloCell* copyCell = cell->clone();
            pPi0CellContainer.push_back(copyCell);
            addedCellsMap[cellHash] = copyCell;
        }
    }

    return StatusCode::SUCCESS;
}

#endif
