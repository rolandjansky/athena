/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
#include "VrtSecInclusive/NtupleVars.h"
#include "VrtSecInclusive/Tools.h"

#include <iostream>

#include "TH1D.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"

//-------------------------------------------------

using namespace std;

namespace VKalVrtAthena {
  

  //____________________________________________________________________________________________________
  bool VrtSecInclusive::selectTrack_d0Cut       ( const xAOD::TrackParticle* trk ) const { return ( fabs( trk->d0() ) > m_jp.d0TrkPVDstMinCut && fabs( trk->d0() ) < m_jp.d0TrkPVDstMaxCut ); }
  bool VrtSecInclusive::selectTrack_z0Cut       ( const xAOD::TrackParticle* trk ) const { return ( fabs( trk->z0() ) > m_jp.z0TrkPVDstMinCut && fabs( trk->z0() ) < m_jp.z0TrkPVDstMaxCut ); }
  bool VrtSecInclusive::selectTrack_d0errCut    ( const xAOD::TrackParticle* trk ) const { const double cov11 = trk->definingParametersCovMatrix()(0,0); return cov11 < m_jp.d0TrkErrorCut*m_jp.d0TrkErrorCut; }
  bool VrtSecInclusive::selectTrack_z0errCut    ( const xAOD::TrackParticle* trk ) const { const double cov22 = trk->definingParametersCovMatrix()(1,1); return cov22 < m_jp.z0TrkErrorCut*m_jp.z0TrkErrorCut; }
  bool VrtSecInclusive::selectTrack_d0signifCut ( const xAOD::TrackParticle*     ) const { return true; }
  bool VrtSecInclusive::selectTrack_z0signifCut ( const xAOD::TrackParticle*     ) const { return true; }
  bool VrtSecInclusive::selectTrack_pTCut       ( const xAOD::TrackParticle* trk ) const { return trk->pt() > m_jp.TrkPtCut; }
  bool VrtSecInclusive::selectTrack_chi2Cut     ( const xAOD::TrackParticle* trk ) const { return trk->chiSquared() / (trk->numberDoF()+AlgConsts::infinitesimal) < m_jp.TrkChi2Cut; }
  
  //____________________________________________________________________________________________________
  bool VrtSecInclusive::selectTrack_hitPattern( const xAOD::TrackParticle* trk ) const {
    
    uint8_t PixelHits = 0;
    uint8_t SctHits   = 0; 
    uint8_t BLayHits  = 0;
    uint8_t PixShare  = 0;
    uint8_t SctShare  = 0;
    uint8_t TRTHits   = 0;
    
    if( !(trk->summaryValue( PixelHits, xAOD::numberOfPixelHits               ) ) ) PixelHits =0;
    if( !(trk->summaryValue( SctHits,   xAOD::numberOfSCTHits                 ) ) ) SctHits   =0;
    if( !(trk->summaryValue( BLayHits,  xAOD::numberOfInnermostPixelLayerHits ) ) ) BLayHits  =0;
    if( !(trk->summaryValue( PixShare,  xAOD::numberOfPixelSharedHits         ) ) ) PixShare  =0;
    if( !(trk->summaryValue( SctShare,  xAOD::numberOfSCTSharedHits           ) ) ) SctShare  =0;
    if( !(trk->summaryValue( TRTHits,   xAOD::numberOfTRTHits                 ) ) ) TRTHits   =0;
    
    uint8_t SharedHits = PixShare + SctShare;

    // do Pixel/SCT/SiHits only if we exclude StandAlone TRT hits
    if( !m_jp.SAloneTRT ) {
      if(PixelHits	     < m_jp.CutPixelHits)  return false;
      if(SctHits             < m_jp.CutSctHits)    return false;
      if((PixelHits+SctHits) < m_jp.CutSiHits)     return false;
      if(BLayHits	     < m_jp.CutBLayHits)   return false;
      if(SharedHits	     > m_jp.CutSharedHits) return false;
    }

    // The folloing part reproduces the track selection in RPVDixpVrt
    if( m_jp.doTRTPixCut ) {
      if(TRTHits == 0 && PixelHits < 2)           return false;
    }
    
    return true;
  }
  
  //____________________________________________________________________________________________________
  bool VrtSecInclusive::selectTrack_notPVassociated  ( const xAOD::TrackParticle* trk ) const {
      return !( VKalVrtAthena::isAssociatedToVertices( trk, m_primaryVertices ) );
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode  VrtSecInclusive::selectTracks() { 
    
    ATH_MSG_DEBUG( " > selectTracks: begin"  );
    
    //--------------------------------------------------------
    //  Extract tracks from xAOD::TrackParticle container
    //
    
    const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
    ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_jp.TrackLocation) );
    
    ATH_MSG_DEBUG( "Extracted xAOD::TrackParticle number=" << trackParticleContainer->size() );
    
    
    if( m_jp.FillNtuple )
      m_ntupleVars->get<unsigned int>( "NumAllTrks" ) = static_cast<int>( trackParticleContainer->size() );
    
    
    //-----------------------------------------------------------
    //  Track selection
    //
    

