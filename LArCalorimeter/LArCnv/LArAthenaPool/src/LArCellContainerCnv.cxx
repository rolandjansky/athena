/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellContainerCnv.h"

// CaloDetDescr includes
#include "CaloDetDescr/CaloDetDescrManager.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/StorableConversions.h"


LArCellContainerCnv::LArCellContainerCnv(ISvcLocator* svcloc)
    :
    // Base class constructor
    LArCellContainerCnvBase::T_AthenaPoolCnv(svcloc), 
    m_detMgr(0)
{}



LArCellContainerCnv::~LArCellContainerCnv()
{}


StatusCode LArCellContainerCnv::initialize()
{
    // Call base clase initialize
    AthenaPoolConverter::initialize();

    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), "LArCellContainerCnv");
    log << MSG::INFO << "initialize()" << endreq;

    // get DetectorStore service
    StoreGateSvc *detStore;
    StatusCode sc=service("DetectorStore",detStore);
    if (sc.isFailure()) {
	log << MSG::FATAL << "DetectorStore service not found !" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << " Found DetectorStore " << endreq;
    }
    
    // Get the pixel helper from the detector store
    sc = detStore->retrieve(m_detMgr);
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get CaloDetDescrManager !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the CaloDetDescrManager " << endreq;
    }

//      m_cellId(0)
//      // Get the calocell id helper from the detector store
//      sc = detStore->retrieve(m_cellId);
//      if (sc.isFailure()) {
//  	log << MSG::FATAL << "Could not get CaloCell_ID helper !" << endreq;
//  	return StatusCode::FAILURE;
//      } 
//      else {
//  	log << MSG::DEBUG << " Found the CaloCell_ID helper. " << endreq;
//      }

    log << MSG::DEBUG << "Converter initialized" << endreq;

    return StatusCode::SUCCESS;
}


StatusCode LArCellContainerCnv::PoolToDataObject(DataObject*& pObj, const Token* token)
{
    // First call base class converter to get DataObject from
    // pool. Then modify as appropriate

    MsgStream log(msgSvc(), "LArCellContainerCnv::PoolToDataObject" );
   
    StatusCode sc = LArCellContainerCnvBase::PoolToDataObject(pObj, token);
    if (sc.isFailure()) {
	log << MSG::FATAL << "Unable to get object from pool" << endreq;
	return StatusCode::FAILURE;
    } else {
	log << MSG::DEBUG << " Found DataObject " << endreq;
    }
    
    // Convert DataObject pointer to LArCellContainer*
    LArCellContainer* larCont = 0;
    SG::fromStorable(pObj, larCont );
    if(!larCont) {
	log << MSG::ERROR << "  failed to cast to LArCellContainer " << endreq ; 
	return StatusCode::FAILURE; 
    }

    // Initialize LArCells with CaloDDE
    for (LArCell* cell : *larCont) {
      cell->setCaloDDE(m_detMgr->get_element (cell->ID()));
    }
    
    return StatusCode::SUCCESS; 
}


