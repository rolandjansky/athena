/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <string>
#include <unistd.h>

#include "boost/lexical_cast.hpp"

#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfStorage/StorageMgr.h"


using namespace std;
using namespace TrigConf;


int main( int argc, char* argv[] ) {

   if(argc==2 && string(argv[1])=="-h") {
      cout << "Usage:\n" << argv[0] << " [TRIGGERDB] [L1PSK]" << endl;
      return 0;
   }
  
   string triggerdb = "TRIGGERDB";
   uint32_t psk = 1;

   if(argc>1)
      triggerdb = string(argv[1]);

   if(argc>2)
      psk = boost::lexical_cast<uint32_t,string>(string(argv[2]));

   unique_ptr<StorageMgr> sm(new StorageMgr(triggerdb));
   
   IPrescaleSetLoader&  pssLoader = sm->prescaleSetLoader();

   while(1) {

      cout << "Reading prescale key " << psk << endl;
      
      PrescaleSet pss;
      pss.setId(psk);

      try {
         pssLoader.load(pss);
      }
      catch(std::runtime_error & e) {
         cout << "Runtime error caught: " << e.what() << endl;
      }

      pss.print("",1);

      int wait(3);
      cout << "Waiting for " << wait << " seconds" << endl;
      if( sleep(wait) != 0)
         break;
   }

}

