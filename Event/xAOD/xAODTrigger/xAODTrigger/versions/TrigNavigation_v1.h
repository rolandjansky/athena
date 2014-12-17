// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigNavigation_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGNAVIGATION_V1_H
#define XAODTRIGGER_VERSIONS_TRIGNAVIGATION_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Interface to the raw trigger navigation information of the event
   ///
   /// This interface is used to access the raw trigger navigation information
   /// in the event. It should only be used directly by experts, regular users
   /// should access the trigger navigation information rather through the
   /// TrigNavStructure class.
   ///
   /// @author Tomasz Bold
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class TrigNavigation_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      TrigNavigation_v1();

      /// expose the navigation information (in serialized form)
      const std::vector< unsigned int >& serialized() const;

      /// sets the serialized form of the navigation from the argument
      void setSerialized( const std::vector< unsigned int >& value );

   }; // class TrigNavigation_v1

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigNavigation_v1, SG::AuxElement );

#endif // XAODTRIGGER_VERSIONS_TRIGNAVIGATION_V1_H
