/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/TruthRelatedMuonPlotOrganizer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

typedef ElementLink< xAOD::TruthParticleContainer > TruthLink;

namespace Muon{
  
TruthRelatedMuonPlotOrganizer::TruthRelatedMuonPlotOrganizer(PlotBase* pParent, const std::string& sDir, bool doBinnedResolutionPlots, std::vector<int> *selPlots):
  PlotBase(pParent, sDir)
  // Truth related plots
, m_oMatchedPlots(nullptr)
, m_oMatchedRecoPlots(nullptr)
, m_oMSHitDiffPlots(nullptr)
, m_oMuonHitDiffSummaryPlots(nullptr)
, m_oMuonTruthHitPlots(nullptr)
, m_oMuonResolutionPlots(nullptr)
, m_oDefParamPullPlots(nullptr)
, m_oMomentumTruthPullPlots_Tail(nullptr)
, m_oMomentumTruthPullPlots_NoTail(nullptr)
, m_oMatchedRecoElossPlots(nullptr)

{
  
  if (!selPlots) {
    m_selPlots.clear();
    for (unsigned int i=0; i<MAX_TRUTHRELATEDPLOTCLASS; i++) m_selPlots.push_back(i);
  }
  else m_selPlots = *selPlots;
  
  for (auto p: m_selPlots) {
    switch (p) {	
    case TRK_MATCHEDTRUE:
      m_oMatchedPlots = new Trk::ParamPlots(this, "/kinematics/", "Matched Muons");
      m_allPlots.push_back(m_oMatchedPlots);
      break;
    case TRK_MATCHEDRECO:
      m_oMatchedRecoPlots = new Trk::ParamPlots(this, "/kinematicsReco/", "Matched Muons");
      m_allPlots.push_back(m_oMatchedRecoPlots);
      break;
    case TRK_MSHITDIFF:
      m_oMSHitDiffPlots = new Trk::MSHitDiffPlots(this,"/hits/");
      m_allPlots.push_back(m_oMSHitDiffPlots);
      break;
    case MUON_HITDIFF:
      m_oMuonHitDiffSummaryPlots = new Muon::MuonHitDiffSummaryPlots(this,"/hits/");
      m_allPlots.push_back(m_oMuonHitDiffSummaryPlots);
      break;
    case MUON_TRUTHHIT:
      m_oMuonTruthHitPlots = new Muon::MuonTruthHitPlots(this,"/truthHits/");
      m_allPlots.push_back(m_oMuonTruthHitPlots);
      break;
    case MUON_RESOL:
      m_oMuonResolutionPlots = new Muon::MuonResolutionPlots(this, "/resolution/","",doBinnedResolutionPlots);
      m_allPlots.push_back(m_oMuonResolutionPlots);
      break;
    case TRK_DEFPARAMPULLS:
      m_oDefParamPullPlots = new Trk::DefParamPullPlots(this, "/pulls/","");
      m_allPlots.push_back(m_oDefParamPullPlots);
      break;
    case MUON_PULLSTAIL:          
      m_oMomentumTruthPullPlots_Tail = new Muon::MomentumTruthPullPlots(this,"/momentumPulls/","Tail");
      m_allPlots.push_back(m_oMomentumTruthPullPlots_Tail);
      break;
    case MUON_PULLSNOTAIL:
      m_oMomentumTruthPullPlots_NoTail = new Muon::MomentumTruthPullPlots(this,"/momentumPulls/","NoTail");
      m_allPlots.push_back(m_oMomentumTruthPullPlots_NoTail);
      break;
    case MUON_PARAMELOSS:
      m_oMatchedRecoElossPlots = new Muon::MuonParamElossPlots(this,"/Eloss/");
      m_allPlots.push_back(m_oMatchedRecoElossPlots);
      break;
    }
  }
  
}
TruthRelatedMuonPlotOrganizer::~TruthRelatedMuonPlotOrganizer()
{
  for (auto plots: m_allPlots)  delete plots;
  m_allPlots.clear();
}
  

