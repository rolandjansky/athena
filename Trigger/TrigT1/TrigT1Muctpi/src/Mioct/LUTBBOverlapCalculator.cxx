/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTBBOverlapCalculator.cxx 650693 2015-03-01 16:53:48Z masato $

// STL include(s):
#include <string>
#include <iomanip>

// XercesC include(s):
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLString.hpp>

// Local include(s):
#include "LUTBBOverlapCalculator.h"
#include "BarrelSector.h"
#include "../Common/PtMultiplicitySet.h"

namespace LVL1MUCTPI {

   LUTBBOverlapCalculator::LUTBBOverlapCalculator()
      : LUTOverlapCalculatorBase( "LUTBBOverlapCalculator" ) {

   }

   LUTBBOverlapCalculator::LUTBBOverlapCalculator( const LUTBBOverlapCalculator& calc )
      : LUTOverlapCalculatorBase( calc ) {

   }

   /**
    * @param node One of the BB nodes of the XML file
    * @param dumpLut Print the LUT while reading it
    */
  StatusCode LUTBBOverlapCalculator::initialize( const xercesc::DOMNode* bbnode, bool dumpLut,const std::string& runPeriod ) {

     if( runPeriod == "RUN2" ){
       setMaximumThreshold( 6 );
       REPORT_VERBOSE_MSG( "RunPeriod=" << runPeriod << " setMaximumThreshold to 6" );
     }

      // Clear the internal LUT:
      m_lut.clear();

      //
      // Check if the received node is of the right type:
      //
      if( bbnode->getNodeType() != xercesc::DOMNode::ELEMENT_NODE ) {
         REPORT_ERROR_MSG( "Received node is not a DOMElement" );
         return StatusCode::RECOVERABLE;
      }

      REPORT_VERBOSE_MSG( "Looping over \"BBElement\"-s:" );

      const xercesc::DOMNodeList* lut_def_list = bbnode->getChildNodes();
      for( XMLSize_t i = 0; i < lut_def_list->getLength(); ++i ) {

         const xercesc::DOMNode* lut_node = lut_def_list->item( i );
         char* cname = xercesc::XMLString::transcode( lut_node->getNodeName() );
         std::string sname( cname );
         xercesc::XMLString::release( &cname );

         if( sname != "BBElement" ) continue;

         const xercesc::DOMElement* lut_element =
            dynamic_cast< const xercesc::DOMElement* >( lut_node );
         if( ! lut_element ) {
            REPORT_ERROR_MSG( "Unable to cast LUT node into DOMElement" );
            return StatusCode::RECOVERABLE;
         }

         XMLCh* roi1_name  = xercesc::XMLString::transcode( "RoI1" );
         XMLCh* roi2_name  = xercesc::XMLString::transcode( "RoI2" );
         XMLCh* ovlp1_name = xercesc::XMLString::transcode( "Ovlp1" );
         XMLCh* ovlp2_name = xercesc::XMLString::transcode( "Ovlp2" );

         const XMLCh* roi1  = lut_element->getAttribute( roi1_name );
         const XMLCh* roi2  = lut_element->getAttribute( roi2_name );
         const XMLCh* ovlp1 = lut_element->getAttribute( ovlp1_name );
         const XMLCh* ovlp2 = lut_element->getAttribute( ovlp2_name );

         xercesc::XMLString::release( &roi1_name );
         xercesc::XMLString::release( &roi2_name );
         xercesc::XMLString::release( &ovlp1_name );
         xercesc::XMLString::release( &ovlp2_name );

         const int roi1_n   = xercesc::XMLString::parseInt( roi1 );
         const int roi2_n   = xercesc::XMLString::parseInt( roi2 );
         const bool ovlp1_b = xercesc::XMLString::parseInt( ovlp1 );
         const bool ovlp2_b = xercesc::XMLString::parseInt( ovlp2 );

         //
         // Create the word to be put into the LUT:
         //
         const unsigned int element = ( roi1_n & 0x1f ) | ( ovlp1_b ? 0x20 : 0x0 ) |
            ( ( roi2_n & 0x1f ) << 6 ) | ( ovlp2_b ? 0x800 : 0x0 );

         m_lut[ element ] = true;

         if( dumpLut ) {
            REPORT_VERBOSE_MSG( "  --> RoI1: " << roi1_n << " RoI2: " << roi2_n << " Ovlp1: "
                                << ovlp1_b << " Ovlp2: " << ovlp2_b << " Element: 0x"
                                << std::hex << std::setw( 3 ) << std::setfill( '0' ) << element << std::dec );
         }
      }

      REPORT_VERBOSE_MSG( "Number of LUT elements: " << m_lut.size() );

      return StatusCode::SUCCESS;
   }

