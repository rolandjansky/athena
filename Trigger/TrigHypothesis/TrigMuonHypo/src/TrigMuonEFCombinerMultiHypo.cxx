/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuonEFCombinerMultiHypo.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

TrigMuonEFCombinerMultiHypo::TrigMuonEFCombinerMultiHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator){
  declareProperty("AcceptAll", m_acceptAll=true);
  declareProperty("RejectCBmuons", m_rejectCBmuons=false);
  std::vector<float> def_bins;
  def_bins.push_back(0.);
  def_bins.push_back(9.9);
  std::vector<float> def_thrs;
  def_thrs.push_back(0.*CLHEP::GeV);
  std::vector<unsigned int> def_mult;
  def_mult.push_back(0);
  m_Nmult = 0;
  declareProperty("PtBins", m_ptBins=def_bins);
  declareProperty("PtThresholds", m_ptThresholds=def_thrs);
  declareProperty("PtMultiplicity", m_ptMultiplicity=def_mult);
  declareProperty("DoNscan", m_nscan=false);
  declareProperty("ConeSize", m_conesize=0.3);
  declareMonitoredStdContainer("Pt",  m_fex_pt);
  declareMonitoredStdContainer("Eta", m_fex_eta);
  declareMonitoredStdContainer("Phi", m_fex_phi);
  declareMonitoredStdContainer("Nmuons", m_fex_nmuons);

  m_bins = 0;
  m_mult = 0;
  m_storeGate=0;

}

TrigMuonEFCombinerMultiHypo::~TrigMuonEFCombinerMultiHypo(){
}

HLT::ErrorCode TrigMuonEFCombinerMultiHypo::hltInitialize(){

  msg() << MSG::DEBUG << "TrigMuonEFCombinerMultiHypo::hltInitialize()" << endreq; 
  
  if(m_acceptAll) {
    msg() << MSG::INFO
	  << "Accepting all the events with not cut!"
	  << endreq;
  } else {
    m_mult = m_ptMultiplicity.size();
    m_bins = m_ptBins.size();
    m_Nmult  = *max_element(m_ptMultiplicity.begin(), m_ptMultiplicity.end())
      - *min_element(m_ptMultiplicity.begin(), m_ptMultiplicity.end()) + 1;  // determine muon multiplicity  

    // Print pt threshold info
    msg() << MSG::INFO << "Pt threshold multiplicity = " << m_Nmult << endreq; 
    unsigned int n_offset=0;
    unsigned int mult = m_ptMultiplicity[0];
    m_masks.push_back( 1 << mult );
    for (std::vector<float>::size_type i=0; i<m_mult;++i) {

      if (mult != m_ptMultiplicity[i]) { // check if we cross boundary to next muon
	m_masks.push_back( 1 << mult );
	mult=m_ptMultiplicity[i];
	n_offset++;
      }

      msg() << MSG::INFO
	    << "bin " << m_ptBins[i+n_offset] << " - " <<  m_ptBins[i+n_offset+1]
	    << " with Pt Threshold of " << (m_ptThresholds[i])/CLHEP::GeV
	    << " GeV and multiplity ID : " << m_ptMultiplicity[i] << endreq;
    }

    // check threshold vector sizes
    if (m_mult != m_ptThresholds.size() || m_mult != m_bins - m_Nmult ) {
      msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endreq;
      msg() << MSG::INFO << "m_mult: " << m_mult 
	    << ", m_bins: " << m_bins 
	    << ", m_Nmult: " << m_Nmult << endreq;
      
      return HLT::BAD_JOB_SETUP;
    }

    msg() << MSG::INFO << "DoNscan: "<<m_nscan<<endreq;

  }

  msg() << MSG::INFO
	<< "Initialization completed successfully"
	<< endreq;
	
  return HLT::OK;
}


HLT::ErrorCode TrigMuonEFCombinerMultiHypo::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}


