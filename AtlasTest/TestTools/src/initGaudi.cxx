/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
}


