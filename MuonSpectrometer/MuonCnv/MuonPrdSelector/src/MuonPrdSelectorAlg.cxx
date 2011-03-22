/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonPrdSelectorAlg.cxx
//   Source file for class MuonPrdSelectorAlg
///////////////////////////////////////////////////////////////////

#include "MuonPrdSelector/MuonPrdSelectorAlg.h"

// Gaudi includes
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include <iomanip>

// Constructor with parameters:
MuonPrdSelectorAlg::MuonPrdSelectorAlg(const std::string &name, ISvcLocator *pSvcLocator) 
  : Algorithm(name,pSvcLocator),

    m_StoreGate(0), m_detMgr(0),
    m_mdtIdHelper(0), m_cscIdHelper(0), m_rpcIdHelper(0), m_tgcIdHelper(0), 
    m_muonIdCutTool("MuonIdCutTool/MuonIdCutTool"),
    m_mdtPRDs_in(0), m_rpcPRDs_in(0), m_tgcPRDs_in(0), m_cscPRDs_in(0),
    m_mdtPRDs_out(0), m_rpcPRDs_out(0), m_tgcPRDs_out(0), m_cscPRDs_out(0)



{
  declareProperty("MDT_PRDinputContainer"      , m_inputContainer_mdt  = "MDT_DriftCircles_unfiltered"  );
  declareProperty("MDT_PRDoutputContainer"     , m_outputContainer_mdt = "MDT_DriftCircles"             );

  declareProperty("RPC_PRDinputContainer"      , m_inputContainer_rpc  = "RPC_Measurements_unfiltered"  );
  declareProperty("RPC_PRDoutputContainer"     , m_outputContainer_rpc = "RPC_Measurements"             );

  declareProperty("TGC_PRDinputContainer"      , m_inputContainer_tgc  = "TGC_Measurements_unfiltered"  );
  declareProperty("TGC_PRDoutputContainer"     , m_outputContainer_tgc = "TGC_Measurements"             );

  declareProperty("CSC_PRDinputContainer"      , m_inputContainer_csc  = "CSC_Measurements_unfiltered"      );
  declareProperty("CSC_PRDoutputContainer"     , m_outputContainer_csc = "CSC_Measurements"                 );
  








}


