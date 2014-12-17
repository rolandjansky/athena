// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTauRoI_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_EMTAUROI_V1_H
#define XAODTRIGGER_VERSIONS_EMTAUROI_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// xAOD include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class describing a LVL1 em/tau region of interest
   ///
   /// This class describes the properties of an em/tau region of interest (RoI)
   /// in the xAOD format.
   ///
   /// @author Lukas Heinrich <Lukas.Heinrich@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   /// @author Alan Watson <Alan.Watson@cern.ch>
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class EmTauRoI_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      EmTauRoI_v1();

      /// Initialise the object with its most important properties
      void initialize( uint32_t roiword, float eta, float phi );

      /// RoI word types
      enum RoIType {
         CPRoIWord  = 0, ///< This is a Run 1 EM/Tau RoI word
         EMRoIWord  = 1, ///< This is a Run 2 EM RoI word
         TauRoIWord = 2  ///< This is a Run 2 Tau RoI word
      };
      /// Get the type of the RoI word
      RoIType roiType() const;

      /// The pseudorapidity (\f$\eta\f$) of the em/tau candidate
      float eta() const;
      /// Set the pseudorapidity of the em/tau candidate
      void setEta( float v );
 
      /// The azimuthal angle (\f$\phi\f$) of the em/tau candidate
      float phi() const;
      /// Set the azimuthal angle of the em/tau candidate
      void setPhi( float v );

      /// The "raw" RoI word describing the em/tau candidate
      uint32_t roiWord() const;
      /// Set the "raw" RoI word, describing the em/tau candidate
      void setRoIWord( uint32_t value );

      /// The pattern describing which thresholds were passed by the jet
      uint32_t thrPattern() const;

      /// @name Energies deposited in various areas around the RoI
      /// @{

      /// The ET of the RoI Core cluster (2x2 towers, EM+Had)
      float core() const;
      /// Set the ET of the RoI Core cluster (2x2 towers, EM+Had)
      void setCore( float value );

      /// The deposited E<sub>T</sub> from the "EM cluster"
      float emClus() const;
      /// Set the deposited E<sub>T</sub> from the "EM cluster"
      void setEmClus( float value );

      /// The deposited E<sub>T</sub> from the "tau cluster"
      float tauClus() const;
      /// Set the deposited E<sub>T</sub> from the "tau cluster"
      void setTauClus( float value );

      /// The EM calorimeter isolation (outer ring of EM towers)
      float emIsol() const;
      /// Set the EM calorimeter isolation (outer ring of EM towers)
      void setEmIsol( float value );

      /// The hadron calorimeter isolation (outer ring of had towers)
      float hadIsol() const;
      /// Set the hadron calorimeter isolation (outer ring of had towers)
      void setHadIsol( float value );

      /// The E<sub>T</sub> deposited in the inner hadronic isolation region
      /// (2x2 core)
      float hadCore() const;
      /// Set the E<sub>T</sub> deposited in the inner hadronic isolation region
      void setHadCore( float value );

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

   }; // class EmTauRoI_v1

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::EmTauRoI_v1, SG::AuxElement );

#endif // XAODTRIGGER_VERSIONS_EMTAUROI_V1_H
