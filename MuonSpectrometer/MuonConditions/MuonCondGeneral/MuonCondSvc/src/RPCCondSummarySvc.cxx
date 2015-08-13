/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
//#include <iterator>

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

#include "Identifier/IdentifierHash.h"
//Muon
#include "MuonIdHelpers/RpcIdHelper.h"
//#include "MuonCondInterface/IRpcDetectorStatusDbTool.h"
#include "MuonCondInterface/IRPCConditionsSvc.h"

/////////////////////////////////////////////////////////
#include "MuonCondSvc/RPCCondSummarySvc.h"
#include "Identifier/Identifier.h"




using namespace std;
// Constructor
RPCCondSummarySvc::RPCCondSummarySvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_reportingServices(name),
  m_pHelper(0),
  m_detStore("DetectorStore",name),
  m_rpc_StatusSvc("RPC_STATUSConditionsSvc",name),
  m_rpc_DCSSvc("RPC_DCSConditionsSvc",name),
  m_noReports(true){
  m_reportingServices.push_back("RPC_STATUSConditionsSvc");
  //m_reportingServices.push_back("RPC_DCSConditionsSvc");
  declareProperty("ConditionsServices",m_reportingServices);
  declareProperty("UseSimulation",m_usesimulation=false);
}

//Initialize
StatusCode 
RPCCondSummarySvc::initialize(){
  StatusCode sc(StatusCode::SUCCESS);
  //
  msg(MSG::INFO)<<"*************Initialize"<<endreq;
  m_noReports = m_reportingServices.empty();
  
 
  StoreGateSvc * detStore;
  StatusCode status = service("DetectorStore",detStore);
  if (status.isFailure()) {
    msg(MSG::FATAL) << "DetectorStore service not found !" << endreq;
  } else {
    msg(MSG::INFO) << "DetectorStore service found !" << endreq;
    
  }  
 
 sc = m_detStore->retrieve(m_pHelper, "RPCIDHELPER" );
  if (sc.isFailure()){
     msg(MSG::FATAL) << " Cannot retrieve RpcIdHelper " << endreq;
     return sc;
  }


  if(m_usesimulation){
    msg(MSG::INFO) << "Load RPC_DCSService by hand for simulation maps !" << endreq;
    StatusCode sc(StatusCode::SUCCESS);
  
    if (StatusCode::SUCCESS not_eq sc) {
      msg(MSG::ERROR)<<"Could not retrieve the summary service"<<endreq;
    }
    sc = m_rpc_StatusSvc.retrieve();
  }


  if (m_noReports){
    sc=StatusCode::SUCCESS;
    msg(MSG::INFO)<<"No services were selected for the RPC summary"<<endreq;
  } else {
    sc = m_reportingServices.retrieve();
    if ( sc.isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve " << m_reportingServices << endreq;
      return StatusCode::FAILURE;
    }
    
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator pSvc= m_reportingServices.begin();
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator pLastSvc= m_reportingServices.end();
    for (;pSvc not_eq pLastSvc; ++pSvc){
      const std::string& svcName = pSvc->name();
      msg(MSG::INFO)<<"Using "<< svcName << endreq;
      if (m_detStore->regFcn(&IRPCConditionsSvc::initInfo,&**pSvc,
                           &RPCCondSummarySvc::update_RPC,this) != StatusCode::SUCCESS){ 
        msg(MSG::WARNING)<<"Unable to register call back for "<<svcName<<endreq; 
       
      } else {
        msg(MSG::INFO)<<"initInfo registered for call-back for "<<svcName<<endreq;
        //std::cout << " initInfo registered for call-back for"<< svcName << std::endl;
      }
    }
    
  }   
  
  
  return sc;
}

//Finalize
StatusCode
RPCCondSummarySvc::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  //Code
  return sc;
}

