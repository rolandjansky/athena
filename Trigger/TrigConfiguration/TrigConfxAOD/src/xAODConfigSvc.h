// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFXAOD_XAODCONFIGSVC_H
#define TRIGCONFXAOD_XAODCONFIGSVC_H

// Gaudi/Athena include(s):
#include "AthenaKernel/SlotSpecificObj.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger include(s):
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequenceList.h"

#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/HLTPrescalesSet.h"
#include "TrigConfData/L1PrescalesSet.h"
#include "TrigConfData/L1BunchGroupSet.h"

// xAOD include(s):
#include "xAODTrigger/TriggerMenu.h"
#include "xAODTrigger/TriggerMenuContainer.h"
#include "xAODTrigger/TriggerMenuAuxContainer.h"
#include "xAODTrigger/TriggerMenuJson.h"
#include "xAODTrigger/TriggerMenuJsonContainer.h"
#include "xAODTrigger/TriggerMenuJsonAuxContainer.h"
#include "xAODTrigger/TrigConfKeys.h"

#include <shared_mutex>

namespace TrigConf {

   /**
    *  @short Small utility class to wrap a pointer to a const xAOD::TriggerMenu
    *
    *         The SG::SlotSpecificObj class invokes an object's default constructor
    *         and does not allow for the object to be later changed, here we wrap
    *         the pointer we want to store in a trivial class such that it can be
    *         updated over time to point to different menus in different slots
    */
   struct MenuPtrWrapper {
      MenuPtrWrapper() : m_ptr(nullptr) {}
      const xAOD::TriggerMenu* m_ptr;
   };

   /**
    *  @short Small utility class to wrap a pointer to a const xAOD::TriggerMenuJson
    */
   struct TriggerMenuJsonPtrWrapper {
      TriggerMenuJsonPtrWrapper() : m_ptr(nullptr) {}
      const xAOD::TriggerMenuJson* m_ptr;
   };

