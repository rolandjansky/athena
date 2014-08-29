/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/MDT_DeadTubeConditionsSvc.h"
#include "MuonCondSvc/MuonHierarchy.h"
#include "MuonCondInterface/IMDT_DeadTubeConditionsTool.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
//#include "PathResolver/PathResolver.h"
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/Property.h"

#include "Identifier/IdentifierHash.h"
#include <iostream>
#include <set>
#include <string>



MDT_DeadTubeConditionsSvc::MDT_DeadTubeConditionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_condDataTool("MDT_DeadTubeConditionsTool")
{
 
  declareProperty( "DeadTubeInfofromCool",m_deadtubeInfofromCool=true);
  declareProperty( "MDT_DeadTubeConditionsTool",  m_condDataTool, "MDT DeadTube Info from COOL");
}

MDT_DeadTubeConditionsSvc::~MDT_DeadTubeConditionsSvc()
{
}

StatusCode MDT_DeadTubeConditionsSvc::initialize()
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
  if(m_deadtubeInfofromCool)
    {
      StatusCode sc = m_condDataTool.retrieve();
      if ( sc.isFailure() )
	{
	  
	  msg(MSG::ERROR) << "Could not retrieve MDT_DeadTubeConditionsTool" << endreq;
	}
      else
	{
	  
	  msg(MSG::INFO)<<"MDT_DeadTubeConditionsTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool<<endreq;
	}
            
      std::vector<std::string> folderNames;
      folderNames.push_back((m_condDataTool)->DeadTubeFolderName());
      
      
      msg(MSG::INFO)<<"Register call-back  against "<<folderNames.size()<<" folders listed below "<<endreq;
      //  bool aFolderFound = false;
      short ic=0;
      for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
	{
	  ++ic;
	  msg(MSG::INFO)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
	  if (detStore->contains<CondAttrListCollection>(*ifld)) {
	    //    aFolderFound=true;
	    msg(MSG::INFO)<<"     found in the DetStore"<<endreq;
	    const DataHandle<CondAttrListCollection> MDTDeadTubeData;
	    if (detStore->regFcn(&IMDT_DeadTubeConditionsSvc::initInfo,
				 dynamic_cast<IMDT_DeadTubeConditionsSvc *>(this),
				 MDTDeadTubeData,
				 *ifld)!=StatusCode::SUCCESS)
	      {
		msg(MSG::WARNING)<<"Unable to register call back for initDeadTubeInfo against folder <"<<(*ifld)<<">";
	      }
	    else msg(MSG::INFO)<<"initDeadTubeInfo registered for call-back against folder <"<<(*ifld)<<">"<<endreq;
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

StatusCode MDT_DeadTubeConditionsSvc::finalize()
{
  
  
  return StatusCode::SUCCESS;
}


StatusCode MDT_DeadTubeConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  msg(MSG::INFO) << "queryInterface Start" << endreq;
  if(IMDT_DeadTubeConditionsSvc::interfaceID().versionMatch(riid) )
    {
      msg(MSG::INFO) << "versionMatch=true" << endreq;
      *ppvIF = this;
    }else if ( IMDTConditionsSvc::interfaceID().versionMatch(riid) ){
      *ppvIF = dynamic_cast<IMDTConditionsSvc*>(this);
      msg(MSG::INFO) << "service cast***************************" << endreq;
      
    } else {
      msg(MSG::INFO) << "cannot find the interface!" << endreq;
      return AthService::queryInterface(riid, ppvIF);
    }
  msg(MSG::INFO) << "queryInterface succesfull" << endreq;
  //addRef();  // is this needed ??
  return StatusCode::SUCCESS;
}


	
StatusCode MDT_DeadTubeConditionsSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  msg(MSG::INFO)<<"initDeadTubeInfo has been called"<<endreq;
  msg(MSG::INFO)<<"ToolHandle in init DeadTube Info - <"<<m_condDataTool<<">"<<endreq;
   
  if(m_deadtubeInfofromCool)
    {
      
      StatusCode sc = m_condDataTool->loadParameters(I, keys);
      if (sc.isFailure())
	{
	  msg(MSG::WARNING)<<"Reading DeadTube from COOL failed; NO MDT DeadTube INFO AVAILABLE"<<endreq;
	}
    }
    
  return StatusCode::SUCCESS;
}


const std::vector<std::string>& MDT_DeadTubeConditionsSvc::deadTubes() const{
  
  unsigned int size_new =m_condDataTool->deadTubes().size();
  msg(MSG::DEBUG)<<"Dead SERVICE: Number of DEAD TUBE: "<<size_new <<endreq;
  return m_condDataTool->deadTubes();
}


const std::vector<std::string>& MDT_DeadTubeConditionsSvc::deadStations() const{
   
  msg(MSG::DEBUG)<<"CONFIGURATION SERVICE: Number of DEAD STATIONS NOT AVAILABLE: "<<endreq;
  
  return m_cachedDeadStations;
}


const std::vector<std::string>& MDT_DeadTubeConditionsSvc::deadMultiLayers() const{
   
  msg(MSG::DEBUG)<<"CONFIGURATION SERVICE: Number of DEAD ML NOT AVAILABLE: "<<endreq;
  
  return m_cachedDeadMultiLayers;
}


bool MDT_DeadTubeConditionsSvc::canReportAbout(MuonConditions::Hierarchy h) {
  return (h == MuonConditions::MDT_TECHNOLOGY);
} 




bool MDT_DeadTubeConditionsSvc::isGoodChannel(const Identifier & /*Id*/) const{
  
  return true;  
}


bool MDT_DeadTubeConditionsSvc::isGoodChamber(const Identifier & /*Id*/) const{
 
  return true;
 }

bool MDT_DeadTubeConditionsSvc::isGoodMultiLayer(const Identifier & /*Id*/) const{
 
  return true;
 }



const std::vector<Identifier>& MDT_DeadTubeConditionsSvc::deadTubesId() const{
  
  unsigned int size_new =m_condDataTool->deadTubesId().size();
  msg(MSG::DEBUG)<<"DCS SERVICE: Number of DEAD TUBE: "<<size_new <<endreq;
  return m_condDataTool->deadTubesId();
}


const std::vector<Identifier>& MDT_DeadTubeConditionsSvc::deadStationsId() const{
   
  msg(MSG::DEBUG)<<"CONFIGURATION SERVICE: Number of DEAD STATIONS NOT AVAILABLE: "<<endreq;
  
  return m_cachedDeadStationsId;
}


const std::vector<Identifier>& MDT_DeadTubeConditionsSvc::deadMultiLayersId() const{
   
  msg(MSG::DEBUG)<<"CONFIGURATION SERVICE: Number of DEAD ML NOT AVAILABLE: "<<endreq;
  
  return m_cachedDeadMultiLayersId;
}

const std::vector<Identifier>& MDT_DeadTubeConditionsSvc::deadLayersId() const{
   
  msg(MSG::DEBUG)<<"CONFIGURATION SERVICE: Number of DEAD LAYERS NOT AVAILABLE: "<<endreq;
  
  return m_cachedDeadLayersId;
}

const std::vector<Identifier>& MDT_DeadTubeConditionsSvc::List_Chambers_with_deadTube() const{
   
  msg(MSG::DEBUG)<<"CONFIGURATION SERVICE: Name of STATIONS WITH DEAD TUBE: "<<endreq;
  
  return m_condDataTool->List_Chambers_with_deadTube();
}
