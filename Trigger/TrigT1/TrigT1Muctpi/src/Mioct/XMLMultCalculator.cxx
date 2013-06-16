/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: XMLMultCalculator.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// STL include(s):
#include <string>
#include <sstream>

// XercesC include(s):
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLString.hpp>

// Local include(s):
#include "XMLMultCalculator.h"
#include "../Common/SectorConstants.h"

using namespace std;
using namespace xercesc;

namespace LVL1MUCTPI {

   bool operator== ( const XMLMultCalculator& calc1,
                     const XMLMultCalculator& calc2 ) {

      return ( calc1.m_lut == calc2.m_lut );

   }

   XMLMultCalculator::XMLMultCalculator( const xercesc::DOMNode* node )
      : m_lut( MAX_NUMBER_OF_THRESHOLDS,
               vector< bool >( MAX_NUMBER_OF_THRESHOLDS, false ) ),
        m_logger( "XMLMultCalculator" ) {

      if( node ) initialize( node );

   }

   XMLMultCalculator::XMLMultCalculator( const XMLMultCalculator& parent )
      : m_lut( parent.m_lut ), m_logger( "XMLMultCalculator" ) {

   }

   XMLMultCalculator::~XMLMultCalculator() {

   }

   void XMLMultCalculator::initialize( const xercesc::DOMNode* node ) {

      //
      // Re-initialise the LUT:
      //
      m_lut.clear();
      m_lut.insert( m_lut.begin(), MAX_NUMBER_OF_THRESHOLDS,
                    vector< bool >( MAX_NUMBER_OF_THRESHOLDS, false ) );

      if( node->getNodeType() != DOMNode::ELEMENT_NODE ) {
         m_logger.send( ERROR, "initialize() : Received node is not a DOMElement" );
         return;
      }

      const DOMElement* top_element = dynamic_cast< const DOMElement* >( node );
      if( ! top_element ) {
         m_logger.send( ERROR, "initialize() : dynamic cast to DOMElement failed" );
         return;
      }

      const DOMNodeList* lut_list = top_element->getChildNodes();

      m_logger.send( VERBOSE, "initialize() : Looping over thresholds" );

      for( XMLSize_t i = 0; i < lut_list->getLength(); ++i ) {

         const DOMNode* lut_node = lut_list->item( i );
         char* cname = XMLString::transcode( lut_node->getNodeName() );
         string sname( cname );
         XMLString::release( &cname );

         if( sname != "Threshold" ) continue;

         const DOMElement* lut_element = dynamic_cast< const DOMElement* >( lut_node );
         if( ! lut_element ) {
            m_logger.send( ERROR, "initialize() : Unable to cast LUT node into DOMElement" );
            return;
         }

         XMLCh*   id_name = XMLString::transcode( "ID" );
         XMLCh* thr1_name = XMLString::transcode( "thr1" );
         XMLCh* thr2_name = XMLString::transcode( "thr2" );
         XMLCh* thr3_name = XMLString::transcode( "thr3" );
         XMLCh* thr4_name = XMLString::transcode( "thr4" );
         XMLCh* thr5_name = XMLString::transcode( "thr5" );
         XMLCh* thr6_name = XMLString::transcode( "thr6" );

         const XMLCh* id   = lut_element->getAttribute( id_name );
         const XMLCh* thr1 = lut_element->getAttribute( thr1_name );
         const XMLCh* thr2 = lut_element->getAttribute( thr2_name );
         const XMLCh* thr3 = lut_element->getAttribute( thr3_name );
         const XMLCh* thr4 = lut_element->getAttribute( thr4_name );
         const XMLCh* thr5 = lut_element->getAttribute( thr5_name );
         const XMLCh* thr6 = lut_element->getAttribute( thr6_name );

         XMLString::release( &id_name );
         XMLString::release( &thr1_name );
         XMLString::release( &thr2_name );
         XMLString::release( &thr3_name );
         XMLString::release( &thr4_name );
         XMLString::release( &thr5_name );
         XMLString::release( &thr6_name );

         const int   id_n = XMLString::parseInt( id );
         const int thr1_n = XMLString::parseInt( thr1 );
         const int thr2_n = XMLString::parseInt( thr2 );
         const int thr3_n = XMLString::parseInt( thr3 );
         const int thr4_n = XMLString::parseInt( thr4 );
         const int thr5_n = XMLString::parseInt( thr5 );
         const int thr6_n = XMLString::parseInt( thr6 );

         m_lut[ id_n - 1 ][ 0 ] = ( thr1_n != 0 ) ? true : false;
         m_lut[ id_n - 1 ][ 1 ] = ( thr2_n != 0 ) ? true : false;
         m_lut[ id_n - 1 ][ 2 ] = ( thr3_n != 0 ) ? true : false;
         m_lut[ id_n - 1 ][ 3 ] = ( thr4_n != 0 ) ? true : false;
         m_lut[ id_n - 1 ][ 4 ] = ( thr5_n != 0 ) ? true : false;
         m_lut[ id_n - 1 ][ 5 ] = ( thr6_n != 0 ) ? true : false;

         ostringstream message;
         message << "  threshold " << id_n << ": th1=" << m_lut[ id_n - 1 ][ 0 ]
                 << "; th2=" << m_lut[ id_n - 1 ][ 1 ] << "; th3=" << m_lut[ id_n - 1 ][ 2 ]
                 << "; th4=" << m_lut[ id_n - 1 ][ 3 ] << "; th5=" << m_lut[ id_n - 1 ][ 4 ]
                 << "; th6=" << m_lut[ id_n - 1 ][ 5 ];
         m_logger.send( VERBOSE, message.str() );

      }

      return;

   }

   PtMultiplicitySet
   XMLMultCalculator::calculateMultiplicity( unsigned int pt ) const {

      PtMultiplicitySet result;

      if( ( pt >= 1 ) && ( pt <= MAX_NUMBER_OF_THRESHOLDS ) ) {
         for( unsigned int i = 0; i < MAX_NUMBER_OF_THRESHOLDS; ++i ) {
            if( m_lut[ pt - 1 ][ i ] ) result.addMuon( i + 1 );
         }
      }

      return result;

   }

} // LVL1MUCTPI
