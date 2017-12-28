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
  : AthAlgorithm(name,pSvcLocator),

    m_detMgr(0),
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
  ATH_MSG_DEBUG( "initialize() called"  );

  // retrieve MuonDetectorManager
  std::string managerName="Muon";
  if ( detStore()->retrieve(m_detMgr).isFailure() ) {
    ATH_MSG_INFO( "Could not find the MuonReadoutGeometry Manager: "
                  << managerName << " ! "  );
  } 
  else {
    m_mdtIdHelper = m_detMgr->mdtIdHelper();
    m_cscIdHelper = m_detMgr->cscIdHelper();
    m_rpcIdHelper = m_detMgr->rpcIdHelper();
    m_tgcIdHelper = m_detMgr->tgcIdHelper();
  } 

  // retrieve test tool
  ATH_CHECK(  m_muonIdCutTool.retrieve() );

  
  try {     
    m_mdtPRDs_out = new Muon::MdtPrepDataContainer(m_mdtIdHelper->module_hash_max());
  } catch(const std::bad_alloc&) {
    ATH_MSG_FATAL( "Could not create a new MDT PrepRawData container!" );
    return StatusCode::FAILURE;
  }
  m_mdtPRDs_out->addRef();

 
    
    try {
      m_rpcPRDs_out = new Muon::RpcPrepDataContainer(m_rpcIdHelper->module_hash_max());
    } catch(const std::bad_alloc&) {
      ATH_MSG_FATAL( "Could not create a new RPC PrepRawData container!" );
      return StatusCode::FAILURE;
    }
    m_rpcPRDs_out->addRef();

    try {
      m_tgcPRDs_out = new Muon::TgcPrepDataContainer(m_tgcIdHelper->module_hash_max());
    } catch(const std::bad_alloc&) {
      ATH_MSG_FATAL( "Could not create a new TGC PrepRawData container!" );
      return StatusCode::FAILURE;
    }
    m_tgcPRDs_out->addRef();
  
    
    try {
      m_cscPRDs_out = new Muon::CscStripPrepDataContainer(m_cscIdHelper->module_hash_max());
    } catch(const std::bad_alloc&) {
      ATH_MSG_FATAL( "Could not create a new CSC PrepRawData container!" );
      return StatusCode::FAILURE;
    }
    m_cscPRDs_out->addRef();
    
    
    return StatusCode::SUCCESS;
}

StatusCode MuonPrdSelectorAlg::execute()
{ 
  ATH_MSG_DEBUG( "execute() called"  );
  ATH_CHECK( retrieveContainers() );
  ATH_CHECK( recordContainers() );
  ATH_CHECK( performSelection() );
  return StatusCode::SUCCESS;
}

StatusCode MuonPrdSelectorAlg::finalize()
{
  ATH_MSG_DEBUG( "finalize() called"  );
  return StatusCode::SUCCESS;
}


StatusCode MuonPrdSelectorAlg::retrieveContainers() {
  ATH_MSG_DEBUG( "retrieveContainers() called"  );
  
  //resetting the inputcollections.
  m_mdtPRDs_in = 0;
  m_rpcPRDs_in = 0;
  m_tgcPRDs_in = 0;
  m_cscPRDs_in = 0;

  StatusCode sc = evtStore()->retrieve( m_mdtPRDs_in, m_inputContainer_mdt );
  if( sc.isFailure() ) {
    ATH_MSG_WARNING( "Could not retrieve MDT PRDs input container " 
                     << m_inputContainer_mdt  );
  } else {
    ATH_MSG_DEBUG( "Retrieved MDT PRDs input container " << m_inputContainer_mdt << " ( " 
                   << m_mdtPRDs_in->size() << " ) "  );
  }

  sc = evtStore()->retrieve( m_rpcPRDs_in, m_inputContainer_rpc );
  if( sc.isFailure() ) {
    ATH_MSG_WARNING( "Could not retrieve RPC PRDs input container " 
                     << m_inputContainer_rpc  );
  } else {
    ATH_MSG_DEBUG( "Retrieved RPC PRDs input container " << m_inputContainer_rpc << " ( " 
                   << m_rpcPRDs_in->size() << " ) "  );
  }

  sc = evtStore()->retrieve( m_tgcPRDs_in, m_inputContainer_tgc );
  if( sc.isFailure() ) {
    ATH_MSG_WARNING( "Could not retrieve TGC PRDs input container " 
                     << m_inputContainer_tgc  );
  } else {
    ATH_MSG_DEBUG( "Retrieved TGC PRDs input container " << m_inputContainer_tgc << " ( " 
                   << m_tgcPRDs_in->size() << " ) "  );
  }

  sc = evtStore()->retrieve( m_cscPRDs_in, m_inputContainer_csc );
  if( sc.isFailure() ) {
    ATH_MSG_WARNING( "Could not retrieve CSC PRDs input container " 
                     << m_inputContainer_csc  );
  } else {
    ATH_MSG_DEBUG( "Retrieved CSC PRDs input container " << m_inputContainer_csc << " ( " 
                   << m_cscPRDs_in->size() << " ) "  );
  }

  return StatusCode::SUCCESS;
}


