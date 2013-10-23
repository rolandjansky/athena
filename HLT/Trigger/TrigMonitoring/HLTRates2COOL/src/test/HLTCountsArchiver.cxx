/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>

#include <boost/program_options.hpp>

#include "HLTRates2COOL/HLTCountsArchiverRCApp.h"


#include "ipc/core.h"

#include "ers/ers.h"
//#include "cmdl/cmdargs.h"

#include <string>
#include "RunControl/Common/CmdLineParser.h"
#include "RunControl/Common/Exceptions.h"
#include "RunControl/ItemCtrl/ItemCtrl.h"

#include <owl/regexp.h>


#include "HLTRates2COOL/JobConfig.h"
#include "HLTRates2COOL/HLTCountOHReceiver.h"

// OH include files
// #include <oh/OHRootReceiver.h>
// #include <oh/OHSubscriber.h>
// #include "HLTRates2COOL/HLTCountOHReceiver.h"

ERS_DECLARE_ISSUE (HLTCountsArchiver, ExitNow, "FATAL ERROR: ", )

using namespace std;
using namespace daq;

namespace po = boost::program_options;

int main( int argc, char ** argv )
{

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


   // ***
   // define commandline parameters
   // ***

   po::options_description desc("Application that writes the HLT trigger counts for each LB to COOL");
   desc.add_options()
      ("cooldb,d",      boost::program_options::value<std::string>(), "COOL db connection tech://schema=;dbname=\nsqlite://;schema=trigconf.db;dbname=TRIGCONF")
      ("server",        boost::program_options::value<std::string>(), "IS server name (default 'HistogrammingHLT')")
      ("l2provider",    boost::program_options::value<std::string>(), "OH provider name (default 'Top-LVL2-L2-Segment-1-Gatherer')")
      ("l2hist",        boost::program_options::value<std::string>(), "histogram name pattern (default '/LB/EXPERT/TrigSteer_L2/SignatureAcceptance')")
      ("efprovider",    boost::program_options::value<std::string>(), "OH provider name (default 'Top-EF-EBEF-Segment')")
      ("efhist",        boost::program_options::value<std::string>(), "histogram name pattern (default '/LB/EXPERT/TrigSteer_EF/SignatureAcceptance')");


   // ***
   // Get commandline parameters
   // ***
   rc::CmdLineParser cmdParser(argc, argv, true);

   po::variables_map vm;
   po::store(boost::program_options::parse_command_line(argc,argv,desc), vm);
   po::notify(vm);

   hltca::JobConfig conf;

   // ***
   // define job defaults
   // ***

   conf.PartitionName() = cmdParser.partitionName();
   conf.CoolDb()        = "sqlite://;schema=testhltcounters.db;dbname=COMP200";
   conf.ISServer()      = "HistogrammingHLT";
   conf.Provider("L2")  = "Top-LVL2-L2-Segment-1-Gatherer";
   conf.Histogram("L2") = "/LB/EXPERT/TrigSteer_L2/SignatureAcceptance";
   conf.Provider("EF")  = "Top-EF-EBEF-Segment";
   conf.Histogram("EF") = "/LB/EXPERT/TrigSteer_EF/SignatureAcceptance";


   /****
    * set job configuration from command line parameters
    */
   if(vm.count("cooldb"))      conf.CoolDb()        = vm["cooldb"].as<std::string>();
   if(vm.count("server"))      conf.ISServer()      = vm["server"].as<std::string>();
   if(vm.count("l2provider"))  conf.Provider("L2")  = vm["l2provider"].as<std::string>();
   if(vm.count("l2hist"))      conf.Histogram("L2") = vm["l2hist"].as<std::string>();
   if(vm.count("efprovider"))  conf.Provider("EF")  = vm["efprovider"].as<std::string>();
   if(vm.count("efhist"))      conf.Histogram("EF") = vm["efhist"].as<std::string>();
   

   std::cout << conf << std::endl;

   // ***
   // get partition object
   // ***
   
   IPCPartition *partition = new IPCPartition(conf.PartitionName());

   // ***
   // create the receiver and subscribe it to the desired histogram updates
   // ***

   hltca::HLTCountOHReceiver * receiver = new hltca::HLTCountOHReceiver();

   bool serialise_callbacks = true; // avoid receiving of multiple histogramms at the same time
   OHSubscriber ohhs( *partition, conf.ISServer(), *receiver, serialise_callbacks );
   bool value_callback = true;  // false==event_callback (calls receiver.objectChanged)
   OWLRegexp provl2(conf.Provider("L2"));
   OWLRegexp histl2(conf.Histogram("L2"));
   ohhs.subscribe( provl2, histl2, value_callback ); // last value: subscribe for receiving histogram values (default off)
   OWLRegexp provef(conf.Provider("EF"));
   OWLRegexp histef(conf.Histogram("EF"));
   ohhs.subscribe( provef, histef, value_callback ); // last value: subscribe for receiving histogram values (default off)


   // ***
   // run the RC application
   // ***
   try {
      rc::ItemCtrl itemCtrl(cmdParser,
                            shared_ptr<rc::Controllable>(new hltca::HLTCountsArchiverRCApp(conf, partition, receiver)));
      itemCtrl.init();
      itemCtrl.run();
   }
   catch(daq::rc::CmdLineHelp& ex) {
      cout << desc << endl;
      cout << ex.message() << endl;
   }
   catch (const std::exception &ex) {
      HLTCountsArchiver::ExitNow issue(ERS_HERE, ex);
      ers::fatal(issue);
   }
   catch (...) {
      HLTCountsArchiver::ExitNow issue(ERS_HERE, "unknown exception caught");
      ers::fatal(issue);
   }

   delete receiver;
   delete partition;

   std::exit (EXIT_SUCCESS);




}
