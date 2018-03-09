/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MCBunchCrossingTool.cxx 780643 2016-10-27 03:39:39Z ssnyder $

// System include(s):
#include <cstdlib>
#include <functional>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// DB include(s):
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// Local include(s):
#include "MCBunchCrossingTool.h"
#include "TrigBunchCrossingTool/BunchCrossing.h"

namespace Trig {

   /// Folder in the MC MetaData that we need to read
   static const std::string MC_DIGI_PARAM = "/Digitization/Parameters";
   /// The parameter in the MetaData that we're interested in
   static const std::string BEAM_INTENSITY_PATTERN = "BeamIntensityPattern";

   MCBunchCrossingTool::MCBunchCrossingTool( const std::string& /*type*/,
                                             const std::string& name,
                                             const IInterface* /*parent*/ )
      : BunchCrossingToolBase( name ),
        m_isConfigured(false),
        m_id(),
        m_incidentSvc( "IncidentSvc", name ) {

      // Declare the interfaces provided by the tool:
      declareInterface< IBunchCrossingTool >( this );
      declareInterface< IBunchCrossingConfProvider >( this );

      // Declare the properties of the tool:
      declareProperty( "MinBunchIntensity", m_minBunchIntensity = 0.1,
                       "Minimum intensity for a bunch crossing to be considered a "
                       "filled crossing" );
   }

