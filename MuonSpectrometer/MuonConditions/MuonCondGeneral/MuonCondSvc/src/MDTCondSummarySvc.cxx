/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/MDTCondSummarySvc.h"
//STL includes
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"
//Athena
#include "Identifier/IdentifierHash.h"
//Muon
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonCondInterface/IMDTConditionsSvc.h"

#include "Identifier/Identifier.h"



using namespace std;

// Constructor
MDTCondSummarySvc::MDTCondSummarySvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_reportingServices(name),m_pHelper(0),
  m_detStore("DetectorStore",name),m_noReports(true){
  // default services
  //  m_reportingServices.push_back("MDT_DCSConditionsSvc");
  
  declareProperty("ConditionsServices",m_reportingServices);
  declareProperty("DetStore", m_detStore);
}


StatusCode
MDTCondSummarySvc::initialize(){
  StatusCode sc(StatusCode::FAILURE);
  m_noReports = m_reportingServices.empty();

  sc = m_detStore.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "DetectorStore service not found !" << endmsg;
    return sc;
  } else {
    msg(MSG::INFO) << "DetectorStore service found !" << endmsg;
  }  
  
  sc = m_detStore->retrieve(m_pHelper, "MDTIDHELPER" );
  if (sc.isFailure())
    {
      msg(MSG::FATAL) << " Cannot retrieve MdtIdHelper " << endmsg;
      return StatusCode::FAILURE;
    }

  
  if (m_noReports){
    sc=StatusCode::SUCCESS;
    msg(MSG::INFO)<<"No services were selected for the MDT summary"<<endmsg;
  } else {
    sc = m_reportingServices.retrieve();
    if ( sc.isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve " << m_reportingServices << endmsg;
      return StatusCode::FAILURE;
    }
    
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator pSvc= m_reportingServices.begin();
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator pLastSvc= m_reportingServices.end();
    for (;pSvc not_eq pLastSvc; ++pSvc){
      const std::string& svcName = pSvc->name();
      msg(MSG::INFO)<<"Using "<< svcName << endmsg;
      if (m_detStore->regFcn(&IMDTConditionsSvc::initInfo,&**pSvc,
                           &MDTCondSummarySvc::update_MDT,this) != StatusCode::SUCCESS){ 
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
MDTCondSummarySvc::finalize(){
  msg(MSG::INFO)<<"Thank-you for using the MDTCondSummarySvc, version "<<PACKAGE_VERSION<<endmsg;
  //Code
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

  msg(MSG::DEBUG)<<"Register Call Back for MDT System"<<endmsg;

   return StatusCode::SUCCESS;
}

StatusCode MDTCondSummarySvc::initInfo(IOVSVC_CALLBACK_ARGS){

  msg(MSG::INFO)<<"Not to be called just dummy"<<endmsg;
   return StatusCode::SUCCESS;
}




bool MDTCondSummarySvc::isGoodMultiLayer(const Identifier & Id) const{
  bool result=true;
  // check ID
  Identifier MultilayerId = m_pHelper->multilayerID(Id);
  Identifier ChamberId = m_pHelper->elementID(Id);
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
	msg(MSG::DEBUG)<<" Dead Multilayer from the service  are not availables "<<(*svc) <<endmsg;
	
      }
    }
    
  } 
  
  return result;
}



bool MDTCondSummarySvc::isGoodChannel(const Identifier & Id) const{
  bool result=true;   
  Identifier TubeId = Id;
  Identifier MultilayerId = m_pHelper->multilayerID(Id);
  Identifier ChamberId = m_pHelper->elementID(Id);
  if (not m_noReports){
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq  lastSvc;svc++){

      if ((*svc)->deadStationsId().size()!=0) {
	
	bool found = std::binary_search((*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),ChamberId,Compare);
	if(found) result= false;
        msg(MSG::DEBUG)<<" Chamber Dropped by DCS or not installed at all "<<endmsg;
      }
      
      if ((*svc)->deadMultiLayersId().size()!=0 && result==true){

	bool found = std::binary_search((*svc)->deadMultiLayersId().begin(),(*svc)->deadMultiLayersId().end(),MultilayerId,Compare);
	if(found) result= false;
	msg(MSG::DEBUG)<<" MultiLayer Dropped by DCS or not installed at all "<<endmsg;
      }	 
      
      if ((*svc)->deadTubesId().size()!=0 && result==true){
	bool found = std::binary_search((*svc)->deadTubesId().begin(),(*svc)->deadTubesId().end(),TubeId,Compare);
	if(found) result= false;
		
	
      }else{
	msg(MSG::DEBUG)<<" Dead Channel from the service  are not availables "<<(*svc) <<endmsg;
	result=true;
      }
    }
  }
  return result;
}
  
