/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFCbTrackFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigMuonEFCbTrackFillerTool.h"

namespace D3PD {

   TrigMuonEFCbTrackFillerTool::TrigMuonEFCbTrackFillerTool( const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent )
      : BlockFillerTool< TrigMuonEFCbTrack >( type, name, parent )
   {
     // Avoid coverity warnings
     m_saveFitResults = true;
     m_saveHitInfo = true;
     book().ignore();

     declareProperty( "SaveFitResults", m_saveFitResults = false );
     declareProperty( "SaveHitInfo",    m_saveHitInfo = false );
   }

   StatusCode TrigMuonEFCbTrackFillerTool::book() {
      if( m_saveFitResults ) {
         CHECK( addVariable( "matchChi2", m_matchChi2 ) );
      }
      if( m_saveHitInfo ) {
         CHECK( addVariable( "NIdSctHits",   m_nIdSctHits ) );
         CHECK( addVariable( "NIdPixelHits", m_nIdPixelHits ) );
         CHECK( addVariable( "NTrtHits",     m_nTrtHits ) );
      }
      return StatusCode::SUCCESS;
   }

   StatusCode TrigMuonEFCbTrackFillerTool::fill( const TrigMuonEFCbTrack& muon ) {
      if( m_saveFitResults ) {
         *m_matchChi2 = muon.matchChi2();
      }  
      if( m_saveHitInfo ) {
         *m_nIdSctHits   = muon.NIdSctHits();
         *m_nIdPixelHits = muon.NIdPixelHits();
         *m_nTrtHits     = muon.NTrtHits();
      }
      return StatusCode::SUCCESS;
   }

} // namespace D3PD
