/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_CalibModuleListTool.h"

SCT_CalibModuleListTool::SCT_CalibModuleListTool(const std::string& type, const std::string& name, const IInterface* parent) :
   base_class(type, name, parent),
   m_pSCTHelper(0),
   m_detStore("DetectorStore", name),
   m_IOVDbSvc( "IOVDbSvc", name )
{
}

StatusCode SCT_CalibModuleListTool::initialize() {
   if (msgLvl(MSG::DEBUG)) msg(MSG:: DEBUG) << "Initializing SCT_CalibModuleListTool "  << endmsg;

   StatusCode sc = m_detStore->retrieve( m_pSCTHelper, "SCT_ID" );
   if ( sc.isFailure() ) {
      msg( MSG::ERROR ) << "Unable to retrieve SCTHelper" << endmsg;
      return sc;
   }

   sc = m_MonitorConditionsTool.retrieve();
   if ( sc.isFailure() ) {
      msg( MSG::ERROR ) << "Unable to retrieve MonitorConditionsSvc" << endmsg;
      return sc;
   }

   sc = m_IOVDbSvc.retrieve();
   if ( sc.isFailure() ) {
      msg( MSG::ERROR ) << "Unable to retrieve IOVDbSvc" << endmsg;
      return sc;
   }

   return StatusCode::SUCCESS;
}

StatusCode SCT_CalibModuleListTool::finalize() {
   return StatusCode::SUCCESS;
}

StatusCode
SCT_CalibModuleListTool::queryInterface(const InterfaceID & riid, void** ppvInterface ) {
   if ( ISCT_CalibModuleListTool::interfaceID().versionMatch(riid) ) {
      *ppvInterface = dynamic_cast<ISCT_CalibModuleListTool*>(this);
   } else {
      return AthAlgTool::queryInterface(riid, ppvInterface);
   }
   addRef();
   return StatusCode::SUCCESS;
}

StatusCode SCT_CalibModuleListTool::readModuleList( std::map< Identifier, std::set<Identifier> >& moduleList ) {
   //--- Read strips using SCT_MonitorConditionsSvc
   SCT_ID::const_id_iterator waferIdItr  = m_pSCTHelper->wafer_begin();
   SCT_ID::const_id_iterator waferIdItrE = m_pSCTHelper->wafer_end();
   for( ; waferIdItr != waferIdItrE; ++waferIdItr ) {
      if ( m_pSCTHelper->side( *waferIdItr ) != 0 ) continue;
      Identifier waferId  = *waferIdItr;
      Identifier moduleId = m_pSCTHelper->module_id( waferId );

      std::set<Identifier> stripIdList;
      m_MonitorConditionsTool->badStrips( moduleId, stripIdList );
      if ( !stripIdList.empty() ) moduleList.insert( std::map< Identifier, std::set<Identifier> >::value_type( moduleId, stripIdList ) );
   }

   //--- Drop the folder to avoid conflict with SCTCalibWriteSvc
   if ( !( m_IOVDbSvc->dropObject( "/SCT/Derived/Monitoring", true ) ) ) msg( MSG::ERROR ) << "Unable to drop /SCT/Derived/Monitoring" << endmsg;

   return StatusCode::SUCCESS;
}
