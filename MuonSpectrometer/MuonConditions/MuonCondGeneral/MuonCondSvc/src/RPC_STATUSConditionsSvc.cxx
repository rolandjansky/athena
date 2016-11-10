/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/RPC_STATUSConditionsSvc.h"

#include "MuonCondInterface/IRpcDetectorStatusDbTool.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
//#include "PathResolver/PathResolver.h"
#include "StoreGate/StoreGateSvc.h"

#include <set>
#include <string>
#include <algorithm>

#include "Identifier/Identifier.h"
#include "GaudiKernel/Property.h"

#include "Identifier/IdentifierHash.h"
#include <iostream>




RPC_STATUSConditionsSvc::RPC_STATUSConditionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_condDataTool("RpcDetectorStatusDbTool"),
  m_RPC_PanelEfficiency(0)
{
 
  //  declareProperty( "DCSInfofromCool",m_dcsInfofromCool=true);
  declareProperty( "RpcDetectorStatusDbTool",  m_condDataTool, "RPC Info from COOL");
}

RPC_STATUSConditionsSvc::~RPC_STATUSConditionsSvc()
{
}

StatusCode RPC_STATUSConditionsSvc::initialize()
{
  
  msg(MSG::INFO) << "Initializing " << name() << " - package version " 
		 << PACKAGE_VERSION << endmsg;
  
  StoreGateSvc * detStore;
  StatusCode status = service("DetectorStore",detStore);
  if (status.isFailure()) {
    msg(MSG::FATAL) << "DetectorStore service not found !" << endmsg; 
  } else {
    msg(MSG::INFO) << "DetectorStore service found !" << endmsg; 
    
  }
  //if(m_dcsInfofromCool)
  //{
  StatusCode sc = m_condDataTool.retrieve();
  if ( sc.isFailure() )
    {
      
      msg(MSG::ERROR) << "Could not retrieve RPC_STATUSConditionsTool" << endmsg;
    }
  else
    {
      
      msg(MSG::INFO)<<"RPC_STATUSConditionsTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool<<endmsg;
    }
  
  
  
  std::vector<std::string> folderNames;
  folderNames.push_back((m_condDataTool)->FolderName());
   
  msg(MSG::INFO)<<"Register call-back  against "<<folderNames.size()<<" folders listed below "<<endmsg;
  //bool aFolderFound = false;
  short ic=0;
  for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
    {
      ++ic;
      msg(MSG::INFO)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
      if (detStore->contains<CondAttrListCollection>(*ifld)) {
	//aFolderFound=true;
	msg(MSG::INFO)<<"     found in the DetStore"<<endmsg;
	const DataHandle<CondAttrListCollection> RPCData;
	if (detStore->regFcn(&IRPC_STATUSConditionsSvc::initInfo,
			     dynamic_cast<IRPC_STATUSConditionsSvc *>(this),
			     RPCData,
			     *ifld)!=StatusCode::SUCCESS)
	  {
	    msg(MSG::WARNING)<<"Unable to register call back for initDCSInfo against folder <"<<(*ifld)<<">";
	    //return StatusCode::FAILURE;
	  }
	    else msg(MSG::INFO)<<"initDCSInfo registered for call-back against folder <"<<(*ifld)<<">"<<endmsg;
      }
      else
	{   
	  msg(MSG::WARNING)<<"Folder "<<(*ifld)
			   <<" NOT found in the DetStore --- failing to init ???"<<endmsg;
	  //	      break;
	}
    }
  
  
  return StatusCode::SUCCESS;
}

StatusCode RPC_STATUSConditionsSvc::finalize()
{

     msg(MSG::INFO) << "Finalize" << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode RPC_STATUSConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  msg(MSG::INFO) << "queryInterface Start" << endmsg;
  if(IRPC_STATUSConditionsSvc::interfaceID().versionMatch(riid) )
    {
      msg(MSG::INFO) << "versionMatch=true" << endmsg;
      msg(MSG::INFO) << "OK***************************" << endmsg;
      *ppvInterface = this;      
    } else if ( IRPCConditionsSvc::interfaceID().versionMatch(riid) ) {
      *ppvInterface = dynamic_cast<IRPCConditionsSvc*>(this);
      msg(MSG::INFO) << "service cast***************************" << endmsg;
    } else {
      msg(MSG::INFO) << "cannot find the interface!***************************" << endmsg;
     
      return AthService::queryInterface(riid, ppvInterface);
    }
  msg(MSG::INFO) << "queryInterface succesfull" << endmsg;
  addRef(); 
  return StatusCode::SUCCESS;
}


	
StatusCode RPC_STATUSConditionsSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  msg(MSG::INFO)<<"initDCSInfo has been called"<<endmsg;
  msg(MSG::INFO)<<"ToolHandle in initMappingModel - <"<<m_condDataTool<<">"<<endmsg;
   
  //  if(m_dcsInfofromCool)
  // {
      
      StatusCode sc = m_condDataTool->loadParameterStatus(I, keys);
      if (sc.isFailure())
	{
	  msg(MSG::WARNING)<<"Reading DCS from COOL failed; NO INFO AVAILABLE"<<endmsg;
	}
      
      // }
    
  return StatusCode::SUCCESS;
}


