/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MuonChamberIDSelector.cxx
//   Source file for class MuonChamberIDSelector
///////////////////////////////////////////////////////////////////

#include "MdtRawDataMonitoring/MuonChamberIDSelector.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "Identifier/IdentifierHash.h"
#include "MuonDQAUtils/MuonChamberNameConverter.h"

#include <iomanip>

// Constructor with parameters:
MuonChamberIDSelector::MuonChamberIDSelector(const std::string &name, ISvcLocator *pSvcLocator) 
  : AthAlgorithm(name,pSvcLocator),
   m_StoreGate(0),
   m_detMgr(0),
   m_mdtIdHelper(0),
   m_cscIdHelper(0),
   m_rpcIdHelper(0),
   m_tgcIdHelper(0),
   m_mdtchambersId(0),
   m_rpcchambersId(0) 
{}

MuonChamberIDSelector::~MuonChamberIDSelector()
{
  ATH_MSG_INFO( " deleting MuonChamberIDSelector " );
}

// Initialize method:
StatusCode MuonChamberIDSelector::initialize()
{
  StatusCode sc;
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG( "initialize() called" );

  IToolSvc* toolsvc;
  sc=service("ToolSvc",toolsvc);
  if( sc.isFailure() ){
    ATH_MSG_WARNING("Could not get tool sevices "); 
    return( StatusCode::FAILURE );
  }

  // Retrieve StoreGate
  sc=service("StoreGateSvc",m_StoreGate);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "StoreGate service not found !" );
    return StatusCode::FAILURE;
  } 

  // Retrieve DetectorStore
  StoreGateSvc* detStore = 0;
  sc = service( "DetectorStore", detStore );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get DetectorStore");
    return sc;
  }
  
  // retrieve MuonDetectorManager
  std::string managerName="Muon";
  sc=detStore->retrieve(m_detMgr);
  if (sc.isFailure()) {
    ATH_MSG_INFO( "Could not find the MuonGeoModel Manager: "	<< managerName << " ! " );
  } 

  // initialize MuonIdHelpers
  if (m_detMgr) {
    m_mdtIdHelper = m_detMgr->mdtIdHelper();
    m_cscIdHelper = m_detMgr->cscIdHelper();
    m_rpcIdHelper = m_detMgr->rpcIdHelper();
    m_tgcIdHelper = m_detMgr->tgcIdHelper();
  } else {
    m_mdtIdHelper = 0;
    m_cscIdHelper = 0;
    m_rpcIdHelper = 0;
    m_tgcIdHelper = 0;
  }

  return sc;
}

StatusCode MuonChamberIDSelector::execute()
{ 
  ATH_MSG_DEBUG( "execute() called" );

  StatusCode   sc = ChamberperformSelection();
  if( sc.isFailure() ) ATH_MSG_DEBUG( "unable to perform selection" );
  
  return StatusCode::SUCCESS;
}

StatusCode MuonChamberIDSelector::finalize()
{
  ATH_MSG_DEBUG( "finalize() called" );
  
  return StatusCode::SUCCESS;
}


StatusCode MuonChamberIDSelector::ChamberperformSelection() {
  StatusCode sc = StatusCode::SUCCESS ;
  
  //sc = selectMDT(std::vector<Identifier>& mdtchambersId) ;
  sc = selectMDT() ;
  sc = selectRPC() ;
  sc = selectTGC() ;
  sc = selectCSC() ;

  return sc ;
}

//StatusCode MuonChamberIDSelector::selectMDT(std::vector<Identifier>& mdtchambersId) {
StatusCode MuonChamberIDSelector::selectMDT() {
  StatusCode sc = StatusCode::SUCCESS ;
  ATH_MSG_DEBUG( "in MDT ChambersSelectorID vector" );  

  std::vector<Identifier>::const_iterator  idfirst = m_mdtIdHelper->module_begin();
  std::vector<Identifier>::const_iterator  idlast =  m_mdtIdHelper->module_end();

  IdContext mdtModuleContext = m_mdtIdHelper->module_context();
  Identifier Id;
  IdentifierHash Idhash;
   	 
  for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
    {    
      Id=*i;
      int gethash_code = m_mdtIdHelper->get_hash(Id, Idhash, &mdtModuleContext); 
    
      m_mdtchambersId->push_back(Id);     
      std::string extid = m_mdtIdHelper->show_to_string(Id);
      ATH_MSG_DEBUG( "Adding the chamber Identifier: " << extid );
      if (gethash_code == 0) ATH_MSG_DEBUG(" its hash Id is "<< Idhash );
      else                   ATH_MSG_DEBUG("  hash Id NOT computed "<< Idhash );
    } 
 
  return sc;

}

StatusCode MuonChamberIDSelector::selectRPC() {

  StatusCode sc = StatusCode::SUCCESS ;
  ATH_MSG_DEBUG( "in RPC ChambersSelectorID vector" );  

  std::vector<Identifier>::const_iterator  idfirst = m_rpcIdHelper->module_begin();
  std::vector<Identifier>::const_iterator  idlast =  m_rpcIdHelper->module_end();

  IdContext rpcModuleContext = m_rpcIdHelper->module_context();
  Identifier Id;
  IdentifierHash Idhash;
  m_rpcchambersId = new std::vector<Identifier>;
  	 
  for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
    {    
      Id=*i;
      int gethash_code = m_rpcIdHelper->get_hash(Id, Idhash, &rpcModuleContext); 
      m_rpcchambersId->push_back(Id);
      std::string extid = m_rpcIdHelper->show_to_string(Id);
      ATH_MSG_DEBUG( "Adding the chamber Identifier: " << extid );
      if (gethash_code == 0) ATH_MSG_DEBUG(" its hash Id is "<< Idhash );
      else                   ATH_MSG_DEBUG("  hash Id NOT computed "<< Idhash );
    } 
 
  return sc ;

}

StatusCode MuonChamberIDSelector::selectTGC() {

  StatusCode sc = StatusCode::SUCCESS ;
  ATH_MSG_DEBUG( "in TGC ChambersSelectorID vector" );  
 
  return sc ;

}

StatusCode MuonChamberIDSelector::selectCSC() {

  StatusCode sc = StatusCode::SUCCESS ;
  ATH_MSG_DEBUG( "in CSC ChambersSelectorID vector" );  
 
  return sc ;

}
 
