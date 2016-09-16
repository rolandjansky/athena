/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*

* @file HLTCountsArchiverRCApp.h
* @author Joerg Stelzer
* $Author: stelzer $
* $Revision: 1.1 $
* $Date: 2010-02-05 $
*
* @brief the class for writing HLT trigger counts to COOL online
*/

#ifndef hltca_HLTCountsArchiverRCApp
#define hltca_HLTCountsArchiverRCApp

// This macro interferes with some tdaq headers:
#undef PACKAGE_VERSION

#include <string>

#include "HLTRates2COOL/JobConfig.h"
#include "RunControl/Common/Controllable.h"
#include "HLTRates2COOL/HLTCountCoolWriter.h"

#include "ipc/core.h"

namespace hltca {
   class HLTCountsArchiverRCApp;
   class HLTCountOHReceiver;
   class JobConfig;
}

class IPCPartition;
class OHSubscriber;

class hltca::HLTCountsArchiverRCApp : public daq::rc::Controllable {
public:
   /**
    * Constructor.  Only the parameter name is required (this is the named published in IPC.)
    */
   HLTCountsArchiverRCApp(JobConfig& conf, IPCPartition* part, hltca::HLTCountOHReceiver * rec);
   virtual ~HLTCountsArchiverRCApp() noexcept;

   virtual void prepareForRun(const daq::rc::TransitionCmd&);
   virtual void stopArchiving(const daq::rc::TransitionCmd&);

   hltca::JobConfig& conf() { return fJobConfig; }

private:

   void    writeToCool();  // writes the objects held by the receiver to cool

   bool getRunNumberFromIS();

   hltca::JobConfig             fJobConfig; // object to hold different job configuration parameters
   hltca::HLTCountCoolWriter    fCoolWriter; // the class that does the writing to COOL
   IPCPartition*                fIPCPartition; // the ipc partition object
   hltca::HLTCountOHReceiver*   fReceiver;

};

#endif
