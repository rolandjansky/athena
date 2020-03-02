// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODConfigSvc.h 631651 2014-11-27 18:33:16Z lheinric $
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

// xAOD include(s):
#include "xAODTrigger/TriggerMenu.h"
#include "xAODTrigger/TriggerMenuContainer.h"
#include "xAODTrigger/TriggerMenuAuxContainer.h"
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
      virtual const HLTChainList* chainList() const override;
      /// Get the HLT chains
      virtual const HLTChainList& chains() const override;

      /// Get the HLT sequences
      virtual const HLTSequenceList* sequenceList() const override;
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

      /// @name Dummy implementation of the IHLTConfigSvc interface
      /// @{

      /// Loads prescale sets in online running
      virtual StatusCode updatePrescaleSets( uint /*requestcount*/ ) override {
         return StatusCode::FAILURE;
      }

      /// Updates the prescales on the chain in online running
      virtual StatusCode assignPrescalesToChains( uint /*lumiblock*/ ) override {
         return StatusCode::FAILURE;
      }

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


      SG::ReadHandleKey<xAOD::TrigConfKeys> m_eventKey{this, "EventObjectName", "TrigConfKeys", 
        "Key for the event-level configuration identifier object"};
      Gaudi::Property<std::string> m_metaName{this, "MetaObjectName", "TriggerMenu", 
        "Key for the trigger configuration metadata object"};

      Gaudi::Property<bool> m_stopOnFailure{this, "StopOnFailure", true, "Flag for stopping the job in case of a failure"};
      /// Internal state of the service
      bool m_isInFailure;

      /// The configuration objects copied from all input files
      std::unique_ptr<xAOD::TriggerMenuAuxContainer> m_tmcAux;
      /// The configuration objects copied from all input files
      std::unique_ptr<xAOD::TriggerMenuContainer> m_tmc;

      /// The mutex used to to restrict access to m_tmc when it is being written to
      std::shared_mutex m_sharedMutex;

      // The menu currently being used by each slot (wrapped 'const xAOD::TriggerMenu' ptr)
      SG::SlotSpecificObj<MenuPtrWrapper> m_menu;

      /// The "translated" LVL1 configuration object
      SG::SlotSpecificObj<CTPConfig> m_ctpConfig;
      /// The "translated" HLT configuration object
      SG::SlotSpecificObj<HLTChainList> m_chainList;
      /// The "translated" HLT configuration object
      SG::SlotSpecificObj<HLTSequenceList> m_sequenceList;
      /// The "translated" bunch group set object
      SG::SlotSpecificObj<BunchGroupSet> m_bgSet;

      /// Connection to the metadata store
      ServiceHandle< StoreGateSvc > m_metaStore{this, "MetaDataStore", "InputMetaDataStore"};

   }; // class xAODConfigSvc

} // namespace TrigConf

#endif // TRIGCONFXAOD_XAODCONFIGSVC_H
