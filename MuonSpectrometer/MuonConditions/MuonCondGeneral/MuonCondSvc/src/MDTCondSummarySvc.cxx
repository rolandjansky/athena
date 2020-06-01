/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/MDTCondSummarySvc.h"

#include <vector>
#include <list>
#include <algorithm>
#include <sstream>

MDTCondSummarySvc::MDTCondSummarySvc( const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_reportingServices(name),
  m_detStore("DetectorStore",name),m_noReports(true) {
  declareProperty("ConditionsServices",m_reportingServices);
  declareProperty("DetStore", m_detStore);
}


StatusCode
MDTCondSummarySvc::initialize(){
  m_noReports = m_reportingServices.empty();
  ATH_CHECK(m_detStore.retrieve());
  ATH_CHECK(m_idHelperSvc.retrieve());

  if (m_noReports){
    ATH_MSG_DEBUG("No services were selected for the MDT summary");
  } else {
    ATH_CHECK(m_reportingServices.retrieve());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator pSvc= m_reportingServices.begin();
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator pLastSvc= m_reportingServices.end();
    for (;pSvc not_eq pLastSvc; ++pSvc){
      const std::string& svcName = pSvc->name();
      ATH_MSG_DEBUG("Using "<< svcName);
      if (m_detStore->regFcn(&IMDTConditionsSvc::initInfo,&**pSvc,
                           &MDTCondSummarySvc::update_MDT,this) != StatusCode::SUCCESS){ 
        ATH_MSG_WARNING("Unable to register call back for "<<svcName); 
      } else {
        ATH_MSG_DEBUG("initInfo registered for call-back for "<<svcName);
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
MDTCondSummarySvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IMDTConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IMDTConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


StatusCode MDTCondSummarySvc::update_MDT(IOVSVC_CALLBACK_ARGS){
   ATH_MSG_DEBUG("Register Call Back for MDT System");
   return StatusCode::SUCCESS;
}

StatusCode MDTCondSummarySvc::initInfo(IOVSVC_CALLBACK_ARGS){
   ATH_MSG_DEBUG("Not to be called just dummy");
   return StatusCode::SUCCESS;
}

bool MDTCondSummarySvc::isGoodMultiLayer(const Identifier & Id) const{
  bool result=true;
  // check ID
  Identifier MultilayerId = m_idHelperSvc->mdtIdHelper().multilayerID(Id);
  Identifier ChamberId = m_idHelperSvc->mdtIdHelper().elementID(Id);
  if (not m_noReports){
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq  lastSvc;svc++){
      if ((*svc)->deadStationsId().size()!=0) {
	bool found = std::binary_search((*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),ChamberId,Compare);
	if(found) result= false;
      }
      
      if ((*svc)->deadMultiLayersId().size()!=0 && result==true){
	bool found = std::binary_search((*svc)->deadMultiLayersId().begin(),(*svc)->deadMultiLayersId().end(),MultilayerId,Compare);
	if(found) result= false;
      }else{
	ATH_MSG_DEBUG(" Dead Multilayer from the service  are not availables "<<(*svc));
	
      }
    }
    
  } 
  
  return result;
}



bool MDTCondSummarySvc::isGoodChannel(const Identifier & Id) const{
  bool result=true;   
  Identifier TubeId = Id;
  Identifier MultilayerId = m_idHelperSvc->mdtIdHelper().multilayerID(Id);
  Identifier ChamberId = m_idHelperSvc->mdtIdHelper().elementID(Id);
  if (not m_noReports){
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq  lastSvc;svc++){

      if ((*svc)->deadStationsId().size()!=0) {
	
	bool found = std::binary_search((*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),ChamberId,Compare);
	if(found) result= false;
        ATH_MSG_DEBUG(" Chamber Dropped by DCS or not installed at all");
      }
      
      if ((*svc)->deadMultiLayersId().size()!=0 && result==true){

	bool found = std::binary_search((*svc)->deadMultiLayersId().begin(),(*svc)->deadMultiLayersId().end(),MultilayerId,Compare);
	if(found) result= false;
	ATH_MSG_DEBUG(" MultiLayer Dropped by DCS or not installed at all");
      }	 
      
      if ((*svc)->deadTubesId().size()!=0 && result==true){
	bool found = std::binary_search((*svc)->deadTubesId().begin(),(*svc)->deadTubesId().end(),TubeId,Compare);
	if(found) result= false;
		
	
      }else{
	ATH_MSG_WARNING(" Dead Channel from the service are not availables "<<(*svc));
	result=true;
      }
    }
  }
  return result;
}
  
bool MDTCondSummarySvc::isGood(const Identifier & Id) const{
  bool total_result = true;
//  int counter=0;
  Identifier MultilayerId = m_idHelperSvc->mdtIdHelper().multilayerID(Id);
  Identifier ChamberId = m_idHelperSvc->mdtIdHelper().elementID(Id);
  if (not m_noReports){
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->deadStationsId().size()!=0) 
	{
	  
	  bool found = std::binary_search( 
					  (*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),ChamberId,Compare);
	  if(found) total_result = false;
	    ATH_MSG_WARNING(" Chamber Dropped by DCS or not installed at all");
	  
	}else if ((*svc)->deadMultiLayersId().size()!=0){
	  
	  bool found = std::binary_search( 
					  (*svc)->deadMultiLayersId().begin(),(*svc)->deadMultiLayersId().end(),MultilayerId,Compare);
	  if(found) total_result= false;

	}else total_result = true;
      ATH_MSG_VERBOSE("Thank-you for using the MDTCondSummarySvc, service");
    }   
  }
  
  return total_result;
}

bool MDTCondSummarySvc::isGoodChamber(const Identifier & Id) const{
  bool result=true;
  int counter =0;
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq  lastSvc;svc++){
      ATH_MSG_VERBOSE(" Dead Stations from the service , size= "<<(*svc)->deadStationsId().size());
      if ((*svc)->deadStationsId().size()!=0){

	bool found = std::binary_search( 
  	   (*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),Id,Compare);

	if(found) counter++;
      }
      else{
	ATH_MSG_DEBUG(" Dead Stations from the service  are not availables "<<(*svc));
	
      }
    }
  
    if (counter!=0) result = false; 
    ATH_MSG_WARNING(" Dead Stations from the service  "<< counter);
    
    return result;
  
}



