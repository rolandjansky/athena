/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTTauMonTool.cxx
 *
 *    Artur Kalinowski (Artur.Kalinowski@cern.ch)
 *    Based on HLTMonTool example by Christiane Risler and Martin zur Nedden
 *    Maintained by ccuenca, jengbou
 */

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"
#include "AnalysisUtils/AnalysisMisc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/TriggerInfo.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "EventInfo/EventInfo.h"
#include <EventInfo/EventID.h>
#include "xAODEventInfo/EventInfo.h"

#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
//#include "TrigSteeringEvent/TrigOperationalInfo.h"
//#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

//#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include "TrigConfL1Data/PrescaleSet.h"

#include "TrigTauEmulation/Level1EmulationTool.h"
#include "TrigTauEmulation/HltEmulationTool.h"

#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauDefs.h"
#include "xAODTau/TauxAODHelpers.h"

#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/EmTauRoIContainer.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "VxVertex/VxContainer.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TEfficiency.h"
#include "TProfile.h"

#include <vector>
#include <iostream>
#include <fstream>
//#define _USE_MATH_DEFINES
#include <math.h>


#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "HLTTauMonTool.h"


using namespace std;
using namespace AnalysisUtils;

const float PI=2.0*acos(0.);
const float TWOPI=2.0*PI;

