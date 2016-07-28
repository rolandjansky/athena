/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/CSC_DCSConditionsSvc.h"

#include "MuonCondInterface/ICSC_DCSConditionsTool.h"
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




CSC_DCSConditionsSvc::CSC_DCSConditionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_condDataTool("CSC_DCSConditionsTool")
{
 
  declareProperty( "DCSInfofromCool",m_dcsInfofromCool=true);
  declareProperty( "CSC_DCSConditionsTool",  m_condDataTool, "CSC DCS Info from COOL");
}

CSC_DCSConditionsSvc::~CSC_DCSConditionsSvc()
{
}

StatusCode CSC_DCSConditionsSvc::initialize()
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
	  
	  msg(MSG::ERROR) << "Could not retrieve CSC_DCSConditionsTool" << endmsg;
	}
      else
	{
	  
	  msg(MSG::INFO)<<"CSC_DCSConditionsTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool<<endmsg;
	}
      std::vector<std::string> folderNames;
      folderNames.push_back((m_condDataTool)->ChamberFolder());
      folderNames.push_back((m_condDataTool)->HVFolderName());
      
      
      msg(MSG::INFO)<<"Register call-back  against "<<folderNames.size()<<" folders listed below "<<endmsg;
      //bool aFolderFound = false;
      short ic=0;
      for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
	{
	  ++ic;
	  msg(MSG::INFO)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
	  if (detStore->contains<CondAttrListCollection>(*ifld)) {
	    //    aFolderFound=true;
	    msg(MSG::INFO)<<"     found in the DetStore"<<endmsg;
	    const DataHandle<CondAttrListCollection> CSCDCSData;
	    if (detStore->regFcn(&ICSC_DCSConditionsSvc::initInfo,
				 dynamic_cast<ICSC_DCSConditionsSvc *>(this),
				 CSCDCSData,
				 *ifld)!=StatusCode::SUCCESS)
	      {
		msg(MSG::WARNING)<<"Unable to register call back for initDCSInfo against folder <"<<(*ifld)<<">";
	      }
	    else msg(MSG::INFO)<<"initDCSInfo registered for call-back against folder <"<<(*ifld)<<">"<<endmsg;
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

StatusCode CSC_DCSConditionsSvc::finalize()
{
  
  msg(MSG::INFO) << "Finalize" << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode CSC_DCSConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  msg(MSG::INFO) << "queryInterface Start" << endmsg;
  if(ICSC_DCSConditionsSvc::interfaceID().versionMatch(riid) )
    {
      msg(MSG::INFO) << "versionMatch=true" << endmsg;
      msg(MSG::INFO) << "OK***************************" << endmsg;
      *ppvInterface = this;      
    } else if ( ICSCConditionsSvc::interfaceID().versionMatch(riid) ) {
      *ppvInterface = dynamic_cast<ICSCConditionsSvc*>(this);
      msg(MSG::INFO) << "service cast***************************" << endmsg;
    } else {
      msg(MSG::INFO) << "cannot find the interface!***************************" << endmsg;
      
      return AthService::queryInterface(riid, ppvInterface);
    }
  msg(MSG::INFO) << "queryInterface succesfull" << endmsg;
  addRef(); 
  return StatusCode::SUCCESS;
}



StatusCode CSC_DCSConditionsSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  msg(MSG::INFO)<<"initDCSInfo has been called"<<endmsg;
  msg(MSG::INFO)<<"ToolHandle in initMappingModel - <"<<m_condDataTool<<">"<<endmsg;
  
  if(m_dcsInfofromCool)
    {
      
      StatusCode sc = m_condDataTool->loadParameters(I, keys);
      if (sc.isFailure())
	{
	  msg(MSG::WARNING)<<"Reading DCS from COOL failed; NO CSC DCS INFO AVAILABLE"<<endmsg;
	}
      
    }
  
  return StatusCode::SUCCESS;
}


bool CSC_DCSConditionsSvc::canReportAbout(MuonConditions::Hierarchy h) {
  return (h == MuonConditions::CSC_TECHNOLOGY);
} 


bool CSC_DCSConditionsSvc::isGoodChamber(const Identifier & /*Id*/) const{
  return true;
}


bool CSC_DCSConditionsSvc::isGoodWireLayer(const Identifier & /*Id*/) const{
  return true;
}

bool CSC_DCSConditionsSvc::isGood(const Identifier & /*Id*/) const{
  return true;
}


const std::vector<Identifier>& CSC_DCSConditionsSvc::deadStationsId() const{
  
  unsigned int size_new =m_condDataTool->deadStationsId().size();
 
  msg(MSG::VERBOSE)<<"DCS SERVICE: Number of DEAD CHAMBERS: "<<size_new <<endmsg;
  
  return m_condDataTool->deadStationsId();
}




const std::vector<Identifier>& CSC_DCSConditionsSvc::deadWireLayersId() const{
   
  
  unsigned int size_new =m_condDataTool->deadWireLayersId().size();
  
  msg(MSG::VERBOSE)<<"DCS SERVICE: Number of DEAD Wire Layer: "<<size_new <<endmsg;
  
  return m_condDataTool->deadWireLayersId();
  
}


const std::vector<std::string>& CSC_DCSConditionsSvc::deadStations() const{
  
  unsigned int size_new =m_condDataTool->deadStations().size();
 
  msg(MSG::VERBOSE)<<"DCS SERVICE: Number of DEAD CHAMBERS: "<<size_new <<endmsg;
  
  return m_condDataTool->deadStations();
}




const std::vector<std::string>& CSC_DCSConditionsSvc::deadWireLayers() const{
   
  
  unsigned int size_new =m_condDataTool->deadWireLayers().size();
  
  msg(MSG::VERBOSE)<<"DCS SERVICE: Number of DEAD Wire Layer: "<<size_new <<endmsg;
  
  return m_condDataTool->deadWireLayers();
  
}