// Initialize method:
StatusCode MuonPrdSelectorAlg::initialize()
{
  StatusCode sc;
  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "initialize() called" << endreq;

  IToolSvc* toolsvc;
  sc=service("ToolSvc",toolsvc);
  if( sc.isFailure() ){
    log<< MSG::WARNING <<"Could not get tool sevices "<<endreq; 
    return( StatusCode::FAILURE );
  }

  // Retrieve StoreGate
  sc=service("StoreGateSvc",m_StoreGate);
  if (sc.isFailure()) {
    log << MSG::WARNING << "StoreGate service not found !" << endreq;
    return StatusCode::FAILURE;
  } 

  // Retrieve DetectorStore
  StoreGateSvc* detStore = 0;
  sc = service( "DetectorStore", detStore );
  if (sc.isFailure()) {
    log <<MSG::FATAL << "Could not get DetectorStore"<<endreq;
    return sc;
  }
  
  // retrieve MuonDetectorManager
  std::string managerName="Muon";
  sc=detStore->retrieve(m_detMgr);
  if (sc.isFailure()) {
    log << MSG::INFO << "Could not find the MuonReadoutGeometry Manager: "
	<< managerName << " ! " << endreq;
  } 

  // initialize MuonIdHelpers
  if (m_detMgr) {
    m_mdtIdHelper = m_detMgr->mdtIdHelper();
    m_cscIdHelper = m_detMgr->cscIdHelper();
    m_rpcIdHelper = m_detMgr->rpcIdHelper();
    m_tgcIdHelper = m_detMgr->tgcIdHelper();
  } 

  // retrieve test tool

  sc = m_muonIdCutTool.retrieve();
  if(sc.isFailure())
    {
      log<<MSG::FATAL<<"Could not retrieve Test tool"<<endreq;
      return StatusCode::FAILURE;
    }
  else
    {
      log<<MSG::DEBUG<<"Retrieved Test Tool"
	     << m_muonIdCutTool <<" ! "<<endreq;
    }


  //Creating the output PrepDataContainers


  
  try {     
    m_mdtPRDs_out = new Muon::MdtPrepDataContainer(m_mdtIdHelper->module_hash_max());
  } catch(std::bad_alloc) {
    log << MSG::FATAL
	<< "Could not create a new MDT PrepRawData container!"<< endreq;
    return StatusCode::FAILURE;
  }
  m_mdtPRDs_out->addRef();

 
    
    try {
      m_rpcPRDs_out = new Muon::RpcPrepDataContainer(m_rpcIdHelper->module_hash_max());
    } catch(std::bad_alloc) {
      log << MSG::FATAL
	  << "Could not create a new RPC PrepRawData container!"<< endreq;
      return StatusCode::FAILURE;
    }
    m_rpcPRDs_out->addRef();

    try {
      m_tgcPRDs_out = new Muon::TgcPrepDataContainer(m_tgcIdHelper->module_hash_max());
    } catch(std::bad_alloc) {
      log << MSG::FATAL
	  << "Could not create a new TGC PrepRawData container!"<< endreq;
      return StatusCode::FAILURE;
    }
    m_tgcPRDs_out->addRef();
  
    
    try {
      m_cscPRDs_out = new Muon::CscStripPrepDataContainer(m_cscIdHelper->module_hash_max());
    } catch(std::bad_alloc) {
      log << MSG::FATAL
    	  << "Could not create a new CSC PrepRawData container!"<< endreq;
      return StatusCode::FAILURE;
    }
    m_cscPRDs_out->addRef();
    
    
    return sc;
}

StatusCode MuonPrdSelectorAlg::execute()
{ 
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "execute() called" << endreq;

  
  StatusCode sc = retrieveContainers() ; 
  if( sc.isFailure() ) {
    log << MSG::DEBUG << "Failed to retrieve containers " << endreq;
    return StatusCode::SUCCESS ;
  }
  
  sc = recordContainers();
  if( sc.isFailure() ) log << MSG::DEBUG << "Failed to record containers " << endreq;
  
  sc = performSelection();
  if( sc.isFailure() ) log << MSG::DEBUG << "unable to perform selection" << endreq;
  
  return StatusCode::SUCCESS;
}

StatusCode MuonPrdSelectorAlg::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "finalize() called" << endreq;
  
  return StatusCode::SUCCESS;
}


StatusCode MuonPrdSelectorAlg::retrieveContainers() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "retrieveContainers() called" << endreq;
 
  
  //resetting the inputcollections.
  m_mdtPRDs_in = 0;
  m_rpcPRDs_in = 0;
  m_tgcPRDs_in = 0;
  m_cscPRDs_in = 0;

  StatusCode sc = m_StoreGate->retrieve( m_mdtPRDs_in, m_inputContainer_mdt );
  if( sc.isFailure() ) {
    log << MSG::WARNING << "Could not retrieve MDT PRDs input container " 
	<< m_inputContainer_mdt << endreq;
  } else {
    log << MSG::DEBUG << "Retrieved MDT PRDs input container " << m_inputContainer_mdt << " ( " 
	<< m_mdtPRDs_in->size() << " ) " << endreq;
  }

  sc = m_StoreGate->retrieve( m_rpcPRDs_in, m_inputContainer_rpc );
  if( sc.isFailure() ) {
    log << MSG::WARNING << "Could not retrieve RPC PRDs input container " 
	<< m_inputContainer_rpc << endreq;
  } else {
    log << MSG::DEBUG << "Retrieved RPC PRDs input container " << m_inputContainer_rpc << " ( " 
	<< m_rpcPRDs_in->size() << " ) " << endreq;
  }

  sc = m_StoreGate->retrieve( m_tgcPRDs_in, m_inputContainer_tgc );
  if( sc.isFailure() ) {
    log << MSG::WARNING << "Could not retrieve TGC PRDs input container " 
	<< m_inputContainer_tgc << endreq;
  } else {
    log << MSG::DEBUG << "Retrieved TGC PRDs input container " << m_inputContainer_tgc << " ( " 
	<< m_tgcPRDs_in->size() << " ) " << endreq;
  }

  sc = m_StoreGate->retrieve( m_cscPRDs_in, m_inputContainer_csc );
  if( sc.isFailure() ) {
    log << MSG::WARNING << "Could not retrieve CSC PRDs input container " 
  	<< m_inputContainer_csc << endreq;
  } else {
    log << MSG::DEBUG << "Retrieved CSC PRDs input container " << m_inputContainer_csc << " ( " 
  	<< m_cscPRDs_in->size() << " ) " << endreq;
  }

  return StatusCode::SUCCESS;
}


