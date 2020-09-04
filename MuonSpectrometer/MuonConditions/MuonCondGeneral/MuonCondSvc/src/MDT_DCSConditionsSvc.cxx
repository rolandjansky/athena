/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/MDT_DCSConditionsSvc.h"
#include "MuonCondInterface/IMDT_DCSConditionsTool.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/StoreGateSvc.h"
#include <set>
#include <string>
#include <algorithm>
#include "Identifier/Identifier.h"
#include "Gaudi/Property.h"
#include "Identifier/IdentifierHash.h"
#include <iostream>

MDT_DCSConditionsSvc::MDT_DCSConditionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_condDataTool("MDT_DCSConditionsTool")
{
 
  declareProperty( "DCSInfofromCool",m_dcsInfofromCool=true);
  declareProperty( "MDT_DCSConditionsTool",  m_condDataTool, "MDT DCS Info from COOL");
}

MDT_DCSConditionsSvc::~MDT_DCSConditionsSvc()
{
}

StatusCode MDT_DCSConditionsSvc::initialize()
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
  if(m_dcsInfofromCool)
    {
      StatusCode sc = m_condDataTool.retrieve();
      if ( sc.isFailure() )
	{
	  
	  ATH_MSG_ERROR( "Could not retrieve MDT_DCSConditionsTool"  );
	}
      else
	{
	  
	  ATH_MSG_VERBOSE("MDT_DCSConditionsTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool );
	}
      std::vector<std::string> folderNames;
      folderNames.push_back((m_condDataTool)->DropChamberFolderName());
      folderNames.push_back((m_condDataTool)->LVFolderName());
      if ((m_condDataTool)->Simulation_Setup()==false){
      folderNames.push_back((m_condDataTool)->HVFolderName());
      folderNames.push_back((m_condDataTool)->JTAGFolderName());
      folderNames.push_back((m_condDataTool)->SetPointsV1FolderName());
      folderNames.push_back((m_condDataTool)->SetPointsV0FolderName());
      }
      ATH_MSG_VERBOSE("Register call-back  against "<<folderNames.size()<<" folders listed below " );
      //  bool aFolderFound = false;
      short ic=0;
      for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
	{
	  ++ic;
	  ATH_MSG_VERBOSE(" Folder n. "<<ic<<" <"<<(*ifld)<<">" );
	  if (detStore->contains<CondAttrListCollection>(*ifld)) {
	    //    aFolderFound=true;
	    ATH_MSG_VERBOSE("     found in the DetStore" );
	    const DataHandle<CondAttrListCollection> MDTDCSData;
	    if (detStore->regFcn(&IMDT_DCSConditionsSvc::initInfo,
				 dynamic_cast<IMDT_DCSConditionsSvc *>(this),
				 MDTDCSData,
				 *ifld)!=StatusCode::SUCCESS)
	      {
		ATH_MSG_WARNING("Unable to register call back for initDCSInfo against folder <"<<(*ifld)<<">" );
	      }
	    else ATH_MSG_VERBOSE("initDCSInfo registered for call-back against folder <"<<(*ifld)<<">" );
	  }
	  else
	    {   
	      ATH_MSG_WARNING("Folder "<<(*ifld)
                              <<" NOT found in the DetStore --- failing to init ???" );
	    }
	}
    }
  
  return StatusCode::SUCCESS;
}

StatusCode MDT_DCSConditionsSvc::finalize()
{
  
  ATH_MSG_VERBOSE( "Finalize"  );
  return StatusCode::SUCCESS;
}


StatusCode MDT_DCSConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if(IMDT_DCSConditionsSvc::interfaceID().versionMatch(riid) )
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



StatusCode MDT_DCSConditionsSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  ATH_MSG_VERBOSE("initDCSInfo has been called" );
  ATH_MSG_VERBOSE("ToolHandle in initMappingModel - <"<<m_condDataTool<<">" );
  
  if(m_dcsInfofromCool)
    {
      
      StatusCode sc = m_condDataTool->loadParameters(I, keys);
      if (sc.isFailure())
	{
	  ATH_MSG_WARNING("Reading DCS from COOL failed; NO MDT DCS INFO AVAILABLE" );
	}
      
    }
  
  return StatusCode::SUCCESS;
}


bool MDT_DCSConditionsSvc::canReportAbout(MuonConditions::Hierarchy h) {
  return (h == MuonConditions::MDT_TECHNOLOGY);
} 


bool MDT_DCSConditionsSvc::isGoodChamber(const Identifier & /*Id*/) const{
  return true;
}


bool MDT_DCSConditionsSvc::isGoodMultiLayer(const Identifier & /*Id*/) const{
  return true;
}



bool MDT_DCSConditionsSvc::isGoodChannel(const Identifier & /*Id*/) const{
  return true;
}


const std::vector<std::string>& MDT_DCSConditionsSvc::deadStations() const{
  
  unsigned int size_new =m_condDataTool->deadStations().size();
 
  ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD CHAMBERS: "<<size_new);
  
  return m_condDataTool->deadStations();
}


const std::vector<std::string>& MDT_DCSConditionsSvc::deadTubes() const{
  
    ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD TUBES NOT AVAILABLE: ");
  
  return m_cachedDeadTubes;
}


const std::vector<std::string>& MDT_DCSConditionsSvc::deadMultiLayers() const{
   
    ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD ML: ");
  
  return m_cachedDeadMultiLayers;
}

const std::vector<Identifier>& MDT_DCSConditionsSvc::deadStationsId() const{
  
  unsigned int size_new =m_condDataTool->deadStationsId().size();
 
  ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD CHAMBERS: "<<size_new);
  
  return m_condDataTool->deadStationsId();
}


const std::vector<Identifier>& MDT_DCSConditionsSvc::deadLayersId() const{
   
    ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD LAYERS NOT AVAILABLE: ");
  
   return m_cachedDeadLayersId;
}

const std::vector<Identifier>& MDT_DCSConditionsSvc::deadTubesId() const{
   
    ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD TUBES NOT AVAILABLE: ");
  
   return m_cachedDeadTubesId;
}


const std::vector<Identifier>& MDT_DCSConditionsSvc::deadMultiLayersId() const{
   
  
  unsigned int size_new =m_condDataTool->deadMultiLayersId().size();
  
  ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD ML: "<<size_new);
  
  return m_condDataTool->deadMultiLayersId();
  
}


const std::vector<Identifier>& MDT_DCSConditionsSvc::List_Chambers_with_deadTube() const{
    
    ATH_MSG_DEBUG("DCS SERVICE: Number of DEAD TUBE NOT AVAILABLE");
  return m_Chambers_with_deadTube;
}
