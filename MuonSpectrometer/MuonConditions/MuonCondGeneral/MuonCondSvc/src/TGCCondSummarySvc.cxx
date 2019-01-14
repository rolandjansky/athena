/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/TGCCondSummarySvc.h"
//STL includes
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include <iterator>
#include "GaudiKernel/ISvcLocator.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"
//Athena
#include "Identifier/IdentifierHash.h"
//Muon
#include "MuonIdHelpers/TgcIdHelper.h"
//#include "MuonCondInterface/IMuonDetectorStatusDbTool.h"
#include "MuonCondInterface/ITGCConditionsSvc.h"

#include "Identifier/Identifier.h"



using namespace std;

// Constructor
TGCCondSummarySvc::TGCCondSummarySvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator), 
  m_reportingServices(name),
  m_pHelper(0),
  m_detStore("DetectorStore",name),
  m_noReports(true){
  //  m_reportingServices.push_back("MDT_TGCConditionsSvc"); has to be empty!!!
  declareProperty("ConditionsServices",m_reportingServices);
  declareProperty("DetStore", m_detStore);
}


StatusCode
TGCCondSummarySvc::initialize(){
  StatusCode sc(StatusCode::FAILURE);
  m_noReports = m_reportingServices.empty();  
  
  sc = m_detStore.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "DetectorStore service not found !" << endmsg;
    return sc;
  } else {
    msg(MSG::INFO) << "DetectorStore service found !" << endmsg;
  }  
  
 
  if (m_noReports){
    sc=StatusCode::SUCCESS;
    msg(MSG::INFO)<<"No services were selected for the TGC summary"<<endmsg;
  } else {
   
    sc = m_reportingServices.retrieve();
    if ( sc.isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve " << m_reportingServices << endmsg;
      return StatusCode::FAILURE;
    }
    
    ServiceHandleArray<ITGCConditionsSvc>::const_iterator pSvc= m_reportingServices.begin();
    ServiceHandleArray<ITGCConditionsSvc>::const_iterator pLastSvc= m_reportingServices.end();
    for (;pSvc not_eq pLastSvc; ++pSvc){
      const std::string& svcName = pSvc->name();
      msg(MSG::INFO)<<"Using "<< svcName << endmsg;
      if (m_detStore->regFcn(&ITGCConditionsSvc::initInfo,&**pSvc,
                           &TGCCondSummarySvc::update_TGC,this) != StatusCode::SUCCESS){ 
        msg(MSG::WARNING)<<"Unable to register call back for "<<svcName<<endmsg; 
      } else {
        msg(MSG::INFO)<<"initInfo registered for call-back for "<<svcName<<endmsg;
      }
    }
  }   
  return sc;
}



//Finalize
StatusCode
TGCCondSummarySvc::finalize(){
  msg(MSG::INFO)<<"Thank-you for using the TGCCondSummarySvc, version "<<PACKAGE_VERSION<<endmsg;
  //Code
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

  msg(MSG::INFO)<<"Register Call Back for TGC System"<<endmsg;

   return StatusCode::SUCCESS;
}

StatusCode TGCCondSummarySvc::initInfo(IOVSVC_CALLBACK_ARGS){

  msg(MSG::INFO)<<"Not to be called just dummy"<<endmsg;
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
	msg(MSG::INFO)<<" Dead Stations from the service are not available "<<(*svc) <<endmsg;
	
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
	msg(MSG::INFO)<<" Dead Stations from the service  are not availables "<<(*svc) <<endmsg;
      }
    }
    
  }
  return m_emptyId; 
}


