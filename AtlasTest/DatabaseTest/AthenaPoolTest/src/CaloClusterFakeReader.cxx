/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloClusterFakeWriter.cxx
 *
 * @brief Test Algorithm for POOL I/O tests, reads CaloCluster objects
 * from the transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: CaloClusterFakeReader.cxx,v 1.8 2008-08-24 11:02:49 schaffer Exp $
 *
 */

#include "CaloClusterFakeReader.h"

// Event includes
#include "CaloEvent/CaloClusterContainer.h"

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
CaloClusterFakeReader::CaloClusterFakeReader(const std::string &name, 
					     ISvcLocator *pSvcLocator) :
    Algorithm(name,pSvcLocator),
    m_storeGate(0),
    m_caloMgr(0),
    m_calocellId(0)
{}

CaloClusterFakeReader::~CaloClusterFakeReader()
{}

// Initialize method:
StatusCode CaloClusterFakeReader::initialize()
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "CaloClusterFakeReader::initialize()" << endreq;

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
	log << MSG::ERROR << "Could not get LArEM_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the LArEM_ID helper. " << endreq;
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
StatusCode CaloClusterFakeReader::execute() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "CaloClusterFakeReader::execute()" << endreq;

    // Retrieve container
    const CaloClusterContainer* clusCont;
    StatusCode sc=m_storeGate->retrieve(clusCont,"LArClusterCont");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Container '" << "LArClusterCont" 
	    << "' could not be retrieved from StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Container '" << "LArClusterCont"
	    << "' retrieved from StoreGate" << endreq;
    }

    CaloClusterContainer::const_iterator first = clusCont->begin();
    CaloClusterContainer::const_iterator last  = clusCont->end();

    LArCellContFakeCreator creator;

    // Create container
    const CaloCellContainer* caloCont1 = creator.createCaloCellContainer(m_calocellId,
									 m_caloMgr,
									 log);

    CaloCellContainer::const_iterator first1 = caloCont1->begin();
    CaloCellContainer::const_iterator last1  = caloCont1->end();

    bool error = false;
    int ncells = 0;
    int ncellErrs = 0;

    for(; first != last; ++first) {
	CaloCluster::cell_iterator first2 = (*first)->cell_begin();
	CaloCluster::cell_iterator last2  = (*first)->cell_end();
    
	for(; first2 != last2 && first1 != last1; ++first2, ++first1) {
	    if (((*first2)->ID()    !=  (*first1)->ID()) ||
		(fabs((*first2)->energy()  - (*first1)->energy()  ) > 0.01*fabs((*first1)->energy())) || 
		(fabs((*first2)->time()    - (*first1)->time()    ) > 0.0002*fabs((*first1)->time())) || 
		(fabs((*first2)->quality() - (*first1)->quality() ) > 0.0000001) || 
		(fabs((*first2)->eta()     - (*first1)->eta()     ) > 0.0000001) || 
		(fabs((*first2)->phi()     - (*first1)->phi()     ) > 0.0000001)) {
		log << MSG::ERROR << "LArCell1,2 differ: id "
		<< m_calocellId->show_to_string((*first2)->ID())
		<< m_calocellId->show_to_string((*first1)->ID()) << endreq
		<< " energy  " << (*first2)->energy() << " "
		<< (*first1)->energy() << endreq
		<< " time    " << (*first2)->time() << " "
		<< (*first1)->time() << endreq
		<< " quality " << (*first2)->quality() << " "
		<< (*first1)->quality() << endreq
		<< " eta     " << (*first2)->eta() << " "
		<< (*first1)->eta() << endreq
		<< " phi     " << (*first2)->phi() << " "
		<< (*first1)->phi()
		<< endreq;
		error = true;
		ncellErrs++;
	    }
	    ncells++;
	}
    }

    printCells(clusCont);

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
void CaloClusterFakeReader::printCells(const CaloClusterContainer* clusCont) const
{

    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "CaloClusterFakeReader::printCells from cluster" << endreq;

    // loop on all cells

    CaloClusterContainer::const_iterator first = clusCont->begin();
    CaloClusterContainer::const_iterator last  = clusCont->end();
    for(; first != last; ++first) {

	const CaloCluster* clus = *first;

	log << MSG::DEBUG << "CaloCluster: e, eta, phi EMB1 "
	    << " " << (*first)->eSample  (CaloSampling::EMB1)
	    << " " << (*first)->etaSample(CaloSampling::EMB1)
	    << " " << (*first)->phiSample(CaloSampling::EMB1)
	    << endreq;
	log << MSG::DEBUG << "CaloCluster: e, eta, phi EMB2 "
	    << " " << (*first)->eSample  (CaloSampling::EMB2)
	    << " " << (*first)->etaSample(CaloSampling::EMB2)
	    << " " << (*first)->phiSample(CaloSampling::EMB2)
	    << endreq;
	log << MSG::DEBUG << "CaloCluster: e, eta, phi EMB3 "
	    << " " << (*first)->eSample  (CaloSampling::EMB3)
	    << " " << (*first)->etaSample(CaloSampling::EMB3)
	    << " " << (*first)->phiSample(CaloSampling::EMB3)
	    << endreq;


	CaloCluster::cell_iterator first1 = clus->cell_begin();
	CaloCluster::cell_iterator last1  = clus->cell_end();
    
	for(; first1 != last1; ++first1) {
	
	    log << MSG::DEBUG << "  CaloCell: id "
		<< m_calocellId->show_to_string((*first1)->ID())
		<< " energy  " << (*first1)->energy()
		<< " time    " << (*first1)->time()
		<< " quality " << (*first1)->quality()
		<< std::setprecision(5)
		<< " eta     " << (*first1)->eta()
		<< " phi     " << (*first1)->phi()
		<< endreq;
	}
    }
    
    log << MSG::DEBUG << "CaloClusterFakeReader::printCells from cluster end" << endreq;

}


// Finalize method:
StatusCode CaloClusterFakeReader::finalize() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "CaloClusterFakeReader::finalize()" << endreq;

    return StatusCode::SUCCESS;
}


