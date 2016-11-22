/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFTrackHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigEgammaHypo
//
// AUTHOR:   P. Conde Muino
//           Performs the cuts on the ID variables. 
//           I.Grabowska-Bold, update Sept 2006
//
// ********************************************************************
//
#include <list>
#include <iterator>
#include <sstream>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//
#include "TrigEgammaHypo/TrigEFTrackHypo.h"
//
#include "xAODTracking/TrackParticleContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/ITHistSvc.h"

class ISvcLocator;

template <class SRC>
inline const DataVector<xAOD::TrackParticle>** dvec_cast(SRC** ptr) { 
  return (const DataVector<xAOD::TrackParticle>**)(ptr); 
} 


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFTrackHypo::TrigEFTrackHypo(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator),
    m_acceptAll(true)
{
  declareProperty("AcceptAll",      m_acceptAll);
  declareProperty("ApplyTrackCut", m_applyTrackCut=false);
  declareProperty("NumBLayerHits", m_numBLayerHits=0, "Number of B-layer hits required");
  declareProperty("NumPixelHits", m_numPixelHits=0, "Number of pixel hits required");
  declareProperty("NumSCTHits", m_numSCTHits=0, "Number of SCT htis required");
  declareProperty("NumTRTHits", m_numTRTHits=0, "Number of TRT hits required");
  declareProperty("TRTRatio", m_TRTRatio=0.0, "TRT high threshold hit ratio (min.)");
  declareProperty("Chi2", m_chi2=-1.0, "Maximum chi2 of the track fit");
  declareProperty("D0", m_d0=-1.0, "Maximum D0 (mm)");
  declareProperty("Pt", m_pt=0.0, "Minimum Pt (MeV)");
  declareProperty("NumTracks", m_numTracks=1, 
		  "Number of tracks satisfying the selection");

  declareMonitoredCollection("pt", *dvec_cast(&m_TrkParticleCont), &xAOD::TrackParticle::pt);
  declareMonitoredCollection("Phi",*dvec_cast(&m_TrkParticleCont), &xAOD::TrackParticle::phi);
  declareMonitoredCollection("Eta",*dvec_cast(&m_TrkParticleCont), &xAOD::TrackParticle::eta);
  declareMonitoredCollection("d0", *dvec_cast(&m_TrkParticleCont), &xAOD::TrackParticle::d0);
  declareMonitoredCollection("z0", *dvec_cast(&m_TrkParticleCont), &xAOD::TrackParticle::z0);
  declareMonitoredVariable("numTrkPart",m_numTrkPart);
  m_TrkParticleCont=nullptr;
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFTrackHypo::~TrigEFTrackHypo()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//
//----------------------------------
//          beginRun method:
//----------------------------------------------------------------------------
HLT::ErrorCode TrigEFTrackHypo::hltBeginRun(){

  msg() << MSG::INFO << "TrigEFTrackHypo::beginRun()" << endmsg;
  return HLT::OK;

}
//----------------------------------------------------------------------------

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFTrackHypo::hltInitialize() {
// ----------------------------------------------------------------------

  msg() << MSG::INFO << "TrigEFTrackHypo initialize()" << endmsg;

  msg() << MSG::INFO
      << "Initialization of TrigEFTrackHypo completed successfully"
      << endmsg;

  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFTrackHypo::hltFinalize() {
// ----------------------------------------------------------------------

  msg() << MSG::INFO << "TrigEFTrackHypo finalize()" << endmsg;

  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFTrackHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
// ----------------------------------------------------------------------

  // default value, it will be set to true if selection satisfied
  pass = false;
  m_TrkParticleCont=0;

  int outputLevel = msgLvl();

  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << name() << ": in execute()" << endmsg;

  // AcceptAll property = true means selection cuts should not be applied
  if (m_acceptAll){
    if(outputLevel <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" << endmsg;
    pass=true;
    return HLT::OK;
  }
  else {
    if(outputLevel <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
	    << endmsg;
  }


  // get Trk::Tracks's from the trigger element:
  //--------------------------------------------------
  bool accepted=false;

  //const Rec::TrackParticleContainer* m_TrkParticleCont(0);

  if ( HLT::OK != getFeature(outputTE, m_TrkParticleCont) ) {
    msg() << MSG::ERROR << "Failed to get TrackParticleContainer from the trigger element " << endmsg;
    return HLT::NAV_ERROR;
  }

  if ( !m_TrkParticleCont ) {
    msg() << MSG::DEBUG << "No TrackParticleContainer present" << endmsg;
    return HLT::OK;
  }

  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << " REGTEST: Got " << m_TrkParticleCont->size() 
	<< " TrackParticleContainer's associated to the TE " << endmsg;
  
  m_numTrkPart=m_TrkParticleCont->size();

  int ntracks=0;
  for(const auto trkIt : *m_TrkParticleCont){
    // apply the cuts
    // ---------------

    bool ok=true;
    if (m_applyTrackCut) {
      float d0 = trkIt->d0(); 
      float pt = trkIt->pt();
      //Summary values 
      uint8_t nblayer(0),npixel(0),nsct(0),ntrt(0),ntrt_out(0),ntrt_ht(0),ntrt_ht_out(0),expectBlayerHit(0);
      trkIt->summaryValue(nblayer,xAOD::numberOfInnermostPixelLayerHits);
      trkIt->summaryValue(npixel,xAOD::numberOfPixelHits);
      trkIt->summaryValue(npixel,xAOD::numberOfSCTHits);
      trkIt->summaryValue(ntrt,xAOD::numberOfTRTHits);
      trkIt->summaryValue(ntrt_out,xAOD::numberOfTRTOutliers);
      trkIt->summaryValue(ntrt_ht,xAOD::numberOfTRTHighThresholdHits);
      trkIt->summaryValue(ntrt_ht_out,xAOD::numberOfTRTHighThresholdOutliers);
      trkIt->summaryValue(expectBlayerHit,xAOD::expectInnermostPixelLayerHit);
      int ntrt_total = ntrt + ntrt_out;
      float trtratio = 0.0;
      if ( (ntrt+ntrt_out) > 0) {
	trtratio = static_cast<float>(ntrt_ht+ntrt_ht_out)/(ntrt+ntrt_out);
      }

      if (expectBlayerHit && 
       	  nblayer < m_numBLayerHits) ok = false;
      //if (nblayer < m_numBLayerHits) ok = false;
      if (npixel < m_numPixelHits) ok = false;
      if (nsct < m_numSCTHits) ok = false;
      if (ntrt_total < m_numTRTHits) ok = false;
      if (trtratio < m_TRTRatio) ok = false;
      if (m_d0>0 && d0 > m_d0) ok = false;
      if (pt < m_pt) ok = false;
    }

    //     msg() << MSG::DEBUG 
    // 	  << "Track : N(B-layer)=" << nblayer 
    // 	  << " N(B-layer)=" << nblayer
    // 	  << " N(pixel)=" << npixel 
    // 	  << " N(SCT)=" << nsct
    // 	  << " N(TRT)=" << ntrt_total
    // 	  << " TRTratio=" << trtratio
    // 	  << " D0=" << d0
    // 	  << " Pt=" << pt
    // 	  << endmsg;
    // ---------------------------------------------------
    // At least one track passed cuts: accept the event:
    // ---------------------------------------------------
    
    if (ok) ntracks ++;
    
  } // end of loop in tracks

  //  msg() << MSG::DEBUG << "Number of tracks found: " << ntracks << endmsg;
  if (ntracks >= m_numTracks) accepted = true;

  if(accepted){

    pass =true;
    
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " REGTEST: Event accepted ! " << endmsg;
    
  }
  else{
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " REGTEST: No track was found !! Event rejected " << endmsg;
  }
  
  // Time total TrigEFTrackHypo execution time.
  // -------------------------------------
  
  return HLT::OK;
}
