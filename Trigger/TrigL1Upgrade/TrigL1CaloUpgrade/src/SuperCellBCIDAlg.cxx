
/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *
 * Name :       SuperCellBCIDAlg.cxx
 * PACKAGE :    Trigger/TrigL1Upgrade/L1CaloUpgrade/SuperCellBCIDAlg
 *
 * AUTHOR :     Denis Oliveira Damazio
 *
 * PURPOSE :    prepares SuperCellContainer to have the offset BCID-dependent correctio
n
 *
 * **/

#include "SuperCellBCIDAlg.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODEventInfo/EventInfo.h"

SuperCellBCIDAlg::SuperCellBCIDAlg( const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm( name, pSvcLocator), 
    m_tool("CaloLumiBCIDTool/CaloLumiBCIDSCToolDefault")
{
  declareProperty ("SCellContainerIn", m_sCellContainerIn = "SCellContainer",
                   "SG key for the input supercell LAr channel container.");
  declareProperty ("SCellContainerOut", m_sCellContainerOut = "SCellBCID",
                   "SG key for the output supercell LAr channel container.");

}

StatusCode
SuperCellBCIDAlg::initialize()
{

	CHECK ( m_tool.retrieve() );
	return StatusCode::SUCCESS;
}

StatusCode
SuperCellBCIDAlg::execute()
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
		cell->setEnergy( sc->energy() - m_tool->average( sc->ID(), bcid ) );
		new_scell_cont->push_back( cell );
	}

	return StatusCode::SUCCESS;
}