HLT::ErrorCode TrigMuonEFCombinerMultiHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

  m_storeGate = store();

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endreq;
  ATH_MSG_DEBUG("rejectCB muons: "<<m_rejectCBmuons);
  //resetting the monitoring variables
  m_fex_pt.clear();
  m_fex_eta.clear();
  m_fex_phi.clear();
  m_fex_nmuons.clear();
  m_tmp_eta.clear();
  m_tmp_phi.clear();

  if(m_acceptAll) {
    pass = true;
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG
	    << "Accept property is set: taking all the events"
	    << endreq;
    }
    return HLT::OK;
  }

  bool debug = msgLvl() <= MSG::DEBUG;

  // Some debug output:
  if(debug) msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;

  // Get muonContainer linked to the outputTE
  const xAOD::MuonContainer*  muonContainer=0;
  if(getFeature(outputTE, muonContainer)!=HLT::OK) {
    if (debug) msg() << MSG::DEBUG << "no xAOD::MuonContainer Feature found" << endreq;
    return HLT::MISSING_FEATURE;
  } else {
    if (!muonContainer) {
      if (debug) msg() << MSG::DEBUG << "null xAOD::MuonContainer Feature found" << endreq;
      return HLT::MISSING_FEATURE;
    }
    if (debug) msg() << MSG::DEBUG << "xAOD::MuonContainer found with size " << muonContainer->size() << endreq;
  } 

  // init result vector, muon counter
  std::vector<unsigned int> hypo_results;
  std::vector<unsigned int> ismucomb;
  unsigned int mu_count=0;


  // loop on the muons within the RoI
  for (unsigned int j=0; j<muonContainer->size(); j++ ) {
    unsigned int ismuoncomb=0;

    msg() << MSG::DEBUG << "Looking at xAOD::Muon " << j << endreq;
    const xAOD::Muon* muon = muonContainer->at(j);
    if (!muon) {
      if (debug) msg() << MSG::DEBUG << "No xAOD::Muon found." << endreq;
      continue;
    } else {


      if (muon->primaryTrackParticle()) { // was there a muon in this RoI ?


	const xAOD::TrackParticle* tr = muon->trackParticle(xAOD::Muon::CombinedTrackParticle);
	if (!tr) {
	  if (debug) msg() << MSG::DEBUG << "No CombinedTrackParticle found." << endreq;
	  if(m_rejectCBmuons){ 
	    ismuoncomb=0;
	    ismucomb.push_back(ismuoncomb);
	  }
	  continue;
	} else {
	  if (debug) msg() << MSG::DEBUG
			   << "Retrieved combined track with abs pt "
			   << (*tr).pt()/CLHEP::GeV << " GeV. Charge=" << tr->charge() << " and match chi2 = " << tr->chiSquared() << endreq;

	  m_fex_pt.push_back(tr->pt()/CLHEP::GeV*tr->charge());
	  float eta = tr->eta();
	  m_fex_eta.push_back(eta);
	  m_fex_phi.push_back(tr->phi());

	  mu_count++;
	  float absEta = fabs(eta);
	  float threshold = 0;
	  unsigned int tmp = 0;

	  // loop over eta bins for all multiplicities 
	  unsigned int n_offset = 0;
	  unsigned int mult = m_ptMultiplicity[0];
	  // 2012.05.20 kn fix
	  // for (std::vector<float>::size_type k=0; k<=m_mult; ++k) { 
	  for (std::vector<float>::size_type k=0; k<m_mult; ++k) { 

	    // check if we cross boundary to next muon
	    if (mult != m_ptMultiplicity[k]) { 
	      mult=m_ptMultiplicity[k];
	      n_offset++;
	    }

	    // find treshold for eta bin
	    if (absEta > m_ptBins[k+n_offset] && absEta <= m_ptBins[k+n_offset+1]) {
	      threshold = m_ptThresholds[k];
		   
	      // check if muon passes threshold 
	      if (fabs(tr->pt())/CLHEP::GeV > (threshold/CLHEP::GeV)){ 
		unsigned int mult = m_ptMultiplicity[k]; // get multiplicity id
		tmp += 1 << mult;
		if(debug) msg() << MSG::DEBUG << " REGTEST muon pt " << tr->pt()/CLHEP::GeV << " GeV "
				<< " passed threshold, mult=" << mult
				<< ", mask=" << tmp
				<< ", mu_count=" << mu_count 
				<< endreq;
		ismuoncomb=1;

	      }
	      else ismuoncomb=0;
	    }
	  } // end loop over eta bins for all multiplicities
	       
	  hypo_results.push_back( tmp ); // store result
	  if(m_nscan){
	    m_tmp_eta.push_back(tr->eta());
	    m_tmp_phi.push_back(tr->phi());
	  }

	  ismucomb.push_back(ismuoncomb);
	  if(debug) msg() << MSG::DEBUG << " REGTEST muon pt is " << tr->pt()/CLHEP::GeV << " GeV "
			  << " with Charge " << tr->charge()
			  << " and threshold cut is " << threshold/CLHEP::GeV << " GeV"
			  << " so hypothesis mask is " << (tmp) << " for muon #" << mu_count << endreq;
	}
	 
      }
    }
  }

   
  m_fex_nmuons.push_back(mu_count);  // monitoring

   
  if(debug) {
    msg() << MSG::DEBUG << " REGTEST muon masks are " ;
    for (unsigned int i=0; i<mu_count; i++) {
      for (unsigned int j=0; j<m_Nmult; j++) {	 
	if ( m_masks[j] & hypo_results[i]) {
	  msg() << "1";
	}
	else {
	  msg() << "0";
	}
	msg() << " for mask " << m_masks[j] 
	      << "( j= " << j 
	      << ") with hypo_result: " << hypo_results[i]  << ", ";	 
      }
    }
    msg () << endreq;
  }
  // calculate result, assumption is that different thresholds are inclusive.
  // find matching combination of muons to hypos
  // problem equivalent to find rang of matrix 
  // probably faster to do N^M comparisons...

  // first check number of muons.
  pass = true;
  if ( mu_count < m_Nmult && !m_rejectCBmuons) {
    pass = false;
  }
  // number of muons sufficient, check match
  else {
    // find candidates per partial hypo and satisfied partial-hypo per candidate
    std::vector<unsigned int> tmp_hypo(m_Nmult,0);
    std::vector<unsigned int> tmp_mu(mu_count,0);
    for (unsigned int i=0; i<mu_count;i++) {
      tmp_mu[i] = bitsum(hypo_results[i]); // store how many hypo muon candidate satisfies
      for (unsigned int j=0;j<m_Nmult;j++) {
	if ( m_masks[j] & hypo_results[i] ) {
	  if(!m_rejectCBmuons) tmp_hypo[j]++;
	}
      } 
    }
    // check every partial hypo is satisfied at least once.
    for (unsigned int i=0;i<m_Nmult && pass;i++){
      if (!m_rejectCBmuons && tmp_hypo[i]==0) pass = false;
      if(m_rejectCBmuons && ismucomb[i]==1) pass = false;
    }

    //check for narrow scan triggers
    float deta,dphi=10;
    unsigned int nInCone=0;
    int npass=0;
    if(m_nscan && pass){
      for(unsigned int i=0; i<m_tmp_eta.size(); i++){
	if(m_rejectCBmuons && ismucomb[i]==1) continue;
      	nInCone=0;
      	for(unsigned int j=i+1; j<m_tmp_eta.size(); j++){
    	  deta = fabs(m_tmp_eta[i]-m_tmp_eta[j]);
    	  dphi = getdphi(m_tmp_phi[i],m_tmp_phi[j]);
      	  if(deta<2*m_conesize && dphi<2*m_conesize){
      	    nInCone++;
      	  }
      	}
      	if(nInCone>=m_Nmult-1) npass++;
      }
      if(npass<=0) pass=false;
    }

    if (pass) {
      // assumption is that partial hypo's are ranked and inclusive.
      // hypo is satisfied if rank of binary hypo matrix is >= m_Nmult. 
      // since partial hypo is inclusive we can also order the mu candidates
      // and see if diagonal elements are non-zero.

      // order muon candidates tmp_mu
      std::sort(tmp_mu.begin(),tmp_mu.end());
     
      // check diagonal for last m_Nmult elements
      if(!m_rejectCBmuons){
	for (unsigned int i=0; i<m_Nmult && pass; i++) {
	  if ( *(tmp_mu.end()-i-1) < (m_Nmult-i) ) pass = false; 
  
	  if(debug) {
	    msg() << MSG::DEBUG << " REGTEST ranked comparison of muon " 
		  << i << ", hypoes satisfied are " <<  *(tmp_mu.end()-i-1) 
		  << ", required are " << (m_Nmult-i) 
		  << ", pass = " << pass << endreq  ;
	  }
	}
      }
    } // if (pass) ...
  } // else    
 
  return HLT::OK;
}  
 

int TrigMuonEFCombinerMultiHypo::bitsum(unsigned long il) {
  int ret = 0;
  while(il) { 
    ret += il&1;
    il = il >> 1;
  }
  return ret;
}

float TrigMuonEFCombinerMultiHypo::getdphi(float phi1, float phi2){
  float dphi = phi1-phi2;
  if(dphi > TMath::Pi()) dphi -= TMath::TwoPi();
  if(dphi < -1*TMath::Pi()) dphi += TMath::TwoPi();
  return fabs(dphi);
}

