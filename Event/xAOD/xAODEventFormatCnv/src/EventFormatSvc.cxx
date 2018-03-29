/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <fstream>
#include <ctime>
#include <locale>
#include <memory>

// Athena/Gaudi include(s):
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/GaudiException.h"
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"

// Local include(s):
#include "EventFormatSvc.h"

/// Helper function for checking StatusCode return values in void functions
#define CHECK_VOID( EXP )                                        \
   do {                                                          \
      if( EXP.isFailure() ) {                                    \
         REPORT_ERROR( MSG::FATAL )                              \
            << "Failed to execute: " << #EXP;                    \
         throw GaudiException( "Failed to execute \"" #EXP "\"", \
                               "xAODMaker::EventFormatSvc",      \
                               StatusCode::FAILURE );            \
      }                                                          \
   } while( false )

namespace xAODMaker {

   EventFormatSvc::EventFormatSvc( const std::string& name,
                                   ISvcLocator* svcLoc )
      : AthService( name, svcLoc ),
        m_clidSvc( "ClassIDSvc", name ),
        m_eventStore( "StoreGateSvc", name ),
        m_inputMetaStore( "StoreGateSvc/InputMetaDataStore", name ),
        m_metaStore( "StoreGateSvc/MetaDataStore", name ),
        m_incidentSvc( "IncidentSvc", name ),
        m_warnedCLIDs(),
        m_ef( 0 ), m_firstEvent( true ) {

      declareProperty( "ObjectName", m_objectName = "EventFormat" );

      declareProperty( "ClassIDSvc", m_clidSvc );
      declareProperty( "EventStore", m_eventStore );
      declareProperty( "InputMetaDataStore", m_inputMetaStore );
      declareProperty( "MetaDataStore", m_metaStore );
      declareProperty( "IncidentSvc", m_incidentSvc );
      declareProperty( "FormatNames", m_formatNames );
   }