  void TruthRelatedMuonPlotOrganizer::fill(const xAOD::TruthParticle& truthMu, const xAOD::Muon& mu, const xAOD::TrackParticleContainer* MSTracks, float weight){
    if (m_oMatchedPlots) m_oMatchedPlots->fill( truthMu , weight);
    if (m_oMuonHitDiffSummaryPlots) m_oMuonHitDiffSummaryPlots->fill(mu, truthMu, weight);
    if (m_oMuonTruthHitPlots) m_oMuonTruthHitPlots->fill(mu,weight);
 
  //new for eloss
    if (m_oMatchedRecoElossPlots) m_oMatchedRecoElossPlots->fill(truthMu, mu , weight );
 
  // Tracking related plots
  const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
  //const xAOD::TrackParticle* meTrk = mu.trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
  
  if (!primaryTrk) return;
  if (m_oMatchedRecoPlots) m_oMatchedRecoPlots->fill( *primaryTrk , weight);
  if (m_oMSHitDiffPlots) m_oMSHitDiffPlots->fill(*primaryTrk, truthMu, weight);
  if (m_oMuonResolutionPlots) m_oMuonResolutionPlots->fill(*primaryTrk, truthMu, weight);
  if (m_oDefParamPullPlots) m_oDefParamPullPlots->fill(*primaryTrk, truthMu, weight);
  
  if (m_oMomentumTruthPullPlots_NoTail || m_oMomentumTruthPullPlots_Tail) {
    //muon spectrometer track at MS entry (not extrapolated)
    const xAOD::TrackParticle *msTrk(nullptr);  
    //muon extrapolated to IP
    //const xAOD::TrackParticle* msExtrapTrk(0);
  
    ////////////////// @@@ sorting out the mess with the link to the extrapolated muon
    //for 20.1.0...
    /// const xAOD::TrackParticle* msExtrapTrk = mu.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle); // points to the ExtrapolatedMuonSpectrometerTrackParticle, the ExtrapolatedMuonSpectrometerTrackParticle link doesn't exist
    //for 20.1.3...
    //const xAOD::TrackParticle* msExtrapTrk = mu.trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
  
    //trying to accomodate both in a way that the code compiles in both releases
    if (mu.isAvailable< ElementLink<xAOD::TrackParticleContainer> >("extrapolatedMuonSpectrometerTrackParticleLink") && (mu.auxdata<ElementLink<xAOD::TrackParticleContainer> >("extrapolatedMuonSpectrometerTrackParticleLink")).isValid() ) {
      //cool, we got both links:
      //int correctEnumForExtrap = ((int)xAOD::Muon::MuonSpectrometerTrackParticle)+2;
      //msExtrapTrk = mu.trackParticle((xAOD::Muon::TrackParticleType) correctEnumForExtrap);
      msTrk = mu.trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );
    }
    else {
      // gymnastics to get msTrk...
      const ElementLink<xAOD::TrackParticleContainer>& msExtrapTrkLink = mu.trackParticleLink(xAOD::Muon::MuonSpectrometerTrackParticle);      
      if (msExtrapTrkLink.isValid()) {
	//msExtrapTrk = mu.trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );
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
#ifndef XAOD_ANALYSIS
    float eloss=0; 
    if (mu.parameter(eloss,xAOD::Muon::EnergyLoss)) { 
      if ( mu.energyLossType()!=xAOD::Muon::Tail ) { //to test MEASURED energy loss 
	if (m_oMomentumTruthPullPlots_NoTail) m_oMomentumTruthPullPlots_NoTail->fill(mu, msTrk, truthMu, weight); 
      } 
      else { 
	if (m_oMomentumTruthPullPlots_Tail) m_oMomentumTruthPullPlots_Tail->fill(mu, msTrk, truthMu, weight); //to test PARAMETRIZED energy loss 
      } 
    }
#endif // not XAOD_ANALYSIS
  }
}

  void TruthRelatedMuonPlotOrganizer::fill(const xAOD::TruthParticle& truthMu, const xAOD::TrackParticle& muTP, float weight){
  // Tracking related plots
    if (m_oMatchedPlots) m_oMatchedPlots->fill( truthMu, weight );
    if (m_oMatchedRecoPlots) m_oMatchedRecoPlots->fill( muTP , weight);
    if (m_oDefParamPullPlots) m_oDefParamPullPlots->fill(muTP, truthMu, weight);
    if (m_oMuonResolutionPlots) m_oMuonResolutionPlots->fill(muTP, truthMu, weight);
    if (m_oMSHitDiffPlots) m_oMSHitDiffPlots->fill(muTP, truthMu, weight);
}
  
}//close namespace

