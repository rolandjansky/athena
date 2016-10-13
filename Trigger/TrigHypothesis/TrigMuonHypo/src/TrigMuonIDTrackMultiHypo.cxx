/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <set>
#include <cmath>

#include "TrigMuonHypo/TrigMuonIDTrackMultiHypo.h"

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "TrigT1Interfaces/RecMuonRoI.h"

class ISvcLocator;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuonIDTrackMultiHypo::TrigMuonIDTrackMultiHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty("AcceptAll",        m_acceptAll       = false);         //pvn true -> false

  declareProperty("UseMuon",        m_UseMuon       = false);

  std::vector<float> def_bins;
  def_bins.push_back(0);
  def_bins.push_back(10);
  std::vector<float> def_thrs;
  def_thrs.push_back(2.5 * CLHEP::GeV);
  declareProperty("PtBins1",   m_ptBins1   = def_bins);
  declareProperty("PtBins2",   m_ptBins2   = def_bins);
  declareProperty("PtBins3",   m_ptBins3   = def_bins);
  declareProperty("PtThresholds1", m_ptThresholds1 = def_thrs);
  declareProperty("PtThresholds2", m_ptThresholds2 = def_thrs);
  declareProperty("PtThresholds3", m_ptThresholds3 = def_thrs);
  declareProperty("Multiplicity1", m_Multiplicity1 = 0);
  declareProperty("Multiplicity2", m_Multiplicity2 = 0);
  declareProperty("Multiplicity3", m_Multiplicity3 = 0);
  declareProperty("LowMassCut",    m_LowMassCut   = -1); // not yet implemented
  declareProperty("HighMassCut",   m_HighMassCut   = -1); // not yet implemented
  declareProperty("TrkAlgo",    m_TrkAlgo = "InDetTrigTrackingxAODCnv_Muon_FTF");

  declareProperty("UseMuRoiDrOnly", m_UseMuRoiDrOnly = false);
  declareProperty("UseMuRoiDr", m_UseMuRoiDr = false);
  declareProperty("MuRoiDrMax", m_MuRoiDrMax = -1);
  declareProperty("MuRoiDrMin", m_MuRoiDrMin = -1);


  declareMonitoredStdContainer("nTracks",   m_mon_nTracks);
  declareMonitoredStdContainer("TrkPt",   m_mon_TrkPt);
  declareMonitoredStdContainer("MuRoiDr",   m_mon_MuRoiDr);
  // declareMonitoredStdContainer("invMass",   m_mon_invMass); // not yet implemented
}

