/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/RPC_DCSConditionsSvc.h"

#include "MuonCondInterface/IRPC_DCSConditionsTool.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/StoreGateSvc.h"

#include <set>
#include <string>
#include <algorithm>

#include "Identifier/Identifier.h"
#include "GaudiKernel/Property.h"

#include "Identifier/IdentifierHash.h"
#include <iostream>

//using namespace std;


RPC_DCSConditionsSvc::RPC_DCSConditionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_condDataTool("RPC_DCSConditionsTool")
{
 
  declareProperty( "DCSInfofromCool",m_dcsInfofromCool=true);
  declareProperty( "RPC_DCSConditionsTool",  m_condDataTool, "RPC DCS Info from COOL");
}

RPC_DCSConditionsSvc::~RPC_DCSConditionsSvc()
{
}

StatusCode RPC_DCSConditionsSvc::initialize()
{
  
  msg(MSG::INFO) << "Initializing " << name() << " - package version " 
		 << PACKAGE_VERSION << endreq;
  
  StoreGateSvc * detStore;
  StatusCode status = service("DetectorStore",detStore);
  if (status.isFailure()) {
    msg(MSG::FATAL) << "DetectorStore service not found !" << endreq; 
  } else {
    msg(MSG::INFO) << "DetectorStore service found !" << endreq; 
    
  }
  if(m_dcsInfofromCool)
    {
      StatusCode sc = m_condDataTool.retrieve();
      if ( sc.isFailure() )
	{
	  
	  msg(MSG::ERROR) << "Could not retrieve RPC_DCSConditionsTool" << endreq;
	}
      else
	{
	  
	  msg(MSG::INFO)<<"RPC_DCSConditionsTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool<<endreq;
	}
      std::vector<std::string> folderNames;
      folderNames.push_back((m_condDataTool)->PanelOffFolderName());
      folderNames.push_back((m_condDataTool)->PanelDeadFolderName());
     
      msg(MSG::INFO)<<"Register call-back  against "<<folderNames.size()<<" folders listed below "<<endreq;
      //   bool aFolderFound = false;
      short ic=0;
      for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
	{
	  ++ic;
	  msg(MSG::INFO)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
	  if (detStore->contains<CondAttrListCollection>(*ifld)) {
	    //    aFolderFound=true;
	    msg(MSG::INFO)<<"     found in the DetStore"<<endreq;
	    const DataHandle<CondAttrListCollection> RPCDCSData;
	    if (detStore->regFcn(&IRPC_DCSConditionsSvc::initInfo,
				 dynamic_cast<IRPC_DCSConditionsSvc *>(this),
				 RPCDCSData,
				 *ifld)!=StatusCode::SUCCESS)
	      {
		msg(MSG::WARNING)<<"Unable to register call back for initDCSInfo against folder <"<<(*ifld)<<">";
	      }
	    else msg(MSG::INFO)<<"initDCSInfo registered for call-back against folder <"<<(*ifld)<<">"<<endreq;
	  }
	  else
	    {   
	      msg(MSG::WARNING)<<"Folder "<<(*ifld)
			       <<" NOT found in the DetStore --- failing to init ???"<<endreq;
	    }
	}
    }
  
  return StatusCode::SUCCESS;
}

StatusCode RPC_DCSConditionsSvc::finalize()
{
  
  msg(MSG::INFO) << "Finalize" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode RPC_DCSConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  msg(MSG::INFO) << "queryInterface Start" << endreq;
  if(IRPC_DCSConditionsSvc::interfaceID().versionMatch(riid) )
    {
      msg(MSG::INFO) << "versionMatch=true" << endreq;
      msg(MSG::INFO) << "OK***************************" << endreq;
      *ppvInterface = this;      
    } else if ( IRPCConditionsSvc::interfaceID().versionMatch(riid) ) {
      *ppvInterface = dynamic_cast<IRPCConditionsSvc*>(this);
      msg(MSG::INFO) << "service cast***************************" << endreq;
    } else {
      msg(MSG::INFO) << "cannot find the interface!***************************" << endreq;
      
      return AthService::queryInterface(riid, ppvInterface);
    }
  msg(MSG::INFO) << "queryInterface succesfull" << endreq;
  addRef(); 
  return StatusCode::SUCCESS;
}



