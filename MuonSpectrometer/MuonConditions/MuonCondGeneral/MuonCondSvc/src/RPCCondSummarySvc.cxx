/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/RPCCondSummarySvc.h"

#include <vector>
#include <list>
#include <algorithm>
#include <sstream>

RPCCondSummarySvc::RPCCondSummarySvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_reportingServices(name),
  m_detStore("DetectorStore",name),
  m_rpc_StatusSvc("RPC_STATUSConditionsSvc",name),
  m_rpc_DCSSvc("RPC_DCSConditionsSvc",name),
  m_noReports(true){
  m_reportingServices.push_back("RPC_STATUSConditionsSvc");
  declareProperty("ConditionsServices",m_reportingServices);
  declareProperty("UseSimulation",m_usesimulation=false);
}

//Initialize
StatusCode 
RPCCondSummarySvc::initialize(){
  m_noReports = m_reportingServices.empty();
  ATH_CHECK(m_detStore.retrieve());
  ATH_CHECK(m_idHelperSvc.retrieve());
  if(m_usesimulation){
    ATH_MSG_DEBUG("Load RPC_DCSService by hand for simulation maps !");
    ATH_CHECK(m_rpc_StatusSvc.retrieve());
  }

  if (m_noReports){
    ATH_MSG_DEBUG("No services were selected for the RPC summary");
  } else {
    ATH_CHECK(m_reportingServices.retrieve());

    ServiceHandleArray<IRPCConditionsSvc>::const_iterator pSvc= m_reportingServices.begin();
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator pLastSvc= m_reportingServices.end();
    for (;pSvc not_eq pLastSvc; ++pSvc){
      const std::string& svcName = pSvc->name();
      ATH_MSG_DEBUG("Using "<< svcName);
      if (m_detStore->regFcn(&IRPCConditionsSvc::initInfo,&**pSvc,
                           &RPCCondSummarySvc::update_RPC,this) != StatusCode::SUCCESS){ 
        ATH_MSG_WARNING("Unable to register call back for "<<svcName); 
       
      } else {
        ATH_MSG_DEBUG("initInfo registered for call-back for "<<svcName);
      }
    }
    
  }
  return StatusCode::SUCCESS;
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
  ATH_MSG_DEBUG("Register Call Back for RPC System");
  return StatusCode::SUCCESS;
}

StatusCode RPCCondSummarySvc::initInfo(IOVSVC_CALLBACK_ARGS){
  ATH_MSG_DEBUG("Not to be called just dummy");
  return StatusCode::SUCCESS;
}



bool RPCCondSummarySvc::isGoodPanel(const Identifier & Id) const{
  
  bool result=true;
  int counter =0;
  Identifier PanelId = m_idHelperSvc->rpcIdHelper().panelID(Id);
  //Identifier PanelId = Id;
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
  for (;svc not_eq  lastSvc;svc++){
    ATH_MSG_DEBUG(" Eff Panels from the service, size= "<<(*svc)->EffPanelId().size());
    if ((*svc)->EffPanelId().size()!=0){
      bool found = std::binary_search((*svc)->EffPanelId().begin(),(*svc)->EffPanelId().end(),PanelId,Compare);
      if(found) counter++;
    }
    else{
      ATH_MSG_DEBUG(" Eff Panels from the service  are not available "<<(*svc));
    }
  }
  
  if (counter!=0) result = false; 
  ATH_MSG_DEBUG(" Eff. Panels from the service  "<< counter);
  
  return result;
  
}

bool RPCCondSummarySvc::isGoodStrip(const Identifier & Id) const{
  
  bool result=true;   
  Identifier StripId = Id;
  Identifier PanelId = m_idHelperSvc->rpcIdHelper().panelID(Id);
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
	ATH_MSG_DEBUG(" Dead Multilayer from the service  are not availables "<<(*svc));
      }
    } 
  }   
  
  return result;
}

