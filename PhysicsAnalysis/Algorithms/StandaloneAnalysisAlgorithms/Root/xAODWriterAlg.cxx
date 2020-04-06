//
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//

// System include(s):
#include <regex>

// ROOT include(s):
#include <TClass.h>
#include <TFile.h>

// Core include(s):
#include "AthContainers/normalizedTypeinfoName.h"
#include "EventLoop/Worker.h"
#include "SystematicsHandles/Helpers.h"

// Local include(s):
#include "StandaloneAnalysisAlgorithms/xAODWriterAlg.h"

namespace CP {

   xAODWriterAlg::xAODWriterAlg( const std::string& name, ISvcLocator* svcLoc )
      : EL::AnaAlgorithm( name, svcLoc ) {

      // Declare the algorithm's properties.
      declareProperty( "OutputStreamName", m_outputStreamName = "ANALYSIS",
                       "Stream name of the output file to use" );
      declareProperty( "ItemList", m_itemList,
                       "Objects to write to the output file" );
      declareProperty( "BasketSize", m_basketSize = 32000,
                       "(Starter) Basket size for the created branches" );
      declareProperty( "SplitLevel", m_splitLevel = 0,
                       "Split level for the created branches" );
   }

   StatusCode xAODWriterAlg::initialize() {

      // Make sure that the xAOD::TEvent object managed by EventLoop is the
      // "active" one.
      evtStore()->event()->setActive();

      // Set up the systematics list.
      ATH_CHECK( m_systematicsList.initialize() );

      // Access the file of the output stream.
      TFile* ofile = wk()->getOutputFile( m_outputStreamName );
      if( ! ofile ) {
         ATH_MSG_FATAL( "Couldn't access output file for stream \""
                        << m_outputStreamName << "\"" );
         return StatusCode::FAILURE;
      }

      // Write to this output file.
      ANA_CHECK( m_event.writeTo( ofile ) );

      // Reset the internal flag(s).
      m_itemListInitialized = false;

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode xAODWriterAlg::execute() {

      // If this is the first event, figure out which objects can actually be
      // written out.
      if( ! m_itemListInitialized ) {
         ANA_CHECK( setup() );
         m_itemListInitialized = true;
      }

      // Write all objects to the output file.
      xAOD::TEvent* event = evtStore()->event();
      for( const Item& item : m_writtenItemList ) {

         // Get the object. See the description in @c xAOD::TEvent::retrieve
         // (the const version) for an explanation of this implementation.
         static const bool SILENT = true;
         static const bool METADATA = false;
         const void* obj = event->getOutputObject( item.name, *( item.type ),
                                                   METADATA );
         if( ! obj ) {
            obj = event->getInputObject( item.name, *( item.type ), SILENT,
                                         METADATA );
         } else {
            event->getInputObject( item.name, *( item.type ), SILENT,
                                   METADATA );
         }

         // Check that we succeeded.
         if( ! obj ) {
            ATH_MSG_FATAL( "Couldn't retrieve object \"" << item.name << "\"" );
            return StatusCode::FAILURE;
         }

         // Record it to the output for the current event.
         ANA_CHECK( m_event.record( const_cast< void* >( obj ), item.typeName,
                                    item.name, m_basketSize, m_splitLevel ) );
      }

      // Write the event.
      if( m_event.fill() <= 0 ) {
         ATH_MSG_FATAL( "There was an error writing out the event" );
         return StatusCode::FAILURE;
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode xAODWriterAlg::finalize() {

      // Access the file of the output stream.
      TFile* ofile = wk()->getOutputFile( m_outputStreamName );
      if( ! ofile ) {
         ATH_MSG_FATAL( "Couldn't access output file for stream \""
                        << m_outputStreamName << "\"" );
         return StatusCode::FAILURE;
      }

      // Finish writing to this output file.
      ANA_CHECK( m_event.finishWritingTo( ofile ) );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode xAODWriterAlg::setup() {

      // Loop over all of the declared items.
      for( const std::string& stringItem : m_itemList ) {

         // Interpret the item string.
         static const std::regex itemRegex( "([^#]+)#([^\\.]+\\.?)(.*)" );
         std::smatch itemMatch;
         if( ! std::regex_match( stringItem, itemMatch, itemRegex ) ) {
            ATH_MSG_FATAL( "Item \"" << stringItem
                           << "\" is not of the form: \"Type#Name\"" );
            return StatusCode::FAILURE;
         }
         ATH_MSG_VERBOSE( "Found item: " << itemMatch[ 1 ] << "#"
                          << itemMatch[ 2 ] << itemMatch[ 3 ] );

         // Consider all systematics. Not usin CP::SysListHandle::foreach, to
         // be able to exit the for-loop early if necessary.
         auto sysVector = m_systematicsList.systematicsVector();
         for( const auto& sys : sysVector ) {

            // Event store key for the object under consideration.
            const std::string key = makeSystematicsName( itemMatch[ 2 ], sys );

            // Whether or not the object will be available, as long as
            // variable selection rules were set up for it, let xAOD::TEvent
            // know about them.
            if( itemMatch[ 3 ] != "" ) {
               ATH_MSG_DEBUG( "Calling setAuxItemList( \"" << key << "\""
                              << ", \"" << itemMatch[ 3 ]
                              << "\" )" );
               m_event.setAuxItemList( key, itemMatch[ 3 ] );
            }

            // Construct an Item object.
            Item item;
            item.name = key;
            TClass* cl = TClass::GetClass( itemMatch[ 1 ].str().c_str() );
            if( ! cl ) {
               ATH_MSG_FATAL( "Type \"" << itemMatch[ 1 ] << "\" not found" );
               return StatusCode::FAILURE;
            }
            item.type = cl->GetTypeInfo();
            if( ! item.type ) {
               ATH_MSG_FATAL( "No compiled dictionary found for \""
                              << itemMatch[ 1 ] << "\"" );
               return StatusCode::FAILURE;
            }
            item.typeName = SG::normalizedTypeinfoName( *( item.type ) );

            // Check if the item is available.
            static const bool SILENT = true;
            static const bool METADATA = false;
            xAOD::TEvent* event = evtStore()->event();
            if( event->getOutputObject( item.name, *( item.type ), METADATA ) ||
                event->getInputObject( item.name, *( item.type ), SILENT,
                                       METADATA ) ) {
                  m_writtenItemList.push_back( item );
                  ATH_MSG_DEBUG( "Scheduling \"" << itemMatch[ 1 ] << "#"
                                 << key << "\" for writing" );
            }

            // If there was no %SYS% pattern in the object name, stop the loop
            // over the systematics now.
            if( key == itemMatch[ 2 ] ) {
               break;
            }
         }
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace CP
