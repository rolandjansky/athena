// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoI_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_ENERGYSUMROI_V1_H
#define XAODTRIGGER_VERSIONS_ENERGYSUMROI_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// xAOD include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class describing a LVL1 energy-sum "RoI"
   ///
   /// This class describes the properties of the LVL1 energy-sum calculation
   /// in the xAOD format.
   ///
   /// @author Lukas Heinrich <Lukas.Heinrich@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   /// @author Alan Watson <Alan.Watson@cern.ch>
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class EnergySumRoI_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      EnergySumRoI_v1();

      /// Initialise the object with its most important properties
      void initialize( uint32_t roiword0, uint32_t roiword1, uint32_t roiword2,
                       float energyX, float energyY, float energyT );

      /// @name RoI word accessor functions
      /// @{

      /// The first RoI word produced by the L1Calo hardware
      uint32_t roiWord0() const;
      /// Set the first "raw" RoI word describing the energy-sum RoI
      void setRoIWord0( uint32_t value );

      /// The second RoI word produced by the L1Calo hardware
      uint32_t roiWord1() const;
      /// Set the second "raw" RoI word describing the energy-sum RoI
      void setRoIWord1( uint32_t value );

      /// The third RoI word produced by the L1Calo hardware
      uint32_t roiWord2() const;
      /// Set the third "raw" RoI word describing the energy-sum RoI
      void setRoIWord2( uint32_t value );

      /// @}

      /// @name Helper functions accessing the threshold patterns
      /// @{

      /// The bit-pattern describing the passed sum-E<sub>T</sub> thresholds
      uint32_t thrPatSumET() const;
      /// The bit-pattern describing the passed missing-E<sub>T</sub> thresholds
      uint32_t thrPatMET() const;
      /// The bit-pattern describing the passed missing-E<sub>T</sub> significance
      /// thresholds
      uint32_t thrPatMETSig() const;

      /// @}

      /// @name Helper functions accessing the overflow bits
      /// @{

      /// The overflow flag for the E<sub>X</sub> calculation
      bool overflowX() const;
      /// The overflow flag for the E<sub>Y</sub> calculation
      bool overflowY() const;
      /// The overflow flag for the E<sub>T</sub> calculation
      bool overflowT() const;

      /// @}

      /// @name Thresholds passed by the RoI
      /// @{

      /// Add a new threshold that was passed
      void addThreshold( const std::string& name );
      /// Remove all the passed thresholds
      void clearThresholds();

      /// The names of the thresholds passed
      const std::vector< std::string >& thrNames() const;

      /// @}

      /// @name Missing E<sub>T</sub> values
      /// @{

      /// The total energy deposited in the X direction
      float energyX() const;
      /// Set the total energy deposited in the X direction
      void setEnergyX( float value );
      /// The missing energy in the X direction
      float exMiss() const { return -energyX(); }

      /// The total energy deposited in the Y direction
      float energyY() const;
      /// Set the total energy deposited in the Y direction
      void setEnergyY( float value );
      /// The missing energy in the Y direction
      float eyMiss() const { return -energyY(); }

      /// The deposited total transverse energy
      float energyT() const;
      /// Set the deposited total transverse energy
      void setEnergyT( float value );

      /// @}

   }; // class EnergySumRoI_v1

} // namespace xAOD

// Declare the inheritace of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::EnergySumRoI_v1, SG::AuxElement );

#endif // XAODTRIGGER_VERSIONS_ENERGYSUMROI_V1_H
