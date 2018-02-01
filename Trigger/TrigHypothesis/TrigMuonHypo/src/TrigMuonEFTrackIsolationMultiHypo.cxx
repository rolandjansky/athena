/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigMuonHypo/TrigMuonEFTrackIsolationMultiHypo.h"

#include "xAODMuon/MuonContainer.h"

#include "xAODTrigger/TrigPassBits.h"

class ISvcLocator;

/**
 * Constructor.
 * Declare variables to get from job options and
 * the monitored variables.
 */ 
TrigMuonEFTrackIsolationMultiHypo::TrigMuonEFTrackIsolationMultiHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator){
  
  declareProperty("AcceptAll", m_acceptAll=true);
  declareProperty("DoAbsCut", m_abscut=true); //true for absolute cuts, false for sumpt/pt
  declareProperty("useVarIso", m_useVarIso=false); //true for offline isolation variables, false for online
  declareProperty("RequireCombinedMuon", m_requireCombined=true); // true unless doing ms-only iso
  declareProperty("PtCone02Cut",m_ptcone02_cut=-1.0); //convention is < 0 means don't cut
  declareProperty("PtCone03Cut",m_ptcone03_cut=-1.0); //convention is < 0 means don't cut
  declareProperty("PtThresholds",m_ptThresholds); //convention is < 0 means don't cut
	
  declareMonitoredStdContainer("PtCone02", m_fex_ptcone02,  IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("PtCone03", m_fex_ptcone03,  IMonitoredAlgo::AutoClear);
}

/**
 * Destructor. 
 */
TrigMuonEFTrackIsolationMultiHypo::~TrigMuonEFTrackIsolationMultiHypo() {}

/**
 * Initialize the algorithm.
 * Here we print out the cut values.
 */
HLT::ErrorCode TrigMuonEFTrackIsolationMultiHypo::hltInitialize(){

  if(m_acceptAll) {
    ATH_MSG_INFO("Accepting all the events with not cut!");
  } else {
    if(m_ptcone02_cut < 0.0 && m_ptcone03_cut < 0.0) {
      ATH_MSG_FATAL("Configured to apply cuts, but not cut was specified");	
      return HLT::BAD_JOB_SETUP;	 
    }
    if(m_ptcone02_cut > 0.0) {
      if(m_abscut) {
	ATH_MSG_INFO("Requiring sum pT in 0.2 cone < " << m_ptcone02_cut.value() << " MeV");
      } else {
	ATH_MSG_INFO("Requiring sum pT in 0.2 cone / muon pT < " << m_ptcone02_cut.value());
      }//relative cut
    }
    if(m_ptcone03_cut > 0.0) {
      if(m_abscut) {
	ATH_MSG_INFO("Requiring sum pT in 0.3 cone < " << m_ptcone03_cut.value() << " MeV");
      } else {
	ATH_MSG_INFO("Requiring sum pT in 0.3 cone / muon pT < " << m_ptcone03_cut.value());
      }//relative cut
    }//cut on 0.3 cone
  }
    
  
  return HLT::OK;
}

/**
 * Finalize the algorithm - nothing to do here yet.
 */
HLT::ErrorCode TrigMuonEFTrackIsolationMultiHypo::hltFinalize()
{	
  return HLT::OK;
}

/**
 * Execute the algorithm.
 * Here we apply the cuts to the muon(s).
 * The hypo passes if at least one muon passes the cuts.
 */
HLT::ErrorCode TrigMuonEFTrackIsolationMultiHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {

  if(msgLvl() <= MSG::DEBUG) ATH_MSG_DEBUG("in execute()");

  pass = false; // fail by default

  if(m_acceptAll) {
    pass = true;
    if(msgLvl() <= MSG::DEBUG) {
      ATH_MSG_DEBUG("Accept property is set: taking all the events");
    }
    return HLT::OK;
  }//acceptAll
  
  const bool debug = msgLvl() <= MSG::DEBUG;

  // Some debug output:
  if(debug) ATH_MSG_DEBUG("outputTE->ID(): " << outputTE->getId());

  // Get the muon container from the outputTE
  const xAOD::MuonContainer* muonContainer(0);
  if(getFeature(outputTE, muonContainer)!=HLT::OK || muonContainer==0) {
    if (debug) ATH_MSG_DEBUG("no MuonContainer Feature found");
    return HLT::MISSING_FEATURE;
  }

  // make pass bits object to store the result per muon
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::MuonContainer>(muonContainer);
  std::deque<float> passedPts;
  for(auto muon : *muonContainer) {
    
    const xAOD::Muon::MuonType muontype = muon->muonType();
    if(m_requireCombined && muontype != xAOD::Muon::MuonType::Combined && muontype != xAOD::Muon::MuonType::SegmentTagged ) continue;
    else if(muontype != xAOD::Muon::MuonType::MuonStandAlone && muontype != xAOD::Muon::MuonType::Combined && muontype != xAOD::Muon::MuonType::SegmentTagged) continue;

    float ptcone20(-1), ptcone30(-1);
    bool res = false; 
    if(m_useVarIso){
      res = muon->isolation(ptcone20, xAOD::Iso::IsolationType::ptvarcone20);
      if(!res) 
        ATH_MSG_WARNING("Problem accessing ptvarcone20, " << ptcone20);
    }
    else{
      res = muon->isolation(ptcone20, xAOD::Iso::IsolationType::ptcone20);
      if(!res) 
        ATH_MSG_WARNING("Problem accessing ptcone20, " << ptcone20);
    }
    if(m_useVarIso){
      res = muon->isolation(ptcone30, xAOD::Iso::IsolationType::ptvarcone30);
      if(!res) 
        ATH_MSG_WARNING("Problem accessing ptvarcone30, " << ptcone30);
    }
    else{
      res = muon->isolation(ptcone30, xAOD::Iso::IsolationType::ptcone30);
      if(!res) 
        ATH_MSG_WARNING("Problem accessing ptcone30, " << ptcone30);
    }

    // monitoring
    m_fex_ptcone02.push_back(ptcone20/1000.0);
    m_fex_ptcone03.push_back(ptcone30/1000.0);
    
    bool goodmu=true;
    
    //absolute cut 
    if(m_abscut) {
      if(m_ptcone02_cut > 0.0) {
	if( ptcone20 >= m_ptcone02_cut ) goodmu=false;
      }
      if(m_ptcone03_cut > 0.0) {
	if( ptcone30 >= m_ptcone03_cut )
	  goodmu=false;
      }
      
      if(debug) {
	ATH_MSG_DEBUG("Muon with pT cone 0.2 = " << ptcone20 << ", pT cone 0.3 = " << ptcone30 << " so result for this muon is " << (goodmu?"true":"false"));
      } 
    }//absolute cut      
    else { //relative cut
      
      const double mupt=muon->pt();
      ATH_MSG_DEBUG("Muon with pT " << mupt);
	
      // now we can make the cut(s)
      if(m_ptcone02_cut > 0.0) {
	if( ptcone20/mupt >= m_ptcone02_cut ) goodmu=false;
      }
      if(m_ptcone03_cut > 0.0) {
	if( ptcone30/mupt >= m_ptcone03_cut ) goodmu=false;
      }
      
      if(debug) {
	ATH_MSG_DEBUG("Muon with pT cone 0.2 / pt = " << ptcone20/mupt << ", pT cone 0.3 / pt = " << ptcone30/mupt << " so result for this muon is " << (goodmu?"true":"false"));
      } 
      
    }//relative cut
    
    
    if(goodmu) {
      passedPts.push_back(muon->pt());
      xBits->markPassing(muon, muonContainer,true); // set bit for this muon in TrigPassBits mask
    }//muon passed
  }//loop over isolation objects
  
  pass = checkThresholds(passedPts);
  if(debug) {
    ATH_MSG_DEBUG("Algo result = " << (pass?"true":"false"));
  }
			 
  // store TrigPassBits result
  if ( attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK ) {
    ATH_MSG_ERROR("Could not store TrigPassBits! ");
  }

  return HLT::OK;
}//hltExecute

bool TrigMuonEFTrackIsolationMultiHypo::checkThresholds(std::deque<float>& passedPts){
  if(passedPts.size() < m_ptThresholds.size())
    return false;
  std::sort(passedPts.begin(), passedPts.end(), std::greater<>());
  for(size_t i = 0; i < m_ptThresholds.size(); ++i){
    if(passedPts.at(i) < m_ptThresholds.at(i))
      return false;
  }
  return true;
}
