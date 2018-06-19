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
  
  ATH_MSG_INFO( "Initializing " << name() << " - package version " 
                << PACKAGE_VERSION  );
  
  StoreGateSvc * detStore;
  StatusCode status = service("DetectorStore",detStore);
  if (status.isFailure()) {
    ATH_MSG_FATAL( "DetectorStore service not found !"  );
  } else {
    ATH_MSG_VERBOSE( "DetectorStore service found !"  );
    
  }

  StatusCode sc = m_condDataTool.retrieve();
  if ( sc.isFailure() )
    {
      
      ATH_MSG_ERROR( "Could not retrieve MuonAlignmentErrorDbTool"  );
    }
  else
    {
	  
      ATH_MSG_VERBOSE("MuonAlignmentErrorTool retrieved with statusCode = "<<sc<<" pointer = "<<m_condDataTool );
    }

  
//  int I=0;
  std::list<std::string> keys;
// at this stage the folder is not yet known to conddb and the information cannot be read - do it via callback
// if (initInfo(I,keys).isFailure())
//   {
//     return StatusCode::FAILURE;
//   }
      ATH_MSG_VERBOSE("dopo init "<<sc );

      std::vector<std::string> folderNames;
      folderNames.push_back((m_condDataTool)->ErrorFolderName());


      ATH_MSG_VERBOSE("Register call-back  against "<<folderNames.size()<<" folders listed below " );

      short ic=0;
      for (std::vector<std::string>::const_iterator ifld =folderNames.begin(); ifld!=folderNames.end(); ++ifld )
	{
	  ++ic;
	  ATH_MSG_VERBOSE(" Folder n. "<<ic<<" <"<<(*ifld)<<">" );
	  if (detStore->contains<CondAttrListCollection>(*ifld)) {
	    //    aFolderFound=true;
	    ATH_MSG_VERBOSE("     found in the DetStore" );
	    const DataHandle<CondAttrListCollection> MUONERRORData;
	    if (detStore->regFcn(&IMuonAlignmentErrorDbSvc::initInfo,
				 dynamic_cast<IMuonAlignmentErrorDbSvc *>(this),
				 MUONERRORData,
				 *ifld)!=StatusCode::SUCCESS)
	      {
		ATH_MSG_WARNING("Unable to register call back for initErrorInfo against folder <"<<(*ifld)<<">" );
	      }
	    ATH_MSG_VERBOSE("initInfo registered for call-back against folder <"<<(*ifld)<<">" );
	  }
	  else
	    {   
	      ATH_MSG_WARNING("Folder "<<(*ifld)
                              <<" NOT found in the DetStore --- failing to init ???" );
	    }
	}
 
      return StatusCode::SUCCESS;
}

StatusCode MuonAlignmentErrorDbSvc::finalize()
{
  
  ATH_MSG_VERBOSE( "Finalize"  );
  return StatusCode::SUCCESS;
}
/*
StatusCode MuonAlignmentErrorDbSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  ATH_MSG_VERBOSE( "queryInterface Start"  );
  if(IMuonAlignmentErrorDbSvc::interfaceID().versionMatch(riid) )
    {
      ATH_MSG_VERBOSE( "versionMatch=true"  );
      *ppvIF = this;
    }else if ( IMuonAlignmentErrorDbSvc::interfaceID().versionMatch(riid) ){
    *ppvIF = dynamic_cast<IMuonAlignmentErrorSvc*>(this);
    ATH_MSG_VERBOSE( "service cast***************************"  );

  } else {
    ATH_MSG_VERBOSE( "cannot find the interface!"  );
    return AthService::queryInterface(riid, ppvIF);
  }
  ATH_MSG_VERBOSE( "queryInterface successful"  );
  //addRef();  // is this needed ??                                                                                             
  return StatusCode::SUCCESS;
}
*/

StatusCode MuonAlignmentErrorDbSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
 
  if(IMuonAlignmentErrorDbSvc::interfaceID().versionMatch(riid) )
    {
      *ppvInterface = (IMuonAlignmentErrorDbSvc*)this;
    } else {
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef(); 
  return StatusCode::SUCCESS;
}



StatusCode MuonAlignmentErrorDbSvc::initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  ATH_MSG_VERBOSE("initInfo has been called" );
  ATH_MSG_VERBOSE("ToolHandle in initMappingModel - <"<<m_condDataTool<<">" );
  
  
  StatusCode sc = m_condDataTool->loadParameters(I, keys);
  if (sc.isFailure())
    {
      ATH_MSG_WARNING("Reading Alignment Error from COOL failed; NO ERROR INFO AVAILABLE" );
    }
  
  
  
  return StatusCode::SUCCESS;
}


const std::string& MuonAlignmentErrorDbSvc::ListErrorValue() const{
  

 
  ATH_MSG_VERBOSE("ALIGNMENT ERROR SERVICE");
  
  return m_condDataTool->errorValue();
}