StatusCode MuonPrdSelectorAlg::recordContainers() {
  StatusCode sc = evtStore()->record( m_mdtPRDs_out, m_outputContainer_mdt );
  if( sc.isFailure() ) ATH_MSG_DEBUG( "Could not record MDT PRDs to output container " << m_outputContainer_mdt  );

  sc = evtStore()->record( m_rpcPRDs_out, m_outputContainer_rpc );
  if( sc.isFailure() ) ATH_MSG_DEBUG( "Could not record RPC PRDs to output container " << m_outputContainer_rpc  );

  sc = evtStore()->record( m_tgcPRDs_out, m_outputContainer_tgc );
  if( sc.isFailure() ) ATH_MSG_DEBUG( "Could not record TGC PRDs to output container " << m_outputContainer_tgc  );

  sc = evtStore()->record( m_cscPRDs_out, m_outputContainer_csc );
  if( sc.isFailure() ) ATH_MSG_DEBUG( "Could not record CSC PRDs to output container " << m_outputContainer_csc  );
  return sc;
}

StatusCode MuonPrdSelectorAlg::performSelection() {
  if( m_mdtPRDs_in ) ATH_CHECK( selectMDTs() );
  if( m_rpcPRDs_in ) ATH_CHECK( selectRPCs() );
  if( m_tgcPRDs_in ) ATH_CHECK( selectTGCs() );
  if( m_cscPRDs_in ) ATH_CHECK( selectCSCs() );
  
  return StatusCode::SUCCESS;
}


StatusCode MuonPrdSelectorAlg::selectMDTs() {
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
	ATH_MSG_DEBUG( "Writing Event."  );
      }
      else
	ATH_MSG_DEBUG( "Rejecting Event."  );

    }	      
	  

    Identifier newID = (driftCircleColl)->identify();
    ATH_MSG_DEBUG( "Adding New Collection to MDT list:" << newID  );
    if (StatusCode::SUCCESS != m_mdtPRDs_out->addCollection(driftCircleColl,hashID )) {
      ATH_MSG_ERROR( "Couldn't record MDT Drift Circle Collection with hash=" << hashID
                     << " in StoreGate!"  );
      continue;
    }
    else{
      ATH_MSG_DEBUG( " Added collection with hashID " << hashID
                     << " in StoreGate!"  );
      
    }
  }
  return sc ;
}


StatusCode MuonPrdSelectorAlg::selectRPCs() {
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
	ATH_MSG_DEBUG( "Writing Event."  );
      }
      else
	ATH_MSG_DEBUG( "Rejecting Event."  );

    }	      
	  

    Identifier newID = (rpcPrdColl)->identify();
    ATH_MSG_DEBUG( "Adding New Collection to RPC list:" << newID  );
    if (StatusCode::SUCCESS != m_rpcPRDs_out->addCollection(rpcPrdColl,hashID )) {
      ATH_MSG_ERROR( "Couldn't record RPC Drift Circle Collection with hashID=" << hashID
                     << " in StoreGate!"  );
      continue;
    }
    else{
      ATH_MSG_DEBUG( " Added collection with hashID " << hashID
                     << " in StoreGate!"  );
      
    }
  }
  return sc ;
}


StatusCode MuonPrdSelectorAlg::selectTGCs() {
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
	ATH_MSG_DEBUG( "Writing Event."  );
      }
      else
	ATH_MSG_DEBUG( "Rejecting Event."  );

    }	      
	  

    Identifier newID = (tgcPrdColl)->identify();
    ATH_MSG_DEBUG( "Adding New Collection to TGC list:" << newID  );
    if (StatusCode::SUCCESS != m_tgcPRDs_out->addCollection(tgcPrdColl,hashID )) {
      ATH_MSG_ERROR( "Couldn't record TGC Drift Circle Collection with hashID=" << hashID
                     << " in StoreGate!"  );
      continue;
    }
    else{
      ATH_MSG_DEBUG( " Added collection with hashID " << hashID
                     << " in StoreGate!"  );
    }
  }
  return sc ;
}


StatusCode MuonPrdSelectorAlg::selectCSCs() {
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
	ATH_MSG_DEBUG( "Writing Event."  );
      }
      else
	ATH_MSG_DEBUG( "Rejecting Event."  );
    }	      
	  

    Identifier newID = (cscPrdColl)->identify();
    ATH_MSG_DEBUG( "Adding New Collection to CSC list:" << newID  );
    if (StatusCode::SUCCESS != m_cscPRDs_out->addCollection(cscPrdColl,hashID )) {
      ATH_MSG_ERROR( "Couldn't record CSC Collection with hashID=" << hashID
                     << " in StoreGate!"  );
      continue;
    }
    else{
      ATH_MSG_DEBUG( " Added collection with hashID " << hashID
                     << " in StoreGate!"  );
      
    }	
  }
  return sc ;
}


