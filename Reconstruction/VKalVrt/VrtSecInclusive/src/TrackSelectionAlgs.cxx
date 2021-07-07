/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
#include "VrtSecInclusive/NtupleVars.h"
#include "VrtSecInclusive/Tools.h"

#include "xAODEgamma/ElectronxAODHelpers.h"

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
    uint8_t SCTHits   = 0; 
    uint8_t BLayHits  = 0;
    uint8_t PixShare  = 0;
    uint8_t SCTShare  = 0;
    uint8_t TRTHits   = 0;
    
    if( !(trk->summaryValue( PixelHits, xAOD::numberOfPixelHits               ) ) ) PixelHits =0;
    if( !(trk->summaryValue( SCTHits,   xAOD::numberOfSCTHits                 ) ) ) SCTHits   =0;
    if( !(trk->summaryValue( BLayHits,  xAOD::numberOfInnermostPixelLayerHits ) ) ) BLayHits  =0;
    if( !(trk->summaryValue( PixShare,  xAOD::numberOfPixelSharedHits         ) ) ) PixShare  =0;
    if( !(trk->summaryValue( SCTShare,  xAOD::numberOfSCTSharedHits           ) ) ) SCTShare  =0;
    if( !(trk->summaryValue( TRTHits,   xAOD::numberOfTRTHits                 ) ) ) TRTHits   =0;
    
    uint8_t SharedHits = PixShare + SCTShare;

    // do Pixel/SCT/SiHits only if we exclude StandAlone TRT hits
    if( !m_jp.SAloneTRT ) {
      if(PixelHits           < m_jp.CutPixelHits)  return false;
      if(SCTHits             < m_jp.CutSctHits)    return false;
      if((PixelHits+SCTHits) < m_jp.CutSiHits)     return false;
      if(BLayHits            < m_jp.CutBLayHits)   return false;
      if(SharedHits          > m_jp.CutSharedHits) return false;
    }

    // The folloing part reproduces the track selection in RPVDixpVrt
    if( m_jp.doTRTPixCut ) {
      if(TRTHits == 0 && PixelHits < 2)           return false;
    }
    
    if( PixelHits == 0 && SCTHits < 6 ) return false;
    
    return true;
  }
  
  //____________________________________________________________________________________________________
  bool VrtSecInclusive::selectTrack_hitPatternTight( const xAOD::TrackParticle* trk ) const {
    uint8_t PixelHits = 0;
    uint8_t SCTHits   = 0; 
    uint8_t TRTHits   = 0;
    
    if( !(trk->summaryValue( PixelHits, xAOD::numberOfPixelHits               ) ) ) PixelHits =0;
    if( !(trk->summaryValue( SCTHits,   xAOD::numberOfSCTHits                 ) ) ) SCTHits   =0;
    if( !(trk->summaryValue( TRTHits,   xAOD::numberOfTRTHits                 ) ) ) TRTHits   =0;
    
    if( trk->pt() > 20.e3 ) return true;
    
    if( SCTHits < m_jp.CutTightSCTHits                   ) return false;
    
    if( fabs( trk->eta() ) < 1.7 ) {
      if( TRTHits < m_jp.CutTightTRTHits ) return false;
    }
    
    return true;
  }
  
  //____________________________________________________________________________________________________
  bool VrtSecInclusive::selectTrack_notPVassociated  ( const xAOD::TrackParticle* trk ) const {
      return !( VKalVrtAthena::isAssociatedToVertices( trk, m_primaryVertices ) );
  }

  //____________________________________________________________________________________________________
  bool VrtSecInclusive::selectTrack_LRTR3Cut( const xAOD::TrackParticle* trk ) const {
      uint8_t npix = 0;
      trk->summaryValue(npix, xAOD::numberOfPixelHits);
      uint8_t nsct = 0;
      trk->summaryValue(nsct, xAOD::numberOfSCTHits);
      uint8_t nSiHits = npix + nsct ;
      uint8_t nSCTHoles=0;
      trk->summaryValue(nSCTHoles, xAOD::numberOfSCTHoles);

      double dTheta = std::fabs(TMath::ATan2(std::fabs(trk->d0()),trk->z0())-2*std::atan(std::exp(-1*trk->eta())));
      bool geometric_cut = dTheta < 1. || std::fabs(trk->z0()) < 200. ;

      bool z0_cut = trk->z0() <= 500. ;
      bool chi2_cut = (trk->chiSquared()/ (trk->numberDoF()+AlgConsts::infinitesimal)) <= 9. ;
      bool NSiHits_cut = nSiHits >=8 ;
      bool NSCTHits_cut = nsct >= 7 ;
      bool NSCTHoles_cut = nSCTHoles <= 1;

      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": z0_cut, chi2_cut, NSiHits_cut, NSCTHits_cut, NSCTHoles_cut  = " <<z0_cut<<", "<<chi2_cut<<", "<<NSiHits_cut<<", "<<NSCTHits_cut<<", "<< NSCTHoles_cut );
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": npix, nsct, nSiHits, nSCTHoles, dTheta, z0, d0, chi2  = " <<unsigned(npix)<<", "<<unsigned(nsct)<<", "<<unsigned(nSiHits)<<", "<<unsigned(nSCTHoles)<<", "<< dTheta<<", "<< trk->z0()<<", "<< trk->d0()<<", " <<trk->chiSquared() ) ;

      const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = trk->patternRecoInfo();
      bool isLRT = patternReco.test(49) ;
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Track is LRT  = " << isLRT ) ;
      if (isLRT) {  // apply all cuts to LRT tracks 
        return (z0_cut && chi2_cut && NSiHits_cut && NSCTHits_cut && NSCTHoles_cut && geometric_cut);
      }
      else{ // not LRT track; only apply SiHit cut
       return NSiHits_cut ;
     }
  }

  
  //____________________________________________________________________________________________________
  void VrtSecInclusive::selectTrack( const xAOD::TrackParticle* trk ) {
    
    if( !m_decor_isSelected ) m_decor_isSelected = std::make_unique< SG::AuxElement::Decorator< char > >( "is_selected" + m_jp.augVerString );
    
    // Setup cut functions
    if( 0 == m_trackSelectionFuncs.size() && !m_jp.passThroughTrackSelection ) {
      
      // These cuts are optional. Specified by JobProperty
      if( m_jp.do_PVvetoCut )             m_trackSelectionFuncs.emplace_back( &VrtSecInclusive::selectTrack_notPVassociated );
      if( m_jp.do_d0Cut )                 m_trackSelectionFuncs.emplace_back( &VrtSecInclusive::selectTrack_d0Cut );
      if( m_jp.do_z0Cut )                 m_trackSelectionFuncs.emplace_back( &VrtSecInclusive::selectTrack_z0Cut );
      if( m_jp.do_d0errCut )              m_trackSelectionFuncs.emplace_back( &VrtSecInclusive::selectTrack_d0errCut );
      if( m_jp.do_z0errCut )              m_trackSelectionFuncs.emplace_back( &VrtSecInclusive::selectTrack_z0errCut );
      if (m_jp.doSelectTracksWithLRTCuts) m_trackSelectionFuncs.emplace_back( &VrtSecInclusive::selectTrack_LRTR3Cut );
      //if( m_jp.do_d0signifCut ) m_trackSelectionFuncs.emplace_back( &VrtSecInclusive::selectTrack_d0signifCut ); // not implemented yet
      //if( m_jp.do_z0signifCut ) m_trackSelectionFuncs.emplace_back( &VrtSecInclusive::selectTrack_z0signifCut ); // not implemented yet
      
      // These cuts are used by default
      m_trackSelectionFuncs.emplace_back( &VrtSecInclusive::selectTrack_hitPattern );
      m_trackSelectionFuncs.emplace_back( &VrtSecInclusive::selectTrack_hitPatternTight );
      m_trackSelectionFuncs.emplace_back( &VrtSecInclusive::selectTrack_chi2Cut );
      m_trackSelectionFuncs.emplace_back( &VrtSecInclusive::selectTrack_pTCut );
      
    }
    
    if(  std::find( m_selectedTracks->begin(), m_selectedTracks->end(), trk ) != m_selectedTracks->end() ) return;
    
    std::vector<bool> cutBits;
    
    for( auto func : m_trackSelectionFuncs ) cutBits.emplace_back( (this->*func)( trk ) );
      
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
      
      (*m_decor_isSelected)( *trk ) = true;
      if (m_jp.doSelectTracksFromElectrons) {
        const xAOD::TrackParticle *id_tr;
        id_tr = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(trk);
        if (id_tr != nullptr){
        (*m_decor_isSelected)( *id_tr ) = true; }
    }
      
      m_selectedTracks->emplace_back( trk );
      
      if( m_jp.FillNtuple ) m_ntupleVars->get< vector<int> >( "SelTrk_barcode" ).emplace_back(barcode); // will need this later          
      
      ATH_MSG_VERBOSE( " > " << __FUNCTION__ << ": Track index " << trk->index() << " has been selected." );
      ATH_MSG_VERBOSE( " > " << __FUNCTION__ << ": Track index " << trk->index()
                       << " parameter:"
                       << " pt = "  << trk->pt()
                       << " eta = " << trk->eta()
                       << " d0 = "  << trk->d0()
                       << " z0 = "  << trk->z0() << "." );
        
    }
      
  }
  
  //____________________________________________________________________________________________________
  StatusCode  VrtSecInclusive::selectTracksInDet() { 
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": begin"  );
    
    //--------------------------------------------------------
    //  Extract tracks from xAOD::TrackParticle container
    //
    
    const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
    ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_jp.TrackLocation) );
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Extracted xAOD::TrackParticle number=" << trackParticleContainer->size() );
    
    if( m_jp.FillNtuple )
      m_ntupleVars->get<unsigned int>( "NumAllTrks" ) = static_cast<int>( trackParticleContainer->size() );
    
    
    // Loop over tracks
    for( auto *trk : *trackParticleContainer ) { selectTrack( trk ); }
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of total ID tracks   = " << trackParticleContainer->size() );
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of selected tracks   = " << m_selectedTracks->size() );
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode  VrtSecInclusive::selectTracksFromMuons() { 
    
    const xAOD::MuonContainer* muons ( nullptr );
    ATH_CHECK( evtStore()->retrieve( muons, "Muons") );
    
    
    for( const auto& muon : *muons ) {
      const auto* trk = muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
      
      if( !trk ) continue;
      // remove calo-tagged muons when selecting muons
      if (m_jp.doRemoveCaloTaggedMuons) {
        if (muon->muonType() == xAOD::Muon::CaloTagged) continue;
      }
      selectTrack( trk );
      
    }
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of total muons       = " << muons->size() );
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of selected tracks   = " << m_selectedTracks->size() );
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode  VrtSecInclusive::selectTracksFromElectrons() { 
    
    const xAOD::ElectronContainer *electrons( nullptr );
    ATH_CHECK( evtStore()->retrieve( electrons, "Electrons" ) );
    
    for( const auto& electron : *electrons ) {
      if( 0 == electron->nTrackParticles() ) continue;
      
      // The first track is the best-matched track
      const auto* trk = electron->trackParticle(0);
      
      if( !trk ) continue;
      selectTrack( trk );
    }
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of total electrons   = " << electrons->size() );
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of selected tracks   = " << m_selectedTracks->size() );
    
    return StatusCode::SUCCESS;
  }
  
} // end of namespace VKalVrtAthena
