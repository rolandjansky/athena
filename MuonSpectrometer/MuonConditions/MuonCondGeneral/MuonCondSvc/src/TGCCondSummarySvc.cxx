/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/TGCCondSummarySvc.h"

#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include <iterator>

// Constructor
TGCCondSummarySvc::TGCCondSummarySvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator), 
  m_reportingServices(name),
  m_detStore("DetectorStore",name),
  m_noReports(true){
  declareProperty("ConditionsServices",m_reportingServices);
  declareProperty("DetStore", m_detStore);
}


StatusCode
TGCCondSummarySvc::initialize(){
  m_noReports = m_reportingServices.empty();  
  ATH_CHECK(m_detStore.retrieve());
  if (m_noReports){
    ATH_MSG_DEBUG("No services were selected for the TGC summary");
  } else {
   
    ATH_CHECK(m_reportingServices.retrieve());

    ServiceHandleArray<ITGCConditionsSvc>::const_iterator pSvc= m_reportingServices.begin();
    ServiceHandleArray<ITGCConditionsSvc>::const_iterator pLastSvc= m_reportingServices.end();
    for (;pSvc not_eq pLastSvc; ++pSvc){
      const std::string& svcName = pSvc->name();
      ATH_MSG_DEBUG("Using "<< svcName);
      if (m_detStore->regFcn(&ITGCConditionsSvc::initInfo,&**pSvc,
                           &TGCCondSummarySvc::update_TGC,this) != StatusCode::SUCCESS){ 
        ATH_MSG_WARNING("Unable to register call back for "<<svcName); 
      } else {
        ATH_MSG_DEBUG("initInfo registered for call-back for "<<svcName);
      }
    }
  }   
  return StatusCode::SUCCESS;
}

StatusCode
TGCCondSummarySvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( ITGCConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ITGCConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode TGCCondSummarySvc::update_TGC(IOVSVC_CALLBACK_ARGS){
   ATH_MSG_DEBUG("Register Call Back for TGC System");
   return StatusCode::SUCCESS;
}

StatusCode TGCCondSummarySvc::initInfo(IOVSVC_CALLBACK_ARGS){
   ATH_MSG_DEBUG("Not to be called just dummy");
   return StatusCode::SUCCESS;
}


bool TGCCondSummarySvc::isGoodChamber(const Identifier & Id){
  
  bool result= true;
  int counter= 0;
  
  if (not m_noReports){
    ServiceHandleArray<ITGCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<ITGCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq  lastSvc;svc++){
      if ((*svc)->deadStationsId().size()!=0){
	bool found = std::binary_search( 
  	   (*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),Id,Compare);
        if(found) counter++;
      }else{
	ATH_MSG_DEBUG(" Dead Stations from the service are not available "<<(*svc));
      }
    }
  }
  
  if (counter!=0) result = false; 
  return result;
  
}

const std::vector<Identifier>& TGCCondSummarySvc::deadStationsId(){
  m_emptyId.clear();
  if (not m_noReports){
    ServiceHandleArray<ITGCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<ITGCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq  lastSvc;svc++){
      if ((*svc)->deadStationsId().size()!=0){
	return (*svc)->deadStationsId();
      }else{
	ATH_MSG_DEBUG(" Dead Stations from the service  are not availables "<<(*svc));
      }
    }
  }
  return m_emptyId; 
}


