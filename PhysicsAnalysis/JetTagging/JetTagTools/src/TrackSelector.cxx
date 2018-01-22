/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/TrackSelector.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh" 
#include "GeoPrimitives/GeoPrimitives.h"
#include "GaudiKernel/IToolSvc.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include <TMath.h>
#include <string>
#include <bitset>
#include <algorithm>

namespace Analysis {

  static const InterfaceID IID_ITrackSelector("Analysis::TrackSelector", 1, 0);

  TrackSelector::TrackSelector(const std::string& type, 
			       const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_primaryVertex(Amg::Vector3D()),
    m_ntri(0),
    m_ntrf(0),
    m_trackToVertexTool("Reco::TrackToVertex") {

    declareInterface<TrackSelector>(this);
    declareProperty("trackToVertexTool", m_trackToVertexTool);
    declareProperty("useBLayerHitPrediction", m_useBLayerHitPrediction = false);
    declareProperty("usePerigeeParameters", m_usePerigeeParameters = false);
    declareProperty("pTMin", m_pTMin = 1.*Gaudi::Units::GeV);
    declareProperty("usepTDepTrackSel", m_usepTDepTrackSel = false);
    declareProperty("pTMinOffset", m_pTMinOffset = 0.);
    declareProperty("pTMinSlope", m_pTMinSlope = 0.);
    declareProperty("d0Max", m_d0Max = 1.*Gaudi::Units::mm);
    declareProperty("z0Max", m_z0Max = 1.5*Gaudi::Units::mm);
    declareProperty("sigd0Max",m_sigd0Max = 999.*Gaudi::Units::mm);
    declareProperty("sigz0Max",m_sigz0Max = 999.*Gaudi::Units::mm);
    declareProperty("etaMax", m_etaMax = 9999.);
    declareProperty("useTrackSummaryInfo", m_useTrackSummaryInfo = true);
    declareProperty("nHitBLayer", m_nHitBLayer = 1);
    declareProperty("nHitPix", m_nHitPix = 1);
    declareProperty("nHitSct", m_nHitSct = 0);
    declareProperty("nHitSi", m_nHitSi = 7);
    declareProperty("nHitTrt", m_nHitTrt = 0);
    declareProperty("nHitTrtHighE", m_nHitTrtHighE = 0);
    declareProperty("useDeadPixInfo", m_useDeadPixInfo = true);
    declareProperty("useDeadSctInfo", m_useDeadSctInfo = true);
    declareProperty("useTrackQualityInfo", m_useTrackQualityInfo = true);
    declareProperty("fitChi2", m_fitChi2 = 99999.);
    declareProperty("fitProb", m_fitProb = -1.);
    declareProperty("fitChi2OnNdfMax",m_fitChi2OnNdfMax=999.);
    declareProperty("inputTrackCollection", m_inputTrackCollection);
    declareProperty("outputTrackCollection", m_outputTrackCollection);
    declareProperty("useAntiPileUpCuts", m_useAntiPileUpCuts = false);
    declareProperty("antiPileUpSigD0Cut", m_antiPileUpSigD0Cut = 3.0);
    declareProperty("antiPileUpSigZ0Cut", m_antiPileUpSigZ0Cut = 3.8);
    declareProperty("antiPileUpNHitSiCut", m_antiPileUpNHitSiCut = 9);
    declareProperty("antiPileUpNHolePixCut", m_antiPileUpNHolePixCut = 9);
    declareProperty("useTrackingTightDefinition", m_useTrackingTightDefinition = false);	
  }

  TrackSelector::~TrackSelector() {
  }

  const InterfaceID& TrackSelector::interfaceID() {
    return IID_ITrackSelector;
  }

  StatusCode TrackSelector::initialize() {
    for(int i=0;i<numCuts;i++) m_ntrc[i]=0;
    /** retrieving ToolSvc: */
    IToolSvc* toolSvc;
    StatusCode sc = service("ToolSvc", toolSvc);
    if (StatusCode::SUCCESS != sc) {
      ATH_MSG_ERROR("#BTAG# Can't get ToolSvc");
      return StatusCode::FAILURE;
    }
    /** retrieving TrackToVertex: */
    if ( m_trackToVertexTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackToVertexTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_trackToVertexTool);
    }

    if (m_useTrackingTightDefinition){
      m_d0Max = 2*Gaudi::Units::mm;
      m_z0Max = 3*Gaudi::Units::mm;
      m_pTMin = 0.4*Gaudi::Units::GeV;
    }
 
    /** dump cuts: */
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "#BTAG# TrackSelector " << name() << " cuts: " << endmsg;
      msg(MSG::DEBUG) << "#BTAG#     - pT >= " << m_pTMin << endmsg;
      msg(MSG::DEBUG) << "#BTAG#     - |eta| <= " << m_etaMax << endmsg;
      msg(MSG::DEBUG) << "#BTAG#     - |d0| <= " << m_d0Max << endmsg;
      msg(MSG::DEBUG) << "#BTAG#     - |z0| <= " << m_z0Max << endmsg;
      msg(MSG::DEBUG) << "#BTAG#     - |sigd0| <= " << m_sigd0Max << endmsg;
      msg(MSG::DEBUG) << "#BTAG#     - |sigz0| <= " << m_sigz0Max << endmsg;
      if(m_useAntiPileUpCuts) {
        msg(MSG::DEBUG) << "#BTAG#     - antiPUcut: reject tracks with |sigz0| > " << m_antiPileUpSigZ0Cut 
                        << " when |sigd0| < " << m_antiPileUpSigD0Cut << endmsg;
      }
      if(m_useTrackSummaryInfo) {
	msg(MSG::DEBUG) << "#BTAG#     - nbHitsBLayer >= " << m_nHitBLayer << endmsg;
	if(m_useDeadPixInfo) {
	  msg(MSG::DEBUG) << "#BTAG#     - nbHitsPix+nbDeadPix >= " << m_nHitPix << endmsg;
	} else {
	  msg(MSG::DEBUG) << "#BTAG#     - nbHitsPix >= " << m_nHitPix << endmsg;
	}
	if(m_useBLayerHitPrediction)
	  msg(MSG::DEBUG) << "#BTAG#     using conddb for b-layer hit requirements " << endmsg;
	  
	if(m_useDeadSctInfo) {
	  msg(MSG::DEBUG) << "#BTAG#     - nbHitsSct+nbDeadSct >= " << m_nHitSct << endmsg;
	} else {
	  msg(MSG::DEBUG) << "#BTAG#     - nbHitsSct >= " << m_nHitSct << endmsg;
	}
	int nhsi = m_nHitSi;
	if(m_useAntiPileUpCuts) nhsi = m_antiPileUpNHitSiCut;
	if(m_useDeadPixInfo) {
	  if(m_useDeadSctInfo) {
	    msg(MSG::DEBUG) << "#BTAG#     - nbHitsSi+nbDeadPix+nbDeadSct >= " << nhsi << endmsg;
	  } else {
	    msg(MSG::DEBUG) << "#BTAG#     - nbHitsSi+nbDeadPix >= " << nhsi << endmsg;
	  }
	} else {
	  if(m_useDeadSctInfo) {
	    msg(MSG::DEBUG) << "#BTAG#     - nbHitsSi+nbDeadSct >= " << nhsi << endmsg;
	  } else {
	    msg(MSG::DEBUG) << "#BTAG#     - nbHitsSi >= " << nhsi << endmsg;
	  }
	}
	msg(MSG::DEBUG) << "#BTAG#     - nbHitsTrt >= " << m_nHitTrt << endmsg;
	msg(MSG::DEBUG) << "#BTAG#     - nbHitsTrtHighE >= " << m_nHitTrtHighE << endmsg;
      }
      msg(MSG::DEBUG) << "#BTAG#     - fit chi2 <= " << m_fitChi2 << endmsg;
      msg(MSG::DEBUG) << "#BTAG#     - fit proba >= " << m_fitProb << endmsg;
      msg(MSG::DEBUG) << "#BTAG#     - fit chi2 / ndf <= " << m_fitChi2OnNdfMax << endmsg;
    }

