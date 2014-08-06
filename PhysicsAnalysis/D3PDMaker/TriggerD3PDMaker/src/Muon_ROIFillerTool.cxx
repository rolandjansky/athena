/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Muon_ROIFillerTool.cxx 504000 2012-06-05 16:28:50Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "Muon_ROIFillerTool.h"

namespace D3PD {

   Muon_ROIFillerTool::Muon_ROIFillerTool( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
      : BlockFillerTool< Muon_ROI >( type, name, parent )
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

   StatusCode Muon_ROIFillerTool::book() {

      if( m_writeThrInfo ) {
         CHECK( addVariable( "thrName", m_thrName,
                             "Names of the passed thresholds" ) );
      }
      if( m_writeRoIWord ) {
         CHECK( addVariable( "RoIWord", m_roiWord,
                             "32-bit RoI word given by the LVL1 muon trigger" ) );
      }
      if( m_writeDecodedInfo ) {
         CHECK( addVariable( "thrNumber", m_thrNumber,
                             "Number of the highest threshold this RoI passed" ) );
         CHECK( addVariable( "RoINumber", m_roi,
                             "\"RoI\" (pad) number of the muon candidate" ) );
         CHECK( addVariable( "sectorAddress", m_sectorAddress,
                             "The encoded \"full\" sector address" ) );
         CHECK( addVariable( "firstCandidate", m_firstCandidate,
                             "True if the candidate had the highest pT in its sector" ) );
         CHECK( addVariable( "moreCandInRoI", m_moreCandInRoI,
                             "True if there were other muons detected in the same pad" ) );
         CHECK( addVariable( "moreCandInSector", m_moreCandInSector,
                             "True if there were >2 muons detected in the same sector" ) );
         CHECK( addVariable( "source", m_source,
                             "Muon candidate system ID (Barrel=0, Endcap=1, Forward=2)" ) );
         CHECK( addVariable( "hemisphere", m_hemisphere,
                             "Hemisphere of the muon candidate (Negative=0, Positive=1)" ) );
         CHECK( addVariable( "charge", m_charge,
                             "Charge of the muon candidate (Negative=0, Positive=1,"
                             " Undefined=100)" ) );
         CHECK( addVariable( "vetoed", m_vetoed,
                             "True if the candidate was vetoed in the multiplicity sum" ) );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode Muon_ROIFillerTool::fill( const Muon_ROI& roi ) {

      if( m_writeThrInfo ) {
         *m_thrName = roi.getThrName();
      }
      if( m_writeRoIWord ) {
         *m_roiWord = roi.getROIWord();
      }
      if( m_writeDecodedInfo ) {
         *m_thrNumber = roi.getThrNumber();
         *m_roi = roi.getRoI();
         *m_sectorAddress = roi.getSectorAddress();
         *m_firstCandidate = roi.isFirstCandidate();
         *m_moreCandInRoI = roi.isMoreCandInRoI();
         *m_moreCandInSector = roi.isMoreCandInSector();
         switch( roi.getSource() ) {
         case Muon_ROI::Barrel:
            *m_source = 0;
            break;
         case Muon_ROI::Endcap:
            *m_source = 1;
            break;
         case Muon_ROI::Forward:
            *m_source = 2;
            break;
         default:
            *m_source = -1;
            break;
         }
         switch( roi.getHemisphere() ) {
         case Muon_ROI::Negative:
            *m_hemisphere = 0;
            break;
         case Muon_ROI::Positive:
            *m_hemisphere = 1;
            break;
         default:
            *m_hemisphere = -1;
            break;
         }
         switch( roi.getCharge() ) {
         case Muon_ROI::Neg:
            *m_charge = 0;
            break;
         case Muon_ROI::Pos:
            *m_charge = 1;
            break;
         case Muon_ROI::Undef:
            *m_charge = 100;
            break;
         default:
            *m_charge = -1;
            break;
         }
         *m_vetoed = roi.isVetoed();
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
