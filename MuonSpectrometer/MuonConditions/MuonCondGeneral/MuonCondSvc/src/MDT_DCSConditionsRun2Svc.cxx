/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/MDT_DCSConditionsRun2Svc.h"
#include "MuonCondInterface/IMDT_DCSConditionsRun2Tool.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/StoreGateSvc.h"
#include <set>
#include <string>
#include <algorithm>
#include "Identifier/Identifier.h"
#include "Gaudi/Property.h"
#include "Identifier/IdentifierHash.h"
#include <iostream>

MDT_DCSConditionsRun2Svc::MDT_DCSConditionsRun2Svc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_condDataTool("MDT_DCSConditionsRun2Tool")
{
 
  declareProperty( "DCSInfofromCool",m_dcsInfofromCool=true);
  declareProperty( "MDT_DCSConditionsRun2Tool",  m_condDataTool, "MDT DCS Info from COOL");
}

MDT_DCSConditionsRun2Svc::~MDT_DCSConditionsRun2Svc()
{
}

StatusCode MDT_DCSConditionsRun2Svc::initialize()
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
  if(m_dcsInfofromCool)
    {
      StatusCode sc = m_condDataTool.retrieve();
      if ( sc.isFailure() )
	{
	  
	  msg(MSG::ERROR) << "Could not retrieve MDT_DCSConditionsRun2Tool" << endmsg;
	}
      else
	{
	  
	  msg(MSG::DEBUG)<<"MDT_DCSConditionsRun2Tool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool<<endmsg;
	}
      std::vector<std::string> folderNames;
      
      folderNames.push_back((m_condDataTool)->LVFolderName());
      folderNames.push_back((m_condDataTool)->HVFolderName());
      //folderNames.push_back((m_condDataTool)->JTAGFolderName());
      
      // }
      msg(MSG::INFO)<<"Register call-back  against "<<folderNames.size()<<" folders listed below "<<endmsg;
      //  bool aFolderFound = false;
      short ic=0;
      for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
	{
	  ++ic;
	  msg(MSG::DEBUG)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
	  if (detStore->contains<CondAttrListCollection>(*ifld)) {
	    //    aFolderFound=true;
	    msg(MSG::DEBUG)<<"     found in the DetStore"<<endmsg;
	    const DataHandle<CondAttrListCollection> MDTDCSData;
	    if (detStore->regFcn(&IMDT_DCSConditionsRun2Svc::initInfo,
				 dynamic_cast<IMDT_DCSConditionsRun2Svc *>(this),
				 MDTDCSData,
				 *ifld)!=StatusCode::SUCCESS)
	      {
		msg(MSG::WARNING)<<"Unable to register call back for initDCSInfo against folder <"<<(*ifld)<<">";
	      }
	    else msg(MSG::DEBUG)<<"initDCSInfo registered for call-back against folder <"<<(*ifld)<<">"<<endmsg;
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

StatusCode MDT_DCSConditionsRun2Svc::finalize()
{
  
  msg(MSG::INFO) << "Finalize" << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode MDT_DCSConditionsRun2Svc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if(IMDT_DCSConditionsRun2Svc::interfaceID().versionMatch(riid) )
    {
      *ppvInterface = this;      
    } else if ( IMDTConditionsSvc::interfaceID().versionMatch(riid) ) {
      *ppvInterface = dynamic_cast<IMDTConditionsSvc*>(this);
    } else {
      
      return AthService::queryInterface(riid, ppvInterface);
    }
  addRef(); 
  return StatusCode::SUCCESS;
}



StatusCode MDT_DCSConditionsRun2Svc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  msg(MSG::DEBUG)<<"initDCSInfo has been called"<<endmsg;
  msg(MSG::DEBUG)<<"ToolHandle in initMappingModel - <"<<m_condDataTool<<">"<<endmsg;
  
  if(m_dcsInfofromCool)
    {
      
      StatusCode sc = m_condDataTool->loadParameters(I, keys);
      if (sc.isFailure())
	{
	  msg(MSG::WARNING)<<"Reading DCS from COOL failed; NO MDT DCS INFO AVAILABLE"<<endmsg;
	}
      
    }
  
  return StatusCode::SUCCESS;
}


bool MDT_DCSConditionsRun2Svc::canReportAbout(MuonConditions::Hierarchy h) {
  return (h == MuonConditions::MDT_TECHNOLOGY);
} 


bool MDT_DCSConditionsRun2Svc::isGoodChamber(const Identifier & /*Id*/) const{
  return true;
}


bool MDT_DCSConditionsRun2Svc::isGoodMultiLayer(const Identifier & /*Id*/) const{
  return true;
}



bool MDT_DCSConditionsRun2Svc::isGoodChannel(const Identifier & /*Id*/) const{
  return true;
}


const std::vector<std::string>& MDT_DCSConditionsRun2Svc::deadStations() const{
  
  unsigned int size_new =m_condDataTool->deadStations().size();
 
  ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD CHAMBERS: "<<size_new);
  
  return m_condDataTool->deadStations();
}


const std::vector<std::string>& MDT_DCSConditionsRun2Svc::deadTubes() const{
  
    ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD TUBES NOT AVAILABLE: ");
  
  return m_cachedDeadTubes;
}


const std::vector<std::string>& MDT_DCSConditionsRun2Svc::deadMultiLayers() const{
   
    ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD ML: ");
  
  return m_cachedDeadMultiLayers;
}

const std::vector<Identifier>& MDT_DCSConditionsRun2Svc::deadStationsId() const{
  
  unsigned int size_new =m_condDataTool->deadStationsId().size();
 
  ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD CHAMBERS: "<<size_new);
  
  return m_condDataTool->deadStationsId();
}


const std::vector<Identifier>& MDT_DCSConditionsRun2Svc::deadLayersId() const{
   
    ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD LAYERS NOT AVAILABLE: ");
  
   return m_cachedDeadLayersId;
}

const std::vector<Identifier>& MDT_DCSConditionsRun2Svc::deadTubesId() const{
   
    ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD TUBES NOT AVAILABLE: ");
  
   return m_cachedDeadTubesId;
}


const std::vector<Identifier>& MDT_DCSConditionsRun2Svc::deadMultiLayersId() const{
   
  
  unsigned int size_new =m_condDataTool->deadMultiLayersId().size();
  
  ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD ML: "<<size_new);
  
  return m_condDataTool->deadMultiLayersId();
  
}


const std::vector<Identifier>& MDT_DCSConditionsRun2Svc::List_Chambers_with_deadTube() const{
    
    ATH_MSG_DEBUG("DCS SERVICE: Number of DEAD TUBE NOT AVAILABLE");
  return m_Chambers_with_deadTube;
}
