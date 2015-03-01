/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTEFOverlapCalculator.cxx 650693 2015-03-01 16:53:48Z masato $

// STL include(s):
#include <string>
#include <iomanip>

// XercesC include(s):
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLString.hpp>

// Local include(s):
#include "LUTEFOverlapCalculator.h"
#include "EndcapSector.h"
#include "ForwardSector.h"

namespace LVL1MUCTPI {

   LUTEFOverlapCalculator::LUTEFOverlapCalculator()
      : LUTOverlapCalculatorBase( "LUTEFOverlapCalculator" ) {

   }

   LUTEFOverlapCalculator::LUTEFOverlapCalculator( const LUTEFOverlapCalculator& calc )
      : LUTOverlapCalculatorBase( calc ) {

   }

   /**
    * @param node One of the EF nodes of the XML file
    * @param dumpLut Print the LUT while reading it
    */
  StatusCode LUTEFOverlapCalculator::initialize( const xercesc::DOMNode* bbnode, bool dumpLut, const std::string& runPeriod ) {

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

      REPORT_VERBOSE_MSG( "Looping over \"EFElement\"-s:" );

      const xercesc::DOMNodeList* lut_def_list = bbnode->getChildNodes();
      for( XMLSize_t i = 0; i < lut_def_list->getLength(); ++i ) {

         const xercesc::DOMNode* lut_node = lut_def_list->item( i );
         char* cname = xercesc::XMLString::transcode( lut_node->getNodeName() );
         std::string sname( cname );
         xercesc::XMLString::release( &cname );

         if( sname != "EFElement" ) continue;

         const xercesc::DOMElement* lut_element =
            dynamic_cast< const xercesc::DOMElement* >( lut_node );
         if( ! lut_element ) {
            REPORT_ERROR_MSG( "Unable to cast LUT node into DOMElement" );
            return StatusCode::RECOVERABLE;
         }

         XMLCh* eroi_name  = xercesc::XMLString::transcode( "ERoI" );
         XMLCh* froi_name  = xercesc::XMLString::transcode( "FRoI" );

         const XMLCh* eroi  = lut_element->getAttribute( eroi_name );
         const XMLCh* froi  = lut_element->getAttribute( froi_name );

         xercesc::XMLString::release( &eroi_name );
         xercesc::XMLString::release( &froi_name );

         const int eroi_n   = xercesc::XMLString::parseInt( eroi );
         const int froi_n   = xercesc::XMLString::parseInt( froi );

         //
         // Create the word to be put into the LUT:
         //
         const unsigned int element = ( eroi_n & 0xff ) | ( ( froi_n & 0x3f ) << 8 );

         m_lut[ element ] = true;

         if( dumpLut ) {
            REPORT_VERBOSE_MSG( "  --> ERoI: " << eroi_n << " FRoI: " << froi_n
                                << " Element: 0x" << std::hex << std::setw( 4 )
                                << std::setfill( '0' ) << element << std::dec );
         }
      }

      REPORT_VERBOSE_MSG( "Number of LUT elements: " << m_lut.size() );

      return StatusCode::SUCCESS;
   }

   void LUTEFOverlapCalculator::calculate( const EndcapSector& esec,
                                           const ForwardSector& fsec ) const {

      //
      // Calculate some helper quantities:
      //
      bool eseccand1present = false, eseccand2present = false;
      bool fseccand1present = false, fseccand2present = false;
      int eseccand1pt = -1, eseccand2pt = -1;
      int fseccand1pt = -1, fseccand2pt = -1;
      if( esec.getPtCand1() != 7 ) {
         eseccand1present = true;
         eseccand1pt = esec.getPtCand1();
      }
      if( esec.getPtCand2() != 7 ) {
         eseccand2present = true;
         eseccand2pt = esec.getPtCand2();
      }
      if( fsec.getPtCand1() != 7 ) {
         fseccand1present = true;
         fseccand1pt = fsec.getPtCand1();
      }
      if( fsec.getPtCand2() != 7 ) {
         fseccand2present = true;
         fseccand2pt = fsec.getPtCand2();
      }

      //
      // Find if a rule exists for esec cand1 and fsec cand1:
      //
      unsigned int element = ( esec.getROI1() & 0xff ) | ( ( fsec.getROI1() & 0x3f ) << 8 );
      if( eseccand1present && fseccand1present &&
          ( eseccand1pt <= m_thr ) && ( fseccand1pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it;
         if( ( it = m_lut.find( element ) ) != m_lut.end() ) {
            if( it->second ) {
               if( eseccand1pt < fseccand1pt ) {
                  esec.setCand1Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 1 in Endcap sector "
                                      << esec.getSectorNumber() << " in the "
                                      << esec.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               } else {
                  fsec.setCand1Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 1 in Forward sector "
                                      << fsec.getSectorNumber() << " in the "
                                      << fsec.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               }
            }
         }
      }

      //
      // Find if a rule exists for esec cand2 and fsec cand1:
      //
      element = ( esec.getROI2() & 0xff ) | ( ( fsec.getROI1() & 0x3f ) << 8 );
      if( eseccand2present && fseccand1present &&
          ( eseccand2pt <= m_thr ) && ( fseccand1pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it;
         if( ( it = m_lut.find( element ) ) != m_lut.end() ) {
            if( it->second ) {
               if( eseccand2pt < fseccand1pt ) {
                  esec.setCand2Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 2 in Endcap sector "
                                      << esec.getSectorNumber() << " in the "
                                      << esec.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               } else {
                  fsec.setCand1Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 1 in Forward sector "
                                      << fsec.getSectorNumber() << " in the "
                                      << fsec.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               }
            }
         }
      }

      //
      // Find if a rule exists for esec cand1 and fsec cand2:
      //
      element = ( esec.getROI1() & 0xff ) | ( ( fsec.getROI2() & 0x3f ) << 8 );
      if( eseccand1present && fseccand2present &&
          ( eseccand1pt <= m_thr ) && ( fseccand2pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it;
         if( ( it = m_lut.find( element ) ) != m_lut.end() ) {
            if( it->second ) {
               if( eseccand1pt < fseccand2pt ) {
                  esec.setCand1Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 1 in Endcap sector "
                                      << esec.getSectorNumber() << " in the "
                                      << esec.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               } else {
                  fsec.setCand2Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 2 in Forward sector "
                                      << fsec.getSectorNumber() << " in the "
                                      << fsec.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               }
            }
         }
      }

      //
      // Find if a rule exists for esec cand2 and fsec cand2:
      //
      element = ( esec.getROI2() & 0xff ) | ( ( fsec.getROI2() & 0x3f ) << 8 );
      if( eseccand2present && fseccand2present &&
          ( eseccand2pt <= m_thr ) && ( fseccand2pt <= m_thr ) ) {
         std::map< unsigned int, bool >::const_iterator it;
         if( ( it = m_lut.find( element ) ) != m_lut.end() ) {
            if( it->second ) {
               if( eseccand2pt < fseccand2pt ) {
                  esec.setCand2Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 2 in Endcap sector "
                                      << esec.getSectorNumber() << " in the "
                                      << esec.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               } else {
                  fsec.setCand2Supressed( true );
#ifndef NDEBUG
                  REPORT_VERBOSE_MSG( "Candidate 2 in Forward sector "
                                      << fsec.getSectorNumber() << " in the "
                                      << fsec.getRapidityString()
                                      << " rapidity region supressed" );
#endif // NDEBUG
               }
            }
         }
      }

      return;
   }

} // namespace LVL1MUCTPI
