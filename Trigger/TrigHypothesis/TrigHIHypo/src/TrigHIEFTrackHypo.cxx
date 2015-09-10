/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFTrackHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHIHypo
//
// AUTHOR:  
//           Performs the cuts on the ID variables. 
//           I.Grabowska-Bold, created Sept 2011
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
#include "TrigHIEFTrackHypo.h"
//
#include "Particle/TrackParticleContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/ITHistSvc.h"

class ISvcLocator;

template <class SRC>
inline const DataVector<Rec::TrackParticle>** dvec_cast(SRC** ptr) { 
  return (const DataVector<Rec::TrackParticle>**)(ptr); 
} 


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigHIEFTrackHypo::TrigHIEFTrackHypo(const std::string& name, ISvcLocator* pSvcLocator):
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
  declareProperty("MinNumTracks", m_numMinTracks=1, 
		  "Minimal number of tracks satisfying the selection");
  declareProperty("MaxNumTracks", m_numMaxTracks=10000, 
		  "Max number of tracks satisfying the selection");

  declareMonitoredCollection("pt", *dvec_cast(&TrkParticleCont), &monPt);
  declareMonitoredCollection("Phi",*dvec_cast(&TrkParticleCont), &monPhi);
  declareMonitoredCollection("Eta",*dvec_cast(&TrkParticleCont), &monEta);
  declareMonitoredCollection("d0", *dvec_cast(&TrkParticleCont), &mond0);
  declareMonitoredCollection("z0", *dvec_cast(&TrkParticleCont), &monz0);
  declareMonitoredVariable("numTrkPartIn",m_numTrkPartIn);
  declareMonitoredVariable("numTrkPartOut",m_numTrkPartOut);

  TrkParticleCont = 0;
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigHIEFTrackHypo::~TrigHIEFTrackHypo()
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
HLT::ErrorCode TrigHIEFTrackHypo::hltBeginRun(){

  msg() << MSG::INFO << "TrigHIEFTrackHypo::beginRun()" << endreq;
  return HLT::OK;

}
//----------------------------------------------------------------------------

// ----------------------------------------------------------------------
HLT::ErrorCode TrigHIEFTrackHypo::hltInitialize() {
// ----------------------------------------------------------------------

  msg() << MSG::INFO << "TrigHIEFTrackHypo initialize()" << endreq;

  msg() << MSG::INFO
      << "Initialization of TrigHIEFTrackHypo completed successfully"
      << endreq;

  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigHIEFTrackHypo::hltFinalize() {
// ----------------------------------------------------------------------

  msg() << MSG::INFO << "TrigHIEFTrackHypo finalize()" << endreq;

  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigHIEFTrackHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
// ----------------------------------------------------------------------

  // default value, it will be set to true if selection satisfied
  pass = false;
  TrkParticleCont=0;

  int outputLevel = msgLvl();

  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << name() << ": in execute()" << endreq;

  // AcceptAll property = true means selection cuts should not be applied
  if (m_acceptAll){
    if(outputLevel <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" << endreq;
    pass=true;
    return HLT::OK;
  }
  else {
    if(outputLevel <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
	    << endreq;
  }


  // get Trk::Tracks's from the trigger element:
  //--------------------------------------------------
  bool accepted=false;

  //const Rec::TrackParticleContainer* TrkParticleCont(0);

  if ( HLT::OK != getFeature(outputTE, TrkParticleCont) ) {
    msg() << MSG::ERROR << "Failed to get TrackParticleContainer from the trigger element " << endreq;
    return HLT::NAV_ERROR;
  }

  if ( !TrkParticleCont ) {
    msg() << MSG::DEBUG << "No TrackParticleContainer present" << endreq;
    return HLT::OK;
  }

  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << " REGTEST: Got " << TrkParticleCont->size() 
	<< " TrackParticleContainer's associated to the TE " << endreq;
  
  m_numTrkPartIn=TrkParticleCont->size();
  
  int ntracks=0;
  for(Rec::TrackParticleContainer::const_iterator trkIt = TrkParticleCont->begin();
      trkIt != TrkParticleCont->end(); trkIt++){
    
    // apply the cuts
    // ---------------

    const Trk::TrackSummary* summary = (*trkIt)->trackSummary();
    
    if(summary == 0){
      msg() << MSG::WARNING 
	  << " Atention! There is a track with no summary information linked ! " << endreq;
    }
    if(summary == 0 ) continue;  // ????

    bool ok=true;
    if (m_applyTrackCut) {
      //    int ncontrib_pixel = summary->get(Trk::numberOfContribPixelLayers);
      int nblayer = summary->get(Trk::numberOfBLayerHits);
      int npixel = summary->get(Trk::numberOfPixelHits);
      int nsct = summary->get(Trk::numberOfSCTHits);
      int ntrt = summary->get(Trk::numberOfTRTHits);
      int ntrt_out = summary->get(Trk::numberOfTRTOutliers);
      int ntrt_ht = summary->get(Trk::numberOfTRTHighThresholdHits);
      int ntrt_ht_out = summary->get(Trk::numberOfTRTHighThresholdOutliers);
      int ntrt_total = ntrt + ntrt_out;
      float trtratio = 0.0;
      if ( (ntrt+ntrt_out) > 0) {
	trtratio = static_cast<float>(ntrt_ht+ntrt_ht_out)/(ntrt+ntrt_out);
      }
      float d0 = 0.0;
      const Trk::Perigee* perigee = (*trkIt)->measuredPerigee();
      if (perigee) d0 = perigee->parameters()[Trk::d0];
      float pt = (*trkIt)->pt();
      
//       if (summary->get(Trk::expectBLayerHit) && 
// 	  nblayer < m_numBLayerHits) ok = false;
      if (nblayer < m_numBLayerHits) ok = false;
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
// 	  << endreq;
    // ---------------------------------------------------
    // At least one track passed cuts: accept the event:
    // ---------------------------------------------------

    if (ok) ntracks ++;

  } // end of loop in tracks

  //  msg() << MSG::DEBUG << "Number of tracks found: " << ntracks << endreq;
  if (ntracks >= m_numMinTracks && ntracks <= m_numMaxTracks) accepted = true;
  
  if(accepted){

    m_numTrkPartOut=ntracks;

    pass =true;
    
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " REGTEST: Event accepted ! " << endreq;
    
  }
  else{
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " REGTEST: No track was found !! Event rejected " << endreq;
  }
  
  // Time total TrigHIEFTrackHypo execution time.
  // -------------------------------------
  
  return HLT::OK;
}
