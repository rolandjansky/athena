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
#include "Gaudi/Property.h"

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
  
  ATH_MSG_INFO( "Initializing " << name() << " - package version " 
                << PACKAGE_VERSION  );
  
  StoreGateSvc * detStore;
  StatusCode status = service("DetectorStore",detStore);
  if (status.isFailure()) {
    ATH_MSG_FATAL( "DetectorStore service not found !"  );
  } else {
    ATH_MSG_VERBOSE( "DetectorStore service found !"  );
    
  }
  //if(m_dcsInfofromCool)
  //{
  StatusCode sc = m_condDataTool.retrieve();
  if ( sc.isFailure() )
    {
      
      ATH_MSG_ERROR( "Could not retrieve RPC_STATUSConditionsTool"  );
    }
  else
    {
      
      ATH_MSG_VERBOSE("RPC_STATUSConditionsTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool );
    }
  
  
  
  std::vector<std::string> folderNames;
  folderNames.push_back((m_condDataTool)->FolderName());
   
  ATH_MSG_VERBOSE("Register call-back  against "<<folderNames.size()<<" folders listed below " );
  //bool aFolderFound = false;
  short ic=0;
  for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
    {
      ++ic;
      ATH_MSG_VERBOSE(" Folder n. "<<ic<<" <"<<(*ifld)<<">" );
      if (detStore->contains<CondAttrListCollection>(*ifld)) {
	//aFolderFound=true;
	ATH_MSG_VERBOSE("     found in the DetStore" );
	const DataHandle<CondAttrListCollection> RPCData;
	if (detStore->regFcn(&IRPC_STATUSConditionsSvc::initInfo,
			     dynamic_cast<IRPC_STATUSConditionsSvc *>(this),
			     RPCData,
			     *ifld)!=StatusCode::SUCCESS)
	  {
	    ATH_MSG_WARNING("Unable to register call back for initDCSInfo against folder <"<<(*ifld)<<">" );
	    //return StatusCode::FAILURE;
	  }
        else ATH_MSG_VERBOSE("initDCSInfo registered for call-back against folder <"<<(*ifld)<<">" );
      }
      else
	{   
	  ATH_MSG_WARNING("Folder "<<(*ifld)
                          <<" NOT found in the DetStore --- failing to init ???" );
	  //	      break;
	}
    }
  
  
  return StatusCode::SUCCESS;
}

StatusCode RPC_STATUSConditionsSvc::finalize()
{

  ATH_MSG_VERBOSE( "Finalize"  );
  return StatusCode::SUCCESS;
}


StatusCode RPC_STATUSConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if(IRPC_STATUSConditionsSvc::interfaceID().versionMatch(riid) )
    {
      *ppvInterface = this;      
    } else if ( IRPCConditionsSvc::interfaceID().versionMatch(riid) ) {
      *ppvInterface = dynamic_cast<IRPCConditionsSvc*>(this);
    } else {
     
      return AthService::queryInterface(riid, ppvInterface);
    }
  addRef(); 
  return StatusCode::SUCCESS;
}


	
StatusCode RPC_STATUSConditionsSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  ATH_MSG_VERBOSE("initDCSInfo has been called" );
  ATH_MSG_VERBOSE("ToolHandle in initMappingModel - <"<<m_condDataTool<<">" );
   
  //  if(m_dcsInfofromCool)
  // {
      
      StatusCode sc = m_condDataTool->loadParameterStatus(I, keys);
      if (sc.isFailure())
	{
	  ATH_MSG_WARNING("Reading DCS from COOL failed; NO INFO AVAILABLE" );
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
  
   
  ATH_MSG_VERBOSE("SERVICE: Number of Panel: " );
  
  return m_condDataTool->EffPanelId();
}


const std::vector<Identifier>& RPC_STATUSConditionsSvc::EffStripId() const{
  
  ATH_MSG_VERBOSE("Eff Strip RPC " );
  
  return m_condDataTool->EffStripId();
}

const std::vector<Identifier>& RPC_STATUSConditionsSvc::offPanelId() const{
   
  ATH_MSG_VERBOSE("DCS SERVICE: RPC  off PANEL NOT AVAILABLE: " );
  
  return m_cachedoffPanelId;
 
  
}

const std::vector<Identifier>& RPC_STATUSConditionsSvc::deadPanelId() const{
   
  ATH_MSG_VERBOSE("DCS SERVICE: RPC  dead PANEL NOT AVAILABLE: " );
  
  return m_cacheddeadPanelId;
 
  
}
const std::map<Identifier,double>& RPC_STATUSConditionsSvc::RPC_EfficiencyMap(){
  
  ATH_MSG_VERBOSE("Efficiency Map per RPC panel" );
  //std::cout<<"Efficiency Map per RPC panel: "<<m_condDataTool->RPC_EfficiencyMap().size() <<std::endl;
  return m_condDataTool->RPC_EfficiencyMap();
}
const std::map<Identifier,double>& RPC_STATUSConditionsSvc::RPC_EfficiencyGapMap(){
  
  ATH_MSG_VERBOSE("EfficiencyGap Map per RPC panel" );
  
  return m_condDataTool->RPC_EfficiencyGapMap();
}


const std::map<Identifier,double>& RPC_STATUSConditionsSvc::RPC_MeanClusterSizeMap(){
  
  ATH_MSG_VERBOSE("MeanClusterSize Map per RPC panel" );
  
  return m_condDataTool->RPC_MeanClusterSizeMap();
}


const std::map<Identifier,double>& RPC_STATUSConditionsSvc::RPC_FracClusterSize1Map(){
  
  ATH_MSG_VERBOSE("FracClusterSize1 Map per RPC panel" );
  
  return m_condDataTool->RPC_FracClusterSize1Map();
}


const std::map<Identifier,double>& RPC_STATUSConditionsSvc::RPC_FracClusterSize2Map(){
  
  ATH_MSG_VERBOSE("FracClusterSize2 Map per RPC panel" );
  
  return m_condDataTool->RPC_FracClusterSize2Map();
}

const std::map<Identifier,double>& RPC_STATUSConditionsSvc::RPC_FracClusterSize3Map(){
  
  ATH_MSG_VERBOSE("FracClusterSize3 Map per RPC panel" );
  
  return m_condDataTool->RPC_FracClusterSize3Map();
}

const std::map<Identifier,std::string>& RPC_STATUSConditionsSvc::RPC_DeadStripListMap(){
  
  ATH_MSG_VERBOSE("DeadStripList Map per RPC panel" );
  
  return m_condDataTool->RPC_DeadStripListMap();
}


const std::map<Identifier,float>& RPC_STATUSConditionsSvc::RPC_FracDeadStripMap(){
  
  ATH_MSG_VERBOSE("FracDeadStrip Map per RPC panel" );
  
  return m_condDataTool->RPC_FracDeadStripMap();
}

const std::map<Identifier,int>& RPC_STATUSConditionsSvc::RPC_ProjectedTracksMap(){
  
  ATH_MSG_VERBOSE("ProjectedTracks Map per RPC panel" );
  
  return m_condDataTool->RPC_ProjectedTracksMap();
}


const std::map<Identifier,int>& RPC_STATUSConditionsSvc::RPC_DeadStripList(){
  
  ATH_MSG_VERBOSE("DeadStripList per RPC panel" );
  
  return m_condDataTool->RPC_DeadStripList();
}


const std::map<Identifier,std::vector<double> >& RPC_STATUSConditionsSvc::RPC_TimeMapforStrip(){
  
  ATH_MSG_VERBOSE("DeadStripList per RPC panel" );
  
  return m_condDataTool->RPC_TimeMapforStrip();
}
