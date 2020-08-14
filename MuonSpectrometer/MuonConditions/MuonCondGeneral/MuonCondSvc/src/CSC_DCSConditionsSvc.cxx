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
#include "Gaudi/Property.h"

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
	  
	  ATH_MSG_ERROR( "Could not retrieve CSC_DCSConditionsTool"  );
	}
      else
	{
	  
	  ATH_MSG_VERBOSE("CSC_DCSConditionsTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool );
	}
      std::vector<std::string> folderNames;
      folderNames.push_back((m_condDataTool)->ChamberFolder());
      folderNames.push_back((m_condDataTool)->HVFolderName());
      
      
      ATH_MSG_VERBOSE("Register call-back  against "<<folderNames.size()<<" folders listed below " );
      //bool aFolderFound = false;
      short ic=0;
      for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
	{
	  ++ic;
	  ATH_MSG_VERBOSE(" Folder n. "<<ic<<" <"<<(*ifld)<<">"  );
	  if (detStore->contains<CondAttrListCollection>(*ifld)) {
	    //    aFolderFound=true;
	    ATH_MSG_VERBOSE("     found in the DetStore" );
	    const DataHandle<CondAttrListCollection> CSCDCSData;
	    if (detStore->regFcn(&ICSC_DCSConditionsSvc::initInfo,
				 dynamic_cast<ICSC_DCSConditionsSvc *>(this),
				 CSCDCSData,
				 *ifld)!=StatusCode::SUCCESS)
	      {
		ATH_MSG_WARNING("Unable to register call back for initDCSInfo against folder <"<<(*ifld)<<">"  );
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

StatusCode CSC_DCSConditionsSvc::finalize()
{
  
  ATH_MSG_VERBOSE( "Finalize"  );
  return StatusCode::SUCCESS;
}


StatusCode CSC_DCSConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if(ICSC_DCSConditionsSvc::interfaceID().versionMatch(riid) )
    {
      *ppvInterface = this;      
    } else if ( ICSCConditionsSvc::interfaceID().versionMatch(riid) ) {
      *ppvInterface = dynamic_cast<ICSCConditionsSvc*>(this);
    } else {
      
      return AthService::queryInterface(riid, ppvInterface);
    }
  addRef(); 
  return StatusCode::SUCCESS;
}



StatusCode CSC_DCSConditionsSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  ATH_MSG_VERBOSE("initDCSInfo has been called" );
  ATH_MSG_VERBOSE("ToolHandle in initMappingModel - <"<<m_condDataTool<<">" );
  
  if(m_dcsInfofromCool)
    {
      
      StatusCode sc = m_condDataTool->loadParameters(I, keys);
      if (sc.isFailure())
	{
	  ATH_MSG_WARNING("Reading DCS from COOL failed; NO CSC DCS INFO AVAILABLE" );
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
 
  ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD CHAMBERS: "<<size_new  );
  
  return m_condDataTool->deadStationsId();
}




const std::vector<Identifier>& CSC_DCSConditionsSvc::deadWireLayersId() const{
   
  
  unsigned int size_new =m_condDataTool->deadWireLayersId().size();
  
  ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD Wire Layer: "<<size_new  );
  
  return m_condDataTool->deadWireLayersId();
  
}


const std::vector<std::string>& CSC_DCSConditionsSvc::deadStations() const{
  
  unsigned int size_new =m_condDataTool->deadStations().size();
 
  ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD CHAMBERS: "<<size_new  );
  
  return m_condDataTool->deadStations();
}




const std::vector<std::string>& CSC_DCSConditionsSvc::deadWireLayers() const{
   
  
  unsigned int size_new =m_condDataTool->deadWireLayers().size();
  
  ATH_MSG_VERBOSE("DCS SERVICE: Number of DEAD Wire Layer: "<<size_new  );
  
  return m_condDataTool->deadWireLayers();
  
}

