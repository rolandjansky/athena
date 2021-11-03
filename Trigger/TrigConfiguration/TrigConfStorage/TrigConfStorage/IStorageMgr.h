/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_IStorageMgr
#define TrigConf_IStorageMgr 

#include "TrigConfBase/MsgStream.h"

#include "TrigConfStorage/ILoader.h"
#include "TrigConfStorage/IBunchGroupLoader.h"
#include "TrigConfStorage/IBunchGroupSetLoader.h"
#include "TrigConfStorage/ICTPConfigLoader.h"
#include "TrigConfStorage/ICTPFilesLoader.h"
#include "TrigConfStorage/ICaloInfoLoader.h"
#include "TrigConfStorage/ICaloSinCosLoader.h"
#include "TrigConfStorage/ICaloJetInputLoader.h"
#include "TrigConfStorage/IDeadTimeLoader.h"
#include "TrigConfStorage/IHLTFrameLoader.h"
#include "TrigConfStorage/IHLTPrescaleSetLoader.h"
#include "TrigConfStorage/IHLTPrescaleSetCollectionLoader.h"
#include "TrigConfStorage/IJobOptionTableLoader.h"
#include "TrigConfStorage/ILutCamLoader.h"
#include "TrigConfStorage/IMasterTableLoader.h"
#include "TrigConfStorage/IMenuLoader.h"
#include "TrigConfStorage/IMuctpiLoader.h"
#include "TrigConfStorage/IMuonThresholdSetLoader.h"
#include "TrigConfStorage/IPrescaleSetLoader.h"
#include "TrigConfStorage/IPrescaledClockLoader.h"
#include "TrigConfStorage/IPrioritySetLoader.h"
#include "TrigConfStorage/IRandomLoader.h"
#include "TrigConfStorage/ISequenceContentTableLoader.h"
#include "TrigConfStorage/ISignatureContentTableLoader.h"
#include "TrigConfStorage/IThresholdConfigLoader.h"
#include "TrigConfStorage/IThresholdMonitorLoader.h"
#include "TrigConfStorage/ITriggerThresholdLoader.h"
#include "TrigConfStorage/ITriggerThresholdValueLoader.h"
#include "TrigConfStorage/ITriggerTypeLoader.h"
#include "TrigConfStorage/IL1TopoMenuLoader.h"

#include <iostream>
#include <memory>

namespace TrigConf {

   /**@brief Interface for the StorageMgr (TriggerDB) and XMLStorageMgr (XML) classes*/
   class IStorageMgr {
   public:

      /**@brief destructor*/       
      virtual ~IStorageMgr(){};

      // setting log level of all loaders
      virtual void setLevel(MSGTC::Level) = 0;
      
      // setting CTP and L1 version to be accessible by all the L1 loaders
      virtual void setCTPVersion(unsigned int) = 0;
      virtual void setL1Version(unsigned int) = 0;
      virtual unsigned int ctpVersion() const = 0;
      virtual unsigned int l1Version() const = 0;

      virtual IL1TopoMenuLoader&               l1topoMenuLoader() = 0;
      virtual IMasterTableLoader&              masterTableLoader() = 0;
      virtual IMenuLoader&                     menuLoader() = 0;
      virtual IMuctpiLoader&                   muctpiLoader() = 0;
      virtual IDeadTimeLoader&                 deadTimeLoader() = 0;
      virtual IPrescaledClockLoader&           prescaledClockLoader() = 0;
      virtual IRandomLoader&                   randomLoader() = 0;
      virtual IThresholdConfigLoader&          thresholdConfigLoader() = 0;
      virtual ITriggerThresholdLoader&         triggerThresholdLoader() = 0;
      virtual ITriggerThresholdValueLoader&    triggerThresholdValueLoader() = 0;
      virtual IThresholdMonitorLoader&         thresholdMonitorLoader() = 0;
      virtual ICTPFilesLoader&                 ctpFilesLoader() = 0;
      virtual ICaloJetInputLoader&             caloJetInputLoader() = 0;
      virtual ICaloSinCosLoader&               caloSinCosLoader() = 0;
      virtual ICaloInfoLoader&                 caloInfoLoader() = 0;
      virtual ILutCamLoader&                   lutcamLoader() = 0;
      virtual IPrescaleSetLoader&              prescaleSetLoader() = 0;
      virtual IPrioritySetLoader&              prioritySetLoader() = 0;
      virtual IBunchGroupLoader&               bunchGroupLoader() = 0;
      virtual IBunchGroupSetLoader&            bunchGroupSetLoader() = 0;
      virtual ICTPConfigLoader&                ctpConfigLoader() = 0;
      virtual ICTPConfigLoader&                ctpConfigOnlineLoader() = 0;
      virtual IHLTFrameLoader&                 hltFrameLoader() = 0;
      virtual IJobOptionTableLoader&           jobOptionTableLoader() = 0;

   };

}

#endif
