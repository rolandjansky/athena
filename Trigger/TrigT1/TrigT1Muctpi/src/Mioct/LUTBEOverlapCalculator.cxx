/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTBEOverlapCalculator.cxx 650693 2015-03-01 16:53:48Z masato $

// STL include(s):
#include <sstream>
#include <iomanip>

// XercesC include(s):
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLString.hpp>

// Local include(s):
#include "LUTBEOverlapCalculator.h"
#include "BarrelSector.h"
#include "EndcapSector.h"
#include "../Common/PtMultiplicitySet.h"

namespace LVL1MUCTPI {

   LUTBEOverlapCalculator::LUTBEOverlapCalculator()
      : LUTOverlapCalculatorBase( "LUTBEOverlapCalculator" ) {

   }

   LUTBEOverlapCalculator::LUTBEOverlapCalculator( const LUTBEOverlapCalculator& calc )
      : LUTOverlapCalculatorBase( calc ) {

   }

  StatusCode LUTBEOverlapCalculator::initialize( const xercesc::DOMNode* benode, bool dumpLut, const std::string& runPeriod ) {

     if( runPeriod == "RUN2" ){
       setMaximumThreshold( 3 );
       REPORT_VERBOSE_MSG( "RunPeriod=" << runPeriod << " setMaximumThreshold to 3" );
     }

      // Clear the internal LUT:
      m_lut.clear();

      //
      // Check if the received node is of the right type:
      //
      if( benode->getNodeType() != xercesc::DOMNode::ELEMENT_NODE ) {
         REPORT_ERROR_MSG( "Received node is not a DOMElement" );
         return StatusCode::RECOVERABLE;
      }

      REPORT_VERBOSE_MSG( "Looping over \"BEElement\"-s:" );

      //
      // Initialize the B-E LUT:
      //
      const xercesc::DOMNodeList* lut_lines = benode->getChildNodes();
      for( XMLSize_t i = 0; i < lut_lines->getLength(); ++i ) {

         const xercesc::DOMNode* lut_line = lut_lines->item( i );
         char* cname = xercesc::XMLString::transcode( lut_line->getNodeName() );
         std::string sname( cname );
         xercesc::XMLString::release( &cname );

         if( sname != "BEElement" ) continue;

         const xercesc::DOMElement* lut_element =
            dynamic_cast< const xercesc::DOMElement* >( lut_line );
         if( ! lut_element ) {
            REPORT_ERROR_MSG( "Unable to cast LUT node into DOMElement" );
            return StatusCode::RECOVERABLE;
         }

         XMLCh* broi_name = xercesc::XMLString::transcode( "BRoI" );
         XMLCh* bpt_name  = xercesc::XMLString::transcode( "BPt" );
         XMLCh* eroi_name = xercesc::XMLString::transcode( "ERoI" );
         XMLCh* q_name    = xercesc::XMLString::transcode( "EChrg" );

         const XMLCh* broi = lut_element->getAttribute( broi_name );
         const XMLCh* bpt  = lut_element->getAttribute( bpt_name );
         const XMLCh* eroi = lut_element->getAttribute( eroi_name );
         const XMLCh* q    = lut_element->getAttribute( q_name );

         xercesc::XMLString::release( &broi_name );
         xercesc::XMLString::release( &bpt_name );
         xercesc::XMLString::release( &eroi_name );
         xercesc::XMLString::release( &q_name );

         const unsigned int broi_n = xercesc::XMLString::parseInt( broi );
         const unsigned int bpt_n  = xercesc::XMLString::parseInt( bpt );
         const unsigned int eroi_n = xercesc::XMLString::parseInt( eroi );
         const unsigned int q_n    = xercesc::XMLString::parseInt( q );

         //
         // Create the word to be put into the LUT:
         //
         const unsigned int element = ( broi_n & 0x1f ) | ( ( bpt_n & 0x7 ) << 5 ) |
            ( ( eroi_n & 0xff ) << 8 ) | ( ( q_n & 0x1 ) << 16 );

         m_lut[ element ] = true;

         if( dumpLut ) {
            REPORT_VERBOSE_MSG( "  --> BRoI: " << broi_n << " BPt: " << bpt_n
                                << " ERoI: " << eroi_n << " EChrg: " << q_n << " Element: 0x"
                                << std::hex << std::setw( 5 ) << std::setfill( '0' ) << element
                                << std::dec );
         }
      }

      REPORT_VERBOSE_MSG( "Number of LUT elements: " << m_lut.size() );

      return StatusCode::SUCCESS;
   }

