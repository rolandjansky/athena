// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODConfigSvc.h 631651 2014-11-27 18:33:16Z lheinric $
#ifndef TRIGCONFXAOD_XAODCONFIGSVC_H
#define TRIGCONFXAOD_XAODCONFIGSVC_H

// Gaudi/Athena include(s):
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

namespace TrigConf {

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
      virtual StatusCode initialize();
      /// Function finalising the service
      virtual StatusCode finalize();

      /// @name Implementation of the IILVL1ConfigSvc interface
      /// @{

      /// Get the LVL1 trigger menu
      virtual const CTPConfig* ctpConfig() const;

      /// Get the LVL1 threshold configuruation (not available from xAOD)
      virtual const ThresholdConfig* thresholdConfig() const {
         return 0;
      }

      /// Get the LVL1 bunch group set
      virtual const BunchGroupSet* bunchGroupSet() const;

      /// Get the LVL1 prescale key
      virtual uint32_t lvl1PrescaleKey() const;

      /// @}

      /// @name Implementation of the IIHLTConfigSvc interface
      /// @{

      /// Get the HLT chains
      virtual const HLTChainList* chainList() const;
      /// Get the HLT chains
      virtual const HLTChainList& chains() const;

      /// Get the HLT sequences
      virtual const HLTSequenceList* sequenceList() const;
      /// Get the HLT sequences
      virtual const HLTSequenceList& sequences() const;

      /// Get the Super Master Key
      virtual uint32_t masterKey() const;

      /// Get the HLT prescale key
      virtual uint32_t hltPrescaleKey() const;

      /// @}

      /// @name Implementation of the IL1TopoConfigSvc interface
      /// @{

      /// Get the LVL1 topo menu (not available from xAOD)
      virtual const TXC::L1TopoMenu* menu() const {
         return 0;
      }

      /// @}

      /// @name Dummy implementation of the ILVL1ConfigSvc interface
      /// @{

      /// Get the MuCTPI's online configuration
      virtual const Muctpi* muctpiConfig() const {
         return 0;
      }

      /// @}

      /// @name Dummy implementation of the IHLTConfigSvc interface
      /// @{

      /// Loads prescale sets in online running
      virtual StatusCode updatePrescaleSets( uint /*requestcount*/ ) {
         return StatusCode::FAILURE;
      }

      /// Updates the prescales on the chain in online running
      virtual StatusCode assignPrescalesToChains( uint /*lumiblock*/ ) {
         return StatusCode::FAILURE;
      }

      /// @}

      /// Function describing to Gaudi the interface(s) implemented
      virtual StatusCode queryInterface( const InterfaceID& riid,
                                         void** ppvIf );

      /// Function handling the incoming incidents
      virtual void handle( const Incident& inc );

   private:
      /// Function reading in a new metadata object from the input
      StatusCode readMetadata();
      /// Function setting up the service for a new event
      StatusCode prepareEvent();

      /// Key for the event-level configuration identifier object
      std::string m_eventName;
      /// Key for the trigger configuration metadata object
      std::string m_metaName;

      /// Flag for stopping the job in case of a failure
      bool m_stopOnFailure;
      /// Internal state of the service
      bool m_isInFailure;

      /// The configuration object of the current input file
      const xAOD::TriggerMenuContainer* m_tmc;
      /// The active configuration for the current event
      const xAOD::TriggerMenu* m_menu;

      /// The "translated" LVL1 configuration object
      CTPConfig m_ctpConfig;
      /// The "translated" HLT configuration object
      HLTChainList m_chainList;
      /// The "translated" HLT configuration object
      HLTSequenceList m_sequenceList;
      /// The "translated" bunch group set object
      BunchGroupSet m_bgSet;

      /// Connection to the event store
      ServiceHandle< StoreGateSvc > m_eventStore;
      /// Connection to the metadata store
      ServiceHandle< StoreGateSvc > m_metaStore;

   }; // class xAODConfigSvc

} // namespace TrigConf

#endif // TRIGCONFXAOD_XAODCONFIGSVC_H
