/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthRelatedMuonPlots.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

typedef ElementLink< xAOD::TruthParticleContainer > TruthLink;

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

    //muon spectrometer track at MS entry (not extrapolated)
    const xAOD::TrackParticle *msTrk(0);  
    //muon extrapolated to IP
    const xAOD::TrackParticle* msExtrapTrk(0);
    
    ////////////////// @@@ sorting out the mess with the link to the extrapolated muon
    //for 20.1.0...
    /// const xAOD::TrackParticle* msExtrapTrk = mu.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle); // points to the ExtrapolatedMuonSpectrometerTrackParticle, the ExtrapolatedMuonSpectrometerTrackParticle link doesn't exist
    //for 20.1.3...
    //const xAOD::TrackParticle* msExtrapTrk = mu.trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);

    //trying to accomodate both in a way that the code compiles in both releases
    if (mu.isAvailable< ElementLink<xAOD::TrackParticleContainer> >("extrapolatedMuonSpectrometerTrackParticleLink") && (mu.auxdata<ElementLink<xAOD::TrackParticleContainer> >("extrapolatedMuonSpectrometerTrackParticleLink")).isValid() ) {
      //cool, we got both links:
      int correctEnumForExtrap = ((int)xAOD::Muon::MuonSpectrometerTrackParticle)+2;
      msExtrapTrk = mu.trackParticle((xAOD::Muon::TrackParticleType) correctEnumForExtrap);
      msTrk = mu.trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );
    }
    else {
      // gymnastics to get msTrk...
      ElementLink<xAOD::TrackParticleContainer> msExtrapTrkLink = mu.trackParticleLink(xAOD::Muon::MuonSpectrometerTrackParticle);      
      if (msExtrapTrkLink.isValid()) {
	msExtrapTrk = mu.trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );
	TruthLink truthLink_muTrk;
	if( (*msExtrapTrkLink)->isAvailable<TruthLink>("truthParticleLink") ) {
	  truthLink_muTrk = (*msExtrapTrkLink)->auxdata<TruthLink>("truthParticleLink");
	}
	if (truthLink_muTrk.isValid()) {
	  for (const auto trk: *MSTracks) {
	    TruthLink truthLink_msTrk;
	    if( trk->isAvailable<TruthLink>("truthParticleLink") ) {
	      truthLink_msTrk = trk->auxdata<TruthLink>("truthParticleLink");
	      if (truthLink_msTrk.isValid()) {
		if (truthLink_msTrk == truthLink_muTrk) {
		  msTrk = trk; //got it!
		  break;
		}
	      }
	    }
	  }
	}
      }
    }
    
    if (msExtrapTrk) {
      m_oMuonMSResolutionPlots.fill( *msExtrapTrk, truthMu);
      m_oMSDefParamPullPlots.fill( *msExtrapTrk, truthMu);
    }

 
    float eloss=0; 
    if (mu.parameter(eloss,xAOD::Muon::EnergyLoss)) { 
      if ( mu.energyLossType()!=xAOD::Muon::Tail ) { //to test MEASURED energy loss 
        m_oMomentumTruthPullPlots_NoTail.fill(mu, msTrk, truthMu); 
      } 
      else { 
        m_oMomentumTruthPullPlots_Tail.fill(mu, msTrk, truthMu); //to test PARAMETRIZED energy loss 
      } 
    } 
 
    if (m_doMuonTree && m_oMuonTree) { 
      m_oMuonTree->fillTree(mu, msTrk, truthMu); 
    } 
}