   StatusCode EventFormatSvc::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " Output object key: " << m_objectName );

      // Retrieve the needed service(s):
      CHECK( m_clidSvc.retrieve() );
      CHECK( m_eventStore.retrieve() );
      CHECK( m_inputMetaStore.retrieve() );
      CHECK( m_metaStore.retrieve() );
      CHECK( m_incidentSvc.retrieve() );

      // Set up the service to listen to end-of-event incidents:
      m_incidentSvc->addListener( this, IncidentType::EndEvent );

      // Clean up:
      m_warnedCLIDs.clear();

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This function can be used by some client code to access the EventFormat
    * object describing the contents of the event store.
    *
    * It's debatable how much use case there is for accessing this info in
    * Athena, we'll have to see. The code could also be extended later on
    * to allow access to the EventFormat object read from an input xAOD
    * file. To be seen...
    *
    * @returns The EventFormat object describing the current contents of the
    *          event store
    */
   const xAOD::EventFormat* EventFormatSvc::eventFormat() {

      // Check if we already have an object:
      if( ! m_ef ) {
         // Request one then:
         const Incident inc( "xAODMaker::EventFormatSvc",
                             IncidentType::EndEvent );
         handle( inc );
         // Reset the first event flag, as we obviously didn't get to the
         // end of the first event yet.
         m_firstEvent = true;
      }

      // Return the object:
      return m_ef;
   }

   void EventFormatSvc::handle( const Incident& inc ) {

      // Tell the user what we're doing:
      ATH_MSG_DEBUG( "Callback received with incident: "
                     << inc.type() );

      // Check that we received the correct incident:
      if( inc.type() != IncidentType::EndEvent ) {
         ATH_MSG_WARNING( "Unknown incident type received: "
                          << inc.type() );
         return;
      }

      // Create the object if it doesn't exist yet:
      if( ! m_ef ) {

         // Create the new object:
         auto ef = std::make_unique< xAOD::EventFormat >();
         ATH_MSG_VERBOSE( "Creating new xAOD::EventFormat object" );

         // Check if there's such a metadata object on the input:
         if( m_inputMetaStore->contains< xAOD::EventFormat >( m_objectName ) ) {
            // If yes, copy its payload to our managed object:
            const xAOD::EventFormat* input = nullptr;
            CHECK_VOID( m_inputMetaStore->retrieve( input, m_objectName ) );
            *ef = *input;
            ATH_MSG_VERBOSE( "Copied the metadata from the (first) input "
                             "file" );
         }

         // Record the object to the output:
         CHECK_VOID( m_metaStore->record( std::move( ef ), m_objectName ) );
         // If this was successful, set the m_ef pointer:
         CHECK_VOID( m_metaStore->retrieve( m_ef, m_objectName ) );
      }

      // Collect information into the object:
      const int newObjects = collectMetadata( *m_ef );
      if( ( ! m_firstEvent ) && newObjects ) {
         ATH_MSG_DEBUG( newObjects << " new objects encountered during "
                        << "event processing (not the first event!)" );
      }

      // Now we're past the first event:
      m_firstEvent = false;

      return;
   }

   StatusCode EventFormatSvc::queryInterface( const InterfaceID& riid,
                                              void** ppvIf ) {

      // Check if the user asked for the IEventFormatSvc interface:
      if( riid == IEventFormatSvc::interfaceID() ) {
         *ppvIf = static_cast< IEventFormatSvc* >( this );
         addRef();
         return StatusCode::SUCCESS;
      }
      // Check if the user asked for the IIncidentListener interface:
      else if( riid == IIncidentListener::interfaceID() ) {
         *ppvIf = static_cast< IIncidentListener* >( this );
         addRef();
         return StatusCode::SUCCESS;
      }

      // If not, let the base class handle the request:
      return AthService::queryInterface( riid, ppvIf );
   }

   /**
    * This function is used internally to collect the necessary metadata
    * from the event store (StoreGate) and fill the EventFormat object
    * of the service.
    *
    * The function returns the number of "new" objects encountered, which
    * should only be !=0 on the first event in principle. But by now we have
    * a few use cases where this is not true...
    *
    * @param ef The EventFormat object to fill with the metadata
    * @returns The number of "new" objects found in the event
    */
   int EventFormatSvc::collectMetadata( xAOD::EventFormat& ef ) {

      // Ask StoreGate for all the object that it's holding on to:
      const std::vector< const SG::DataProxy* > proxies =
         m_eventStore->proxies();

      // Number of new objects encountered:
      int newObjects = 0;

      // Loop over them:
      std::vector< const SG::DataProxy* >::const_iterator itr = proxies.begin();
      std::vector< const SG::DataProxy* >::const_iterator end = proxies.end();
      for( ; itr != end; ++itr ) {

         // Construct the branch name that Athena I/O is going to give to
         // this object:
         const std::string branchName = ( *itr )->name();

         // Ignore the temporary containers created by the HLT, as those change
         // event by event:
         if( branchName.find( "HLTAutoKey_" ) == 0 ) continue;

         // Get the type name of this object:
         std::string typeName;
         if( m_clidSvc->getTypeInfoNameOfID( ( *itr )->clID(),
                                             typeName ).isFailure() ) {
            // Print a warning if this CLID didn't produce a warning yet:
            if( m_warnedCLIDs.insert( ( *itr )->clID() ).second ) {
               ATH_MSG_WARNING( "Couldn't get type name for CLID = "
                                << ( *itr )->clID() );
            }
            continue;
         }

         // Only consider xAOD objects. Note that the type name will not
         // necessarily start with xAOD, but it should have it somewhere.
         // Alternatively, the type name could be listed in FormatNames.
         if( typeName.find( "xAOD" ) == std::string::npos &&
             std::find (m_formatNames.begin(), m_formatNames.end(), typeName) ==
             m_formatNames.end())
         {
           continue;
         }

         // Check if we already know about this object:
         if( ef.exists( branchName ) ) continue;

         // This will be a new object:
         ++newObjects;

         // Retrieve the hash of this name:
         const uint32_t hash = m_eventStore->stringToKey( branchName,
                                                          ( *itr )->clID() );

         // Add the info:
         ef.add( xAOD::EventFormatElement( branchName, typeName, "",
                                           hash ) );

         // Tell the user what happened:
         ATH_MSG_VERBOSE( "Adding info: branchName = \"" << branchName
                          << ", typeName = \"" << typeName << "\""
                          << ", hash = 0x" << std::hex << std::setw( 8 )
                          << std::setfill( '0' ) << hash );
      }

      // Return how many new objects were encountered:
      return newObjects;
   }

} // namespace xAODMaker