StatusCode MuonPrdSelectorAlg::recordContainers() {
  MsgStream log(msgSvc(), name());
  StatusCode sc = m_StoreGate->record( m_mdtPRDs_out, m_outputContainer_mdt );
  if( sc.isFailure() ) log << MSG::DEBUG << "Could not record MDT PRDs to output container " << m_outputContainer_mdt << endreq;

  sc = m_StoreGate->record( m_rpcPRDs_out, m_outputContainer_rpc );
  if( sc.isFailure() ) log << MSG::DEBUG << "Could not record RPC PRDs to output container " << m_outputContainer_rpc << endreq;

  sc = m_StoreGate->record( m_tgcPRDs_out, m_outputContainer_tgc );
  if( sc.isFailure() ) log << MSG::DEBUG << "Could not record TGC PRDs to output container " << m_outputContainer_tgc << endreq;

  sc = m_StoreGate->record( m_cscPRDs_out, m_outputContainer_csc );
  if( sc.isFailure() ) log << MSG::DEBUG << "Could not record CSC PRDs to output container " << m_outputContainer_csc << endreq;
  return sc;
}

StatusCode MuonPrdSelectorAlg::performSelection() {
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS ;

  if( m_mdtPRDs_in ) sc = selectMDTs() ;
  if( m_rpcPRDs_in ) sc = selectRPCs() ;
  if( m_tgcPRDs_in ) sc = selectTGCs() ;
  if( m_cscPRDs_in ) sc = selectCSCs() ;
  
  return sc ;
}


StatusCode MuonPrdSelectorAlg::selectMDTs() {
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS ;
  
  // Clearing the outputContainers
  m_mdtPRDs_out->cleanup();
  
  
  Muon::MdtPrepDataContainer::const_iterator mdtColl = m_mdtPRDs_in->begin();
  Muon::MdtPrepDataContainer::const_iterator last_mdtColl = m_mdtPRDs_in->end();

  unsigned int i_mdtColl(0);
  
  
  for( ; mdtColl!=last_mdtColl ; ++mdtColl, ++i_mdtColl ){ //for each collection

    Identifier ID = (*mdtColl)->identify() ;
    IdentifierHash hashID = (*mdtColl)->identifyHash();
    Muon::MdtPrepDataCollection * driftCircleColl = new Muon::MdtPrepDataCollection(hashID);
    (driftCircleColl)->setIdentifier(ID);
   
    Muon::MdtPrepDataCollection::const_iterator mdtItr = (*mdtColl)->begin();
    Muon::MdtPrepDataCollection::const_iterator mdtEnd = (*mdtColl)->end();
    int i = 0;
    
    for(; mdtItr != mdtEnd; ++mdtItr,i++ ) { //for each hit in the collection
      Identifier hitID =(*mdtItr)->identify();

      if (!m_muonIdCutTool->isCut(hitID)){ //write event to new collection

	driftCircleColl->push_back(new Muon::MdtPrepData(**mdtItr));
	log << MSG::DEBUG << "Writing Event." << endreq;
	      }
      else
	log << MSG::DEBUG << "Rejecting Event." << endreq;

    }	      
	  

    Identifier newID = (driftCircleColl)->identify();
    log << MSG::DEBUG << "Adding New Collection to MDT list:" << newID << endreq;
    if (StatusCode::SUCCESS != m_mdtPRDs_out->addCollection(driftCircleColl,hashID )) {
      log << MSG::ERROR
	  << "Couldn't record MDT Drift Circle Collection with hash=" << hashID
	  << " in StoreGate!" << endreq;
      continue;
    }
    else{
      log << MSG::DEBUG << " Added collection with hashID " << hashID
	  << " in StoreGate!" << endreq;
      
    }
  
    

	
  }
  return sc ;
  
}







