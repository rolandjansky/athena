/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTEEOverlapCalculator.cxx 650693 2015-03-01 16:53:48Z masato $

// STL include(s):
#include <sstream>
#include <iomanip>

// XercesC include(s):
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLString.hpp>

// Local include(s):
#include "LUTEEOverlapCalculator.h"
#include "EndcapSector.h"
#include "../Common/PtMultiplicitySet.h"

namespace LVL1MUCTPI {

   LUTEEOverlapCalculator::LUTEEOverlapCalculator()
      : LUTOverlapCalculatorBase( "LUTEEOverlapCalculator" ) {

   }

   LUTEEOverlapCalculator::LUTEEOverlapCalculator( const LUTEEOverlapCalculator& calc )
      : LUTOverlapCalculatorBase( calc ) {

   }

   StatusCode LUTEEOverlapCalculator::initialize( const xercesc::DOMNode* eenode,
                                                  bool dumpLut, const std::string& runPeriod ) {

     if( runPeriod == "RUN2" ){
       setMaximumThreshold( 6 );
       REPORT_VERBOSE_MSG( "RunPeriod=" << runPeriod << " setMaximumThreshold to 6" );
     }

      // Clear the internal LUT:
      m_lut.clear();

      //
      // Check if the received node is of the right type:
      //
      if( eenode->getNodeType() != xercesc::DOMNode::ELEMENT_NODE ) {
         REPORT_ERROR_MSG( "Received node is not a DOMElement" );
         return StatusCode::RECOVERABLE;
      }

      REPORT_VERBOSE_MSG( "Looping over \"EEElement\"-s:" );

      //
      // Initialize the E-E LUT:
      //
      const xercesc::DOMNodeList* lut_lines = eenode->getChildNodes();
      for( XMLSize_t i = 0; i < lut_lines->getLength(); ++i ) {

         const xercesc::DOMNode* lut_line = lut_lines->item( i );

         char* cname = xercesc::XMLString::transcode( lut_line->getNodeName() );
         std::string sname( cname );
         xercesc::XMLString::release( &cname );

         if( sname != "EEElement" ) continue;

         const xercesc::DOMElement* lut_element =
            dynamic_cast< const xercesc::DOMElement* >( lut_line );
         if( ! lut_element ) {
            REPORT_ERROR_MSG( "Unable to cast LUT node into DOMElement" );
            return StatusCode::RECOVERABLE;
         }

         XMLCh* roi1_name = xercesc::XMLString::transcode( "RoI1" );
         XMLCh* roi2_name = xercesc::XMLString::transcode( "RoI2" );

         const XMLCh* roi1 = lut_element->getAttribute( roi1_name );
         const XMLCh* roi2 = lut_element->getAttribute( roi2_name );

         xercesc::XMLString::release( &roi1_name );
         xercesc::XMLString::release( &roi2_name );

         const unsigned int roi1_n = xercesc::XMLString::parseInt( roi1 );
         const unsigned int roi2_n = xercesc::XMLString::parseInt( roi2 );

         //
         // Create the word to be put into the LUT:
         //
         const unsigned int element = ( roi1_n & 0xff ) | ( ( roi2_n & 0xff ) << 8 );

         m_lut[ element ] = true;

         if( dumpLut ) {
            REPORT_VERBOSE_MSG( "  --> RoI1: " << roi1_n << " RoI2: " << roi2_n
                                << " Element: 0x" << std::hex << std::setw( 4 )
                                << std::setfill( '0' ) << element << std::dec );
         }
      }

      REPORT_VERBOSE_MSG( "Number of LUT elements: " << m_lut.size() );

      return StatusCode::SUCCESS;
   }

   void LUTEEOverlapCalculator::calculate( const EndcapSector& sector1,
                                           const EndcapSector& sector2 ) const {

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
      unsigned int element = ( sector1.getROI1() & 0xff ) |
         ( ( sector2.getROI1() & 0xff ) << 8 );
      if( sec1cand1present && sec2cand1present &&
          ( sec1cand1pt <= m_thr ) && ( sec2cand1pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it;
         if( ( it = m_lut.find( element ) ) != m_lut.end() ) {
            if( it->second ) {
               if( sec1cand1pt < sec2cand1pt ) {
                  sector1.setCand1Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 1 in Endcap sector " << sector1.getSectorNumber()
                                      << " in the " << sector1.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               } else {
                  sector2.setCand1Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 1 in Endcap sector " << sector2.getSectorNumber()
                                      << " in the " << sector2.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               }
            }
         }
      }

      //
      // Find if a rule exists for sec1 cand2 and sec2 cand1:
      //
      element = ( sector1.getROI2() & 0xff ) |
         ( ( sector2.getROI1() & 0xff ) << 8 );
      if( sec1cand2present && sec2cand1present &&
          ( sec1cand2pt <= m_thr ) && ( sec2cand1pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it;
         if( ( it = m_lut.find( element ) ) != m_lut.end() ) {
            if( it->second ) {
               if( sec1cand2pt < sec2cand1pt ) {
                  sector1.setCand2Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 2 in Endcap sector " << sector1.getSectorNumber()
                                      << " in the " << sector1.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               } else {
                  sector2.setCand1Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 1 in Endcap sector " << sector2.getSectorNumber()
                                      << " in the " << sector2.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               }
            }
         }
      }

      //
      // Find if a rule exists for sec1 cand1 and sec2 cand2:
      //
      element = ( sector1.getROI1() & 0xff ) |
         ( ( sector2.getROI2() & 0xff ) << 8 );
      if( sec1cand1present && sec2cand2present &&
          ( sec1cand1pt <= m_thr ) && ( sec2cand2pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it;
         if( ( it = m_lut.find( element ) ) != m_lut.end() ) {
            if( it->second ) {
               if( sec1cand1pt < sec2cand2pt ) {
                  sector1.setCand1Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 1 in Endcap sector " << sector1.getSectorNumber()
                                      << " in the " << sector1.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               } else {
                  sector2.setCand2Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 2 in Endcap sector " << sector2.getSectorNumber()
                                      << " in the " << sector2.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               }
            }
         }
      }

      //
      // Find if a rule exists for sec1 cand2 and sec2 cand2:
      //
      element = ( sector1.getROI2() & 0xff ) |
         ( ( sector2.getROI2() & 0xff ) << 8 );
      if( sec1cand2present && sec2cand2present &&
          ( sec1cand2pt <= m_thr ) && ( sec2cand2pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it;
         if( ( it = m_lut.find( element ) ) != m_lut.end() ) {
            if( it->second ) {
               if( sec1cand2pt < sec2cand2pt ) {
                  sector1.setCand2Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 2 in Endcap sector " << sector1.getSectorNumber()
                                      << " in the " << sector1.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               } else {
                  sector2.setCand2Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 2 in Endcap sector " << sector2.getSectorNumber()
                                      << " in the " << sector2.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               }
            }
         }
      }

      return;
   }

} // namespace LVL1MUCTPI
