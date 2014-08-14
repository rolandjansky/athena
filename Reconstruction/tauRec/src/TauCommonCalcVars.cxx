/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
//TODO: rename

#include <GaudiKernel/IToolSvc.h>
#include <GaudiKernel/ListItem.h>

#include "tauRec/TauCandidateData.h"

#include "tauRec/TauCommonCalcVars.h"
#include "tauRec/KineUtils.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TauCommonCalcVars::TauCommonCalcVars(const std::string &type,
    const std::string &name,
    const IInterface *parent) :
TauToolBase(type, name, parent) {
    declareInterface<TauToolBase > (this);
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
StatusCode TauCommonCalcVars::execute(TauCandidateData *data) {

    xAOD::TauJet *pTau = data->xAODTau;

    if (pTau == NULL) {
        ATH_MSG_ERROR("no candidate given");
        return StatusCode::FAILURE;
    }

    /////////////////////////////////////////////////
    // Calculate variables that are always valid   
    ////////////////////////////////////////////////

    // Leading track pT and et/pt(lead track)
    if (pTau->nTracks() > 0) {
      pTau->setDetail( xAOD::TauJetParameters::leadTrkPt, static_cast<float>( pTau->track(0)->pt() ) );
      
      float emscale_ptEM = 0;
      float emscale_ptHad = 0;
      
      if ( !pTau->detail( xAOD::TauJetParameters::etEMAtEMScale, emscale_ptEM ) ) 
	{
	  ATH_MSG_DEBUG("retrieval of tau detail failed. stopping calculation of further variables");
	  return StatusCode::SUCCESS;
	}

      if ( !pTau->detail( xAOD::TauJetParameters::etHadAtEMScale, emscale_ptHad ) )
	{
	  ATH_MSG_DEBUG("retrieval of tau detail failed. stopping calculation of further variables");
	  return StatusCode::SUCCESS;
	}
      
      //EM scale

      pTau->setDetail( xAOD::TauJetParameters::etOverPtLeadTrk, static_cast<float>( (emscale_ptEM + emscale_ptHad) / pTau->track(0)->pt() ) );

        //switch to LC energy scale
        //pDetails->setEtOverPtLeadTrk(pDetails->LC_TES_precalib() / fabs(pTau->track(0)->pt()));
    }

    // XXX still need to decide whether we want to fill loose track variables anymore
    // // Leading loose track pT and et/pt(lead loose track)
    // if (pDetails->nLooseTrk() > 0) {
    //     pDetails->setLeadLooseTrkPt(pDetails->looseTrk(0)->pt());
    //     //EM scale
    //     pDetails->setEtOverPtLeadLooseTrk((pDetails->seedCalo_etHadCalib() + pDetails->seedCalo_etEMCalib()) / fabs(pDetails->looseTrk(0)->pt()));
    //     //LC scale
    //     //pDetails->setEtOverPtLeadLooseTrk(pDetails->LC_TES_precalib() / fabs(pDetails->looseTrk(0)->pt()));
    // }

    // invariant mass of track system
    if ((pTau->nTracks() + pTau->nWideTracks()) > 0) {

        TLorentzVector sumOfTrackVector;
	TLorentzVector tempTrackVector;

        for (unsigned int i = 0; i != pTau->nTracks(); ++i)
	  {
	    tempTrackVector.SetPtEtaPhiM( pTau->track(i)->pt(),  pTau->track(i)->eta(),  pTau->track(i)->phi(),  pTau->track(i)->m());
            sumOfTrackVector += tempTrackVector;
	  }
        for (unsigned int i = 0; i != pTau->nWideTracks(); ++i)
	  {
	    tempTrackVector.SetPtEtaPhiM( pTau->wideTrack(i)->pt(),  pTau->wideTrack(i)->eta(),  pTau->wideTrack(i)->phi(),  pTau->wideTrack(i)->m());
            sumOfTrackVector += tempTrackVector;
	  }

	pTau->setDetail( xAOD::TauJetParameters::massTrkSys, static_cast<float>( sumOfTrackVector.M() ) );
	float tempfloat = 0;
	if ( pTau->detail( xAOD::TauJetParameters::massTrkSys, tempfloat ) )
		ATH_MSG_VERBOSE("set massTrkSys " << tempfloat);
    }

    // width of track system squared (trkWidth2)
    if (pTau->nTracks() > 1) {

        double leadTrkPhi = pTau->track(0)->phi();
        double leadTrkEta = pTau->track(0)->eta();

        double ptSum = 0;
        double sumWeightedDR = 0;
        double sumWeightedDR2 = 0;

        for (unsigned int i = 0; i != pTau->nTracks(); ++i) {

	  double deltaR = Tau1P3PKineUtils::deltaR(leadTrkEta, leadTrkPhi, pTau->track(i)->eta(), pTau->track(i)->phi() );     

	  ptSum += pTau->track(i)->pt();
	  sumWeightedDR += deltaR * (pTau->track(i)->pt());
	  sumWeightedDR2 += deltaR * deltaR * (pTau->track(i)->pt());
        }

        for (unsigned int i = 0; i != pTau->nWideTracks(); ++i) {

	  double deltaR = Tau1P3PKineUtils::deltaR(leadTrkEta, leadTrkPhi, pTau->wideTrack(i)->eta(), pTau->wideTrack(i)->phi() );     
	  
	  ptSum += pTau->wideTrack(i)->pt();
	  sumWeightedDR += deltaR * (pTau->wideTrack(i)->pt());
	  sumWeightedDR2 += deltaR * deltaR * (pTau->wideTrack(i)->pt());
        }

        double trkWidth2 = sumWeightedDR2 / ptSum - sumWeightedDR * sumWeightedDR / ptSum / ptSum;

        if (trkWidth2 > 0.) pTau->setDetail( xAOD::TauJetParameters::trkWidth2, static_cast<float>( trkWidth2 ) );
        else pTau->setDetail( xAOD::TauJetParameters::trkWidth2, static_cast<float>( 0. ) );
    }

    // Calculation for seedCalo_trkAvgDist and seedCalo_trkRmsDist
    
    //FF: use now the 4-vector of the tau intermediate axis
    //P4EEtaPhiM P4CaloSeed(1., pDetails->seedCalo_eta(), pDetails->seedCalo_phi(), 0.);

    if ((pTau->nWideTracks() + pTau->nTracks()) > 0) {

        double ptSum = 0;
        double sumWeightedDR = 0;
        double sumWeightedDR2 = 0;

        for (unsigned int i = 0; i != pTau->nTracks(); ++i) {

          double deltaR = Tau1P3PKineUtils::deltaR(pTau->eta(), pTau->phi(), pTau->track(i)->eta(), pTau->track(i)->phi() );     
	
	  ptSum += pTau->track(i)->pt();
	  sumWeightedDR += deltaR * (pTau->track(i)->pt());
	  sumWeightedDR2 += deltaR * deltaR * (pTau->track(i)->pt());
        }

        for (unsigned int i = 0; i != pTau->nWideTracks(); ++i) {

          double deltaR = Tau1P3PKineUtils::deltaR(pTau->eta(), pTau->phi(), pTau->wideTrack(i)->eta(), pTau->wideTrack(i)->phi() );     
          
	  ptSum += pTau->wideTrack(i)->pt();
	  sumWeightedDR += deltaR * (pTau->wideTrack(i)->pt());
	  sumWeightedDR2 += deltaR * deltaR * (pTau->wideTrack(i)->pt());
        }

        if (ptSum > 0.0001) {
	  // seedCalo_trkAvgDist
	  pTau->setDetail( xAOD::TauJetParameters::trkAvgDist, static_cast<float>( sumWeightedDR / ptSum ) );

	  float tempfloat;
	  if ( pTau->detail( xAOD::TauJetParameters::trkAvgDist, tempfloat ) )
	    ATH_MSG_VERBOSE("set seedCalo_trkAvgDist " << tempfloat );

	  // seedCalo_trkRmsDist
	  double trkRmsDist2 = sumWeightedDR2 / ptSum - sumWeightedDR * sumWeightedDR / ptSum / ptSum;
	  if (trkRmsDist2 > 0) {
	    pTau->setDetail( xAOD::TauJetParameters::trkRmsDist, static_cast<float>( sqrt(trkRmsDist2) ) );
	  } else {
	    pTau->setDetail( xAOD::TauJetParameters::trkRmsDist, static_cast<float>( 0. ) );
	  }
	  if ( pTau->detail( xAOD::TauJetParameters::trkRmsDist, tempfloat ) )
	    ATH_MSG_VERBOSE("set seedCalo_trkRmsDist " << tempfloat );
        }
    }

    return StatusCode::SUCCESS;
}