   /**
    *  @short Trigger configuration service used when reading an xAOD file
    *
    *         This implementation of the ITrigConfigSvc interface can be
    *         used when reading an xAOD file in Athena to get the correct
    *         configuration for the current event.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 631651 $
    * $Date: 2014-11-27 19:33:16 +0100 (Thu, 27 Nov 2014) $
    */
   class xAODConfigSvc : public AthService,
                         public virtual ITrigConfigSvc,
                         public virtual IIncidentListener {

   public:
      /// Standard service constructor
      xAODConfigSvc( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the service
      virtual StatusCode initialize() override;
      /// Function finalising the service
      virtual StatusCode finalize() override;

      /// @name Implementation of the IILVL1ConfigSvc interface
      /// @{

      /// Get the LVL1 trigger menu
      virtual const CTPConfig* ctpConfig() const override;

      /// Get the LVL1 threshold configuruation (not available from xAOD)
      virtual const ThresholdConfig* thresholdConfig() const override {
         return 0;
      }

      /// Get the LVL1 bunch group set
      virtual const BunchGroupSet* bunchGroupSet() const override;

      /// Get the LVL1 prescale key
      virtual uint32_t lvl1PrescaleKey() const override;

      /// @}

      /// @name Implementation of the IIHLTConfigSvc interface
      /// @{

      /// Get the HLT chains
      virtual const HLTChainList& chains() const override;

      /// Get the HLT sequences
      virtual const HLTSequenceList& sequences() const override;

      /// Get the Super Master Key
      virtual uint32_t masterKey() const override;

      /// Get the HLT prescale key
      virtual uint32_t hltPrescaleKey() const override;

      /// @}

      /// @name Implementation of the IL1TopoConfigSvc interface
      /// @{

      /// Get the LVL1 topo menu (not available from xAOD)
      virtual const TXC::L1TopoMenu* menu() const override {
         return 0;
      }

      /// @}

      /// @name Dummy implementation of the ILVL1ConfigSvc interface
      /// @{

      /// Get the MuCTPI's online configuration
      virtual const Muctpi* muctpiConfig() const override {
         return 0;
      }

      /// @}

      /// @name Impliment the JSON config interface.
      /// @{

      /// Returns the JSON configured HLTMenu ptree
      virtual const HLTMenu& hltMenu(const EventContext& ctx = Gaudi::Hive::currentContext()) const override;

      /// Returns the JSON configured L1 ptree
      virtual const L1Menu& l1Menu(const EventContext& ctx = Gaudi::Hive::currentContext()) const override;

      /// Returns the JSON configured HLT prescales ptree
      virtual const HLTPrescalesSet& hltPrescalesSet(const EventContext& ctx = Gaudi::Hive::currentContext()) const override;

      /// Returns the JSON configured L1 prescales ptree
      virtual const L1PrescalesSet& l1PrescalesSet(const EventContext& ctx = Gaudi::Hive::currentContext()) const override;

      /// Returns the JSON configured bunchgroup ptree
      virtual const L1BunchGroupSet& l1BunchGroupSet(const EventContext& ctx = Gaudi::Hive::currentContext()) const override;

      /// @}

      /// Function describing to Gaudi the interface(s) implemented
      virtual StatusCode queryInterface( const InterfaceID& riid,
                                         void** ppvIf ) override;

      /// Function handling the incoming incidents
      virtual void handle( const Incident& inc ) override;

      std::string configurationSource() const override {
         return "";
      }

   private:
      /// Function reading in a new metadata object from the input
      StatusCode readMetadata();
      /// Function setting up the service for a new event
      StatusCode prepareEvent();

      /// Helper function for copying into the service's private data store
      void copyMetadataToPersonalStore(const xAOD::TriggerMenuJsonContainer* input, xAOD::TriggerMenuJsonContainer* existing);

      /// Do per-event updating of R3 JSON-based metadata, reading the data direct from the Conditions and Detector stores (UseInFileMetadata=False case).
      StatusCode prepareEventRun3Athena(const EventContext& context);

      /// Do per-event decoding for R3 in-file serialised xAOD::TriggerMenuJson metadata
      StatusCode prepareEventxAODTriggerMenuJson(const xAOD::TrigConfKeys* keys, const EventContext& context);

      /// Do per-event decoding for R2 in-file serialised xAOD::TriggerMenu metadata 
      StatusCode prepareEventxAODTriggerMenu(const xAOD::TrigConfKeys* keys, const EventContext& context);

      /// Helper function to find a JSON in a given TriggerMenuJsonContainer using a given key, extract its ptree data
      /// @param humanName Name to print if things go wrong
      /// @param metaContainer Metadata container of TriggerMenuJson objects from which to load a ptree
      /// @param keyToCheck The key of the ptree to load
      /// @param cacheOfLoadedMenuPtr Slot's cache of the currently loaded TriggerMenuJson
      /// @param DataStructure dataStructure object to fill with the TriggerMenuJson's payload
      StatusCode loadPtree(const std::string& humanName, 
                           const xAOD::TriggerMenuJsonContainer* metaContainer, 
                           const uint32_t keyToCheck,
                           TriggerMenuJsonPtrWrapper& cacheOfLoadedMenuPtr,
                           DataStructure& dataStructure);

      SG::ReadHandleKey<xAOD::TrigConfKeys> m_eventKey{this, "EventObjectName", "TrigConfKeys", 
        "Key for the event-level configuration identifier object"};

      /// @name Names for reading the R2 (and R1) AOD metadata payload
      /// @{
      Gaudi::Property<std::string> m_metaName{this, "MetaObjectName", "TriggerMenu", 
        "Key for the trigger configuration metadata object"};
      /// @}

      /// @name Names for reading the R3 AOD metadata payload
      /// @{
      Gaudi::Property< std::string > m_metaNameJSON_hlt {this, "JSONMetaObjectNameHLT", "TriggerMenuJson_HLT",
        "StoreGate key for the xAOD::TriggerMenuJson HLT configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_l1 {this, "JSONMetaObjectNameL1", "TriggerMenuJson_L1",
        "StoreGate key for the xAOD::TriggerMenuJson L1 configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_hltps {this, "JSONMetaObjectNameHLTPS", "TriggerMenuJson_HLTPS",
        "StoreGate key for the xAOD::TriggerMenuJson HLT prescales configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_l1ps {this, "JSONMetaObjectNameL1PS", "TriggerMenuJson_L1PS",
        "StoreGate key for the xAOD::TriggerMenuJson L1 prescales configuration object"};

      Gaudi::Property< std::string > m_metaNameJSON_bg {this, "JSONMetaObjectNameBunchgroup", "TriggerMenuJson_BG",
        "StoreGate key for the xAOD::TriggerMenuJson BunchGroup configuration object"};
      /// @}

      /// @name Names for reading the R3 payload directly (RAWtoESD, RAWtoALL)
      /// @{

      Gaudi::Property< std::string > m_hltMenuName{this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu",
        "HLT Menu Key, for when UseInFileMetadata=False. Not a ReadHandleKey, as from the DetectorStore."};

      Gaudi::Property< std::string > m_l1MenuName{this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu",
        "L1 Menu Key, for when UseInFileMetadata=False. Not a ReadHandleKey, as from the DetectorStore"};

      SG::ReadCondHandleKey<TrigConf::HLTPrescalesSet> m_HLTPrescaleSetKey{this, "HLTPrescales", "HLTPrescales", 
         "HLT prescales set condition handle, for when UseInFileMetadata=False"};
 
       SG::ReadCondHandleKey<TrigConf::L1PrescalesSet> m_L1PrescaleSetKey{this, "L1Prescales", "L1Prescales", 
         "L1 prescales set condition handle, for when UseInFileMetadata=False"};
      /// @}

      Gaudi::Property<bool> m_useInFileMetadata{this, "UseInFileMetadata", true, "Flag for reading all configuration from the input POOL file(s). "
        "This mode should be used everywhere except for: RAWtoALL from bytestream, RAWtoESD from bytestream. "
        "If set to false, only the R3 configuration is supported."};

      Gaudi::Property<bool> m_stopOnFailure{this, "StopOnFailure", true, "Flag for stopping the job in case of a failure"};
      /// Internal state of the service
      bool m_isInFailure;

      /// 
      /// @name The configuration objects copied from all input files. R1 and R2 AOD
      /// @{
      std::unique_ptr<xAOD::TriggerMenuAuxContainer> m_tmcAux;
      std::unique_ptr<xAOD::TriggerMenuContainer> m_tmc;

      // The menu currently being used by each slot (wrapped 'const xAOD::TriggerMenu' ptr)
      SG::SlotSpecificObj<MenuPtrWrapper> m_menu;
      /// @}

      /// 
      /// @name The configuration objects copied from all input files. R3 AOD
      /// @{
      std::unique_ptr<xAOD::TriggerMenuJsonAuxContainer> m_hltJsonAux;
      std::unique_ptr<xAOD::TriggerMenuJsonContainer> m_hltJson;
      std::unique_ptr<xAOD::TriggerMenuJsonAuxContainer> m_l1JsonAux;
      std::unique_ptr<xAOD::TriggerMenuJsonContainer> m_l1Json;
      std::unique_ptr<xAOD::TriggerMenuJsonAuxContainer> m_hltpsJsonAux;
      std::unique_ptr<xAOD::TriggerMenuJsonContainer> m_hltpsJson;
      std::unique_ptr<xAOD::TriggerMenuJsonAuxContainer> m_l1psJsonAux;
      std::unique_ptr<xAOD::TriggerMenuJsonContainer> m_l1psJson;
      std::unique_ptr<xAOD::TriggerMenuJsonAuxContainer> m_bgJsonAux;
      std::unique_ptr<xAOD::TriggerMenuJsonContainer> m_bgJson;

      // The menu JSONs being used in the current event by each slot (wrapped 'const xAOD::TriggerMenuJson' ptr)
      SG::SlotSpecificObj<TriggerMenuJsonPtrWrapper> m_currentHltJson;
      SG::SlotSpecificObj<TriggerMenuJsonPtrWrapper> m_currentL1Json;
      SG::SlotSpecificObj<TriggerMenuJsonPtrWrapper> m_currentHltpsJson;
      SG::SlotSpecificObj<TriggerMenuJsonPtrWrapper> m_currentL1psJson;
      SG::SlotSpecificObj<TriggerMenuJsonPtrWrapper> m_currentBgJson;

      // The decoded menu JSON data stored in ptree objects
      SG::SlotSpecificObj<HLTMenu> m_currentHlt;
      SG::SlotSpecificObj<L1Menu> m_currentL1;
      SG::SlotSpecificObj<HLTPrescalesSet> m_currentHltps;
      SG::SlotSpecificObj<L1PrescalesSet> m_currentL1ps;
      SG::SlotSpecificObj<L1BunchGroupSet> m_currentBg;
      /// @}

      /// The mutex used to to restrict access to m_tmc when it is being written to
      std::shared_mutex m_sharedMutex;


      /// 
      /// @name The legacy configuration objects which are populated either from a R1, R2 TriggerMenuContainer or the R3 JSONs
      /// @{
      /// The "translated" LVL1 configuration object
      SG::SlotSpecificObj<CTPConfig> m_ctpConfig;
      /// The "translated" HLT configuration object
      SG::SlotSpecificObj<HLTChainList> m_chainList;
      /// The "translated" HLT configuration object
      SG::SlotSpecificObj<HLTSequenceList> m_sequenceList;
      /// The "translated" bunch group set object
      SG::SlotSpecificObj<BunchGroupSet> m_bgSet;
      /// @}

      /// Connection to the metadata store
      ServiceHandle< StoreGateSvc > m_metaStore{this, "MetaDataStore", "InputMetaDataStore"};

      /// Is decoded R2 format data available?
      bool m_triggerMenuContainerAvailable;
      /// Is decoded R3 format data available?
      bool m_menuJSONContainerAvailable;

   }; // class xAODConfigSvc

} // namespace TrigConf



#endif // TRIGCONFXAOD_XAODCONFIGSVC_H
