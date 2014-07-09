/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include <string>
#include <sstream>
#include <stdint.h>

#include "TrigConfigSvc/TrigConfigSvcARA.h"

int main(int argc, char *argv[]) {

   std::string inputFile("AOD.pool.root");

   if(argc>1) inputFile = std::string(argv[1]);

   unsigned int runnumber = 5200; // 106303
   if(argc>2) {
      std::stringstream s(argv[2]);
      s >> runnumber;
   }

   std::cout << "Test program to read configuration from " << inputFile << std::endl;
   
   std::vector< std::string > filenames;
   filenames.push_back(inputFile);
   
   TrigConf::TrigConfigSvcARA tcs( filenames );
   tcs.setVerbose();
   std::cout << "update for runnumber " << runnumber << std::endl;
   tcs.update(runnumber,1);

   std::cout << tcs << std::endl;

   return 1;
}
