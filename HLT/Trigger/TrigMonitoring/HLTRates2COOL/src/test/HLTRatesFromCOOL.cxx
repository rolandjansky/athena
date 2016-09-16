/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

#include "HLTRates2COOL/HLTCountCoolWriter.h"
#include "HLTRates2COOL/HLTCounter.h"

int main( int argc, char ** argv )
{
   //cool connection

   unsigned int run_number = 0;
   unsigned int lb_number = 2;

   boost::program_options::options_description desc("Application that writes the HLT trigger counts for each LB to COOL");
   desc.add_options()
      ("help,h",           "print usage and exit")
      ("cooldb,d",         boost::program_options::value<std::string>(), "COOL db connection ( default \"COOL_TRIGGER\")")
      ("triggerpattern,t", boost::program_options::value<std::string>(), "application name")
      ("run,r",            boost::program_options::value<unsigned int>(&run_number)->default_value(0), "run (required)")
//       ("lb,l",             boost::program_options::value<unsigned int>(&lb_number), "lb")
      ;


   // ***
   // Get commandline parameters
   // ***

   boost::program_options::variables_map vm;
   boost::program_options::store(boost::program_options::parse_command_line(argc,argv,desc), vm);
   boost::program_options::notify(vm);

   if(vm.count("help")) {
      std::cout << desc << std::endl;
      return EXIT_SUCCESS;
   }


   std::string triggerpattern( "" );
   std::string coolConnection( "COOLONL_TRIGGER/COMP200" );
   // or "sqlite://;schema=trigconf.db;dbname=COMP200"

   if(vm.count("cooldb")) coolConnection = vm["cooldb"].as<std::string>();
   if(vm.count("triggerpattern")) triggerpattern = vm["triggerpattern"].as<std::string>();
//    if(vm.count("run")) run_number = vm["run"].as<unsigned int>();
//    if(vm.count("lb")) lb_number = vm["lb"].as<unsigned int>();

   if(run_number == 0) {
      std::cout << "run number required!" << std::endl << desc << std::endl;
      return EXIT_SUCCESS;
   }
   


   hltca::HLTCountCoolWriter coolWriter(coolConnection);
 
   std::vector<hltca::HLTCounter> counters;

   coolWriter.readHLTCountersPayload(run_number,lb_number,counters,hltca::HLTCounter::L2);
   
   for(std::vector<hltca::HLTCounter>::iterator it = counters.begin();
       it != counters.end(); it++) {
      if (it->input()<=0) continue;
      std::cout << *it << std::endl;
   }

   return 0;
}
