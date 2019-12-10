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

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger include(s):
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"

// EDM include(s):
#include "xAODTrigger/TriggerMenuContainer.h"
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
      /// 
      SG::WriteHandleKey<xAOD::TrigConfKeys> m_eventName {this, "EventObjectName", "TrigConfKeys",
        "StoreGate key for the event object"};

       SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey {this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu",
        "HLT Menu key, for use if IsJSONConfig=True"};

       SG::ReadHandleKey<TrigConf::L1Menu> m_L1MenuKey {this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu",
        "L1 Menu key, for use if IsJSONConfig=True"};

      Gaudi::Property< std::string > m_metaName {this, "MetaObjectName", "TriggerMenu",
        "StoreGate key for the configuration object"};

      Gaudi::Property< bool > m_isJSONConfig {this, "IsJSONConfig", true,
        "If converting from a JSON menu (Run3) or from the TrigConfigSvc (Runs 1, 2)"};

      ServiceHandle< TrigConf::ITrigConfigSvc > m_trigConf {this, "TrigConfigSvc", "TrigConfigSvc",
        "The TrigConfigSvc"};

      ServiceHandle< StoreGateSvc > m_metaStore {this, "MetaDataStore", "MetaDataStore",
        "The MetaDataStore"};

      StatusCode populateFromTrigConf(xAOD::TriggerMenu* menu) const;

      StatusCode populateFromJSON(xAOD::TriggerMenu* menu, const EventContext& ctx) const;

      StatusCode populateBunchGroup(xAOD::TriggerMenu* menu) const;

      /// Trigger configuration key type (used just internally)
      typedef std::pair< uint32_t, std::pair< uint32_t, uint32_t > > TrigKey_t;

      /// The configuration object that we are writing
      mutable xAOD::TriggerMenuContainer* m_tmc;

      /// Trigger configuration keys that are already converted
      mutable std::set< TrigKey_t > m_convertedKeys;

      /// The mutex to prevent us from writing more than one configuration at a time
      mutable std::mutex m_mutex;

   }; // class xAODMenuWriterMT

} // namespace TrigConf

#endif // TRIGCONFXAOD_XAODMENUWRITERMT_H
