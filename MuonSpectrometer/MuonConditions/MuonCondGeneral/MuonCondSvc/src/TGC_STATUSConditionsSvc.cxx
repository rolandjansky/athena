/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/TGC_STATUSConditionsSvc.h"
#include "MuonCondInterface/ITGCConditionsSvc.h"
#include "MuonCondInterface/ITGC_STATUSConditionsTool.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/StoreGateSvc.h"

#include <set>
#include <list>
#include <string>
#include <algorithm>
#include <iostream>

#include "Identifier/Identifier.h"
#include "Gaudi/Property.h"
#include "Identifier/IdentifierHash.h"

TGC_STATUSConditionsSvc::TGC_STATUSConditionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_condDataTool("TGC_STATUSConditionsTool")
{
  declareProperty( "UseCoolDB", m_useCool = true);
  declareProperty( "TGC_STATUSConditionsTool", m_condDataTool, "TGC Detector Status from COOL");
}



TGC_STATUSConditionsSvc::~TGC_STATUSConditionsSvc()
{
}



StatusCode TGC_STATUSConditionsSvc::initialize()
{
  ATH_MSG_INFO( "Initializing "
		 << name() 
		 << " - version" 
		 << PACKAGE_VERSION 
                );
  
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
      
      ATH_MSG_ERROR( "Could not retrieve TgcDetectorStatusDbTool"  );
    }
  else
    {
      
      ATH_MSG_VERBOSE("TgcDetectorStatusDbTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool );
    }
  
  
  
  std::vector<std::string> folderNames;
  folderNames.push_back((m_condDataTool)->FolderName());
   
  ATH_MSG_VERBOSE("Register call-back  against "<<folderNames.size()<<" folders listed below " );
  // bool aFolderFound = false;
  short ic=0;
  for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
    {
      ++ic;
      ATH_MSG_VERBOSE(" Folder n. "<<ic<<" <"<<(*ifld)<<">" );
      if (detStore->contains<CondAttrListCollection>(*ifld)) {
	//	aFolderFound=true;
	ATH_MSG_VERBOSE("     found in the DetStore" );
	const DataHandle<CondAttrListCollection> TGCData;
	if (detStore->regFcn(&ITGC_STATUSConditionsSvc::initInfo,
			     dynamic_cast<ITGC_STATUSConditionsSvc *>(this),
			     TGCData,
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



StatusCode TGC_STATUSConditionsSvc::finalize()
{

  ATH_MSG_VERBOSE( "Finalize"  );
  
  return StatusCode::SUCCESS;
}



StatusCode TGC_STATUSConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if(ITGC_STATUSConditionsSvc::interfaceID().versionMatch(riid) )
    {
      *ppvInterface = this;      
    } else if ( ITGCConditionsSvc::interfaceID().versionMatch(riid) ) {
      *ppvInterface = dynamic_cast<ITGCConditionsSvc*>(this);
    } else {
     
      return AthService::queryInterface(riid, ppvInterface);
    }
  addRef(); 
  return StatusCode::SUCCESS;
}

	
StatusCode TGC_STATUSConditionsSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys)) {

  ATH_MSG_VERBOSE( "initTGCInfo() has been called for "
		 << keys.size()
		 << " keys"
                );
   
  if (m_useCool) {
    
      
      StatusCode sc = m_condDataTool->loadParameterStatus(I, keys);
      if (sc.isFailure())
	{
	  ATH_MSG_WARNING("Reading TGC INFO from COOL failed; NO TGC INFO AVAILABLE" );
	}
      
    }
  
  return StatusCode::SUCCESS;
}



bool TGC_STATUSConditionsSvc::canReportAbout(MuonConditions::Hierarchy h) {

  return (h == MuonConditions::TGC_TECHNOLOGY);
} 
bool TGC_STATUSConditionsSvc::isGoodChamber(const Identifier & /*Id*/){
  return true;
}

const std::vector<Identifier>& TGC_STATUSConditionsSvc::deadStationsId(){
  
  unsigned int size_new =m_condDataTool->deadStationsId().size();
 
  ATH_MSG_VERBOSE("TGC STATUS SERVICE: Number of DEAD CHAMBERS: "<<size_new  );
  
  return m_condDataTool->deadStationsId();
}