bool RPC_STATUSConditionsSvc::isGoodPanel(const Identifier & /*Id*/) const{

  return true;
}



bool RPC_STATUSConditionsSvc::isGoodStrip(const Identifier & /*Id*/) const{
  
  return true;
}


const std::vector<Identifier>& RPC_STATUSConditionsSvc::EffPanelId() const{
  
   
  msg(MSG::VERBOSE)<<"SERVICE: Number of Panel: "<<endmsg;
  
  return m_condDataTool->EffPanelId();
}


const std::vector<Identifier>& RPC_STATUSConditionsSvc::EffStripId() const{
  
  msg(MSG::VERBOSE)<<"Eff Strip RPC "<<endmsg;
  
  return m_condDataTool->EffStripId();
}

const std::vector<Identifier>& RPC_STATUSConditionsSvc::offPanelId() const{
   
  msg(MSG::VERBOSE)<<"DCS SERVICE: RPC  off PANEL NOT AVAILABLE: "<<endmsg;
  
  return m_cachedoffPanelId;
 
  
}

const std::vector<Identifier>& RPC_STATUSConditionsSvc::deadPanelId() const{
   
  msg(MSG::VERBOSE)<<"DCS SERVICE: RPC  dead PANEL NOT AVAILABLE: "<<endmsg;
  
  return m_cacheddeadPanelId;
 
  
}
const std::map<Identifier,double>& RPC_STATUSConditionsSvc::RPC_EfficiencyMap(){
  
  msg(MSG::VERBOSE)<<"Efficiency Map per RPC panel"<<endmsg;
  //std::cout<<"Efficiency Map per RPC panel: "<<m_condDataTool->RPC_EfficiencyMap().size() <<std::endl;
  return m_condDataTool->RPC_EfficiencyMap();
}
const std::map<Identifier,double>& RPC_STATUSConditionsSvc::RPC_EfficiencyGapMap(){
  
  msg(MSG::VERBOSE)<<"EfficiencyGap Map per RPC panel"<<endmsg;
  
  return m_condDataTool->RPC_EfficiencyGapMap();
}


const std::map<Identifier,double>& RPC_STATUSConditionsSvc::RPC_MeanClusterSizeMap(){
  
  msg(MSG::VERBOSE)<<"MeanClusterSize Map per RPC panel"<<endmsg;
  
  return m_condDataTool->RPC_MeanClusterSizeMap();
}


const std::map<Identifier,double>& RPC_STATUSConditionsSvc::RPC_FracClusterSize1Map(){
  
  msg(MSG::VERBOSE)<<"FracClusterSize1 Map per RPC panel"<<endmsg;
  
  return m_condDataTool->RPC_FracClusterSize1Map();
}


const std::map<Identifier,double>& RPC_STATUSConditionsSvc::RPC_FracClusterSize2Map(){
  
  msg(MSG::VERBOSE)<<"FracClusterSize2 Map per RPC panel"<<endmsg;
  
  return m_condDataTool->RPC_FracClusterSize2Map();
}

const std::map<Identifier,double>& RPC_STATUSConditionsSvc::RPC_FracClusterSize3Map(){
  
  msg(MSG::VERBOSE)<<"FracClusterSize3 Map per RPC panel"<<endmsg;
  
  return m_condDataTool->RPC_FracClusterSize3Map();
}

const std::map<Identifier,std::string>& RPC_STATUSConditionsSvc::RPC_DeadStripListMap(){
  
  msg(MSG::VERBOSE)<<"DeadStripList Map per RPC panel"<<endmsg;
  
  return m_condDataTool->RPC_DeadStripListMap();
}


const std::map<Identifier,float>& RPC_STATUSConditionsSvc::RPC_FracDeadStripMap(){
  
  msg(MSG::VERBOSE)<<"FracDeadStrip Map per RPC panel"<<endmsg;
  
  return m_condDataTool->RPC_FracDeadStripMap();
}

const std::map<Identifier,int>& RPC_STATUSConditionsSvc::RPC_ProjectedTracksMap(){
  
  msg(MSG::VERBOSE)<<"ProjectedTracks Map per RPC panel"<<endmsg;
  
  return m_condDataTool->RPC_ProjectedTracksMap();
}


const std::map<Identifier,int>& RPC_STATUSConditionsSvc::RPC_DeadStripList(){
  
  msg(MSG::VERBOSE)<<"DeadStripList per RPC panel"<<endmsg;
  
  return m_condDataTool->RPC_DeadStripList();
}


const std::map<Identifier,std::vector<double> >& RPC_STATUSConditionsSvc::RPC_TimeMapforStrip(){
  
  msg(MSG::VERBOSE)<<"DeadStripList per RPC panel"<<endmsg;
  
  return m_condDataTool->RPC_TimeMapforStrip();
}
