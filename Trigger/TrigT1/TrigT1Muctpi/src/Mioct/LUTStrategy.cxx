/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTStrategy.cxx 650693 2015-03-01 16:53:48Z masato $

// STL include(s):
#include <sstream>
#include <algorithm>
#include <vector>

// XercesC include(s):
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMElement.hpp>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Trigger include(s):
#include "TrigConfMuctpi/EntityResolver.h"
#include "TrigConfMuctpi/ErrorHandler.h"

// Local include(s):
#include "LUTStrategy.h"

namespace LVL1MUCTPI {

   LUTStrategy::LUTStrategy( const MultiplicityLogic* logic )
      : OverlapStrategy( "LUT-Based-Overlap-Strategy" ), m_logger( "LUTStrategy" ),
        m_multiplicityLogic( logic ) {

      //
      // Create a valid, but empty configuration as a start:
      //
      LUTMultiplicityCalculator calc( m_multiplicityLogic );
      m_calculator_list.push_back( calc );

      for( unsigned int i = 0; i < 8; ++i ) {
         MioctID id( i, NEGATIVE );
         m_calculator_map[ id ] = &m_calculator_list.back();
      }
      for( unsigned int i = 0; i < 8; ++i ) {
         MioctID id( i, POSITIVE );
         m_calculator_map[ id ] = &m_calculator_list.back();
      }
   }

   LUTStrategy::LUTStrategy( const LUTStrategy& strategy )
      : OverlapStrategy( "LUT-Based-Overlap-Strategy" ), m_logger( "LUTStrategy" ),
        m_multiplicityLogic( strategy.m_multiplicityLogic ) {

      // Replicate the list:
      for( std::list< LUTMultiplicityCalculator >::const_iterator it =
              strategy.m_calculator_list.begin(); it != strategy.m_calculator_list.end();
           ++it ) {
         m_calculator_list.push_back( *it );
      }

      // Replicate the map:
      for( std::map< MioctID, const LUTMultiplicityCalculator* >::const_iterator it =
              strategy.m_calculator_map.begin(); it != strategy.m_calculator_map.end();
           ++it ) {

         for( std::list< LUTMultiplicityCalculator >::const_iterator calc =
                 m_calculator_list.begin(); calc != m_calculator_list.end(); ++calc ) {
            if( ( *it->second ) == ( *calc ) ) {
               m_calculator_map[ it->first ] = &( *calc );
               break;
            }
         }
      }
   }

