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
  ISvcLocator* pSvcLocator) :Algorithm(name,pSvcLocator),
   m_log(msgSvc(),name),
   p_detstore(0),p_coolhistsvc(0),
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
  m_log << MSG::INFO << "CoolHistExample::initialize() called" << endreq;

  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    m_log << MSG::FATAL << "Detector store not found" << endreq;
    return StatusCode::FAILURE;
  }
  // get the CoolHistSvc
  if (StatusCode::SUCCESS!=service("CoolHistSvc",p_coolhistsvc)) {
    m_log << MSG::FATAL << "Could not get CoolHistSvc" << endreq;
    return StatusCode::FAILURE;
  }
  // register IOV callback function for the COOL folder
  const DataHandle<CondAttrListCollection> aptr,aptr2;
  if (StatusCode::SUCCESS==p_detstore->regFcn(&CoolHistExample::callBack,
      this,aptr,m_histfolder)) {
    m_log << MSG::INFO << "Registered IOV callback for " << m_histfolder << 
      endreq;
  } else {
    m_log << MSG::ERROR << "Registration of IOV callback failed for "
	  << m_histfolder << endreq;
  }
  m_log << "Will access hisogram name " << m_histfolder << " from COOL folder "
	<< m_histfolder << " channel " << m_channel << endreq;
  if (m_retrievedir) {
    if (StatusCode::SUCCESS==p_detstore->regFcn(&CoolHistExample::callBack,
        this,aptr2,m_dirfolder)) {
      m_log << MSG::INFO << "Registered IOV callback for " << m_dirfolder <<
        endreq;
    } else {
      m_log << MSG::ERROR << "Registration of IOV callback failed for " 
	  << m_dirfolder << endreq;
    }
    m_log << "Will retrieve TDirectory " << m_dirname << " using COOL folder "
	  << m_dirfolder << endreq;
  }
  return StatusCode::SUCCESS;
}

StatusCode CoolHistExample::execute() {
  // access histogram via pointer, pointer is kept up to date via callback
  if (m_histptr!=0) {
    m_log << MSG::INFO << "Printing the information for histogram "
	  << m_histname << endreq;
    m_histptr->Print();
  } else {
    m_log << MSG::ERROR << "Pointer to histogram " << m_histname << 
      " is not valid" << endreq;
  }
  if (m_objptr!=0) {
    m_log << MSG::INFO << "Printing the information for TObject " << m_objname
	  << " : Classname " << m_objptr->ClassName() << endreq;
    m_objptr->Print();
  } else {
    m_log << MSG::ERROR << "Pointer to TObject " << m_objname << 
      " is not valid" << endreq;
  }
  if (m_retrievedir) {
    if (m_dirptr!=0) {
      m_log << MSG::INFO << "Printing the information for TDirectory " << 
        m_dirname << endreq;
      m_dirptr->ls();
      // get a histogram in the subdirectory
      TH1F* eat;
      m_dirptr->GetObject("EAT_mtop_1",eat);
      if (eat!=0) {
        m_log << MSG::INFO << 
	  "Printing the information for contained EAT_mtop_1" << endreq;
        eat->Print();
      } else {
        m_log << MSG::ERROR << "Cannot access contained EAT_mtop_1" << endreq;
      }
    } else {
      m_log << MSG::ERROR << "Pointer to TDirectory " << m_dirname << 
        " is not valid" << endreq;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CoolHistExample::finalize() {
  m_log << MSG::INFO << "In CoolHistExample::finalize" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode CoolHistExample::callBack( IOVSVC_CALLBACK_ARGS_P( /* I */ ,
keys) ) {
  // printout the list of keys invoked - will normally only be for our
  // histogram folder
  m_log << MSG::INFO << "CoolHistExample callback invoked for keys:";
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end();++itr) m_log << *itr << " ";
  m_log << endreq;
  // check all the keys, if we find the histogram folder, update the pointer
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end();++itr) {
    if (*itr==m_histfolder) {
      // try to retrieve the histogram
      if (StatusCode::SUCCESS==p_coolhistsvc->getHist(m_histfolder,
			      m_channel,m_histname,m_histptr)) {
        if (m_histptr!=0) {
	  m_log << MSG::INFO << "Cached pointer to histogram: " << 
	    m_histptr << endreq;
        } else { 
  	  m_log << MSG::ERROR << "Could not cache pointer to histogram "
	      << m_histname << endreq;
	}
      } else {
	m_log << MSG::ERROR << "Cannot retrieve pointer to histogram "
	      << m_histname << endreq;
      }
      // try to retrieve the generic TObject
      if (StatusCode::SUCCESS==p_coolhistsvc->getTObject(m_histfolder,
			      m_channel,m_objname,m_objptr)) {
        if (m_objptr!=0) {
	  m_log << MSG::INFO << "Cached pointer to object: " << 
	    m_objptr << endreq;
        } else { 
  	  m_log << MSG::ERROR << "Could not cache pointer to object "
	      << m_objname << endreq;
	}
      } else {
	m_log << MSG::ERROR << "Cannot retrieve pointer to object "
	      << m_objname << endreq;
      }
    }
    if (*itr==m_dirfolder) {
      // try to retrieve TDirectory
      TObject* dirptr;
      if (StatusCode::SUCCESS==p_coolhistsvc->getTObject(m_dirfolder,
						 1,m_dirname,dirptr)) {
	m_dirptr=dynamic_cast<TDirectory*>(dirptr);
	if (m_dirptr!=0) {
	  m_log << MSG::INFO << "Cached pointer to directory " << m_dirname
		<< endreq;
	} else {
	  m_log << MSG::ERROR << "Could not cast pointer to directory "
		<< m_dirname << endreq;
	}
      } else {
	m_log << MSG::ERROR << "Could not get pointer to directory " << 
	  m_dirname << endreq;
      }
    }
  }
  return StatusCode::SUCCESS;
}

