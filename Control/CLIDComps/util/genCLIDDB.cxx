/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include <cassert>
#include <exception>
#include <iostream>
#include <string>
#include "GaudiKernel/IClassManager.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "GaudiKernel/Bootstrap.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

// Suppress roofit banner.  Besides the output, it spins up cling and
// spits out ubsan warnings.
int doBanner() { return 0; }

int inputError(const std::string& errDescr, const po::options_description& optDescr ) {
  cerr << errDescr << "\n" << optDescr << endl;
  return 1;
}
int gaudiError(const std::string& errDescr) {
  cerr << errDescr << endl;
  return 2;
}

//wrote a better version!
IAppMgrUI* initGaudi(const std::string& options, ISvcLocator*& svcLocator) {
  IAppMgrUI* theApp = Gaudi::createApplicationMgr();
  SmartIF<IProperty> propMgr(theApp);
  if(strlen(options.c_str())) {
    propMgr->setProperty("JobOptionsPath",options); 
  } else {
    propMgr->setProperty( "JobOptionsType", "NONE" ); //no joboptions given
  }
  theApp->configure();
  theApp->initialize();
  svcLocator = Gaudi::svcLocator();
  return theApp;

}

int main(int argc, char* argv[]) {
  // Declare the supported options.
  po::options_description desc("clidDB_gen allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("package,p", po::value<string>(), "package we want to load clids from")
    ("input,i", po::value<string>(), "optional path to input clid db file")
    ("output,o", po::value<string>(), "optional path to resulting clid db file")
    ("jobopts,j", po::value<string>(), "name of optional job options txt file, located at ../share/jobopts")
    ;
  string packageName("ALL");
  string inputCLIDDB;
  string outFileName;

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
  } catch (const exception& e) {
    return inputError(e.what(), desc);
  }

  if (vm.count("help")) {
    cout << desc << endl;
    return 0;
  }
  
  if (vm.count("package")) {
    cout << "Generating clid.db for package " 
	 << vm["package"].as<string>() << ".\n";
    packageName = vm["package"].as<string>();
  } else {
    return inputError("Please specify a package using option --package.\n", 
		      desc);
  }

  if (vm.count("output")) {
    outFileName = vm["output"].as<string>();
  } else {
    outFileName = packageName + "_clid.db";
  }
  cout << "Resulting clid.db will be written to " 
       << outFileName << ".\n";

  ISvcLocator* pSvcLoc(0);
  if (vm.count("jobopts")) { 
    if (!initGaudi(vm["jobopts"].as<string>(), pSvcLoc)) {
      return gaudiError("clidDB_gen can not run");
    }  
  } else {
    if (!initGaudi("CLIDComps/minimalPrintout.opts",pSvcLoc)) {
      return gaudiError("clidDB_gen can not run");
    }  
  }

  if ( 0 == pSvcLoc ) {
    return gaudiError( "NULL pointer to ISvcLocator" );
  }
  SmartIF<IClassManager> pICM(pSvcLoc);
  if (!pICM.isValid()) {
    gaudiError("can not get IClassManager");
  }

  IClassIDSvc* pClassIDSvc(0);
  if (!(pSvcLoc->service("ClassIDSvc", pClassIDSvc, true).isSuccess())) {
    cerr << "can not get ClassIDSvc, no clid.db will be generated" << endl;
    return 1;
  }

  if ( 0 == pClassIDSvc ) {
    return gaudiError("NULL pointer to IClassIDSvc");
  }

  IProperty *pCLIDSvcProp(dynamic_cast<IProperty*>(pClassIDSvc));
  if ( 0 == pCLIDSvcProp ) {
    return gaudiError("NULL pointer to IClassIDSvc's property");
  }

  if (vm.count("input")) {
    cout << "Reading clid.db from " 
	 << vm["input"].as<string>() << ".\n";
    if (!(pCLIDSvcProp->setProperty( "CLIDDBFiles", 
				     "{\"" + vm["input"].as<string>() + "\"}" )).isSuccess()) {
      return inputError("Error setting ClassIDSvc.CLIDDBFiles to " 
			+ vm["input"].as<string>(), 
			desc);
    }
  }

  pCLIDSvcProp->setProperty( "OutputFileName", outFileName );

  if (!pICM->loadModule(packageName).isSuccess()) {
    return gaudiError("can not load module " + packageName);
  }

  //fill clid db
  if (!(pClassIDSvc->reinitialize()).isSuccess()) {
    return gaudiError("can not reinitialize ClassIDSvc");
  }

  //write out merged clid db on service finalize
  return (pClassIDSvc->finalize()).isSuccess() ? 0 : -1;
}