StatusCode MuonPrdSelectorAlg::selectRPCs() {
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS ;
  
  // Clearing the outputContainers
  m_rpcPRDs_out->cleanup();
  
  
  Muon::RpcPrepDataContainer::const_iterator rpcColl = m_rpcPRDs_in->begin();
  Muon::RpcPrepDataContainer::const_iterator last_rpcColl = m_rpcPRDs_in->end();

  unsigned int i_rpcColl(0);
  
  
  for( ; rpcColl!=last_rpcColl ; ++rpcColl, ++i_rpcColl ){ //for each collection

    Identifier ID = (*rpcColl)->identify() ;
    IdentifierHash hashID = (*rpcColl)->identifyHash();
    Muon::RpcPrepDataCollection * rpcPrdColl = new Muon::RpcPrepDataCollection(hashID);
    (rpcPrdColl)->setIdentifier(ID);
   
    Muon::RpcPrepDataCollection::const_iterator rpcItr = (*rpcColl)->begin();
    Muon::RpcPrepDataCollection::const_iterator rpcEnd = (*rpcColl)->end();
    int i = 0;
    
    for(; rpcItr != rpcEnd; ++rpcItr,i++ ) { //for each hit in the collection
      Identifier hitID =(*rpcItr)->identify();

      if (!m_muonIdCutTool->isCut(hitID)){ //write event to new collection

	rpcPrdColl->push_back(new Muon::RpcPrepData(**rpcItr));
	log << MSG::DEBUG << "Writing Event." << endreq;
	      }
      else
	log << MSG::DEBUG << "Rejecting Event." << endreq;

    }	      
	  

    Identifier newID = (rpcPrdColl)->identify();
    log << MSG::DEBUG << "Adding New Collection to RPC list:" << newID << endreq;
    if (StatusCode::SUCCESS != m_rpcPRDs_out->addCollection(rpcPrdColl,hashID )) {
      log << MSG::ERROR
	  << "Couldn't record RPC Drift Circle Collection with hashID=" << hashID
	  << " in StoreGate!" << endreq;
      continue;
    }
    else{
      log << MSG::DEBUG << " Added collection with hashID " << hashID
	  << " in StoreGate!" << endreq;
      
    }
  
    

	
  }
  return sc ;
  
}


