/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthRelatedMuonPlots.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

TruthRelatedMuonPlots::TruthRelatedMuonPlots(PlotBase* pParent, std::string sDir,bool doBinnedResolutionPlots, bool doMuonTree):
    PlotBase(pParent, sDir),
    // truth related information
    m_doMuonTree(doMuonTree),

    m_oMatchedPlots(this, "/kinematics/", "Matched Muons"),
    
    m_oMSHitDiffPlots(this,"/hits/"),
    m_oMuonHitDiffSummaryPlots(this, "/hits/"),

    m_oMuonResolutionPlots(this, "/resolution/","",doBinnedResolutionPlots),
    m_oMuonMSResolutionPlots(this, "/resolution/","MS",doBinnedResolutionPlots),
    m_oMuonIDResolutionPlots(this, "/resolution/","ID",doBinnedResolutionPlots),
    
    m_oDefParamPullPlots(this, "/Pulls/",""),
    m_oMSDefParamPullPlots(this, "/Pulls/","MS"),
    m_oIDDefParamPullPlots(this, "/Pulls/","ID"),

    m_oMomentumTruthPullPlots_Tail(this,"/momentumPulls/","Tail"),
    m_oMomentumTruthPullPlots_NoTail(this,"/momentumPulls/","NoTail"),

    m_oMuonTree(NULL)
{
  if (m_doMuonTree) {
    m_oMuonTree = new Muon::MuonTree(this,"/");
  }
}

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

    float eloss=0;
    if (mu.parameter(eloss,xAOD::Muon::EnergyLoss)) {
      if ( mu.energyLossType()!=xAOD::Muon::Tail ) { //to test MEASURED energy loss
    	m_oMomentumTruthPullPlots_NoTail.fill(mu, msTrkIP, truthMu);
      }
      else {
    	m_oMomentumTruthPullPlots_Tail.fill(mu, msTrkIP, truthMu); //to test PARAMETRIZED energy loss
      }
    }

    if (m_doMuonTree && m_oMuonTree) {
      m_oMuonTree->fillTree(mu, msTrkIP, truthMu);
    }
}
