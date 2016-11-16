/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "CaloInterface/ICaloCellMakerTool.h"

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
    , m_cellMakerTool("")
    , m_calo_dd_man(NULL)
    , m_calo_id(NULL)
    , m_caloCellContainerName("AllCalo")
    , m_pPi0CellContainer(NULL)
    , m_pi0CellContainerName("TauCommonPi0Cells")
{
    declareProperty("CellMakerTool", m_cellMakerTool);
    declareProperty("CaloCellContainerName",   m_caloCellContainerName);
    declareProperty("Pi0CellContainerName",    m_pi0CellContainerName);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

TauPi0CreateROI::~TauPi0CreateROI() {
}

StatusCode TauPi0CreateROI::initialize() {
    
    // retrieve tools
    ATH_MSG_DEBUG( "Retrieving tools" );
    CHECK( m_cellMakerTool.retrieve() );
    
    // initialize calo cell geo
    m_calo_dd_man  = CaloDetDescrManager::instance();
    m_calo_id      = m_calo_dd_man->getCaloCell_ID();

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

    //---------------------------------------------------------------------
    // Create CustomCellContainer and register in StoreGate
    //---------------------------------------------------------------------
    m_pPi0CellContainer = new CaloCellContainer(SG::OWN_ELEMENTS);
    CHECK( evtStore()->record(m_pPi0CellContainer, m_pi0CellContainerName) );

    // symlink as INavigable4MomentumCollection (as in CaloRec/CaloCellMaker)
    CHECK(evtStore()->symLink(m_pPi0CellContainer, static_cast<INavigable4MomentumCollection*> (0)));

    return StatusCode::SUCCESS;
}

StatusCode TauPi0CreateROI::execute(xAOD::TauJet& pTau) {

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
    const CaloCellContainer *pCellContainer = NULL;
    CHECK( evtStore()->retrieve(pCellContainer, m_caloCellContainerName) );
    
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
        storeCell(cell);
    }

    return StatusCode::SUCCESS;
}

StatusCode TauPi0CreateROI::eventFinalize() {

    //---------------------------------------------------------------------
    // use the m_cellMakerTool to finalize the custom CaloCellContainer
    //---------------------------------------------------------------------
    CHECK( m_cellMakerTool->process(static_cast<CaloCellContainer*> (m_pPi0CellContainer)) );

    return StatusCode::SUCCESS;
}

StatusCode TauPi0CreateROI::finalize() {
    return StatusCode::SUCCESS;
}

void TauPi0CreateROI::storeCell(const CaloCell* cell){
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
        m_pPi0CellContainer->push_back(copyCell);
        m_addedCellsMap[cellHash] = copyCell;
    }
}

#endif
