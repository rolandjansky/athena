/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTau_ROIFillerTool.cxx 504000 2012-06-05 16:28:50Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "EmTau_ROIFillerTool.h"

namespace D3PD {

   EmTau_ROIFillerTool::EmTau_ROIFillerTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
      : BlockFillerTool< EmTau_ROI >( type, name, parent )
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

   StatusCode EmTau_ROIFillerTool::book() {

      if( m_writeThrInfo ) {
         CHECK( addVariable( "thrNames", m_thrNames,
                             "Names of the passed thresholds" ) );
         CHECK( addVariable( "thrValues", m_thrValues,
                             "Values of the passed thresholds" ) );
      }
      if( m_writeRoIWord ) {
         CHECK( addVariable( "RoIWord", m_roiWord,
                             "32-bit RoI word produced by the L1Calo hardware" ) );
      }
      if( m_writeDecodedInfo ) {
         CHECK( addVariable( "core", m_core,
                             "ET of the RoI Core cluster (2x2 towers, EM+Had)" ) );
         CHECK( addVariable( "EMClus", m_emClus,
                             "Deposited ET of the \"EM cluster\"" ) );
         CHECK( addVariable( "tauClus", m_tauClus,
                             "Deposited ET of the \"tau cluster\"" ) );
         CHECK( addVariable( "EMIsol", m_emIsol,
                             "EM calorimeter isolation (outer ring of EM towers)" ) );
         CHECK( addVariable( "hadIsol", m_hadIsol,
                             "Hadron calorimeter isolation (outer ring of had towers)" ) );
         CHECK( addVariable( "hadCore", m_hadCore,
                             "ET deposited in the inner hadronic isolation region "
                             "(2x2 core)" ) );
         CHECK( addVariable( "thrPattern", m_thrPattern,
                             "Bit-pattern describing the passed thresholds" ) );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode EmTau_ROIFillerTool::fill( const EmTau_ROI& roi ) {

      if( m_writeThrInfo ) {
         *m_thrNames = roi.getThresholdNames();
         *m_thrValues = roi.getThresholdValues();
      }
      if( m_writeRoIWord ) {
         *m_roiWord = roi.getROIWord();
      }
      if( m_writeDecodedInfo ) {
         *m_core = roi.getCore();
         *m_emClus = roi.getEMClus();
         *m_tauClus = roi.getTauClus();
         *m_emIsol = roi.getEMIsol();
         *m_hadIsol = roi.getHadIsol();
         *m_hadCore = roi.getHadCore();
         *m_thrPattern = roi.getThrPattern();
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
