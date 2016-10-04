/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigConf2CoolOnline.cxx
 * @author Joerg Stelzer
 * $Author: stelzer $
 * $Revision: 1.1 $
 * $Date: 2008-07-28 13:58:08 $
 *
 * @brief Application to write trigger configuration from TriggerDB to COOL
 *
 */

#undef PACKAGE_VERSION

#include "TrigConf2COOLOnline/OnlTrigC2CWriter.h"

#include "ipc/core.h"
#include "ers/ers.h"
#include "cmdl/cmdargs.h"

#include <string>
#include "RunControl/Common/CmdLineParser.h"
#include "RunControl/Common/Exceptions.h"
#include "RunControl/ItemCtrl/ItemCtrl.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace std;
using namespace daq;
using namespace TrigConf;

ERS_DECLARE_ISSUE (TrigConf, ExitNow, "FATAL ERROR: ", )




ERS_DECLARE_ISSUE(TrigConf, Exception, ERS_EMPTY, ERS_EMPTY)

ERS_DECLARE_ISSUE_BASE (  TrigConf,
                          CmdLineError,
                          TrigConf::Exception,
                          "Error parsing the command line options: " << reason,
                          ERS_EMPTY,
                          ((std::string) reason)
                          )



int main (int argc, char **argv) {

   for(int i=0; i<argc; ++i)
      cout << argv[i] << " ";
   cout << endl;
  
   try {
      IPCCore::init(argc,argv);
   }
   catch(daq::ipc::CannotInitialize& e) {
      ers::fatal(e);
      abort();
   }
   catch(daq::ipc::AlreadyInitialized& e) {
      ers::warning(e);
   }
  
   // parse commandline parameters
   po::options_description desc("This program writes the trigger configuration for the current run to COOL");
   desc.add_options()
      ("help,h", "print usage and exit")
      ("cooldb",  po::value<string>(), "COOL db connection tech://schema=;dbname=\nsqlite://;schema=trigconf.db;dbname=TRIGCONF")
      ("triggerdb",  po::value<string>(), "TriggerDB connection alias")
      ("l1is",  "if set L1 key is written to IS")
      ("l1cool", "if set L1 information is written to COOL")
      ("name,n",  po::value<string>(), "application name")
      ("mckcool", "if set MCK is written to COOL")
      ;
  
   // Get commandline parameters
   po::variables_map vm;
   po::store( po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);
   po::notify(vm);
  
   string cooldb("sqlite://;schema=trigconf.db;dbname=TRIGCONF");

   if(vm.count("cooldb")) {
      cooldb = vm["cooldb"].as<string>();
      cout << "Application parameter cooldb=" << cooldb << endl;
   } else if(!vm.count("help")) {
      ers::fatal( TrigConf::CmdLineError(ERS_HERE, "No cool connection parameter has been defined") );
      return EXIT_FAILURE;
   }
   bool l1is = vm.count("l1is")>0;
   bool l1cool = vm.count("l1cool")>0;
   bool mckcool = vm.count("mckcool")>0;
   string triggerdb( vm.count("triggerdb") ? vm["triggerdb"].as<string>() : string("") );

   rc::CmdLineParser cmdParser(argc, argv, true);

   try {
      rc::ItemCtrl itemCtrl(cmdParser,
                            shared_ptr<rc::Controllable>(new OnlTrigC2CWriter(cmdParser.partitionName(), cooldb, triggerdb, l1is, l1cool, mckcool)));
      itemCtrl.init();
      itemCtrl.run();
   }
   catch(daq::rc::CmdLineHelp& ex) {
      cout << desc << endl;
      cout << ex.message() << endl;
   }
   catch(ers::Issue& ex) {
      TrigConf::ExitNow issue(ERS_HERE, ex);
      ers::fatal(ex);
      return EXIT_FAILURE;
   }
   catch(po::error& ex) {
      ers::fatal(rc::CmdLineError(ERS_HERE, ex.what(), ex));
      return EXIT_FAILURE;
   }
  
   return EXIT_SUCCESS;

}
