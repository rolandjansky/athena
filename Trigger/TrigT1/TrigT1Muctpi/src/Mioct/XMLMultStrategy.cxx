/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: XMLMultStrategy.cxx 472835 2011-12-06 21:05:16Z stelzer $

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

// Local include(s):
#include "XMLMultStrategy.h"

using namespace std;
using namespace xercesc;

namespace LVL1MUCTPI {

   bool operator< ( const std::pair< SectorID, bool >& id1,
                    const std::pair< SectorID, bool >& id2 ) {

      if( id1.second != id2.second ) {
         return ( id1.second < id2.second );
      } else {
         return ( id1.first < id2.first );
      }

   }

   XMLMultStrategy::XMLMultStrategy()
      : MultiplicityStrategy( "XML-Based-Multiplicity-Strategy" ),
        m_exStrategy(), m_calculatorMap(), m_calculatorList(),
        m_logger( "XMLMultStrategy" ) {

   }

   XMLMultStrategy::XMLMultStrategy( const std::string& xmlMultFile )
      : MultiplicityStrategy( "XML-Based-Multiplicity-Strategy" ),
        m_exStrategy(), m_calculatorMap(), m_calculatorList(),
        m_logger( "XMLMultStrategy" ) {

      initialize( xmlMultFile );

   }

   XMLMultStrategy::XMLMultStrategy( const XMLMultStrategy& strategy )
      : MultiplicityStrategy( "XML-Based-Multiplicity-Strategy" ),
        m_logger( "XMLMultStrategy" ) {

      // Replicate the list:
      for( std::list< XMLMultCalculator >::const_iterator it =
              strategy.m_calculatorList.begin();
           it != strategy.m_calculatorList.end(); ++it ) {
         m_calculatorList.push_back( *it );
      }

      // Replicate the map:
      for( std::map< std::pair< SectorID, bool >,
              const XMLMultCalculator* >::const_iterator it =
              strategy.m_calculatorMap.begin();
           it != strategy.m_calculatorMap.end(); ++it ) {

         for( std::list< XMLMultCalculator >::const_iterator calc =
                 m_calculatorList.begin();
              calc != m_calculatorList.end(); ++calc ) {
            if( ( *it->second ) == ( *calc ) ) {
               m_calculatorMap[ it->first ] = &( *calc );
               break;
            }
         }

      }

   }

   XMLMultStrategy::~XMLMultStrategy() {

   }

