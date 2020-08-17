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
#include "Gaudi/Property.h"

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
  
  ATH_MSG_INFO( "Initializing " << name() << " - package version " 
                << PACKAGE_VERSION  );
  
  StoreGateSvc * detStore;
  StatusCode status = service("DetectorStore",detStore);
  if (status.isFailure()) {
    ATH_MSG_FATAL( "DetectorStore service not found !"  );
  } else {
    ATH_MSG_VERBOSE( "DetectorStore service found !"  );
    
  }
  if(m_deadtubeInfofromCool)
    {
      StatusCode sc = m_condDataTool.retrieve();
      if ( sc.isFailure() )
	{
	  
	  ATH_MSG_ERROR( "Could not retrieve MDT_DeadTubeConditionsTool"  );
	}
      else
	{
	  
	  ATH_MSG_VERBOSE("MDT_DeadTubeConditionsTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool );
	}
            
      std::vector<std::string> folderNames;
      folderNames.push_back((m_condDataTool)->DeadTubeFolderName());
      
      
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
	    const DataHandle<CondAttrListCollection> MDTDeadTubeData;
	    if (detStore->regFcn(&IMDT_DeadTubeConditionsSvc::initInfo,
				 dynamic_cast<IMDT_DeadTubeConditionsSvc *>(this),
				 MDTDeadTubeData,
				 *ifld)!=StatusCode::SUCCESS)
	      {
		ATH_MSG_WARNING("Unable to register call back for initDeadTubeInfo against folder <"<<(*ifld)<<">" );
	      }
	    else ATH_MSG_VERBOSE("initDeadTubeInfo registered for call-back against folder <"<<(*ifld)<<">" );
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

StatusCode MDT_DeadTubeConditionsSvc::finalize()
{
  
  
  return StatusCode::SUCCESS;
}


StatusCode MDT_DeadTubeConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if(IMDT_DeadTubeConditionsSvc::interfaceID().versionMatch(riid) )
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


	
StatusCode MDT_DeadTubeConditionsSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  ATH_MSG_VERBOSE("initDeadTubeInfo has been called" );
  ATH_MSG_VERBOSE("ToolHandle in init DeadTube Info - <"<<m_condDataTool<<">" );
   
  if(m_deadtubeInfofromCool)
    {
      
      StatusCode sc = m_condDataTool->loadParameters(I, keys);
      if (sc.isFailure())
	{
	  ATH_MSG_WARNING("Reading DeadTube from COOL failed; NO MDT DeadTube INFO AVAILABLE" );
	}
    }
    
  return StatusCode::SUCCESS;
}


const std::vector<std::string>& MDT_DeadTubeConditionsSvc::deadTubes() const{
  
  unsigned int size_new =m_condDataTool->deadTubes().size();
  ATH_MSG_DEBUG("Dead SERVICE: Number of DEAD TUBE: "<<size_new  );
  return m_condDataTool->deadTubes();
}


const std::vector<std::string>& MDT_DeadTubeConditionsSvc::deadStations() const{
   
  ATH_MSG_DEBUG("CONFIGURATION SERVICE: Number of DEAD STATIONS NOT AVAILABLE: " );
  
  return m_cachedDeadStations;
}


const std::vector<std::string>& MDT_DeadTubeConditionsSvc::deadMultiLayers() const{
   
  ATH_MSG_DEBUG("CONFIGURATION SERVICE: Number of DEAD ML NOT AVAILABLE: " );
  
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
  ATH_MSG_DEBUG("DCS SERVICE: Number of DEAD TUBE: "<<size_new  );
  return m_condDataTool->deadTubesId();
}


const std::vector<Identifier>& MDT_DeadTubeConditionsSvc::deadStationsId() const{
   
  ATH_MSG_DEBUG("CONFIGURATION SERVICE: Number of DEAD STATIONS NOT AVAILABLE: " );
  
  return m_cachedDeadStationsId;
}


const std::vector<Identifier>& MDT_DeadTubeConditionsSvc::deadMultiLayersId() const{
   
  ATH_MSG_DEBUG("CONFIGURATION SERVICE: Number of DEAD ML NOT AVAILABLE: " );
  
  return m_cachedDeadMultiLayersId;
}

const std::vector<Identifier>& MDT_DeadTubeConditionsSvc::deadLayersId() const{
   
  ATH_MSG_DEBUG("CONFIGURATION SERVICE: Number of DEAD LAYERS NOT AVAILABLE: " );
  
  return m_cachedDeadLayersId;
}

const std::vector<Identifier>& MDT_DeadTubeConditionsSvc::List_Chambers_with_deadTube() const{
   
  ATH_MSG_DEBUG("CONFIGURATION SERVICE: Name of STATIONS WITH DEAD TUBE: " );
  
  return m_condDataTool->List_Chambers_with_deadTube();
}
