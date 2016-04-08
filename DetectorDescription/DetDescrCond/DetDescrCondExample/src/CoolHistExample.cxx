/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CoolHistExample.cxx - simple example of algorithm accessing CoolHistSvc
// for a reference histogram
// Richard Hawkings, started 09/2/07
//

#include "GaudiKernel/ISvcLocator.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrCondTools/ICoolHistSvc.h"
#include "TH1.h"
#include "TDirectory.h"
#include "DetDescrCondExample/CoolHistExample.h"
#include "StoreGate/StoreGateSvc.h"

CoolHistExample::CoolHistExample(const std::string& name, 
  ISvcLocator* pSvcLocator) :AthAlgorithm(name,pSvcLocator),
   p_coolhistsvc(0),
   m_histfolder("/DEMO/TESTHIST1"),
   m_dirfolder("/DEMO/TESTDIR1"),
   m_histname("/REF/t3333/vtxprobTD"),
   m_objname("fileGUID"),
   m_dirname("DefTag"),
   m_channel(10),
   m_retrievedir(false)
{
  // declare properties
  declareProperty("RetrieveDir",m_retrievedir);
}

CoolHistExample::~CoolHistExample() {}

StatusCode CoolHistExample::initialize()
{
  ATH_MSG_INFO("CoolHistExample::initialize() called");

  // get the CoolHistSvc
  if (StatusCode::SUCCESS!=service("CoolHistSvc",p_coolhistsvc)) {
    ATH_MSG_FATAL("Could not get CoolHistSvc");
    return StatusCode::FAILURE;
  }
  // register IOV callback function for the COOL folder
  const DataHandle<CondAttrListCollection> aptr,aptr2;
  if (StatusCode::SUCCESS==detStore()->regFcn(&CoolHistExample::callBack,
      this,aptr,m_histfolder)) {
    ATH_MSG_INFO("Registered IOV callback for "+m_histfolder );
  } else {
    ATH_MSG_ERROR("Registration of IOV callback failed for "+m_histfolder);
  }
  ATH_MSG_INFO("Will access hisogram name "+m_histfolder+" from COOL folder "+
	       m_histfolder+" channel " << m_channel);
  if (m_retrievedir) {
    if (StatusCode::SUCCESS==detStore()->regFcn(&CoolHistExample::callBack,
        this,aptr2,m_dirfolder)) {
      ATH_MSG_INFO("Registered IOV callback for "+m_dirfolder);
    } else {
      ATH_MSG_ERROR("Registration of IOV callback failed for "+m_dirfolder);
    }
    ATH_MSG_INFO("Will retrieve TDirectory "+m_dirname+" using COOL folder "+
		 m_dirfolder);
  }
  return StatusCode::SUCCESS;
}

StatusCode CoolHistExample::execute() {
  // access histogram via pointer, pointer is kept up to date via callback
  if (m_histptr!=0) {
    ATH_MSG_INFO("Printing the information for histogram "+m_histname);
    m_histptr->Print();
  } else {
    ATH_MSG_ERROR("Pointer to histogram "+m_histname+" is not valid");
  }
  if (m_objptr!=0) {
    ATH_MSG_INFO("Printing the information for TObject "+m_objname+
		 " : Classname " << m_objptr->ClassName());
    m_objptr->Print();
  } else {
    ATH_MSG_ERROR("Pointer to TObject "+m_objname+" is not valid");
  }
  if (m_retrievedir) {
    if (m_dirptr!=0) {
      ATH_MSG_INFO("Printing the information for TDirectory "+m_dirname);
      m_dirptr->ls();
      // get a histogram in the subdirectory
      TH1F* eat;
      m_dirptr->GetObject("EAT_mtop_1",eat);
      if (eat!=0) {
        ATH_MSG_INFO("Printing the information for contained EAT_mtop_1" );
        eat->Print();
      } else {
        ATH_MSG_ERROR("Cannot access contained EAT_mtop_1");
      }
    } else {
      ATH_MSG_ERROR("Pointer to TDirectory "+m_dirname+" is not valid");
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CoolHistExample::finalize() {
  ATH_MSG_INFO("In CoolHistExample::finalize");
  return StatusCode::SUCCESS;
}

StatusCode CoolHistExample::callBack( IOVSVC_CALLBACK_ARGS_P( /* I */ ,
keys) ) {
  // printout the list of keys invoked - will normally only be for our
  // histogram folder
  std::string keylist;
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end();++itr) keylist+=" "+*itr;
  ATH_MSG_INFO("CoolHistExample callback invoked for keys:"+keylist);
  // check all the keys, if we find the histogram folder, update the pointer
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end();++itr) {
    if (*itr==m_histfolder) {
      // try to retrieve the histogram
      if (StatusCode::SUCCESS==p_coolhistsvc->getHist(m_histfolder,
			      m_channel,m_histname,m_histptr)) {
        if (m_histptr!=0) {
	  ATH_MSG_INFO("Cached pointer to histogram: " << m_histptr);
        } else { 
  	  ATH_MSG_ERROR("Could not cache pointer to histogram "+m_histname);
	}
      } else {
	ATH_MSG_ERROR("Cannot retrieve pointer to histogram "+m_histname);
      }
      // try to retrieve the generic TObject
      if (StatusCode::SUCCESS==p_coolhistsvc->getTObject(m_histfolder,
			      m_channel,m_objname,m_objptr)) {
        if (m_objptr!=0) {
	  ATH_MSG_INFO("Cached pointer to object: " << m_objptr);
        } else { 
  	  ATH_MSG_ERROR("Could not cache pointer to object "+m_objname);
	}
      } else {
	ATH_MSG_ERROR("Cannot retrieve pointer to object "+m_objname);
      }
    }
    if (*itr==m_dirfolder) {
      // try to retrieve TDirectory
      TObject* dirptr;
      if (StatusCode::SUCCESS==p_coolhistsvc->getTObject(m_dirfolder,
						 1,m_dirname,dirptr)) {
	m_dirptr=dynamic_cast<TDirectory*>(dirptr);
	if (m_dirptr!=0) {
	  ATH_MSG_INFO("Cached pointer to directory "+m_dirname);
	} else {
	  ATH_MSG_ERROR("Could not cast pointer to directory "+m_dirname);
	}
      } else {
      ATH_MSG_ERROR("Could not get pointer to directory "+m_dirname);
      }
    }
  }
  return StatusCode::SUCCESS;
}

