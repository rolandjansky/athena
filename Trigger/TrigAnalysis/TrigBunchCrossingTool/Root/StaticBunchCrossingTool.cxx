/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StaticBunchCrossingTool.cxx 749252 2016-05-24 09:30:51Z krasznaa $

// Local include(s):
#include "TrigBunchCrossingTool/StaticBunchCrossingTool.h"
#include "StaticConfigs.h"

namespace Trig {

   /**
    * The constructor reads all the configurations out of "StaticConfigs.h" and
    * sets up the tool using those.
    */
   StaticBunchCrossingTool::StaticBunchCrossingTool( const std::string& name )
      : BunchCrossingToolBase( name ),
        m_knownBGKeys() {

      // Declare the property of the tool:
      declareProperty( "BGKey", m_bgkey = 0 );

      //
      // Learn the static bunch group configurations:
      //
      for( int i = 0; i < BGK_CONF_N; ++i ) {
         for( int j = 0; j < BGK_CONF_NUM[ i ]; ++j ) {
            m_knownBGKeys[ BGK_CONF_KEY[ i ] ].push_back( BGK_CONF[ i ][ j ] );
         }
      }
   }

   StatusCode StaticBunchCrossingTool::initialize() {

      // Let the user know what's happening:
      ATH_MSG_INFO( "Initialising tool" );
      ATH_MSG_INFO( "  Bunch group config : " << m_bgkey );

      // Load the configuration specified by the tool's property:
      ATH_CHECK( loadConfig( m_bgkey ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This function tries to load one of the pre-defined configurations. The
    * code knows about all the bunch-group settings up to BGKey=105 at the
    * moment.
    *
    * @param bgkey Bunch group key of the requested configuration
    * @returns <code>StatusCode::SUCCESS</code> if loading the configuration was
    *          successful, <code>StatusCode::FAILURE</code> otherwise
    */
   StatusCode StaticBunchCrossingTool::loadConfig( int bgkey ) {

      //
      // Check if we have this configuration:
      //
      std::map< int, std::vector< int > >::const_iterator bgset;
      if( ( bgset = m_knownBGKeys.find( bgkey ) ) == m_knownBGKeys.end() ) {
         ATH_MSG_ERROR( "Couldn't find configuration for bunch group key: "
                        << bgkey );
         return StatusCode::FAILURE;
      }

      //
      // Now let the base class interpret the configuration:
      //
      ATH_CHECK( loadSingleBunches( bgset->second ) );
      ATH_CHECK( loadBunchTrains( bgset->second ) );
      ATH_CHECK( loadUnpairedBunches( std::vector< int >(),
                                      std::vector< int >() ) );

      // Print the configuration to give some feedback to the user:
      printConfig();

      return StatusCode::SUCCESS;
   }

   /**
    * To make it possible to quickly try out new configurations, this public
    * function configures the underlying tool from a list of bunches (and
    * optionally bunch intensities) given by the user.
    *
    * Only used for testing at the moment.
    *
    * @param bunches The filled bunches which should be interpreted
    * @param intensities The intensities of these filled bunches (optional)
    * @returns <code>StatusCode::SUCCESS</code> if loading the configuration was
    *          successful, <code>StatusCode::FAILURE</code> otherwise
    */
   StatusCode StaticBunchCrossingTool::
   loadConfig( const std::vector< int >& filledBunches,
               const std::vector< float >& filledIntensities,
               const std::vector< int >& unpairedBunches,
               const std::vector< float >& unpairedIntensities ) {

      //
      // Let the base class interpret the configuration:
      //
      ATH_CHECK( loadSingleBunches( filledBunches, filledIntensities ) );
      ATH_CHECK( loadBunchTrains( filledBunches, filledIntensities ) );
      ATH_CHECK( loadUnpairedBunches( unpairedBunches, unpairedBunches,
                                      unpairedIntensities,
                                      unpairedIntensities ) );

      // Print the configuration to give some feedback to the user:
      printConfig();

      return StatusCode::SUCCESS;
   }

   /**
    * This helper function is here to make it possible to extract the bunch
    * configuration from an MC file in an Athena environment, and then use
    * that as a hardcoded configuration to initialise this tool.
    *
    * In this case the argument is the intensities of the bunches. From which
    * the code uses the same logic as MCBunchCrossingTool to find the filled
    * bunches.
    *
    * @param bunches The intensities of the BCIDs. Starting with BCID 0.
    * @returns <code>StatusCode::SUCCESS</code> if loading the configuration was
    *          successful, <code>StatusCode::FAILURE</code> otherwise
    */
   StatusCode StaticBunchCrossingTool::
   loadConfig( const std::vector< float >& bunches ) {

      // Check that the user specified something useful:
      if( ! bunches.size() ) {
         ATH_MSG_ERROR( "Empty container received" );
         return StatusCode::FAILURE;
      }

      // Translate it into vectors of filled bunches and intensities:
      std::vector< int > filled_bunches;
      std::vector< float > filled_intensities;

      // Minimum bunch intensity to consider a bunch filled:
      static const float MIN_BUNCH_INTENSITY = 0.1;

      // Check if the pattern "fits into" the LHC:
      if( BunchCrossing::MAX_BCID % bunches.size() ) {

         ATH_MSG_INFO( "Bunch pattern doesn't \"fit into\" "
                       << BunchCrossing::MAX_BCID );
         // The loop doesn't go all the way up to MAX_BCID/2 in order not
         // to produce "weird" patterns half way. This should be pretty safe
         // to do, because the MC BCIDs will only be in the range defined by
         // the pattern from the metadata.
         for( int i = 0; i < ( BunchCrossing::MAX_BCID / 2 - 20 ); ++i ) {
            const int pos1 = i % bunches.size();
            const int pos2 = bunches.size() - 1 - ( i % bunches.size() );
            if( bunches[ pos1 ] > MIN_BUNCH_INTENSITY ) {
               filled_bunches.push_back( i );
               filled_intensities.push_back( bunches[ pos1 ] );
            }
            if( bunches[ pos2 ] > MIN_BUNCH_INTENSITY ) {
               filled_bunches.push_back( BunchCrossing::MAX_BCID - 1 - i );
               filled_intensities.push_back( bunches[ pos2 ] );
            }
         }

      } else {

         // If the sample size fits into the number of available bunches,
         // the algorithm is pretty simple:
         ATH_MSG_INFO( "Bunch pattern \"fits into\" "
                       << BunchCrossing::MAX_BCID );
         for( int i = 0; i < BunchCrossing::MAX_BCID; ++i ) {
            const int pos = i % bunches.size();
            if( bunches[ pos ] > MIN_BUNCH_INTENSITY ) {
               filled_bunches.push_back( i );
               filled_intensities.push_back( bunches[ pos ] );
            }
         }

      }

      //
      // Now let the base class interpret the information:
      //
      ATH_CHECK( loadSingleBunches( filled_bunches, filled_intensities ) );
      ATH_CHECK( loadBunchTrains( filled_bunches, filled_intensities ) );
      ATH_CHECK( loadUnpairedBunches( std::vector< int >(),
                                      std::vector< int >() ) );

      // Print the configuration to give some feedback to the user:
      printConfig();

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace Trig