StatusCode 
RPCCondSummarySvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IRPCConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IRPCConditionsSvc*>(this);
  } else {
 
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


StatusCode RPCCondSummarySvc::update_RPC(IOVSVC_CALLBACK_ARGS){
 msg(MSG::INFO)<<"Register Call Back for RPC System"<<endreq;

   return StatusCode::SUCCESS;
}

StatusCode RPCCondSummarySvc::initInfo(IOVSVC_CALLBACK_ARGS){
  msg(MSG::INFO)<<"Not to be called just dummy"<<endreq;

   return StatusCode::SUCCESS;
}



bool RPCCondSummarySvc::isGoodPanel(const Identifier & Id) const{
  
  bool result=true;
  int counter =0;
  Identifier PanelId = m_pHelper->panelID(Id);
  //Identifier PanelId = Id;
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
  for (;svc not_eq  lastSvc;svc++){
    msg(MSG::INFO)<<" Eff Panels from the service , size= "<<(*svc)->EffPanelId().size()<<endreq;
    if ((*svc)->EffPanelId().size()!=0){
      bool found = std::binary_search((*svc)->EffPanelId().begin(),(*svc)->EffPanelId().end(),PanelId,Compare);
      if(found) counter++;
    }
    else{
      msg(MSG::INFO)<<" Eff Panels from the service  are not available "<<(*svc) <<endreq;
    }
  }
  
  if (counter!=0) result = false; 
  msg(MSG::INFO)<<" Eff. Panels from the service  "<< counter <<endreq;
  
  return result;
  
}

bool RPCCondSummarySvc::isGoodStrip(const Identifier & Id) const{
  
  bool result=true;   
  Identifier StripId = Id;
  Identifier PanelId = m_pHelper->panelID(Id);
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
  if (not m_noReports){
    for (;svc not_eq  lastSvc;svc++){
      if ((*svc)->EffPanelId().size()!=0) {
	bool found = std::binary_search((*svc)->EffPanelId().begin(),(*svc)->EffPanelId().end(),PanelId,Compare);
	if(found) result= false;
      }
      if ((*svc)->EffStripId().size()!=0 && result==true){
	bool found = std::binary_search((*svc)->EffStripId().begin(),(*svc)->EffStripId().end(),StripId,Compare);
	if(found) result= false;
      }else{
	msg(MSG::INFO)<<" Dead Multilayer from the service  are not availables "<<(*svc) <<endreq;
      }
    } 
  }   
  
  return result;
}

const std::vector<Identifier>& RPCCondSummarySvc::EffPanelId() const{
  // emptyId.clear();
  if (not m_noReports){
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq lastSvc;++svc){
      if((*svc)->EffPanelId().size()) return (*svc)->EffPanelId();
      else msg(MSG::DEBUG)<<" Eff Panel from the service  are not availables "<<(*svc) <<endreq;
    }
    msg(MSG::INFO)<<" Efficiency per Panel RPC  RPCConditionsSummarySvc "<<endreq;
     
  }
   return emptyId;  
}



const std::vector<Identifier>& RPCCondSummarySvc::EffStripId() const{
  // emptyId.clear();
  if (not m_noReports){
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    
    for (;svc not_eq lastSvc;++svc){
      if((*svc)->EffStripId().size()) return (*svc)->EffStripId();
	else msg(MSG::DEBUG)<<" Eff Strip from the service  are not availables "<<(*svc) <<endreq;
    }
    msg(MSG::INFO)<<" Efficiency per Strip RPC  RPCConditionsSummarySvc "<<endreq;
     
  }
   return emptyId;  
}

const std::vector<Identifier>& RPCCondSummarySvc::deadPanelId() const{
  
  if (not m_noReports){
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq lastSvc;++svc){ 
      if((*svc)->deadPanelId().size())return (*svc)->deadPanelId();
      else msg(MSG::DEBUG)<<" DeadPanel from the service  are not availables "<<(*svc) <<endreq;
    }
    msg(MSG::INFO)<<" dead panel RPC  RPCConditionsSummarySvc "<<endreq;
     
  }
   return emptyId;  
}

const std::vector<Identifier>& RPCCondSummarySvc::offPanelId() const{
  
  if (not m_noReports){
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

    for (;svc not_eq lastSvc;++svc){
   
      if((*svc)->offPanelId().size())return (*svc)->offPanelId();
      else msg(MSG::DEBUG)<<" OffPanel from the service  are not availables "<<(*svc) <<endreq;
    }
    msg(MSG::INFO)<<" off panel RPC  RPCConditionsSummarySvc "<<endreq;
  }
  return emptyId;  
}


// only for simulation

