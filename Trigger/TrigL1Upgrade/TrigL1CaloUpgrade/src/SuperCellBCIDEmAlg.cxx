
/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *
 * Name :       SuperCellBCIDEmAlg.cxx
 * PACKAGE :    Trigger/TrigL1Upgrade/L1CaloUpgrade/SuperCellBCIDEmAlg
 *
 * AUTHOR :     Denis Oliveira Damazio
 *
 * PURPOSE :    prepares SuperCellContainer to have the offset BCID-dependent correctio
n
 *
 * **/

#include "SuperCellBCIDEmAlg.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "xAODEventInfo/EventInfo.h"

SuperCellBCIDEmAlg::SuperCellBCIDEmAlg( const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm( name, pSvcLocator), 
    m_tool("CaloLumiBCIDTool/CaloLumiBCIDToolDefault"),
    m_scidtool("CaloSuperCellIDTool")
{
  declareProperty ("SCellContainerIn", m_sCellContainerIn = "SCellContainer",
                   "SG key for the input supercell LAr channel container.");
  declareProperty ("SCellContainerOut", m_sCellContainerOut = "SCellBCID",
                   "SG key for the output supercell LAr channel container.");

}

StatusCode
SuperCellBCIDEmAlg::initialize()
{

	CHECK ( m_tool.retrieve() );
	CHECK ( m_scidtool.retrieve() );
	return StatusCode::SUCCESS;
}

StatusCode
SuperCellBCIDEmAlg::execute()
{

	const CaloCellContainer* scells_from_sg;
	if ( evtStore()->retrieve(scells_from_sg,m_sCellContainerIn).isFailure() ){
                ATH_MSG_WARNING ("did not find cell container");
                return StatusCode::SUCCESS;
        }
	const xAOD::EventInfo* evt;
	if ( evtStore()->retrieve(evt,"EventInfo").isFailure() ){
                ATH_MSG_WARNING ("did not find EventInfo object");
                return StatusCode::SUCCESS;
        }
	int bcid = evt->bcid();
	
	CaloCellContainer* new_scell_cont = new CaloCellContainer(SG::OWN_ELEMENTS);
	ATH_CHECK( evtStore()->record(new_scell_cont, m_sCellContainerOut) );
	new_scell_cont->reserve( scells_from_sg->size() );
	for(auto sc : *scells_from_sg){
		if ( !sc ) continue;
		CaloCell* cell = sc->clone();
		// calculate the BCID for all the cells associated to the SC
		// since this is emulation
		float correction(0.0);
		std::vector<Identifier> ids = m_scidtool->superCellToOfflineID( sc->ID() );
		for ( auto id : ids ) correction+=m_tool->average( id, bcid ) ;
		cell->setEnergy( sc->energy() - correction );
		new_scell_cont->push_back( cell );
	}

	return StatusCode::SUCCESS;
}