const std::vector<std::string>& MDTCondSummarySvc::deadStations() const{
  //  m_empty.clear();
  if (not m_noReports){
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    
    for (;svc not_eq  lastSvc;svc++){
      if ((*svc)->deadStations().size()!=0){
	return (*svc)->deadStations();
      }else{
	ATH_MSG_WARNING(" Dead Stations from the service  are not availables "<<(*svc));
	
      }
    }
    
  } 
  return m_empty;
}


const std::vector<std::string>& MDTCondSummarySvc::deadTubes() const{
  //  m_empty.clear(); 
  if (not m_noReports){
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->deadTubes().size()!=0){
	return (*svc)->deadTubes();
      }else{

	ATH_MSG_WARNING(" Dead Tubes from the service  are not availables "<<(*svc));
      }
    }     
  }
  return m_empty;  
}


const std::vector<std::string>& MDTCondSummarySvc::deadMultiLayers() const{
  // m_empty.clear(); 
  if (not m_noReports){
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->deadMultiLayers().size()!=0){
	return (*svc)->deadMultiLayers();
      }else {
	ATH_MSG_WARNING(" Dead Stations MDTConditionsSummarySvc ");
	
      }
    }
  }
  return m_empty; 
  }




const std::vector<Identifier>& MDTCondSummarySvc::deadStationsId() const{
  //m_emptyId.clear();
  if (not m_noReports){
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->deadStationsId().size()!=0){
	return (*svc)->deadStationsId();
      }else{
	ATH_MSG_WARNING(" Dead Stations from the service  are not availables "<<(*svc));
      }
    }
    
  }
  return m_emptyId; 
}


const std::vector<Identifier>& MDTCondSummarySvc::deadTubesId() const{
  //m_emptyId.clear();
  if (not m_noReports){
  ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
  for (;svc not_eq lastSvc;++svc){
    if ((*svc)->deadTubesId().size()!=0){
      return (*svc)->deadTubesId();
      } else{ 
	ATH_MSG_WARNING(" Dead Tubes from the service  are not availables "<<(*svc));
      }     
  }
  }
  return m_emptyId;  
}


const std::vector<Identifier>& MDTCondSummarySvc::deadMultiLayersId() const{
  //m_emptyId.clear();
  if (not m_noReports){
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq lastSvc;++svc){  
      if ((*svc)->deadMultiLayersId().size()!=0){
	return (*svc)->deadMultiLayersId();
      }else{
	ATH_MSG_WARNING(" Dead ML from the service  are not availables "<<(*svc));
      }
    }
    
  }
   return m_emptyId;  
}


const std::vector<Identifier>& MDTCondSummarySvc::deadLayersId() const{
  //m_emptyId.clear();
  if (not m_noReports){
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq lastSvc;++svc){  
      if ((*svc)->deadLayersId().size()!=0){
	return (*svc)->deadLayersId();
      }else{
	ATH_MSG_WARNING(" Dead LAYERS from the service  are not availables "<<(*svc));
      }
    }
    
  }
   return m_emptyId;  
}


const std::vector<Identifier>& MDTCondSummarySvc::List_Chambers_with_deadTube() const{
  //m_emptyId.clear();
  if (not m_noReports){
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq lastSvc;++svc){  
      if ((*svc)->List_Chambers_with_deadTube().size()!=0){
	return (*svc)->List_Chambers_with_deadTube();
      }else{
	ATH_MSG_WARNING(" Dead Tube Chambers from the service  are not availables "<<(*svc));
      }
    }
    
  }
   return m_emptyId;  
}

