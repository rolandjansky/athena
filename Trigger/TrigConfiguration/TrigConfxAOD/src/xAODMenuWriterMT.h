// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "StoreGate/StoreGateSvc.h"

// Trigger include(s):
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfInterfaces/IHLTConfigSvc.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/HLTPrescalesSet.h"
#include "TrigConfData/L1PrescalesSet.h"

// EDM include(s):
#include "xAODTrigger/TriggerMenuContainer.h"
#include "xAODTrigger/TriggerMenuJsonContainer.h"
#include "xAODTrigger/TrigConfKeys.h"

namespace TrigConf {

   /**
    *  @short Algorithm used to write the light-weight xAOD configuration
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

      SG::WriteHandleKey<xAOD::TrigConfKeys> m_eventName {this, "EventObjectName", "TrigConfKeys",
        "StoreGate key for the event object"};

      SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey {this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu",
        "HLT Menu key, for use if IsJSONConfig=True"};

      SG::ReadHandleKey<TrigConf::L1Menu> m_L1MenuKey {this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu",
        "L1 Menu key, for use if IsJSONConfig=True"};

      SG::ReadCondHandleKey<TrigConf::HLTPrescalesSet> m_HLTPrescaleSetInputKey{this, "HLTPrescales", "HLTPrescales", 
        "HLT prescales set condition handle"};

      SG::ReadCondHandleKey<TrigConf::L1PrescalesSet> m_L1PrescaleSetInputKey{this, "L1Prescales", "L1Prescales", 
        "L1 prescales set condition handle"};

      Gaudi::Property< std::string > m_metaName {this, "MetaObjectName", "TriggerMenu",
        "StoreGate key for the xAOD::TriggerMenu configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_hlt {this, "JSONMetaObjectNameHLT", "TriggerMenuJson_HLT",
        "StoreGate key for the xAOD::TriggerMenuJson HLT configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_l1 {this, "JSONMetaObjectNameL1", "TriggerMenuJson_L1",
        "StoreGate key for the xAOD::TriggerMenuJson L1 configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_hltps {this, "JSONMetaObjectNameHLTPS", "TriggerMenuJson_HLTPS",
        "StoreGate key for the xAOD::TriggerMenuJson HLT prescales configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_l1ps {this, "JSONMetaObjectNameL1PS", "TriggerMenuJson_L1PS",
        "StoreGate key for the xAOD::TriggerMenuJson L1 prescales configuration object"};

      // TODO
      // Gaudi::Property< std::string > m_metaNameJSON_bg {this, "JSONMetaObjectNameBunchgroup", "TriggerMenuJson_BG",
      //   "StoreGate key for the xAOD::TriggerMenuJson BunchGroup configuration object"};

      Gaudi::Property< bool > m_isL1JSONConfig {this, "IsL1JSONConfig", true,
        "If converting from a L1 JSON menu (Run3) or from the TrigConfigSvc (Runs 1, 2)"};

      Gaudi::Property< bool > m_isHLTJSONConfig {this, "IsHLTJSONConfig", true,
        "If converting from a HLT JSON menu (Run3) or from the TrigConfigSvc (Runs 1, 2)"};

      Gaudi::Property< bool > m_writexAODTriggerMenu {this, "WritexAODTriggerMenu", true,
        "Flag to control the writing of xAOD::TriggerMenu metadata into the output file. This is the R2 persistent format."}; 

      Gaudi::Property< bool > m_writexAODTriggerMenuJson {this, "WritexAODTriggerMenuJson", true,
        "Flag to control the writing of xAOD::TriggerMenuJson metadata into the output file. This is the R3 persistent format."};

      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_l1TrigConf{this, "LVL1ConfigSvc", "LVL1ConfigSvc", "The LVL1 TrigConfigSvc"};

      ServiceHandle<TrigConf::IHLTConfigSvc> m_hltTrigConf{this, "HLTConfigSvc", "HLTConfigSvc", "The HLT TrigConfigSvc"};

      ServiceHandle< StoreGateSvc > m_metaStore {this, "MetaDataStore", "MetaDataStore",
        "The MetaDataStore"};

      ToolHandle<IKeyWriterTool> m_keyWriterTool{this, "KeyWriterTool", "KeyWriterTool/OfflineKeyWriterTool", 
          "Writes the keys which are configured when the trigger configuration is written to in-file metadata."};

      StatusCode populateL1FromTrigConf(xAOD::TriggerMenu* menu) const;

      StatusCode populateHLTFromTrigConf(xAOD::TriggerMenu* menu) const;

      StatusCode populateL1FromJSON(xAOD::TriggerMenu* menu, const EventContext& ctx) const;

      StatusCode populateHLTFromJSON(xAOD::TriggerMenu* menu, const EventContext& ctx) const;

      StatusCode populateBunchGroup(xAOD::TriggerMenu* menu) const;

      /// Trigger configuration key type (used just internally)
      typedef std::pair< uint32_t, std::pair< uint32_t, uint32_t > > TrigKey_t;

      /// The configuration object that we are writing when WritexAODTriggerMenu
      mutable xAOD::TriggerMenuContainer* m_tmc;

      //  The configuration objects that we are writing when WritexAODTriggerMenuJson
      mutable xAOD::TriggerMenuJsonContainer* m_menuJSON_hlt;
      mutable xAOD::TriggerMenuJsonContainer* m_menuJSON_l1;
      mutable xAOD::TriggerMenuJsonContainer* m_menuJSON_hltps;
      mutable xAOD::TriggerMenuJsonContainer* m_menuJSON_l1ps;
      // mutable xAOD::TriggerMenuJsonContainer* m_menuJSON_bg;

      /// Trigger configuration keys that are already converted when WritexAODTriggerMenu is true
      mutable std::set< TrigKey_t > m_convertedKeys;

      /// Trigger configuration keys that are already converted when WritexAODTriggerMenuJson is true
      mutable std::set< uint32_t > m_converted_smk;
      mutable std::set< uint32_t > m_converted_hltpsk;
      mutable std::set< uint32_t > m_converted_l1psk;
      // mutable std::set< uint32_t > m_converted_bg;

      /// The mutex to prevent us from writing more than one configuration at a time
      mutable std::mutex m_mutex;

   }; // class xAODMenuWriterMT

} // namespace TrigConf

#endif // TRIGCONFXAOD_XAODMENUWRITERMT_H
