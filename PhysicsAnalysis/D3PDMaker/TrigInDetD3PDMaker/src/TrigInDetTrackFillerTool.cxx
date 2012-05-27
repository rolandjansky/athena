/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigInDetTrackFillerTool.cxx 502719 2012-05-27 21:28:59Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigInDetTrack.h"

// Local include(s):
#include "TrigInDetTrackFillerTool.h"

namespace D3PD {
  
   TrigInDetTrackFillerTool::TrigInDetTrackFillerTool( const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent )
      : BlockFillerTool< TrigInDetTrack >( type, name, parent )
   {
      // Avoid coverity warnings.
      m_writeBasicInfo = true;
      m_writeHypoInfo = true;
      m_writeDetailedInfo = true;
      book().ignore();

      declareProperty( "WriteBasicInfo", m_writeBasicInfo = false,
                       "Writes basic information" );
      declareProperty( "WriteHypoInfo", m_writeHypoInfo = false,
                       "Writes information for hypothesis" );
      declareProperty( "WriteDetailedInfo", m_writeDetailedInfo = false,
                       "Writes detailed information" );
   }

   /**
    * Here the output variables are declared to the D3PDMaker framework.
    */
   StatusCode TrigInDetTrackFillerTool::book() {

      if( m_writeBasicInfo ) {
         CHECK( addVariable( "algorithmId", m_algId,
                             "Algorithm ID (NULLID=0, SITRACKID=1, IDSCANID=2, "
                             "TRTLUTID=3, TRTXKID=4)" ) );
         CHECK( addVariable( "trackStatus", m_status,
                             "Track status (0=good, 1=no parameters, 2=no covariance)" ) );
         CHECK( addVariable( "chi2Ndof",    m_chi2,
                             "Chi2 of the track fit normalized on number of DOF" ) );
      }
      if( m_writeHypoInfo ) {
         CHECK( addVariable( "nStrawHits",        m_nStrawHits,
                             "Number of TRT hits associated with track" ) );
         CHECK( addVariable( "nHighThrHits",      m_nTRHits,
                             "Number of high-threshold TRT hits associated with track" ) ); 
         CHECK( addVariable( "nPixelSpacePoints", m_nPixelSpacePoints,
                             "Number of Pixel spacepoints associated with track" ) );
         CHECK( addVariable( "nSCT_SpacePoints",  m_nSCT_SpacePoints,
                             "Number of SCT spacepoints associated with track" ) );
      }
      if( m_writeDetailedInfo ) {
         CHECK( addVariable( "hitPattern", m_hitPattern,
                             "Hit pattern of silicon spacepoints" ) );
         CHECK( addVariable( "nStraw",     m_nStraw,
                             "Number of TRT straws intersected by track" ) );
         CHECK( addVariable( "nStrawTime", m_nStrawTime,
                             "Number of TRT straws with valid drift time intersected "
                             "by track" ) );
      }

      return StatusCode::SUCCESS;
   }

   /**
    * Here the output variables are filled with the information coming from a
    * TrigInDetTrack object.
    */
   StatusCode TrigInDetTrackFillerTool::fill( const TrigInDetTrack& tr ) {

      if( m_writeBasicInfo ) {

         *m_algId = tr.algorithmId();
         *m_chi2 = tr.chi2();
         *m_status = 0;
         if( ! tr.param() ) {
            REPORT_MESSAGE( MSG::DEBUG ) << "TrigInDetTrack has no parameters";
            *m_status = 1;
         } else {
            if( ! tr.param()->cov() ) {
               REPORT_MESSAGE( MSG::DEBUG ) << "TrigInDetTrackFitPars has no covariance";
               *m_status = 2;
            }
         }
      }
      if( m_writeHypoInfo ) {

         *m_nPixelSpacePoints = tr.NPixelSpacePoints();
         *m_nSCT_SpacePoints = tr.NSCT_SpacePoints();
         *m_nStrawHits = tr.NStrawHits(); 
         *m_nTRHits = tr.NTRHits();

      }
      if( m_writeDetailedInfo ) {

         *m_hitPattern = tr.HitPattern();
         *m_nStraw = tr.NStraw();
         *m_nStrawTime = tr.NStrawTime();

      }

      return StatusCode::SUCCESS;
   }

}
