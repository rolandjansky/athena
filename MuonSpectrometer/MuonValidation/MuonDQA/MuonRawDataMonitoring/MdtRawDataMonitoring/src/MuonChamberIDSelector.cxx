/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtRawDataMonitoring/MuonChamberIDSelector.h"
#include "MuonDQAUtils/MuonChamberNameConverter.h"

#include <iomanip>

// Constructor with parameters:
MuonChamberIDSelector::MuonChamberIDSelector(const std::string &name, ISvcLocator *pSvcLocator) 
  : AthAlgorithm(name,pSvcLocator),
   m_mdtchambersId(0),
   m_rpcchambersId(0) 
{}

// Initialize method:
StatusCode MuonChamberIDSelector::initialize()
{
  ATH_CHECK(AthAlgorithm::initialize());
  ATH_MSG_DEBUG( "initialize() called" );
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode MuonChamberIDSelector::execute()
{ 
  ATH_MSG_DEBUG( "execute() called" );

  StatusCode   sc = ChamberperformSelection();
  if( sc.isFailure() ) ATH_MSG_DEBUG( "unable to perform selection" );
  
  return StatusCode::SUCCESS;
}

StatusCode MuonChamberIDSelector::ChamberperformSelection() {
  StatusCode sc = StatusCode::SUCCESS;
  sc = selectMDT() ;
  sc = selectRPC() ;
  sc = selectTGC() ;
  sc = selectCSC() ;
  return sc ;
}

StatusCode MuonChamberIDSelector::selectMDT() {
  StatusCode sc = StatusCode::SUCCESS ;
  ATH_MSG_DEBUG( "in MDT ChambersSelectorID vector" );  

  std::vector<Identifier>::const_iterator  idfirst = m_idHelperSvc->mdtIdHelper().module_begin();
  std::vector<Identifier>::const_iterator  idlast =  m_idHelperSvc->mdtIdHelper().module_end();

  IdContext mdtModuleContext = m_idHelperSvc->mdtIdHelper().module_context();
  Identifier Id;
  IdentifierHash Idhash;
   	 
  for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
    {    
      Id=*i;
      int gethash_code = m_idHelperSvc->mdtIdHelper().get_hash(Id, Idhash, &mdtModuleContext); 
    
      m_mdtchambersId->push_back(Id);     
      std::string extid = m_idHelperSvc->mdtIdHelper().show_to_string(Id);
      ATH_MSG_DEBUG( "Adding the chamber Identifier: " << extid );
      if (gethash_code == 0) ATH_MSG_DEBUG(" its hash Id is "<< Idhash );
      else                   ATH_MSG_DEBUG("  hash Id NOT computed "<< Idhash );
    } 
 
  return sc;

}

StatusCode MuonChamberIDSelector::selectRPC() {

  StatusCode sc = StatusCode::SUCCESS ;
  ATH_MSG_DEBUG( "in RPC ChambersSelectorID vector" );  

  std::vector<Identifier>::const_iterator  idfirst = m_idHelperSvc->rpcIdHelper().module_begin();
  std::vector<Identifier>::const_iterator  idlast =  m_idHelperSvc->rpcIdHelper().module_end();

  IdContext rpcModuleContext = m_idHelperSvc->rpcIdHelper().module_context();
  Identifier Id;
  IdentifierHash Idhash;
  m_rpcchambersId = new std::vector<Identifier>;
  	 
  for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
    {    
      Id=*i;
      int gethash_code = m_idHelperSvc->rpcIdHelper().get_hash(Id, Idhash, &rpcModuleContext); 
      m_rpcchambersId->push_back(Id);
      std::string extid = m_idHelperSvc->rpcIdHelper().show_to_string(Id);
      ATH_MSG_DEBUG( "Adding the chamber Identifier: " << extid );
      if (gethash_code == 0) ATH_MSG_DEBUG(" its hash Id is "<< Idhash );
      else                   ATH_MSG_DEBUG("  hash Id NOT computed "<< Idhash );
    } 
 
  return sc ;

}
