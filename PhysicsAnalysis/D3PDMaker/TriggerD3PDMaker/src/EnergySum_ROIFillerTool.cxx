/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySum_ROIFillerTool.cxx 504000 2012-06-05 16:28:50Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "EnergySum_ROIFillerTool.h"

namespace D3PD {

   EnergySum_ROIFillerTool::EnergySum_ROIFillerTool( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
      : BlockFillerTool< EnergySum_ROI >( type, name, parent )
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

   StatusCode EnergySum_ROIFillerTool::book() {

      if( m_writeThrInfo ) {
         CHECK( addVariable( "thrNames", m_thresholds,
                             "Names of the passed thresholds" ) );
      }
      if( m_writeRoIWord ) {
         CHECK( addVariable( "RoIWord0", m_roiWord0,
                             "First RoI word produced by the L1Calo hardware", 0 ) );
         CHECK( addVariable( "RoIWord1", m_roiWord1,
                             "Second RoI word produced by the L1Calo hardware", 0 ) );
         CHECK( addVariable( "RoIWord2", m_roiWord2,
                             "Third RoI word produced by the L1Calo hardware", 0 ) );
      }
      if( m_writeDecodedInfo ) {
         CHECK( addVariable( "ExMiss", m_eXmiss,
                             "Missing energy in the X direction", 0.0 ) );
         CHECK( addVariable( "EyMiss", m_eYmiss,
                             "Missing energy in the Y direction", 0.0 ) );
         CHECK( addVariable( "energyT", m_energyT,
                             "Total transverse energy deposited", -99999.9 ) );
         CHECK( addVariable( "overflowX", m_overflowX,
                             "Overflow flag of the E_x calculation", false ) );
         CHECK( addVariable( "overflowY", m_overflowY,
                             "Overflow flag of the E_y calculation", false ) );
         CHECK( addVariable( "overflowT", m_overflowT,
                             "Overflow flag of the E_T calculation", false ) );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode EnergySum_ROIFillerTool::fill( const EnergySum_ROI& roi ) {

      if( m_writeThrInfo ) {
         *m_thresholds = roi.getThresholds();
      }
      if( m_writeRoIWord ) {
         *m_roiWord0 = roi.getROIWord0();
         *m_roiWord1 = roi.getROIWord1();
         *m_roiWord2 = roi.getROIWord2();
      }
      if( m_writeDecodedInfo ) {
         *m_eXmiss   = roi.getExMiss();
         *m_eYmiss   = roi.getEyMiss();
         *m_energyT = roi.getEnergyT();
         *m_overflowX = roi.getOverflowX();
         *m_overflowY = roi.getOverflowY();
         *m_overflowT = roi.getOverflowT();
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
