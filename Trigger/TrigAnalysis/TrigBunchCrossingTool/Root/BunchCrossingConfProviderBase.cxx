/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchCrossingConfProviderBase.cxx 748399 2016-05-19 14:55:47Z krasznaa $

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossingConfProviderBase.h"
#include "TrigBunchCrossingTool/BunchCrossing.h"

namespace Trig {

   //
   // Initialize the static variable(s):
   //
   const std::string
   BunchCrossingConfProviderBase::BUNCH_CONFIG_INCIDENT_NAME = "BunchConfig";

   /**
    * The implementation just loops over all the possible BCIDs and puts the
    * IDs of the ones which are reported to be filled, into the output
    * vector.
    *
    * @see BunchCrossingConfProviderBase::configuredIntensitiesBeam1()
    * @see BunchCrossingConfProviderBase::configuredIntensitiesBeam2()
    */
   std::vector< int >
   BunchCrossingConfProviderBase::configuredBCIDs() const {

      std::vector< int > bunches;
      for( int i = 0; i < BunchCrossing::MAX_BCID; ++i ) {
         if( isFilled( i ) ) {
            bunches.push_back( i );
         }
      }

      return bunches;
   }

   /**
    * The implementation just loops over all the possible BCIDs, and puts the
    * intensities of the filled ones into the output vector. This way the result
    * is in sync with the output of configuredBCIDs().
    *
    * @see BunchCrossingConfProviderBase::configuredBCIDs()
    */
   std::vector< float >
   BunchCrossingConfProviderBase::configuredIntensitiesBeam1() const {

      std::vector< float > intensities;
      for( int i = 0; i < BunchCrossing::MAX_BCID; ++i ) {
         if( isFilled( i ) ) {
            intensities.push_back( bcIntensity( i, Beam1 ) );
         }
      }

      return intensities;
   }

   /**
    * The implementation just loops over all the possible BCIDs, and puts the
    * intensities of the filled ones into the output vector. This way the result
    * is in sync with the output of configuredBCIDs().
    *
    * @see BunchCrossingConfProviderBase::configuredBCIDs()
    */
   std::vector< float >
   BunchCrossingConfProviderBase::configuredIntensitiesBeam2() const {

      std::vector< float > intensities;
      for( int i = 0; i < BunchCrossing::MAX_BCID; ++i ) {
         if( isFilled( i ) ) {
            intensities.push_back( bcIntensity( i, Beam2 ) );
         }
      }

      return intensities;
   }

   /**
    * The implementation just loops over all the possible BCIDs and puts the
    * IDs of the ones which are reported to be unpaired, into the output
    * vector.
    *
    * @see BunchCrossingConfProviderBase::configuredUnpairedIntensitiesBeam1()
    */
   std::vector< int >
   BunchCrossingConfProviderBase::configuredUnpairedBCIDsBeam1() const {

      std::vector< int > bunches;
      for( int i = 0; i < BunchCrossing::MAX_BCID; ++i ) {
         if( isUnpaired( i ) && isBeam1( i ) ) {
            bunches.push_back( i );
         }
      }

      return bunches;
   }

   /**
    * The implementation just loops over all the possible BCIDs and puts the
    * IDs of the ones which are reported to be unpaired, into the output
    * vector.
    *
    * @see BunchCrossingConfProviderBase::configuredUnpairedIntensitiesBeam2()
    */
   std::vector< int >
   BunchCrossingConfProviderBase::configuredUnpairedBCIDsBeam2() const {

      std::vector< int > bunches;
      for( int i = 0; i < BunchCrossing::MAX_BCID; ++i ) {
         if( isUnpaired( i ) && isBeam2( i ) ) {
            bunches.push_back( i );
         }
      }

      return bunches;
   }

   /**
    * The implementation just loops over all the possible BCIDs, and puts the
    * intensities of the unpaired ones into the output vector. This way the result
    * is in sync with the output of configuredUnpairedBCIDsBeam1().
    *
    * @see BunchCrossingConfProviderBase::configuredUnpairedBCIDsBeam1()
    */
   std::vector< float >
   BunchCrossingConfProviderBase::configuredUnpairedIntensitiesBeam1() const {

      std::vector< float > intensities;
      for( int i = 0; i < BunchCrossing::MAX_BCID; ++i ) {
         if( isUnpaired( i ) && isBeam1( i ) ) {
            intensities.push_back( bcIntensity( i, Beam1 ) );
         }
      }

      return intensities;
   }

   /**
    * The implementation just loops over all the possible BCIDs, and puts the
    * intensities of the unpaired ones into the output vector. This way the result
    * is in sync with the output of configuredUnpairedBCIDsBeam2().
    *
    * @see BunchCrossingConfProviderBase::configuredUnpairedBCIDsBeam2()
    */
   std::vector< float >
   BunchCrossingConfProviderBase::configuredUnpairedIntensitiesBeam2() const {

      std::vector< float > intensities;
      for( int i = 0; i < BunchCrossing::MAX_BCID; ++i ) {
         if( isUnpaired( i ) && isBeam2( i ) ) {
            intensities.push_back( bcIntensity( i, Beam2 ) );
         }
      }

      return intensities;
   }

} // namespace Trig
