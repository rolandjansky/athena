// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODConfigTool.h 631651 2014-11-27 18:33:16Z lheinric $
#ifndef TRIGCONFXAOD_XAODCONFIGTOOL_H
#define TRIGCONFXAOD_XAODCONFIGTOOL_H

// Infrastructure include(s):
#include "AsgTools/AsgMetadataTool.h"

// Trigger configuration include(s):
#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequenceList.h"

// xAOD include(s):
#include "xAODTrigger/TriggerMenu.h"
#include "xAODTrigger/TriggerMenuContainer.h"

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

      /// @name Function(s) implementing the asg::IAsgTool interface
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// @}

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

   protected:
      /// @name Callback function(s) from AsgMetadataTool
      /// @{

      /// Function called when a new input file is opened
      virtual StatusCode beginInputFile();

      /// Function called when a new event is loaded
      virtual StatusCode beginEvent();

      /// @}

   private:
      /// Key for the event-level configuration identifier object
      std::string m_eventName;
      /// Key for the trigger configuration metadata object
      std::string m_metaName;

      /// The configuration object of the current input file
      const xAOD::TriggerMenuContainer* m_tmc;
      /// The active configuration for the current event
      const xAOD::TriggerMenu* m_menu;

      // Hide these from cling --- otherwise, we get warnings about
      // the use of boost::multi_index in TrigConfL1Data.
#ifndef __CLING__
      /// The "translated" LVL1 configuration object
      CTPConfig m_ctpConfig;
      /// The "translated" HLT configuration object
      HLTChainList m_chainList;
      /// The "translated" HLT configuration object
      HLTSequenceList m_sequenceList;
      /// The "translated" bunch group set object
      BunchGroupSet m_bgSet;
#endif

   }; // class xAODConfigTool

} // namespace TrigConf

#endif // TRIGCONFXAOD_XAODCONFIGTOOL_H