const std::vector<Identifier>& RPCCondSummarySvc::EffPanelId() const{
  // m_emptyId.clear();
  if (not m_noReports){
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq lastSvc;++svc){
      if((*svc)->EffPanelId().size()) return (*svc)->EffPanelId();
      else ATH_MSG_DEBUG(" Eff Panel from the service  are not availables "<<(*svc));
    }
    ATH_MSG_DEBUG(" Efficiency per Panel RPC  RPCConditionsSummarySvc ");
     
  }
   return m_emptyId;  
}



const std::vector<Identifier>& RPCCondSummarySvc::EffStripId() const{
  // m_emptyId.clear();
  if (not m_noReports){
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    
    for (;svc not_eq lastSvc;++svc){
      if((*svc)->EffStripId().size()) return (*svc)->EffStripId();
	else ATH_MSG_DEBUG(" Eff Strip from the service  are not availables "<<(*svc));
    }
    ATH_MSG_DEBUG(" Efficiency per Strip RPC  RPCConditionsSummarySvc ");
     
  }
   return m_emptyId;  
}

const std::vector<Identifier>& RPCCondSummarySvc::deadPanelId() const{
  
  if (not m_noReports){
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
    for (;svc not_eq lastSvc;++svc){ 
      if((*svc)->deadPanelId().size())return (*svc)->deadPanelId();
      else ATH_MSG_DEBUG(" DeadPanel from the service  are not availables "<<(*svc));
    }
    ATH_MSG_DEBUG(" dead panel RPC  RPCConditionsSummarySvc ");
     
  }
   return m_emptyId;  
}

const std::vector<Identifier>& RPCCondSummarySvc::offPanelId() const{
  
  if (not m_noReports){
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
    ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

    for (;svc not_eq lastSvc;++svc){
   
      if((*svc)->offPanelId().size())return (*svc)->offPanelId();
      else ATH_MSG_DEBUG(" OffPanel from the service  are not availables "<<(*svc));
    }
    ATH_MSG_DEBUG(" off panel RPC  RPCConditionsSummarySvc ");
  }
  return m_emptyId;  
}


// only for simulation

const std::map<Identifier,double>& RPCCondSummarySvc::RPC_EfficiencyMap(){
static  std::map<Identifier ,double> s_RPCCondSummarySvc_RPC_PanelEfficiencyMap_empty;
  s_RPCCondSummarySvc_RPC_PanelEfficiencyMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
    //std::cout <<"Efficiency per Panel RPC  RPCConditionsSummarySvc  "<<(*svc)->RPC_EfficiencyMap().size() <<std::endl;
    if((*svc)->RPC_EfficiencyMap().size()) return (*svc)->RPC_EfficiencyMap();
    else ATH_MSG_DEBUG("RPC_EfficiencyMap from the service  are not availables "<<(*svc));
  }
  ATH_MSG_DEBUG(" Efficiency per Panel RPC  RPCConditionsSummarySvc ");
  ATH_MSG_DEBUG("Efficiency Map per RPC panel");
  return s_RPCCondSummarySvc_RPC_PanelEfficiencyMap_empty;
}


const std::map<Identifier,double>& RPCCondSummarySvc::RPC_EfficiencyGapMap(){
static  std::map<Identifier ,double> s_RPCCondSummarySvc_RPC_GapEfficiencyMap_empty;
 
  s_RPCCondSummarySvc_RPC_GapEfficiencyMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_EfficiencyGapMap().size()) return (*svc)->RPC_EfficiencyGapMap();
    else ATH_MSG_DEBUG("RPC_EfficiencyGapMap from the service  are not availables "<<(*svc));
  }
  ATH_MSG_DEBUG(" GapEfficiency per  RPC  RPCConditionsSummarySvc ");
  ATH_MSG_DEBUG("GapEfficiency Map per RPC ");
  return s_RPCCondSummarySvc_RPC_GapEfficiencyMap_empty;
}

