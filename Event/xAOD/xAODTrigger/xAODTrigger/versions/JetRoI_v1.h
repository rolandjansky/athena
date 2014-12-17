// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetRoI_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_JETROI_V1_H
#define XAODTRIGGER_VERSIONS_JETROI_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// xAOD include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class describing a LVL1 jet region of interest
   ///
   /// This class describes the properties of a jet region of interest (RoI)
   /// in the xAOD format.
   ///
   /// @author Lukas Heinrich <Lukas.Heinrich@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   /// @author Alan Watson <Alan.Watson@cern.ch>
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class JetRoI_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      JetRoI_v1();

      /// Initialise the object with its most important properties
      void initialize( uint32_t roiword, float eta, float phi );

      /// The pseudorapidity (\f$\eta\f$) of the jet candidate
      float eta() const;
      /// Set the pseudorapidity of the jet candidate
      void setEta( float v );
 
      /// The azimuthal angle (\f$\phi\f$) of the jet candidate
      float phi() const;
      /// Set the azimuthal angle of the jet candidate
      void setPhi( float v );

      /// The "raw" RoI word describing the jet candidate
      uint32_t roiWord() const;
      /// Set the "raw" RoI word, describing the jet candidate
      void setRoIWord( uint32_t value );

      /// The pattern describing which thresholds were passed by the jet
      uint32_t thrPattern() const;

      /// @name Energies deposited in various areas around the RoI
      /// @{

      /// The energy deposited in a 0.4x0.4 area around the RoI
      float et4x4() const;
      /// Set the energy deposited in a 0.4x0.4 area around the RoI
      void setEt4x4( float value );

      /// The energy deposited in a 0.6x0.6 area around the RoI
      float et6x6() const;
      /// Set the energy deposited in a 0.6x0.6 area around the RoI
      void setEt6x6( float value );

      /// The energy deposited in a 0.8x0.8 area around the RoI
      float et8x8() const;
      /// Set the energy deposited in a 0.8x0.8 area around the RoI
      void setEt8x8( float value );

      /// @}

      /// @name Thresholds passed by the RoI
      /// @{

      /// Add a new threshold that was passed by the RoI
      void addThreshold( const std::string& name, float value );
      /// Remove all the passed thresholds from the RoI
      void clearThresholds();

      /// The names of the thresholds passed by jet candidate
      const std::vector< std::string >& thrNames() const;
      /// The values of the thresholds (in MeV) passed by the jet candidate
      const std::vector< float >& thrValues() const;

      /// @}

   }; // class JetRoI_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::JetRoI_v1, SG::AuxElement );

#endif // XAODTRIGGER_VERSIONS_JETROI_V1_H