   void LUTBBOverlapCalculator::calculate( const BarrelSector& sector1,
                                           const BarrelSector& sector2 ) const {

      //
      // Calculate some helper quantities:
      //
      bool sec1cand1present = false, sec1cand2present = false;
      bool sec2cand1present = false, sec2cand2present = false;
      int sec1cand1pt = -1, sec1cand2pt = -1;
      int sec2cand1pt = -1, sec2cand2pt = -1;
      if( sector1.getPtCand1() != 7 ) {
         sec1cand1present = true;
         sec1cand1pt = sector1.getPtCand1();
      }
      if( sector1.getPtCand2() != 7 ) {
         sec1cand2present = true;
         sec1cand2pt = sector1.getPtCand2();
      }
      if( sector2.getPtCand1() != 7 ) {
         sec2cand1present = true;
         sec2cand1pt = sector2.getPtCand1();
      }
      if( sector2.getPtCand2() != 7 ) {
         sec2cand2present = true;
         sec2cand2pt = sector2.getPtCand2();
      }

      //
      // Find if a rule exists for sec1 cand1 and sec2 cand1:
      //
      unsigned int element = ( sector1.getROI1() & 0x1f ) |
         ( sector1.isCand1BarrelOverlap() ? 0x20 : 0x0 ) |
         ( ( sector2.getROI1() & 0x1f ) << 6 ) |
         ( sector2.isCand1BarrelOverlap() ? 0x800 : 0x0 );
      if( sec1cand1present && sec2cand1present &&
          ( sec1cand1pt <= m_thr ) && ( sec2cand1pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it;
         if( ( it = m_lut.find( element ) ) != m_lut.end() ) {
            if( it->second ) {
               if( sec1cand1pt < sec2cand1pt ) {
                  sector1.setCand1Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 1 in Barrel sector "
                                      << sector1.getSectorNumber() << " in the "
                                      << sector1.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               } else {
                  sector2.setCand1Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 1 in Barrel sector "
                                      << sector2.getSectorNumber() << " in the "
                                      << sector2.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               }
            }
         }
      }

      //
      // Find if a rule exists for sec1 cand2 and sec2 cand1:
      //
      element = ( sector1.getROI2() & 0x1f ) |
         ( sector1.isCand2BarrelOverlap() ? 0x20 : 0x0 ) |
         ( ( sector2.getROI1() & 0x1f ) << 6 ) |
         ( sector2.isCand1BarrelOverlap() ? 0x800 : 0x0 );
      if( sec1cand2present && sec2cand1present &&
          ( sec1cand2pt <= m_thr ) && ( sec2cand1pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it;
         if( ( it = m_lut.find( element ) ) != m_lut.end() ) {
            if( it->second ) {
               if( sec1cand2pt < sec2cand1pt ) {
                  sector1.setCand2Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 2 in Barrel sector "
                                      << sector1.getSectorNumber() << " in the "
                                      << sector1.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               } else {
                  sector2.setCand1Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 1 in Barrel sector "
                                      << sector2.getSectorNumber() << " in the "
                                      << sector2.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               }
            }
         }
      }

      //
      // Find if a rule exists for sec1 cand1 and sec2 cand2:
      //
      element = ( sector1.getROI1() & 0x1f ) |
         ( sector1.isCand1BarrelOverlap() ? 0x20 : 0x0 ) |
         ( ( sector2.getROI2() & 0x1f ) << 6 ) |
         ( sector2.isCand2BarrelOverlap() ? 0x800 : 0x0 );
      if( sec1cand1present && sec2cand2present &&
          ( sec1cand1pt <= m_thr ) && ( sec2cand2pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it;
         if( ( it = m_lut.find( element ) ) != m_lut.end() ) {
            if( it->second ) {
               if( sec1cand1pt < sec2cand2pt ) {
                  sector1.setCand1Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 1 in Barrel sector "
                                      << sector1.getSectorNumber() << " in the "
                                      << sector1.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               } else {
                  sector2.setCand2Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 2 in Barrel sector "
                                      << sector2.getSectorNumber() << " in the "
                                      << sector2.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               }
            }
         }
      }

      //
      // Find if a rule exists for sec1 cand2 and sec2 cand2:
      //
      element = ( sector1.getROI2() & 0x1f ) |
         ( sector1.isCand2BarrelOverlap() ? 0x20 : 0x0 ) |
         ( ( sector2.getROI2() & 0x1f ) << 6 ) |
         ( sector2.isCand2BarrelOverlap() ? 0x800 : 0x0 );
      if( sec1cand2present && sec2cand2present &&
          ( sec1cand2pt <= m_thr ) && ( sec2cand2pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it;
         if( ( it = m_lut.find( element ) ) != m_lut.end() ) {
            if( it->second ) {
               if( sec1cand2pt < sec2cand2pt ) {
                  sector1.setCand2Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 2 in Barrel sector "
                                      << sector1.getSectorNumber() << " in the "
                                      << sector1.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               } else {
                  sector2.setCand2Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 2 in Barrel sector "
                                      << sector2.getSectorNumber() << " in the "
                                      << sector2.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               }
            }
         }
      }

      return;
   }

} // namespace LVL1MUCTPI
