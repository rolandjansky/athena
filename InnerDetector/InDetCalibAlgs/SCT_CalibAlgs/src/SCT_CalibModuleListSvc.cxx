/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_CalibModuleListSvc.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"

SCT_CalibModuleListSvc::SCT_CalibModuleListSvc(const std::string& name, ISvcLocator* pSvcLocator ) :
    AthService(name, pSvcLocator),
    m_pSCTHelper(0),
    m_detStore("DetectorStore", name),
    m_MonitorConditionsSvc( "SCT_MonitorConditionsSvc", name ),
    m_IOVDbSvc( "IOVDbSvc", name ){

}
    
SCT_CalibModuleListSvc::~SCT_CalibModuleListSvc(){
  //nop
}
   
   
StatusCode SCT_CalibModuleListSvc::initialize(){
  if (msgLvl(MSG::DEBUG)) msg(MSG:: DEBUG) << "Initializing SCT_CalibModuleListSvc "  << endreq;
  
  StatusCode sc = m_detStore->retrieve( m_pSCTHelper, "SCT_ID" );
   if ( sc.isFailure() ) {
     msg( MSG::ERROR ) << "Unable to retrieve SCTHelper" << endreq;
     return sc;
   }

  sc = m_MonitorConditionsSvc.retrieve();
  if ( sc.isFailure() ) {
    msg( MSG::ERROR ) << "Unable to retrieve MonitorConditionsSvc" << endreq;
    return sc;
  }
  
  sc = m_IOVDbSvc.retrieve();
   if ( sc.isFailure() ) {
     msg( MSG::ERROR ) << "Unable to retrieve IOVDbSvc" << endreq;
     return sc;
   }
  
  return StatusCode::SUCCESS;
    
}

StatusCode SCT_CalibModuleListSvc::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode 
SCT_CalibModuleListSvc::queryInterface(const InterfaceID & riid, void** ppvInterface ){
  if ( ISCT_CalibModuleListSvc::interfaceID().versionMatch(riid) ) {
   *ppvInterface = dynamic_cast<ISCT_CalibModuleListSvc*>(this);
  } else {
   return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode SCT_CalibModuleListSvc::readModuleList( std::map< Identifier, std::set<Identifier> >& moduleList ) {
  //--- Read strips using SCT_MonitorConditionsSvc
  SCT_ID::const_id_iterator waferIdItr  = m_pSCTHelper->wafer_begin();
  SCT_ID::const_id_iterator waferIdItrE = m_pSCTHelper->wafer_end();
  for( ; waferIdItr != waferIdItrE; ++waferIdItr ) {
    if ( m_pSCTHelper->side( *waferIdItr ) != 0 ) continue;
    Identifier waferId  = *waferIdItr;
    Identifier moduleId = m_pSCTHelper->module_id( waferId );

    std::set<Identifier> stripIdList;
    m_MonitorConditionsSvc->badStrips( moduleId, stripIdList );
    if ( !stripIdList.empty() ) moduleList.insert( std::map< Identifier, std::set<Identifier> >::value_type( moduleId, stripIdList ) );
  }

  //--- Drop the folder to avoid conflict with SCTCalibWriteSvc
  if ( !( m_IOVDbSvc->dropObject( "/SCT/Derived/Monitoring", true ) ) ) msg( MSG::ERROR ) << "Unable to drop /SCT/Derived/Monitoring" << endreq;

  return StatusCode::SUCCESS;
}