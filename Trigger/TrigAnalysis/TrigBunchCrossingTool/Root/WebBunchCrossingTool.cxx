/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: WebBunchCrossingTool.cxx 745783 2016-05-10 06:47:18Z krasznaa $

// System include(s):
#include <sstream>
#include <iostream>

// ROOT include(s):
#include "TSocket.h"
#include "TUrl.h"
#include "TString.h"

// xAOD include(s):
#include "xAODEventInfo/EventInfo.h"

// Local include(s):
#include "TrigBunchCrossingTool/WebBunchCrossingTool.h"
#include "TrigBunchCrossingTool/BunchCrossing.h"
#include "json/elements.h"
#include "json/reader.h"

namespace Trig {

   /// Address of the default trigger configuration server
   static const char* TRIGCONF_ADDRESS = "atlas-trigconf.cern.ch";
   /// Folder for the bunchgroup configuration
   static const char* TRIGCONF_BGFOLDER = "bunchgroups";
   /// Port of the trigger configuration web server
   static const Int_t TRIGCONF_PORT = 80;
   /// Default timeout in miliseconds for reading the webpages
   static const Long_t TRIGCONF_TIMEOUT = 20000;
   /// Code of the collision bunch crossings
   static const Int_t COLLISION_BUNCHGROUP_INDEX = 1;
   /// First index of the unpaired bunch crossings
   static const Int_t UNPAIRED_BUNCHGROUP_1_INDEX = 4;
   /// Second index of the unpaired bunch crossings
   static const Int_t UNPAIRED_BUNCHGROUP_2_INDEX = 5;

   /// Output operator for IOV objects
   MsgStream& operator<< ( MsgStream& stream,
                           const WebBunchCrossingTool::IOV& iov );

   WebBunchCrossingTool::WebBunchCrossingTool( const std::string& name )
      : BunchCrossingToolBase( name ),
        m_bgkey( -1 ), m_iov( 0, 0 ), m_knownConfigs(),
        m_configMap() {

      declareProperty( "ServerAddress", m_serverAddress = TRIGCONF_ADDRESS );
      declareProperty( "ServerTimeout", m_serverTimeout = TRIGCONF_TIMEOUT );
   }

