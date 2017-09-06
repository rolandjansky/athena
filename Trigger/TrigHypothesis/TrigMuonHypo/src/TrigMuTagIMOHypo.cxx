/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *****************************************************
//
// NAME:     TrigMuTagIMOHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigMuonHypo
//
// AUTHOR:   Andrea Coccaro (AT ge DOT infn DOT it)
//
// *****************************************************

#include "TrigMuonHypo/TrigMuTagIMOHypo.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;



//** ----------------------------------------------------------------------------------------------------------------- **//


TrigMuTagIMOHypo::TrigMuTagIMOHypo(const std::string & name, ISvcLocator* pSvcLocator) :
  HLT::HypoAlgo(name, pSvcLocator)
{
  declareProperty("AcceptAll", m_acceptAll = false);
  std::vector<float> def_bins;
  def_bins.push_back(0);
  def_bins.push_back(2.5); 
  std::vector<float> def_thrs;
  def_thrs.push_back(10.*CLHEP::GeV);
  declareProperty("PtBins", m_ptBins=def_bins);
  declareProperty("PtThresholds", m_ptThresholds=def_thrs);

  declareMonitoredVariable("CutCounter", m_cutCounter);
  declareMonitoredVariable("Pt",         m_pt);
  declareMonitoredVariable("Eta",        m_eta);
  declareMonitoredVariable("Phi",        m_phi);

  m_bins = 0;

}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigMuTagIMOHypo::~TrigMuTagIMOHypo(){}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigMuTagIMOHypo::hltInitialize() { 

  if(m_acceptAll)
    msg() << MSG::INFO << "Accepting all the events" << endmsg;
  else {
    m_bins = m_ptBins.size() - 1;
    if (m_bins != m_ptThresholds.size()) {
      msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endmsg;
      return HLT::BAD_JOB_SETUP;   
    }
     
    for (std::vector<float>::size_type i=0; i<m_bins;++i)
      msg() << MSG::INFO << "bin " << m_ptBins[i] << " - " <<  m_ptBins[i+1] << " with Pt Threshold of " << (m_ptThresholds[i])/CLHEP::GeV << " GeV" << endmsg;

  }

  msg() << MSG::INFO << "Initialization completed successfully" << endmsg;

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigMuTagIMOHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  
  if(msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Executing TrigMuTagIMOHypo" << endmsg;
  
  //* initialise monitoring variables *//
  m_cutCounter = -1;

  //* AcceptAll declare property setting *//
  if (m_acceptAll) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST: AcceptAll property is set: taking all events" << endmsg;
    m_cutCounter = 1;
    pass = true;
    return HLT::OK;
  } else 
    msg() << MSG::DEBUG << "REGTEST: AcceptAll property not set: applying selection" << endmsg;
  
 //* Get RoI descriptor *//
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode stat = getFeature(outputTE, roiDescriptor, "");
  
  if (stat == HLT::OK) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Using outputTE: " 
	    << "RoI id " << roiDescriptor->roiId()
	    << ", Phi = " <<  roiDescriptor->phi()
	    << ", Eta = " << roiDescriptor->eta() << endmsg;
    }
  } else {
    if (msgLvl() <= MSG::WARNING) 
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element " << endmsg;

    return HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::MISSING_FEATURE);
  }

  bool result = false;

  //* Get vector of TrigMuonEFInfo *//
  const TrigMuonEFInfoContainer* trigMuonEFInfoColl = 0;
  if (getFeature(outputTE, trigMuonEFInfoColl, "MuTagIMO_EF") != HLT::OK) {

    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Failed to get TrigMuTagIMO collection" << endmsg;

    return HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::MISSING_FEATURE); 

  } else if (!trigMuonEFInfoColl) {

    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Empty TrigMuTagIMO collection" << endmsg;
    
    return HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::MISSING_FEATURE); 

  } else {

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Got collection with " << trigMuonEFInfoColl->size() << " TrigMuonEF" << endmsg;
  }

  if (!(trigMuonEFInfoColl->size())) {

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "No TrigMuonEFInfo to analyse" << endmsg;

    return HLT::OK;
  }

  //* To separate bad input TE and true behaviour *//
  m_cutCounter++;

  float threshold = 0;

  for (const TrigMuonEFInfo* pMuonEFInfo : *trigMuonEFInfoColl) {

    const TrigMuonEFInfoTrackContainer* trigMuonEFInfoTrackContainer = pMuonEFInfo->TrackContainer();

    if (!(trigMuonEFInfoTrackContainer->size())) {

      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "No TrigMuonEFInfoTrack to analyse" << endmsg;

      continue;
    }

    const TrigMuonEFInfoTrack* infoTrack = (*trigMuonEFInfoTrackContainer->begin());
    if (infoTrack->MuonType() != 9) continue;

    const TrigMuonEFCbTrack* combinedTrack = nullptr;
    if (pMuonEFInfo->hasLegacyTrack())
      combinedTrack = pMuonEFInfo->legacyCombinedTrack();
    else
      combinedTrack = pMuonEFInfo->TrackContainer()->front()->CombinedTrack();

    //* Fill monitoring histos
    m_pt  = combinedTrack->pt()  ? combinedTrack->pt()  : -999;
    m_eta = combinedTrack->eta() ? combinedTrack->eta() : -999;
    m_phi = combinedTrack->eta() ? combinedTrack->phi() : -999;

    double eta=-log(sqrt(1+(combinedTrack->cotTh())*(combinedTrack->cotTh()))-(combinedTrack->cotTh()));
    float absEta = fabs(eta);

    for (std::vector<float>::size_type k=0; k<m_bins; ++k)
      if (absEta > m_ptBins[k] && absEta < m_ptBins[k+1])
	threshold = m_ptThresholds[k]; 

    if (fabs(combinedTrack->pt())/CLHEP::GeV > (threshold/CLHEP::GeV)) 
      result = true;

    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " REGTEST muon pt is " << combinedTrack->pt()/CLHEP::GeV << " GeV " << " with Charge " << combinedTrack->charge()
	    << " and threshold cut is " << threshold/CLHEP::GeV << " GeV" << " so hypothesis is " << (result?"true":"false") << endmsg;

  }

  if(result) m_cutCounter++;
  pass = result;

  return HLT::OK;
}  


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigMuTagIMOHypo::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigMuTagIMOHypo" << endmsg;

  return HLT::OK;
}

