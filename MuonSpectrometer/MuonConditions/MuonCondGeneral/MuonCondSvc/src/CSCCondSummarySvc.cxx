/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/CSCCondSummarySvc.h"
//STL includes
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include <iterator>
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
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonCondInterface/ICSCConditionsSvc.h"

#include "Identifier/Identifier.h"


using namespace std;

// Constructor
CSCCondSummarySvc::CSCCondSummarySvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_reportingServices(name),
  m_cscHelper(0),
  m_detStore("DetectorStore",name),
  m_noReports(true){
  //  m_reportingServices.push_back("CSC_DCSConditionsSvc");
  declareProperty("ConditionsServices",m_reportingServices);
  declareProperty("DetStore", m_detStore);
  declareProperty("UseSimulation",m_usesimulation=false);
}


StatusCode
CSCCondSummarySvc::initialize(){
  StatusCode sc(StatusCode::FAILURE);
  m_noReports = m_reportingServices.empty();
 
  sc = m_detStore.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "DetectorStore service not found !" << endmsg;
    return sc;
  } else {
    msg(MSG::INFO) << "DetectorStore service found !" << endmsg;
  } 
 
  sc = m_detStore->retrieve(m_cscHelper, "CSCIDHELPER" );
  if (sc.isFailure())
    {
      msg(MSG::FATAL) << " Cannot retrieve CscIdHelper " << endmsg;
      return sc;
    }

  if(m_usesimulation) {
    
    m_reportingServices.empty(); 
    msg(MSG::INFO) << "Simulation setup: No Conditions Data for CSC !" << endmsg;
 
  } else msg(MSG::INFO) << "Data DCS Conditions CSC !" << endmsg;
  
  if (m_noReports){
    sc=StatusCode::SUCCESS;
    msg(MSG::INFO)<<"No services were selected for the CSC summary"<<endmsg;
  } else {
    sc = m_reportingServices.retrieve();
    if ( sc.isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve " << m_reportingServices << endmsg;
      return StatusCode::FAILURE;
    }
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator pSvc= m_reportingServices.begin();
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator pLastSvc= m_reportingServices.end();
     for (;pSvc not_eq pLastSvc; ++pSvc){
   
       const std::string& svcName = pSvc->name();
      msg(MSG::INFO)<<"Using "<< svcName << endmsg;
      if (m_detStore->regFcn(&ICSCConditionsSvc::initInfo,&**pSvc,
                           &CSCCondSummarySvc::update_CSC,this) != StatusCode::SUCCESS){ 
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
CSCCondSummarySvc::finalize(){
  msg(MSG::INFO)<<"Thank-you for using the CSCCondSummarySvc, version "<<PACKAGE_VERSION<<endmsg;
  //Code
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

  msg(MSG::INFO)<<"Register Call Back for CSC System"<<endmsg;

   return StatusCode::SUCCESS;
}

StatusCode CSCCondSummarySvc::initInfo(IOVSVC_CALLBACK_ARGS){

  msg(MSG::INFO)<<"Not to be called just dummy"<<endmsg;
   return StatusCode::SUCCESS;
}


bool CSCCondSummarySvc::isGoodWireLayer(const Identifier & Id) const{
  bool result=true;
  // check ID
  
  Identifier ChamberId = m_cscHelper->elementID(Id);
  
  // Identifier WireLayerId = m_cscIdHelper->channelID(ChamberId, 1, wirelayer,1,1);
     
  if (not m_noReports){
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
    for (;svc not_eq  lastSvc;svc++){
      if ((*svc)->deadStationsId().size()!=0) {
	bool found = std::binary_search((*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),ChamberId,Compare);
	if(found) result= false;
      }
      
    //   if ((*svc)->deadWireLayersId().size()!=0 && result==true){
//  	bool found = std::binary_search((*svc)->deadWireLayersId().begin(),(*svc)->deadWireLayersId().end(),WirelayerId,Compare);
//  	if(found) result= false;
//       }
//       else{
// 	msg(MSG::INFO)<<" Dead Wirelayer from the service  are not availables "<<(*svc) <<endmsg;
	
//       }
    }
    
  } 
  
  return result;
}




bool CSCCondSummarySvc::isGood(const Identifier & Id) const{
  bool total_result = true;
//  int counter=0;
//  Identifier WirelayerId = m_cscHelper->multilayerID(Id);
  Identifier ChamberId = m_cscHelper->elementID(Id);
  if (not m_noReports){
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
   
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->deadStationsId().size()!=0) 
	{
	  msg(MSG::VERBOSE)<<" dentro chamber dropped "<<endmsg; 
	  bool found = std::binary_search( 
					  (*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),ChamberId,Compare);
	  if(found) total_result = false;
	  msg(MSG::VERBOSE)<<" Chamber Dropped by DCS or not installed at all "<<endmsg;
	  
// 	}else if ((*svc)->deadWireLayersId().size()!=0){
	
// 	bool found = std::binary_search( 
// 					(*svc)->deadWireLayersId().begin(),(*svc)->deadWireLayersId().end(),WirelayerId,Compare);
// 	if(found) total_result= false;
// 	msg(MSG::VERBOSE)<<" Chamber with WireLayer Dropped  "<<endmsg;
	
	}else total_result = true;
      msg(MSG::VERBOSE)<<"Thank-you for using the CSCCondSummarySvc,  service "<<endmsg;
    }   
  }

  return total_result;
  
}

bool CSCCondSummarySvc::isGoodChamber(const Identifier & Id) const{
  bool result=true;
  int counter =0;
  //Identifier chamberId = m_cscHelper->elementID(Id);
   ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
    for (;svc not_eq  lastSvc;svc++){
      msg(MSG::INFO)<<" Dead Stations from the service , size= "<<(*svc)->deadStationsId().size()<<endmsg;
      if ((*svc)->deadStationsId().size()!=0){

	bool found = std::binary_search( 
  	   (*svc)->deadStationsId().begin(),(*svc)->deadStationsId().end(),Id,Compare);

	if(found) counter++;
      }
      else{
	msg(MSG::INFO)<<" Dead Stations from the service  are not availables "<<(*svc) <<endmsg;
	
      }
    }
  
    if (counter!=0) result = false; 
    msg(MSG::INFO)<<" Dead Stations from the service  "<< counter <<endmsg;
    
    return result;
  
}






const std::vector<Identifier>& CSCCondSummarySvc::deadStationsId() const{
  //m_emptyId.clear();
  if (not m_noReports){
   ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
    
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->deadStationsId().size()!=0){
	return (*svc)->deadStationsId();
      }else{
	msg(MSG::INFO)<<" Dead Stations from the service  are not availables "<<(*svc) <<endmsg;
      }
    }
    
  }
  return m_emptyId; 
}



