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
#include "Gaudi/Property.h"

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
  
  ATH_MSG_INFO( "Initializing " << name() << " - package version " 
                << PACKAGE_VERSION  );
  
  StoreGateSvc * detStore;
  StatusCode status = service("DetectorStore",detStore);
  if (status.isFailure()) {
    ATH_MSG_FATAL( "DetectorStore service not found !"  );
  } else {
    ATH_MSG_VERBOSE( "DetectorStore service found !"  );
    
  }
  if(m_dqInfofromCool)
    {
      StatusCode sc = m_condDataTool.retrieve();
      if ( sc.isFailure() )
	{
	  
	  ATH_MSG_ERROR( "Could not retrieve MDT_DQConditionsTool"  );
	}
      else
	{
	  
	  ATH_MSG_VERBOSE("MDT_DQConditionsTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool );
	}
            
      std::vector<std::string> folderNames;
      folderNames.push_back((m_condDataTool)->DeadFolderName());
      folderNames.push_back((m_condDataTool)->NoisyFolderName());
      
      
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
	    const DataHandle<CondAttrListCollection> MDTDQData;
	    if (detStore->regFcn(&IMDT_DQConditionsSvc::initInfo,
				 dynamic_cast<IMDT_DQConditionsSvc *>(this),
				 MDTDQData,
				 *ifld)!=StatusCode::SUCCESS)
	      {
		ATH_MSG_WARNING("Unable to register call back for initDQInfo against folder <"<<(*ifld)<<">" );
	      }
	    ATH_MSG_VERBOSE("initDQInfo registered for call-back against folder <"<<(*ifld)<<">" );
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

StatusCode MDT_DQConditionsSvc::finalize()
{
  
  
  return StatusCode::SUCCESS;
}


StatusCode MDT_DQConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if(IMDT_DQConditionsSvc::interfaceID().versionMatch(riid) )
    {
      *ppvIF = this;
    }else if ( IMDTConditionsSvc::interfaceID().versionMatch(riid) ){
      *ppvIF = dynamic_cast<IMDTConditionsSvc*>(this);
      
    } else {
      return AthService::queryInterface(riid, ppvIF);
    }
  //addRef();  // is this needed ??
  return StatusCode::SUCCESS;
}


	
StatusCode MDT_DQConditionsSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  ATH_MSG_VERBOSE("initDQInfo has been called" );
  ATH_MSG_VERBOSE("ToolHandle in init DQ Info - <"<<m_condDataTool<<">" );
   
  if(m_dqInfofromCool)
    {
      
      StatusCode sc = m_condDataTool->loadParameters(I, keys);
      if (sc.isFailure())
	{
	  ATH_MSG_WARNING("Reading DQ from COOL failed; NO MDT DQ INFO AVAILABLE" );
	}
    }
    
  return StatusCode::SUCCESS;
}


const std::vector<std::string>& MDT_DQConditionsSvc::deadTubes() const{

  return m_cachedDeadTubes;
}


const std::vector<std::string>& MDT_DQConditionsSvc::deadStations() const{
   
  ATH_MSG_DEBUG("CONFIGURATION SERVICE: Number of DEAD STATIONS NOT AVAILABLE: " );
  
  return m_cachedDeadStations;
}


const std::vector<std::string>& MDT_DQConditionsSvc::deadMultiLayers() const{
   
  ATH_MSG_DEBUG("CONFIGURATION SERVICE: Number of DEAD ML NOT AVAILABLE: " );
  
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
  ATH_MSG_DEBUG("DCS SERVICE: Number of DEAD TUBE: "<<size_new  );
  return m_condDataTool->deadTubesId();
}


const std::vector<Identifier>& MDT_DQConditionsSvc::deadStationsId() const{
   
  ATH_MSG_DEBUG("CONFIGURATION SERVICE: Number of DEAD STATIONS NOT AVAILABLE: " );
  
  return m_cachedDeadStationsId;
}


const std::vector<Identifier>& MDT_DQConditionsSvc::deadMultiLayersId() const{
   
  ATH_MSG_DEBUG("CONFIGURATION SERVICE: Number of DEAD ML NOT AVAILABLE: " );
  
  return m_cachedDeadMultiLayersId;
}


const std::vector<Identifier>& MDT_DQConditionsSvc::deadLayersId() const{
   
  ATH_MSG_DEBUG("CONFIGURATION SERVICE: Number of DEAD LAYER NOT AVAILABLE: " );
  
  return m_cachedDeadLayersId;
}

const std::vector<Identifier>& MDT_DQConditionsSvc::List_Chambers_with_deadTube() const{
   
  ATH_MSG_DEBUG("CONFIGURATION SERVICE: Name of STATIONS WITH DEAD TUBE: " );
  
  return m_condDataTool->List_Chambers_with_deadTube();
}
