/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ctest_athena_service.h

// Provides a main program similar to athena that loads a test
// service CtestSvc.
//
// A series of user-defined functions are optionally called. It is
// intended that component test code reside in these functions.
//
// A default implementation is provided for each of these functions
// and the user must define the corresponding symbol if the function
// is supplied by the user. These symbols must be defined before this
// header is included.

#include <string>
#include <list>

//**********************************************************************
// User functions.
//**********************************************************************

// List lines to be included in the job options file.
// The input list is the default. User may delete any of these (at
// some risk) or prepend or append any desired entries.

void set_job_options(std::list<std::string>& opts);

#ifndef CTEST_ATHENA_set_job_options
void set_job_options(std::list<std::string>& opts) { }
#endif

//**********************************************************************

// Called during initialization of the test service. Return value is
// used to increment the overall return value.
// This is the place to put testing code.

int service_initialize();

#ifndef CTEST_ATHENA_service_initialize
int service_initialize(const Service* psvc) {
  return 0;
}
#endif

//**********************************************************************
// Implementation.
//**********************************************************************

#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"

using std::string;
using std::list;
using std::ifstream;
using std::ofstream;
using std::cout;
using std::endl;

//**********************************************************************
namespace { 

static bool verbose = 8;

static int return_status = 0;

}

//**********************************************************************

// Copy of athena main.

int athena() {

  // Create an instance of an application manager
  IInterface* iface = Gaudi::createApplicationMgr();
  SmartIF<IProperty> propMgr ( IID_IProperty, iface );
  SmartIF<IAppMgrUI> appMgr  ( IID_IAppMgrUI, iface );
  if( !appMgr.isValid() || !propMgr.isValid() ) {
    std::cout << "Fatal error while creating the ApplicationMgr " << std::endl;
    return 1;
  }

  // Get the input configuration file from arguments (defaulting to "jobOptions.txt")
  //std:: string opts = (argc>1) ? argv[1] : "jobOptions.txt";
  string opts = "jobOptions.txt";

  propMgr->setProperty( "JobOptionsPath", opts );
  if( opts.substr( opts.length() - 3, 3 ) == ".py" ) {

    // Python script specified - override defaults
    propMgr->setProperty( "JobOptionsType", "NONE" );
    propMgr->setProperty( "DLLs",
       "['AthenaServices', 'GaudiPython', 'McEventSelector', 'StoreGate']" );
    propMgr->setProperty( "ExtSvc",
       "['StoreGateSvc', 'McEventSelector/EventSelector']" );
    propMgr->setProperty( "Runable",        "PythonScriptingSvc" );
    propMgr->setProperty( "EventLoop",      "AthenaEventLoopMgr" );
    propMgr->setProperty( "OutStreamType",  "AthenaOutputStream" );
  }

  // Run the application manager and process events
  return (appMgr->run().isSuccess()) ? 0 : 1;

}

//**********************************************************************

// Test service class.

const InterfaceID& IID_CtestSvc(2222,2,2);

class CtestSvc : public Service {
public:
  // Constructor.
  CtestSvc(const std::string& name, ISvcLocator* sl)
  : Service(name, sl) {
    if ( verbose > 8 ) cout << "CtestSvc constructor called." << endl;
  }
  // Interface query.
  StatusCode queryInterface(const IID& rid, void** ppvIF) {
    if ( verbose > 0 ) cout << "CtestSvc querying interface: " << endl;
    if ( IID_CtestSvc == rid ) {
      if ( verbose > 8 )cout << "success" << endl;
      *ppvIF = this;
      return StatusCode::SUCCESS;
    }
    if ( verbose > 8 ) cout << "failure" << endl;
    return StatusCode::SUCCESS;
  }
  // Initialize.
  StatusCode initialize() {
    if ( verbose > 8 ) cout << "CtestSvc initializing." << endl;
    Service::initialize();
    setProperties();
    return_status += service_initialize(this);
    return StatusCode::SUCCESS;
  }
  // Finalize.
  StatusCode finalize() {
    if ( verbose > 8 ) cout << "CtestSvc finalizing." << endl;
    return StatusCode::SUCCESS;
  }
};

namespace {
static SvcFactory<CtestSvc> s_factory;
const ISvcFactory& CtestSvcFactory = s_factory;
}

//**********************************************************************

// Remember the coommand from the last call to cfgdir.
namespace {
  string lastcom = "";
}

