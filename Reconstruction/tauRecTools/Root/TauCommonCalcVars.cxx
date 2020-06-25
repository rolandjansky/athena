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
//
// 17/03/2010: (AK) change to P4Helpers
// 16/05/2011: (FF) fix if primaryVertexContainer==NULL (coverity 21734)
//   Dez 2011: (FF) switch to full LC calibrated tau 4-vector for some variables
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
// Initializer
//-----------------------------------------------------------------------------

StatusCode TauCommonCalcVars::initialize() {
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------

StatusCode TauCommonCalcVars::finalize() {
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauCommonCalcVars::execute(xAOD::TauJet& pTau) const {

    /////////////////////////////////////////////////
    // Calculate variables that are always valid   
    ////////////////////////////////////////////////

    //init some vars
    pTau.setDetail( xAOD::TauJetParameters::SumPtTrkFrac, static_cast<float>( 0 ) );

    // Leading track pT and et/pt(lead track)
    if (pTau.nTracks() > 0) {
      pTau.setDetail( xAOD::TauJetParameters::leadTrkPt, static_cast<float>( pTau.track(0)->pt() ) );
      
      float emscale_ptEM = 0;
      float emscale_ptHad = 0;
      
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
	TLorentzVector tempTrackVector;

        for (const xAOD::TauTrack* tauTrk : tauTracks)
	  {
	    tempTrackVector=tauTrk->p4();
            sumOfTrackVector += tempTrackVector;
	  }
	pTau.setDetail( xAOD::TauJetParameters::massTrkSys, static_cast<float>( sumOfTrackVector.M() ) );
	float tempfloat = 0;
	if ( pTau.detail( xAOD::TauJetParameters::massTrkSys, tempfloat ) )
	  ATH_MSG_VERBOSE("set massTrkSys " << tempfloat);
    }

    if (tauTracks.size()> 0 && pTau.nTracks()>1) {

      double leadTrkPhi = pTau.track(0)->phi();//fix this depending on how we choose to define this
      double leadTrkEta = pTau.track(0)->eta();//dito. new TauJet_v3 track sorting doesn't guarantee this will be the same track

        double ptSum = 0;
        double sumWeightedDR = 0;
        double sumWeightedDR2 = 0;


        for (const xAOD::TauTrack* tauTrk : tauTracks){

	  double deltaR = Tau1P3PKineUtils::deltaR(leadTrkEta, leadTrkPhi, tauTrk->eta(), tauTrk->phi() );     

	  ptSum += tauTrk->pt();
	  sumWeightedDR += deltaR * (tauTrk->pt());
	  sumWeightedDR2 += deltaR * deltaR * (tauTrk->pt());

        }

        double trkWidth2 = sumWeightedDR2 / ptSum - sumWeightedDR * sumWeightedDR / ptSum / ptSum;

        if (trkWidth2 > 0.) pTau.setDetail( xAOD::TauJetParameters::trkWidth2, static_cast<float>( trkWidth2 ) );
        else pTau.setDetail( xAOD::TauJetParameters::trkWidth2, static_cast<float>( 0. ) );
    }

    if (tauTracks.size()> 0) {

        double ptSum = 0;
        double innerPtSum = 0;
        double sumWeightedDR = 0;
        double innerSumWeightedDR = 0;
        double sumWeightedDR2 = 0;


        for (const xAOD::TauTrack* tauTrk : tauTracks){

          double deltaR = Tau1P3PKineUtils::deltaR( ( inTrigger() ? pTau.eta() : pTau.etaIntermediateAxis()), pTau.phi(), tauTrk->eta(), tauTrk->phi() );     
	
	  ptSum += tauTrk->pt();
	  sumWeightedDR += deltaR * (tauTrk->pt());
	  sumWeightedDR2 += deltaR * deltaR * (tauTrk->pt());

	  //add calculation of innerTrkAvgDist
	  if(tauTrk->flag(xAOD::TauJetParameters::TauTrackFlag::classifiedCharged)){
	    innerPtSum += tauTrk->pt();
	    innerSumWeightedDR += deltaR * (tauTrk->pt());
	  }
        }

        if (ptSum > 0.0001) {
	  // seedCalo_trkAvgDist
	  pTau.setDetail( xAOD::TauJetParameters::trkAvgDist, static_cast<float>( sumWeightedDR / ptSum ) );

	  float tempfloat;
	  if ( pTau.detail( xAOD::TauJetParameters::trkAvgDist, tempfloat ) )
	    ATH_MSG_VERBOSE("set seedCalo_trkAvgDist " << tempfloat );

	  // seedCalo_trkRmsDist
	  double trkRmsDist2 = sumWeightedDR2 / ptSum - sumWeightedDR * sumWeightedDR / ptSum / ptSum;
	  if (trkRmsDist2 > 0) {
	    pTau.setDetail( xAOD::TauJetParameters::trkRmsDist, static_cast<float>( sqrt(trkRmsDist2) ) );
	  } else {
	    pTau.setDetail( xAOD::TauJetParameters::trkRmsDist, static_cast<float>( 0. ) );
	  }
	  if ( pTau.detail( xAOD::TauJetParameters::trkRmsDist, tempfloat ) )
	    ATH_MSG_VERBOSE("set seedCalo_trkRmsDist " << tempfloat );
        }

         if (ptSum > 0.0) {
	   
     	  // SumPtTrkFrac
     	  pTau.setDetail( xAOD::TauJetParameters::SumPtTrkFrac, static_cast<float>( 1. - innerPtSum/ptSum ) );
     	  // FIXME!!! put pileup correction here once availabe
     	  // FIXME!!! for now set corrected version same as uncorrected
     	  pTau.setDetail( xAOD::TauJetParameters::SumPtTrkFracCorrected, static_cast<float>( 1. - innerPtSum/ptSum ) );
      
       	 } else {

	   pTau.setDetail( xAOD::TauJetParameters::SumPtTrkFrac, static_cast<float>( 0.0 ) );
	   pTau.setDetail( xAOD::TauJetParameters::SumPtTrkFracCorrected, static_cast<float>( 0.0 ) );

	 }

	 if (ptSum > 0.00 && innerPtSum > 0.0) {
	   
	   ATH_MSG_DEBUG("innerSumWeightedDR = " << innerSumWeightedDR << " innerPtSum = " << innerPtSum << " ptSum = " << ptSum );
	   
     	   // InnerTrkAvgDist
     	   pTau.setDetail( xAOD::TauJetParameters::innerTrkAvgDist, static_cast<float>( innerSumWeightedDR / innerPtSum ) );
     	   // FIXME!!! put pileup correction here once availabe
     	   // FIXME!!! for now set corrected version same as uncorrected
     	   pTau.setDetail( xAOD::TauJetParameters::innerTrkAvgDistCorrected, static_cast<float>( innerSumWeightedDR / innerPtSum ) );
	   
	 } else {
     	   pTau.setDetail( xAOD::TauJetParameters::innerTrkAvgDist, static_cast<float>( 0.0 ));
     	   pTau.setDetail( xAOD::TauJetParameters::innerTrkAvgDistCorrected, static_cast<float>( 0.0 ));
	 }

    }

    return StatusCode::SUCCESS;
}


