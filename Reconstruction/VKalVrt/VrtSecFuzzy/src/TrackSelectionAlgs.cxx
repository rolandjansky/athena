/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecFuzzy/VrtSecFuzzy.h"
#include "VrtSecFuzzy/Tools.h"

#include <iostream>

#include "TH1D.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"

//-------------------------------------------------

using namespace std;

namespace VKalVrtAthena {
  

  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::selectTrack_d0Cut       ( const xAOD::TrackParticle* trk ) const { return ( std::abs( trk->d0() ) > m_jp.d0TrkPVDstMinCut && std::abs( trk->d0() ) < m_jp.d0TrkPVDstMaxCut ); }
  bool VrtSecFuzzy::selectTrack_z0Cut       ( const xAOD::TrackParticle* trk ) const { return ( std::abs( trk->z0() ) > m_jp.z0TrkPVDstMinCut && std::abs( trk->z0() ) < m_jp.z0TrkPVDstMaxCut ); }
  bool VrtSecFuzzy::selectTrack_pTCut       ( const xAOD::TrackParticle* trk ) const { return trk->pt() > m_jp.TrkPtCut; }
  bool VrtSecFuzzy::selectTrack_chi2Cut     ( const xAOD::TrackParticle* trk ) const { return trk->chiSquared() / (trk->numberDoF()+AlgConsts::infinitesimal) < m_jp.TrkChi2Cut; }

  //____________________________________________________________________________________________________
  bool VrtSecFuzzy::selectTrack_notPVassociated  ( const xAOD::TrackParticle* trk ) const {
      return !( VKalVrtAthena::isAssociatedToVertices( trk, m_primaryVertices ) );
  }
  
  bool VrtSecFuzzy::selectTrack_hitAndPt( const xAOD::TrackParticle* trk ) const {
    uint8_t IBLHits = 0;
    
    if( !(trk->summaryValue( IBLHits, xAOD::numberOfInnermostPixelLayerHits        ) ) ) IBLHits =0;
    
    if( trk->pt() > 1000.0) return true;
    if( IBLHits > 0) return false;
    
    return true;
  }
  
  bool VrtSecFuzzy::selectTrack_nSiHitsCut( const xAOD::TrackParticle* trk ) const {
    
    uint8_t PixelHits = 0;
    uint8_t SCTHits   = 0; 
    
    if( !(trk->summaryValue( PixelHits, xAOD::numberOfPixelHits               ) ) ) PixelHits =0;
    if( !(trk->summaryValue( SCTHits,   xAOD::numberOfSCTHits                 ) ) ) SCTHits   =0;
    
    uint8_t SiHits = PixelHits + SCTHits;

    if(SiHits < 2)           return false;
    
    return true;
  }
 
  //____________________________________________________________________________________________________
  void VrtSecFuzzy::storeSelectedTracks( const xAOD::TrackParticle* trk ) {
    
    if( !m_decor_isSelected ){
      m_decor_isSelected = std::make_unique< SG::AuxElement::Decorator< char > >( "is_selected_fuzzy" );
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": m_decor_isSelected is set now");
    }

    // Setup cut functions
    if( 0 == m_trackSelectionFuncs.size() ) {
      
      // These cuts are optional. Specified by JobProperty
      if( m_jp.do_PVvetoCut )   m_trackSelectionFuncs.emplace_back( &VrtSecFuzzy::selectTrack_notPVassociated );
      if( m_jp.do_d0Cut )       m_trackSelectionFuncs.emplace_back( &VrtSecFuzzy::selectTrack_d0Cut );
      if( m_jp.do_z0Cut )       m_trackSelectionFuncs.emplace_back( &VrtSecFuzzy::selectTrack_z0Cut );
      if( m_jp.do_trkChi2Cut )  m_trackSelectionFuncs.emplace_back( &VrtSecFuzzy::selectTrack_chi2Cut );

      // These cuts are used by default
      m_trackSelectionFuncs.emplace_back( &VrtSecFuzzy::selectTrack_pTCut ); 
      m_trackSelectionFuncs.emplace_back( &VrtSecFuzzy::selectTrack_nSiHitsCut ); 
 
    }
    
    if(  std::find( m_selectedTracks->begin(), m_selectedTracks->end(), trk ) != m_selectedTracks->end() ) return;

    std::vector<bool> cutBits;
    
    for( auto func : m_trackSelectionFuncs ) cutBits.emplace_back( (this->*func)( trk ) );
      
    // Good track should not find any false bit
    bool isGood_standard = ( std::find( cutBits.begin(), cutBits.end(), false ) == cutBits.end() );
      
    if( isGood_standard ) {
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": no false bit" );
        
      // Store the selected track to the new m_selectedTracks
      // Here we firstly need to register the empty pointer to the m_selectedTracks,
      // then need to do deep copy after then. This is the feature of xAOD.
      
      (*m_decor_isSelected)( *trk ) = true; 
      
      m_selectedTracks->emplace_back( trk );
      
      
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Track index " << trk->index() << " has been selected." );
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Track index " << trk->index()
                       << " parameter:"
                       << " pt = "  << trk->pt()
                       << " eta = " << trk->eta()
                       << " d0 = "  << trk->d0()
                       << " z0 = "  << trk->z0() << "." );
        
    }
      
  }
  
  //____________________________________________________________________________________________________
  StatusCode  VrtSecFuzzy::selectInDetTracks() { 
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": begin"  );
    
    //--------------------------------------------------------
    //  Extract tracks from xAOD::TrackParticle container
    //
    
    const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
    ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_jp.TrackLocation) );
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Extracted xAOD::TrackParticle number=" << trackParticleContainer->size() );
    
    // Loop over tracks
    for( auto *trk : *trackParticleContainer ) { storeSelectedTracks( trk ); }
    
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of total ID tracks   = " << trackParticleContainer->size() );
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of selected tracks   = " << m_selectedTracks->size() );
    
    return StatusCode::SUCCESS;
  }

  
} // end of namespace VKalVrtAthena