bool MDTCondSummarySvc::isGood(const Identifier & Id) const{
  bool total_result = true;
//  int counter=0;
  Identifier MultilayerId = m_pHelper->multilayerID(Id);
  Identifier ChamberId = m_pHelper->elementID(Id);
  if (not m_noReports){
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->deadStationsId().size()!=0) 
	{
	  
	  bool found = std::binary_search( 
					  (*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),ChamberId,Compare);
	  if(found) total_result = false;
	    msg(MSG::VERBOSE)<<" Chamber Dropped by DCS or not installed at all "<<endmsg;
	  
	}else if ((*svc)->deadMultiLayersId().size()!=0){
	  
	  bool found = std::binary_search( 
					  (*svc)->deadMultiLayersId().begin(),(*svc)->deadMultiLayersId().end(),MultilayerId,Compare);
	  if(found) total_result= false;
	  
	  //} else if ((*svc)->deadTubesId().size()!=0){
	  //int size = (*svc)->deadTubesId().size();
	  //bool found = std::binary_search( 
	  //				  (*svc)->List_Chambers_with_deadTube().begin(),(*svc)->List_Chambers_with_deadTube().end(),ChamberId,Compare);
	  //if(found) total_result= false;
          //msg(MSG::INFO)<<" Chamber with Tubes dead "<<endmsg;	  
	  
	}else total_result = true;
      msg(MSG::VERBOSE)<<"Thank-you for using the MDTCondSummarySvc,  service "<<endmsg;
    }   
  }
  
  return total_result;
}

bool MDTCondSummarySvc::isGoodChamber(const Identifier & Id) const{
  bool result=true;
  int counter =0;
  // Identifier chamberId = m_pHelper->elementID(Id);
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IMDTConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq  lastSvc;svc++){
      msg(MSG::VERBOSE)<<" Dead Stations from the service , size= "<<(*svc)->deadStationsId().size()<<endmsg;
      if ((*svc)->deadStationsId().size()!=0){

	bool found = std::binary_search( 
  	   (*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),Id,Compare);

	if(found) counter++;
      }
      else{
	msg(MSG::DEBUG)<<" Dead Stations from the service  are not availables "<<(*svc) <<endmsg;
	
      }
    }
  
    if (counter!=0) result = false; 
    msg(MSG::VERBOSE)<<" Dead Stations from the service  "<< counter <<endmsg;
    
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
	msg(MSG::VERBOSE)<<" Dead Stations from the service  are not availables "<<(*svc) <<endmsg;
	
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

	msg(MSG::VERBOSE)<<" Dead Tubes from the service  are not availables "<<(*svc) <<endmsg;
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
	msg(MSG::VERBOSE)<<" Dead Stations MDTConditionsSummarySvc "<<endmsg;
	
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
	msg(MSG::VERBOSE)<<" Dead Stations from the service  are not availables "<<(*svc) <<endmsg;
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
	msg(MSG::VERBOSE)<<" Dead Tubes from the service  are not availables "<<(*svc) <<endmsg;
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
	msg(MSG::VERBOSE)<<" Dead ML from the service  are not availables "<<(*svc) <<endmsg;
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
	msg(MSG::VERBOSE)<<" Dead LAYERS from the service  are not availables "<<(*svc) <<endmsg;
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
	msg(MSG::VERBOSE)<<" Dead Tube Chambers from the service  are not availables "<<(*svc) <<endmsg;
      }
    }
    
  }
   return m_emptyId;  
}

