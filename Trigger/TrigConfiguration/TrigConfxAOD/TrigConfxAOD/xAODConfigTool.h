// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFXAOD_XAODCONFIGTOOL_H
#define TRIGCONFXAOD_XAODCONFIGTOOL_H

// Infrastructure include(s):
#include "AsgTools/AsgMetadataTool.h"

// Trigger configuration include(s):
// Run 2 / legacy structures
#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequenceList.h"
// Run 3 structures
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/HLTPrescalesSet.h"
#include "TrigConfData/L1PrescalesSet.h"
#include "TrigConfData/L1BunchGroupSet.h"
#include "TrigConfData/DataStructure.h"

// xAOD include(s):
#include "xAODTrigger/TriggerMenu.h"
#include "xAODTrigger/TriggerMenuContainer.h"

#include "xAODTrigger/TriggerMenuJson.h"
#include "xAODTrigger/TriggerMenuJsonContainer.h"

#include "xAODTrigger/TrigConfKeys.h"

#include "AsgTools/CurrentContext.h"

namespace TrigConf {

   /// Trigger configuration metadata tool for xAOD analysis
   ///
   /// This tool is meant to be used when analysing xAOD files both in Athena
   /// and in ROOT, to get access to the trigger configuration information of
   /// the processed events.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631651 $
   /// $Date: 2014-11-27 19:33:16 +0100 (Thu, 27 Nov 2014) $
   ///
   class xAODConfigTool : public asg::AsgMetadataTool,
                          public virtual ITrigConfigTool {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS( xAODConfigTool, ITrigConfigTool )

   public:
      /// Constructor for standalone usage
      xAODConfigTool( const std::string& name = "TrigConf::xAODConfigTool" );

      /// Out-of-line dtor so that we don't need to define Impl in the header.
      ~xAODConfigTool();

      /// @name Function(s) implementing the asg::IAsgTool interface
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize() override;

      /// @}

      /// @name Implementation of the IILVL1ConfigSvc interface
      /// @{

      /// Get the LVL1 trigger menu
      virtual const CTPConfig* ctpConfig() const override ;

      /// Get the LVL1 threshold configuruation (not available from xAOD)
      virtual const ThresholdConfig* thresholdConfig() const override {
         return 0;
      }

      /// Get the LVL1 bunch group set
      virtual const BunchGroupSet* bunchGroupSet() const override ;

      /// Get the LVL1 prescale key
      virtual uint32_t lvl1PrescaleKey() const override ;

      /// @}

      /// @name Implementation of the IIHLTConfigSvc interface
      /// @{

      /// Get the HLT chains
      virtual const HLTChainList& chains() const override ;

      /// Get the HLT sequences
      virtual const HLTSequenceList& sequences() const override ;

      /// Get the Super Master Key
      virtual uint32_t masterKey() const override ;

      /// Get the HLT prescale key
      virtual uint32_t hltPrescaleKey() const override ;

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

   protected:
      /// @name Callback function(s) from AsgMetadataTool
      /// @{

      /// Function called when a new input file is opened
      virtual StatusCode beginInputFile() override;

      /// Function called when a new event is loaded
      virtual StatusCode beginEvent() override;

      /// Internal call to check / load from a file with Run2 metadata
      StatusCode beginEvent_Run2(const xAOD::TrigConfKeys* keys);

      /// Internal call to check / load from a file with Run3 metadata
      StatusCode beginEvent_Run3(const xAOD::TrigConfKeys* keys);

      /// @}

   private:

      /// @name Run 3 helper functions
      /// @{

      /// Locates a Run3 TriggerMenuJson object inside a container by key. Loads it into the m_currentXXXJson ptr
      StatusCode loadJsonByKey(const std::string& humanName, 
         const xAOD::TriggerMenuJsonContainer* metaContainer, 
         const uint32_t keyToCheck, 
         const xAOD::TriggerMenuJson*& ptrToSet);

      /// Load all m_currentXXXJson serialised JSON data into ptrees inside m_impl
      StatusCode loadPtrees();

      /// Load single m_currentXXXJson serialised JSON data into ptree
      StatusCode loadPtree(const std::string& humanName,
         const xAOD::TriggerMenuJson* menu,
         DataStructure& dataStructure);

      /// @}

      /// Key for the event-level configuration identifier object
      std::string m_eventName;
      /// Key for the trigger configuration metadata object (Run 2)
      std::string m_metaName_run2;
      /// Key for the trigger configuration metadata objects (Run 3)
      std::string m_metaNameJSON_hlt;
      std::string m_metaNameJSON_l1;
      std::string m_metaNameJSON_hltps;
      std::string m_metaNameJSON_l1ps;
      std::string m_metaNameJSON_bg;

      /// The configuration object of the current input file (for Run2 AOD)
      const xAOD::TriggerMenuContainer* m_tmc;
      /// The configuration object of the current input file (for Run3 AOD)
      const xAOD::TriggerMenuJsonContainer* m_hltJson;
      const xAOD::TriggerMenuJsonContainer* m_l1Json;
      const xAOD::TriggerMenuJsonContainer* m_hltpsJson;
      const xAOD::TriggerMenuJsonContainer* m_l1psJson;
      const xAOD::TriggerMenuJsonContainer* m_bgJson;

      /// The active configuration for the current event (For Run2 AOD)
      const xAOD::TriggerMenu* m_menu;
      /// The active configuration for the current event (For Run3 AOD)
      const xAOD::TriggerMenuJson* m_currentHltJson;
      const xAOD::TriggerMenuJson* m_currentL1Json;
      const xAOD::TriggerMenuJson* m_currentHltpsJson;
      const xAOD::TriggerMenuJson* m_currentL1psJson;
      const xAOD::TriggerMenuJson* m_currentBgJson;

      /// Is decoded R2 format data available?
      bool m_triggerMenuContainerAvailable;
      /// Is decoded R3 format data available?
      bool m_menuJSONContainerAvailable;

      // A few members moved to an impl class to hide them from cling.
      // Otherwise, we get warnings about the use of boost::multi_index
      // in TrigConfL1Data.
      // Cling requires that the Impl class itself be declared public.
   public:
      struct Impl;
   private:
      std::unique_ptr<Impl> m_impl;
   }; // class xAODConfigTool

} // namespace TrigConf

#endif // TRIGCONFXAOD_XAODCONFIGTOOL_H
