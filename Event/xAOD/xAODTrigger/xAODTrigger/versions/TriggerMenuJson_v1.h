// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_MENUJSON_v1_H
#define XAODTRIGGER_VERSIONS_MENUJSON_v1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Raw JSON data for a given type of menu file.
   ///
   /// Associated to the menu file's configuration key.
   ///
   class TriggerMenuJson_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      TriggerMenuJson_v1();

      /// @name Configuration identifier variables
      /// @{

      /// Get the key of this configuration
      uint32_t key() const;
      /// Set the key of this configuration
      void setKey( uint32_t value );

      /// Get the name of this configuration
      const std::string& name() const;
      /// Set the name key of this configuration
      void setName( const std::string& name );

      /// @}

      /// @name Payload configuration access
      /// @{

      /// Get the JSON payload
      const std::string& payload() const;
      /// Set the JSON payload
      void setPayload( const std::string& payload );

      /// @}

   }; // class TriggerMenuJson_v1

} // namespace xAOD

#endif // XAODTRIGGER_VERSIONS_MENUJSON_v1_H
