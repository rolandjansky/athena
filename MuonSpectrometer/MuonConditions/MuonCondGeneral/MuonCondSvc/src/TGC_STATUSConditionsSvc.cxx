/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/TGC_STATUSConditionsSvc.h"

#include "MuonCondInterface/ITGCConditionsSvc.h"
#include "MuonCondInterface/ITGC_STATUSConditionsTool.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
//#include "PathResolver/PathResolver.h"
#include "StoreGate/StoreGateSvc.h"

#include <set>
#include <list>
#include <string>
#include <algorithm>
#include <iostream>

#include "Identifier/Identifier.h"
#include "GaudiKernel/Property.h"

#include "Identifier/IdentifierHash.h"


//using namespace std;



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
  msg(MSG::INFO) << "Initializing "
		 << name() 
		 << " - version" 
		 << PACKAGE_VERSION 
		 << endmsg;
  
  StoreGateSvc * detStore;
  StatusCode status = service("DetectorStore",detStore);
  if (status.isFailure()) {
    msg(MSG::FATAL) << "DetectorStore service not found !" << endmsg; 
  } else {
    msg(MSG::INFO) << "DetectorStore service found !" << endmsg; 
    
  }
  //if(m_dcsInfofromCool)
  //{
  StatusCode sc = m_condDataTool.retrieve();
  if ( sc.isFailure() )
    {
      
      msg(MSG::ERROR) << "Could not retrieve TgcDetectorStatusDbTool" << endmsg;
    }
  else
    {
      
      msg(MSG::INFO)<<"TgcDetectorStatusDbTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool<<endmsg;
    }
  
  
  
  std::vector<std::string> folderNames;
  folderNames.push_back((m_condDataTool)->FolderName());
   
  msg(MSG::INFO)<<"Register call-back  against "<<folderNames.size()<<" folders listed below "<<endmsg;
  // bool aFolderFound = false;
  short ic=0;
  for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
    {
      ++ic;
      msg(MSG::INFO)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
      if (detStore->contains<CondAttrListCollection>(*ifld)) {
	//	aFolderFound=true;
	msg(MSG::INFO)<<"     found in the DetStore"<<endmsg;
	const DataHandle<CondAttrListCollection> TGCData;
	if (detStore->regFcn(&ITGC_STATUSConditionsSvc::initInfo,
			     dynamic_cast<ITGC_STATUSConditionsSvc *>(this),
			     TGCData,
			     *ifld)!=StatusCode::SUCCESS)
	  {
	    msg(MSG::WARNING)<<"Unable to register call back for initDCSInfo against folder <"<<(*ifld)<<">";
	    //return StatusCode::FAILURE;
	  }
	    else msg(MSG::INFO)<<"initDCSInfo registered for call-back against folder <"<<(*ifld)<<">"<<endmsg;
      }
      else
	{   
	  msg(MSG::WARNING)<<"Folder "<<(*ifld)
			   <<" NOT found in the DetStore --- failing to init ???"<<endmsg;
	  //	      break;
	}
    }
  





  
  return StatusCode::SUCCESS;
}



StatusCode TGC_STATUSConditionsSvc::finalize()
{

  msg(MSG::INFO) << "Finalize" << endmsg;
  
  return StatusCode::SUCCESS;
}



StatusCode TGC_STATUSConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
   msg(MSG::INFO) << "queryInterface Start" << endmsg;
  if(ITGC_STATUSConditionsSvc::interfaceID().versionMatch(riid) )
    {
      msg(MSG::INFO) << "versionMatch=true" << endmsg;
      msg(MSG::INFO) << "OK***************************" << endmsg;
      *ppvInterface = this;      
    } else if ( ITGCConditionsSvc::interfaceID().versionMatch(riid) ) {
      *ppvInterface = dynamic_cast<ITGCConditionsSvc*>(this);
      msg(MSG::INFO) << "service cast***************************" << endmsg;
    } else {
      msg(MSG::INFO) << "cannot find the interface!***************************" << endmsg;
     
      return AthService::queryInterface(riid, ppvInterface);
    }
  msg(MSG::INFO) << "queryInterface succesfull" << endmsg;
  addRef(); 
  return StatusCode::SUCCESS;
}

	
StatusCode TGC_STATUSConditionsSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys)) {

  msg(MSG::INFO) << "initTGCInfo() has been called for "
		 << keys.size()
		 << " keys"
		 << endmsg;
   
  if (m_useCool) {
    
      
      StatusCode sc = m_condDataTool->loadParameterStatus(I, keys);
      if (sc.isFailure())
	{
	  msg(MSG::WARNING)<<"Reading TGC INFO from COOL failed; NO TGC INFO AVAILABLE"<<endmsg;
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
 
  msg(MSG::VERBOSE)<<"TGC STATUS SERVICE: Number of DEAD CHAMBERS: "<<size_new <<endmsg;
  
  return m_condDataTool->deadStationsId();
}
