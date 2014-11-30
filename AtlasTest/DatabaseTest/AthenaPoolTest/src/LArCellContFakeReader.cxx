/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArCellContFakeReader.cxx
 *
 * @brief Test Algorithm for POOL I/O uses LArCellContainer as test
 * data
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: LArCellContFakeReader.cxx,v 1.10 2008-11-22 16:59:48 schaffer Exp $
 *
 */


//<<<<<< INCLUDES                                                       >>>>>>

#include "LArCellContFakeReader.h"

// Event includes
//#include "LArRecEvent/LArCellContainer.h"

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
LArCellContFakeReader::LArCellContFakeReader(const std::string &name, 
					     ISvcLocator *pSvcLocator) :
    Algorithm(name,pSvcLocator),
    m_storeGate(0),
    m_caloMgr(0),
    m_calocellId(0)
{}

// Initialize method:
StatusCode LArCellContFakeReader::initialize()
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "LArCellContFakeReader::initialize()" << endreq;

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
StatusCode LArCellContFakeReader::execute() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "LArCellContFakeReader::execute()" << endreq;

    // Retrieve container
    const CaloCellContainer* caloCont;
    StatusCode sc=m_storeGate->retrieve(caloCont,"CaloCellCont1");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Container '" << "CaloCellCont" 
	    << "' could not be retrieved from StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Container '" << "CaloCellCont"
	    << "' retrieved from StoreGate" << endreq;
    }

    LArCellContFakeCreator creator;

    // Create container
    const CaloCellContainer* caloCont1 = creator.createCaloCellContainer(m_calocellId,
									 m_caloMgr,
									 log);

    CaloCellContainer::const_iterator first = caloCont->begin();
    CaloCellContainer::const_iterator last  = caloCont->end();
    CaloCellContainer::const_iterator first1 = caloCont1->begin();
    CaloCellContainer::const_iterator last1  = caloCont1->end();
    
    bool error = false;
    int ncells = 0;
    int ncellErrs = 0;
    
    for(; first != last && first1 != last1; ++first, ++first1) {
	if (((*first)->ID()    !=  (*first1)->ID()) ||
	    (fabs((*first)->energy()  - (*first1)->energy()  ) > 0.01*fabs((*first)->energy())) || 
	    (fabs((*first)->time()    - (*first1)->time()    ) > 0.0002*fabs((*first)->time())) || 
	    (fabs((*first)->quality() - (*first1)->quality() ) > 0) || 
	    (fabs((*first)->provenance() - (*first1)->provenance() ) > 0) || 
	    (fabs((*first)->eta()     - (*first1)->eta()     ) > 0.0000001) || 
	    (fabs((*first)->phi()     - (*first1)->phi()     ) > 0.0000001)) {
	    log << MSG::ERROR << "CaloCell1,2 differ: id "
		<< m_calocellId->show_to_string((*first)->ID())
		<< m_calocellId->show_to_string((*first1)->ID()) << endreq
		<< " energy  " << (*first)->energy() << " "
		<< (*first1)->energy() << endreq
		<< " time    " << (*first)->time() << " "
		<< (*first1)->time() << endreq
		<< " quality " << (*first)->quality() << " "
		<< (*first1)->quality() << endreq
		<< " provenance " << (*first)->provenance() << " "
		<< (*first1)->provenance() << endreq
		<< " eta     " << (*first)->eta() << " "
		<< (*first1)->eta() << endreq
		<< " phi     " << (*first)->phi() << " "
		<< (*first1)->phi()
		<< endreq;
	    error = true;
	    ncellErrs++;
	}
	ncells++;
    }

    printCells(caloCont);

    if (error) {
	log << MSG::ERROR << "Errors: " << ncellErrs << endreq;
	return StatusCode::FAILURE;
    }
    else {
	log << MSG::INFO << "Successful check of CaloCellContainer I/O. Cells read: " << ncells << endreq;
    }
	
    return StatusCode::SUCCESS;
}

// Print cells
void LArCellContFakeReader::printCells(const CaloCellContainer* caloCont) const
{

    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "LArCellContFakeReader::printRDOs()" << endreq;

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
    
    log << MSG::DEBUG << "LArCellContFakeReader::printCells() end" << endreq;

}


// Finalize method:
StatusCode LArCellContFakeReader::finalize() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "LArCellContFakeReader::finalize()" << endreq;

    return StatusCode::SUCCESS;
}


