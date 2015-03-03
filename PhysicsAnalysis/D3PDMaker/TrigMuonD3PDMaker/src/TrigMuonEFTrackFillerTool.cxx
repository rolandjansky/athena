/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFTrackFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigMuonEFTrackFillerTool.h"

namespace D3PD {

   TrigMuonEFTrackFillerTool::TrigMuonEFTrackFillerTool( const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent )
      : BlockFillerTool< TrigMuonEFTrack >( type, name, parent )
   {
     // Avoid coverity warnings.
     m_saveFitResults = true;
     m_saveHitInfo = true;
     book().ignore();

     declareProperty( "SaveFitResults", m_saveFitResults = false );
     declareProperty( "SaveHitInfo",    m_saveHitInfo = false );
   }

   StatusCode TrigMuonEFTrackFillerTool::book() {

      if( m_saveFitResults ) {
         CHECK( addVariable( "charge",   m_charge,   "Charge of the track", -1.0 ) );
         CHECK( addVariable( "d0",       m_d0,       "D0 parameter of track", -999.9 ) );
         CHECK( addVariable( "z0",       m_z0,       "Z0 parameter of track", -999.9 ) );
         CHECK( addVariable( "chi2",     m_chi2,     "Track fit goodness", 5000.0 ) );
         CHECK( addVariable( "chi2prob", m_chi2prob ) );

         CHECK( addVariable( "posX", m_posX ) );
         CHECK( addVariable( "posY", m_posY ) );
         CHECK( addVariable( "posZ", m_posZ ) );
      }

      if( m_saveHitInfo ) {
         CHECK( addVariable( "NRpcHitsPhi", m_nRpcHitsPhi ) );
         CHECK( addVariable( "NTgcHitsPhi", m_nTgcHitsPhi ) );
         CHECK( addVariable( "NCscHitsPhi", m_nCscHitsPhi ) );
         CHECK( addVariable( "NRpcHitsEta", m_nRpcHitsEta ) );
         CHECK( addVariable( "NTgcHitsEta", m_nTgcHitsEta ) );
         CHECK( addVariable( "NCscHitsEta", m_nCscHitsEta ) );
         CHECK( addVariable( "NMdtHits",    m_nMdtHits ) );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode TrigMuonEFTrackFillerTool::fill( const TrigMuonEFTrack& muon ) {

      if( m_saveFitResults ) {
         *m_charge   = muon.charge();
         *m_d0       = muon.d0();
         *m_z0       = muon.z0();
         *m_chi2     = muon.chi2();
         *m_chi2prob = muon.chi2prob();

         *m_posX = muon.posX();
         *m_posY = muon.posY();
         *m_posZ = muon.posZ();
      }

      if( m_saveHitInfo ) {
         *m_nRpcHitsPhi = muon.NRpcHitsPhi();
         *m_nTgcHitsPhi = muon.NTgcHitsPhi();
         *m_nCscHitsPhi = muon.NCscHitsPhi();
         *m_nRpcHitsEta = muon.NRpcHitsEta();
         *m_nTgcHitsEta = muon.NTgcHitsEta();
         *m_nCscHitsEta = muon.NCscHitsEta();
         *m_nMdtHits    = muon.NMdtHits();
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
