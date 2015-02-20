/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CoolKernel/Exception.h"
#include "HLTRates2COOL/HLTCountsArchiverRCApp.h"
#include "HLTRates2COOL/HLTCountOHReceiver.h"
#include "HLTRates2COOL/HLTCounter.h"

#include "ipc/partition.h"

#include <oh/OHSubscriber.h>

#include "rc/RunParams.h"


hltca::HLTCountsArchiverRCApp::HLTCountsArchiverRCApp(JobConfig& config, IPCPartition* part, hltca::HLTCountOHReceiver * rec) :
   Controllable(),
   fJobConfig(config),
   fCoolWriter(config.CoolDb()),
   fIPCPartition(part),
   fReceiver(rec)
{}

hltca::HLTCountsArchiverRCApp::~HLTCountsArchiverRCApp() noexcept
{}


void 
hltca::HLTCountsArchiverRCApp::prepareForRun(const daq::rc::TransitionCmd&) { 
   std::cout << "HLTCountsArchiverRCApp::prepareForRun: CLEAR RECEIVER" << std::endl;
   fReceiver->ClearStorage();
}

void
hltca::HLTCountsArchiverRCApp::stopArchiving(const daq::rc::TransitionCmd&) {
   std::cout << "HLTCountsArchiverRCApp::stopArchiving: WRITE COOL" << std::endl;
   writeToCool();
}


void
hltca::HLTCountsArchiverRCApp::writeToCool() {

   int lastlb = fReceiver->last_LB();

   if(lastlb<=0) return;  // n_lb<0 -> no LB written, n_lb==0 -> LB0 writter (also not good)

   if(!getRunNumberFromIS()) return;

   bool needToOpen = !fCoolWriter.dbIsOpen();
   bool readOnly = false;
   if(needToOpen) fCoolWriter.openDb(readOnly);

   try{
      fCoolWriter.createSchema();
   } catch(...) {
      // just in case
   }

   try {

      unsigned int n_lb = (unsigned int)lastlb;

      // enable buffered writing
      fCoolWriter.setupStorageBuffers();

      // loop over all lumiblocks
      for(unsigned int lb = 1; lb <= n_lb; ++lb) {

         // loop over both levels
         for(int tl=0; tl<2; ++tl ) {
            HLTCounter::TriggerLevel level = tl==0?HLTCounter::L2:HLTCounter::EF;

            // get the data from the receiver by LB and trigger level
            const std::vector<hltca::HLTCounter>& hltCounters = fReceiver->hltCounters(lb, level);

            //             // example printout for LB 2 and L2
            //             if(lb==2 && level==HLTCounter::L2) {
            //                for(std::vector<hltca::HLTCounter>::const_iterator it = hltCounters.begin(); 
            //                    it != hltCounters.end(); it++) {
            //                   const HLTCounter & hltc = *it;
            //                   std::cout << hltc << std::endl;
            //                }
            //             }

            // write to COOL (note that buffering is switched on, so writing happens only at the end)
            fCoolWriter.writeHLTCountersPayload(conf().CurrentRunNumber(), lb, hltCounters, level);
         }
      }

      fCoolWriter.flushStorageBuffers();

   }
   catch(cool::Exception ex) {
      std::cerr << "cool::exception: Could not write to COOL: " << ex.what() << std::endl;
      return;
   }
   catch(std::exception ex) {
      std::cerr << "std::exception: Could not write to COOL: " << ex.what() << std::endl;
      return;
   }

   if(needToOpen) fCoolWriter.closeDb();
   
   return;
  
}

bool
hltca::HLTCountsArchiverRCApp::getRunNumberFromIS() {
   ISInfoDictionary dict(*fIPCPartition);

   if ( !dict.contains("RunParams.SOR_RunParams") ) {
      std::cerr << "RunParams.SOR_RunParams doesn't exist" << std::endl;
      return false;
   }

   RunParams sor;
   dict.getValue( "RunParams.SOR_RunParams", sor );

   // https://svnweb.cern.ch/trac/atlastdaq/browser/DAQ/online/is/trunk/is/infodictionary.h

   conf().CurrentRunNumber() = sor.run_number;

   ERS_INFO("Received new run number: " << conf().CurrentRunNumber());

   return true;
}

