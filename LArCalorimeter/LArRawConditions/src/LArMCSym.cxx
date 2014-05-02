/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMCSym.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "LArTools/LArCablingService.h"
#include "CaloIdentifier/LArID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include <iostream> 
using namespace std ;

LArMCSym::LArMCSym()
  : m_cablingService(0)
{
  
  // retrieve DetectorStore
  StatusCode sc = Gaudi::svcLocator()->service("DetectorStore", m_detStore);
  if (sc.isFailure()) {
    std::cout << "LArMCSym: Unable to retrieve pointer to DetectorStore " << std::endl;
  }
  
  // retrieve the CaloIdMgr
   const DataHandle<CaloIdManager> caloIdMgr;
  sc = m_detStore->retrieve(caloIdMgr);
  if (sc.isFailure()) {
    std::cout << "LArMCSym: Unable to retrieve caloIdMgr from DetectorStore " << std::endl;
  }
  
  // retrieve the helpers
  m_lar_em_id = caloIdMgr->getEM_ID();
  if (!m_lar_em_id) {
    std::cout << "LArMCSym: Could not access LArEM_ID helper " << std::endl;
  }
  m_lar_hec_id = caloIdMgr->getHEC_ID();
  if (!m_lar_hec_id) {
    std::cout << "LArMCSym: Could not access LArHEC_ID helper " << std::endl;
  }
  m_lar_fcal_id = caloIdMgr->getFCAL_ID();
  if (!m_lar_fcal_id) {
    std::cout << "LArMCSym: Could not access LArFCAL_ID helper " << std::endl;
  }
  
  // get CablingService
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  sc   = svcLoc->service( "ToolSvc",toolSvc  );
  if(sc.isSuccess()) {
    sc = toolSvc->retrieveTool("LArCablingService",m_cablingService);
      if(sc.isFailure()){
	std::cout << "LArMCSym: Could not retrieve Tool " << std::endl;
      }
  } else {
    std::cout << "LArMCSym: Could not retrieve ToolSvc " << std::endl;
  }
  
  /*  m_cablingService->set(m_lar_em_id);
      m_cablingService->set(m_lar_hec_id);
      m_cablingService->set(m_lar_fcal_id);*/
  
}

LArMCSym::~LArMCSym() {}

HWIdentifier LArMCSym::ZPhiSym(const HWIdentifier& NotSymOnId) const
{
  
  // Translate online ID to offline ID
  Identifier NotSymOffId;
  try {
    NotSymOffId = m_cablingService->cnvToIdentifier( NotSymOnId ) ;
  }
  catch(LArID_Exception & except ) {
    std::cout << "Invalid LArTBSignalChannelID = " << std::endl;
    throw except;
  }

  const AtlasDetectorID& atlas_id = *m_lar_em_id;
  int barrel_ec = 0;
  int sampling  = 0;
  int region    = 0;
  int eta       = 0;
  int pos_neg   = 0;
  int module    = 0;
  
  
  // retrieve fields of offline ID
  
  if(atlas_id.is_lar_em(NotSymOffId) ) {
    barrel_ec = abs( m_lar_em_id->barrel_ec(NotSymOffId) ) ;
    sampling  = m_lar_em_id->sampling(NotSymOffId);
    region    = m_lar_em_id->region(NotSymOffId);
    eta       = m_lar_em_id->eta(NotSymOffId);
  } else if(atlas_id.is_lar_hec(NotSymOffId)){
    pos_neg   = abs( m_lar_hec_id->pos_neg(NotSymOffId) ) ;
    sampling  = m_lar_hec_id->sampling(NotSymOffId);
    region    = m_lar_hec_id->region(NotSymOffId);
    eta       = m_lar_hec_id->eta(NotSymOffId);
  }else if(atlas_id.is_lar_fcal(NotSymOffId) ){
    pos_neg   = abs( m_lar_fcal_id->pos_neg(NotSymOffId) ) ;
    module    = m_lar_fcal_id->module(NotSymOffId);
    // eta       = m_lar_fcal_id->eta(NotSymOffId);
    eta = 0 ;
  }

  // apply phi symmetry
  int phi = 0;
  // apply z symmetry.  (done by the abs of pos_neg,barrel_ec above)
  // eta = abs(eta);

//  std::cout << "MCSym " << barrel_ec << " " << sampling << " " << region << " "
//            << eta << " " << phi << std::endl;


  // Build z-phi symmetric offline ID
  Identifier SymOffId;
  if(atlas_id.is_lar_em(NotSymOffId) ) {
    SymOffId = m_lar_em_id->channel_id(barrel_ec, sampling, region, eta, phi );
  } else if(atlas_id.is_lar_hec(NotSymOffId)){ 
    SymOffId = m_lar_hec_id->channel_id(pos_neg, sampling, region, eta, phi );
  } else if(atlas_id.is_lar_fcal(NotSymOffId)){ 
    SymOffId = m_lar_fcal_id->channel_id(pos_neg, module, eta, phi );
  }    

  // convert it back to online ID
  HWIdentifier SymOnId = m_cablingService->createSignalChannelID(SymOffId);  

  return SymOnId;
}

Identifier LArMCSym::ZPhiSym(const Identifier& NotSymOffId) const
{
  const AtlasDetectorID& atlas_id = *m_lar_em_id;
  int barrel_ec = 0;
  int sampling  = 0;
  int region    = 0;
  int eta       = 0;
  int pos_neg   = 0;
  int module    = 0;
  
  // retrieve fields of offline ID
  
  if(atlas_id.is_lar_em(NotSymOffId) ) {
    barrel_ec = abs(m_lar_em_id->barrel_ec(NotSymOffId));
    sampling  = m_lar_em_id->sampling(NotSymOffId);
    region    = m_lar_em_id->region(NotSymOffId);
    eta       = m_lar_em_id->eta(NotSymOffId);
  } else if(atlas_id.is_lar_hec(NotSymOffId)){ 
    pos_neg   = abs(m_lar_hec_id->pos_neg(NotSymOffId));
    sampling  = m_lar_hec_id->sampling(NotSymOffId);
    region    = m_lar_hec_id->region(NotSymOffId);
    eta       = m_lar_hec_id->eta(NotSymOffId);
  }else if(atlas_id.is_lar_fcal(NotSymOffId) ){
    pos_neg   = abs(m_lar_fcal_id->pos_neg(NotSymOffId));
    module    = m_lar_fcal_id->module(NotSymOffId);
//    eta       = m_lar_fcal_id->eta(NotSymOffId);
    eta=0;
  }

  // apply phi symmetry
  int phi = 0;
  // apply z symmetry
  // eta = abs(eta);

  // Build z-phi symmetric offline ID
  Identifier SymOffId;
  if(atlas_id.is_lar_em(NotSymOffId) ) {
    SymOffId = m_lar_em_id->channel_id(barrel_ec, sampling, region, eta, phi );
  } else if(atlas_id.is_lar_hec(NotSymOffId)){ 
    SymOffId = m_lar_hec_id->channel_id(pos_neg, sampling, region, eta, phi );
  } else if(atlas_id.is_lar_fcal(NotSymOffId)){ 
    SymOffId = m_lar_fcal_id->channel_id(pos_neg, module, eta, phi );
  }    
  return SymOffId;

}
