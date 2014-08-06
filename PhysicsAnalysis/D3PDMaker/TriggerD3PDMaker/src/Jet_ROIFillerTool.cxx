/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Jet_ROIFillerTool.cxx 504000 2012-06-05 16:28:50Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "Jet_ROIFillerTool.h"

namespace D3PD {

   Jet_ROIFillerTool::Jet_ROIFillerTool( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
      : BlockFillerTool< Jet_ROI >( type, name, parent )
   {
      // Avoid coverity warnings
      m_writeThrInfo = true;
      m_writeRoIWord = true;
      m_writeDecodedInfo = true;
      book().ignore();

      declareProperty( "WriteThrInfo", m_writeThrInfo = false );
      declareProperty( "WriteRoIWord", m_writeRoIWord = false );
      declareProperty( "WriteDecodedInfo", m_writeDecodedInfo = false );

   }

   StatusCode Jet_ROIFillerTool::book() {

      if( m_writeThrInfo ) {
         CHECK( addVariable( "thrNames", m_thrNames,
                             "Names of the passed thresholds" ) );
         CHECK( addVariable( "thrValues", m_thrValues,
                             "Values of the passed thresholds" ) );
         CHECK( addVariable( "thrPattern", m_thrPattern,
                             "Bit-pattern describing the passed thresholds" ) );
      }
      if( m_writeRoIWord ) {
         CHECK( addVariable( "RoIWord", m_roiWord,
                             "RoI word produced by the L1Calo hardware" ) );
      }
      if( m_writeDecodedInfo ) {
         CHECK( addVariable( "et4x4", m_et4x4,
                             "ET deposited in a 4x4 cluster" ) );
         CHECK( addVariable( "et6x6", m_et6x6,
                             "ET deposited in a 6x6 cluster" ) );
         CHECK( addVariable( "et8x8", m_et8x8,
                             "ET deposited in an 8x8 cluster" ) );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode Jet_ROIFillerTool::fill( const Jet_ROI& roi ) {

      if( m_writeThrInfo ) {
         *m_thrNames = roi.getThresholdNames();
         *m_thrValues = roi.getThresholdValues();
         *m_thrPattern = roi.getThrPattern();
      }
      if( m_writeRoIWord ) {
         *m_roiWord = roi.getROIWord();
      }
      if( m_writeDecodedInfo ) {
         *m_et4x4 = roi.getET4x4();
         *m_et6x6 = roi.getET6x6();
         *m_et8x8 = roi.getET8x8();
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
