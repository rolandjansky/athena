/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFIsolationFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigMuonEFIsolationFillerTool.h"

namespace D3PD {

   TrigMuonEFIsolationFillerTool::
   TrigMuonEFIsolationFillerTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
      : BlockFillerTool< TrigMuonEFIsolation >( type, name, parent )
   {
      // Avoid coverity warnings.
      m_savePtCone = true;
      m_saveEtCone = true;
      m_saveTrackPosition = true;
      book().ignore();

      declareProperty( "SavePtCone", m_savePtCone = false,
                       "Save the ptcone variables?" );
      declareProperty( "SaveEtCone", m_saveEtCone = false,
                       "Save the etcone variables?" );
      declareProperty( "SaveTrackPosition", m_saveTrackPosition = false,
                       "Save the track position index?" );
   }

   StatusCode TrigMuonEFIsolationFillerTool::book() {

      if( m_savePtCone ) {
         CHECK( addVariable( "sumTrkPtCone02", m_sumTrkPtCone02,
                             "sum of pT of tracks in 0.2 cone" ) );
         CHECK( addVariable( "sumTrkPtCone03", m_sumTrkPtCone03,
                             "sum of pT of tracks in 0.2 cone" ) );
      }

      if( m_saveEtCone ) {
         CHECK( addVariable( "sumEtCone01", m_sumEtCone01,
                             "sum of calorimeter Et in 0.1 cone" ) );
         CHECK( addVariable( "sumEtCone02", m_sumEtCone02,
                             "sum of calorimeter Et in 0.2 cone" ) );
         CHECK( addVariable( "sumEtCone03", m_sumEtCone03,
                             "sum of calorimeter Et in 0.3 cone" ) );
         CHECK( addVariable( "sumEtCone04", m_sumEtCone04,
                             "sum of calorimeter Et in 0.4 cone" ) );
      }

      if( m_saveTrackPosition ) {
         CHECK( addVariable( "trackPosition", m_trackPosition,
                             "Position of the TrigMuonEFInfoTrack within the "
                             "TrigMuonEFInfo object" ) );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode TrigMuonEFIsolationFillerTool::fill( const TrigMuonEFIsolation& muon ) {

      if( m_savePtCone ) {
         *m_sumTrkPtCone02 = muon.sumTrkPtCone02();
         *m_sumTrkPtCone03 = muon.sumTrkPtCone03();
      }

      if( m_saveEtCone ) {
         *m_sumEtCone01 = muon.sumEtCone01();
         *m_sumEtCone02 = muon.sumEtCone02();
         *m_sumEtCone03 = muon.sumEtCone03();
         *m_sumEtCone04 = muon.sumEtCone04();
      }

      if( m_saveTrackPosition ) {
         *m_trackPosition  = muon.trackPosition();
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