   StatusCode MCBunchCrossingTool::initialize() {

      // Report about the initialization:
      ATH_MSG_INFO( "Initializing MCBunchCrossingTool - package version: "
                    << PACKAGE_VERSION );
      ATH_MSG_INFO( "  Maximal bunch spacing: " << m_maxBunchSpacing << " ns" );
      ATH_MSG_INFO( "  Length of train front: " << m_frontLength << " ns" );
      ATH_MSG_INFO( "  Length of train tail : " << m_tailLength << " ns" );
      ATH_MSG_INFO( "  Minimum bunch-x int. : " << m_minBunchIntensity );

      //
      // Register the callback(s):
      //
      if( detStore()->contains< AthenaAttributeList >( MC_DIGI_PARAM ) ) {
         const DataHandle< AthenaAttributeList > aptr;
         CHECK( detStore()->regFcn( &MCBunchCrossingTool::update, this, aptr,
                                    MC_DIGI_PARAM, true ) );
      }
      m_isConfigured = false;

      // Finally retrieve the incident service:
      CHECK( m_incidentSvc.retrieve() );

      return StatusCode::SUCCESS;
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool MCBunchCrossingTool::isFilled( bcid_type bcid ) const {

      if( ! m_isConfigured ) return true;
      else return BunchCrossingToolBase::isFilled( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool MCBunchCrossingTool::isBeam1( bcid_type bcid ) const {

      if( ! m_isConfigured ) return true;
      else return BunchCrossingToolBase::isBeam1( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool MCBunchCrossingTool::isBeam2( bcid_type bcid ) const {

      if( ! m_isConfigured ) return true;
      else return BunchCrossingToolBase::isBeam2( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   float MCBunchCrossingTool::bcIntensity( bcid_type bcid,
                                           BeamType type ) const {

      if( ! m_isConfigured ) return 1.0;
      else return BunchCrossingToolBase::bcIntensity( bcid, type );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   IBunchCrossingTool::BunchCrossingType
   MCBunchCrossingTool::bcType( bcid_type bcid ) const {

      if( ! m_isConfigured ) return Single;
      else return BunchCrossingToolBase::bcType( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   int MCBunchCrossingTool::gapBeforeBunch( bcid_type bcid,
                                            BunchDistanceType dtype,
                                            BunchFillType ftype ) const {

      if( ! m_isConfigured ) return -1;
      else return BunchCrossingToolBase::gapBeforeBunch( bcid, dtype, ftype );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   int MCBunchCrossingTool::gapAfterBunch( bcid_type bcid,
                                           BunchDistanceType dtype,
                                           BunchFillType ftype ) const {

      if( ! m_isConfigured ) return -1;
      else return BunchCrossingToolBase::gapAfterBunch( bcid, dtype, ftype );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   std::vector< bool > MCBunchCrossingTool::bunchesInFront( bcid_type bcid,
                                                            int bunches ) const {

      if( ! m_isConfigured ) {
         std::vector< bool > result;
         result.push_back( true );
         for( int i = 0; i < bunches - 1; ++i ) {
            result.push_back( false );
         }
         return result;
      } else {
         return BunchCrossingToolBase::bunchesInFront( bcid, bunches );
      }
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   std::vector< bool > MCBunchCrossingTool::bunchesAfter( bcid_type bcid,
                                                          int bunches ) const {

      if( ! m_isConfigured ) {
         std::vector< bool > result;
         result.push_back( true );
         for( int i = 0; i < bunches - 1; ++i ) {
            result.push_back( false );
         }
         return result;
      } else {
         return BunchCrossingToolBase::bunchesAfter( bcid, bunches );
      }
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   std::vector< float > MCBunchCrossingTool::bunchIntInFront( bcid_type bcid,
                                                              int bunches,
                                                              BeamType type ) const {

      if( ! m_isConfigured ) {
         std::vector< float > result;
         result.push_back( 1.0 );
         for( int i = 0; i < bunches - 1; ++i ) {
            result.push_back( 0.0 );
         }
         return result;
      } else {
         return BunchCrossingToolBase::bunchIntInFront( bcid, bunches, type );
      }
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   std::vector< float > MCBunchCrossingTool::bunchIntAfter( bcid_type bcid,
                                                            int bunches,
                                                            BeamType type ) const {

      if( ! m_isConfigured ) {
         std::vector< float > result;
         result.push_back( 1.0 );
         for( int i = 0; i < bunches - 1; ++i ) {
            result.push_back( 0.0 );
         }
         return result;
      } else {
         return BunchCrossingToolBase::bunchIntAfter( bcid, bunches, type );
      }
   }

   /**
    * In case the tool is not configured (we're not processing a pileup MC file) it
    * should not report 0 filled bunches, but 1. The override of this function makes
    * sure that this is what happens...
    */
   unsigned int MCBunchCrossingTool::numberOfFilledBunches() const {

      if( ! m_isConfigured ) return 1;
      else return BunchCrossingToolBase::numberOfFilledBunches();
   }

   /**
    * This tool returns a hash created from the string version of the bunch configuration.
    * Luckily the MC files contain the full configuration in an std::string, so it's
    * quite easy to create a unique ID from them.
    */
   IBunchCrossingConfProvider::configid_type MCBunchCrossingTool::configID() const {

      return m_id;
   }

   /**
    * This function is re-implemented from the base class to take into account whether
    * the tool managed to find the configuration or not. In Athena running the tool will
    * report all bunches as single empty bunches when it's not "configured". So the base
    * class would give a configuration with all the bunches filled.
    *
    * Instead this function returns an empty vector when the configuration is not available,
    * and only lets the base class do it's work when the configuration was found.
    */
   std::vector< int > MCBunchCrossingTool::configuredBCIDs() const {

      if( ! m_isConfigured ) return std::vector< int >();
      else return BunchCrossingConfProviderBase::configuredBCIDs();
   }

   /**
    * This function is re-implemented from the base class to take into account whether
    * the tool managed to find the configuration or not. In Athena running the tool will
    * report all bunches as single empty bunches when it's not "configured". So the base
    * class would give a configuration with all the bunches filled.
    *
    * Instead this function returns an empty vector when the configuration is not available,
    * and only lets the base class do it's work when the configuration was found.
    *
    * When the configuration is available, we still can't use the base class's function,
    * as it would be asking for the Beam1 intensities instead of the collision intensities.
    */
   std::vector< float > MCBunchCrossingTool::configuredIntensitiesBeam1() const {

      if( ! m_isConfigured ) return std::vector< float >();

      std::vector< float > intensities;
      for( int i = 0; i < BunchCrossing::MAX_BCID; ++i ) {
         if( isFilled( i ) ) {
            intensities.push_back( bcIntensity( i, Crossing ) );
         }
      }

      return intensities;
   }

   /**
    * The MC meta-data stores relative bunch luminosities. That value is stores in the
    * Beam1 variable by convention, and this variable is left empty.
    */
   std::vector< float > MCBunchCrossingTool::configuredIntensitiesBeam2() const {

      return std::vector< float >();
   }

   /**
    * This function takes care of reading the MC pile-up configuration from the MetaData
    * of the input file.
    */
   StatusCode MCBunchCrossingTool::update( IOVSVC_CALLBACK_ARGS_P( /*I*/, keys ) ) {

      ATH_MSG_INFO( "Updating the bunch configuration" );

      // Reset the internal settings:
      bool run_update = false;
      m_isConfigured = false;
      m_id = 0;

      // Check what kind of keys we got. In principle the function should only
      // receive the "/Digitization/Parameters" key.
      msg( MSG::DEBUG ) << "Update called with " << keys.size() << " keys:";
      std::list< std::string >::const_iterator itr = keys.begin();
      std::list< std::string >::const_iterator end = keys.end();
      for( ; itr != end; ++itr ) {
         if( *itr == MC_DIGI_PARAM ) run_update = true;
         msg() << *itr;
      }
      msg() << endmsg;
      // If that's not the key that we received after all, let's just return
      // silently...
      if( ! run_update ) return StatusCode::SUCCESS;

      // Retrieve the object describing the configuration:
      const DataHandle< AthenaAttributeList > digiParam;
      CHECK( detStore()->retrieve( digiParam, MC_DIGI_PARAM ) );

      // The data to extract:
      std::vector< float > bunches;
      // And the helper variable to extract it with:
      std::string sbunches;

      // Try to find the bunch pattern:
      AthenaAttributeList::const_iterator attr_itr = digiParam->begin();
      AthenaAttributeList::const_iterator attr_end = digiParam->end();
      for( ; attr_itr != attr_end; ++attr_itr ) {

         if( attr_itr->specification().name() == BEAM_INTENSITY_PATTERN ) {
            sbunches = attr_itr->data< std::string >();
            REPORT_MESSAGE( MSG::VERBOSE ) << "Beam pattern string: " << sbunches;

            // Translate the string into a vector of floats:
            bunches = tokenize( sbunches );
            REPORT_MESSAGE( MSG::DEBUG ) << "Beam pattern: " << bunches;

            break;
         }
      }

      // Check if we found the metadata:
      if( ! bunches.size() ) {
         ATH_MSG_INFO( "Bunch structure information not found in: "
                          << MC_DIGI_PARAM );
         ATH_MSG_INFO( "Tool will report all BCIDs as single filled bunches" );
         m_isConfigured = false;

         // Let everybody know that the configuration of the tool has changed:
         m_incidentSvc->fireIncident( Incident( "BunchConfiguration update",
                                                BUNCH_CONFIG_INCIDENT_NAME ) );
         return StatusCode::SUCCESS;
      }

      // Translate it into vectors of filled bunches and intensities:
      std::vector< int > filled_bunches;
      std::vector< float > filled_intensities;

      // Check if the pattern "fits into" the LHC:
      if( BunchCrossing::MAX_BCID % bunches.size() ) {

         ATH_MSG_INFO( "Bunch pattern doesn't \"fit into\" " << BunchCrossing::MAX_BCID );
         // The loop doesn't go all the way up to MAX_BCID/2 in order not to produce "weird"
         // patterns half way. This should be pretty safe to do, because the MC BCIDs will
         // only be in the range defined by the pattern from the metadata.
         for( int i = 0; i < ( BunchCrossing::MAX_BCID / 2 - 20 ); ++i ) {
            int pos1 = i % bunches.size();
            int pos2 = bunches.size() - 1 - ( i % bunches.size() );
            if( bunches[ pos1 ] > m_minBunchIntensity ) {
               filled_bunches.push_back( i );
               filled_intensities.push_back( bunches[ pos1 ] );
            }
            if( bunches[ pos2 ] > m_minBunchIntensity ) {
               filled_bunches.push_back( BunchCrossing::MAX_BCID - 1 - i );
               filled_intensities.push_back( bunches[ pos2 ] );
            }
         }

      } else {

         // If the sample size fits into the number of available bunches, the algorithm
         // is pretty simple:
         ATH_MSG_INFO( "Bunch pattern \"fits into\" " << BunchCrossing::MAX_BCID );
         for( int i = 0; i < BunchCrossing::MAX_BCID; ++i ) {
            int pos = i % bunches.size();
            if( bunches[ pos ] > m_minBunchIntensity ) {
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
      m_isConfigured = true;
      m_id = static_cast< configid_type >( std::hash< std::string >()( sbunches ) );
      REPORT_MESSAGE( MSG::DEBUG ) << "ID assigned to configuration: "
                                   << m_id;

      // Let everybody know that the configuration of the tool has changed:
      m_incidentSvc->fireIncident( Incident( "BunchConfiguration update",
                                             BUNCH_CONFIG_INCIDENT_NAME ) );

      return StatusCode::SUCCESS;
   }

   /**
    * This helper function is used to convert a string of type
    * "[0.0, 0.0, 1.0, 1.0, 1.0]" into a vector of floats. As it happens, the
    * digitization stores the bunch pattern in such a fancy style...
    *
    * @param pattern The pattern extracted from the MC file metadata
    * @returns The "decoded" bunch pattern
    */
   std::vector< float > MCBunchCrossingTool::tokenize( const std::string& pattern ) {

      // The result vector:
      std::vector< float > result;

      // Find '[':
      std::string::size_type pos = pattern.find( '[', 0 );
      if( pos == pattern.npos ) return result;

      // Loop over the string, finding all the float numbers in it:
      std::string::size_type prev_pos = pos + 1;
      bool last_number = false;
      for( ; ; ) {
         // Find the "end" of the number:
         pos = pattern.find( ',', prev_pos );
         if( pos == pattern.npos ) {
            pos = pattern.find( ']', prev_pos );
            if( pos == pattern.npos ) {
               break;
            }
            last_number = true; // Show that we reached the end of the string
         }

         // Convert the string into a float:
         std::string number = pattern.substr( prev_pos, pos - prev_pos );
         result.push_back( atof( number.c_str() ) );

         prev_pos = pos + 1;
         if( last_number ) break; // Leave the loop if we're finished...
      }

      return result;
   }

} // namesapce Trig