void MuonPrdSelectorAlg::print(){
  Muon::MdtPrepDataContainer::const_iterator mdtColl = m_mdtPRDs_in->begin();
  Muon::MdtPrepDataContainer::const_iterator last_mdtColl = m_mdtPRDs_in->end();
  unsigned int i_coll(0);
  for( ; mdtColl!=last_mdtColl ; ++mdtColl, ++i_coll ){
    Identifier ID = (*mdtColl)->identify() ;  
    ATH_MSG_DEBUG( "MDT  IN Collection(" << std::setw(2) << i_coll << ") : " << ID  );
  }

  mdtColl = m_mdtPRDs_out->begin();
  last_mdtColl = m_mdtPRDs_out->end();
  i_coll = 0 ;
  for( ; mdtColl!=last_mdtColl ; ++mdtColl, ++i_coll ){
    Identifier ID = (*mdtColl)->identify() ;  
    ATH_MSG_DEBUG( "MDT OUT Collection(" << std::setw(2) << i_coll << ") : " << ID  );
  }

  Muon::RpcPrepDataContainer::const_iterator rpcColl = m_rpcPRDs_in->begin();
  Muon::RpcPrepDataContainer::const_iterator last_rpcColl = m_rpcPRDs_in->end();
  i_coll = 0 ;
  for( ; rpcColl!=last_rpcColl ; ++rpcColl, ++i_coll ){
    Identifier ID = (*rpcColl)->identify() ;  
    ATH_MSG_DEBUG( "RPC  IN Collection(" << std::setw(2) << i_coll << ") : " << ID  );
  }

  rpcColl = m_rpcPRDs_out->begin();
  last_rpcColl = m_rpcPRDs_out->end();
  i_coll = 0 ;
  for( ; rpcColl!=last_rpcColl ; ++rpcColl, ++i_coll ){
    Identifier ID = (*rpcColl)->identify() ;  
    ATH_MSG_DEBUG( "RPC OUT Collection(" << std::setw(2) << i_coll << ") : " << ID  );
  }


  Muon::TgcPrepDataContainer::const_iterator tgcColl = m_tgcPRDs_in->begin();
  Muon::TgcPrepDataContainer::const_iterator last_tgcColl = m_tgcPRDs_in->end();
  i_coll = 0 ;
  for( ; tgcColl!=last_tgcColl ; ++tgcColl, ++i_coll ){
    Identifier ID = (*tgcColl)->identify() ;  
    ATH_MSG_DEBUG( "TGC  IN Collection(" << std::setw(2) << i_coll << ") : " << ID  );
  }

  tgcColl = m_tgcPRDs_out->begin();
  last_tgcColl = m_tgcPRDs_out->end();
  i_coll = 0 ;
  for( ; tgcColl!=last_tgcColl ; ++tgcColl, ++i_coll ){
    Identifier ID = (*tgcColl)->identify() ;  
    ATH_MSG_DEBUG( "TGC OUT Collection(" << std::setw(2) << i_coll << ") : " << ID  );
  }

  Muon::CscStripPrepDataContainer::const_iterator cscColl = m_cscPRDs_in->begin();
  Muon::CscStripPrepDataContainer::const_iterator last_cscColl = m_cscPRDs_in->end();
  i_coll = 0 ;
  for( ; cscColl!=last_cscColl ; ++cscColl, ++i_coll ){
    Identifier ID = (*cscColl)->identify() ;  
    ATH_MSG_DEBUG( "CSC  IN Collection(" << std::setw(2) << i_coll << ") : " << ID  );
  }

  cscColl = m_cscPRDs_out->begin();
  last_cscColl = m_cscPRDs_out->end();
  i_coll = 0 ;
  for( ; cscColl!=last_cscColl ; ++cscColl, ++i_coll ){
    Identifier ID = (*cscColl)->identify() ;  
    ATH_MSG_DEBUG( "CSC OUT Collection(" << std::setw(2) << i_coll << ") : " << ID  );
  }

  const DataHandle<Muon::MdtPrepDataCollection> mdtCollection;
  const DataHandle<Muon::MdtPrepDataCollection> lastColl;
  i_coll = 0;
  if (evtStore()->retrieve(mdtCollection,lastColl) ==StatusCode::SUCCESS) {
    for ( ; mdtCollection != lastColl ; ++mdtCollection ) {
      ATH_MSG_DEBUG( "DIRECTLY FROM COLL (" <<std::setw(2) << i_coll << ") " << mdtCollection->identify()  );
    }
  }

}



