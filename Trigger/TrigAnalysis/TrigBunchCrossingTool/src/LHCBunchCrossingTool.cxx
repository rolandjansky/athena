/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LHCBunchCrossingTool.cxx 780643 2016-10-27 03:39:39Z ssnyder $

// System include(s):
#include <inttypes.h>
#include <algorithm>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaKernel/IOVTime.h"

// DB include(s):
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListException.h"

// Local include(s):
#include "LHCBunchCrossingTool.h"

namespace Trig {

   //
   // Some constants specifying the DB folders this tool is interested in:
   //
   static const std::string LHC_FILLPARAMS_FOLDER = "/TDAQ/OLC/LHC/FILLPARAMS";
   static const std::string LHC_BUNCHDATA_FOLDER = "/TDAQ/OLC/LHC/BUNCHDATA";

   LHCBunchCrossingTool::LHCBunchCrossingTool( const std::string& /*type*/,
                                               const std::string& name,
                                               const IInterface* /*parent*/ )
      : BunchCrossingToolBase( name ),
        m_id(),
        m_intValid(false),
        m_incidentSvc( "IncidentSvc", name ),
        m_iovSvc( "IOVDbSvc", name ),
        m_beam1Bunches(0),
        m_beam2Bunches(0),
        m_luminousBunches(0)
   {

      // Declare the interfaces provided by the tool:
      declareInterface< IBunchCrossingTool >( this );
      declareInterface< IBunchCrossingConfProvider >( this );

      // Declare the properties of the tool:
      declareProperty( "IntensityChannel", m_intChannel = 1,
                       "Channel to get the bunch intensities from "
                       "(0 for BPTX, 1 for BCT)" );
   }

   StatusCode LHCBunchCrossingTool::initialize() {

      // Report about the initialization:
      ATH_MSG_INFO( "Initializing LHCBunchCrossingTool - package version: "
                    << PACKAGE_VERSION );
      ATH_MSG_INFO( "  Maximal bunch spacing: " << m_maxBunchSpacing << " ns" );
      ATH_MSG_INFO( "  Length of train front: " << m_frontLength << " ns" );
      ATH_MSG_INFO( "  Length of train tail : " << m_tailLength << " ns" );
      std::string intChannel = "UNKNOWN (falling back to BPTX)";
      if( m_intChannel == 0 ) {
         intChannel = "BPTX";
      } else if( m_intChannel == 1 ) {
         intChannel = "BCT";
      } else {
         // Reset it to something meaningful:
         m_intChannel = 0;
      }
      ATH_MSG_INFO( "  Beam intensity source: " << intChannel );

      // Retrieve the needed service(s):
      CHECK( m_incidentSvc.retrieve() );
      CHECK( m_iovSvc.retrieve() );

      //
      // Register the callback(s):
      //
      const DataHandle< AthenaAttributeList > aptr;
      CHECK( detStore()->regFcn( &LHCBunchCrossingTool::update, this, aptr,
                                 LHC_FILLPARAMS_FOLDER, true ) );
      const DataHandle< CondAttrListCollection > cptr;
      CHECK( detStore()->regFcn( &LHCBunchCrossingTool::update, this, cptr,
                                 LHC_BUNCHDATA_FOLDER, true ) );

      return StatusCode::SUCCESS;
   }

   /**
    * The configuration ID is derived from the IOV of the currently loaded
    * conditions. The LHC fill number could also be a good number in the future,
    * but I'm not sure how to extract that at the moment.
    */
   IBunchCrossingConfProvider::configid_type
   LHCBunchCrossingTool::configID() const {

      return m_id;
   }

   /**
    * Override the function in case no valid beam intensity is available for the
    * moment.
    */
   std::vector< float >
   LHCBunchCrossingTool::configuredIntensitiesBeam1() const {

      if( m_intValid ) {
         return BunchCrossingConfProviderBase::configuredIntensitiesBeam1();
      } else {
         return std::vector< float >();
      }
   }

   /**
    * Override the function in case no valid beam intensity is available for the
    * moment.
    */
   std::vector< float >
   LHCBunchCrossingTool::configuredIntensitiesBeam2() const {

      if( m_intValid ) {
         return BunchCrossingConfProviderBase::configuredIntensitiesBeam2();
      } else {
         return std::vector< float >();
      }
   }

   /**
    * Override the function in case no valid beam intensity is available for the
    * moment.
    */
   std::vector< float >
   LHCBunchCrossingTool::configuredUnpairedIntensitiesBeam1() const {

      if( m_intValid ) {
         return BunchCrossingConfProviderBase::
            configuredUnpairedIntensitiesBeam1();
      } else {
         return std::vector< float >();
      }
   }

   /**
    * Override the function in case no valid beam intensity is available for the
    * moment.
    */
   std::vector< float >
   LHCBunchCrossingTool::configuredUnpairedIntensitiesBeam2() const {

      if( m_intValid ) {
         return BunchCrossingConfProviderBase::
            configuredUnpairedIntensitiesBeam2();
      } else {
         return std::vector< float >();
      }
   }