    return StatusCode::SUCCESS;
  }


  StatusCode TrackSelector::finalize() {
    ATH_MSG_VERBOSE("#BTAG#  tracks selected: In= " << m_ntri);
    for(int i=0;i<numCuts;i++) ATH_MSG_VERBOSE("#BTAG#  cut" << i << "= " << m_ntrc[i]);
    ATH_MSG_VERBOSE("#BTAG#  Out= " << m_ntrf);
    return StatusCode::SUCCESS;
  }


  bool TrackSelector::selectTrack(const xAOD::TrackParticle* track, double refPt) {

    /** for debugging purposes: */
    std::bitset<numCuts> failedCuts;
    
    double trackD0;
    double trackZ0;
    double tracksigD0;
    double tracksigZ0;
    if(m_usePerigeeParameters) {
      trackD0 = track->d0();
      trackZ0 = track->z0() - m_primaryVertex.z();
      tracksigD0 = TMath::Sqrt(track->definingParametersCovMatrix()(0,0));
      tracksigZ0 = TMath::Sqrt(track->definingParametersCovMatrix()(1,1));
    } else {
      // extrapolate with the TrackToVertex tool:
      const Trk::Perigee* perigee = m_trackToVertexTool->perigeeAtVertex(*track, m_primaryVertex);
      if (perigee==0) {
        ATH_MSG_WARNING("#BTAG#  Extrapolation failed. Rejecting track... ");
        return false;
      }
      trackD0 = perigee->parameters()[Trk::d0];
      trackZ0 = perigee->parameters()[Trk::z0];
      tracksigD0 = TMath::Sqrt((*perigee->covariance())(Trk::d0,Trk::d0));
      tracksigZ0 = TMath::Sqrt((*perigee->covariance())(Trk::z0,Trk::z0));
      delete perigee;
    }

    ATH_MSG_VERBOSE( "#BTAG#  Track "
		     << " Eta= " << track->eta() << " Phi= " << track->phi() << " pT= " <<track->pt()
		     << " d0= " << trackD0
		     << " z0= " << trackZ0 << " sigd0= " << tracksigD0 << " sigz0: " << tracksigZ0 );
    // if (m_usepTDepTrackSel){
    //   m_pTMin = m_pTMinOffset + m_pTMinSlope*m_SumTrkPt;
    // }

    /** apply cuts: */
    bool pass = true;
    if(track->pt() < m_pTMin) {
      pass = false;
      failedCuts.set(pTMin);
    } else if (refPt > 0 && m_usepTDepTrackSel && track->pt() < m_pTMinOffset + m_pTMinSlope*refPt) {
      pass = false;
      failedCuts.set(pTMin);
    }
    if(fabs(trackD0)>m_d0Max) {
      pass = false;
      failedCuts.set(d0Max);
    }
    if(fabs(trackZ0*sin(track->theta()))>m_z0Max) {
      pass = false;
      failedCuts.set(z0Max);
    }
    if (tracksigD0>m_sigd0Max) {
      pass = false;
      failedCuts.set(sigd0Max);
    }
    if (tracksigZ0>m_sigz0Max) {
      pass = false;
      failedCuts.set(sigz0Max);
    }
    if(m_useAntiPileUpCuts) {
      if(fabs(trackZ0/tracksigZ0)>m_antiPileUpSigZ0Cut && fabs(trackD0/tracksigD0)<m_antiPileUpSigD0Cut) {
        pass = false;
        failedCuts.set(sigz0Max);
        failedCuts.set(sigd0Max);
      }
    }
    if(fabs(track->eta())>m_etaMax) {
      pass = false;
      failedCuts.set(etaMax);
    }
    if(m_useTrackSummaryInfo) {
      uint8_t nb=0;
      track->summaryValue(nb, xAOD::numberOfInnermostPixelLayerHits);
      if(nb < m_nHitBLayer) {
	failedCuts.set(nHitBLayer);
	if(!m_useBLayerHitPrediction) { 
	  pass = false;
	  failedCuts.set(deadBLayer);
	} else {
	  uint8_t ehib=1;
	  if (!track->summaryValue(ehib,xAOD::expectInnermostPixelLayerHit)) {
	    ATH_MSG_WARNING("#BTAG# expectInnermostPixelLayerHit not computed in  TrackSummary: assuming true");
	    ehib=1;
	  }
	  if(ehib) {  // check if module was alive
	    pass = false;
	    failedCuts.set(deadBLayer);
	  }
	}
      }
      uint8_t nhp=0;
      track->summaryValue(nhp, xAOD::numberOfPixelHoles);
      if(m_useAntiPileUpCuts) {
	if(nhp>=m_antiPileUpNHolePixCut) {
	  pass = false;
	}
      }
      uint8_t np=0;
      track->summaryValue(np, xAOD::numberOfPixelHits);
      if(m_useDeadPixInfo) 
      {
	uint8_t ndead;
	track->summaryValue(ndead, xAOD::numberOfPixelDeadSensors);
	np += std::max((int)ndead, 0);
      }
      if(np < m_nHitPix) {
	pass = false;
	failedCuts.set(nHitPix);
      }
      uint8_t ns=0;
      track->summaryValue(ns, xAOD::numberOfSCTHits);
      if(m_useDeadSctInfo)
      {
	uint8_t ndead;
        track->summaryValue(ndead, xAOD::numberOfSCTDeadSensors);
	ns += std::max((int)ndead,0);
      }
      if(ns < m_nHitSct) {
	pass = false;
	failedCuts.set(nHitSct);
      }
      if((np+ns) < m_nHitSi) {
	pass = false;
	failedCuts.set(nHitSi);
      }
      if(m_useAntiPileUpCuts) {
	if((np+ns) < m_antiPileUpNHitSiCut) {
	  pass = false;
	  failedCuts.set(nHitSi);
	}
      }
      uint8_t nh=0;
      track->summaryValue(nh, xAOD::numberOfTRTHits);//ms
      if(nh < m_nHitTrt) {
	pass = false;
	failedCuts.set(nHitTrt);
      }
      uint8_t nhe=0;
      track->summaryValue(nhe, xAOD::numberOfTRTHighThresholdHits);//ms
      if(nhe < m_nHitTrtHighE) {
	pass = false;
	failedCuts.set(nHitTrtHighE);
      }
      // Tracking CP group Tight definition
      if(m_useTrackingTightDefinition) {
	uint8_t nibl, nnibl;
	track->summaryValue(nibl , xAOD::numberOfPixelHits);
	track->summaryValue(nnibl, xAOD::numberOfPixelHits);
	bool innerHitsCrit = ((nibl+nnibl)>0);
	bool lowetaCrit  = fabs(track->eta())> 1.65 && (ns+np)>=11;
	bool highetaCrit = fabs(track->eta())<=1.65 && (ns+np)>=9 ;
	bool pixholeCrit = (nhp==0) ;
	bool isTight = innerHitsCrit && pixholeCrit && (lowetaCrit || highetaCrit);
	if (!isTight){
	  pass=false;
	  failedCuts.set(trackingTightDef);
	}
	  
      }
    }
    
    // Now the fit quality
    double chi2 = track->chiSquared();
    int ndf = track->numberDoF();
    double proba = 1.;
    if(ndf>0 && chi2>=0.) {
      Genfun::CumulativeChiSquare myCumulativeChiSquare(ndf);
      proba = 1.-myCumulativeChiSquare(chi2);
    }
    if(chi2>m_fitChi2) {
      pass = false;
      failedCuts.set(fitChi2);
    }
    if(proba<m_fitProb) {
      pass = false;
      failedCuts.set(fitProb);
    }
    if(ndf>0) {
      if(chi2/double(ndf)>m_fitChi2OnNdfMax) {
        pass = false;
        failedCuts.set(fitChi2OnNdfMax);
      }
    } else {
      pass = false;
      failedCuts.set(fitChi2OnNdfMax);
    }

    ///std::string showPassedCuts = ~failedCuts.to_string(); // not available !
    if( msgLvl(MSG::VERBOSE) ){
      ATH_MSG_VERBOSE("#BTAG#  passedCuts for track ");
      for(int i=0;i<numCuts;i++) {
	int passl = ~failedCuts[(m_Cuts)i];
	if(passl) m_ntrc[i]++;
	msg(MSG::VERBOSE) << passl;
      } 
      msg(MSG::VERBOSE) << endmsg;
    }

    m_passedCuts = ~failedCuts;
    return pass;
  }

}
