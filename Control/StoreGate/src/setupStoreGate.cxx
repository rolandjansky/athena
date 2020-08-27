/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/src/setupStoreGate.cxx
 * @author scott snyder <snyder@bnl.gov>, after S. Binet's code from TestPolicy.
 * @date Aug, 2014
 * @brief Helper for setting up StoreGate for regression tests.
 */


#include "StoreGate/setupStoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include <fstream>
#include <iostream>
#include <list>
#include <cassert>


namespace Athena_test {


bool setupStoreGate (std::string progname, 
                     std::string options_file /*= ""*/)
{
  // Build job options file if CppUnit_jobOptions.txt is unavailable
  if (options_file.empty()) {
    std::string::size_type pos = progname.rfind ("/");
    if (pos != std::string::npos)
      progname.erase (0, pos+1);
    pos = progname.rfind (".");
    if (pos != std::string::npos)
      progname.erase (pos, std::string::npos);
    options_file = "./" + progname + "_generated.txt";
    (void)remove (options_file.c_str());
    std::ofstream jobopt(options_file.c_str());

    // Build a default list of options.
    {
      std::list<std::string> opts;
      opts.push_back("#pragma print off");
      opts.push_back("ApplicationMgr.Dlls += { \"StoreGate\" };");
      //opts.push_back("ApplicationMgr.Dlls += { \"CLIDSvc\" };");
      opts.push_back("ApplicationMgr.ExtSvc += { \"ClassIDSvc\" };");
      opts.push_back("ApplicationMgr.ExtSvc += { \"StoreGateSvc\", \"StoreGateSvc/DetectorStore\", \"StoreGateSvc/HistoryStore\" };");
      opts.push_back("ApplicationMgr.ExtSvc += { \"ActiveStoreSvc\" };");
      opts.push_back("AuditorSvc.Auditors  += { \"AlgContextAuditor\"};");
      opts.push_back("StoreGateSvc.OutputLevel = 2;");
      opts.push_back("StoreGateSvc.ActivateHistory = false;");
      //opts.push_back("CLIDSvc.OutputLevel = 2;");
      opts.push_back("MessageSvc.useColors = false;");
      opts.push_back("MessageSvc.OutputLevel = 3;");
      opts.push_back("#include \"IOVSvc/IOVSvc.txt\"");
      // Build job options file from list.
      for ( std::list<std::string>::const_iterator iopt=opts.begin();
            iopt!=opts.end(); ++iopt ) 
      {
        jobopt << *iopt << std::endl;
      }
    }
    jobopt.close();
  }

  try {
    /// Get StoreGateSvc
    ISvcLocator* svcLoc = 0;
    if (!Athena_test::initGaudi(options_file, svcLoc)) {
      std::cerr << "This test can not be run" << std::endl;
      return false;
    }  
    assert( 0 != svcLoc);
    
    StoreGateSvc* storeGate = 0;
    static const bool CREATE(true);
    bool sc = ( svcLoc->service( "StoreGateSvc", 
                                 storeGate, CREATE) ).isSuccess();
    assert( sc );
    assert( 0 != storeGate );
    
    if ( false == sc || 0 == storeGate ) {
      std::string error = "No valid pointer to StoreGateSvc !!";
      std::cerr << error << std::endl;
      return false;
    } else {
      return true;
    }
  } catch ( std::exception& e ) {
    std::cerr << "Caught : " << e.what() << std::endl;
    return false;
  }
}


} // namespace Athena_test
