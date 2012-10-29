/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StaticBunchCrossingToolSA.cxx 511865 2012-07-31 08:44:12Z krasznaa $

// Local include(s):
#include "TrigBunchCrossingTool/StaticBunchCrossingToolSA.h"
#include "StaticConfigs.h"

namespace Trig {

   /**
    * The constructor reads all the configurations out of "StaticConfigs.h" and
    * sets up the tool using those.
    */
   StaticBunchCrossingToolSA::StaticBunchCrossingToolSA()
      : BunchCrossingToolBase(), m_knownBGKeys() {

      // Set the proper message source:
      log().setSource( "StaticBunchCrossingToolSA" );

      //
      // Learn the static bunch group configurations:
      //
      for( int i = 0; i < BGK_CONF_N; ++i ) {
         for( int j = 0; j < BGK_CONF_NUM[ i ]; ++j ) {
            m_knownBGKeys[ BGK_CONF_KEY[ i ] ].push_back( BGK_CONF[ i ][ j ] );
         }
      }
   }

   /**
    * This function tries to load one of the pre-defined configurations. The
    * code knows about all the bunch-group settings up to BGKey=105 at the
    * moment.
    *
    * @param bgkey Bunch group key of the requested configuration
    * @returns <code>true</code> if loading the configuration was successful,
    *          <code>false</code> otherwise
    */
   bool StaticBunchCrossingToolSA::loadConfig( int bgkey ) {

      //
      // Check if we have this configuration:
      //
      std::map< int, std::vector< int > >::const_iterator bgset;
      if( ( bgset = m_knownBGKeys.find( bgkey ) ) == m_knownBGKeys.end() ) {
         REPORT_ERROR_MSG( "Couldn't find configuration for bunch group key: "
                           << bgkey );
         return false;
      }

      //
      // Now let the base class interpret the configuration:
      //
      if( ! loadSingleBunches( bgset->second ) ) {
         REPORT_ERROR_MSG( "Failed to interpret single bunches with key: " << bgkey );
         return false;
      }
      if( ! loadBunchTrains( bgset->second ) ) {
         REPORT_ERROR_MSG( "Failed to interpret bunch trains with key: " << bgkey );
         return false;
      }
      if( ! loadUnpairedBunches( std::vector< int >(), std::vector< int >() ) ) {
         REPORT_ERROR_MSG( "Failed to interpret single bunches with key: " << bgkey );
         return false;
      }

      // Print the configuration to give some feedback to the user:
      printConfig();

      return true;
   }

   /**
    * To make it possible to quickly try out new configurations, this public function
    * configures the underlying tool from a list of bunches (and optionally bunch
    * intensities) given by the user.
    *
    * Only used for testing at the moment.
    *
    * @param bunches The filled bunches which should be interpreted
    * @param intensities The intensities of these filled bunches (optional)
    * @returns <code>true</code> if loading the configuration was successful,
    *          <code>false</code> otherwise
    */
   bool StaticBunchCrossingToolSA::loadConfig( const std::vector< int >& filledBunches,
                                               const std::vector< float >& filledIntensities,
                                               const std::vector< int >& unpairedBunches,
                                               const std::vector< float >& unpairedIntensities ) {

      //
      // Let the base class interpret the configuration:
      //
      if( ! loadSingleBunches( filledBunches, filledIntensities ) ) {
         REPORT_ERROR_MSG( "Failed to interpret single bunches from: " << filledBunches
                           << ", intensities: " << filledIntensities );
         return false;
      }
      if( ! loadBunchTrains( filledBunches, filledIntensities ) ) {
         REPORT_ERROR_MSG( "Failed to interpret bunch trains from: " << filledBunches
                           << ", intensities: " << filledIntensities );
         return false;
      }
      if( ! loadUnpairedBunches( unpairedBunches, unpairedBunches,
                                 unpairedIntensities, unpairedIntensities ) ) {
         REPORT_ERROR_MSG( "Failed to interpret unpaired bunches from: " << unpairedBunches
                           << ", intensities: " << unpairedIntensities );
         return false;
      }

      // Print the configuration to give some feedback to the user:
      printConfig();

      return true;
   }

   void StaticBunchCrossingToolSA::setMaxBunchSpacing( int spacing ) {

      BunchCrossingToolBase::setMaxBunchSpacing( spacing );
      return;
   }

   int StaticBunchCrossingToolSA::maxBunchSpacing() const {

      return BunchCrossingToolBase::maxBunchSpacing();
   }

   void StaticBunchCrossingToolSA::setFrontLength( int length ) {

      BunchCrossingToolBase::setFrontLength( length );
      return;
   }

   int StaticBunchCrossingToolSA::frontLength() const {

      return BunchCrossingToolBase::frontLength();
   }

   void StaticBunchCrossingToolSA::setTailLength( int length ) {

      BunchCrossingToolBase::setTailLength( length );
      return;
   }

   int StaticBunchCrossingToolSA::tailLength() const {

      return BunchCrossingToolBase::tailLength();
   }

} // namespace Trig