   void XMLMultStrategy::initialize( const std::string& xmlMultFile ) {

      //
      // Reset the object:
      //
      m_calculatorList.clear();
      m_calculatorMap.clear();

      REPORT_MSG(DEBUG, "initialize() : Reading configuration from file: " << xmlMultFile );
      //
      // Initialize XercesC:
      //
      try {

         XMLPlatformUtils::Initialize();

      } catch( const XMLException& ex ) {

         ostringstream message;
         char* error_message = XMLString::transcode( ex.getMessage() );
         message << "initialize() : Failed to initialize XercesC because: "
                 << error_message;
         XMLString::release( &error_message );
         m_logger.send( ERROR, message.str() );
         return;

      }

      //
      // Parse the LUT XML file:
      //
      XercesDOMParser* xmlParser = new XercesDOMParser();

      try {

         xmlParser->parse( xmlMultFile.c_str() );

      } catch( const XMLException& ex ) {

         ostringstream message;
         message << "initialize() : XMLException received while parsing the XML file."
                 << endl;
         message << "initialize() : Message: " << XMLString::transcode( ex.getMessage() );
         m_logger.send( ERROR, message.str() );
         return;

      } catch( const DOMException& ex ) {

         ostringstream message;
         message << "initialize() : DOMException received while parsing the XML file."
                 << endl;
         message << "initialize() : Message: " << XMLString::transcode( ex.getMessage() );
         m_logger.send( ERROR, message.str() );
         return;

      } catch(...) {

         m_logger.send( ERROR,
                        "initialize() : XML file parsing failed with unknown exception" );
         return;

      }

      const DOMDocument* xmlDocument = xmlParser->getDocument();

      {
         ostringstream message;
         message << "initialize() : Document has "
                 << xmlDocument->getChildNodes()->getLength() << " child nodes" << endl;
         message << "initialize() : Their names are:";
         m_logger.send( VERBOSE, message.str() );
         for( XMLSize_t i = 0; i < xmlDocument->getChildNodes()->getLength(); ++i ) {
            const XMLCh* name = xmlDocument->getChildNodes()->item( i )->getNodeName();
            char* cname = XMLString::transcode( name );
            ostringstream message;
            message << "initialize() :   --> " << cname;
            m_logger.send( VERBOSE, message.str() );
            XMLString::release( &cname );
         }
      }

      const DOMNode* sectorsNode = xmlDocument->getFirstChild();
      const DOMNodeList* sectorNodes = sectorsNode->getChildNodes();

      for( XMLSize_t i = 0; i < sectorNodes->getLength(); ++i ) {

         const DOMNode* sectorNode = sectorNodes->item( i );

         char* cname = XMLString::transcode( sectorNode->getNodeName() );
         string sname( cname );
         XMLString::release( &cname );

         if( sname != "Sector" ) continue;

         const DOMElement* sectorElement = dynamic_cast< const DOMElement* >( sectorNode );
         if( ! sectorElement ) {
            m_logger.send( ERROR,
                           "initialize() : Unable to cast Sector node into DOMElement" );
            return;
         }

         m_logger.send( VERBOSE,
                        ">>>>>>>>>>>>>> Starting to initialize calculator <<<<<<<<<<<<<<" );
         XMLMultCalculator calc( sectorNode );
         m_logger.send( VERBOSE,
                        ">>>>>>>>>>>>>> Calculator initialization finished <<<<<<<<<<<<<<" );

         // Check if this calculator is not defined yet:
         list< XMLMultCalculator >::const_iterator calc_it;
         if( ( calc_it = find( m_calculatorList.begin(), m_calculatorList.end(),
                               calc ) ) == m_calculatorList.end() ) {
            m_calculatorList.push_back( calc );
            calc_it = find( m_calculatorList.begin(), m_calculatorList.end(), calc );
            m_logger.send( VERBOSE, "initialize() : This is a new calculator definition" );
         } else {
            m_logger.send( VERBOSE,
                           "initialize() : This calculator definition already exists" );
         }

         //
         // Extract the Sector IDs that this calculator represents:
         //
         XMLCh* sector_ids_name = XMLString::transcode( "IDs" );
         const XMLCh* sector_ids = sectorElement->getAttribute( sector_ids_name );
         XMLString::release( &sector_ids_name );

         char* cids = XMLString::transcode( sector_ids );
         string sids( cids );
         XMLString::release( &cids );

         vector< int > ids;
         istringstream stream_ids( sids );
         while( ! stream_ids.eof() ) {
            int id;
            stream_ids >> id;
            ids.push_back( id );
         }

         //
         // Assing the correct sectors to this calculator:
         //
         const int candidate_border = ( ( MAX_BARREL_SECTOR + 1 ) +
                                        ( MAX_ENDCAP_SECTOR + 1 ) +
                                        ( MAX_FORWARD_SECTOR + 1 ) ) * 2;
         m_logger.send( VERBOSE, "initialize() : This calculator represents sectors:" );
         for( vector< int >::const_iterator id = ids.begin(); id != ids.end(); ++id ) {
            bool firstCand = ( *id >= candidate_border ) ? false : true;
            int lid = ( *id >= candidate_border ) ? ( *id - candidate_border ) : ( *id );
            SectorID sid( lid );
            m_calculatorMap[ std::make_pair( sid, firstCand ) ] = &*calc_it;

            ostringstream message;
            message << "initialize() :  --> " << sid << ( firstCand ? " fist candidate" :
                                                          " second candidate" );
            m_logger.send( VERBOSE, message.str() );
         }

      }

      ostringstream message;
      message << "Number of different calculators created: " << m_calculatorList.size();
      m_logger.send( VERBOSE, message.str() );

      return;

   }

   PtMultiplicitySet
   XMLMultStrategy::calculateMultiplicity( unsigned int pt,
                                           const SectorID& id,
                                           bool firstCandidate ) const {

      PtMultiplicitySet result;

      if( m_calculatorMap.find( std::make_pair( id, firstCandidate ) ) !=
          m_calculatorMap.end() ) {
         result = m_calculatorMap.find( std::make_pair( id, firstCandidate ) )->second->calculateMultiplicity( pt );
      } else {
         m_logger.send( ERROR, "Multiplicity calculation asked for unknown sector!" );
         result = m_exStrategy.calculateMultiplicity( pt, id, firstCandidate );
      }

      return result;

   }

} // namespace LVL1MUCTPI
