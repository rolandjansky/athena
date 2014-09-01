/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file LArCellContFakeWriter.cxx
 *
 * @brief Test Algorithm for POOL I/O uses LArCellContainer as test
 * data
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: LArCellContFakeWriter.cxx,v 1.9 2008-11-22 16:59:48 schaffer Exp $
 *
 */


//<<<<<< INCLUDES                                                       >>>>>>

#include "LArCellContFakeWriter.h"

// Event includes
#include "CaloEvent/CaloCellContainer.h"

// DetDescr includes
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

// TES include
#include "StoreGate/StoreGateSvc.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"

// test includes
#include "LArCellContFakeCreator.h"


// Constructor with parameters:
LArCellContFakeWriter::LArCellContFakeWriter(const std::string &name, 
					     ISvcLocator *pSvcLocator) :
    Algorithm(name,pSvcLocator),
    m_storeGate(0),
    m_caloMgr(0),
    m_calocellId(0)
{}

// Initialize method:
StatusCode LArCellContFakeWriter::initialize()
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "LArCellContFakeWriter::initialize()" << endreq;

    // get StoreGate service
    StatusCode sc=service("StoreGateSvc",m_storeGate);
    if (sc.isFailure()) {
	log << MSG::ERROR << "StoreGate service not found !" << endreq;
	return StatusCode::FAILURE;
    } else {}

    // get DetectorStore service
    StoreGateSvc * detStore;
    sc=service("DetectorStore",detStore);
    if (sc.isFailure()) {
	log << MSG::ERROR << "DetectorStore service not found !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << " Found DetectorStore " << endreq;
    }
    
    // Get the lar em helper from the detector store
    sc = detStore->retrieve(m_calocellId, "CaloCell_ID");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Could not get CaloCell_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the CaloCell_ID helper. " << endreq;
    }

    // Get the lar dd mgr
    sc = detStore->retrieve(m_caloMgr);
    if (sc.isFailure()) {
	log << MSG::ERROR << "Unable to retrieve CaloDetDescrManager from DetectorStore" << endreq;
	return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode LArCellContFakeWriter::execute() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "LArCellContFakeWriter::execute()" << endreq;

    LArCellContFakeCreator creator;

    // Create container
    CaloCellContainer* caloCont = creator.createCaloCellContainer(m_calocellId,
								  m_caloMgr,
								  log);
        
    if (!caloCont) {
	log << MSG::ERROR << "LarCont has not been created !" << endreq;
	return StatusCode::FAILURE;
    } 

    // register the cell container into StoreGate
    StatusCode sc = m_storeGate->record(caloCont,"CaloCellCont1");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Container '" << "CaloCellCont" 
	    << "' could not be registered in StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Container '" << "CaloCellCont"
	    << "' registered in StoreGate" << endreq;
    }

    // Save symlink
//     CaloCellContainer* caloColl=0;
//     sc = m_storeGate->symLink(larCont,caloColl);
//     if (sc != StatusCode::SUCCESS) {
// 	log << MSG::ERROR 
// 	    << "Error making symLink for CaloCellContainer in  TDS "
// 	    << endreq;
// 	return sc;
//     } 
//     else {
// 	log << MSG::DEBUG << "Registered symlink to CaloCellContainer" << endreq;
//     }


    printCells(caloCont);

    return StatusCode::SUCCESS;
}

// Print cells
void LArCellContFakeWriter::printCells(const CaloCellContainer* caloCont) const
{

    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "LArCellContFakeWriter::printRDOs()" << endreq;

    // loop on all cells

    CaloCellContainer::const_iterator first = caloCont->begin();
    CaloCellContainer::const_iterator last  = caloCont->end();
    
    for(; first != last; ++first) {
	
	log << MSG::DEBUG << "CaloCell: id "
	    << m_calocellId->show_to_string((*first)->ID())
	    << " energy  " << (*first)->energy()
	    << " time    " << (*first)->time()
	    << " quality " << (*first)->quality()
	    << " provenance " << (*first)->provenance()
	    << std::setprecision(5)
	    << " eta     " << (*first)->eta()
	    << " phi     " << (*first)->phi()
	    << endreq;
    }
    
    log << MSG::DEBUG << "LArCellContFakeWriter::printCells() end" << endreq;

}


// Finalize method:
StatusCode LArCellContFakeWriter::finalize() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "LArCellContFakeWriter::finalize()" << endreq;

    return StatusCode::SUCCESS;
}