StatusCode MuonPrdSelectorAlg::selectTGCs() {
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS ;
  
  // Clearing the outputContainers
  m_tgcPRDs_out->cleanup();
  
  
  Muon::TgcPrepDataContainer::const_iterator tgcColl = m_tgcPRDs_in->begin();
  Muon::TgcPrepDataContainer::const_iterator last_tgcColl = m_tgcPRDs_in->end();

  unsigned int i_tgcColl(0);
  
  
  for( ; tgcColl!=last_tgcColl ; ++tgcColl, ++i_tgcColl ){ //for each collection

    Identifier ID = (*tgcColl)->identify() ;
    IdentifierHash hashID = (*tgcColl)->identifyHash();
    Muon::TgcPrepDataCollection * tgcPrdColl = new Muon::TgcPrepDataCollection(hashID);
    (tgcPrdColl)->setIdentifier(ID);
   
    Muon::TgcPrepDataCollection::const_iterator tgcItr = (*tgcColl)->begin();
    Muon::TgcPrepDataCollection::const_iterator tgcEnd = (*tgcColl)->end();
    int i = 0;
    
    for(; tgcItr != tgcEnd; ++tgcItr,i++ ) { //for each hit in the collection
      Identifier hitID =(*tgcItr)->identify();

      if (!m_muonIdCutTool->isCut(hitID)){ //write event to new collection

	tgcPrdColl->push_back(new Muon::TgcPrepData(**tgcItr));
	log << MSG::DEBUG << "Writing Event." << endreq;
	      }
      else
	log << MSG::DEBUG << "Rejecting Event." << endreq;

    }	      
	  

    Identifier newID = (tgcPrdColl)->identify();
    log << MSG::DEBUG << "Adding New Collection to TGC list:" << newID << endreq;
    if (StatusCode::SUCCESS != m_tgcPRDs_out->addCollection(tgcPrdColl,hashID )) {
      log << MSG::ERROR
	  << "Couldn't record TGC Drift Circle Collection with hashID=" << hashID
	  << " in StoreGate!" << endreq;
      continue;
    }
    else{
      log << MSG::DEBUG << " Added collection with hashID " << hashID
	  << " in StoreGate!" << endreq;
      
    }
  
    

	
  }
  return sc ;
  
}




StatusCode MuonPrdSelectorAlg::selectCSCs() {
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS ;
  
  // Clearing the outputContainers
  m_cscPRDs_out->cleanup();
  
  
  Muon::CscStripPrepDataContainer::const_iterator cscColl = m_cscPRDs_in->begin();
  Muon::CscStripPrepDataContainer::const_iterator last_cscColl = m_cscPRDs_in->end();

  unsigned int i_cscColl(0);
  
  
  for( ; cscColl!=last_cscColl ; ++cscColl, ++i_cscColl ){ //for each collection

    Identifier ID = (*cscColl)->identify() ;
    IdentifierHash hashID = (*cscColl)->identifyHash();
    Muon::CscStripPrepDataCollection * cscPrdColl = new Muon::CscStripPrepDataCollection(hashID);
    (cscPrdColl)->setIdentifier(ID);
   
    Muon::CscStripPrepDataCollection::const_iterator cscItr = (*cscColl)->begin();
    Muon::CscStripPrepDataCollection::const_iterator cscEnd = (*cscColl)->end();
    int i = 0;
    
    for(; cscItr != cscEnd; ++cscItr,i++ ) { //for each hit in the collection
      Identifier hitID =(*cscItr)->identify();

      if (!m_muonIdCutTool->isCut(hitID)){ //write event to new collection

	cscPrdColl->push_back(new Muon::CscStripPrepData(**cscItr));
	log << MSG::DEBUG << "Writing Event." << endreq;
	      }
      else
	log << MSG::DEBUG << "Rejecting Event." << endreq;

    }	      
	  

    Identifier newID = (cscPrdColl)->identify();
    log << MSG::DEBUG << "Adding New Collection to CSC list:" << newID << endreq;
    if (StatusCode::SUCCESS != m_cscPRDs_out->addCollection(cscPrdColl,hashID )) {
      log << MSG::ERROR
	  << "Couldn't record CSC Collection with hashID=" << hashID
	  << " in StoreGate!" << endreq;
      continue;
    }
    else{
      log << MSG::DEBUG << " Added collection with hashID " << hashID
	  << " in StoreGate!" << endreq;
      
    }	
  }
  return sc ;
  
}