const std::map<Identifier,double>& RPCCondSummarySvc::RPC_MeanClusterSizeMap(){
static  std::map<Identifier ,double> s_RPCCondSummarySvc_RPC_PanelMeanClusterSizeMap_empty;

  s_RPCCondSummarySvc_RPC_PanelMeanClusterSizeMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_MeanClusterSizeMap().size()) return (*svc)->RPC_MeanClusterSizeMap();
    else ATH_MSG_DEBUG("RPC_MeanClusterSizeMap from the service  are not availables "<<(*svc));
  }
  ATH_MSG_DEBUG(" MeanClusterSize per Panel RPC  RPCConditionsSummarySvc ");
  ATH_MSG_DEBUG("MeanClusterSize Map per RPC panel");
  return s_RPCCondSummarySvc_RPC_PanelMeanClusterSizeMap_empty;
}

const std::map<Identifier,double>& RPCCondSummarySvc::RPC_FracClusterSize1Map(){
static  std::map<Identifier ,double> s_RPCCondSummarySvc_RPC_PanelFracClusterSize1Map_empty;
 
  s_RPCCondSummarySvc_RPC_PanelFracClusterSize1Map_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_FracClusterSize1Map().size()) return (*svc)->RPC_FracClusterSize1Map();
    else ATH_MSG_DEBUG("RPC_FracClusterSize1Map from the service  are not availables "<<(*svc));
  }
  ATH_MSG_DEBUG("FracClusterSize1 per Panel RPC  RPCConditionsSummarySvc ");
  ATH_MSG_DEBUG("FracClusterSize1 Map per RPC panel");
  return s_RPCCondSummarySvc_RPC_PanelFracClusterSize1Map_empty;
}

const std::map<Identifier,double>& RPCCondSummarySvc::RPC_FracClusterSize2Map(){
static  std::map<Identifier ,double> s_RPCCondSummarySvc_RPC_PanelFracClusterSize2Map_empty;
 
  s_RPCCondSummarySvc_RPC_PanelFracClusterSize2Map_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_FracClusterSize2Map().size()) return (*svc)->RPC_FracClusterSize2Map();
    else ATH_MSG_DEBUG("RPC_FracClusterSize2Map from the service  are not availables "<<(*svc));
  }
  ATH_MSG_DEBUG("FracClusterSize2 per Panel RPC  RPCConditionsSummarySvc ");
  ATH_MSG_DEBUG("FracClusterSize2 Map per RPC panel");
  return s_RPCCondSummarySvc_RPC_PanelFracClusterSize2Map_empty;
}

const std::map<Identifier,double>& RPCCondSummarySvc::RPC_FracClusterSize3Map(){
static  std::map<Identifier ,double> s_RPCCondSummarySvc_RPC_PanelFracClusterSize3Map_empty;
 
  s_RPCCondSummarySvc_RPC_PanelFracClusterSize3Map_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_FracClusterSize3Map().size()) return (*svc)->RPC_FracClusterSize3Map();
    else ATH_MSG_DEBUG("RPC_FracClusterSize3Map from the service  are not availables "<<(*svc));
  }
  ATH_MSG_DEBUG("FracClusterSize3 per Panel RPC  RPCConditionsSummarySvc ");
  ATH_MSG_DEBUG("FracClusterSize3 Map per RPC panel");
  return s_RPCCondSummarySvc_RPC_PanelFracClusterSize3Map_empty;
}

const std::map<Identifier,std::string>& RPCCondSummarySvc::RPC_DeadStripListMap(){
static  std::map<Identifier ,std::string> s_RPCCondSummarySvc_RPC_PanelDeadStripListMap_empty;

  s_RPCCondSummarySvc_RPC_PanelDeadStripListMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_DeadStripListMap().size()) return (*svc)->RPC_DeadStripListMap();
    else ATH_MSG_DEBUG("RPC_DeadStripListMap from the service  are not availables "<<(*svc));
  }
  ATH_MSG_DEBUG("DeadStripList per Panel RPC  RPCConditionsSummarySvc");
  ATH_MSG_DEBUG("DeadStripList Map per RPC panel");
  return s_RPCCondSummarySvc_RPC_PanelDeadStripListMap_empty;
}