/*
StatusCode  HLTTauMonTool::test2StepTracking(){

  ATH_MSG_DEBUG("Executing HLTTauMonTool::test2StepTracking()");

  // Testing track links:
  const xAOD::TauJetContainer * tauPrecisionCont = 0;
  if( evtStore()->retrieve(tauPrecisionCont, "HLT_xAOD__TauJetContainer_TrigTauRecMerged").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve HLT_xAOD__TauJetContainer_TrigTauRecMerged container. Exiting!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Testing 2step tracking loop over TrigTauMerged container...");
  xAOD::TauJetContainer::const_iterator tauPrecision = tauPrecisionCont->begin(),tauPrecisionCont_e = tauPrecisionCont->end();
  for(; tauPrecision!=tauPrecisionCont_e; ++tauPrecision){
    float pt = (*tauPrecision)->pt();
    float eta = (*tauPrecision)->eta();
    float phi = (*tauPrecision)->phi();
    int nTrack = (*tauPrecision)->nTracks();
    ATH_MSG_DEBUG("tauPrecision: pt "<< pt <<", eta " << eta << ", phi " << phi << ", #tracks "<< nTrack);

    bool goodLinks(true);
    std::vector< ElementLink< xAOD::TrackParticleContainer > > myLinks = xAOD::TauHelpers::trackParticleLinks((*tauPrecision), xAOD::TauJetParameters::classifiedCharged);
    ATH_MSG_DEBUG("Size of links for tauPrecision: " << myLinks.size());
    for(unsigned int i=0;i<myLinks.size();++i){
      ATH_MSG_DEBUG("Get the link: " << i);
      ElementLink< xAOD::TrackParticleContainer > theLink = myLinks.at(i);
      ATH_MSG_DEBUG("IsValid: " << theLink.isValid());
      if(!theLink.isValid()) { goodLinks=false; ATH_MSG_WARNING("Track-tau link IsValid: " << theLink.isValid()); continue;}
      ATH_MSG_DEBUG("TheID: " << theLink.dataID());
      ATH_MSG_DEBUG("TheKey: " << theLink.key());
      ATH_MSG_DEBUG("TheIndex: " << theLink.index());    
    }
    if(goodLinks)for (unsigned int i=0;i<(*tauPrecision)->nTracks();++i) {
        const xAOD::TrackParticle* trk = (*tauPrecision)->track(i)->track();
        ATH_MSG_DEBUG("track "<< i <<"-th: index "<< trk->index() << ", d0 " << trk->d0() << ", phi0 "<< trk->phi0() << ", theta " << trk->theta() << ", qOverP  " << trk->qOverP());
    }
  }

  // check that the ptonly chains is fired
  if(!getTDT()->isPassed("HLT_tau25_perf_ptonly",m_HLTTriggerCondition)) return StatusCode::SUCCESS;
  if(!getTDT()->isPassed("L1_TAU12IM",m_L1TriggerCondition)) return StatusCode::SUCCESS; 

  // get list of good offline taus
  std::vector<TLorentzVector> good_offline_taus;

  const xAOD::TauJetContainer* tauCont = 0;
  if( m_storeGate->retrieve(tauCont, "TauJets").isFailure()  ){
	ATH_MSG_WARNING("Failed to retrieve TauJets container. Exiting!");
	//return StatusCode::FAILURE;
  }
  xAOD::TauJetContainer::const_iterator offlinetau,offlinetau_end = tauCont->end();
  for(offlinetau=tauCont->begin();offlinetau!=offlinetau_end; ++offlinetau){
    if((*offlinetau)){ 

    TLorentzVector TauTLV = (*offlinetau)->p4();
    double eta_Tau = TauTLV.Eta();
    if(fabs(eta_Tau) > 2.47) continue;
    double pt_Tau = TauTLV.Pt();
    if(pt_Tau<m_effOffTauPtCut) continue;
    int ntrack_TAU = (*offlinetau)->nTracks();
    if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
    bool good_tau = (*offlinetau)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
    bool best_tau = good_tau;
    if(!best_tau) continue;
    good_offline_taus.push_back(TauTLV);

    }
  }

  ATH_MSG_DEBUG("Number of good offline taus: "<<good_offline_taus.size());
  // test FTF tracking only if there is at least a good offline tau
  if(good_offline_taus.size()==0) return StatusCode::SUCCESS;

  ATH_MSG_DEBUG("Passed ptonly with number of good offline taus: "<<good_offline_taus.size());

  std::vector<TLorentzVector> JetSeed_L1, JetSeed_ptonly, JetSeed_track, JetSeed_tracktwo, JetSeed_perf_track, JetSeed_perf_tracktwo;

  Trig::FeatureContainer f_pt = ( getTDT()->features("HLT_tau25_perf_ptonly",m_HLTTriggerCondition) );
  Trig::FeatureContainer::combination_const_iterator comb_pt(f_pt.getCombinations().begin()), combEnd_pt(f_pt.getCombinations().end());
  for(comb_pt=f_pt.getCombinations().begin();comb_pt!=combEnd_pt;++comb_pt){
  	const std::vector<Trig::Feature<xAOD::TauJetContainer> >  feat_tau_pt = comb_pt->get<xAOD::TauJetContainer>("TrigTauRecMerged",m_HLTTriggerCondition);
        std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator cont_tau_pt = feat_tau_pt.begin(), cont_tau_pt_e = feat_tau_pt.end();
        for(; cont_tau_pt != cont_tau_pt_e; ++cont_tau_pt){
        	if(cont_tau_pt->cptr()){
                	xAOD::TauJetContainer::const_iterator tau_pt = cont_tau_pt->cptr()->begin(),tau_pt_e = cont_tau_pt->cptr()->end();
                        for(; tau_pt != tau_pt_e; ++tau_pt) {
                        	TLorentzVector tlv_tau_pt = (*tau_pt)->p4();
                                for(unsigned int offline=0;offline<good_offline_taus.size();offline++){
                                	if(tlv_tau_pt.DeltaR(good_offline_taus.at(offline))<0.3){
						JetSeed_ptonly.push_back( tlv_tau_pt ); 
						TLorentzVector jet;
						jet.SetPtEtaPhiM((*tau_pt)->ptJetSeed(),(*tau_pt)->etaJetSeed(), (*tau_pt)->phiJetSeed(), (*tau_pt)->mJetSeed());
						JetSeed_L1.push_back(jet);
						break; 
					}
                                }
                        }
                }
        }
  }

  // in 50ns collisions idperf are seeded by L1_TAU12 instead of TAU12IM, so need to make sure we start from TAU12IM
  //std::vector<float> L1_eta, L1_phi;
  //Trig::FeatureContainer f_L1 = ( getTDT()->features("L1_TAU12IM",m_L1TriggerCondition) ); 
  //Trig::FeatureContainer::combination_const_iterator comb_L1(f_L1.getCombinations().begin()), combEnd_L1(f_L1.getCombinations().end());
  //for(;comb_L1!=combEnd_L1;++comb_L1){
  //	const std::vector< Trig::Feature<TrigRoiDescriptor> > vec_roi = comb_L1->get<TrigRoiDescriptor>("initialRoI",m_L1TriggerCondition);
  //	std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roi = vec_roi.begin(), roi_e = vec_roi.end();
  //	for(roi = vec_roi.begin(); roi != roi_e; ++roi) if(roi->cptr()) {L1_eta.push_back(roi->cptr()->eta());L1_phi.push_back(roi->cptr()->phi());}
  //}

  // check that the event fired or not the idperf chain
  if(getTDT()->isPassed("HLT_tau25_idperf_track",m_HLTTriggerCondition) ){ 
  	Trig::FeatureContainer f_track = ( getTDT()->features("HLT_tau25_idperf_track",m_HLTTriggerCondition) );
	Trig::FeatureContainer::combination_const_iterator comb_track(f_track.getCombinations().begin()), combEnd_track(f_track.getCombinations().end());
	for(comb_track=f_track.getCombinations().begin();comb_track!=combEnd_track;++comb_track){
		const std::vector<Trig::Feature<xAOD::TauJetContainer> >  feat_tau_track = comb_track->get<xAOD::TauJetContainer>("TrigTauRecPreselection",m_HLTTriggerCondition);
		std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator cont_tau_track = feat_tau_track.begin(), cont_tau_track_e = feat_tau_track.end();
		for(; cont_tau_track != cont_tau_track_e; ++cont_tau_track){
			if(cont_tau_track->cptr()){
				xAOD::TauJetContainer::const_iterator tau_track = cont_tau_track->cptr()->begin(),tau_track_e = cont_tau_track->cptr()->end();
				for(; tau_track != tau_track_e; ++tau_track) {
					TLorentzVector tlv_tau_track = (*tau_track)->p4();
//					for(unsigned int l1=0;l1<L1_eta.size();l1++) if(deltaR(L1_eta.at(l1),tlv_tau_track.Eta(),L1_phi.at(l1),tlv_tau_track.Phi())<0.3){
						for(unsigned int offline=0;offline<good_offline_taus.size();offline++){
							if(tlv_tau_track.DeltaR(good_offline_taus.at(offline))<0.3){JetSeed_track.push_back( tlv_tau_track ); break; }
						}
//					}
				}
			}
		}
	}
  }
  if(getTDT()->isPassed("HLT_tau25_idperf_tracktwo",m_HLTTriggerCondition) ){
        Trig::FeatureContainer f_tracktwo = ( getTDT()->features("HLT_tau25_idperf_tracktwo",m_HLTTriggerCondition) );
        Trig::FeatureContainer::combination_const_iterator comb_tracktwo(f_tracktwo.getCombinations().begin()), combEnd_tracktwo(f_tracktwo.getCombinations().end());
        for(comb_tracktwo=f_tracktwo.getCombinations().begin();comb_tracktwo!=combEnd_tracktwo;++comb_tracktwo){
                const std::vector<Trig::Feature<xAOD::TauJetContainer> >  feat_tau_tracktwo = comb_tracktwo->get<xAOD::TauJetContainer>("TrigTauRecPreselection",m_HLTTriggerCondition);
                std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator cont_tau_tracktwo = feat_tau_tracktwo.begin(), cont_tau_tracktwo_e = feat_tau_tracktwo.end();
                for(; cont_tau_tracktwo != cont_tau_tracktwo_e; ++cont_tau_tracktwo){
                        if(cont_tau_tracktwo->cptr()){
                                xAOD::TauJetContainer::const_iterator tau_tracktwo = cont_tau_tracktwo->cptr()->begin(),tau_tracktwo_e = cont_tau_tracktwo->cptr()->end();
                                for(; tau_tracktwo != tau_tracktwo_e; ++tau_tracktwo) {
                                        TLorentzVector tlv_tau_tracktwo = (*tau_tracktwo)->p4();
//                                        for(unsigned int l1=0;l1<L1_eta.size();l1++) if(deltaR(L1_eta.at(l1),tlv_tau_tracktwo.Eta(),L1_phi.at(l1),tlv_tau_tracktwo.Phi())<0.3){
                                                for(unsigned int offline=0;offline<good_offline_taus.size();offline++){
                                                        if(tlv_tau_tracktwo.DeltaR(good_offline_taus.at(offline))<0.3){JetSeed_tracktwo.push_back( tlv_tau_tracktwo ); break; }
                                                }
//                                        }
                                }
                        }
                }
        }
  }
  
  // check that the perf_tracktwo and the perf_track chains fired as well
  if(getTDT()->isPassed("HLT_tau25_perf_track",m_HLTTriggerCondition)){
	Trig::FeatureContainer f_track = ( getTDT()->features("HLT_tau25_perf_track",m_HLTTriggerCondition) );
	Trig::FeatureContainer::combination_const_iterator comb_track(f_track.getCombinations().begin()), combEnd_track(f_track.getCombinations().end());
	for(comb_track=f_track.getCombinations().begin();comb_track!=combEnd_track;++comb_track){
                const std::vector<Trig::Feature<xAOD::TauJetContainer> >  feat_tau_track = comb_track->get<xAOD::TauJetContainer>("TrigTauRecPreselection",m_HLTTriggerCondition);
                std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator cont_tau_track = feat_tau_track.begin(), cont_tau_track_e = feat_tau_track.end();
                for(; cont_tau_track != cont_tau_track_e; ++cont_tau_track){
                        if(cont_tau_track->cptr()){
                                xAOD::TauJetContainer::const_iterator tau_track = cont_tau_track->cptr()->begin(),tau_track_e = cont_tau_track->cptr()->end();
                                for(; tau_track != tau_track_e; ++tau_track) {
                                        TLorentzVector tlv_tau_track = (*tau_track)->p4();
                                        for(unsigned int offline=0;offline<good_offline_taus.size();offline++){
                                                if(tlv_tau_track.DeltaR(good_offline_taus.at(offline))<0.3){JetSeed_perf_track.push_back( tlv_tau_track ); break; }
                                        }
                                }
                        }
                }
        }			
  }
  if(getTDT()->isPassed("HLT_tau25_perf_tracktwo",m_HLTTriggerCondition)){
  	Trig::FeatureContainer f_tracktwo = ( getTDT()->features("HLT_tau25_perf_tracktwo",m_HLTTriggerCondition) );
	Trig::FeatureContainer::combination_const_iterator comb_tracktwo(f_tracktwo.getCombinations().begin()), combEnd_tracktwo(f_tracktwo.getCombinations().end());
	for(comb_tracktwo=f_tracktwo.getCombinations().begin();comb_tracktwo!=combEnd_tracktwo;++comb_tracktwo){
                const std::vector<Trig::Feature<xAOD::TauJetContainer> >  feat_tau_tracktwo = comb_tracktwo->get<xAOD::TauJetContainer>("TrigTauRecPreselection",m_HLTTriggerCondition);
                std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator cont_tau_tracktwo = feat_tau_tracktwo.begin(), cont_tau_tracktwo_e = feat_tau_tracktwo.end();
                for(; cont_tau_tracktwo != cont_tau_tracktwo_e; ++cont_tau_tracktwo){
                        if(cont_tau_tracktwo->cptr()){
                                xAOD::TauJetContainer::const_iterator tau_tracktwo = cont_tau_tracktwo->cptr()->begin(),tau_tracktwo_e = cont_tau_tracktwo->cptr()->end();
                                for(; tau_tracktwo != tau_tracktwo_e; ++tau_tracktwo) {
                                        TLorentzVector tlv_tau_tracktwo = (*tau_tracktwo)->p4();
                                        for(unsigned int offline=0;offline<good_offline_taus.size();offline++){
                                                if(tlv_tau_tracktwo.DeltaR(good_offline_taus.at(offline))<0.3){JetSeed_perf_tracktwo.push_back( tlv_tau_tracktwo ); break; }
                                        }
                                }
                        }
                }
        }
  }

  // print out JetSeeds 
  for(unsigned int t=0;t<JetSeed_L1.size();t++) ATH_MSG_DEBUG("ptonly: "<<JetSeed_L1.at(t).Pt()<<" "<<JetSeed_L1.at(t).Eta()<<" "<<JetSeed_L1.at(t).Phi()<<" "<<JetSeed_L1.at(t).E());

  // in case idperf_track and idperf_tracktwo have different decision -> ??
  // copy&paste code for the HLTTrackPreselection

  std::vector<bool> idperf_track, idperf_tracktwo, perf_track, perf_tracktwo;
  for(unsigned int t=0; t<JetSeed_ptonly.size();t++){
	idperf_track.push_back(false);
	idperf_tracktwo.push_back(false);
	perf_track.push_back(false);
	perf_tracktwo.push_back(false);
	for(unsigned int p=0;p<JetSeed_track.size();p++) if( JetSeed_ptonly.at(t).DeltaR(JetSeed_track.at(p)) < 0.1) idperf_track.back()=true;
	for(unsigned int p=0;p<JetSeed_tracktwo.size();p++) if( JetSeed_ptonly.at(t).DeltaR(JetSeed_tracktwo.at(p)) < 0.1) idperf_tracktwo.back()=true;
        for(unsigned int p=0;p<JetSeed_perf_track.size();p++) if( JetSeed_ptonly.at(t).DeltaR(JetSeed_perf_track.at(p)) < 0.1) perf_track.back()=true;
        for(unsigned int p=0;p<JetSeed_perf_tracktwo.size();p++) if( JetSeed_ptonly.at(t).DeltaR(JetSeed_perf_tracktwo.at(p)) < 0.1) perf_tracktwo.back()=true;

	if(idperf_track.back()!=idperf_tracktwo.back()) { 
//		ATH_MSG_WARNING("decisions for idperf_track and idperf_tracktwo are different!");
// 		ATH_MSG_WARNING("idperf_track  "<<getTDT()->isPassed("HLT_tau25_idperf_track",m_HLTTriggerCondition)<<", idperf_tracktwo "<<getTDT()->isPassed("HLT_tau25_idperf_tracktwo",m_HLTTriggerCondition));
//		ATH_MSG_WARNING("JetSeed_track size "<<JetSeed_track.size()<<", JetSeed_tracktwo size "<<JetSeed_tracktwo.size());
//		ATH_MSG_WARNING("ptonly eta "<<JetSeed_L1.at(t).Eta()<<", phi "<<JetSeed_L1.at(t).Phi() );
//		for(unsigned int p=0;p<JetSeed_track.size();p++) ATH_MSG_WARNING("track eta "<<JetSeed_track.at(p).Eta()<<", phi "<<JetSeed_track.at(p).Phi() );
//		for(unsigned int p=0;p<JetSeed_tracktwo.size();p++) ATH_MSG_WARNING("tracktwo eta "<<JetSeed_tracktwo.at(p).Eta()<<", phi "<<JetSeed_tracktwo.at(p).Phi() );
	}
	if(perf_track.back()!=perf_tracktwo.back()) { 
//                ATH_MSG_WARNING("decisions for perf_track and perf_tracktwo are different!");
//                ATH_MSG_WARNING("perf_track  "<<getTDT()->isPassed("HLT_tau25_perf_track",m_HLTTriggerCondition)<<", perf_tracktwo "<<getTDT()->isPassed("HLT_tau25_perf_tracktwo",m_HLTTriggerCondition));
//                ATH_MSG_WARNING("JetSeed_perf_track size "<<JetSeed_perf_track.size()<<", JetSeed_perf_tracktwo size "<<JetSeed_perf_tracktwo.size());
//		ATH_MSG_WARNING("ptonly eta "<<JetSeed_L1.at(t).Eta()<<", phi "<<JetSeed_L1.at(t).Phi() );
//                for(unsigned int p=0;p<JetSeed_perf_track.size();p++) ATH_MSG_WARNING("perf track eta "<<JetSeed_perf_track.at(p).Eta()<<", phi "<<JetSeed_perf_track.at(p).Phi() );
//                for(unsigned int p=0;p<JetSeed_perf_tracktwo.size();p++) ATH_MSG_WARNING("perf tracktwo eta "<<JetSeed_perf_tracktwo.at(p).Eta()<<", phi "<<JetSeed_perf_tracktwo.at(p).Phi() );  
      }
  }


  const xAOD::TrackParticleContainer* FTF=0;
  const xAOD::TrackParticleContainer* FTFCore=0;
  const xAOD::TrackParticleContainer* FTFIso=0;

  if ( m_storeGate->retrieve( FTF , "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTF").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTF  container. Exiting.");
    return StatusCode::SUCCESS;
  }
  if ( m_storeGate->retrieve( FTFCore , "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauCore_FTF").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauCore_FTF  container. Exiting.");
    return StatusCode::SUCCESS;
  }
  if ( m_storeGate->retrieve( FTFIso , "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTF").isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTF  container. Exiting.");
    return StatusCode::SUCCESS;
  }  

  xAOD::TrackParticleContainer::const_iterator itrk, itrk_new;  

  // get tracks in RoI
  for(unsigned int t=0; t<JetSeed_L1.size();t++){

        if(idperf_track.at(t)!=idperf_tracktwo.at(t)){
                ATH_MSG_WARNING(getRunNr()<<" "<<getEventNr()<<": decisions for idperf_track and idperf_tracktwo are different! track "<<idperf_track.at(t)<<" tracktwo "<<idperf_tracktwo.at(t));
		ATH_MSG_WARNING("JetSeed (pt,eta,phi) "<<JetSeed_L1.at(t).Pt()<<", "<<JetSeed_L1.at(t).Eta()<<", "<<JetSeed_L1.at(t).Phi());
		for(itrk=FTF->begin();itrk!=FTF->end();itrk++){
                	//const Trk::Track* trk = (*itrk)->track();
                	//const Trk::Perigee* tp = trk->perigeeParameters();
                	//float trk_eta = tp->eta();
                	//float trk_phi = tp->parameters()[Trk::phi];
                	//float trk_z0 = tp->parameters()[Trk::z0];
                        const Trk::Perigee tp = (*itrk)->perigeeParameters();
                        float trk_eta = tp.eta();
                        float trk_phi = tp.parameters()[Trk::phi];
                        float trk_z0 = tp.parameters()[Trk::z0];
			float dR = deltaR(JetSeed_L1.at(t).Eta(),trk_eta,JetSeed_L1.at(t).Phi(),trk_phi);
                        ATH_MSG_WARNING("FTF trk (pt,eta,phi,z0), dR: "<<(*itrk)->pt()<<", "<<trk_eta<<", "<<trk_phi<<", "<<trk_z0<<", "<<dR);
                }
		for(itrk=FTFCore->begin();itrk!=FTFCore->end();itrk++){
                        const Trk::Perigee tp = (*itrk)->perigeeParameters();
                        float trk_eta = tp.eta();
                        float trk_phi = tp.parameters()[Trk::phi];
                        float trk_z0 = tp.parameters()[Trk::z0];
                        float dR = deltaR(JetSeed_L1.at(t).Eta(),trk_eta,JetSeed_L1.at(t).Phi(),trk_phi);
                        ATH_MSG_WARNING("FTF Core trk (pt,eta,phi,z0), dR: "<<(*itrk)->pt()<<", "<<trk_eta<<", "<<trk_phi<<", "<<trk_z0<<", "<<dR);
                }
		for(itrk=FTFIso->begin();itrk!=FTFIso->end();itrk++){
                        const Trk::Perigee tp = (*itrk)->perigeeParameters();
                        float trk_eta = tp.eta();
                        float trk_phi = tp.parameters()[Trk::phi];
                        float trk_z0 = tp.parameters()[Trk::z0];
                        float dR = deltaR(JetSeed_L1.at(t).Eta(),trk_eta,JetSeed_L1.at(t).Phi(),trk_phi);
                        ATH_MSG_WARNING("FTF Iso trk (pt,eta,phi,z0), dR: "<<(*itrk)->pt()<<", "<<trk_eta<<", "<<trk_phi<<", "<<trk_z0<<", "<<dR);
                }
		continue;
        }

        if(!idperf_track.at(t) || !idperf_tracktwo.at(t)){
                ATH_MSG_DEBUG("idperf_track or idperf_tracktwo failed!");
                continue;
        }

	float RoI_eta(JetSeed_L1.at(t).Eta()), RoI_phi(JetSeed_L1.at(t).Phi());
	float trk_pt_min = 0.;

	std::vector<float> FTF_pt, FTF_eta, FTF_phi, FTF_z0, FTF_nCore, FTF_nWide;
	std::vector<float> FTFCore_pt, FTFCore_eta, FTFCore_phi, FTFCore_z0, FTFCore_nCore, FTFCore_nWide;
	std::vector<float> FTFIso_pt, FTFIso_eta, FTFIso_phi, FTFIso_z0, FTFIso_nCore, FTFIso_nWide;

	// get 1step tracks and try trk selection
	if(FTF->begin()==FTF->end()) ATH_MSG_WARNING("1step track container empty");
	float ltrk_pt(0.), ltrk_eta(0.), ltrk_phi(0.), ltrk_z0(0.);
	bool applydz0(true); 
	for(itrk=FTF->begin();itrk!=FTF->end();itrk++){
		const Trk::Perigee tp = (*itrk)->perigeeParameters();
                float trk_eta = tp.eta();
		float trk_phi = tp.parameters()[Trk::phi];		
		float trk_z0 = tp.parameters()[Trk::z0];
                float dPhi = deltaPhi(RoI_phi,trk_phi);
                float dEta = fabs(RoI_eta - trk_eta);
		float dR = deltaR(RoI_eta,trk_eta,RoI_phi,trk_phi);
		float trk_pt((*itrk)->pt());
		if(trk_pt<trk_pt_min) continue;
		if(dPhi>0.4 || dEta>0.4 || fabs(trk_z0)>225) continue;
		// avoid duplicated tracks 
		bool duplicated(false);
		for(itrk_new=FTF->begin();itrk_new!=itrk;itrk_new++){
			const Trk::Perigee tp = (*itrk_new)->perigeeParameters();
                	float trk_new_eta = tp.eta();
                	float trk_new_phi = tp.parameters()[Trk::phi]; 
			if(deltaR(trk_eta,trk_new_eta,trk_phi,trk_new_phi)<0.0001) duplicated = true;
		}
		if(duplicated) continue;
		if(dR<=0.2 && trk_pt>ltrk_pt) { ltrk_pt = trk_pt; ltrk_eta = trk_eta; ltrk_phi = trk_phi; ltrk_z0 = trk_z0;}
	}
	if(ltrk_pt==0.){ 
		ATH_MSG_WARNING(JetSeed_L1.at(t).Pt()<<", no leading trk found after 1step trk");
		ltrk_eta = RoI_eta;
		ltrk_phi = RoI_phi;		
		applydz0 = false;
	}
	for(itrk=FTF->begin();itrk!=FTF->end();itrk++){
		const Trk::Perigee tp = (*itrk)->perigeeParameters();
                float trk_eta = tp.eta();
                float trk_phi = tp.parameters()[Trk::phi]; 
                float trk_z0 = tp.parameters()[Trk::z0];
		float dPhi = deltaPhi(RoI_phi,trk_phi);
                float dEta = fabs(RoI_eta - trk_eta);
                float trk_pt((*itrk)->pt());
                if(trk_pt<trk_pt_min) continue;
                if(dPhi>0.4 || dEta>0.4 || fabs(trk_z0)>225) continue;
                float dR = deltaR(ltrk_eta,trk_eta,ltrk_phi,trk_phi);
		float dZ0 = fabs(ltrk_z0 - trk_z0);
		if(applydz0 && dZ0>2.) continue;
		// avoid duplicated tracks 
		bool duplicated(false);
		for(itrk_new=FTF->begin();itrk_new!=itrk;itrk_new++){
		        const Trk::Perigee tp_new = (*itrk_new)->perigeeParameters();
                        float trk_new_eta = tp_new.eta();
                        float trk_new_phi = tp_new.parameters()[Trk::phi];
                        if(deltaR(trk_eta,trk_new_eta,trk_phi,trk_new_phi)<0.0001) duplicated = true;
		}
		if(duplicated) continue;
		FTF_pt.push_back(trk_pt);
		FTF_eta.push_back(trk_eta);
		FTF_phi.push_back(trk_phi);
		FTF_z0.push_back(trk_z0);
		FTF_nCore.push_back(false); if(dR<=0.2) FTF_nCore.back() = true; 
		FTF_nWide.push_back(false); if(dR>0.2 && dR<=0.4) FTF_nWide.back() = true;		
	}

	// check emulated decision with TDT:
	unsigned int nCore_track(0), nWide_track(0);
	for(unsigned int x=0;x<FTF_nCore.size();x++){if(FTF_nCore.at(x)) nCore_track++; if(FTF_nWide.at(x)) nWide_track++;}
	bool passed_perf_track(true);
	if( nCore_track>3 || nWide_track>1 ) passed_perf_track = false;
	if(passed_perf_track==perf_track.at(t)) ATH_MSG_DEBUG("1step: emulated and TDT are the SAME");
	if(passed_perf_track!=perf_track.at(t)){ 
		ATH_MSG_WARNING(getRunNr()<<" "<<getEventNr()<<": seed pt "<<JetSeed_L1.at(t).Pt()<<", 1step: emulated and TDT are the DIFFERENT, emulated "<<passed_perf_track<<" TDT "<<perf_track.at(t)); 
		ATH_MSG_WARNING("RoI (eta,phi): "<<RoI_eta<<", "<<RoI_phi);
		ATH_MSG_WARNING("ltrk (pt,eta,phi,z0): "<<ltrk_pt<<", "<<ltrk_eta<<", "<<ltrk_phi<<", "<<ltrk_z0<<", applyz0 "<<applydz0);
		for(unsigned int x=0;x<FTF_pt.size();x++) ATH_MSG_WARNING("trk (pt,eta,phi,z0): "<<FTF_pt.at(x)<<", "<<FTF_eta.at(x)<<", "<<FTF_phi.at(x)<<", "<<FTF_z0.at(x)<<", core "<<FTF_nCore.at(x)<<", wide "<<FTF_nWide.at(x));
		for(itrk=FTF->begin();itrk!=FTF->end();itrk++){
			const Trk::Perigee tp = (*itrk)->perigeeParameters();
			float trk_eta = tp.eta();
                	float trk_phi = tp.parameters()[Trk::phi];
                	float trk_z0 = tp.parameters()[Trk::z0];
			float dR = deltaR(ltrk_eta,trk_eta,ltrk_phi,trk_phi);
			float dz0 = fabs(ltrk_z0 - trk_z0);
			ATH_MSG_WARNING("FTF trk (pt,eta,phi,z0): "<<(*itrk)->pt()<<", "<<trk_eta<<", "<<trk_phi<<", "<<trk_z0<<", dR "<<dR<<", dz0 "<<dz0);
		}
	}

	if(FTFCore->begin()==FTFCore->end()) ATH_MSG_WARNING("2step core track container empty");
	// get 2step tracks and try the trk selection
	float ltrk_tracktwo_pt(0.), ltrk_tracktwo_eta(0.), ltrk_tracktwo_phi(0.), ltrk_tracktwo_z0(0.); applydz0=true;	
	bool updateRoI(true);	
	for(itrk=FTFCore->begin();itrk!=FTFCore->end();itrk++){
		const Trk::Perigee tp = (*itrk)->perigeeParameters();
                float trk_eta = tp.eta();
                float trk_phi = tp.parameters()[Trk::phi];
                float trk_z0 = tp.parameters()[Trk::z0];
		float dPhi = deltaPhi(RoI_phi,trk_phi);
		float dEta = fabs(RoI_eta - trk_eta); 
                float trk_pt((*itrk)->pt());
                if(trk_pt<trk_pt_min) continue;
                bool duplicated(false);
                for(itrk_new=FTFCore->begin();itrk_new!=itrk;itrk_new++){
                        const Trk::Perigee tp_new = (*itrk_new)->perigeeParameters();
                        float trk_new_eta = tp_new.eta();
                        float trk_new_phi = tp_new.parameters()[Trk::phi];
                        if(deltaR(trk_eta,trk_new_eta,trk_phi,trk_new_phi)<0.0001) duplicated = true;
                }
                if(duplicated) continue;
                if(dPhi<=0.1 && dEta<=0.1 && fabs(trk_z0)<=225 && trk_pt>ltrk_tracktwo_pt) { ltrk_tracktwo_pt = trk_pt; ltrk_tracktwo_eta = trk_eta; ltrk_tracktwo_phi = trk_phi; ltrk_tracktwo_z0 = trk_z0;}
        }
        if(ltrk_tracktwo_pt==0.){ 
                ATH_MSG_WARNING(JetSeed_L1.at(t).Pt()<<", no leading trk found after first step of 2step trk");
		updateRoI = false;
        }
	else {
		FTFCore_pt.push_back(ltrk_tracktwo_pt);
                FTFCore_eta.push_back(ltrk_tracktwo_eta);
                FTFCore_phi.push_back(ltrk_tracktwo_phi);
                FTFCore_z0.push_back(ltrk_tracktwo_z0);
                FTFCore_nCore.push_back(true);	
	}	
	if(FTFIso->begin()==FTFIso->end()) ATH_MSG_WARNING(JetSeed_L1.at(t).Pt()<<", 2step iso track container empty");	
	ltrk_tracktwo_pt=0.; ltrk_tracktwo_eta=0.; ltrk_tracktwo_phi=0.; ltrk_tracktwo_z0=0.; applydz0=true;
	for(itrk=FTFIso->begin();itrk!=FTFIso->end();itrk++){
                const Trk::Perigee tp = (*itrk)->perigeeParameters();
                float trk_eta = tp.eta();
                float trk_phi = tp.parameters()[Trk::phi];
                float trk_z0 = tp.parameters()[Trk::z0];
		float dPhi = deltaPhi(RoI_phi,trk_phi);
                float dEta = fabs(RoI_eta - trk_eta);
                float dR = deltaR(RoI_eta,trk_eta,RoI_phi,trk_phi);
                float trk_pt((*itrk)->pt());
                if(trk_pt<trk_pt_min) continue;
		if(dPhi>0.4 || dEta>0.4 || fabs(trk_z0)>225) continue;
		if(updateRoI){
			float dZ0 = fabs(trk_z0 - FTFCore_z0.at(0));
			if(dZ0>10.) continue;
		}
		bool duplicated(false);
                for(itrk_new=FTFIso->begin();itrk_new!=itrk;itrk_new++){
                        const Trk::Perigee tp_new = (*itrk_new)->perigeeParameters();
                        float trk_new_eta = tp_new.eta();
                        float trk_new_phi = tp_new.parameters()[Trk::phi];
                        if(deltaR(trk_eta,trk_new_eta,trk_phi,trk_new_phi)<0.0001) duplicated = true;
                }
                if(duplicated) continue;
                if(dR<=0.2 && trk_pt>ltrk_tracktwo_pt) { ltrk_tracktwo_pt = trk_pt; ltrk_tracktwo_eta = trk_eta; ltrk_tracktwo_phi = trk_phi; ltrk_tracktwo_z0 = trk_z0;}
	}
        if(ltrk_tracktwo_pt==0.){ 
                ATH_MSG_WARNING(JetSeed_L1.at(t).Pt()<<", no leading trk found after second step of 2step trk");
                ltrk_tracktwo_eta = RoI_eta;
                ltrk_tracktwo_phi = RoI_phi;
                applydz0 = false;
        }
        for(itrk=FTFIso->begin();itrk!=FTFIso->end();itrk++){
                const Trk::Perigee tp = (*itrk)->perigeeParameters();
                float trk_eta = tp.eta();
                float trk_phi = tp.parameters()[Trk::phi];
                float trk_z0 = tp.parameters()[Trk::z0];   
		float dPhi = deltaPhi(RoI_phi,trk_phi);
                float dEta = fabs(RoI_eta - trk_eta);
                float trk_pt((*itrk)->pt());
                if(trk_pt<trk_pt_min) continue;
		if(dPhi>0.4 || dEta>0.4 || fabs(trk_z0)>225) continue;
                if(updateRoI){
                        float dZ0 = fabs(trk_z0 - FTFCore_z0.at(0));
                        if(dZ0>10.) continue;
                }
		float dR = deltaR(ltrk_tracktwo_eta,trk_eta,ltrk_tracktwo_phi,trk_phi);
		float dZ0 = fabs(ltrk_tracktwo_z0 - trk_z0);
                if(applydz0 && dZ0>2.) continue;	
		bool duplicated(false);
                for(itrk_new=FTFIso->begin();itrk_new!=itrk;itrk_new++){
                        const Trk::Perigee tp_new = (*itrk_new)->perigeeParameters();
                        float trk_new_eta = tp_new.eta();
                        float trk_new_phi = tp_new.parameters()[Trk::phi];
                        if(deltaR(trk_eta,trk_new_eta,trk_phi,trk_new_phi)<0.0001) duplicated = true;
                }
                if(duplicated) continue;
                FTFIso_pt.push_back((*itrk)->pt());
                FTFIso_eta.push_back(trk_eta);
                FTFIso_phi.push_back(trk_phi);
                FTFIso_z0.push_back(trk_z0);
                FTFIso_nCore.push_back(false); if(dR<=0.2) FTFIso_nCore.back() = true;
                FTFIso_nWide.push_back(false); if(dR>0.2 && dR<=0.4) FTFIso_nWide.back() = true;
        }

	// check emulated decision with TDT:
	unsigned int nCore_tracktwo(0), nWide_tracktwo(0);
	for(unsigned int x=0;x<FTFIso_nCore.size();x++){if(FTFIso_nCore.at(x)) nCore_tracktwo++; if(FTFIso_nWide.at(x)) nWide_tracktwo++;}
	bool passed_perf_tracktwo(true);
	if( nCore_tracktwo>3 || nWide_tracktwo>1 || FTFCore_pt.size()==0 ) passed_perf_tracktwo = false;
	if(passed_perf_tracktwo==perf_tracktwo.at(t)) ATH_MSG_DEBUG("2step: emulated and TDT are the SAME");
	if(passed_perf_tracktwo!=perf_tracktwo.at(t)){
		ATH_MSG_WARNING(getRunNr()<<" "<<getEventNr()<<": seed pt "<<JetSeed_L1.at(t).Pt()<<", 2step: emulated and TDT are the DIFFERENT, emulated "<<passed_perf_tracktwo<<" TDT "<<perf_tracktwo.at(t));
		ATH_MSG_WARNING("RoI (eta,phi): "<<RoI_eta<<", "<<RoI_phi);
                ATH_MSG_WARNING("ltrk (pt,eta,phi,z0): "<<ltrk_tracktwo_pt<<", "<<ltrk_tracktwo_eta<<", "<<ltrk_tracktwo_phi<<", "<<ltrk_tracktwo_z0<<", applyz0 "<<applydz0);
                for(unsigned int x=0;x<FTFCore_pt.size();x++) ATH_MSG_WARNING("Core trk (pt,eta,phi,z0): "<<FTFCore_pt.at(x)<<", "<<FTFCore_eta.at(x)<<", "<<FTFCore_phi.at(x)<<", "<<FTFCore_z0.at(x)<<", core "<<FTFCore_nCore.at(x));
                for(itrk=FTFCore->begin();itrk!=FTFCore->end();itrk++){
                        const Trk::Perigee tp = (*itrk)->perigeeParameters();
                        float trk_eta = tp.eta();
                        float trk_phi = tp.parameters()[Trk::phi];
                        float trk_z0 = tp.parameters()[Trk::z0];
                        float dR = deltaR(ltrk_tracktwo_eta,trk_eta,ltrk_tracktwo_phi,trk_phi);
                        float dz0 = fabs(ltrk_tracktwo_z0 - trk_z0);
                        ATH_MSG_WARNING("FTFCore trk (pt,eta,phi,z0): "<<(*itrk)->pt()<<", "<<trk_eta<<", "<<trk_phi<<", "<<trk_z0<<", dR "<<dR<<", dz0 "<<dz0);
                }
                for(unsigned int x=0;x<FTFIso_pt.size();x++) ATH_MSG_WARNING("Iso trk (pt,eta,phi,z0): "<<FTFIso_pt.at(x)<<", "<<FTFIso_eta.at(x)<<", "<<FTFIso_phi.at(x)<<", "<<FTFIso_z0.at(x)<<", core "<<FTFIso_nCore.at(x)<<", wide "<<FTFIso_nWide.at(x));
                for(itrk=FTFIso->begin();itrk!=FTFIso->end();itrk++){
                        const Trk::Perigee tp = (*itrk)->perigeeParameters();
                        float trk_eta = tp.eta();
                        float trk_phi = tp.parameters()[Trk::phi];
                        float trk_z0 = tp.parameters()[Trk::z0];
                        float dR = deltaR(ltrk_tracktwo_eta,trk_eta,ltrk_tracktwo_phi,trk_phi);
                        float dz0 = fabs(ltrk_tracktwo_z0 - trk_z0);
                        ATH_MSG_WARNING("FTFIso trk (pt,eta,phi,z0): "<<(*itrk)->pt()<<", "<<trk_eta<<", "<<trk_phi<<", "<<trk_z0<<", dR "<<dR<<", dz0 "<<dz0);
                }
	}


	// check events where the emulated perf chains have different decisions
	if(passed_perf_tracktwo!=passed_perf_track){
		ATH_MSG_WARNING(getRunNr()<<" "<<getEventNr()<<": seed pt "<<JetSeed_L1.at(t).Pt()<<", emulated perf track "<<passed_perf_track<<", emulated perf tracktwo "<<passed_perf_tracktwo);
		ATH_MSG_WARNING("RoI (eta,phi): "<<RoI_eta<<", "<<RoI_phi);
		ATH_MSG_WARNING("ltrk in track (pt,eta,phi,z0): "<<ltrk_pt<<", "<<ltrk_eta<<", "<<ltrk_phi<<", "<<ltrk_z0<<", applyz0 "<<applydz0);
                ATH_MSG_WARNING("ltrk in tracktwo (pt,eta,phi,z0): "<<ltrk_tracktwo_pt<<", "<<ltrk_tracktwo_eta<<", "<<ltrk_tracktwo_phi<<", "<<ltrk_tracktwo_z0<<", applyz0 "<<applydz0);
		for(unsigned int x=0;x<FTF_pt.size();x++){
			float dR = deltaR(ltrk_eta,FTF_eta.at(x),ltrk_phi,FTF_phi.at(x)); 
			ATH_MSG_WARNING("trk (pt,eta,phi,z0), dR: "<<FTF_pt.at(x)<<", "<<FTF_eta.at(x)<<", "<<FTF_phi.at(x)<<", "<<FTF_z0.at(x)<<", "<<dR<<", core "<<FTF_nCore.at(x)<<", wide "<<FTF_nWide.at(x));		
		}
		for(itrk=FTF->begin();itrk!=FTF->end();itrk++){
                        const Trk::Perigee tp = (*itrk)->perigeeParameters();
                        float trk_eta = tp.eta();
                        float trk_phi = tp.parameters()[Trk::phi];
                        float trk_z0 = tp.parameters()[Trk::z0];
                        float dR = deltaR(ltrk_tracktwo_eta,trk_eta,ltrk_tracktwo_phi,trk_phi);
                        float dz0 = fabs(ltrk_tracktwo_z0 - trk_z0);
                        ATH_MSG_WARNING("FTF trk (pt,eta,phi,z0): "<<(*itrk)->pt()<<", "<<trk_eta<<", "<<trk_phi<<", "<<trk_z0<<", dR "<<dR<<", dz0 "<<dz0);
                }	
		for(unsigned int x=0;x<FTFCore_pt.size();x++){
			float dR = deltaR(ltrk_tracktwo_eta,FTFCore_eta.at(x),ltrk_tracktwo_phi,FTFCore_phi.at(x)); 
			ATH_MSG_WARNING("Core trk (pt,eta,phi,z0), dR: "<<FTFCore_pt.at(x)<<", "<<FTFCore_eta.at(x)<<", "<<FTFCore_phi.at(x)<<", "<<FTFCore_z0.at(x)<<", "<<dR<<", core "<<FTFCore_nCore.at(x));
		}
		for(itrk=FTFCore->begin();itrk!=FTFCore->end();itrk++){
                        const Trk::Perigee tp = (*itrk)->perigeeParameters();
                        float trk_eta = tp.eta(); 
			float trk_phi = tp.parameters()[Trk::phi];
                        float trk_z0 = tp.parameters()[Trk::z0];
                        float dR = deltaR(ltrk_tracktwo_eta,trk_eta,ltrk_tracktwo_phi,trk_phi);
                        float dz0 = fabs(ltrk_tracktwo_z0 - trk_z0);
                        ATH_MSG_WARNING("FTFCore trk (pt,eta,phi,z0): "<<(*itrk)->pt()<<", "<<trk_eta<<", "<<trk_phi<<", "<<trk_z0<<", dR "<<dR<<", dz0 "<<dz0);
                }
		for(unsigned int x=0;x<FTFIso_pt.size();x++){ 
			float dR = deltaR(ltrk_tracktwo_eta,FTFIso_eta.at(x),ltrk_tracktwo_phi,FTFIso_phi.at(x));
			ATH_MSG_WARNING("Iso trk (pt,eta,phi,z0), dR: "<<FTFIso_pt.at(x)<<", "<<FTFIso_eta.at(x)<<", "<<FTFIso_phi.at(x)<<", "<<FTFIso_z0.at(x)<<", "<<dR<<", core "<<FTFIso_nCore.at(x)<<", wide "<<FTFIso_nWide.at(x));	
		}
                for(itrk=FTFIso->begin();itrk!=FTFIso->end();itrk++){
                        const Trk::Perigee tp = (*itrk)->perigeeParameters();
                        float trk_eta = tp.eta();
                        float trk_phi = tp.parameters()[Trk::phi];
                        float trk_z0 = tp.parameters()[Trk::z0];
                        float dR = deltaR(ltrk_tracktwo_eta,trk_eta,ltrk_tracktwo_phi,trk_phi);
                        float dz0 = fabs(ltrk_tracktwo_z0 - trk_z0);
                        ATH_MSG_WARNING("FTFIso trk (pt,eta,phi,z0): "<<(*itrk)->pt()<<", "<<trk_eta<<", "<<trk_phi<<", "<<trk_z0<<", dR "<<dR<<", dz0 "<<dz0);
                }
	}
  }
  
  return StatusCode::SUCCESS; 
}

*/

