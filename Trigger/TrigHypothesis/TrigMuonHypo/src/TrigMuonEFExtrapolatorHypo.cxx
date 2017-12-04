/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuonEFExtrapolatorHypo.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

TrigMuonEFExtrapolatorHypo::TrigMuonEFExtrapolatorHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator){
  declareProperty("AcceptAll", m_acceptAll=true);
  declareProperty("MSonlyCut", m_msonlyCut=false);
  std::vector<float> def_bins;
  def_bins.push_back(0);
  def_bins.push_back(9.9);
  std::vector<float> def_thrs;
  def_thrs.push_back(0.*CLHEP::GeV);
  declareProperty("PtBins", m_ptBins=def_bins);
  declareProperty("PtThresholds", m_ptThresholds=def_thrs);
  declareMonitoredStdContainer("Pt",  m_fex_pt);
  declareMonitoredStdContainer("Eta", m_fex_eta);
  declareMonitoredStdContainer("Phi", m_fex_phi);

  m_bins = 0;
  m_storeGate=0;

}

TrigMuonEFExtrapolatorHypo::~TrigMuonEFExtrapolatorHypo(){
}

HLT::ErrorCode TrigMuonEFExtrapolatorHypo::hltInitialize(){


  if(m_acceptAll) {
	 ATH_MSG_INFO("Accepting all the events with not cut!");
  } else {
    m_bins = m_ptBins.size() - 1;
    if (m_bins != m_ptThresholds.size()) {
		ATH_MSG_INFO("bad thresholds setup .... exiting!");
      return HLT::BAD_JOB_SETUP;
      //    msg() << MSG::INFO
      //  << "current hypo implementation is dummy" << endreq;
    }

    for (std::vector<float>::size_type i=0; i<m_bins;++i) {
		ATH_MSG_INFO("bin " << m_ptBins[i] << " - " <<  m_ptBins[i+1]
		                    << " with Pt Threshold of " << (m_ptThresholds[i])/CLHEP::GeV
								  << " CLHEP::GeV");
    }
  }

  if(m_msonlyCut) {
     if(msgLvl() <= MSG::DEBUG) {
		  ATH_MSG_DEBUG("Apply GoodPrecisonLayers cut in MS-only triggers");
     }
  }

  ATH_MSG_INFO("Initialization completed successfully");

  return HLT::OK;
}

HLT::ErrorCode TrigMuonEFExtrapolatorHypo::hltFinalize()
{
  ATH_MSG_INFO("in finalize()");
  return HLT::OK;
}

HLT::ErrorCode TrigMuonEFExtrapolatorHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){



  m_storeGate = store();

  if(msgLvl() <= MSG::DEBUG) 
	 ATH_MSG_DEBUG("in execute()");

  //resetting the monitoring variables
  m_fex_pt.clear();
  m_fex_eta.clear();
  m_fex_phi.clear();

  if(m_acceptAll) {
    pass = true;
    if(msgLvl() <= MSG::DEBUG) {
		ATH_MSG_DEBUG("Accept property is set: taking all the events");
    }
    return HLT::OK;
  }

  //  HLT::ErrorCode status;

  bool result = false;
  bool debug = msgLvl() <= MSG::DEBUG;

  // Some debug output:
  if(msgLvl() <= MSG::DEBUG) ATH_MSG_DEBUG("outputTE->ID(): " << outputTE->getId());

  // will do hypo cuts here

  // Get MuonContainer linked to the outputTE

  const xAOD::MuonContainer*  muonContainer(0);
  if(getFeature(outputTE, muonContainer)!=HLT::OK) {
    if (debug) ATH_MSG_DEBUG("no xAOD::MuonContainer Feature found");
    return HLT::MISSING_FEATURE;
  } else {
    if (!muonContainer) { 
      if (debug) ATH_MSG_DEBUG("null xAOD::MuonContainer Feature found");
      return HLT::MISSING_FEATURE; 
    } 
    if (debug) ATH_MSG_DEBUG("vector of xAOD::MuonContainer found with size " << muonContainer->size());
  } 

  for (unsigned int i=0; i<muonContainer->size(); i++){



    const xAOD::Muon* muon = muonContainer->at(i); 
    if (!muon) {
      if (debug) ATH_MSG_DEBUG("No xAOD::Muon found.");
      continue;
    } else {


      if (muon->primaryTrackParticle()) { // was there a muon in this RoI ?


	const xAOD::TrackParticle* tr = muon->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);


	if (!tr) {
	  if (debug) ATH_MSG_DEBUG("No ExtrapolatedMuonSpectrometerTrackParticle found.");
	  continue;
	} else {
	  if (debug) 
	    ATH_MSG_DEBUG("Retrieved ExtrapolatedMuonSpectrometerTrack track with abs pt "
		                << (*tr).pt()/CLHEP::GeV << " GeV");
 
	  m_fex_pt.push_back(tr->pt()/CLHEP::GeV);
	  float eta = tr->eta();
	  m_fex_eta.push_back(eta);
	  m_fex_phi.push_back(tr->phi());

	  float absEta = fabs(eta);
     if (m_msonlyCut) {
       uint8_t nGoodPrcLayers(0);
       if (!muon->summaryValue(nGoodPrcLayers, xAOD::numberOfGoodPrecisionLayers)){
          if (debug) ATH_MSG_DEBUG("No numberOfGoodPrecisionLayers variable foud.");
          continue;
       }
       if(absEta > 1.05 && nGoodPrcLayers < 3) continue;
     }
	  float threshold = 0;
	  for (std::vector<float>::size_type k=0; k<m_bins; ++k) {
	    if (absEta > m_ptBins[k] && absEta <= m_ptBins[k+1]) threshold = m_ptThresholds[k];
	  }
	  if (fabs(tr->pt())/CLHEP::GeV > (threshold/CLHEP::GeV)){
	    result = true;
	  }
	  if(debug) 
		 ATH_MSG_DEBUG(" REGTEST muon pt is " << tr->pt()/CLHEP::GeV << " GeV "
		                   << " with Charge " << tr->charge()
								 << " and threshold cut is " << threshold/CLHEP::GeV << " GeV"
								 << " so hypothesis is " << (result?"true":"false"));
	}
      }
				
    }
  }
  
  pass = result;
  return HLT::OK;
}  