StatusCode RPC_DCSConditionsSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  msg(MSG::INFO)<<"initDCSInfo has been called"<<endreq;
  msg(MSG::INFO)<<"ToolHandle in initMappingModel - <"<<m_condDataTool<<">"<<endreq;
  
  if(m_dcsInfofromCool)
    {
      
      StatusCode sc = m_condDataTool->loadParameters(I, keys);
      if (sc.isFailure())
	{
	  msg(MSG::WARNING)<<"Reading DCS from COOL failed; NO RPC DCS INFO AVAILABLE"<<endreq;
	}
      
    }
  
  return StatusCode::SUCCESS;
}




bool RPC_DCSConditionsSvc::isGoodPanel(const Identifier & /*Id*/) const{
  return true;
}



bool RPC_DCSConditionsSvc::isGoodStrip(const Identifier & /*Id*/) const{
  return true;
}

const std::vector<Identifier>& RPC_DCSConditionsSvc::deadPanelId() const{
   
  
  unsigned int size_new =m_condDataTool->deadPanelId().size();
  
  msg(MSG::VERBOSE)<<"DCS SERVICE: Number of DEAD Panel: "<<size_new <<endreq;
  
  //std::cout<<"DCS SERVICE: Number of DEAD Panel: "<<size_new <<std::endl;
  
  return m_condDataTool->deadPanelId();
  
}
const std::vector<Identifier>& RPC_DCSConditionsSvc::EffPanelId() const{
  
  msg(MSG::VERBOSE)<<"DCS SERVICE: RPC  Eff PANEL NOT AVAILABLE: "<<endreq;
  
  return m_cachedeffPanelId;
}

const std::vector<Identifier>& RPC_DCSConditionsSvc::EffStripId() const{
  
  msg(MSG::VERBOSE)<<"DCS SERVICE: RPC  Eff STRIP NOT AVAILABLE: "<<endreq;
  
  return m_cachedeffStripId;
}


const std::vector<Identifier>& RPC_DCSConditionsSvc::offPanelId() const{
   
  
  unsigned int size_new =m_condDataTool->offPanelId().size();
  
  msg(MSG::VERBOSE)<<"DCS SERVICE: Number of OFF Panel: "<<size_new <<endreq;
  //std::cout<<"DCS SERVICE: Number of OFF Panel: "<<size_new <<std::endl;
  
  
  return m_condDataTool->offPanelId();
  
}



const std::map<Identifier,double>& RPC_DCSConditionsSvc::RPC_EfficiencyMap(){
static  std::map<Identifier ,double> RPCCondSummarySvc_RPC_PanelEfficiencyMap_empty;
 RPCCondSummarySvc_RPC_PanelEfficiencyMap_empty.clear(); 
  msg(MSG::VERBOSE)<<"Efficiency Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelEfficiencyMap_empty;
}
const std::map<Identifier,double>& RPC_DCSConditionsSvc::RPC_EfficiencyGapMap(){
static  std::map<Identifier ,double> RPCCondSummarySvc_RPC_GapEfficiencyMap_empty;
 
  RPCCondSummarySvc_RPC_GapEfficiencyMap_empty.clear(); 
  msg(MSG::VERBOSE)<<"EfficiencyGap Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_GapEfficiencyMap_empty;
}