const std::vector<Identifier>& CSCCondSummarySvc::deadWireLayersId() const{
  //m_emptyId.clear();
  if (not m_noReports){
     ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
     ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
   
    for (;svc not_eq lastSvc;++svc){  
      if ((*svc)->deadWireLayersId().size()!=0){
	return (*svc)->deadWireLayersId();
      }else{
	msg(MSG::INFO)<<" Dead Wire from the service  are not availables "<<(*svc) <<endmsg;
      }
    }
    
  }
   return m_emptyId;  
}


const std::vector<std::string>& CSCCondSummarySvc::deadStations() const{
  //m_emptyId.clear();
  if (not m_noReports){
   ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
    ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
    
    for (;svc not_eq lastSvc;++svc){
      if ((*svc)->deadStations().size()!=0){
	return (*svc)->deadStations();
      }else{
	msg(MSG::INFO)<<" Dead Stations from the service  are not availables "<<(*svc) <<endmsg;
      }
    }
    
  }
  return m_empty; 
}



const std::vector<std::string>& CSCCondSummarySvc::deadWireLayers() const{
  //m_emptyId.clear();
  if (not m_noReports){
     ServiceHandleArray<ICSCConditionsSvc>::const_iterator svc= m_reportingServices.begin();
     ServiceHandleArray<ICSCConditionsSvc>::const_iterator lastSvc= m_reportingServices.end();
   
    for (;svc not_eq lastSvc;++svc){  
      if ((*svc)->deadWireLayers().size()!=0){
	return (*svc)->deadWireLayers();
      }else{
	msg(MSG::INFO)<<" Dead Wire from the service  are not availables "<<(*svc) <<endmsg;
      }
    }
    
  }
   return m_empty;  
}
