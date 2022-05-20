/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <string>

#include "GaudiKernel/IClassManager.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "GaudiKernel/Bootstrap.h"

#include "AthenaKernel/errorcheck.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

// Suppress roofit banner.  Besides the output, it spins up cling and
// spits out ubsan warnings.
int doBanner() { return 0; }

int inputError(const std::string& errDescr, const po::options_description& optDescr ) {
  std::cerr << errDescr << "\n" << optDescr << std::endl;
  return 1;
}

//wrote a better version!
IAppMgrUI* initGaudi(const std::string& options, bool verbose, ISvcLocator*& svcLocator) {
  IAppMgrUI* theApp = Gaudi::createApplicationMgr();
  SmartIF<IProperty> propMgr(theApp);
  if(strlen(options.c_str())) {
    CHECK_WITH_CONTEXT( propMgr->setProperty("JobOptionsPath", options), "initGaudi", nullptr );
  } else {
    //no joboptions given
    CHECK_WITH_CONTEXT( propMgr->setProperty("JobOptionsType", "NONE"), "initGaudi", nullptr );
  }
  if (!verbose) {
    CHECK_WITH_CONTEXT( propMgr->setProperty("OutputLevel", MSG::WARNING), "initGaudi", nullptr );
  }
  CHECK_WITH_CONTEXT( theApp->configure(), "initGaudi", nullptr );
  CHECK_WITH_CONTEXT( theApp->initialize(), "initGaudi", nullptr );
  svcLocator = Gaudi::svcLocator();
  return theApp;

}

int main(int argc, char* argv[]) {
  // Declare the supported options.

  const std::string appName("genCLIDDB");
  po::options_description desc(appName + " allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("package,p", po::value<std::string>(), "package we want to load clids from")
    ("input,i", po::value<std::string>(), "optional path to input clid db file")
    ("output,o", po::value<std::string>(), "optional path to resulting clid db file")
    ("jobopts,j", po::value<std::string>(), "name of optional job options txt file, located at ../share/jobopts")
    ("verbose,v", po::value<bool>()->implicit_value(true), "verbose output")
    ;
  std::string packageName("ALL");
  std::string inputCLIDDB;
  std::string outFileName;

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
  } catch (const std::exception& e) {
    return inputError(e.what(), desc);
  }

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 0;
  }
  
  if (vm.count("package")) {
    packageName = vm["package"].as<std::string>();
  } else {
    return inputError("Please specify a package using option --package.\n", desc);
  }

  if (vm.count("output")) {
    outFileName = vm["output"].as<std::string>();
  } else {
    outFileName = packageName + "_clid.db";
  }

  bool verbose = vm.count("verbose") && vm["verbose"].as<bool>();

  // Initialize Gaudi
  ISvcLocator* pSvcLoc(nullptr);
  std::string jobopts("CLIDComps/minimalPrintout.opts");
  if (vm.count("jobopts")) {
    jobopts = vm["jobopts"].as<std::string>();
  }
  if (!initGaudi(jobopts, verbose, pSvcLoc)) {
    std::cerr << "cannot initialize Gaudi" << std::endl;
    return 2;
  }

  // Now we can use the MessagesSvc
  SmartIF<IMessageSvc> msgSvc(pSvcLoc);
  if (!msgSvc.isValid()) {
    std::cerr << "cannot retrieve MessageSvc" << std::endl;
    return 2;
  }
  MsgStream log(msgSvc, appName);
  if (!verbose) {
    log.setLevel(MSG::WARNING);
  }

  SmartIF<IClassManager> pICM(pSvcLoc);
  if (!pICM.isValid()) {
    log << MSG::ERROR << "cannot retrieve ClassManager" << endmsg;
    return 2;
  }

  IClassIDSvc* pClassIDSvc(nullptr);
  CHECK_WITH_CONTEXT( pSvcLoc->service("ClassIDSvc", pClassIDSvc, true), appName, 2 );

  SmartIF<IProperty> pCLIDSvcProp(pClassIDSvc);
  if (!pCLIDSvcProp.isValid()) {
    log << MSG::ERROR << "cannot retrieve ClassIDSvc" << std::endl;
    return 2;
  }

  log << MSG::INFO << "Writing clid.db for package "
      << vm["package"].as<std::string>() << " to " << outFileName << "." << endmsg;

  // Set properties on CLIDSvc
  std::string dbfiles("{}");
  if (vm.count("input")) {
    log << MSG::INFO << "Reading clid.db from " << vm["input"].as<std::string>() << "." << std::endl;
    dbfiles = "{\"" + vm["input"].as<std::string>() + "\"}";
  }
  CHECK_WITH_CONTEXT( pCLIDSvcProp->setProperty("CLIDDBFiles", dbfiles), appName, 2 );
  CHECK_WITH_CONTEXT( pCLIDSvcProp->setProperty("OutputFileName", outFileName), appName, 2 );

  // Load the module
  CHECK_WITH_CONTEXT( pICM->loadModule(packageName), appName, 2 );

  //fill clid db
  CHECK_WITH_CONTEXT( pClassIDSvc->reinitialize(), appName, 2 );

  //write out merged clid db on service finalize
  return (pClassIDSvc->finalize()).isSuccess() ? 0 : -1;
}