void MuonPrdSelectorAlg::print(){
  MsgStream log(msgSvc(), name());

  Muon::MdtPrepDataContainer::const_iterator mdtColl = m_mdtPRDs_in->begin();
  Muon::MdtPrepDataContainer::const_iterator last_mdtColl = m_mdtPRDs_in->end();
  unsigned int i_coll(0);
  for( ; mdtColl!=last_mdtColl ; ++mdtColl, ++i_coll ){
    Identifier ID = (*mdtColl)->identify() ;  
    log << MSG::DEBUG << "MDT  IN Collection(" << std::setw(2) << i_coll << ") : " << ID << endreq;
  }

  mdtColl = m_mdtPRDs_out->begin();
  last_mdtColl = m_mdtPRDs_out->end();
  i_coll = 0 ;
  for( ; mdtColl!=last_mdtColl ; ++mdtColl, ++i_coll ){
    Identifier ID = (*mdtColl)->identify() ;  
    log << MSG::DEBUG << "MDT OUT Collection(" << std::setw(2) << i_coll << ") : " << ID << endreq;
  }

  Muon::RpcPrepDataContainer::const_iterator rpcColl = m_rpcPRDs_in->begin();
  Muon::RpcPrepDataContainer::const_iterator last_rpcColl = m_rpcPRDs_in->end();
  i_coll = 0 ;
  for( ; rpcColl!=last_rpcColl ; ++rpcColl, ++i_coll ){
    Identifier ID = (*rpcColl)->identify() ;  
    log << MSG::DEBUG << "RPC  IN Collection(" << std::setw(2) << i_coll << ") : " << ID << endreq;
  }

  rpcColl = m_rpcPRDs_out->begin();
  last_rpcColl = m_rpcPRDs_out->end();
  i_coll = 0 ;
  for( ; rpcColl!=last_rpcColl ; ++rpcColl, ++i_coll ){
    Identifier ID = (*rpcColl)->identify() ;  
    log << MSG::DEBUG << "RPC OUT Collection(" << std::setw(2) << i_coll << ") : " << ID << endreq;
  }


  Muon::TgcPrepDataContainer::const_iterator tgcColl = m_tgcPRDs_in->begin();
  Muon::TgcPrepDataContainer::const_iterator last_tgcColl = m_tgcPRDs_in->end();
  i_coll = 0 ;
  for( ; tgcColl!=last_tgcColl ; ++tgcColl, ++i_coll ){
    Identifier ID = (*tgcColl)->identify() ;  
    log << MSG::DEBUG << "TGC  IN Collection(" << std::setw(2) << i_coll << ") : " << ID << endreq;
  }

  tgcColl = m_tgcPRDs_out->begin();
  last_tgcColl = m_tgcPRDs_out->end();
  i_coll = 0 ;
  for( ; tgcColl!=last_tgcColl ; ++tgcColl, ++i_coll ){
    Identifier ID = (*tgcColl)->identify() ;  
    log << MSG::DEBUG << "TGC OUT Collection(" << std::setw(2) << i_coll << ") : " << ID << endreq;
  }

  Muon::CscStripPrepDataContainer::const_iterator cscColl = m_cscPRDs_in->begin();
  Muon::CscStripPrepDataContainer::const_iterator last_cscColl = m_cscPRDs_in->end();
  i_coll = 0 ;
  for( ; cscColl!=last_cscColl ; ++cscColl, ++i_coll ){
    Identifier ID = (*cscColl)->identify() ;  
    log << MSG::DEBUG << "CSC  IN Collection(" << std::setw(2) << i_coll << ") : " << ID << endreq;
  }

  cscColl = m_cscPRDs_out->begin();
  last_cscColl = m_cscPRDs_out->end();
  i_coll = 0 ;
  for( ; cscColl!=last_cscColl ; ++cscColl, ++i_coll ){
    Identifier ID = (*cscColl)->identify() ;  
    log << MSG::DEBUG << "CSC OUT Collection(" << std::setw(2) << i_coll << ") : " << ID << endreq;
  }

  const DataHandle<Muon::MdtPrepDataCollection> mdtCollection;
  const DataHandle<Muon::MdtPrepDataCollection> lastColl;
  i_coll = 0;
  if (m_StoreGate->retrieve(mdtCollection,lastColl) ==StatusCode::SUCCESS) {
    for ( ; mdtCollection != lastColl ; ++mdtCollection ) {
      log << MSG::DEBUG << "DIRECTLY FROM COLL (" <<std::setw(2) << i_coll << ") " << mdtCollection->identify() << endreq;
    }
  }

}