    static SG::AuxElement::Decorator< char > decor_isSelected( "is_selected" );

    // Setup cut functions
    using cutFunc = bool (VrtSecInclusive::*) ( const xAOD::TrackParticle* ) const;
    std::vector<cutFunc> cuts;
    
    // These cuts are optional. Specified by JobProperty
    if( m_jp.do_PVvetoCut )   cuts.emplace_back( &VrtSecInclusive::selectTrack_notPVassociated );
    if( m_jp.do_d0Cut )       cuts.emplace_back( &VrtSecInclusive::selectTrack_d0Cut );
    if( m_jp.do_z0Cut )       cuts.emplace_back( &VrtSecInclusive::selectTrack_d0Cut );
    if( m_jp.do_d0errCut )    cuts.emplace_back( &VrtSecInclusive::selectTrack_d0errCut );
    if( m_jp.do_z0errCut )    cuts.emplace_back( &VrtSecInclusive::selectTrack_z0errCut );
    //if( m_jp.do_d0signifCut ) cuts.emplace_back( &VrtSecInclusive::selectTrack_d0signifCut ); // not implemented yet
    //if( m_jp.do_z0signifCut ) cuts.emplace_back( &VrtSecInclusive::selectTrack_z0signifCut ); // not implemented yet
    
    // These cuts are used by default
    cuts.emplace_back( &VrtSecInclusive::selectTrack_hitPattern );
    cuts.emplace_back( &VrtSecInclusive::selectTrack_chi2Cut );
    cuts.emplace_back( &VrtSecInclusive::selectTrack_pTCut );
    
    
    // Loop over tracks
    for( auto *trk : *trackParticleContainer ) {
      
      std::vector<bool> cutBits;
      
      for( auto func : cuts ) cutBits.emplace_back( (this->*func)( trk ) );
      
      if( m_jp.FillHist ) {
        m_hists["trkSelCuts"]->Fill( 0 );
        for( size_t ibit = 0; ibit < cutBits.size(); ibit++) {
          if( cutBits.at(ibit) ) {
            m_hists["trkSelCuts"]->Fill( ibit+1 );
          } else {
            break;
          }
        }
      }
      
      // Good track should not find any false bit
      bool isGood_standard = ( std::find( cutBits.begin(), cutBits.end(), false ) == cutBits.end() );
      
      if( isGood_standard ) {
        
        // Store the selected track to the new m_selectedTracks
        // Here we firstly need to register the empty pointer to the m_selectedTracks,
        // then need to do deep copy after then. This is the feature of xAOD.
        
        unsigned long barcode=0;
      
        if( m_jp.doTruth ) {  
        
          const auto* truth = getTrkGenParticle(trk);
        
          if ( truth ) {
            barcode = truth->barcode();
          }
        
        }
      
        decor_isSelected( *trk ) = true;
      
        m_selectedTracks->emplace_back( trk );
      
        if( m_jp.FillNtuple ) m_ntupleVars->get< vector<int> >( "SelTrk_barcode" ).emplace_back(barcode); // will need this later          
      
        ATH_MSG_VERBOSE( " > selectTracks: Track index " << trk->index() << " has been selected." );
        ATH_MSG_VERBOSE( " > selectTracks: Track index " << trk->index()
                         << " parameter:"
                         << " pt = "  << trk->pt()
                         << " eta = " << trk->eta()
                         << " d0 = "  << trk->d0()
                         << " z0 = "  << trk->z0() << "." );
        
      }
      
    }
    
    ATH_MSG_DEBUG( "execute: Number of total tracks      = " << trackParticleContainer->size() );
    ATH_MSG_DEBUG( "execute: Number of selected tracks   = " << m_selectedTracks->size() );
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode  VrtSecInclusive::selectTracksFromMuons() { 
    
    const xAOD::MuonContainer* muons ( nullptr );
    ATH_CHECK( evtStore()->retrieve( muons, "Muons") );
    
    static SG::AuxElement::Decorator< char > decor_isSelected( "is_selected" );
    
    for( const auto& muon : *muons ) {
      const auto& primaryTrackLink = muon->primaryTrackParticleLink();
      const auto* trk = *primaryTrackLink;
      if( trk ) {
        decor_isSelected( *trk ) = true;
        m_selectedTracks->emplace_back( trk );
      }
    }
    
    return StatusCode::SUCCESS;
  }
  
  //____________________________________________________________________________________________________
  StatusCode  VrtSecInclusive::selectTracksFromElectrons() { 
    
    const xAOD::ElectronContainer *electrons( nullptr );
    ATH_CHECK( evtStore()->retrieve( electrons, "Electrons" ) );
    
    static SG::AuxElement::Decorator< char > decor_isSelected( "is_selected" );
    
    for( const auto& electron : *electrons ) {
      if( 0 == electron->nTrackParticles() ) continue;
      
      // The first track is the best-matched track
      const auto* trk = electron->trackParticle(0);
      if( trk ) {
        decor_isSelected( *trk ) = true;
        m_selectedTracks->emplace_back( trk );
      }
    }
    
    return StatusCode::SUCCESS;
  }
  
} // end of namespace VKalVrtAthena
