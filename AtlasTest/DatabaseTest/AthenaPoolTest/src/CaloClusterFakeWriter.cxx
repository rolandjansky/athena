/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloClusterFakeWriter.cxx
 *
 * @brief Test Algorithm for POOL I/O tests, writes CaloCluster
 * objects to the transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: CaloClusterFakeWriter.cxx,v 1.12 2008-08-27 10:08:03 schaffer Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "CaloClusterFakeWriter.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

// DetDescr includes
#include "CaloIdentifier/LArEM_ID.h"

// TES include
#include "StoreGate/StoreGateSvc.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"

// test includes
//#include "LArCellContFakeCreator.h"


CaloClusterFakeWriter::CaloClusterFakeWriter(const std::string &name, 
					     ISvcLocator *pSvcLocator) :
    Algorithm(name,pSvcLocator),
    m_storeGate(0),
    m_laremId(0)
{}

CaloClusterFakeWriter::~CaloClusterFakeWriter()
{}

// Initialize method:
StatusCode CaloClusterFakeWriter::initialize()
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "CaloClusterFakeWriter::initialize()" << endreq;

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
    sc = detStore->retrieve(m_laremId, "LArEM_ID");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Could not get LArEM_ID helper !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the LArEM_ID helper. " << endreq;
    }

    // Get the lar dd mgr
//     sc = detStore->retrieve(m_larMgr);
//     if (sc.isFailure()) {
// 	log << MSG::ERROR << "Unable to retrieve LArDetDescrManager from DetectorStore" << endreq;
// 	return StatusCode::FAILURE;
//     }

    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode CaloClusterFakeWriter::execute() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "CaloClusterFakeWriter::execute()" << endreq;

    // retrive the cell container
    const CaloCellContainer* caloCont;
    StatusCode sc = m_storeGate->retrieve(caloCont,"CaloCellCont1");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Container '" << "CaloCellCont" 
	    << "' could not be retrieved from StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << "Container '" << "CaloCellCont"
	    << "' retrieved from StoreGate" << endreq;
    }

    // print out cells
    printCells(caloCont);

    // create clusters
    CaloClusterContainer* lcc = new CaloClusterContainer();

    MsgStream log_dummy(msgSvc(), name()+"_dummy");
    sc = CaloClusterStoreHelper::recordClusters(m_storeGate, lcc, "LArClusterCont", log_dummy);
    if (sc!=StatusCode::SUCCESS){ 
	log << MSG::ERROR << "Container '" << "LArClusterCont" 
	    << "' could not be recorded in StoreGate !" << endreq;
	return StatusCode::FAILURE;
    } 
    else { 
	log << MSG::DEBUG << "Container '" << "LArClusterCont"
	    << "' recorded in StoreGate" << endreq;
    }  

    CaloCellContainer::const_iterator first = caloCont->begin();
    CaloCellContainer::const_iterator last  = caloCont->end();
    int i = 0;
    CaloCluster* clus = 0;
    
    for(; first != last; ++first, ++i) {
	
	log << MSG::DEBUG << "CaloCell: id "
	    << m_laremId->show_to_string((*first)->ID())
	    << " energy  " << (*first)->energy()
	    << " time    " << (*first)->time()
	    << " quality " << (*first)->quality()
	    << std::setprecision(5)
	    << " eta     " << (*first)->eta()
	    << " phi     " << (*first)->phi()
	    << endreq;

	// Put two cells into a cluster
	if (i%2 == 0) {
	    if (clus) lcc->push_back(clus);  // Add previous one
	    clus = CaloClusterStoreHelper::makeCluster();

	}
	clus->addCell(caloCont, *first, 1.0);
	
    }
    if (clus) lcc->push_back(clus);  // Add last one
    clus = 0;

    // Create a dummy log to avoid debug printout for this alg
    sc = CaloClusterStoreHelper::finalizeClusters(m_storeGate, lcc, "LArClusterCont", log_dummy);
    if (sc!=StatusCode::SUCCESS){ 
	log << MSG::ERROR << "Container '" << "LArClusterCont" 
	    << "' could not finalize clusters !" << endreq;
	return StatusCode::FAILURE;
    } 
    else { 
	log << MSG::DEBUG << "Container '" << "LArClusterCont"
	    << "' called finalize on cluster helper" << endreq;
    }  

    // print out cells from cluster container
    printCells(lcc);


    return StatusCode::SUCCESS;
}

// Print cells
void CaloClusterFakeWriter::printCells(const CaloCellContainer* caloCont) const
{

    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "CaloClusterFakeWriter::printCells from Calo cont" << endreq;

    // loop on all cells

    CaloCellContainer::const_iterator first = caloCont->begin();
    CaloCellContainer::const_iterator last  = caloCont->end();
    
    for(; first != last; ++first) {
	
	log << MSG::DEBUG << "CaloCell: id "
	    << m_laremId->show_to_string((*first)->ID())
	    << " energy  " << (*first)->energy()
	    << " time    " << (*first)->time()
	    << " quality " << (*first)->quality()
	    << std::setprecision(5)
	    << " eta     " << (*first)->eta()
	    << " phi     " << (*first)->phi()
	    << endreq;
    }
    
    log << MSG::DEBUG << "CaloClusterFakeWriter::printCells from Calo cont end" << endreq;

}


// Print cells
void CaloClusterFakeWriter::printCells(const CaloClusterContainer* clusCont) const
{

    MsgStream log(msgSvc(), name());
    // limit precision for output comparison
    log << MSG::INFO << "CaloClusterFakeWriter::printCells from cluster" << endreq;

    // loop on all cells

    CaloClusterContainer::const_iterator first = clusCont->begin();
    CaloClusterContainer::const_iterator last  = clusCont->end();
    for(; first != last; ++first) {

	const CaloCluster* clus = *first;

	log << MSG::DEBUG << "CaloCluster: e, eta, phi EMB1 "
	    << " " << (*first)->eSample  (CaloSampling::EMB1)
            << std::setprecision(4)
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
		<< m_laremId->show_to_string((*first1)->ID())
		<< " energy  " << (*first1)->energy()
		<< " time    " << (*first1)->time()
		<< " quality " << (*first1)->quality()
                << std::setprecision(5)
		<< " eta     " << (*first1)->eta()
		<< " phi     " << (*first1)->phi()
		<< endreq;
	}
    }
    
    log << MSG::DEBUG << "CaloClusterFakeWriter::printCells from cluster end" << endreq;

}


// Finalize method:
StatusCode CaloClusterFakeWriter::finalize() 
{
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "CaloClusterFakeWriter::finalize()" << endreq;

    return StatusCode::SUCCESS;
}



