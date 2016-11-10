/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/MuonAlignmentErrorDbSvc.h"

#include "MuonCondInterface/IMuonAlignmentErrorDbTool.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/StoreGateSvc.h"

#include <set>
#include <string>
#include <algorithm>

#include "Identifier/Identifier.h"
#include "GaudiKernel/Property.h"

#include "Identifier/IdentifierHash.h"
#include <iostream>

//using namespace std;


MuonAlignmentErrorDbSvc::MuonAlignmentErrorDbSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_condDataTool("MuonAlignmentErrorDbTool")
{
 

  declareProperty( "MuonAlignmentErrorDbTool",  m_condDataTool, "Alignment Error Info from COOL");
}

MuonAlignmentErrorDbSvc::~MuonAlignmentErrorDbSvc()
{
}

StatusCode MuonAlignmentErrorDbSvc::initialize()
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

  StatusCode sc = m_condDataTool.retrieve();
  if ( sc.isFailure() )
    {
      
      msg(MSG::ERROR) << "Could not retrieve MuonAlignmentErrorDbTool" << endmsg;
    }
  else
    {
	  
      msg(MSG::INFO)<<"MuonAlignmentErrorTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool<<endmsg;
    }

  
//  int I=0;
  std::list<std::string> keys;
// at this stage the folder is not yet known to conddb and the information cannot be read - do it via callback
// if (initInfo(I,keys).isFailure())
//   {
//     return StatusCode::FAILURE;
//   }
      msg(MSG::INFO)<<"dopo init "<<sc<<endmsg;

      std::vector<std::string> folderNames;
      folderNames.push_back((m_condDataTool)->ErrorFolderName());


      msg(MSG::INFO)<<"Register call-back  against "<<folderNames.size()<<" folders listed below "<<endmsg;

      short ic=0;
      for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
	{
	  ++ic;
	  msg(MSG::INFO)<<" Folder n. "<<ic<<" <"<<(*ifld)<<">";
	  if (detStore->contains<CondAttrListCollection>(*ifld)) {
	    //    aFolderFound=true;
	    msg(MSG::INFO)<<"     found in the DetStore"<<endmsg;
	    const DataHandle<CondAttrListCollection> MUONERRORData;
	    if (detStore->regFcn(&IMuonAlignmentErrorDbSvc::initInfo,
				 dynamic_cast<IMuonAlignmentErrorDbSvc *>(this),
				 MUONERRORData,
				 *ifld)!=StatusCode::SUCCESS)
	      {
		msg(MSG::WARNING)<<"Unable to register call back for initErrorInfo against folder <"<<(*ifld)<<">";
	      }
	    else msg(MSG::INFO)<<"initInfo registered for call-back against folder <"<<(*ifld)<<">"<<endmsg;
	  }
	  else
	    {   
	      msg(MSG::WARNING)<<"Folder "<<(*ifld)
			       <<" NOT found in the DetStore --- failing to init ???"<<endmsg;
	    }
	}
 
      return StatusCode::SUCCESS;
}

StatusCode MuonAlignmentErrorDbSvc::finalize()
{
  
  msg(MSG::INFO) << "Finalize" << endmsg;
  return StatusCode::SUCCESS;
}
/*
StatusCode MuonAlignmentErrorDbSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  msg(MSG::INFO) << "queryInterface Start" << endmsg;
  if(IMuonAlignmentErrorDbSvc::interfaceID().versionMatch(riid) )
    {
      msg(MSG::INFO) << "versionMatch=true" << endmsg;
      *ppvIF = this;
    }else if ( IMuonAlignmentErrorDbSvc::interfaceID().versionMatch(riid) ){
    *ppvIF = dynamic_cast<IMuonAlignmentErrorSvc*>(this);
    msg(MSG::INFO) << "service cast***************************" << endmsg;

  } else {
    msg(MSG::INFO) << "cannot find the interface!" << endmsg;
    return AthService::queryInterface(riid, ppvIF);
  }
  msg(MSG::INFO) << "queryInterface succesfull" << endmsg;
  //addRef();  // is this needed ??                                                                                             
  return StatusCode::SUCCESS;
}
*/

StatusCode MuonAlignmentErrorDbSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  msg(MSG::INFO) << "queryInterface Start" << endmsg;
 
  if(IMuonAlignmentErrorDbSvc::interfaceID().versionMatch(riid) )
    {
      *ppvInterface = (IMuonAlignmentErrorDbSvc*)this;
    } else {
    return AthService::queryInterface(riid, ppvInterface);
  }
  msg(MSG::INFO) << "queryInterface succesfull" << endmsg;
  addRef(); 
  return StatusCode::SUCCESS;
}



StatusCode MuonAlignmentErrorDbSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  msg(MSG::INFO)<<"initInfo has been called"<<endmsg;
  msg(MSG::INFO)<<"ToolHandle in initMappingModel - <"<<m_condDataTool<<">"<<endmsg;
  
  
  StatusCode sc = m_condDataTool->loadParameters(I, keys);
  if (sc.isFailure())
    {
      msg(MSG::WARNING)<<"Reading Alignment Error from COOL failed; NO ERROR INFO AVAILABLE"<<endmsg;
    }
  
  
  
  return StatusCode::SUCCESS;
}


const std::string& MuonAlignmentErrorDbSvc::ListErrorValue() const{
  

 
  ATH_MSG_VERBOSE("ALIGNMENT ERROR SERVICE");
  
  return m_condDataTool->errorValue();
}


