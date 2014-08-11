// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenu_v1.h 582798 2014-02-12 15:53:47Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGGERMENU_V1_H
#define XAODTRIGGER_VERSIONS_TRIGGERMENU_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class holding one particular trigger configuration
   ///
   /// This is an extremely simple class for holding the same sort of
   /// information about the trigger configuration as we had in the D3PD
   /// metadata.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 582798 $
   /// $Date: 2014-02-12 16:53:47 +0100 (Wed, 12 Feb 2014) $
   ///
   class TriggerMenu_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      TriggerMenu_v1();

      /// @name Configuration identifier variables
      /// @{

      /// Get the Super Master Key of this configuration
      uint32_t smk() const;
      /// Set the Super Master Key of this configuration
      void setSMK( uint32_t value );

      /// Get the LVL1 prescale key of this configuration
      uint32_t l1psk() const;
      /// Set the LVL1 prescale key of this configuration
      void setL1psk( uint32_t value );

      /// Get the HLT prescale key of this configuration
      uint32_t hltpsk() const;
      /// Set the HLT prescale key of this configuration
      void setHLTpsk( uint32_t value );

      /// @}

      /// @name LVL1 configuration access
      /// @{

      /// Get the CTP IDs of all the LVL1 items
      const std::vector< uint16_t >& itemCtpIds() const;
      /// Set the CTP IDs of all the LVL1 items
      void setItemCtpIds( const std::vector< uint16_t >& value );
      /// Check whether the CTP IDs are available
      bool itemCtpIdsAvailable() const;

      /// Get the names of all the LVL1 items
      const std::vector< std::string >& itemNames() const;
      /// Set the names of all the LVL1 items
      void setItemNames( const std::vector< std::string >& value );
      /// Check whether the LVL1 item names are available
      bool itemNamesAvailable() const;

      /// Get the prescales of all the LVL1 items
      const std::vector< float >& itemPrescales() const;
      /// Set the prescales of all the LVL1 items
      void setItemPrescales( const std::vector< float >& value );
      /// Check whether the LVL1 prescales are available
      bool itemPrescalesAvailable() const;

      /// @}

      /// @name HLT configuration access
      /// @{

      /// Get the IDs of all the HLT chains
      const std::vector< uint16_t >& chainIds() const;
      /// Set the IDs of all the HLT chains
      void setChainIds( const std::vector< uint16_t >& value );
      /// Check if the HLT chain IDs are available
      bool chainIdsAvailable() const;

      /// Get the names of all the HLT chains
      const std::vector< std::string >& chainNames() const;
      /// Set the names of all the HLT chains
      void setChainNames( const std::vector< std::string >& value );
      /// Check if the HLT chain names are available
      bool chainNamesAvailable() const;

      /// Get the names of the parents of the HLT chains
      const std::vector< std::string >& chainParentNames() const;
      /// Set the names of the parents of the HLT chains
      void setChainParentNames( const std::vector< std::string >& value );
      /// Check if the HLT chain parent names are available
      bool chainParentNamesAvailable() const;

      /// Get the prescales of all the chains
      const std::vector< float >& chainPrescales() const;
      /// Set the prescales of all the chains
      void setChainPrescales( const std::vector< float >& value );
      /// Check if the HLT chain prescales are available
      bool chainPrescalesAvailable() const;

      /// Get the re-run prescales of all the chains
      const std::vector< float >& chainRerunPrescales() const;
      /// Set the re-run prescales of all the chains
      void setChainRerunPrescales( const std::vector< float >& value );
      /// Check if the HLT chain re-run prescales are available
      bool chainRerunPrescalesAvailable() const;

      /// Get the pass-through prescales of all the chains
      const std::vector< float >& chainPassthroughPrescales() const;
      /// Set the pass-through prescales of all the chains
      void setChainPassthroughPrescales( const std::vector< float >& value );
      /// Check if the HLT chain pass-through prescales are available
      bool chainPassthroughPrescalesAvailable() const;

      /// @}

      /// @name Bunch group access
      /// @{

      /// Bunch identifiers describing one bunch group
      typedef std::vector< std::vector< uint16_t > > BunchGroup_t;

      /// Get the bunches belonging to each bunch group
      const BunchGroup_t& bunchGroupBunches() const;
      /// Set the bunches belonging to each bunch group
      void setBunchGroupBunches( const BunchGroup_t& value );
      /// Check if the bunches belonging to the bunch groups are available
      bool bunchGroupBunchesAvailable() const;

      /// @}

   }; // class TriggerMenu_v1

} // namespace xAOD

#endif // XAODTRIGGER_VERSIONS_TRIGGERMENU_V1_H