   void LUTBEOverlapCalculator::calculate( const BarrelSector& bsector,
                                           const EndcapSector& esector ) const {

      //
      // Calculate some helper quantities:
      //
      bool bseccand1present = false, bseccand2present = false;
      bool eseccand1present = false, eseccand2present = false;
      int bseccand1pt = -1, bseccand2pt = -1;
      int eseccand1pt = -1, eseccand2pt = -1;
      if( bsector.getPtCand1() != 7 ) {
         bseccand1present = true;
         bseccand1pt = bsector.getPtCand1();
      }
      if( bsector.getPtCand2() != 7 ) {
         bseccand2present = true;
         bseccand2pt = bsector.getPtCand2();
      }
      if( esector.getPtCand1() != 7 ) {
         eseccand1present = true;
         eseccand1pt = esector.getPtCand1();
      }
      if( esector.getPtCand2() != 7 ) {
         eseccand2present = true;
         eseccand2pt = esector.getPtCand2();
      }

      //
      // Find if a rule exists for bsec cand1 and esec cand1:
      //
      unsigned int element1 = ( bsector.getROI1() & 0x1f ) | ( ( bsector.getPtCand1() & 0x7 ) << 5 ) |
         ( ( esector.getROI1() & 0xff ) << 8 ) | 0x10000;
      unsigned int element2 = ( bsector.getROI1() & 0x1f ) | ( ( bsector.getPtCand1() & 0x7 ) << 5 ) |
         ( ( esector.getROI1() & 0xff ) << 8 );
      if( bseccand1present && eseccand1present &&
          ( bseccand1pt <= m_thr ) && ( eseccand1pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it1, it2;
         if( ( ( ( it1 = m_lut.find( element1 ) ) != m_lut.end() ) && ( it1->second ) ) ||
             ( ( ( it2 = m_lut.find( element2 ) ) != m_lut.end() ) && ( it2->second ) ) ) {
            if( bseccand1pt < eseccand1pt ) {
               bsector.setCand1Supressed( true );
#ifndef NDEBUG
               REPORT_VERBOSE_MSG( "Candidate 1 in Barrel sector " << bsector.getSectorNumber()
                                   << " in the " << bsector.getRapidityString()
                                   << " rapidity region supressed" );
#endif // NDEBUG
            } else {
               esector.setCand1Supressed( true );
#ifndef NDEBUG
               REPORT_VERBOSE_MSG( "Candidate 1 in Endcap sector " << esector.getSectorNumber()
                                   << " in the " << esector.getRapidityString()
                                   << " rapidity region supressed" );
#endif // NDEBUG
            }
         }
      }

      //
      // Find if a rule exists for bsec cand2 and esec cand1:
      //
      element1 = ( bsector.getROI2() & 0x1f ) | ( ( bsector.getPtCand2() & 0x7 ) << 5 ) |
         ( ( esector.getROI1() & 0xff ) << 8 ) | 0x10000;
      element2 = ( bsector.getROI2() & 0x1f ) | ( ( bsector.getPtCand2() & 0x7 ) << 5 ) |
         ( ( esector.getROI1() & 0xff ) << 8 );
      if( bseccand2present && eseccand1present &&
          ( bseccand2pt <= m_thr ) && ( eseccand1pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it1, it2;
         if( ( ( ( it1 = m_lut.find( element1 ) ) != m_lut.end() ) && ( it1->second ) ) ||
             ( ( ( it2 = m_lut.find( element2 ) ) != m_lut.end() ) && ( it2->second ) ) ) {
            if( bseccand2pt < eseccand1pt ) {
               bsector.setCand2Supressed( true );
#ifndef NDEBUG
               REPORT_VERBOSE_MSG( "Candidate 2 in Barrel sector " << bsector.getSectorNumber()
                                   << " in the " << bsector.getRapidityString()
                                   << " rapidity region supressed" );
#endif // NDEBUG
            } else {
               esector.setCand1Supressed( true );
#ifndef NDEBUG
               REPORT_VERBOSE_MSG( "Candidate 1 in Endcap sector " << esector.getSectorNumber()
                                   << " in the " << esector.getRapidityString()
                                   << " rapidity region supressed" );
#endif // NDEBUG
            }
         }
      }

      //
      // Find if a rule exists for bsec cand1 and esec cand2:
      //
      element1 = ( bsector.getROI1() & 0x1f ) | ( ( bsector.getPtCand1() & 0x7 ) << 5 ) |
         ( ( esector.getROI2() & 0xff ) << 8 ) | 0x10000;
      element2 = ( bsector.getROI1() & 0x1f ) | ( ( bsector.getPtCand1() & 0x7 ) << 5 ) |
         ( ( esector.getROI2() & 0xff ) << 8 );
      if( bseccand1present && eseccand2present &&
          ( bseccand1pt <= m_thr ) && ( eseccand2pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it1, it2;
         if( ( ( ( it1 = m_lut.find( element1 ) ) != m_lut.end() ) && ( it1->second ) ) ||
             ( ( ( it2 = m_lut.find( element2 ) ) != m_lut.end() ) && ( it2->second ) ) ) {
            if( bseccand1pt < eseccand2pt ) {
               bsector.setCand1Supressed( true );
#ifndef NDEBUG
               REPORT_VERBOSE_MSG( "Candidate 1 in Barrel sector " << bsector.getSectorNumber()
                                   << " in the " << bsector.getRapidityString()
                                   << " rapidity region supressed" );
#endif // NDEBUG
            } else {
               esector.setCand2Supressed( true );
#ifndef NDEBUG
               REPORT_VERBOSE_MSG( "Candidate 2 in Endcap sector " << esector.getSectorNumber()
                                   << " in the " << esector.getRapidityString()
                                   << " rapidity region supressed" );
#endif // NDEBUG
            }
         }
      }

      //
      // Find if a rule exists for bsec cand2 and esec cand2:
      //
      element1 = ( bsector.getROI2() & 0x1f ) | ( ( bsector.getPtCand2() & 0x7 ) << 5 ) |
         ( ( esector.getROI2() & 0xff ) << 8 ) | 0x10000;
      element2 = ( bsector.getROI2() & 0x1f ) | ( ( bsector.getPtCand2() & 0x7 ) << 5 ) |
         ( ( esector.getROI2() & 0xff ) << 8 );
      if( bseccand2present && eseccand2present &&
          ( bseccand2pt <= m_thr ) && ( eseccand2pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it1, it2;
         if( ( ( ( it1 = m_lut.find( element1 ) ) != m_lut.end() ) && ( it1->second ) ) ||
             ( ( ( it2 = m_lut.find( element2 ) ) != m_lut.end() ) && ( it2->second ) ) ) {
            if( bseccand2pt < eseccand2pt ) {
               bsector.setCand2Supressed( true );
#ifndef NDEBUG
               REPORT_VERBOSE_MSG( "Candidate 2 in Barrel sector " << bsector.getSectorNumber()
                                   << " in the " << bsector.getRapidityString()
                                   << " rapidity region supressed" );
#endif // NDEBUG
            } else {
               esector.setCand2Supressed( true );
#ifndef NDEBUG
               REPORT_VERBOSE_MSG( "Candidate 2 in Endcap sector " << esector.getSectorNumber()
                                   << " in the " << esector.getRapidityString()
                                   << " rapidity region supressed" );
#endif // NDEBUG
            }
         }
      }

      return;
   }

} // namespace LVL1MUCTPI