const std::map<Identifier,double>& RPCCondSummarySvc::RPC_EfficiencyMap(){
static  std::map<Identifier ,double> RPCCondSummarySvc_RPC_PanelEfficiencyMap_empty;
  RPCCondSummarySvc_RPC_PanelEfficiencyMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
    //std::cout <<"Efficiency per Panel RPC  RPCConditionsSummarySvc  "<<(*svc)->RPC_EfficiencyMap().size() <<std::endl;
    if((*svc)->RPC_EfficiencyMap().size()) return (*svc)->RPC_EfficiencyMap();
    else msg(MSG::DEBUG)<<"RPC_EfficiencyMap from the service  are not availables "<<(*svc) <<endreq;
  }
  msg(MSG::INFO)<<" Efficiency per Panel RPC  RPCConditionsSummarySvc "<<endreq;
  msg(MSG::VERBOSE)<<"Efficiency Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelEfficiencyMap_empty;
}


const std::map<Identifier,double>& RPCCondSummarySvc::RPC_EfficiencyGapMap(){
static  std::map<Identifier ,double> RPCCondSummarySvc_RPC_GapEfficiencyMap_empty;
 
  RPCCondSummarySvc_RPC_GapEfficiencyMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_EfficiencyGapMap().size()) return (*svc)->RPC_EfficiencyGapMap();
    else msg(MSG::DEBUG)<<"RPC_EfficiencyGapMap from the service  are not availables "<<(*svc) <<endreq;
  }
  msg(MSG::INFO)<<" GapEfficiency per  RPC  RPCConditionsSummarySvc "<<endreq;
  msg(MSG::VERBOSE)<<"GapEfficiency Map per RPC "<<endreq;
 
  return RPCCondSummarySvc_RPC_GapEfficiencyMap_empty;
}

const std::map<Identifier,double>& RPCCondSummarySvc::RPC_MeanClusterSizeMap(){
static  std::map<Identifier ,double> RPCCondSummarySvc_RPC_PanelMeanClusterSizeMap_empty;

  RPCCondSummarySvc_RPC_PanelMeanClusterSizeMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_MeanClusterSizeMap().size()) return (*svc)->RPC_MeanClusterSizeMap();
    else msg(MSG::DEBUG)<<"RPC_MeanClusterSizeMap from the service  are not availables "<<(*svc) <<endreq;
  }
  msg(MSG::INFO)<<" MeanClusterSize per Panel RPC  RPCConditionsSummarySvc "<<endreq;
 
  msg(MSG::VERBOSE)<<"MeanClusterSize Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelMeanClusterSizeMap_empty;
}

const std::map<Identifier,double>& RPCCondSummarySvc::RPC_FracClusterSize1Map(){
static  std::map<Identifier ,double> RPCCondSummarySvc_RPC_PanelFracClusterSize1Map_empty;
 
  RPCCondSummarySvc_RPC_PanelFracClusterSize1Map_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_FracClusterSize1Map().size()) return (*svc)->RPC_FracClusterSize1Map();
    else msg(MSG::DEBUG)<<"RPC_FracClusterSize1Map from the service  are not availables "<<(*svc) <<endreq;
  }
  msg(MSG::INFO)<<" FracClusterSize1 per Panel RPC  RPCConditionsSummarySvc "<<endreq;
  msg(MSG::VERBOSE)<<"FracClusterSize1 Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelFracClusterSize1Map_empty;
}

const std::map<Identifier,double>& RPCCondSummarySvc::RPC_FracClusterSize2Map(){
static  std::map<Identifier ,double> RPCCondSummarySvc_RPC_PanelFracClusterSize2Map_empty;
 
  RPCCondSummarySvc_RPC_PanelFracClusterSize2Map_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_FracClusterSize2Map().size()) return (*svc)->RPC_FracClusterSize2Map();
    else msg(MSG::DEBUG)<<"RPC_FracClusterSize2Map from the service  are not availables "<<(*svc) <<endreq;
  }
  msg(MSG::INFO)<<" FracClusterSize2 per Panel RPC  RPCConditionsSummarySvc "<<endreq;
  msg(MSG::VERBOSE)<<"FracClusterSize2 Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelFracClusterSize2Map_empty;
}

