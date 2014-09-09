/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include "GaudiKernel/System.h"

#include "JobIDSvc.h"

using namespace std;

using namespace Athena; 

/// Standard Constructor
JobIDSvc::JobIDSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc),
    m_JobID()
{
    
  // Get user's input	
//  declareProperty("CLIDDBFiles",  m_DBFiles, 
//		  "list of db files with (CLID, class_name) entries. Loaded at init in svc maps. Files are looked up in DATAPATH");
 // declareProperty("OutputFileName",  m_outputFileName,
//		  "path to clid.db file in which write at finalize entries in m_clidMap. Default ('NULL') is not to write output clid.db");

}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!

StatusCode 
JobIDSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
    if ( IJobIDSvc::interfaceID().versionMatch(riid) )    {
        *ppvInterface = (IJobIDSvc*)this;
    }
    else  {
	// Interface is not directly available: try out a base class
	return AthService::queryInterface(riid, ppvInterface);
    }
    addRef();
    return StatusCode::SUCCESS;
}

StatusCode 
JobIDSvc::initialize()
{
  ATH_MSG_INFO
    ("Initializing " << name() 
     << " - package version " << PACKAGE_VERSION);

  uuid_generate(m_JobID);
  return StatusCode::SUCCESS;
}

StatusCode 
JobIDSvc::finalize()
{
  return AthService::finalize();
}

PJobID_t
JobIDSvc::getJobID() const {
	return (PJobID_t)m_JobID;
}

std::string
JobIDSvc::toString() const {
  std::ostringstream o; o << m_JobID;
  return o.str();
}

void 
JobIDSvc::dump() const {
}


StatusCode
JobIDSvc::reinitialize() {
  ATH_MSG_INFO
    ("RE-initializing " << name() 
     << " - package version " << PACKAGE_VERSION);
  return this->initialize();
}