   StatusCode LHCBunchCrossingTool::update( IOVSVC_CALLBACK_ARGS_P( /*i*/,
                                                                    keys ) ) {

      ATH_MSG_INFO( "Updating the bunch configuration" );

      // Print some info about the received parameter(s):
      bool update_fillparams = false;
      bool update_bunchdata = false;
      msg( MSG::DEBUG ) << "Update called with " << keys.size() << " key(s):";
      std::list< std::string >::const_iterator itr = keys.begin();
      std::list< std::string >::const_iterator end = keys.end();
      for( ; itr != end; ++itr ) {
         if( *itr == LHC_FILLPARAMS_FOLDER )     update_fillparams = true;
         else if( *itr == LHC_BUNCHDATA_FOLDER ) update_bunchdata = true;
         msg() << " " << *itr;
      }
      msg() << endmsg;

      // Warn the user in case the update will probably not succeed:
      if( update_fillparams && ( ! update_bunchdata ) ) {
         ATH_MSG_WARNING( "Will be updating info from the "
                          << LHC_FILLPARAMS_FOLDER << " folder, but not from "
                          << LHC_BUNCHDATA_FOLDER );
         ATH_MSG_WARNING( "The resulting tool configuration will probably be "
                          "incorrect" );
      }

      //
      // If the /TDAQ/OLC/LHC/FILLPARAMS folder is available:
      //
      if( update_fillparams ) {
         const DataHandle< AthenaAttributeList > fillparams;
         CHECK( detStore()->retrieve( fillparams, LHC_FILLPARAMS_FOLDER ) );

         // Decode the BCIDs of the colliding and unpaired bunches:
         m_filledBunches    = decodeColliding( *fillparams );
         m_unpairedBunches1 = decodeUnpaired1( *fillparams );
         m_unpairedBunches2 = decodeUnpaired2( *fillparams );
      }

      //
      // If the /TDAQ/OLC/LHC/BUNCHDATA folder is available:
      //
      if( update_bunchdata ) {
         const DataHandle< CondAttrListCollection > bunchdata;
         CHECK( detStore()->retrieve( bunchdata, LHC_BUNCHDATA_FOLDER ) );

         // Check that the folder has two channels:
         if( bunchdata->size() != 2 ) {
            ATH_MSG_WARNING( "Folder '" << LHC_BUNCHDATA_FOLDER << "' has "
                             << bunchdata->size() << " channels. "
                             << "2 was expected." );
         }

         // Check the validity of the intensity information:
         uint32_t valid = 0xf;
         try {
            if( static_cast< int >( bunchdata->size() ) > m_intChannel ) {
               valid =
                  bunchdata->attributeList( m_intChannel )[ "Valid" ].data< uint32_t >();
            }
         } catch( const coral::AttributeListException& ex ) {
            ATH_MSG_DEBUG( "Caught exception with message: " << ex.what() );
         }
         if( valid & 0xf ) {
            ATH_MSG_WARNING( "Problem with the bunch intensity information. "
                             "Using dummy values instead." );
            m_filledIntBeam1.clear();
            m_filledIntBeam2.clear();
            m_unpairedIntBeam1.clear();
            m_unpairedIntBeam2.clear();
            m_intValid = false;
         } else {
            // Decode the intensities of the colliding and unpaired bunches:
            m_filledIntBeam1 =
               decodeCollIntBeam1( bunchdata->attributeList( m_intChannel ) );
            m_filledIntBeam2 =
               decodeCollIntBeam2( bunchdata->attributeList( m_intChannel ) );
            m_unpairedIntBeam1 =
               decodeUnpairedIntBeam1( bunchdata->attributeList( m_intChannel ) );
            m_unpairedIntBeam2 =
               decodeUnpairedIntBeam2( bunchdata->attributeList( m_intChannel ) );
            m_intValid = true;
         }
      }

      //
      // Now let the base class interpret the information:
      //
      ATH_CHECK( loadSingleBunches( m_filledBunches, m_filledIntBeam1,
                                    m_filledIntBeam2 ) );
      ATH_CHECK( loadBunchTrains( m_filledBunches, m_filledIntBeam1,
                                  m_filledIntBeam2 ) );
      ATH_CHECK( loadUnpairedBunches( m_unpairedBunches1, m_unpairedBunches2,
                                      m_unpairedIntBeam1, m_unpairedIntBeam2 ) );

      // Print the configuration to give some feedback to the user:
      printConfig();

      //
      // Now let's find an ID for this configuration:
      //
      const std::string folder = ( ( update_fillparams & ( ! update_bunchdata ) ) ?
                                   LHC_FILLPARAMS_FOLDER :
                                   LHC_BUNCHDATA_FOLDER );
      std::string foldername, tag;
      IOVRange range;
      bool retrieved;
      unsigned long long bytesRead;
      float readTime;
      if( ! m_iovSvc->getKeyInfo( folder, foldername, tag,
                                  range, retrieved, bytesRead, readTime ) ) {
         REPORT_ERROR( StatusCode::FAILURE )
            << "Couldn't get IOV data about folder: "
            << LHC_FILLPARAMS_FOLDER;
         return StatusCode::FAILURE;
      }
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE ) << "Information about folder: "
                                        << folder;
         REPORT_MESSAGE( MSG::VERBOSE ) << "  foldername: " << foldername;
         REPORT_MESSAGE( MSG::VERBOSE ) << "  tag: " << tag;
         REPORT_MESSAGE( MSG::VERBOSE ) << "  range: " << range;
         REPORT_MESSAGE( MSG::VERBOSE ) << "  retrieved: " << retrieved;
         REPORT_MESSAGE( MSG::VERBOSE ) << "  bytesRead: " << bytesRead;
         REPORT_MESSAGE( MSG::VERBOSE ) << "  readTime: " << readTime;
      }
      // Extract the starting time of the IOV:
      IOVTime time = range.start();
      // Construct the ID:
      if( time.isRunEvent() ) {
         m_id = static_cast< configid_type >( ( ( time.run() & 0xffff ) << 16 ) |
                                              ( time.event() & 0xffff ) );
      } else if( time.isTimestamp() ) {
         m_id = static_cast< configid_type >( time.timestamp() & 0xffffffff );
      } else {
         REPORT_MESSAGE( MSG::WARNING )
            << "Couldn't construct ID for the loaded configuration";
         m_id = 0;
      }
      ATH_MSG_DEBUG( "ID assigned to configuration: " << m_id );

      // Let everybody know that the configuration of the tool has changed:
      m_incidentSvc->fireIncident( Incident( "BunchConfiguration update",
                                             BUNCH_CONFIG_INCIDENT_NAME ) );

      return StatusCode::SUCCESS;
   }

   /**
    * This function is used to decode the /TDAQ/OLC/LHC/FILLPARAMS folder
    * from the conditions database. The decoding follows the information found
    * on:
    *
    * https://twiki.cern.ch/twiki/bin/view/Atlas/CoolOnlineData#Folder_TDAQ_OLC_LHC_FILLPARAMS
    *
    * @param attr The AthenaAttributeList describing the requested folder
    * @returns The BCIDs of the colliding bunch crossings
    */
   std::vector< int >
   LHCBunchCrossingTool::decodeColliding( const coral::AttributeList& attr ) {

      // The result object:
      std::vector< int > result;

      // Extract the raw information out of the attribute list:
      m_beam1Bunches    = attr[ "Beam1Bunches" ].data< uint32_t >();
      m_beam2Bunches    = attr[ "Beam2Bunches" ].data< uint32_t >();
      m_luminousBunches = attr[ "LuminousBunches" ].data< uint32_t >();
      const coral::Blob& bcids = attr[ "BCIDmasks" ].data< coral::Blob >();

      // Give some very verbose feedback:
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE )
            << "Beam1Bunches : " << m_beam1Bunches;
         REPORT_MESSAGE( MSG::VERBOSE )
            << "Beam2Bunches : " << m_beam2Bunches;
         REPORT_MESSAGE( MSG::VERBOSE )
            << "LuminousBunches : " << m_luminousBunches;
         REPORT_MESSAGE( MSG::VERBOSE )
            << "BCIDmasks size : " << bcids.size();
      }

      //
      // Decide which run it looks like:
      //
      if( static_cast< uint32_t >( bcids.size() ) ==
          ( 2 * ( m_beam1Bunches + m_beam2Bunches +
                  m_luminousBunches ) ) ) {

         // This looks like Run 1...

         // Extract the collising bunch crossing BCIDs:
         const int16_t* p =
            static_cast< const short* >( bcids.startingAddress() );
         p += m_beam1Bunches + m_beam2Bunches;
         for( uint32_t i = 0; i < m_luminousBunches; ++i, ++p ) {
            if( msgLvl( MSG::VERBOSE ) ) {
               REPORT_MESSAGE( MSG::VERBOSE ) << "Luminous BCID: " << *p;
            }
            result.push_back( *p );
         }

      } else if( bcids.size() == BunchCrossing::MAX_BCID ) {

         // This looks like Run 2...

         // Extract the collising bunch crossing BCIDs:
         const int8_t* p =
            static_cast< const int8_t* >( bcids.startingAddress() );
         for( int i = 0; i < BunchCrossing::MAX_BCID; ++i, ++p ) {
            if( *p != 0x3 ) {
               continue;
            }
            if( msgLvl( MSG::VERBOSE ) ) {
               REPORT_MESSAGE( MSG::VERBOSE ) << "Luminous BCID: " << i;
            }
            result.push_back( i );
         }

         // A sanity check:
         if( result.size() != m_luminousBunches ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "Number of colliding bunches found (" << result.size()
               << ") differs from value in the DB (" << m_luminousBunches
               << ")";
         }

      } else {
         REPORT_MESSAGE( MSG::ERROR )
            << "The format of " << LHC_FILLPARAMS_FOLDER
            << " is not what's expected!";
      }

      return result;
   }

   /**
    * This function is used to decode the /TDAQ/OLC/LHC/FILLPARAMS folder
    * from the conditions database. The decoding follows the information found
    * on:
    *
    * https://twiki.cern.ch/twiki/bin/view/Atlas/CoolOnlineData#Folder_TDAQ_OLC_LHC_FILLPARAMS
    *
    * @param attr The AthenaAttributeList describing the requested folder
    * @returns The BCIDs of the unpaired bunch crossings from beam1
    */
   std::vector< int >
   LHCBunchCrossingTool::decodeUnpaired1( const coral::AttributeList& attr ) {

      // The result object:
      std::vector< int > result;

      // Extract the raw information out of the attribute list:
      m_beam1Bunches    = attr[ "Beam1Bunches" ].data< uint32_t >();
      m_beam2Bunches    = attr[ "Beam2Bunches" ].data< uint32_t >();
      m_luminousBunches = attr[ "LuminousBunches" ].data< uint32_t >();
      const coral::Blob& bcids = attr[ "BCIDmasks" ].data< coral::Blob >();

      // Give some very verbose feedback:
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE )
            << "Beam1Bunches : " << m_beam1Bunches;
         REPORT_MESSAGE( MSG::VERBOSE )
            << "Beam2Bunches : " << m_beam2Bunches;
         REPORT_MESSAGE( MSG::VERBOSE )
            << "LuminousBunches : " << m_luminousBunches;
         REPORT_MESSAGE( MSG::VERBOSE )
            << "BCIDmasks size : " << bcids.size();
      }

      // Clear the internal info:
      m_beam1Colliding.clear();

      //
      // Decide which run it looks like:
      //
      if( static_cast< uint32_t >( bcids.size() ) ==
          ( 2 * ( m_beam1Bunches + m_beam2Bunches +
                  m_luminousBunches ) ) ) {

         // This looks like Run 1...

         // Extract the unpaired bunch crossing BCIDs from beam 1 only:
         const int16_t* p =
            static_cast< const short* >( bcids.startingAddress() );
         for( uint32_t i = 0; i < m_beam1Bunches; ++i, ++p ) {
            // Only consider bunches that are not in the colliding bunch
            // list:
            if( std::find( m_filledBunches.begin(),
                           m_filledBunches.end(), *p ) ==
                m_filledBunches.end() ) {
               if ( std::find( result.begin(), result.end(), *p ) ==
                    result.end() ) {
                  if( msgLvl( MSG::VERBOSE ) ) {
                     REPORT_MESSAGE( MSG::VERBOSE ) << "B1 unpaired BCID: "
                                                    << *p;
                  }
                  result.push_back( *p );
               }
               m_beam1Colliding.push_back( false );
            } else {
               m_beam1Colliding.push_back( true );
            }
         }

      } else if( bcids.size() == BunchCrossing::MAX_BCID ) {

         // This looks like Run 2...

         // Extract the collising bunch crossing BCIDs:
         const int8_t* p =
            static_cast< const int8_t* >( bcids.startingAddress() );
         for( int i = 0; i < BunchCrossing::MAX_BCID; ++i, ++p ) {
            if( ! ( *p & 0x1 ) ) {
               continue;
            }
            if( *p == 0x1 ) {
               result.push_back( i );
               m_beam1Colliding.push_back( false );
            } else {
               m_beam1Colliding.push_back( true );
            }
         }

         // A sanity check:
         if( m_beam1Colliding.size() != m_beam1Bunches ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "Number of beam 1 bunches found (" << m_beam1Colliding.size()
               << ") differs from value in the DB (" << m_beam1Bunches
               << ")";
         }

      } else {
         REPORT_MESSAGE( MSG::ERROR )
            << "The format of " << LHC_FILLPARAMS_FOLDER
            << " is not what's expected!";
      }

      return result;
   }

   /**
    * This function is used to decode the /TDAQ/OLC/LHC/FILLPARAMS folder
    * from the conditions database. The decoding follows the information found
    * on:
    *
    * https://twiki.cern.ch/twiki/bin/view/Atlas/CoolOnlineData#Folder_TDAQ_OLC_LHC_FILLPARAMS
    *
    * @param attr The AthenaAttributeList describing the requested folder
    * @returns The BCIDs of the unpaired bunch crossings from beam2
    */
   std::vector< int >
   LHCBunchCrossingTool::decodeUnpaired2( const coral::AttributeList& attr ) {

      // The result object:
      std::vector< int > result;

      // Extract the raw information out of the attribute list:
      m_beam1Bunches    = attr[ "Beam1Bunches" ].data< uint32_t >();
      m_beam2Bunches    = attr[ "Beam2Bunches" ].data< uint32_t >();
      m_luminousBunches = attr[ "LuminousBunches" ].data< uint32_t >();
      const coral::Blob& bcids = attr[ "BCIDmasks" ].data< coral::Blob >();

      // Give some very verbose feedback:
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE )
            << "Beam1Bunches : " << m_beam1Bunches;
         REPORT_MESSAGE( MSG::VERBOSE )
            << "Beam2Bunches : " << m_beam2Bunches;
         REPORT_MESSAGE( MSG::VERBOSE )
            << "LuminousBunches : " << m_luminousBunches;
         REPORT_MESSAGE( MSG::VERBOSE )
            << "BCIDmasks size : " << bcids.size();
      }

      // Clear the internal info:
      m_beam2Colliding.clear();

      //
      // Decide which run it looks like:
      //
      if( static_cast< uint32_t >( bcids.size() ) ==
          ( 2 * ( m_beam1Bunches + m_beam2Bunches +
                  m_luminousBunches ) ) ) {

         // This looks like Run 1...

         // Extract the unpaired bunch crossing BCIDs from beam 2 only:
         const int16_t* p =
            static_cast< const short* >( bcids.startingAddress() );
         p += m_beam1Bunches;
         for( uint32_t i = 0; i < m_beam2Bunches; ++i, ++p ) {
            // Only consider bunches that are not in the colliding bunch
            // list:
            if( std::find( m_filledBunches.begin(),
                           m_filledBunches.end(), *p ) ==
                m_filledBunches.end() ) {
               if( std::find( result.begin(), result.end(), *p ) ==
                   result.end() ) {
                  if( msgLvl( MSG::VERBOSE ) ) {
                     REPORT_MESSAGE( MSG::VERBOSE ) << "B2 unpaired BCID: "
                                                    << *p;
                  }
                  result.push_back( *p );
               }
               m_beam2Colliding.push_back( false );
            } else {
               m_beam2Colliding.push_back( true );
            }
         }

      } else if( bcids.size() == BunchCrossing::MAX_BCID ) {

         // This looks like Run 2...

         // Extract the collising bunch crossing BCIDs:
         const int8_t* p =
            static_cast< const int8_t* >( bcids.startingAddress() );
         for( int i = 0; i < BunchCrossing::MAX_BCID; ++i, ++p ) {
            if( ! ( *p & 0x2 ) ) {
               continue;
            }
            if( *p == 0x2 ) {
               result.push_back( i );
               m_beam2Colliding.push_back( false );
            } else {
               m_beam2Colliding.push_back( true );
            }
         }

         // A sanity check:
         if( m_beam2Colliding.size() != m_beam2Bunches ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "Number of beam 2 bunches found (" << m_beam2Colliding.size()
               << ") differs from value in the DB (" << m_beam2Bunches
               << ")";
         }

      } else {
         REPORT_MESSAGE( MSG::ERROR )
            << "The format of " << LHC_FILLPARAMS_FOLDER
            << " is not what's expected!";
      }

      return result;
   }

   /**
    * This function is used to decode the /TDAQ/OLC/LHC/BUNCHDATA folder
    * from the conditions database. The decoding follows the information found
    * on:
    *
    * https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CoolOnlineData#Folder_TDAQ_OLC_LHC_BUNCHDATA
    *
    * @param attr The AthenaAttributeList describing one channel of the
    *             requested folder
    * @returns The beam 1 intensities of the colliding bunches
    */
   std::vector< float >
   LHCBunchCrossingTool::
   decodeCollIntBeam1( const coral::AttributeList& attr ) const {

      // Check the validity of the intensity information:
      const uint32_t valid = attr[ "Valid" ].data< uint32_t >();
      if( valid & 0x3 ) {
         ATH_MSG_WARNING( "Beam 1 intensities not valid" );
         ATH_MSG_WARNING( "Will use dummy values for the beam 1 intensities of "
                          "the colliding bunches" );
         return std::vector< float >();
      }

      // Access the beam 1 intensities:
      const coral::Blob& ints =
         attr[ "B1BunchIntensities" ].data< coral::Blob >();

      // Decode the storage format. This code is taken from the luminosity
      // calculation code, as I was just not able to do this properly based
      // on the TWiki...
      const char* format = static_cast< const char* >( ints.startingAddress() );
      const short x = ( ( *format ) % 100 ) / 10; // Storage size in bytes
      const short y = ( *format ) % 10; // Storage format
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE ) << "Storage format     : x = " << x
                                        << ", y = " << y;
         REPORT_MESSAGE( MSG::VERBOSE ) << "B1 intensities size: "
                                        << ints.size();
      }

      // The temporary result object:
      std::vector< double > tmpresult;

      //
      // Use the specialized template functions to decode the relative bunch
      // intensities, taking the format numbers into account:
      //
      if( y == 0 ) {
         if( x == 1 ) {
            tmpresult = decodeIntV0< int8_t >( ints, m_beam1Colliding, true );
         } else if( x == 2 ) {
            tmpresult = decodeIntV0< int16_t >( ints, m_beam1Colliding, true );
         } else if( x == 4 ) {
            tmpresult = decodeIntV0< float >( ints, m_beam1Colliding, true );
         } else {
            REPORT_MESSAGE( MSG::ERROR )
               << "Storage size (" << x << ") not recognized";
            return std::vector< float >();
         }
      } else if( y == 1 ) {
         if( x == 1 ) {
            tmpresult = decodeIntV1< int8_t >( ints, m_filledBunches );
         } else if( x == 2 ) {
            tmpresult = decodeIntV1< int16_t >( ints, m_filledBunches );
         } else if( x == 4 ) {
            tmpresult = decodeIntV1< float >( ints, m_filledBunches );
         } else {
            REPORT_MESSAGE( MSG::ERROR )
               << "Storage size (" << x << ") not recognized";
            return std::vector< float >();
         }
      } else if( y == 2 ) {
         if( x == 1 ) {
            tmpresult = decodeIntV2< int8_t >( ints, m_filledBunches );
         } else if( x == 2 ) {
            tmpresult = decodeIntV2< int16_t >( ints, m_filledBunches );
         } else if( x == 4 ) {
            tmpresult = decodeIntV2< float >( ints, m_filledBunches );
         } else {
            REPORT_MESSAGE( MSG::ERROR )
               << "Storage size (" << x << ") not recognized";
            return std::vector< float >();
         }
      } else {
         REPORT_MESSAGE( MSG::ERROR )
            << "Storage type (" << y << ") not recognized";
         return std::vector< float >();
      }

      // Multiply these relative intensities with the beam 1 average to
      // get the actual intensities:
      const float average = attr[ "B1BunchAverage" ].data< float >();
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE ) << "B1BunchAverage = " << average;
      }
      std::vector< float > result;
      for( size_t i = 0; i < tmpresult.size(); ++i ) {
         if( x == 4 ) {
            result.push_back( static_cast< float >( tmpresult[ i ] ) );
         } else {
            result.push_back( static_cast< float >( average * tmpresult[ i ] ) );
         }
      }

      return result;
   }

   /**
    * This function is used to decode the /TDAQ/OLC/LHC/BUNCHDATA folder
    * from the conditions database. The decoding follows the information found
    * on:
    *
    * https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CoolOnlineData#Folder_TDAQ_OLC_LHC_BUNCHDATA
    *
    * @param attr The AthenaAttributeList describing one channel of the
    *             requested folder
    * @returns The beam 2 intensities of the colliding bunches
    */
   std::vector< float >
   LHCBunchCrossingTool::
   decodeCollIntBeam2( const coral::AttributeList& attr ) const {

      // Check the validity of the intensity information:
      const uint32_t valid = attr[ "Valid" ].data< uint32_t >();
      if( ( valid >> 2 ) & 0x3 ) {
         ATH_MSG_WARNING( "Beam 2 intensities not valid" );
         ATH_MSG_WARNING( "Will use dummy values for the beam 2 intensities of "
                          "the colliding bunches" );
         return std::vector< float >();
      }

      // Access the beam 2 intensities:
      const coral::Blob& ints =
         attr[ "B2BunchIntensities" ].data< coral::Blob >();

      // Decode the storage format. This code is taken from the luminosity
      // calculation code, as I was just not able to do this properly based
      // on the TWiki...
      const char* format = static_cast< const char* >( ints.startingAddress() );
      const short x = ( ( *format ) % 100 ) / 10; // Storage size in bytes
      const short y = ( *format ) % 10; // Storage format
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE ) << "Storage format     : x = " << x
                                        << ", y = " << y;
         REPORT_MESSAGE( MSG::VERBOSE ) << "B2 intensities size: "
                                        << ints.size();
      }

      // The temporary result object:
      std::vector< double > tmpresult;

      //
      // Use the specialized template functions to decode the relative bunch
      // intensities, taking the format numbers into account:
      //
      if( y == 0 ) {
         if( x == 1 ) {
            tmpresult = decodeIntV0< int8_t >( ints, m_beam2Colliding, true );
         } else if( x == 2 ) {
            tmpresult = decodeIntV0< int16_t >( ints, m_beam2Colliding, true );
         } else if( x == 4 ) {
            tmpresult = decodeIntV0< float >( ints, m_beam2Colliding, true );
         } else {
            REPORT_MESSAGE( MSG::ERROR )
               << "Storage size (" << x << ") not recognized";
            return std::vector< float >();
         }
      } else if( y == 1 ) {
         if( x == 1 ) {
            tmpresult = decodeIntV1< int8_t >( ints, m_filledBunches );
         } else if( x == 2 ) {
            tmpresult = decodeIntV1< int16_t >( ints, m_filledBunches );
         } else if( x == 4 ) {
            tmpresult = decodeIntV1< float >( ints, m_filledBunches );
         } else {
            REPORT_MESSAGE( MSG::ERROR )
               << "Storage size (" << x << ") not recognized";
            return std::vector< float >();
         }
      } else if( y == 2 ) {
         if( x == 1 ) {
            tmpresult = decodeIntV2< int8_t >( ints, m_filledBunches );
         } else if( x == 2 ) {
            tmpresult = decodeIntV2< int16_t >( ints, m_filledBunches );
         } else if( x == 4 ) {
            tmpresult = decodeIntV2< float >( ints, m_filledBunches );
         } else {
            REPORT_MESSAGE( MSG::ERROR )
               << "Storage size (" << x << ") not recognized";
            return std::vector< float >();
         }
      } else {
         REPORT_MESSAGE( MSG::ERROR )
            << "Storage type (" << y << ") not recognized";
         return std::vector< float >();
      }

      // Multiply these relative intensities with the beam 2 average to
      // get the actual intensities:
      const float average = attr[ "B2BunchAverage" ].data< float >();
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE ) << "B2BunchAverage = " << average;
      }
      std::vector< float > result;
      for( size_t i = 0; i < tmpresult.size(); ++i ) {
         if( x == 4 ) {
            result.push_back( static_cast< float >( tmpresult[ i ] ) );
         } else {
            result.push_back( static_cast< float >( average * tmpresult[ i ] ) );
         }
      }

      return result;
   }

   /**
    * This function is used to decode the /TDAQ/OLC/LHC/BUNCHDATA folder
    * from the conditions database. The decoding follows the information found
    * on:
    *
    * https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CoolOnlineData#Folder_TDAQ_OLC_LHC_BUNCHDATA
    *
    * @param attr The AthenaAttributeList describing one channel of the
    *             requested folder
    * @returns The beam 1 unpaired bunch intensities
    */
   std::vector< float >
   LHCBunchCrossingTool::
   decodeUnpairedIntBeam1( const coral::AttributeList& attr ) const {

      // Check the validity of the intensity information:
      const uint32_t valid = attr[ "Valid" ].data< uint32_t >();
      if( valid & 0x3 ) {
         ATH_MSG_WARNING( "Beam 1 intensities not valid" );
         ATH_MSG_WARNING( "Will use dummy values for the beam 1 intensities of "
                          "the unpaired bunches" );
         return std::vector< float >();
      }

      // Access the beam 1 intensities:
      const coral::Blob& ints =
         attr[ "B1BunchIntensities" ].data< coral::Blob >();

      // Decode the storage format. This code is taken from the luminosity
      // calculation code, as I was just not able to do this properly based
      // on the TWiki...
      const char* format = static_cast< const char* >( ints.startingAddress() );
      const short x = ( ( *format ) % 100 ) / 10; // Storage size in bytes
      const short y = ( *format ) % 10; // Storage format
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE ) << "Storage format     : x = " << x
                                        << ", y = " << y;
         REPORT_MESSAGE( MSG::VERBOSE ) << "B1 intensities size: "
                                        << ints.size();
      }

      // The temporary result object:
      std::vector< double > tmpresult;

      //
      // Use the specialized template functions to decode the relative bunch
      // intensities, taking the format numbers into account:
      //
      if( y == 0 ) {
         if( x == 1 ) {
            tmpresult = decodeIntV0< int8_t >( ints, m_beam1Colliding, false );
         } else if( x == 2 ) {
            tmpresult = decodeIntV0< int16_t >( ints, m_beam1Colliding, false );
         } else if( x == 4 ) {
            tmpresult = decodeIntV0< float >( ints, m_beam1Colliding, false );
         } else {
            REPORT_MESSAGE( MSG::ERROR )
               << "Storage size (" << x << ") not recognized";
            return std::vector< float >();
         }
      } else if( y == 1 ) {
         if( x == 1 ) {
            tmpresult = decodeIntV1< int8_t >( ints, m_unpairedBunches1 );
         } else if( x == 2 ) {
            tmpresult = decodeIntV1< int16_t >( ints, m_unpairedBunches1 );
         } else if( x == 4 ) {
            tmpresult = decodeIntV1< float >( ints, m_unpairedBunches1 );
         } else {
            REPORT_MESSAGE( MSG::ERROR )
               << "Storage size (" << x << ") not recognized";
            return std::vector< float >();
         }
      } else if( y == 2 ) {
         if( x == 1 ) {
            tmpresult = decodeIntV2< int8_t >( ints, m_unpairedBunches1 );
         } else if( x == 2 ) {
            tmpresult = decodeIntV2< int16_t >( ints, m_unpairedBunches1 );
         } else if( x == 4 ) {
            tmpresult = decodeIntV2< float >( ints, m_unpairedBunches1 );
         } else {
            REPORT_MESSAGE( MSG::ERROR )
               << "Storage size (" << x << ") not recognized";
            return std::vector< float >();
         }
      } else {
         REPORT_MESSAGE( MSG::ERROR )
            << "Storage type (" << y << ") not recognized";
         return std::vector< float >();
      }

      // Multiply these relative intensities with the beam 1 average to
      // get the actual intensities:
      const float average = attr[ "B1BunchAverage" ].data< float >();
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE ) << "B1BunchAverage = " << average;
      }
      std::vector< float > result;
      for( size_t i = 0; i < tmpresult.size(); ++i ) {
         if( x == 4 ) {
            result.push_back( static_cast< float >( tmpresult[ i ] ) );
         } else {
            result.push_back( static_cast< float >( average * tmpresult[ i ] ) );
         }
      }

      return result;
   }

   /**
    * This function is used to decode the /TDAQ/OLC/LHC/BUNCHDATA folder
    * from the conditions database. The decoding follows the information found
    * on:
    *
    * https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CoolOnlineData#Folder_TDAQ_OLC_LHC_BUNCHDATA
    *
    * @param attr The AthenaAttributeList describing one channel of the
    *             requested folder
    * @returns The beam 2 unpaired bunch intensities
    */
   std::vector< float >
   LHCBunchCrossingTool::
   decodeUnpairedIntBeam2( const coral::AttributeList& attr ) const {

      // Check the validity of the intensity information:
      const uint32_t valid = attr[ "Valid" ].data< uint32_t >();
      if( ( valid >> 2 ) & 0x3 ) {
         ATH_MSG_WARNING( "Beam 2 intensities not valid" );
         ATH_MSG_WARNING( "Will use dummy values for the beam 2 intensities of "
                          "the unpaired bunches" );
         return std::vector< float >();
      }

      // Access the beam 1 intensities:
      const coral::Blob& ints =
         attr[ "B2BunchIntensities" ].data< coral::Blob >();

      // Decode the storage format. This code is taken from the luminosity
      // calculation code, as I was just not able to do this properly based
      // on the TWiki...
      const char* format = static_cast< const char* >( ints.startingAddress() );
      const short x = ( ( *format ) % 100 ) / 10; // Storage size in bytes
      const short y = ( *format ) % 10; // Storage format
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE ) << "Storage format     : x = " << x
                                        << ", y = " << y;
         REPORT_MESSAGE( MSG::VERBOSE ) << "B2 intensities size: "
                                        << ints.size();
      }

      // The temporary result object:
      std::vector< double > tmpresult;

      //
      // Use the specialized template functions to decode the relative bunch
      // intensities, taking the format numbers into account:
      //
      if( y == 0 ) {
         if( x == 1 ) {
            tmpresult = decodeIntV0< int8_t >( ints, m_beam2Colliding, false );
         } else if( x == 2 ) {
            tmpresult = decodeIntV0< int16_t >( ints, m_beam2Colliding, false );
         } else if( x == 4 ) {
            tmpresult = decodeIntV0< float >( ints, m_beam2Colliding, false );
         } else {
            REPORT_MESSAGE( MSG::ERROR )
               << "Storage size (" << x << ") not recognized";
            return std::vector< float >();
         }
      } else if( y == 1 ) {
         if( x == 1 ) {
            tmpresult = decodeIntV1< int8_t >( ints, m_unpairedBunches2 );
         } else if( x == 2 ) {
            tmpresult = decodeIntV1< int16_t >( ints, m_unpairedBunches2 );
         } else if( x == 4 ) {
            tmpresult = decodeIntV1< float >( ints, m_unpairedBunches2 );
         } else {
            REPORT_MESSAGE( MSG::ERROR )
               << "Storage size (" << x << ") not recognized";
            return std::vector< float >();
         }
      } else if( y == 2 ) {
         if( x == 1 ) {
            tmpresult = decodeIntV2< int8_t >( ints, m_unpairedBunches2 );
         } else if( x == 2 ) {
            tmpresult = decodeIntV2< int16_t >( ints, m_unpairedBunches2 );
         } else if( x == 4 ) {
            tmpresult = decodeIntV2< float >( ints, m_unpairedBunches2 );
         } else {
            REPORT_MESSAGE( MSG::ERROR )
               << "Storage size (" << x << ") not recognized";
            return std::vector< float >();
         }
      } else {
         REPORT_MESSAGE( MSG::ERROR )
            << "Storage type (" << y << ") not recognized";
         return std::vector< float >();
      }

      // Multiply these relative intensities with the beam 2 average to
      // get the actual intensities:
      const float average = attr[ "B2BunchAverage" ].data< float >();
      if( msgLvl( MSG::VERBOSE ) ) {
         REPORT_MESSAGE( MSG::VERBOSE ) << "B2BunchAverage = " << average;
      }
      std::vector< float > result;
      for( size_t i = 0; i < tmpresult.size(); ++i ) {
         if( x == 4 ) {
            result.push_back( static_cast< float >( tmpresult[ i ] ) );
         } else {
            result.push_back( static_cast< float >( average * tmpresult[ i ] ) );
         }
      }

      return result;
   }

   /**
    * This function can decode the beam intensity blob in case it was
    * encoded with "storage mode 0".
    *
    * In this mode the blob only holds the information about the
    * filled bunches, as they are defined in the FILLPARAMS folder.
    *
    * @param ints The blob holding the beam intensities
    * @param mask A mask specifying which intensities to extract
    * @param state The flag state to select from the specified mask
    * @returns The selected bunch intensities
    */
   template< typename TYPE >
   std::vector< double >
   LHCBunchCrossingTool::decodeIntV0( const coral::Blob& ints,
                                      const std::vector< bool >& mask,
                                      bool state ) const {

      // The result object:
      std::vector< double > result;

      // A little sanity check:
      if( static_cast< size_t >( ints.size() ) !=
          ( sizeof( TYPE ) * mask.size() + 1 ) ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "The bunch intensities are in an unexpected format";
         return result;
      }

      // Skip the first byte:
      const char* tmpptr = static_cast< const char* >( ints.startingAddress() );
      ++tmpptr;

      // Extract the intensities:
      const TYPE* ptr = reinterpret_cast< const TYPE* >( tmpptr );
      for( size_t i = 0; i < mask.size(); ++i, ++ptr ) {
         // Only care about the bunches that the mask selects:
         if( mask[ i ] == state ) {
            // Convert the value to a float:
            double value = static_cast< double >( *ptr );
            // Depending on the size of the storage, normalize
            // this relative value to 1.0:
            if( typeid( TYPE ) == typeid( int8_t ) ) {
               value /= 100.0;
            } else if( typeid( TYPE ) == typeid( int16_t ) ) {
               value /= 10000.0;
            }
            // Now store it:
            result.push_back( value );
            if( msgLvl( MSG::VERBOSE ) ) {
               REPORT_MESSAGE( MSG::VERBOSE )
                  << ( i + 1 ) << ". bunch's relative intensity : "
                  << value;
            }
         }
      }

      return result;
   }

   /**
    * This function can decode the beam intensity blob in case it was
    * encoded with "storage mode 1".
    *
    * In this mode the blob holds the intensities of all the 3564
    * possible bunches. The code has to select the intensities of the
    * bunches that we're interested in.
    *
    * @param ints The blob holding the beam intensities
    * @param bcids The BCIDs for which the intensities have to be extracted
    * @returns The selected bunch intensities
    */
   template< typename TYPE >
   std::vector< double >
   LHCBunchCrossingTool::decodeIntV1( const coral::Blob& ints,
                                      const std::vector< int >& bcids ) const {

      // The result object:
      std::vector< double > result;

      // A little sanity check:
      if( static_cast< size_t >( ints.size() ) !=
          ( sizeof( TYPE ) * BunchCrossing::MAX_BCID + 1 ) ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "The bunch intensities are in an unexpected format";
         return result;
      }

      // Skip the first byte:
      const char* tmpptr = static_cast< const char* >( ints.startingAddress() );
      ++tmpptr;

      // Extract the intensities:
      const TYPE* ptr = reinterpret_cast< const TYPE* >( tmpptr );
      for( int i = 0; i < BunchCrossing::MAX_BCID; ++i, ++ptr ) {
         // Skip the BCIDs that we're not interested in:
         if( std::find( bcids.begin(), bcids.end(), i ) ==
             bcids.end() ) {
            continue;
         }
         // We are interested in this bunch!
         // Convert the value to a float:
         double value = static_cast< double >( *ptr );
         // Depending on the size of the storage, normalize
         // this relative value to 1.0:
         if( typeid( TYPE ) == typeid( int8_t ) ) {
            value /= 100.0;
         } else if( typeid( TYPE ) == typeid( int16_t ) ) {
            value /= 10000.0;
         }
         // Now store it:
         result.push_back( value );
         if( msgLvl( MSG::VERBOSE ) ) {
            REPORT_MESSAGE( MSG::VERBOSE )
               << "Relative intensity of BCID " << i << ": "
               << value;
         }
      }

      return result;
   }

   /**
    * This function can decode the beam intensity blob in case it was
    * encoded with "storage mode 2".
    *
    * In this mode the blob specifies exactly which BCIDs it holds
    * information about.
    *
    * @param ints The blob holding the beam intensities
    * @param bcids The BCIDs for which the intensities have to be extracted
    * @returns The selected bunch intensities
    */
   template< typename TYPE >
   std::vector< double >
   LHCBunchCrossingTool::decodeIntV2( const coral::Blob& ints,
                                      const std::vector< int >& bcids ) const {

      // The result object:
      std::vector< double > result;

      // Check that the blob meets the minimum size requirements:
      if( ints.size() < 2 ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "The bunch intensities are in an unexpected format";
         return result;
      }

      // Extract how many BCIDs we store information about:
      const char* tmpptr = static_cast< const char* >( ints.startingAddress() );
      ++tmpptr;
      const int16_t* bcidptr = reinterpret_cast< const int16_t* >( tmpptr );
      const int16_t bcid_size = *bcidptr;
      ++bcidptr;

      // Check that the blob meets the exact size requirements:
      if( static_cast< size_t >( ints.size() ) !=
          ( 3 + ( 2 + sizeof( TYPE ) ) * bcid_size ) ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "The bunch intensities are in an unexpected format";
         return result;
      }

      // Extract the BCIDs of the stored intensities:
      std::vector< int16_t > stored_bcids;
      for( int16_t i = 0; i < bcid_size; ++i, ++bcidptr ) {
         stored_bcids.push_back( *bcidptr );
      }

      // Now extract the intensities:
      std::map< int16_t, double > intensities;
      const TYPE* ptr = reinterpret_cast< const TYPE* >( bcidptr );
      for( int16_t i = 0; i < bcid_size; ++i, ++ptr ) {
         // Convert the value to a float:
         double value = static_cast< double >( *ptr );
         // Depending on the size of the storage, normalize
         // this relative value to 1.0:
         if( typeid( TYPE ) == typeid( int8_t ) ) {
            value /= 100.0;
         } else if( typeid( TYPE ) == typeid( int16_t ) ) {
            value /= 10000.0;
         }
         // Store the intensity:
         intensities[ stored_bcids[ i ] ] = value;
         if( msgLvl( MSG::VERBOSE ) ) {
            REPORT_MESSAGE( MSG::VERBOSE )
               << "Relative intensity of BCID " << stored_bcids[ i ] << ": "
               << value;
         }
      }

      // Let's fill up the result vector:
      std::vector< int >::const_iterator itr = bcids.begin();
      std::vector< int >::const_iterator end = bcids.end();
      for( ; itr != end; ++itr ) {
         // Check if we have information about this BCID:
         std::map< int16_t, double >::const_iterator int_itr =
            intensities.find( *itr );
         if( int_itr == intensities.end() ) {
            REPORT_MESSAGE( MSG::ERROR )
               << "Couldn't find bunch intensity information for BCID "
               << *itr;
            return std::vector< double >();
         }
         // Add it to the output:
         result.push_back( int_itr->second );
      }

      return result;
   }

} // namespace Trig