const std::map<Identifier,double>& RPCCondSummarySvc::RPC_FracClusterSize3Map(){
static  std::map<Identifier ,double> RPCCondSummarySvc_RPC_PanelFracClusterSize3Map_empty;
 
  RPCCondSummarySvc_RPC_PanelFracClusterSize3Map_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_FracClusterSize3Map().size()) return (*svc)->RPC_FracClusterSize3Map();
    else msg(MSG::DEBUG)<<"RPC_FracClusterSize3Map from the service  are not availables "<<(*svc) <<endreq;
  }
  msg(MSG::INFO)<<" FracClusterSize3 per Panel RPC  RPCConditionsSummarySvc "<<endreq;
  msg(MSG::VERBOSE)<<"FracClusterSize3 Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelFracClusterSize3Map_empty;
}

const std::map<Identifier,std::string>& RPCCondSummarySvc::RPC_DeadStripListMap(){
static  std::map<Identifier ,std::string> RPCCondSummarySvc_RPC_PanelDeadStripListMap_empty;

  RPCCondSummarySvc_RPC_PanelDeadStripListMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_DeadStripListMap().size()) return (*svc)->RPC_DeadStripListMap();
    else msg(MSG::DEBUG)<<"RPC_DeadStripListMap from the service  are not availables "<<(*svc) <<endreq;
  }
  msg(MSG::INFO)<<" DeadStripList per Panel RPC  RPCConditionsSummarySvc "<<endreq;
  msg(MSG::VERBOSE)<<"DeadStripList Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelDeadStripListMap_empty;
}

const std::map<Identifier,float>& RPCCondSummarySvc::RPC_FracDeadStripMap(){
static  std::map<Identifier ,float> RPCCondSummarySvc_RPC_PanelFracDeadStripMap_empty;

  RPCCondSummarySvc_RPC_PanelFracDeadStripMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_FracDeadStripMap().size()) return (*svc)->RPC_FracDeadStripMap();
    else msg(MSG::DEBUG)<<"RPC_FracDeadStripMap from the service  are not availables "<<(*svc) <<endreq;
  }
  msg(MSG::INFO)<<" FracDeadStrip per Panel RPC  RPCConditionsSummarySvc "<<endreq;
  msg(MSG::VERBOSE)<<"FracDeadStrip Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelFracDeadStripMap_empty;
}

const std::map<Identifier,int>& RPCCondSummarySvc::RPC_ProjectedTracksMap(){
static  std::map<Identifier ,int> RPCCondSummarySvc_RPC_PanelProjectedTracksMap_empty;

  RPCCondSummarySvc_RPC_PanelProjectedTracksMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
     if((*svc)->RPC_ProjectedTracksMap().size()) return (*svc)->RPC_ProjectedTracksMap();
     else msg(MSG::DEBUG)<<"RPC_ProjectedTracksMap from the service  are not availables "<<(*svc) <<endreq;
  }
  msg(MSG::INFO)<<" ProjectedTracks per Panel RPC  RPCConditionsSummarySvc "<<endreq;
  msg(MSG::VERBOSE)<<"ProjectedTracks Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelProjectedTracksMap_empty;
}

const std::map<Identifier,int>& RPCCondSummarySvc::RPC_DeadStripList(){
static  std::map<Identifier ,int> RPCCondSummarySvc_RPC_PanelDeadStripList_empty;

  RPCCondSummarySvc_RPC_PanelDeadStripList_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_DeadStripList().size()) return (*svc)->RPC_DeadStripList();
    else msg(MSG::DEBUG)<<"RPC_DeadStripList from the service  are not availables "<<(*svc) <<endreq;
  }
  msg(MSG::INFO)<<"DeadStripList per Panel RPC  RPCConditionsSummarySvc "<<endreq;
  msg(MSG::VERBOSE)<<"DeadStripList Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelDeadStripList_empty;
} 

const std::map<Identifier,std::vector<double> >& RPCCondSummarySvc::RPC_TimeMapforStrip(){
static  std::map<Identifier ,std::vector<double> > RPCCondSummarySvc_RPC_StripTimeMap_empty;

  RPCCondSummarySvc_RPC_StripTimeMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
  for (;svc not_eq lastSvc;++svc){
     if((*svc)->RPC_TimeMapforStrip().size()) return (*svc)->RPC_TimeMapforStrip();
     else msg(MSG::DEBUG)<<"RPC_TimeMapforStrip from the service  are not availables "<<(*svc) <<endreq; 
  }
  msg(MSG::INFO)<<" Strip Time per Panel RPC  RPCConditionsSummarySvc "<<endreq;
  msg(MSG::VERBOSE)<<"Strip Time Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_StripTimeMap_empty;
}