   StatusCode LUTStrategy::initialize( const std::string& lutXMLFile, bool flagMode,
                                       bool dumpLut, const std::string& runPeriod ) {

      m_calculator_list.clear();
      m_calculator_map.clear();

      REPORT_MSG(DEBUG, "Initializing LUTStrategy from file: " << lutXMLFile << " with a RunPeriod setting " << runPeriod );

      //
      // Initialize XercesC:
      //
      try {

         xercesc::XMLPlatformUtils::Initialize();

      } catch( const xercesc::XMLException& ex ) {
         char* error_message = xercesc::XMLString::transcode( ex.getMessage() );
         REPORT_ERROR_MSG( "Failed to initialize XercesC because: " << error_message );
         xercesc::XMLString::release( &error_message );
         return StatusCode::RECOVERABLE;
      }

      //
      // Parse the LUT XML file:
      //
      xercesc::XercesDOMParser* lutParser = new xercesc::XercesDOMParser();

      try {

         Muctpi::EntityResolver resolver;
         lutParser->setEntityResolver( &resolver );
         Muctpi::ErrorHandler handler;
         lutParser->setErrorHandler( &handler );

         lutParser->parse( lutXMLFile.c_str() );

      } catch( const xercesc::XMLException& ex ) {
         char* error_message = xercesc::XMLString::transcode( ex.getMessage() );
         REPORT_ERROR_MSG( "XMLException received while parsing the LUT XML file." );
         REPORT_ERROR_MSG( "Message: " << error_message );
         xercesc::XMLString::release( &error_message );
         return StatusCode::RECOVERABLE;
      } catch( const xercesc::DOMException& ex ) {
         char* error_message = xercesc::XMLString::transcode( ex.getMessage() );
         REPORT_ERROR_MSG( "DOMException received while parsing the LUT XML file." );
         REPORT_ERROR_MSG( "Message: " << error_message );
         xercesc::XMLString::release( &error_message );
         return StatusCode::RECOVERABLE;
      } catch(...) {
         REPORT_ERROR_MSG( "LUT XML file parsing failed with unknown exception" );
         return StatusCode::RECOVERABLE;
      }

      const xercesc::DOMDocument* lutDocument = lutParser->getDocument();
      const xercesc::DOMNodeList* lutNodes = lutDocument->getChildNodes();
      REPORT_VERBOSE_MSG( "Number of nodes in the LUT XML file: "
                          << lutNodes->getLength() );

      const xercesc::DOMNode* lutNode = 0;
      for( XMLSize_t i = 0; i < lutNodes->getLength(); ++i ) {
         const xercesc::DOMNode* node = lutNodes->item( i );
         char* cnode_name = xercesc::XMLString::transcode( node->getNodeName() );
         std::string snode_name( cnode_name );
         xercesc::XMLString::release( &cnode_name );
         REPORT_VERBOSE_MSG( "Investigating node with name: " << snode_name );
         if( ( snode_name == "MUCTPI_LUT" ) &&
             ( node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE ) ) {
            lutNode = node;
            break;
         }
      }
      if( ! lutNode ) {
         REPORT_ERROR_MSG( "Couldn't find 'MUCTPI_LUT' node in the LUT XML!" );
         return StatusCode::RECOVERABLE;
      }

      const xercesc::DOMNodeList* mioctNodes = lutNode->getChildNodes();
      for( XMLSize_t i = 0; i < mioctNodes->getLength(); ++i ) {

         const xercesc::DOMNode* mioctNode = mioctNodes->item( i );
         char* cname = xercesc::XMLString::transcode( mioctNode->getNodeName() );
         std::string sname( cname );
         xercesc::XMLString::release( &cname );

         if( sname != "Mioct" ) continue;

         const xercesc::DOMElement* mioctElement =
            dynamic_cast< const xercesc::DOMElement* >( mioctNode );
         if( ! mioctElement ) {
            REPORT_ERROR_MSG( "Unable to cast Mioct node into DOMElement" );
            return StatusCode::RECOVERABLE;
         }

         REPORT_VERBOSE_MSG( ">>>>>>>>>>>>>>>> Starting to initialize calculator <<<<<<<<<<<<<<<<" );
         LUTMultiplicityCalculator calc( m_multiplicityLogic );
         CHECK( calc.initialize( mioctNode, flagMode, dumpLut, runPeriod ) );
         REPORT_VERBOSE_MSG( ">>>>>>>>>>>>>>>> Calculator initialization finished <<<<<<<<<<<<<<<<" );

         // Check if this calculator is not defined yet:
         std::list< LUTMultiplicityCalculator >::const_iterator it;
         if( ( it = std::find( m_calculator_list.begin(), m_calculator_list.end(), calc ) ) ==
             m_calculator_list.end() ) {
            m_calculator_list.push_back( calc );
            it = std::find( m_calculator_list.begin(), m_calculator_list.end(), calc );
            REPORT_VERBOSE_MSG( "This is a new calculator definition" );
         } else {
            REPORT_VERBOSE_MSG( "This calculator definition already exists" );
         }

         //
         // Extract the Mioct IDs that this calculator represents:
         //
         XMLCh* mioct_ids_name = xercesc::XMLString::transcode( "IDs" );
         const XMLCh* mioct_ids = mioctElement->getAttribute( mioct_ids_name );
         xercesc::XMLString::release( &mioct_ids_name );

         char* cids = xercesc::XMLString::transcode( mioct_ids );
         std::string sids( cids );
         xercesc::XMLString::release( &cids );

         std::vector< unsigned int > ids;
         std::istringstream stream_ids( sids );
         while( ! stream_ids.eof() ) {
            unsigned int id;
            stream_ids >> id;
            ids.push_back( id );
         }

         //
         // Assign the correct Miocts to this calculator:
         //
         REPORT_VERBOSE_MSG( "This calculator represents Miocts:" );
         for( std::vector< unsigned int >::const_iterator id = ids.begin(); id != ids.end();
              ++id ) {
            if( ( *id ) < 8 ) {
               MioctID mid( ( *id ), NEGATIVE );
               m_calculator_map[ mid ] = &*it;
               REPORT_VERBOSE_MSG( "  --> Mioct " << ( *id )
                                   << " in the NEGATIVE rapidity region" );
            } else {
               MioctID mid( ( *id ) - 8, POSITIVE );
               m_calculator_map[ mid ] = &*it;
               REPORT_VERBOSE_MSG( "  --> Mioct " << ( ( *id ) - 8 )
                                   << " in the POSITIVE rapidity region" );
            }
         }
      }

      REPORT_VERBOSE_MSG( "Number of different calculators created: "
                          << m_calculator_list.size() );

      delete lutParser;

      //
      // Terminate XercesC:
      //
      xercesc::XMLPlatformUtils::Terminate();

      return StatusCode::SUCCESS;
   }

   PtMultiplicitySet LUTStrategy::calculateMultiplicity( const MioctSectorSet& sectorData,
                                                         const MioctID& mioct ) const {

      if( m_calculator_map.find( mioct ) != m_calculator_map.end() ) {
         return m_calculator_map.find( mioct )->second->calculateMultiplicity( sectorData );
      }

      REPORT_ERROR_MSG( "Multiplicity calculation asked for unknown Mioct!" );
      PtMultiplicitySet result;
      return result;
   }

} // namespace LVL1MUCTPI
