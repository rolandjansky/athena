/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file LArCellContFakeWriter.cxx
 *
 * @brief Test Algorithm for POOL I/O uses CaloCellContainer for LAr as test
 * data
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: LArCellContFakeWriter.cxx,v 1.9 2008-11-22 16:59:48 schaffer Exp $
 *
 */



#include "LArCellContFakeWriter.h"

// Event includes
#include "CaloEvent/CaloCellContainer.h"

// DetDescr includes
#include "CaloIdentifier/CaloCell_ID.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// test includes
#include "LArCellContFakeCreator.h"


// Constructor with parameters:
LArCellContFakeWriter::LArCellContFakeWriter(const std::string &name, 
					     ISvcLocator *pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_calocellId(nullptr)
{}

// Initialize method:
StatusCode LArCellContFakeWriter::initialize()
{
    ATH_MSG_INFO( "LArCellContFakeWriter::initialize()"  );
    ATH_CHECK( detStore()->retrieve(m_calocellId, "CaloCell_ID") );
    ATH_CHECK(m_caloMgrKey.initialize());
    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode LArCellContFakeWriter::execute() 
{
    ATH_MSG_DEBUG("LArCellContFakeWriter::execute()" );

    SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
    ATH_CHECK(caloMgrHandle.isValid());

    LArCellContFakeCreator creator;

    // Create container
    MsgStream log(msgSvc(), name());
    CaloCellContainer* caloCont = creator.createCaloCellContainer(m_calocellId,
								  *caloMgrHandle,
								  msg());
        
    if (!caloCont) {
	ATH_MSG_ERROR("LarCont has not been created !" );
	return StatusCode::FAILURE;
    } 

    // register the cell container into StoreGate
    ATH_CHECK( evtStore()->record(caloCont,"CaloCellCont1") );

    printCells(caloCont);

    return StatusCode::SUCCESS;
}

// Print cells
void LArCellContFakeWriter::printCells(const CaloCellContainer* caloCont) const
{
    ATH_MSG_INFO("LArCellContFakeWriter::printRDOs()" );

    // loop on all cells

    CaloCellContainer::const_iterator first = caloCont->begin();
    CaloCellContainer::const_iterator last  = caloCont->end();
    
    for(; first != last; ++first) {
	
	ATH_MSG_DEBUG("CaloCell: id "
	    << m_calocellId->show_to_string((*first)->ID())
	    << " energy  " << (*first)->energy()
	    << " time    " << (*first)->time()
	    << " quality " << (*first)->quality()
	    << " provenance " << (*first)->provenance()
	    << std::setprecision(5)
	    << " eta     " << (*first)->eta()
	    << " phi     " << (*first)->phi()
	    );
    }
    
    ATH_MSG_DEBUG("LArCellContFakeWriter::printCells() end" );
}


// Finalize method:
StatusCode LArCellContFakeWriter::finalize() 
{
    ATH_MSG_INFO("LArCellContFakeWriter::finalize()" );

    return StatusCode::SUCCESS;
}


