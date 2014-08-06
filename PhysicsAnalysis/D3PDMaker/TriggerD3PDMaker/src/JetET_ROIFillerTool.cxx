/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetET_ROIFillerTool.cxx 504000 2012-06-05 16:28:50Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "JetET_ROIFillerTool.h"

namespace D3PD {

   JetET_ROIFillerTool::JetET_ROIFillerTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
      : BlockFillerTool< JetET_ROI >( type, name, parent )
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

   StatusCode JetET_ROIFillerTool::book() {

      if( m_writeThrInfo ) {
         CHECK( addVariable( "thrNames", m_thresholds,
                             "Names of the passed thresholds" ) );
      }
      if( m_writeRoIWord ) {
         CHECK( addVariable( "RoIWord", m_roiWord,
                             "RoI word produced by the L1Calo hardware", 0 ) );
      }
      if( m_writeDecodedInfo ) {
         CHECK( addVariable( "thrPattern", m_thrPattern,
                             "Bit-pattern describing the passed thresholds" ) );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode JetET_ROIFillerTool::fill( const JetET_ROI& roi ) {

      if( m_writeThrInfo ) {
         *m_thresholds = roi.getThresholds();
      }
      if( m_writeRoIWord ) {
         *m_roiWord = roi.getROIWord();
      }
      if( m_writeDecodedInfo ) {
         *m_thrPattern = roi.getThrPattern();
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
