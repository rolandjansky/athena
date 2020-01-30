/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file OnlTrigC2CWriter.h
 * @author Joerg Stelzer
 * $Author: stelzer $
 * $Revision: 1.1 $
 * $Date: 2008-07-28 13:58:08 $
 *
 * @brief the online controlable class for writing configuration
 * information from the TriggerDB to COOL
 */

#ifndef TRIGCONF_ONLTRIGC2CWRITER_H
#define TRIGCONF_ONLTRIGC2CWRITER_H

#include <string>
#include <iostream>
#include "TrigConf2COOLOnline/TC2CConfig.h"
#include "RunControl/Common/Controllable.h"
#include "ipc/partition.h"
#include "is/infodictionary.h"

namespace TrigConf {
  class OnlTrigC2CWriter;
  class TrigConfCoolWriter;
  class HLTFrame;
  class CTPConfig;
}

class ISInfoReceiver;
class ISCallbackInfo;
class ISInfoDictionary;
class IPCPartition;

class TrigConf::OnlTrigC2CWriter : public daq::rc::Controllable {
public:

   /**@brief:constructor.
    * @param name: this is the named published in IPC (required)
    */
   OnlTrigC2CWriter(const std::string & partitionName, const std::string & cooldb, const std::string& triggerdb, bool l1is, bool l1cool, bool mckcool = false );
   OnlTrigC2CWriter(const OnlTrigC2CWriter&) = delete;
   OnlTrigC2CWriter& operator=(OnlTrigC2CWriter const&) = delete;
  
   virtual ~OnlTrigC2CWriter() noexcept;
   virtual void configure    (const daq::rc::TransitionCmd&) override;
   virtual void connect      (const daq::rc::TransitionCmd&) override;     //< connect to COOL and to TriggerDB
   virtual void prepareForRun(const daq::rc::TransitionCmd&) override;
   virtual void unconfigure  (const daq::rc::TransitionCmd&) override;
   virtual void user         (const daq::rc::UserCmd& usrCmd) override;
   virtual void stopArchiving(const daq::rc::TransitionCmd& cmd) override;

   TC2CConfig& conf() { return m_jobConfig; }

private:

   bool readOKS(); // reading the configuration from OKS

   bool subscribeToIS(); // subscribe to IS

   void publishToIS(); // publish to IS

   void readTriggerDb(); // read HLT menu from TriggerDB

   void runNumberCallback(ISCallbackInfo *isc);

   void readRunNumberFromIS();

   void writeHLTPrescaleSetToCool(unsigned int lb, unsigned int pskey, std::string& name);

   TC2CConfig                       m_jobConfig; // object to hold different configuration parameters
   TrigConf::TrigConfCoolWriter*    m_conf2Cool; // the class that does all the work
   std::string                      m_triggerDB;
   ISInfoReceiver*                  m_infoRec;
   IPCPartition*                    m_IPCPartition; // the ipc partition object

   TrigConf::HLTFrame*              m_HLTFrame;  // pointer to the HLTFrame (could be skipped at the moment since it is still a singleton)
   TrigConf::CTPConfig*             m_CTPConfig; // the pointer to the lvl1 ctpc;
   bool m_forceTriggerDBReadAtPrepareForRun { false };
    
};

#endif