const std::map<Identifier,float>& RPCCondSummarySvc::RPC_FracDeadStripMap(){
static  std::map<Identifier ,float> s_RPCCondSummarySvc_RPC_PanelFracDeadStripMap_empty;

  s_RPCCondSummarySvc_RPC_PanelFracDeadStripMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_FracDeadStripMap().size()) return (*svc)->RPC_FracDeadStripMap();
    else ATH_MSG_DEBUG("RPC_FracDeadStripMap from the service  are not availables "<<(*svc));
  }
  ATH_MSG_DEBUG("FracDeadStrip per Panel RPC  RPCConditionsSummarySvc");
  ATH_MSG_DEBUG("FracDeadStrip Map per RPC panel");
  return s_RPCCondSummarySvc_RPC_PanelFracDeadStripMap_empty;
}

const std::map<Identifier,int>& RPCCondSummarySvc::RPC_ProjectedTracksMap(){
static  std::map<Identifier ,int> s_RPCCondSummarySvc_RPC_PanelProjectedTracksMap_empty;

  s_RPCCondSummarySvc_RPC_PanelProjectedTracksMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());

  for (;svc not_eq lastSvc;++svc){
     if((*svc)->RPC_ProjectedTracksMap().size()) return (*svc)->RPC_ProjectedTracksMap();
     else ATH_MSG_DEBUG("RPC_ProjectedTracksMap from the service are not availables "<<(*svc));
  }
  ATH_MSG_DEBUG("ProjectedTracks per Panel RPC  RPCConditionsSummarySvc");
  ATH_MSG_DEBUG("ProjectedTracks Map per RPC panel");
  return s_RPCCondSummarySvc_RPC_PanelProjectedTracksMap_empty;
}

const std::map<Identifier,int>& RPCCondSummarySvc::RPC_DeadStripList(){
static  std::map<Identifier ,int> s_RPCCondSummarySvc_RPC_PanelDeadStripList_empty;

  s_RPCCondSummarySvc_RPC_PanelDeadStripList_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
  for (;svc not_eq lastSvc;++svc){
    if((*svc)->RPC_DeadStripList().size()) return (*svc)->RPC_DeadStripList();
    else ATH_MSG_DEBUG("RPC_DeadStripList from the service  are not availables "<<(*svc));
  }
  ATH_MSG_DEBUG("DeadStripList per Panel RPC  RPCConditionsSummarySvc");
  ATH_MSG_DEBUG("DeadStripList Map per RPC panel");
  return s_RPCCondSummarySvc_RPC_PanelDeadStripList_empty;
} 

const std::map<Identifier,std::vector<double> >& RPCCondSummarySvc::RPC_TimeMapforStrip(){
static  std::map<Identifier ,std::vector<double> > s_RPCCondSummarySvc_RPC_StripTimeMap_empty;

  s_RPCCondSummarySvc_RPC_StripTimeMap_empty.clear();
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator svc(m_reportingServices.begin());
  ServiceHandleArray<IRPCConditionsSvc>::const_iterator lastSvc(m_reportingServices.end());
  for (;svc not_eq lastSvc;++svc){
     if((*svc)->RPC_TimeMapforStrip().size()) return (*svc)->RPC_TimeMapforStrip();
     else ATH_MSG_DEBUG("RPC_TimeMapforStrip from the service  are not availables "<<(*svc)); 
  }
  ATH_MSG_DEBUG(" Strip Time per Panel RPC  RPCConditionsSummarySvc ");
  ATH_MSG_DEBUG("Strip Time Map per RPC panel");
  return s_RPCCondSummarySvc_RPC_StripTimeMap_empty;
}



