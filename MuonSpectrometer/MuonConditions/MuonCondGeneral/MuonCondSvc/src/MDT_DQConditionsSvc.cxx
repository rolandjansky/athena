/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/MDT_DQConditionsSvc.h"
#include "MuonCondSvc/MuonHierarchy.h"
#include "MuonCondInterface/IMDT_DQConditionsTool.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
//#include "PathResolver/PathResolver.h"
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/Property.h"

#include "Identifier/IdentifierHash.h"
#include <iostream>
#include <set>
#include <string>



MDT_DQConditionsSvc::MDT_DQConditionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_condDataTool("MDT_DQConditionsTool")
{
 
  declareProperty( "DQInfofromCool",m_dqInfofromCool=true);
  declareProperty( "MDT_DQConditionsTool",  m_condDataTool, "MDT DQ Info from COOL");
}

MDT_DQConditionsSvc::~MDT_DQConditionsSvc()
{
}

StatusCode MDT_DQConditionsSvc::initialize()
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
  if(m_dqInfofromCool)
    {
      StatusCode sc = m_condDataTool.retrieve();
      if ( sc.isFailure() )
	{
	  
	  msg(MSG::ERROR) << "Could not retrieve MDT_DQConditionsTool" << endmsg;
	}
      else
	{
	  
	  msg(MSG::INFO)<<"MDT_DQConditionsTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool<<endmsg;
	}
            
      std::vector<std::string> folderNames;
      folderNames.push_back((m_condDataTool)->DeadFolderName());
      folderNames.push_back((m_condDataTool)->NoisyFolderName());
      
      
      msg(MSG::INFO)<<"Register call-back  against "<<folderNames.size()<<" folders listed below "<<endmsg;
      //  bool aFolderFound = false;
      short ic=0;
      for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
	{
	  ++ic;
	  msg(MSG::INFO)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
	  if (detStore->contains<CondAttrListCollection>(*ifld)) {
	    //    aFolderFound=true;
	    msg(MSG::INFO)<<"     found in the DetStore"<<endmsg;
	    const DataHandle<CondAttrListCollection> MDTDQData;
	    if (detStore->regFcn(&IMDT_DQConditionsSvc::initInfo,
				 dynamic_cast<IMDT_DQConditionsSvc *>(this),
				 MDTDQData,
				 *ifld)!=StatusCode::SUCCESS)
	      {
		msg(MSG::WARNING)<<"Unable to register call back for initDQInfo against folder <"<<(*ifld)<<">";
	      }
	    else msg(MSG::INFO)<<"initDQInfo registered for call-back against folder <"<<(*ifld)<<">"<<endmsg;
	  }
	  else
	    {   
	      msg(MSG::WARNING)<<"Folder "<<(*ifld)
			       <<" NOT found in the DetStore --- failing to init ???"<<endmsg;
	    }
	}
    }
  
  return StatusCode::SUCCESS;
}

StatusCode MDT_DQConditionsSvc::finalize()
{
  
  
  return StatusCode::SUCCESS;
}


StatusCode MDT_DQConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  msg(MSG::INFO) << "queryInterface Start" << endmsg;
  if(IMDT_DQConditionsSvc::interfaceID().versionMatch(riid) )
    {
      msg(MSG::INFO) << "versionMatch=true" << endmsg;
      *ppvIF = this;
    }else if ( IMDTConditionsSvc::interfaceID().versionMatch(riid) ){
      *ppvIF = dynamic_cast<IMDTConditionsSvc*>(this);
      msg(MSG::INFO) << "service cast***************************" << endmsg;
      
    } else {
      msg(MSG::INFO) << "cannot find the interface!" << endmsg;
      return AthService::queryInterface(riid, ppvIF);
    }
  msg(MSG::INFO) << "queryInterface succesfull" << endmsg;
  //addRef();  // is this needed ??
  return StatusCode::SUCCESS;
}


	
StatusCode MDT_DQConditionsSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  msg(MSG::INFO)<<"initDQInfo has been called"<<endmsg;
  msg(MSG::INFO)<<"ToolHandle in init DQ Info - <"<<m_condDataTool<<">"<<endmsg;
   
  if(m_dqInfofromCool)
    {
      
      StatusCode sc = m_condDataTool->loadParameters(I, keys);
      if (sc.isFailure())
	{
	  msg(MSG::WARNING)<<"Reading DQ from COOL failed; NO MDT DQ INFO AVAILABLE"<<endmsg;
	}
    }
    
  return StatusCode::SUCCESS;
}


const std::vector<std::string>& MDT_DQConditionsSvc::deadTubes() const{

  return m_cachedDeadTubes;
}


const std::vector<std::string>& MDT_DQConditionsSvc::deadStations() const{
   
  msg(MSG::DEBUG)<<"CONFIGURATION SERVICE: Number of DEAD STATIONS NOT AVAILABLE: "<<endmsg;
  
  return m_cachedDeadStations;
}


const std::vector<std::string>& MDT_DQConditionsSvc::deadMultiLayers() const{
   
  msg(MSG::DEBUG)<<"CONFIGURATION SERVICE: Number of DEAD ML NOT AVAILABLE: "<<endmsg;
  
  return m_cachedDeadMultiLayers;
}





bool MDT_DQConditionsSvc::canReportAbout(MuonConditions::Hierarchy h) {
  return (h == MuonConditions::MDT_TECHNOLOGY);
} 




bool MDT_DQConditionsSvc::isGoodChannel(const Identifier & /*Id*/) const{
  
  return true;  
}


bool MDT_DQConditionsSvc::isGoodChamber(const Identifier & /*Id*/) const{
 
  return true;
 }

bool MDT_DQConditionsSvc::isGoodMultiLayer(const Identifier & /*Id*/) const{
 
  return true;
 }



const std::vector<Identifier>& MDT_DQConditionsSvc::deadTubesId() const{
  
  unsigned int size_new =m_condDataTool->deadTubesId().size();
  msg(MSG::DEBUG)<<"DCS SERVICE: Number of DEAD TUBE: "<<size_new <<endmsg;
  return m_condDataTool->deadTubesId();
}


const std::vector<Identifier>& MDT_DQConditionsSvc::deadStationsId() const{
   
  msg(MSG::DEBUG)<<"CONFIGURATION SERVICE: Number of DEAD STATIONS NOT AVAILABLE: "<<endmsg;
  
  return m_cachedDeadStationsId;
}


const std::vector<Identifier>& MDT_DQConditionsSvc::deadMultiLayersId() const{
   
  msg(MSG::DEBUG)<<"CONFIGURATION SERVICE: Number of DEAD ML NOT AVAILABLE: "<<endmsg;
  
  return m_cachedDeadMultiLayersId;
}


const std::vector<Identifier>& MDT_DQConditionsSvc::deadLayersId() const{
   
  msg(MSG::DEBUG)<<"CONFIGURATION SERVICE: Number of DEAD LAYER NOT AVAILABLE: "<<endmsg;
  
  return m_cachedDeadLayersId;
}

const std::vector<Identifier>& MDT_DQConditionsSvc::List_Chambers_with_deadTube() const{
   
  msg(MSG::DEBUG)<<"CONFIGURATION SERVICE: Name of STATIONS WITH DEAD TUBE: "<<endmsg;
  
  return m_condDataTool->List_Chambers_with_deadTube();
}