TrigMuonIDTrackMultiHypo::~TrigMuonIDTrackMultiHypo() {
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonIDTrackMultiHypo::hltInitialize()
{
  msg() << MSG::INFO << "in hltInitialize : " << name() << endreq;
   
  if( m_acceptAll ) {
    msg() << MSG::INFO << "Accepting all the events without cut!" << endreq;
  }

  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonIDTrackMultiHypo::hltFinalize()
{
  msg() << MSG::INFO << "in hltFinalize : " << name() << endreq;
  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonIDTrackMultiHypo::hltBeginRun()
{
  msg() << MSG::INFO << "in hltBeginRun : " << name() << endreq;
  msg() << MSG::INFO << "AcceptAll : " << m_acceptAll << endreq;
  
  if (m_acceptAll) {
    msg() << MSG::INFO
	  << "Accepting all the events with not cut!"
	  << endreq;
  } else {

    msg() << MSG::INFO << "UseMuon = " << m_UseMuon << endreq;

    msg() << MSG::INFO << "TrkAlg = " << m_TrkAlgo << endreq;

    auto bins1 = m_ptBins1.size() - 1;
    if (bins1 != m_ptThresholds1.size()) {
      msg() << MSG::INFO << "bad thresholds setup for #1.... exiting!" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }else{
      msg() << MSG::INFO << "track multiplicity for #1 is " << m_Multiplicity1 << endreq;
      for (std::vector<float>::size_type i = 0; i < bins1; ++i) {
	msg() << MSG::INFO
	      << "bin " << m_ptBins1[i] << " - " <<  m_ptBins1[i + 1]
	      << " with Pt Threshold of " << (m_ptThresholds1[i]) / CLHEP::GeV
	      << " GeV" << endreq;
      }
    }

    auto bins2 = m_ptBins2.size() - 1;
    if (bins2 != m_ptThresholds2.size()) {
      msg() << MSG::INFO << "bad thresholds setup for #2.... exiting!" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }else{
      msg() << MSG::INFO << "track multiplicity for #2 is " << m_Multiplicity2 << endreq;
      for (std::vector<float>::size_type i = 0; i < bins2; ++i) {
	msg() << MSG::INFO
	      << "bin " << m_ptBins2[i] << " - " <<  m_ptBins2[i + 1]
	      << " with Pt Threshold of " << (m_ptThresholds2[i]) / CLHEP::GeV
	      << " GeV" << endreq;
      }
    }
    
    auto bins3 = m_ptBins3.size() - 1;
    if (bins3 != m_ptThresholds3.size()) {
      msg() << MSG::INFO << "bad thresholds setup for #3.... exiting!" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }else{
      msg() << MSG::INFO << "track multiplicity for #3 is " << m_Multiplicity3 << endreq;
      for (std::vector<float>::size_type i = 0; i < bins3; ++i) {
	msg() << MSG::INFO
	      << "bin " << m_ptBins3[i] << " - " <<  m_ptBins3[i + 1]
	      << " with Pt Threshold of " << (m_ptThresholds3[i]) / CLHEP::GeV
	      << " GeV" << endreq;
      }
    }
    
    msg() << MSG::INFO << "LowMassCut : " << m_LowMassCut << " GeV" << endreq;
    msg() << MSG::INFO << "HighMassCut : " << m_HighMassCut << " GeV" << endreq;


    msg() << MSG::INFO << "UseMuRoiDrOnly: " << m_UseMuRoiDrOnly << endreq;
    msg() << MSG::INFO << "UseMuRoiDr: " << m_UseMuRoiDr << endreq;
    msg() << MSG::INFO << "MuRoiDrMin: " << m_MuRoiDrMin << endreq;
    msg() << MSG::INFO << "MuRoiDrMax: " << m_MuRoiDrMax << endreq;
    
  }

   msg() << MSG::INFO
         << "Initialization completed successfully"
         << endreq;

  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonIDTrackMultiHypo::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out)
{

  bool debug = msgLvl() <= MSG::DEBUG;

  if(debug) msg() << MSG::DEBUG << "in hltExecute : " << name() << endreq;
  
  if(m_acceptAll) {
    if( debug ) {
      msg() << MSG::DEBUG
	    << "Accept property is set: taking all the events"
	    << endreq;
    }
    return HLT::OK;
  }


  
  // reset monitoring variables
  bool doMonitor = true;
  if(doMonitor){
    beforeExecMonitors().ignore();
    m_mon_nTracks.clear();
    m_mon_TrkPt.clear();
    m_mon_MuRoiDr.clear();
    // m_mon_invMass.clear();
  }
  
  // ---
  // collect all TEs and create output TEs with ActiveState=true
  // ---
  
  std::vector<HLT::TriggerElement*> vec_allTEs;
  std::vector<HLT::TriggerElement*> vec_outputTEs;
  
  unsigned int tes_in_size = tes_in.size();
  
  if(debug) msg() << MSG::DEBUG << "tes_in size=" << tes_in_size << endreq;
  for(unsigned int i_vec=0; i_vec<tes_in_size; i_vec++) {
    unsigned int n_te = tes_in[i_vec].size();
    if(debug) msg() << MSG::DEBUG << "i_vec=" << i_vec << " : n TEs=" << n_te << endreq;
    for(unsigned i_te=0; i_te<n_te; i_te++) {
      HLT::TriggerElement* te = tes_in[i_vec][i_te];
      if( debug ) {
	std::string label;
	TrigConf::HLTTriggerElement::getLabel (te->getId(), label );
	if(debug) msg() << MSG::DEBUG << "input TE ID(): " << te->getId() << ", Label=" << label << endreq;
      }      
      bool alreadyThere = false;
      for(unsigned int j_te=0; j_te<vec_allTEs.size(); j_te++) {
	if( vec_allTEs[j_te] == te ) {
	  if(debug) msg() << MSG::DEBUG << "at i_vec=" << i_vec << ", i_te=" << i_te <<
		      ": same TE already there at j_te=" << j_te << ", so, skip this TE." << endreq;
	  alreadyThere = true;
	  break;
	}
      }
      if( ! alreadyThere ) vec_allTEs.push_back(te);
    }
  }
  
  unsigned int n_allTEs = vec_allTEs.size(); 
  if(debug) msg() << MSG::DEBUG << "size of vec_allTEs=" << n_allTEs << endreq;
  for(unsigned i_te=0; i_te<n_allTEs; i_te++) {
    HLT::TEVec tes;
    tes.push_back(vec_allTEs[i_te]);
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(tes, type_out);
    if(debug) {
      std::string label;
      TrigConf::HLTTriggerElement::getLabel (outputTE->getId(), label);
      msg() << MSG::DEBUG << "creating outputTE ID(): " << outputTE->getId() << ", Label=" << label << endreq;
    }
    outputTE->setActiveState(true);
    vec_outputTEs.push_back(outputTE);
  }

  bool errorWhenGettingELs = false;
  std::vector<std::vector<const xAOD::TrackParticleContainer*> > IDTrackELVec; 
  std::vector<std::vector<const xAOD::MuonContainer*> > MuTrackELVec; 
  std::vector<std::vector<const LVL1::RecMuonRoI*> > MuRoiELVec; 
  for(unsigned i_te=0; i_te<n_allTEs; i_te++) {
    std::vector<const xAOD::TrackParticleContainer*> IDTrackEL;
    std::vector<const xAOD::MuonContainer*> MuTrackEL;
    std::vector<const LVL1::RecMuonRoI*> MuRoiEL;
    if( !m_UseMuRoiDrOnly ){
      HLT::ErrorCode isIDTrkExtrOK = (m_UseMuon) ? getFeatures(vec_allTEs[i_te], MuTrackEL) : getFeatures(vec_allTEs[i_te], IDTrackEL, m_TrkAlgo);
      if( isIDTrkExtrOK != HLT::OK ) {
	errorWhenGettingELs = true;
	msg() << MSG::WARNING << "i_te=" << i_te << ": fails to find EL for: ID(Mu)Track" << endreq;
	continue;
      }
      if( !m_UseMuon && IDTrackEL.empty() ) {
	errorWhenGettingELs = true;
	msg() << MSG::WARNING << "i_te=" << i_te << ": EL not valid for: IDTrack" << endreq;
	continue;
      }
      if( m_UseMuon && MuTrackEL.empty() ) {
	errorWhenGettingELs = true;
	msg() << MSG::WARNING << "i_te=" << i_te << ": EL not valid for: MuTrack" << endreq;
	continue;
      }
      if(debug) msg() << MSG::DEBUG << "IDTrackEL is OK for i_te=" << i_te << " size of IDTrackEL is " << IDTrackEL.size() << endreq;
      if(debug) msg() << MSG::DEBUG << "MuonTrackEL is OK for i_te=" << i_te << " size of MuonTrackEL is " << MuTrackEL.size() << endreq;
    }
    if( m_UseMuRoiDr ){
      HLT::ErrorCode isMuRoiExtrOK = getFeatures(vec_allTEs[i_te], MuRoiEL);
      if( isMuRoiExtrOK != HLT::OK ) {
	errorWhenGettingELs = true;
	msg() << MSG::WARNING << "i_te=" << i_te << ": fails to find EL for: MuonRoI" << endreq;
	continue;
      }
      if( MuRoiEL.empty() ) {
	errorWhenGettingELs = true;
	msg() << MSG::WARNING << "i_te=" << i_te << ": EL not valid for: MuRoI" << endreq;
	continue;
      }
      if(debug) msg() << MSG::DEBUG << "MuRoiEL is OK for i_te=" << i_te << " size of MuRoiEL is " << MuRoiEL.size() << endreq;
    }
    if( !m_UseMuRoiDrOnly ){
      if(!m_UseMuon)IDTrackELVec.push_back(IDTrackEL);
      if( m_UseMuon)MuTrackELVec.push_back(MuTrackEL);
    }
    if( m_UseMuRoiDr )
      MuRoiELVec.push_back(MuRoiEL);
  }
  
  if( errorWhenGettingELs ) {
    msg() << MSG::WARNING << "error when getting ELs. exitting with all TEs deactive..." << endreq;
    if(doMonitor)afterExecMonitors().ignore();
    for(unsigned i_te=0; i_te<n_allTEs; i_te++) 
      vec_outputTEs[i_te]->setActiveState(false);
    return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
  }
  
  
  
  if( m_UseMuRoiDr && (m_MuRoiDrMax > 0 || m_MuRoiDrMin > 0) ){
    std::vector<float> roi_eta;
    std::vector<float> roi_phi;
    if (debug) msg() << MSG::DEBUG << "looking at dR between muon RoIs" << endreq;
    for(unsigned i_te=0; i_te<n_allTEs; i_te++) {
      std::vector<const LVL1::RecMuonRoI*> VecMuRoiCont = MuRoiELVec[i_te];
      unsigned int total_entries_vecroi = VecMuRoiCont.size();
      if (debug) msg() << MSG::DEBUG << "total_entries_roi = " << total_entries_vecroi << endreq;
      for (unsigned int ivroi=0; ivroi<total_entries_vecroi; ivroi++){
    	const LVL1::RecMuonRoI* VecMuRoi1 = VecMuRoiCont[ivroi];
    	if( !VecMuRoi1 ){
    	  if (debug) msg() << MSG::DEBUG << "failed: VecMuRoi1 is empty" << endreq;
    	  continue;
    	}
	roi_eta.push_back( VecMuRoi1->eta() );
	roi_phi.push_back( VecMuRoi1->phi() );
      }
    }
    bool dr_ok = false;
    unsigned int nrois = roi_eta.size();
    for(unsigned int iroi=0;iroi<nrois;iroi++) {
      if(dr_ok)break;
      for(unsigned int jroi = iroi ; jroi < roi_eta.size() ; jroi++){
	if(iroi==jroi)continue;
	if(dr_ok)break;
	float dR = deltaR( roi_eta.at(iroi), roi_phi.at(iroi),
			   roi_eta.at(jroi), roi_phi.at(jroi)  );
	float max = (m_MuRoiDrMax>0)?(m_MuRoiDrMax):(99999);
	float min = (m_MuRoiDrMin>0)?(m_MuRoiDrMin):(0);
	if( dR < max && dR > min ) dr_ok = true;
	if(debug) msg() << MSG::DEBUG << "dR check: dR = " << dR << " : requirement=[" << m_MuRoiDrMin << "," << m_MuRoiDrMax << "] pass=" << dr_ok << endreq;
	if(doMonitor)m_mon_MuRoiDr.push_back( dR );	
      }
    }
    if( !dr_ok ){
      if(debug) msg() << MSG::DEBUG << "failing dR(two RoIs) requirement [" << m_MuRoiDrMin << "," << m_MuRoiDrMax << "]" << endreq;
      for(unsigned i_te=0; i_te<n_allTEs; i_te++) 
	vec_outputTEs[i_te]->setActiveState(false);
      if(doMonitor)afterExecMonitors().ignore();
      return HLT::OK;
    }else{
      if(debug) msg() << MSG::DEBUG << "at least one combination of two RoIs has passed dR requirement [" << m_MuRoiDrMin << "," << m_MuRoiDrMax << "]" << endreq;
    }
  }

  if( (m_Multiplicity1 <= 0 && m_Multiplicity2 <= 0 && m_Multiplicity3 <= 0) || m_UseMuRoiDrOnly ){
    if(debug) msg() << MSG::DEBUG << "no track multiplicity requirement" << endreq;
    if(debug) msg() << MSG::DEBUG << "event passed" << endreq;
    if(doMonitor)afterExecMonitors().ignore();
    return HLT::OK;
  }
  
  int n_tracks = 0;
  std::vector<float> ptOv;
  std::vector<float> etaOv;
  std::vector<float> phiOv;
  int n_pass_pt1 = 0;
  int n_pass_pt2 = 0;
  int n_pass_pt3 = 0;
  
  for(unsigned i_te=0; i_te<n_allTEs; i_te++) {
    std::vector<const xAOD::TrackParticleContainer*> VecIDTrkCont;
    std::vector<const xAOD::MuonContainer*>          VecMuTrkCont;
    if(!m_UseMuon)VecIDTrkCont = IDTrackELVec[i_te];
    if( m_UseMuon)VecMuTrkCont = MuTrackELVec[i_te];
    unsigned int total_entries_trk = (m_UseMuon) ? VecMuTrkCont.size() : VecIDTrkCont.size();
    if (debug) msg() << MSG::DEBUG << "total_entries_trk = " << total_entries_trk << endreq;
    for (unsigned int ivtrk=0; ivtrk<total_entries_trk; ivtrk++){
      const xAOD::TrackParticleContainer* IDTracks1 = (m_UseMuon) ? NULL : VecIDTrkCont[ivtrk];
      const xAOD::MuonContainer*          MuTracks1 = (m_UseMuon) ? VecMuTrkCont[ivtrk] : NULL;
      if( !m_UseMuon && !IDTracks1 ){
  	if (debug) msg() << MSG::DEBUG << "failed: IDTracks1 is empty" << endreq;
  	continue;
      }
      if( m_UseMuon && !MuTracks1 ){
  	if (debug) msg() << MSG::DEBUG << "failed: MuTracks1 is empty" << endreq;
  	continue;
      }
      unsigned int total_entries = (m_UseMuon) ? MuTracks1->size() : IDTracks1->size();
      if (debug) msg() << MSG::DEBUG << "total_entries = " << total_entries << endreq;
      for (unsigned int itrk = 0;itrk<total_entries;itrk++){
	const xAOD::TrackParticle* track1 = (m_UseMuon) ? NULL : IDTracks1->at(itrk);
	const xAOD::Muon* muon1 = (m_UseMuon) ? MuTracks1->at(itrk) : NULL;
	if( !m_UseMuon && !track1 ){
	  if (debug) msg() << MSG::DEBUG << "failed: track1 is empty" << endreq;
	  continue;
	}
	if( m_UseMuon && !muon1 ){
	  if (debug) msg() << MSG::DEBUG << "failed: muon1 is empty" << endreq;
	  continue;
	}
	
  	float trk_pt = 0;
  	float trk_qpt = 0;
  	float trk_eta = 0;
  	float trk_phi = 0;
  	float trk_z0  = 0;
  	float trk_d0  = 0;
	if(m_UseMuon){
	  trk_pt = fabs( muon1->pt() );
	  trk_qpt = trk_pt * muon1->charge();
	  trk_eta = muon1->eta();
	  trk_phi = muon1->phi();
	}else{
	  trk_pt = fabs( track1->pt() );
	  trk_qpt = trk_pt * track1->charge();
	  trk_eta = track1->eta();
	  trk_phi = track1->phi();
	  trk_z0  = track1->z0();
	  trk_d0  = track1->d0();
	}
	if(debug) msg() << MSG::DEBUG <<
		    " TE=" << i_te <<
		    " iTrkCont=" << ivtrk <<
		    " iTrk=" << itrk <<
		    " qpt=" << trk_qpt <<
		    " eta=" << trk_eta <<
		    " phi=" << trk_phi <<
		    " z0=" << trk_z0 <<
		    " d0=" << trk_d0 << endreq;
	bool already = false;
	for(unsigned int i = 0 ; i < ptOv.size() ; i++){
	  double dr = deltaR( etaOv.at(i), phiOv.at(i), trk_eta, trk_phi);
	  if( dr < 0.001 ){
	    already = true;
	    break;
	  }
	}
	if( already ){
	  if(debug) msg() << MSG::DEBUG << "already used" << endreq;
	  continue;
	}
	ptOv.push_back(trk_pt);
	etaOv.push_back(trk_eta);
	phiOv.push_back(trk_phi);

	float absEta = fabs(trk_eta);
	bool pass_this_track = false;

	if( m_Multiplicity1 > 0 ){
	  float threshold = 9999999;
	  for (std::vector<float>::size_type i = 0; i < m_ptBins1.size()-1; ++i)
	    if (absEta > m_ptBins1[i] && absEta < m_ptBins1[i + 1])
	      threshold = m_ptThresholds1[i];
	  if(debug) msg() << MSG::DEBUG << "threshold1 = " << threshold << endreq;
	  if( trk_pt > threshold ) {
	    n_pass_pt1++;
	    pass_this_track = true;
	    if(debug) msg() << MSG::DEBUG << "pt cut1 passed" << endreq;
	  }else{
	    if(debug) msg() << MSG::DEBUG << "pt cut1 failed" << endreq;
	  }
	}



	if( m_Multiplicity2 > 0 ){
	  float threshold = 9999999;
	  for (std::vector<float>::size_type i = 0; i < m_ptBins2.size()-1; ++i)
	    if (absEta > m_ptBins2[i] && absEta < m_ptBins2[i + 1])
	      threshold = m_ptThresholds2[i];
	  if(debug) msg() << MSG::DEBUG << "threshold2 = " << threshold << endreq;
	  if( trk_pt > threshold ) {
	    n_pass_pt2++;
	    pass_this_track = true;
	    if(debug) msg() << MSG::DEBUG << "pt cut2 passed" << endreq;
	  }else{
	    if(debug) msg() << MSG::DEBUG << "pt cut2 failed" << endreq;
	  }
	}


	if( m_Multiplicity3 > 0 ){
	  float threshold = 9999999;
	  for (std::vector<float>::size_type i = 0; i < m_ptBins3.size()-1; ++i)
	    if (absEta > m_ptBins3[i] && absEta < m_ptBins3[i + 1])
	      threshold = m_ptThresholds3[i];
	  if(debug) msg() << MSG::DEBUG << "threshold3 = " << threshold << endreq;
	  if( trk_pt > threshold ) {
	    n_pass_pt3++;
	    pass_this_track = true;
	    if(debug) msg() << MSG::DEBUG << "pt cut3 passed" << endreq;
	  }else{
	    if(debug) msg() << MSG::DEBUG << "pt cut3 failed" << endreq;
	  }
	}

	if( pass_this_track ){
	  if(doMonitor)m_mon_TrkPt.push_back( trk_pt / CLHEP::GeV );
	  n_tracks++;
	}
      }
    }
  }

  
  if(doMonitor)m_mon_nTracks.push_back(n_tracks);

  bool pass_multiplicity = true;
  if( n_pass_pt1 < m_Multiplicity1 )pass_multiplicity=false;
  if( n_pass_pt2 < m_Multiplicity2 )pass_multiplicity=false;
  if( n_pass_pt3 < m_Multiplicity3 )pass_multiplicity=false;
  if(pass_multiplicity == false){
    if(debug) msg() << MSG::DEBUG << "event failed -> deactivating all TEs" << endreq;
    for(unsigned i_te=0; i_te<n_allTEs; i_te++) 
      vec_outputTEs[i_te]->setActiveState(false);
  }else{
    if(debug) msg() << MSG::DEBUG << "event passed" << endreq;
  }
  if(debug) msg() << MSG::DEBUG << "finish TrigMuonIDTrackMultiHypo" << endreq;
  if(doMonitor)afterExecMonitors().ignore();
  return HLT::OK;
}  


// --------------------------------------------------------------------------------
float TrigMuonIDTrackMultiHypo::deltaPhi(const float& v1, const float& v2){
  double dPhi = v1 - v2;
  while(dPhi>M_PI)  dPhi -= 2.0 * M_PI;
  while(dPhi<-M_PI) dPhi += 2.0 * M_PI;
  dPhi = fabs( dPhi );
  return dPhi;
}
float TrigMuonIDTrackMultiHypo::deltaEta(const float& v1, const float& v2){
  double dEta = v1 - v2;
  dEta = fabs( dEta );
  return dEta;
}
float TrigMuonIDTrackMultiHypo::deltaR(const float& eta1, const float& phi1,
				       const float& eta2, const float& phi2 ){
  double dEta = deltaEta( eta1, eta2 );
  double dPhi = deltaPhi( phi1, phi2 );
  double dR = sqrt( dEta*dEta + dPhi*dPhi);
  return dR;
}
