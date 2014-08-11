// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConf_v1.h 583374 2014-02-14 15:52:54Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_BUNCHCONF_V1_H
#define XAODTRIGGER_VERSIONS_BUNCHCONF_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class holding one particular bunch configuration
   ///
   /// This is a very simple class for storing the output taken from
   /// Trig::IBunchCrossingConfProvider, and re-using it in subsequent
   /// jobs.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 583374 $
   /// $Date: 2014-02-14 16:52:54 +0100 (Fri, 14 Feb 2014) $
   ///
   class BunchConf_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      BunchConf_v1();

      /// @name Configuration identifier accessors
      /// @{

      /// Get the configuration ID that this object describes
      uint32_t configID() const;
      /// Set the configuration ID that this object describes
      void setConfigID( uint32_t value );

      /// @}

      /// @name Basic beam configuration values
      /// @{

      /// Get the BCIDs of the colliding bunches
      const std::vector< int >& collidingBCIDs() const;
      /// Set the BCIDs of the colliding bunches
      void setCollidingBCIDs( const std::vector< int >& value );

      /// Get the intensities of the beam1 bunches
      const std::vector< float >& intensitiesBeam1() const;
      /// Set the intensities of the beam1 bunches
      void setIntensitiesBeam1( const std::vector< float >& value );
      /// Check if the beam1 intensities are available
      bool intensitiesBeam1Available() const;

      /// Get the intensities of the beam2 bunches
      const std::vector< float >& intensitiesBeam2() const;
      /// Set the intensities of the beam2 bunches
      void setIntensitiesBeam2( const std::vector< float >& value );
      /// Check if the beam2 intensities are available
      bool intensitiesBeam2Available() const;

      /// @}

      /// @name Information about the unpaired bunches
      /// @{

      /// Get the BCIDs of the unpaired bunches in beam1
      const std::vector< int >& unpairedBCIDsBeam1() const;
      /// Set the BCIDs of the unpaired bunches in beam1
      void setUnpairedBCIDsBeam1( const std::vector< int >& value );
      /// Check if the beam1 unpaired BCIDs are available
      bool unpairedBCIDsBeam1Available() const;

      /// Get the BCIDs of the unpaired bunches in beam2
      const std::vector< int >& unpairedBCIDsBeam2() const;
      /// Set the BCIDs of the unpaired bunches in beam2
      void setUnpairedBCIDsBeam2( const std::vector< int >& value );
      /// Check if the beam2 unpaired BCIDs are available
      bool unpairedBCIDsBeam2Available() const;

      /// Get the intensities of the unpaired bunches in beam1
      const std::vector< float >& unpairedIntensisitesBeam1() const;
      /// Set the intensities of the unpaired bunches in beam1
      void setUnpairedIntensitiesBeam1( const std::vector< float >& value );
      /// Check if the unpaired beam1 intensities are available
      bool unpairedIntensitiesBeam1Available() const;

      /// Get the intensities of the unpaired bunches in beam2
      const std::vector< float >& unpairedIntensisitesBeam2() const;
      /// Set the intensities of the unpaired bunches in beam2
      void setUnpairedIntensitiesBeam2( const std::vector< float >& value );
      /// Check if the unpaired beam2 intensities are available
      bool unpairedIntensitiesBeam2Available() const;

      /// @}

   }; // class BunchConf_v1

} // namespace xAOD

#endif // XAODTRIGGER_VERSIONS_BUNCHCONF_V1_H