// Function to locate directory pkgname/VERSION/$CMTCONFIG holding
// file filename.
string cfgdir(string pkgname, string filename) {
  system("touch tmp.dat; rm tmp.dat; touch tmp.dat");
  string com = "DIRS=`echo $CMTPATH: | sed 's#:#/";
  com += pkgname;
  com += " #g'`";
  com += "; CDIRS=`for DIR in $DIRS; do if test -d $DIR; then echo $DIR/*/$CMTCONFIG" "; fi; done`";
  com += "; for DIR in $CDIRS; do if test -r $DIR/";
  com += filename;
  com += "; then echo $DIR 1>>tmp.dat 2>/dev/null; fi; done";
  lastcom = com;
  system(com.c_str());
  ifstream exefile("tmp.dat");
  string cfgdir;
  exefile >> cfgdir;
  return cfgdir;
}

//**********************************************************************

// Component test.
// If the required libraries are missing from the shared library path,
// then a subrocess is started using the executable exe.

int ctest(string exe) {

  // Fetch the existing library path.
  // Append and prepend colons.
  string libpath = getenv("LD_LIBRARY_PATH");
  string colon_libpath = ":" + libpath + ":";
  string newlibpath = libpath;
  if ( verbose > 0 ) {
    cout << "LD_LIBRARY_PATH =\n" << libpath << endl;
  }

  string pkg;
  string file;
  string dir;

  // Find athena common.
  // Almost everything needed is here.
  pkg = "Control/AthenaCommon";
  file = "athena.py";
  dir = cfgdir(pkg, file);
  if ( dir.size() == 0 ) {
    if ( verbose > 0 ) {
      cout << "Unable to find package " << pkg << " with " << file << endl;
      cout << lastcom << endl;
    }
    return 1;
  }
  // If not already present, append directory to the path.
  if ( colon_libpath.find(":"+dir+":") == string::npos ) {
    newlibpath += ":" + dir;
  }

  // Find Gaudi service library.
  pkg = "GaudiSvc";
  file = "libGaudiSvc.so";
  dir = cfgdir(pkg, file);
  string com1 = lastcom;
  // If file is not in directory, try lib/file.
  if ( dir.size() == 0 ) {
    dir = cfgdir(pkg, "lib/"+file);
    if ( dir.size() > 0 ) dir += "/lib";
  }
  if ( dir.size() == 0 ) {
    if ( verbose > 0 ) {
      cout << "Unable to find package " << pkg << " with " << file << endl;
      cout << com1 << endl;
      cout << lastcom << endl;
    }
    return 2;
  }
  // If not already present, append directory to the path.
  if ( colon_libpath.find(":"+dir+":") == string::npos ) {
    newlibpath += ":" + dir;
  }

  // If the library path is changed, then restart with a new path.
  if ( newlibpath != libpath ) {
    if ( exe.size() == 0 ) return 1;
    string com = "LD_LIBRARY_PATH=" + newlibpath;
    com += "; " + exe;
    system(com.c_str());
    ifstream instat("return.dat");
    instat >> return_status;
    if ( verbose > 8 ) cout << " Subprocess returns " << return_status << endl;
    return return_status;
  }

  // Build job options file.
  system("touch jobOptions.txt; rm jobOptions.txt");
  ofstream jobopt("jobOptions.txt");
  {
    // Build a default list of options.
    list<string> opts;
    opts.push_back("ApplicationMgr.ExtSvc += { \"CtestSvc\" };");
    opts.push_back("MessageSvc.OutputLevel = 5;");
    // Let user modify this list.
    set_job_options(opts);
    // Build job options file from list.
    for ( list<string>::const_iterator iopt=opts.begin();
          iopt!=opts.end(); ++iopt ) {
      jobopt << *iopt << endl;
    }
  }

  // Run athena.
  if ( verbose > 8 ) cout << endl;
  return_status += athena();

  // Write return status to a file.
  system("touch return.dat; rm return.dat");
  ofstream retfile("return.dat");
  retfile << return_status << endl;

  return return_status;

}

//**********************************************************************

#ifdef CTEST_MAIN
int main(int, char** argv) {

  string exe = argv[0];
  if ( verbose > 5 ) cout << "Running " << exe << endl;
  if ( verbose > 6 ) system("echo with path $LD_LIBRARY_PATH");

  return ctest(exe);

}
#endif

//**********************************************************************
