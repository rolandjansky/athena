/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 minimal gaudi initialization for AthenaServices unit testing
 ------------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: initGaudi.cxx,v 1.5 2005-05-04 00:39:51 calaf Exp $

//<<<<<< INCLUDES                                                       >>>>>>

#include "TestTools/initGaudi.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ModuleIncident.h"
using std::cout;
using std::endl;
using std::string;

namespace Athena_test {
  bool initGaudi(ISvcLocator*& pSvcLoc) {
    return initGaudi(string(), pSvcLoc); //wily hack
  }
  bool initGaudi(const std::string& jobOptsFile, ISvcLocator*& pSvcLoc) {
    string jobOptsPath = jobOptsFile;
    if (access (jobOptsPath.c_str(), R_OK) != 0)
      jobOptsPath = "../share/"+jobOptsFile;
    if (access (jobOptsPath.c_str(), R_OK) != 0) {
      const char* jopath = getenv ("JOBOPTSEARCHPATH");
      if (jopath) {
        char* savepath = new char[strlen(jopath)+1];
        strcpy (savepath, jopath);
        char* saveptr = nullptr;
        char* str = savepath;
        while (char* tok = strtok_r (str, ":", &saveptr)) {
          str = nullptr;
          jobOptsPath = std::string(tok) + "/" + jobOptsFile;
          if (access (jobOptsPath.c_str(), R_OK) == 0) {
            break;
          }
        }
        delete [] savepath;
      }
    }
    bool noJobOpts(jobOptsFile.empty());
    if (!noJobOpts) {
      cout << "\n\nInitializing Gaudi ApplicationMgr using job opts " << jobOptsPath << endl;
    }

    // Create an instance of an application manager
    IInterface* iface = Gaudi::createApplicationMgr();
    if( 0 == iface ) {
      cout << "Fatal error while creating the ApplicationMgr " << endl;
      return false;
    }

    SmartIF<ISvcManager> svcMgr(iface);
    SmartIF<IAppMgrUI> appMgr(iface);
    SmartIF<IProperty> propMgr(iface);
    SmartIF<ISvcLocator> svcLoc(iface);
    if(!svcLoc.isValid() || !appMgr.isValid() || !svcMgr.isValid() || !propMgr.isValid()) {
      cout << "Fatal error while creating the AppMgr smart if " << endl;
      return false;
    }

    pSvcLoc = svcLoc.pRef();

    (propMgr->setProperty( "EvtSel",         "NONE" )).ignore();
    if (noJobOpts) {
      (propMgr->setProperty( "JobOptionsType", "NONE" )).ignore();
    } else {
      (propMgr->setProperty( "JobOptionsType", "FILE" )).ignore();
      (propMgr->setProperty( "JobOptionsPath", jobOptsPath )).ignore();
    }

    if ((appMgr->configure()).isSuccess() &&
	(appMgr->initialize()).isSuccess()) {
      cout<<"ApplicationMgr Ready"<<endl;
      return true;
    } else {
      cout << "Fatal error while initializing the AppMgr" << endl;
      return false;
    }
  }


  // This is for the benefit of the tests in CLIDComps.
  // By default, a dynamic symbol is not exported from a binary unless
  // it's referenced by another library with which we link.
  // A test in CLIDComps creates a ModuleLoadedIncident object in the binary
  // and then passes it to code in CLIDComps.  If ubsan is running,
  // then it will try to verify that this object derives from ModuleIncident.
  // But if the ModuleIncident type_info is not exported from the binary,
  // then the type_info used by the library will be different from that
  // in the binary, causing an ubsan warning.
  // But this test gets linked against TestTools, so we can avoid the warning
  // by referencing the type information here.
  // (An alternative would be to link the binary with --export-dynamic,
  // but that's not portable, so we don't want to put it in a package cmake.)
  void referenceGaudiSymbols()
  {
    ModuleLoadedIncident inc2 ("", "");
  }
}


