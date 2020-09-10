/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauCommonCalcVars.cxx
// package:     Reconstruction/tauRec
// authors:     Stan Lai
// date:        2008-05-18
// 
// This class calculates tau variables after core seed reconstruction           
//-----------------------------------------------------------------------------
#include "tauRecTools/TauCommonCalcVars.h"
#include "tauRecTools/KineUtils.h"
#include <vector>

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TauCommonCalcVars::TauCommonCalcVars(const std::string &name) :
  TauRecToolBase(name) {
  //if TauTrackClassifier is not run, wide&passTrkSelector==classifiedIsolation==modifiedIsolationTrack
  declareProperty("isolationTrackType", m_isolationTrackType=xAOD::TauJetParameters::modifiedIsolationTrack);
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------

TauCommonCalcVars::~TauCommonCalcVars() {
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauCommonCalcVars::execute(xAOD::TauJet& pTau) const {

  /////////////////////////////////////////////////
  // Calculate variables that are always valid   
  ////////////////////////////////////////////////

  //init some vars
  pTau.setDetail( xAOD::TauJetParameters::SumPtTrkFrac, (float) 0. );

  // Leading track pT and et/pt(lead track)
  if (pTau.nTracks() > 0) {
    pTau.setDetail( xAOD::TauJetParameters::leadTrkPt, static_cast<float>( pTau.track(0)->pt() ) );
      
    float emscale_ptEM = 0.;
    float emscale_ptHad = 0.;
      
    if ( !pTau.detail( xAOD::TauJetParameters::etEMAtEMScale, emscale_ptEM ) ) 
      {
	ATH_MSG_DEBUG("retrieval of tau detail failed. stopping calculation of further variables");
	return StatusCode::SUCCESS;
      }

    if ( !pTau.detail( xAOD::TauJetParameters::etHadAtEMScale, emscale_ptHad ) )
      {
	ATH_MSG_DEBUG("retrieval of tau detail failed. stopping calculation of further variables");
	return StatusCode::SUCCESS;
      }
      
    pTau.setDetail( xAOD::TauJetParameters::etOverPtLeadTrk, static_cast<float>( (emscale_ptEM + emscale_ptHad) / pTau.track(0)->pt() ) );
  }

  // invariant mass of track system
  std::vector<const xAOD::TauTrack*> tauTracks = pTau.tracks(xAOD::TauJetParameters::TauTrackFlag::classifiedCharged);
  for( const xAOD::TauTrack* trk : pTau.tracks((xAOD::TauJetParameters::TauTrackFlag) m_isolationTrackType) ) tauTracks.push_back(trk);
  if (tauTracks.size()> 0) {

    TLorentzVector sumOfTrackVector;

    for (const xAOD::TauTrack* tauTrk : tauTracks)
      {
	sumOfTrackVector += tauTrk->p4();
      }
    pTau.setDetail( xAOD::TauJetParameters::massTrkSys, static_cast<float>( sumOfTrackVector.M() ) );
  }

  if (tauTracks.size()> 0 && pTau.nTracks()>1) {
    // BUG?
    // this is equivalent to: if (pTau.nTracks()>1)
    // shouldn't it be: if (tauTracks.size()> 1 && pTau.nTracks()>0) ?
    // currently, 1-prong taus are skipped

    double ptSum = 0.;
    double sumWeightedDR = 0.;
    double sumWeightedDR2 = 0.;

    for (const xAOD::TauTrack* tauTrk : tauTracks) {

      double deltaR = pTau.track(0)->p4().DeltaR(tauTrk->p4());

      ptSum += tauTrk->pt();
      sumWeightedDR += deltaR * tauTrk->pt();
      sumWeightedDR2 += deltaR * deltaR * tauTrk->pt();
    }

    double trkWidth2 = (ptSum!=0.) ? (sumWeightedDR2/ptSum - std::pow(sumWeightedDR/ptSum, 2.)) : 0.;

    if (trkWidth2 > 0.) pTau.setDetail( xAOD::TauJetParameters::trkWidth2, static_cast<float>( trkWidth2 ) );
    else pTau.setDetail( xAOD::TauJetParameters::trkWidth2, (float) 0. );
  }

  if (tauTracks.size()> 0) {

    double ptSum = 0;
    double innerPtSum = 0;
    double sumWeightedDR = 0;
    double innerSumWeightedDR = 0;
    double sumWeightedDR2 = 0;

    for (const xAOD::TauTrack* tauTrk : tauTracks){
      // BUG! should consistently use phiIntermediateAxis - will be fixed in a separate MR
      double deltaR = Tau1P3PKineUtils::deltaR( ( inTrigger() ? pTau.eta() : pTau.etaIntermediateAxis()), pTau.phi(), tauTrk->eta(), tauTrk->phi() );     
      // will become:
      // double deltaR = inTrigger() ? pTau.p4().DeltaR(tauTrk->p4()) : pTau.p4(xAOD::TauJetParameters::IntermediateAxis).DeltaR(tauTrk->p4()); 
      
      ptSum += tauTrk->pt();
      sumWeightedDR += deltaR * tauTrk->pt();
      sumWeightedDR2 += deltaR * deltaR * tauTrk->pt();

      //add calculation of innerTrkAvgDist
      if(tauTrk->flag(xAOD::TauJetParameters::TauTrackFlag::classifiedCharged)){
	innerPtSum += tauTrk->pt();
	innerSumWeightedDR += deltaR * tauTrk->pt();
      }
    }

    if (ptSum > 0.) {
      // seedCalo_trkAvgDist
      pTau.setDetail( xAOD::TauJetParameters::trkAvgDist, static_cast<float>( sumWeightedDR / ptSum ) );

      // seedCalo_trkRmsDist
      double trkRmsDist2 = sumWeightedDR2 / ptSum - pow(sumWeightedDR/ptSum, 2.);
      if (trkRmsDist2 > 0.) {
	pTau.setDetail( xAOD::TauJetParameters::trkRmsDist, static_cast<float>( std::sqrt(trkRmsDist2) ) );
      } 
      else {
	pTau.setDetail( xAOD::TauJetParameters::trkRmsDist, (float) 0. );
      }

      // SumPtTrkFrac
      pTau.setDetail( xAOD::TauJetParameters::SumPtTrkFrac, static_cast<float>( 1. - innerPtSum/ptSum ) );
    }
    else {
      pTau.setDetail( xAOD::TauJetParameters::trkAvgDist, (float) 0. );
      pTau.setDetail( xAOD::TauJetParameters::SumPtTrkFrac, (float) 0. );
    }

    if (innerPtSum > 0.) {	   	   
      // InnerTrkAvgDist
      pTau.setDetail( xAOD::TauJetParameters::innerTrkAvgDist, static_cast<float>( innerSumWeightedDR / innerPtSum ) );
    }
    else {
      pTau.setDetail( xAOD::TauJetParameters::innerTrkAvgDist, (float) 0. );
    }

  }

  return StatusCode::SUCCESS;
}