   /**
    * This is the fairly complicated function which (if necessary) loads the
    * configuration belonging to BG key "bgkey" from either the internal cache
    * of the tool, or from the atlas-trigconf.cern.ch webpage directly.
    *
    * @param bgkey The bunch group key that should be loaded
    * @returns <code>StatusCode::SUCCESS</code> if the operation was successful,
    *          or <code>StatusCode::FAILURE</code> if it wasn't
    */
   StatusCode WebBunchCrossingTool::loadConfig( int bgkey ) {

      //
      // Check if this is the currently loaded configuration or not:
      //
      if( bgkey == m_bgkey ) {
         ATH_MSG_VERBOSE( "BGKey " << bgkey << " already loaded" );
         return StatusCode::SUCCESS;
      }

      // Set the IOV to an invalid state. Since whatever happens next, we
      // can't know for sure which IOV the loaded configuration corresponds to.
      m_iov = IOV( 0, 0 );

      //
      // If not loaded, check if its configuration is already known:
      //
      std::map< int, std::pair< std::vector< int >,
         std::vector< int > > >::const_iterator
         conf_itr = m_knownConfigs.find( bgkey );
      if( conf_itr != m_knownConfigs.end() ) {

         ATH_MSG_DEBUG( "BGKey " << bgkey
                        << " already known, loading from cache" );

         // Let the base class load this configuration:
         ATH_CHECK( loadSingleBunches( conf_itr->second.first ) );
         ATH_CHECK( loadBunchTrains( conf_itr->second.first ) );
         ATH_CHECK( loadUnpairedBunches( conf_itr->second.second,
                                         conf_itr->second.second ) );
         m_bgkey = bgkey;

         // Print the configuration to give some feedback to the user:
         ATH_MSG_INFO( "Loaded bunch group key " << m_bgkey );
         printConfig();

         // Return gracefully:
         return StatusCode::SUCCESS;
      }

      // Let the user know what we're doing:
      ATH_MSG_DEBUG( "Requesting new BG key (" << bgkey << ") from database" );

      //
      // If this is a completely unknown configuration, read the JSON
      // configuration from the webpage:
      //
      std::ostringstream conn;
      conn << TRIGCONF_BGFOLDER << "?key=" << bgkey << "&type=json";
      const std::string json_data = readJSONConfig( conn.str().c_str() );
      ATH_MSG_VERBOSE( "JSON data: " << json_data );

      //
      // Parse the JSON data:
      //
      std::istringstream json_stream( json_data );
      json::Array bgsets;
      try {
         json::Reader::Read( bgsets, json_stream );
      } catch( const json::Reader::ParseException& e ) {
         ATH_MSG_ERROR( "JSON parse exception on string: "
                        << json_data );
         ATH_MSG_ERROR( "What: " << e.what() << ", line/offset: "
                        << e.m_locTokenBegin.m_nLine + 1 << "/"
                        << e.m_locTokenBegin.m_nLineOffset + 1 );
         return StatusCode::FAILURE;
      } catch( const json::Reader::ScanException& e ) {
         ATH_MSG_ERROR( "JSON scan exception on string: "
                        << json_data );
         ATH_MSG_ERROR( "What: " << e.what() << ", line/offset: "
                        << e.m_locError.m_nLine + 1 << "/"
                        << e.m_locError.m_nLineOffset + 1 );
         return StatusCode::FAILURE;
      } catch(...) {
         ATH_MSG_ERROR( "Unknown exception caught while parsing JSON string: "
                        << json_data );
         return StatusCode::FAILURE;
      }

      // BCIDs of the collision bunch crossings:
      std::vector< int > filledBcids;
      // BCIDs of the unpaired bunch crossings:
      std::vector< int > unpairedBcids;

      //
      // Now interpret the JSON data:
      //
      try {

         // A little sanity check:
         if( bgsets.Size() != 1 ) {
            ATH_MSG_ERROR( "Unexpected size (" << bgsets.Size()
                           << ") for the configuration" );
            return StatusCode::FAILURE;
         }

         // Access the only bunhgroup set:
         const json::Object& bgset = bgsets[ 0 ];
         const json::Array& bunchGroups = bgset[ "code" ];

         // Make a check on the BG key provided:
         const json::Number& jsbgkey = bgset[ "bgkey" ];
         if( static_cast< int >( jsbgkey.Value() ) != bgkey ) {
            ATH_MSG_ERROR( "The webpage provided the configuration of the "
                           "wrong BGKey!" );
            ATH_MSG_ERROR( "Provided key " << jsbgkey.Value()
                           << ", should've been: " << bgkey );
            return StatusCode::FAILURE;
         }

         // Another sanity check:
         if( ( bunchGroups.Size() != 8 ) && ( bunchGroups.Size() != 16 ) ) {
            ATH_MSG_ERROR( "Received information about "
                           << bunchGroups.Size() << " bunch groups "
                           << "(!=8|16)" );
            return StatusCode::FAILURE;
         }

         // Select the collision bunch group:
         const json::Array& jbcids = bunchGroups[ COLLISION_BUNCHGROUP_INDEX ];

         // Add each collision bcid to our std::vector:
         json::Array::const_iterator itr = jbcids.Begin();
         json::Array::const_iterator end = jbcids.End();
         for( ; itr != end; ++itr ) {
            const json::Number& bcid = *itr;
            filledBcids.push_back( static_cast< int >( bcid.Value() ) );
         }

         // Select the first unpaired bunch group:
         const json::Array& jbcids1 =
            bunchGroups[ UNPAIRED_BUNCHGROUP_1_INDEX ];
         itr = jbcids1.Begin();
         end = jbcids1.End();
         for( ; itr != end; ++itr ) {
            const json::Number& bcid = *itr;
            unpairedBcids.push_back( static_cast< int >( bcid.Value() ) );
         }

         // Select the second unpaired bunch group:
         const json::Array& jbcids2 =
            bunchGroups[ UNPAIRED_BUNCHGROUP_2_INDEX ];
         itr = jbcids2.Begin();
         end = jbcids2.End();
         for( ; itr != end; ++itr ) {
            const json::Number& bcid = *itr;
            unpairedBcids.push_back( static_cast< int >( bcid.Value() ) );
         }

      } catch( const json::Exception& ex ) {
         ATH_MSG_ERROR( "Caught JSON exception while parsing configuration" );
         ATH_MSG_ERROR( "What: " << ex.what() );
         return StatusCode::FAILURE;
      }

      //
      // Now let the base class interpret the configuration:
      //
      ATH_CHECK( loadSingleBunches( filledBcids ) );
      ATH_CHECK( loadBunchTrains( filledBcids ) );
      ATH_CHECK( loadUnpairedBunches( unpairedBcids, unpairedBcids ) );

      // Remember which BG key we just loaded:
      m_bgkey = bgkey;
      m_knownConfigs[ bgkey ] = std::make_pair( filledBcids, unpairedBcids );
      // Unfortunately we don't get IOVs (at least not meaningful ones) in this
      // case.

      // Print the configuration to give some feedback to the user:
      ATH_MSG_INFO( "Loaded bunch group key " << m_bgkey );
      printConfig();

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This function can be used to load the bunch structure information for a
    * given run and luminosity block number.
    *
    * First the function checks in its cache which BG key this period
    * corresponds to. If the requested luminosity block is already known in the
    * cache, then the function asks loadConfig(int) to load the configuration.
    * If the luminosity block is not known, the function reads the configuration
    * for the entire run from the webpage, and lets cacheJSONConfig(...) load it
    * into the internal cache. After the new configuration is cached, the
    * function calls itself recursively, because this time it will know right
    * away what to do.
    *
    * @param run The run number of the data you're analysing
    * @param lb The luminosity block number of the data you're analysing
    * @returns <code>StatusCode::SUCCESS</code> if the configuration was loaded
    *          successfully, or <code>StatusCode::FAILURE</code> if it wasn't
    */
   StatusCode WebBunchCrossingTool::loadConfig( unsigned int run,
                                                unsigned int lb ) {

      // Check if we already have this configuration loaded:
      if( m_iov == IOV( run, lb ) ) {
         return StatusCode::SUCCESS;
      }

      // Check if the configuration for this LB is already known:
      const IOV iov( run, lb );
      std::map< std::pair< IOV, IOV >, int >::const_iterator c_itr =
         m_configMap.begin();
      std::map< std::pair< IOV, IOV >, int >::const_iterator c_end =
         m_configMap.end();
      for( ; c_itr != c_end; ++c_itr ) {
         if( ( c_itr->first.first <= iov ) &&
             ( iov < c_itr->first.second ) ) {
            // Load the configuration for this BG key (which should be cached
            // by now):
            ATH_CHECK( loadConfig( c_itr->second ) );
            // Remember the IOV that we just loaded:
            m_iov = IOV( run, lb );
            // Return gracefully:
            return StatusCode::SUCCESS;
         }
      }

      // Let the user know what we're doing:
      ATH_MSG_DEBUG( "Requesting new run (" << run << ") from database" );

      //
      // Read the full configuration for the run in question from the webpage:
      //
      std::ostringstream conn;
      conn << TRIGCONF_BGFOLDER << "?run=" << run << "&type=json";
      const std::string json_data = readJSONConfig( conn.str().c_str() );
      ATH_MSG_VERBOSE( "JSON data: " << json_data );

      // Cache this configuration:
      ATH_CHECK( cacheJSONConfig( json_data ) );

      // Instead of trying to find the appropriate entry from the configuration
      // cache here, let's just call this very same function again. Now the
      // first check should find the requested configuration as being in the
      // cache.
      ATH_CHECK( loadConfig( run, lb ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /// This function takes care of keeping the tool up to date with the correct
   /// configuration for the currently loaded event. It needs the EventInfo
   /// object to do so.
   ///
   /// @return <code>StatusCode::SUCCESS</code> if the right configuration could
   ///         be loaded, or <code>StatusCode::FAILURE</code> if not.
   ///
   StatusCode WebBunchCrossingTool::beginEvent() {

      // Get the EventInfo object:
      const xAOD::EventInfo* ei = 0;
      ATH_CHECK( evtStore()->retrieve( ei, "EventInfo" ) );

      // Make sure the right configuration is loaded:
      ATH_CHECK( loadConfig( ei->runNumber(), ei->lumiBlock() ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * I "stole" the implementation of this function from TGHtmlBrowser.cxx.
    * Unfortunately ROOT doesn't publish this code to the outside user, so the
    * best I could do was to copy-paste it.
    *
    * The function checks the size of the page "config" on
    * http://atlas-trigconf.cern.ch/, so that the readJSONConfig(...) function
    * knows how big a memory space to allocate for it. The code doesn't ever
    * use the function directly actually.
    *
    * @param config A page on http://atlas-trigconf.cern.ch/
    * @returns The size of the page in the number of characters
    */
   size_t WebBunchCrossingTool::readSize( const char* config ) const {

      // Create a fixed size buffer for the answer of the page:
      static const Int_t BUF_SIZE = 4096;
      char buf[ BUF_SIZE ];

      // Construct the message asking for the size of the page:
      TString message = "HEAD ";
      message += "http://" + m_serverAddress + "/";
      message += config;
      message += " HTTP/1.0";
      message += "\r\n";
      message += "User-Agent: ROOT-TWebFile/1.1";
      message += "\r\n\r\n";

      // Connect to the server:
      TSocket socket( m_serverAddress.c_str(), TRIGCONF_PORT );
      if( ! socket.IsValid() ) {
         return 0;
      }
      ATH_MSG_VERBOSE( "Valid socket opened for address: " << m_serverAddress
                       << " and port: " << TRIGCONF_PORT );

      // Ask for the information:
      if( socket.SendRaw( message.Data(), message.Length() ) == -1 ) {
         return 0;
      }
      ATH_MSG_VERBOSE( "Request sent for getting the page size" );

      // Wait for data to become available on the socket:
      if( socket.Select( TSocket::kRead, m_serverTimeout ) != 1 ) {
         ATH_MSG_ERROR( "Couldn't get answer within the "
                        << m_serverTimeout << " ms timeout" );
         return 0;
      }

      // Now read the data:
      if( socket.RecvRaw( buf, BUF_SIZE ) == -1 ) {
         return 0;
      }
      ATH_MSG_VERBOSE( "Buffer size message received" );

      // Extract the size of the page from the reply:
      TString reply( buf );
      Ssiz_t idx = reply.Index( "Content-length:", 0, TString::kIgnoreCase );
      if( idx > 0 ) {
         idx += 15;
         TString slen = reply( idx, reply.Length() - idx );
         return ( size_t ) atol( slen.Data() );
      }

      return 0;
   }

   /**
    * This function reads any one of the pages from
    * http://atlas-trigconf.cern.ch/ and returns its contents as an std::string.
    * It could in principle be used to read any of the pages on the server, but
    * the code only uses it to read the JSON configurations of the bunch
    * structure.
    *
    * The function relies on the functionality of readSize(...) to first figure
    * out how much space to reserve for the download.
    *
    * I could've returned the page contents as a char* pointer, but didn't want
    * to introduce too much manual memory management into the code...
    *
    * The function is basically a copy-paste of the readRemote(...) function of
    * TGHtmlBrowser.cxx.
    *
    * @param config A page on http://atlas-trigconf.cern.ch/
    * @returns The contents of the requested page
    */
   std::string
   WebBunchCrossingTool::readJSONConfig( const char* config ) const {

      // The buffer we're going to read the page into:
      char *buf = 0;

      // Check how big a buffer to allocate:
      Ssiz_t size = readSize( config );
      if( size <= 0 ) size = 1024*1024;

      // Construct the message asking for the page:
      TString message = "GET ";
      message += "http://" + m_serverAddress + "/";
      message += config;
      message += "\r\n";

      // Connect to the server:
      TSocket socket( m_serverAddress.c_str(), TRIGCONF_PORT );
      if( ! socket.IsValid() ) {
         return "";
      }
      ATH_MSG_VERBOSE( "Valid socket opened for address: " << m_serverAddress
                       << " and port: " << TRIGCONF_PORT );

      // Ask for the webpage:
      if( socket.SendRaw( message.Data(), message.Length() ) == -1 ) {
         return "";
      }
      ATH_MSG_VERBOSE( "Request sent for getting the page" );

      // Wait for data to become available on the socket:
      if( socket.Select( TSocket::kRead, m_serverTimeout ) != 1 ) {
         ATH_MSG_ERROR( "Couldn't get answer within the "
                        << m_serverTimeout << " ms timeout" );
         return "";
      }

      // Read the answer of the server:
      buf = ( char* ) calloc( size + 1, sizeof( char ) );
      if( socket.RecvRaw( buf, size ) == -1 ) {
         free( buf );
         return "";
      }
      ATH_MSG_VERBOSE( "Page received" );

      // Construct the result, and free up all reserved memory:
      std::string result( buf );
      free( buf );

      return result;
   }

   /**
    * This function receives a JSON configuration string, extracts and stores
    * all the configurations it finds in it.
    *
    * @param data A JSON string
    * @returns <code>StatusCode::SUCCESS</code> if the decoding was successful,
    *          or <code>StatusCode::FAILURE</code> if it wasn't
    */
   StatusCode WebBunchCrossingTool::cacheJSONConfig( const std::string& data ) {

      //
      // Parse the JSON data:
      //
      std::istringstream stream( data );
      json::Array bgsets;
      try {
         json::Reader::Read( bgsets, stream );
      } catch( const json::Reader::ParseException& e ) {
         ATH_MSG_ERROR( "JSON parse exception on string: "
                        << data );
         ATH_MSG_ERROR( "What: " << e.what() << ", line/offset: "
                        << e.m_locTokenBegin.m_nLine + 1 << "/"
                        << e.m_locTokenBegin.m_nLineOffset + 1 );
         return StatusCode::FAILURE;
      } catch( const json::Reader::ScanException& e ) {
         ATH_MSG_ERROR( "JSON scan exception on string: "
                        << data );
         ATH_MSG_ERROR( "What: " << e.what() << ", line/offset: "
                        << e.m_locError.m_nLine + 1 << "/"
                        << e.m_locError.m_nLineOffset + 1 );
         return StatusCode::FAILURE;
      } catch(...) {
         ATH_MSG_ERROR( "Unknown exception caught while parsing JSON string: "
                        << data );
         return StatusCode::FAILURE;
      }

      //
      // Now interpret the JSON data:
      //
      try {

         //
         // Loop over all the configurations:
         //
         json::Array::const_iterator itr = bgsets.Begin();
         json::Array::const_iterator end = bgsets.End();
         for( ; itr != end; ++itr ) {

            // Access the various parameters of the configuration:
            const json::Array& bunchGroups = ( *itr )[ "code" ];
            const json::Number& bgkey      = ( *itr )[ "bgkey" ];
            const json::Array& iovs        = ( *itr )[ "iov" ];
            const json::Number& run1 = iovs[ 0 ][ 0 ];
            const json::Number& lb1  = iovs[ 0 ][ 1 ];
            const json::Number& run2 = iovs[ 1 ][ 0 ];
            const json::Number& lb2  = iovs[ 1 ][ 1 ];

            // A sanity check:
            if( ( bunchGroups.Size() != 8 ) && ( bunchGroups.Size() != 16 ) ) {
               ATH_MSG_ERROR( "Received information about "
                              << bunchGroups.Size() << " bunch groups "
                              << "(!=8|16)" );
               return StatusCode::FAILURE;
            }

            // BCIDs of the collision bunch crossings:
            std::vector< int > filledBcids;
            // BCIDs of the unpaired bunch crossings:
            std::vector< int > unpairedBcids;

            // Select the collision bunch group:
            const json::Array& jbcids =
               bunchGroups[ COLLISION_BUNCHGROUP_INDEX ];

            // Add each collision bcid to our std::vector:
            json::Array::const_iterator itr = jbcids.Begin();
            json::Array::const_iterator end = jbcids.End();
            for( ; itr != end; ++itr ) {
               const json::Number& bcid = *itr;
               filledBcids.push_back( static_cast< int >( bcid.Value() ) );
            }

            // Select the first unpaired bunch group:
            const json::Array& jbcids1 =
               bunchGroups[ UNPAIRED_BUNCHGROUP_1_INDEX ];
            itr = jbcids1.Begin();
            end = jbcids1.End();
            for( ; itr != end; ++itr ) {
               const json::Number& bcid = *itr;
               unpairedBcids.push_back( static_cast< int >( bcid.Value() ) );
            }

            // Select the second unpaired bunch group:
            const json::Array& jbcids2 =
               bunchGroups[ UNPAIRED_BUNCHGROUP_2_INDEX ];
            itr = jbcids2.Begin();
            end = jbcids2.End();
            for( ; itr != end; ++itr ) {
               const json::Number& bcid = *itr;
               unpairedBcids.push_back( static_cast< int >( bcid.Value() ) );
            }

            // Cache this configuration:
            m_knownConfigs[ static_cast< int >( bgkey.Value() ) ] =
               std::make_pair( filledBcids, unpairedBcids );
            IOV iov_begin( static_cast< unsigned int >( run1.Value() ),
                           static_cast< unsigned int >( lb1.Value() ) );
            IOV iov_end( static_cast< unsigned int >( run2.Value() ),
                         static_cast< unsigned int >( lb2.Value() ) );
            m_configMap[ std::make_pair( iov_begin, iov_end ) ] =
               static_cast< int >( bgkey.Value() );

            ATH_MSG_DEBUG( "Cached configuration for IOV range: "
                           << iov_begin << " - " << iov_end );

         }

      } catch( const json::Exception& ex ) {
         ATH_MSG_ERROR( "Caught JSON exception while parsing configuration" );
         ATH_MSG_ERROR( "What: " << ex.what() );
         return StatusCode::FAILURE;
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   WebBunchCrossingTool::IOV::IOV( unsigned int run, unsigned int lb )
      : m_run( run ), m_lb( lb ) {

   }

   unsigned int WebBunchCrossingTool::IOV::run() const {

      return m_run;
   }

   unsigned int WebBunchCrossingTool::IOV::lb() const {

      return m_lb;
   }

   bool WebBunchCrossingTool::IOV::operator< ( const IOV& rhs ) const {

      if( run() != rhs.run() ) {
         return run() < rhs.run();
      } else if( lb() != rhs.lb() ) {
         return lb() < rhs.lb();
      } else {
         return false;
      }
   }

   bool WebBunchCrossingTool::IOV::operator<= ( const IOV& rhs ) const {

      if( run() != rhs.run() ) {
         return run() < rhs.run();
      } else if( lb() != rhs.lb() ) {
         return lb() < rhs.lb();
      } else {
         return true;
      }
   }

   bool WebBunchCrossingTool::IOV::operator== ( const IOV& rhs ) const {

      return ( ( run() == rhs.run() ) && ( lb() == rhs.lb() ) );
   }

   MsgStream& operator<< ( MsgStream& stream,
                           const WebBunchCrossingTool::IOV& iov ) {

      stream << "[run:" << iov.run() << "; lb:" << iov.lb() << "]";
      return stream;
   }

} // namespace Trig
