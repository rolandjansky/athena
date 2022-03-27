// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFXAOD_XAODMENUWRITERMT_H
#define TRIGCONFXAOD_XAODMENUWRITERMT_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <string>
#include <set>
#include <map> // For std::pair...

#include "TrigConfxAOD/IKeyWriterTool.h"

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "CxxUtils/checker_macros.h"

// Trigger include(s):
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/HLTMonitoring.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/HLTPrescalesSet.h"
#include "TrigConfData/L1PrescalesSet.h"
#include "TrigConfData/L1BunchGroupSet.h"

// EDM include(s):
#include "xAODTrigger/TriggerMenuJsonContainer.h"

namespace TrigConf {

   /**
    *  @short Algorithm used to write the light-weight xAOD configuration in the Run3 JSON format
    *
    *         This algorithm needs to be scheduled in jobs that write xAOD
    *         files in Athena in order to write the trigger configuration
    *         into the output file.
    *
    *         It puts a tiny amount of information into each event that is
    *         later used to find the correct configuration of the events,
    *         and also assembles the configuration metadata that is written
    *         into the metadata TTree of the xAOD file at the end of the job.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    * @author Tim Martin <Tim.Martin@cern.ch>
    *
    */
   class xAODMenuWriterMT : public AthReentrantAlgorithm {

   public:
      /// Regular Algorithm constructor
      xAODMenuWriterMT( const std::string& name, ISvcLocator* svcLoc );

      virtual ~xAODMenuWriterMT();

      /// Function initialising the algorithm
      virtual StatusCode initialize() override;

      /// Function executing the algorithm
      virtual StatusCode execute(const EventContext& ctx) const override;

   private:

      SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey {this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu",
        "HLT Menu key"};

      SG::ReadHandleKey<TrigConf::HLTMonitoring> m_HLTMonitoringKey {this, "HLTMonitoringMenu", "DetectorStore+HLTMonitoringMenu",
        "HLT Monitoring key"};

      SG::ReadHandleKey<TrigConf::L1Menu> m_L1MenuKey {this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu",
        "L1 Menu key"};

      SG::ReadCondHandleKey<TrigConf::HLTPrescalesSet> m_HLTPrescaleSetInputKey{this, "HLTPrescales", "HLTPrescales", 
        "HLT prescales set condition handle"};

      SG::ReadCondHandleKey<TrigConf::L1PrescalesSet> m_L1PrescaleSetInputKey{this, "L1Prescales", "L1Prescales", 
        "L1 prescales set condition handle"};

      SG::ReadCondHandleKey<TrigConf::L1BunchGroupSet> m_bgInputKey{this, "L1BunchGroup", "L1BunchGroup", 
        "L1BunchGroupSet condition handl"};

      Gaudi::Property< std::string > m_metaNameJSON_hlt {this, "JSONMetaObjectNameHLT", "TriggerMenuJson_HLT",
        "StoreGate key for the xAOD::TriggerMenuJson HLT configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_hltmonitoring {this, "JSONMetaObjectNameHLTMonitoring", "TriggerMenuJson_HLTMonitoring",
        "StoreGate key for the xAOD::TriggerMenuJson HLT Monitoring configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_l1 {this, "JSONMetaObjectNameL1", "TriggerMenuJson_L1",
        "StoreGate key for the xAOD::TriggerMenuJson L1 configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_hltps {this, "JSONMetaObjectNameHLTPS", "TriggerMenuJson_HLTPS",
        "StoreGate key for the xAOD::TriggerMenuJson HLT prescales configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_l1ps {this, "JSONMetaObjectNameL1PS", "TriggerMenuJson_L1PS",
        "StoreGate key for the xAOD::TriggerMenuJson L1 prescales configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_bg {this, "JSONMetaObjectNameBunchgroup", "TriggerMenuJson_BG",
        "StoreGate key for the xAOD::TriggerMenuJson BunchGroup configuration object"};

      ServiceHandle< StoreGateSvc > m_metaStore {this, "MetaDataStore", "MetaDataStore",
        "The MetaDataStore"};

      ToolHandle<IKeyWriterTool> m_keyWriterTool{this, "KeyWriterTool", "KeyWriterTool/OfflineKeyWriterTool", 
          "Writes the keys which are configured when the trigger configuration is written to in-file metadata."};

      /// Trigger configuration key type (used just internally)
      typedef std::pair< uint32_t, std::pair< uint32_t, uint32_t > > TrigKey_t;

      //  The configuration objects that we are writing
      mutable xAOD::TriggerMenuJsonContainer* m_menuJSON_hlt ATLAS_THREAD_SAFE;
      mutable xAOD::TriggerMenuJsonContainer* m_menuJSON_hltmonitoring ATLAS_THREAD_SAFE;
      mutable xAOD::TriggerMenuJsonContainer* m_menuJSON_l1 ATLAS_THREAD_SAFE;
      mutable xAOD::TriggerMenuJsonContainer* m_menuJSON_hltps ATLAS_THREAD_SAFE;
      mutable xAOD::TriggerMenuJsonContainer* m_menuJSON_l1ps ATLAS_THREAD_SAFE;
      mutable xAOD::TriggerMenuJsonContainer* m_menuJSON_bg ATLAS_THREAD_SAFE;

      /// Trigger configuration keys that are already converted
      mutable std::set< uint32_t > m_converted_smk ATLAS_THREAD_SAFE;
      mutable std::set< uint32_t > m_converted_hltpsk ATLAS_THREAD_SAFE;
      mutable std::set< uint32_t > m_converted_l1psk ATLAS_THREAD_SAFE;
      mutable std::set< uint32_t > m_converted_bg ATLAS_THREAD_SAFE;

      /// The mutex to prevent us from writing more than one configuration at a time
      mutable std::mutex m_mutex;

   }; // class xAODMenuWriterMT

} // namespace TrigConf

#endif // TRIGCONFXAOD_XAODMENUWRITERMT_H
