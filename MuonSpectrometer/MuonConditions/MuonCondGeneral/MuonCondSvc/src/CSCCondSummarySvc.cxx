/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/CSCCondSummarySvc.h"

#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include <iterator>

CSCCondSummarySvc::CSCCondSummarySvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_reportingServices(name),
  m_detStore("DetectorStore",name),
  m_noReports(true) {
  declareProperty("ConditionsServices",m_reportingServices);
  declareProperty("DetStore", m_detStore);
  declareProperty("UseSimulation",m_usesimulation=false);
}


StatusCode
CSCCondSummarySvc::initialize(){
  m_noReports = m_reportingServices.empty();
  ATH_CHECK(m_detStore.retrieve());
  ATH_CHECK(m_idHelperSvc.retrieve());
  
  if(m_usesimulation) {
    m_reportingServices.empty(); 
    ATH_MSG_DEBUG("Simulation setup: No Conditions Data for CSC !");
  } else ATH_MSG_DEBUG("Data DCS Conditions CSC !");
  
  if (m_noReports){
    ATH_MSG_DEBUG("No services were selected for the CSC summary");
  } else {
    ATH_CHECK(m_reportingServices.retrieve());
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator pSvc= m_reportingServices.begin();
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator pLastSvc= m_reportingServices.end();
     for (;pSvc not_eq pLastSvc; ++pSvc){
   
       const std::string& svcName = pSvc->name();
      ATH_MSG_DEBUG("Using "<< svcName);
      if (m_detStore->regFcn(&ICSCConditionsSvc::initInfo,&**pSvc,
                           &CSCCondSummarySvc::update_CSC,this) != StatusCode::SUCCESS){ 
        ATH_MSG_WARNING("Unable to register call back for "<<svcName); 
      } else {
        ATH_MSG_DEBUG("initInfo registered for call-back for "<<svcName);
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
CSCCondSummarySvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( ICSCConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ICSCConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode CSCCondSummarySvc::update_CSC(IOVSVC_CALLBACK_ARGS){
  ATH_MSG_DEBUG("Register Call Back for CSC System");
  return StatusCode::SUCCESS;
}

StatusCode CSCCondSummarySvc::initInfo(IOVSVC_CALLBACK_ARGS){
  ATH_MSG_DEBUG("Not to be called just dummy");
  return StatusCode::SUCCESS;
}


bool CSCCondSummarySvc::isGoodWireLayer(const Identifier & Id) const{
  bool result=true;
  // check ID
  Identifier ChamberId = m_idHelperSvc->cscIdHelper().elementID(Id);
  if (not m_noReports){
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
    for (;svc not_eq  lastSvc;svc++){
      if ((*svc)->deadStationsId().size()!=0) {
	bool found = std::binary_search((*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),ChamberId,Compare);
	if(found) result= false;
      }
    }
    
  } 
  
  return result;
}

bool CSCCondSummarySvc::isGood(const Identifier & Id) const{
  bool total_result = true;
  Identifier ChamberId = m_idHelperSvc->cscIdHelper().elementID(Id);
  if (not m_noReports){
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
   
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->deadStationsId().size()!=0) 
	{
	  ATH_MSG_VERBOSE(" dentro chamber dropped "); 
	  bool found = std::binary_search( 
					  (*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),ChamberId,Compare);
	  if(found) total_result = false;
	  ATH_MSG_VERBOSE(" Chamber Dropped by DCS or not installed at all ");

	}else total_result = true;
      ATH_MSG_VERBOSE("Thank-you for using the CSCCondSummarySvc,  service ");
    }   
  }

  return total_result;
  
}

bool CSCCondSummarySvc::isGoodChamber(const Identifier & Id) const{
  bool result=true;
  int counter =0;
   ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
    for (;svc not_eq  lastSvc;svc++){
      ATH_MSG_WARNING(" Dead Stations from the service , size= "<<(*svc)->deadStationsId().size());
      if ((*svc)->deadStationsId().size()!=0){

	bool found = std::binary_search( 
  	   (*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),Id,Compare);

	if(found) counter++;
      }
      else{
	ATH_MSG_WARNING(" Dead Stations from the service  are not availables "<<(*svc));
	
      }
    }
  
    if (counter!=0) result = false; 
    ATH_MSG_WARNING(" Dead Stations from the service  "<< counter);
    
    return result;
  
}






const std::vector<Identifier>& CSCCondSummarySvc::deadStationsId() const{
  if (not m_noReports){
   ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
    
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->deadStationsId().size()!=0){
	return (*svc)->deadStationsId();
      }else{
	ATH_MSG_WARNING(" Dead Stations from the service are not availables "<<(*svc));
      }
    }
    
  }
  return m_emptyId; 
}



const std::vector<Identifier>& CSCCondSummarySvc::deadWireLayersId() const{
  if (not m_noReports){
     ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
     ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
   
    for (;svc not_eq lastSvc;++svc){  
      if ((*svc)->deadWireLayersId().size()!=0){
	return (*svc)->deadWireLayersId();
      }else{
	ATH_MSG_WARNING(" Dead Wire from the service  are not availables "<<(*svc));
      }
    }
    
  }
   return m_emptyId;  
}


const std::vector<std::string>& CSCCondSummarySvc::deadStations() const{
  if (not m_noReports){
   ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
    
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->deadStations().size()!=0){
	return (*svc)->deadStations();
      }else{
	ATH_MSG_WARNING(" Dead Stations from the service  are not availables "<<(*svc));
      }
    }
    
  }
  return m_empty; 
}



const std::vector<std::string>& CSCCondSummarySvc::deadWireLayers() const{
  if (not m_noReports){
     ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
     ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
   
    for (;svc not_eq lastSvc;++svc){  
      if ((*svc)->deadWireLayers().size()!=0){
	return (*svc)->deadWireLayers();
      }else{
	ATH_MSG_WARNING(" Dead Wire from the service  are not availables "<<(*svc));
      }
    }
    
  }
   return m_empty;  
}