const std::map<Identifier,double>& RPC_DCSConditionsSvc::RPC_MeanClusterSizeMap(){
 static  std::map<Identifier ,double> RPCCondSummarySvc_RPC_PanelMeanClusterSizeMap_empty;

  RPCCondSummarySvc_RPC_PanelMeanClusterSizeMap_empty.clear();
  msg(MSG::VERBOSE)<<"MeanClusterSize Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelMeanClusterSizeMap_empty;
}


const std::map<Identifier,double>& RPC_DCSConditionsSvc::RPC_FracClusterSize1Map(){
static  std::map<Identifier ,double> RPCCondSummarySvc_RPC_PanelFracClusterSize1Map_empty;
 
  RPCCondSummarySvc_RPC_PanelFracClusterSize1Map_empty.clear(); 
  msg(MSG::VERBOSE)<<"FracClusterSize1 Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelFracClusterSize1Map_empty;
}


const std::map<Identifier,double>& RPC_DCSConditionsSvc::RPC_FracClusterSize2Map(){
static  std::map<Identifier ,double> RPCCondSummarySvc_RPC_PanelFracClusterSize2Map_empty;
 
  RPCCondSummarySvc_RPC_PanelFracClusterSize2Map_empty.clear();
  msg(MSG::VERBOSE)<<"FracClusterSize2 Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelFracClusterSize2Map_empty;
}

const std::map<Identifier,double>& RPC_DCSConditionsSvc::RPC_FracClusterSize3Map(){
static  std::map<Identifier ,double> RPCCondSummarySvc_RPC_PanelFracClusterSize3Map_empty;
 
  RPCCondSummarySvc_RPC_PanelFracClusterSize3Map_empty.clear();
  msg(MSG::VERBOSE)<<"FracClusterSize3 Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelFracClusterSize3Map_empty;
}

const std::map<Identifier,std::string>& RPC_DCSConditionsSvc::RPC_DeadStripListMap(){
 static  std::map<Identifier ,std::string> RPCCondSummarySvc_RPC_PanelDeadStripListMap_empty;

  RPCCondSummarySvc_RPC_PanelDeadStripListMap_empty.clear();
  msg(MSG::VERBOSE)<<"DeadStripList Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelDeadStripListMap_empty;
}


const std::map<Identifier,float>& RPC_DCSConditionsSvc::RPC_FracDeadStripMap(){
static  std::map<Identifier ,float> RPCCondSummarySvc_RPC_PanelFracDeadStripMap_empty;

  RPCCondSummarySvc_RPC_PanelFracDeadStripMap_empty.clear(); 
  msg(MSG::VERBOSE)<<"FracDeadStrip Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelFracDeadStripMap_empty;
}

const std::map<Identifier,int>& RPC_DCSConditionsSvc::RPC_ProjectedTracksMap(){
static  std::map<Identifier ,int> RPCCondSummarySvc_RPC_PanelProjectedTracksMap_empty;

  RPCCondSummarySvc_RPC_PanelProjectedTracksMap_empty.clear(); 
  msg(MSG::VERBOSE)<<"ProjectedTracks Map per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelProjectedTracksMap_empty;
}


const std::map<Identifier,int>& RPC_DCSConditionsSvc::RPC_DeadStripList(){
static  std::map<Identifier ,int> RPCCondSummarySvc_RPC_PanelDeadStripList_empty;

  RPCCondSummarySvc_RPC_PanelDeadStripList_empty.clear(); 
  msg(MSG::VERBOSE)<<"DeadStripList per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_PanelDeadStripList_empty;
}

const std::map<Identifier,std::vector<double> >& RPC_DCSConditionsSvc::RPC_TimeMapforStrip(){
static  std::map<Identifier ,std::vector<double> > RPCCondSummarySvc_RPC_StripTimeMap_empty;

  RPCCondSummarySvc_RPC_StripTimeMap_empty.clear(); 
  msg(MSG::VERBOSE)<<"StripTimeMap per RPC panel"<<endreq;
 
  return RPCCondSummarySvc_RPC_StripTimeMap_empty;
}
