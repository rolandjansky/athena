// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetEtRoI_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_JETETROI_V1_H
#define XAODTRIGGER_VERSIONS_JETETROI_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// xAOD include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class describing a LVL1 jet-ET "RoI"
   ///
   /// This class describes the properties of the LVL1 jet-ET calculation
   /// in the xAOD format.
   ///
   /// @author Lukas Heinrich <Lukas.Heinrich@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   /// @author Alan Watson <Alan.Watson@cern.ch>
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class JetEtRoI_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      JetEtRoI_v1();

      /// The RoI word describing the JetET thresholds passed
      uint32_t roiWord() const;
      /// Set the RoI word describing the JetET thresholds passed
      void setRoIWord( uint32_t value );

      /// Helper function accessing the threshold pattern in the RoI word
      uint32_t thrPattern() const;

      /// @name Thresholds passed by the RoI
      /// @{

      /// Add a new threshold that was passed
      void addThreshold( const std::string& name );
      /// Remove all the passed thresholds
      void clearThresholds();

      /// The names of the thresholds passed
      const std::vector< std::string >& thrNames() const;

      /// @}

   }; // class JetEtRoI_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::JetEtRoI_v1, SG::AuxElement );

#endif // XAODTRIGGER_VERSIONS_JETETROI_V1_H
