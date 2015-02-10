/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthRelatedMuonPlots.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

TruthRelatedMuonPlots::TruthRelatedMuonPlots(PlotBase* pParent, std::string sDir,bool doBinnedResolutionPlots):
    PlotBase(pParent, sDir),
    // truth related information
    m_oMatchedPlots(this, "/", "Matched Muons"),
    m_oMSHitDiffPlots(this,"/"),
    m_oMuonHitDiffSummaryPlots(this, "/"),

    m_oMuonResolutionPlots(this, "/resolution/",doBinnedResolutionPlots),
    m_oMuonMSResolutionPlots(this, "/resolutionMS/",doBinnedResolutionPlots),
    m_oMuonIDResolutionPlots(this, "/resolutionID/",doBinnedResolutionPlots),
    m_oDefParamPullPlots(this, "/Pulls/"),
    m_oMSDefParamPullPlots(this, "/PullsMS/"),
    m_oIDDefParamPullPlots(this, "/PullsID/"),

    m_oMomentumDiffPlots(this,"/resolution/")
{}

TruthRelatedMuonPlots::~TruthRelatedMuonPlots() {}

void TruthRelatedMuonPlots::fill(const xAOD::TruthParticle& truthMu, const xAOD::Muon& mu, const xAOD::TrackParticleContainer* MSTracks){
    m_oMatchedPlots.fill( truthMu );
    m_oMuonHitDiffSummaryPlots.fill(mu, truthMu);

    // Tracking related plots
    const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
    if (!primaryTrk) return;
    m_oMSHitDiffPlots.fill(*primaryTrk, truthMu);
    m_oMuonResolutionPlots.fill(*primaryTrk, truthMu);
    m_oDefParamPullPlots.fill(*primaryTrk, truthMu);

    const xAOD::TrackParticle* inDetTrk = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    if (inDetTrk) {
        m_oMuonIDResolutionPlots.fill(*inDetTrk,truthMu);
        m_oIDDefParamPullPlots.fill(*inDetTrk, truthMu);
    }

    const xAOD::TrackParticle* msTrk = mu.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
    if (msTrk) {
        m_oMuonMSResolutionPlots.fill(*msTrk,truthMu);
        m_oMSDefParamPullPlots.fill(*msTrk, truthMu);
    }


    const xAOD::TrackParticle *msTrkIP(0);    
    //if (mu) {
    typedef ElementLink< xAOD::TruthParticleContainer > TruthLink;

    ElementLink<xAOD::TrackParticleContainer> muTrk = mu.muonSpectrometerTrackParticleLink();
    if (muTrk.isValid()) {
      TruthLink truthLink_muTrk;
      if( (*muTrk)->isAvailable<TruthLink>("truthParticleLink") ) {
	truthLink_muTrk = (*muTrk)->auxdata<TruthLink>("truthParticleLink");
      }
      if (truthLink_muTrk.isValid()) {
	for (const auto trk: *MSTracks) {
	  TruthLink truthLink_msTrk;
	  if( trk->isAvailable<TruthLink>("truthParticleLink") ) {
	    truthLink_msTrk = trk->auxdata<TruthLink>("truthParticleLink");
	    if (truthLink_msTrk.isValid()) {
	      if (truthLink_msTrk == truthLink_muTrk) {
		msTrkIP = trk;
		break;
	      }
	    }
	  }
	}
      }
    }
    // }  
    m_oMomentumDiffPlots.fill(mu, msTrkIP, truthMu);
    
}