/*bool  HLTTauMonTool::emulate2StepTracking(float RoI_eta, float RoI_phi, bool do2step, bool doReject0trk, float track_pt_min=0.){

	bool pass(false);
	
  	const xAOD::TrackParticleContainer* FTF=0;
  	const xAOD::TrackParticleContainer* FTFCore=0;
  	const xAOD::TrackParticleContainer* FTFIso=0;

  	if(!do2step) if ( m_storeGate->retrieve( FTF , "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTF").isFailure() ){
    		ATH_MSG_WARNING("Failed to retrieve HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTF  container. Exiting.");
    		return StatusCode::SUCCESS;
  	}
  	if(do2step) if ( m_storeGate->retrieve( FTFCore , "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauCore_FTF").isFailure() ){
    		ATH_MSG_WARNING("Failed to retrieve HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauCore_FTF  container. Exiting.");
    		return StatusCode::SUCCESS;
  	}
  	if(do2step) if ( m_storeGate->retrieve( FTFIso , "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTF").isFailure() ){
    		ATH_MSG_WARNING("Failed to retrieve HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTF  container. Exiting.");
		return StatusCode::SUCCESS;
  	}

  	xAOD::TrackParticleContainer::const_iterator itrk, itrk_new;	

	std::vector<float> FTF_pt, FTF_eta, FTF_phi, FTF_z0, FTF_nCore, FTF_nWide;
	
	if(!do2step){
		ATH_MSG_DEBUG("Emulating 1step tracking");
		if(FTF->begin()==FTF->end()) ATH_MSG_DEBUG("1step track container empty");
        	float ltrk_pt(0.), ltrk_eta(0.), ltrk_phi(0.), ltrk_z0(0.);
        	bool applydz0(true);
        	for(itrk=FTF->begin();itrk!=FTF->end();itrk++){
                	const Trk::Perigee tp = (*itrk)->perigeeParameters();
                	float trk_eta = tp.eta();
                	float trk_phi = tp.parameters()[Trk::phi];
                	float trk_z0 = tp.parameters()[Trk::z0];
                	float dPhi = deltaPhi(RoI_phi,trk_phi);
                	float dEta = fabs(RoI_eta - trk_eta);
                	float dR = deltaR(RoI_eta,trk_eta,RoI_phi,trk_phi);
                	float trk_pt((*itrk)->pt());
                	if(trk_pt<track_pt_min) continue;
                	if(dPhi>0.4 || dEta>0.4 || fabs(trk_z0)>225) continue;
                	bool duplicated(false);
                	for(itrk_new=FTF->begin();itrk_new!=itrk;itrk_new++){
                        	const Trk::Perigee tp = (*itrk_new)->perigeeParameters();
                        	float trk_new_eta = tp.eta();
                        	float trk_new_phi = tp.parameters()[Trk::phi];
                        	if(deltaR(trk_eta,trk_new_eta,trk_phi,trk_new_phi)<0.0001) duplicated = true;
                	}
                	if(duplicated) continue;
                	if(dR<=0.2 && trk_pt>ltrk_pt) { ltrk_pt = trk_pt; ltrk_eta = trk_eta; ltrk_phi = trk_phi; ltrk_z0 = trk_z0;}
        	}
        	if(ltrk_pt==0.){
                	ATH_MSG_DEBUG("No leading trk found after 1step trk");
                	ltrk_eta = RoI_eta;
                	ltrk_phi = RoI_phi;
                	applydz0 = false;
        	}
        	for(itrk=FTF->begin();itrk!=FTF->end();itrk++){
                	const Trk::Perigee tp = (*itrk)->perigeeParameters();
                	float trk_eta = tp.eta();
                	float trk_phi = tp.parameters()[Trk::phi];
                	float trk_z0 = tp.parameters()[Trk::z0];
                	float dPhi = deltaPhi(RoI_phi,trk_phi);
                	float dEta = fabs(RoI_eta - trk_eta);
                	float trk_pt((*itrk)->pt());
                	if(trk_pt<track_pt_min) continue;
                	if(dPhi>0.4 || dEta>0.4 || fabs(trk_z0)>225) continue;
                	float dR = deltaR(ltrk_eta,trk_eta,ltrk_phi,trk_phi);
                	float dZ0 = fabs(ltrk_z0 - trk_z0);
                	if(applydz0 && dZ0>2.) continue;
                	bool duplicated(false);
                	for(itrk_new=FTF->begin();itrk_new!=itrk;itrk_new++){
                        	const Trk::Perigee tp_new = (*itrk_new)->perigeeParameters();
                        	float trk_new_eta = tp_new.eta();
                        	float trk_new_phi = tp_new.parameters()[Trk::phi];
                        	if(deltaR(trk_eta,trk_new_eta,trk_phi,trk_new_phi)<0.0001) duplicated = true;
                	}
                	if(duplicated) continue;
                	FTF_pt.push_back(trk_pt);
                	FTF_eta.push_back(trk_eta);
                	FTF_phi.push_back(trk_phi);
                	FTF_z0.push_back(trk_z0);
                	FTF_nCore.push_back(false); if(dR<=0.2) FTF_nCore.back() = true;
                	FTF_nWide.push_back(false); if(dR>0.2 && dR<=0.4) FTF_nWide.back() = true;
        	}
		ATH_MSG_DEBUG("Emulated 1step trks, nTracks "<<FTF_pt.size());
	}

	if(do2step){
		ATH_MSG_DEBUG("Emulating 1step tracking");
		if(FTFCore->begin()==FTFCore->end()) ATH_MSG_DEBUG("2step core track container empty");
        	float ltrk_pt(0.), ltrk_eta(0.), ltrk_phi(0.), ltrk_z0(0.); 
		bool applydz0(true);
        	bool updateRoI(true);
        	for(itrk=FTFCore->begin();itrk!=FTFCore->end();itrk++){
                	const Trk::Perigee tp = (*itrk)->perigeeParameters();
                	float trk_eta = tp.eta();
                	float trk_phi = tp.parameters()[Trk::phi];
                	float trk_z0 = tp.parameters()[Trk::z0];
                	float dPhi = deltaPhi(RoI_phi,trk_phi);
                	float dEta = fabs(RoI_eta - trk_eta);
                	float trk_pt((*itrk)->pt());
                	if(trk_pt<track_pt_min) continue;
               	 	bool duplicated(false);
                	for(itrk_new=FTFCore->begin();itrk_new!=itrk;itrk_new++){
                        	const Trk::Perigee tp_new = (*itrk_new)->perigeeParameters();
                        	float trk_new_eta = tp_new.eta();
                        	float trk_new_phi = tp_new.parameters()[Trk::phi];
                        	if(deltaR(trk_eta,trk_new_eta,trk_phi,trk_new_phi)<0.0001) duplicated = true;
                	}
                	if(duplicated) continue;
                	if(dPhi<=0.1 && dEta<=0.1 && fabs(trk_z0)<=225 && trk_pt>ltrk_pt) { ltrk_pt = trk_pt; ltrk_eta = trk_eta; ltrk_phi = trk_phi; ltrk_z0 = trk_z0;}
        	}
		float RoI_z0(ltrk_z0);
        	if(ltrk_pt==0.){
                	ATH_MSG_DEBUG("No leading trk found after first step of 2step trk");
                	updateRoI = false;
			if(doReject0trk) return false;
        	} 

	        if(FTFIso->begin()==FTFIso->end()) ATH_MSG_DEBUG("2step iso track container empty");
        	ltrk_pt=0.; ltrk_eta=0.; ltrk_phi=0.; ltrk_z0=0.; applydz0=true;
        	for(itrk=FTFIso->begin();itrk!=FTFIso->end();itrk++){
                	const Trk::Perigee tp = (*itrk)->perigeeParameters();
                	float trk_eta = tp.eta();
                	float trk_phi = tp.parameters()[Trk::phi];
                	float trk_z0 = tp.parameters()[Trk::z0];
                	float dPhi = deltaPhi(RoI_phi,trk_phi);
                	float dEta = fabs(RoI_eta - trk_eta);
                	float dR = deltaR(RoI_eta,trk_eta,RoI_phi,trk_phi);
                	float trk_pt((*itrk)->pt());
                	if(trk_pt<track_pt_min) continue;
                	if(dPhi>0.4 || dEta>0.4 || fabs(trk_z0)>225) continue;
                	if(updateRoI){
                        	float dZ0 = fabs(trk_z0 - RoI_z0);
                        	if(dZ0>10.) continue;
                	}
                	bool duplicated(false);
                	for(itrk_new=FTFIso->begin();itrk_new!=itrk;itrk_new++){
                        	const Trk::Perigee tp_new = (*itrk_new)->perigeeParameters();
                        	float trk_new_eta = tp_new.eta();
                        	float trk_new_phi = tp_new.parameters()[Trk::phi];
                        	if(deltaR(trk_eta,trk_new_eta,trk_phi,trk_new_phi)<0.0001) duplicated = true;
                	}
                	if(duplicated) continue;
                	if(dR<=0.2 && trk_pt>ltrk_pt) { ltrk_pt = trk_pt; ltrk_eta = trk_eta; ltrk_phi = trk_phi; ltrk_z0 = trk_z0;}
        	}
       	 	if(ltrk_pt==0.){
                	ATH_MSG_DEBUG("No leading trk found after second step of 2step trk");
                	ltrk_eta = RoI_eta;
                	ltrk_phi = RoI_phi;
                	applydz0 = false;
        	}
        	for(itrk=FTFIso->begin();itrk!=FTFIso->end();itrk++){
                	const Trk::Perigee tp = (*itrk)->perigeeParameters();
                	float trk_eta = tp.eta();
                	float trk_phi = tp.parameters()[Trk::phi];
                	float trk_z0 = tp.parameters()[Trk::z0];
               	 	float dPhi = deltaPhi(RoI_phi,trk_phi);
                	float dEta = fabs(RoI_eta - trk_eta);
                	float trk_pt((*itrk)->pt());
                	if(trk_pt<track_pt_min) continue;
                	if(dPhi>0.4 || dEta>0.4 || fabs(trk_z0)>225) continue;
                	if(updateRoI){
                	        float dZ0 = fabs(trk_z0 - RoI_z0);
                	        if(dZ0>10.) continue;
                	}
                	float dR = deltaR(ltrk_eta,trk_eta,ltrk_phi,trk_phi);
                	float dZ0 = fabs(ltrk_z0 - trk_z0);
                	if(applydz0 && dZ0>2.) continue;
                	bool duplicated(false);
                	for(itrk_new=FTFIso->begin();itrk_new!=itrk;itrk_new++){
                	        const Trk::Perigee tp_new = (*itrk_new)->perigeeParameters();
                	        float trk_new_eta = tp_new.eta();
                	        float trk_new_phi = tp_new.parameters()[Trk::phi];
                	        if(deltaR(trk_eta,trk_new_eta,trk_phi,trk_new_phi)<0.0001) duplicated = true;
                	}
                	if(duplicated) continue;
                	FTF_pt.push_back((*itrk)->pt());
                	FTF_eta.push_back(trk_eta);
                	FTF_phi.push_back(trk_phi);
                	FTF_z0.push_back(trk_z0);
                	FTF_nCore.push_back(false); if(dR<=0.2) FTF_nCore.back() = true;
               	 	FTF_nWide.push_back(false); if(dR>0.2 && dR<=0.4) FTF_nWide.back() = true;
        	}
		ATH_MSG_DEBUG("Emulated 2step trks, nTracks "<<FTF_pt.size());	
	}	

	unsigned int nCore_track(0), nWide_track(0);
        for(unsigned int x=0;x<FTF_nCore.size();x++){if(FTF_nCore.at(x)) nCore_track++; if(FTF_nWide.at(x)) nWide_track++;}
        if( nCore_track<4 && nWide_track<2 ) pass = true;
	ATH_MSG_DEBUG("nCore "<<nCore_track<<", nWide "<<nWide_track<<", pass "<<pass);
	return pass;
}

*/
