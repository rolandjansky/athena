// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AnalysisConfig_Jpsi.cxx */
/** @author Mark Sutton (sutt@cern.ch) **/
/** @author Peter Wijeratne (paw@hep.ucl.ac.uk) **/
/** @author Christian Gutschow (chris.g@cern.ch) **/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h" 

#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysis/TIDAVertex.h"

#include "TrigInDetAnalysisUtils/Filter_RoiSelector.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"
#include "muonEvent/MuonParamDefs.h"

#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/egamma.h"

#include "TrigIDJpsiMonitoring/AnalysisConfig_Jpsi.h"
#include "TrigIDJpsiMonitoring/JpsiTrack.h"
#include "TrigIDJpsiMonitoring/JpsiTrackSelector.h"
#include "TrigIDJpsiMonitoring/JpsiAnalysis.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

#include "VxVertex/VxContainer.h"

#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"

#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticle/TrigPhotonContainer.h"

#include "TMath.h"
#include <cassert>

using std::cout;
using std::endl;
using std::string;
using std::fabs;
using std::pow;
using std::vector;
using std::map;

//#include "Particle/TrackParticle.h"

using namespace TrigInDetAnalysis;

void AnalysisConfig_Jpsi::loop() {

  ++m_count_before;
  bool isPassed = false;
  //string inclTrigChain = "EF_mu24i_tight";
  string inclTrigChain = "EF_mu20";
  //string inclTrigChain = "EF_mu18"; // MC test
  //string checkTrigger = (m_refChainName.find("Zm") != string::npos)? "EF_2mu13" : string(m_refChainName);
  string checkTrigger = (m_refChainName.find("Zm") != string::npos)? inclTrigChain : string(m_refChainName);
  //string checkTrigger = (m_refChainName.find("Zm") != string::npos)? "EF_mu18_IDTrkNoCut_tight" : string(m_refChainName);
  //if( (*m_tdt)->isPassed("EF_mu20it_tight") )      isPassed = true;
  //else if( (*m_tdt)->isPassed("EF_mu24i_tight") )  isPassed = true;
  if( (*m_tdt)->isPassed(checkTrigger) )  isPassed = true;
  //if( (*m_tdt)->isPassed("EF_2mu13") && (*m_tdt)->isPassed(m_refChainName) )  isPassed = true;
  //if( (*m_tdt)->isPassed(m_refChainName) )  isPassed = true;

  if( isPassed ) {
    ++m_count_after;
    m_provider->msg(MSG::INFO) << "EVENT PASSED TRIGGER: " << checkTrigger << endmsg;

    Trig::ExpertMethods* em = (*m_tdt)->ExperimentalAndExpertMethods();
    em->enable();
    bool HLTTruncated = true;
    HLTTruncated = em->isHLTTruncated();
    if(HLTTruncated) {
      m_provider->msg(MSG::WARNING) << "Incomplete HLT result - ABORTING EVENT." << endmsg;
      return;
    }
    else {
      m_provider->msg(MSG::INFO) << "\033[94;mSTART EVENT -- ALGORITHM: " << m_testChainKey << "\033[m" << endmsg;
      makeClean();
      
      //get beam position
      double xbeam = 0;
      double ybeam = 0;
      //double xbeam_online = 0;
      //double ybeam_online = 0;
      
      if( m_iBeamCondSvc ) {

// #ifndef TIDA_NEWTRACKING_H
//         HepGeom::Point3D<double> vertex = m_iBeamCondSvc->beamPos();
//         //if(fabs(xbeam-vertex.x())>0.1 || fabs(ybeam-vertex.y())>0.1){
//           //m_provider->msg(MSG::INFO) << " using beam position\tx=" << vertex.x() << "\ty=" << vertex.y() << endmsg; 
//         //}
//         xbeam = vertex.x();
//         ybeam = vertex.y();
// #
// else 
	xbeam = m_iBeamCondSvc->beamPos()[0];
	ybeam = m_iBeamCondSvc->beamPos()[1];
// #endif
 
        m_analysis->setBeamRef( xbeam, ybeam );
      }
      /*if( m_iOnlineBeamCondSvc ) {
        HepPoint3D vertex = m_iOnlineBeamCondSvc->beamPos();
        //if (fabs(xbeam-vertex.x())>0.1 || fabs(ybeam-vertex.y())>0.1){
          m_provider->msg(MSG::INFO) << " using online beam position\tx=" << vertex.x() << "\ty=" << vertex.y() << endmsg; 
        //}
        xbeam_online = vertex.x();
        ybeam_online = vertex.y(); 
      }*/
        
      m_events++;
      
      if(m_provider->evtStore()->retrieve(m_eventInfo).isFailure()) {
        cout << "Failed to get EventInfo." << endl;
      }
      unsigned int runNumber   = m_eventInfo->event_ID()->run_number();
      unsigned int eventNumber = m_eventInfo->event_ID()->event_number();

      //cout << "NEW EVENT WITH RUN NUMBER: " << runNumber << " AND EVENT NUMBER: " << eventNumber << endl;

      // bug in 8 TeV muon setup until July 24th 2012
      // causing efficiency to be 100 % --> ignore!
      //if(m_isMuon && runNumber <= 207397)  return;

      //cout << "SOME VERTEX STUFF" << endl;

      vector<TIDA::Vertex*> vertices;
      const VxContainer* primaryVtxCollection;
      if(m_provider->evtStore()->retrieve(primaryVtxCollection, "VxPrimaryCandidate").isFailure()) {
        m_provider->msg(MSG::WARNING) << "Primary vertex container not found" << endmsg;
      }
      else {
	VxContainer::const_iterator vtxitr = primaryVtxCollection->begin();
	for(; vtxitr != primaryVtxCollection->end(); ++vtxitr) {
	  if( (*vtxitr)->vxTrackAtVertex()->size()>0 ) {
	    vertices.push_back( new TIDA::Vertex( (*vtxitr)->recVertex().position().x(), (*vtxitr)->recVertex().position().y(),
						 (*vtxitr)->recVertex().position().z(), 0,0,0, (*vtxitr)->vxTrackAtVertex()->size() ) );
	  }
	}  
      }
      if( m_selectorRef )  m_selectorJ = dynamic_cast<JpsiTrackSelector*>(m_selectorRef); 
      else {  
        return;
      }

      //cout << "Trig::FeatureContainer" << endl;

      bool doTruth = false;
      
      //temp
      vector<const TrigMuonEFTrack*> muonExtrapolatedTracks;
      vector<const TrigMuonEFCbTrack*> muonCombinedTracks;
      vector<const TrigMuonEFCbTrack*> incCombinedTracks;
      vector<const egamma*> tightElectrons_EF; 
      vector<const CaloCluster*> ClusterContainer_EF;
      const Analysis::MuonContainer* Muons = 0;
      vector<const Rec::TrackParticle*> offlineMSOnly;
      vector<const Rec::TrackParticle*> offlineInDet;
      //vector<const Rec::TrackParticle*> offlineInDet2;
      vector<const Analysis::Electron*> offTightElecs;
      vector<int> clusterRoiIDs, tightElecsRoiIDs;
      vector<int> tagRois, probeRois;
      //temp
      
      if( doTruth ) {
        //get truth tracks
        const TruthParticleContainer* truthTracks = 0;
        if(m_provider->evtStore()->contains<TruthParticleContainer>(m_truthCollection)) {
          StatusCode scm = m_provider->evtStore()->retrieve(truthTracks,m_truthCollection);
          if(scm.isFailure()) {
            if(m_events == 1) {
              m_provider->msg(MSG::WARNING) <<"No Collection with name "<<m_truthCollection<<" found in StoreGate" << endmsg;
            }
            else if(m_provider->msgLvl(MSG::DEBUG)) {
              m_provider->msg(MSG::DEBUG)<< "No Collection with name "<<m_truthCollection<<" found in StoreGate" << endmsg;
            }
          }
        } 
        else if (m_provider->msgLvl(MSG::DEBUG)) {
          m_provider->msg(MSG::DEBUG) << "StoreGate does not contain Collection with name "<<m_truthCollection<<" "<<endmsg;
        }
        if(!m_provider->evtStore()->contains<LVL1_ROI>("LVL1_ROI")) m_useTrigger=false;
        m_selectorJ->selectTracks( truthTracks );
      }
      else {
        if( !m_runOnline && m_isMuon ) {
          if(m_provider->evtStore()->contains<Analysis::MuonContainer>(m_muonCollection)) {
            StatusCode scm = m_provider->evtStore()->retrieve(Muons,m_muonCollection);
            if(scm.isFailure()) {
              if(m_events == 1) {
                m_provider->msg(MSG::WARNING) <<"No Collection with name "<<m_muonCollection<<" found in StoreGate" << endmsg;
              }
              else if(m_provider->msgLvl(MSG::DEBUG)) {
                m_provider->msg(MSG::DEBUG)<< "No Collection with name "<<m_muonCollection<<" found in StoreGate" << endmsg;
              }
            }
          } 
          else if(m_provider->msgLvl(MSG::DEBUG)) {
            m_provider->msg(MSG::DEBUG) << "StoreGate does not contain Collection with name "<<m_muonCollection<<" "<<endmsg;
          }
          //cout << "serious business below" << endl;
	  
          if(!m_provider->evtStore()->contains<LVL1_ROI>("LVL1_ROI"))  m_useTrigger=false;
          //iterators over the muon container and retrieve ID tracks only
          Analysis::MuonContainer::const_iterator MuonItr = Muons->begin(), MuonItrEnd = Muons->end();
          for(; MuonItr != MuonItrEnd; ++MuonItr) {
            if((*MuonItr)->isCombinedMuon()&&(*MuonItr)->hasInDetTrackParticle()) offlineInDet.push_back((*MuonItr)->inDetTrackParticle());
            if((*MuonItr)->hasMuonExtrapolatedTrackParticle()) offlineMSOnly.push_back((*MuonItr)->muonExtrapolatedTrackParticle());
          }
        }
        else if(m_isMuon) {      
          if(m_remover) {
            m_remover->clear();


            // MC test stuff:
            // inclusive trigger chain: "EF_mu24i_tight";
            cout << "\n\nMC test: START ..." << endl;

            cout << "MC test: Inclusive trigger chain: " << m_refChainName << endl;
            cout << "MC test: Get features with TrigDef::alsoDeactivateTEs ..." << endl;
            Trig::FeatureContainer fdeac = (*m_tdt)->features(m_refChainName, TrigDefs::alsoDeactivateTEs);
            cout << "MC test: Get combinations ..." << endl;
            const vector<Trig::Combination>& incComboDeac = fdeac.getCombinations();
            vector<Trig::Combination>::const_iterator incComboDeac_itr = incComboDeac.begin();
            vector<Trig::Combination>::const_iterator incComboDeac_itrEnd = incComboDeac.end();
            int nComb = (incComboDeac_itrEnd - incComboDeac_itr);
            cout << "MC test: nComb: " << nComb << endl;
            cout << "MC test: ... return if < 2." << endl;
            if(nComb < 2)  return;


            cout << "MC test: Get features with TrigDef::Physics ..." << endl;
            Trig::FeatureContainer fphys = (*m_tdt)->features(m_refChainName, TrigDefs::Physics);
            cout << "MC test: Get combinations ..." << endl;
            const vector<Trig::Combination>& incCombo = fphys.getCombinations();
            vector<Trig::Combination>::const_iterator incCombo_itr = incCombo.begin();
            vector<Trig::Combination>::const_iterator incCombo_itrEnd = incCombo.end();
            int nCombPhys = (incCombo_itrEnd - incCombo_itr);
            cout << "MC test: nCombPhys, nComb = " << nCombPhys << ", " << nComb << endl;
            cout << "MC test: ... return if nCombPhys < 1." << endl;
            if(nCombPhys < 1)  return;

            // It appears they didn't run our monitoring chain for the MC ...
            /*cout << m_refChainName << endl;
            Trig::FeatureContainer _fconto = (*m_tdt)->features(m_refChainName, TrigDefs::alsoDeactivateTEs);
            cout << "Get monitoring combinations... " << endl;
            const vector<Trig::Combination>& _combo = _fconto.getCombinations();
            vector<Trig::Combination>::const_iterator _c_itro = _combo.begin();
            int monitComb = (_combo.end() - _c_itro);
            cout << "monitoring combinations: " << monitComb << endl;*/

            vector<int> used_combis;
            string tagTE = "", TEName = "";
            //unsigned int m1roiID = 0, m2roiID = 0;
            cout << "MC test: Loop over all tags ..." << endl;
            for(int tag_index = 0; tag_index < nCombPhys; ++tag_index) {
            //incCombo_itr != incCombo_itrEnd; ++incCombo_itr) {
              cout << "   MC test: current tag: " << tag_index << endl;
              vector< Trig::Feature< TrigMuonEFInfoContainer > > tagmuons = incCombo_itr->get< TrigMuonEFInfoContainer >("MuonEFInfo", TrigDefs::Physics, tagTE);
              ++incCombo_itr;
              cout << "   MC test: number of muons in tag combination: " << tagmuons.size() << endl;
              assert(tagmuons.size() == 1 && "WARNING -- wrong number of muons in this combination!!!");
              const HLT::TriggerElement* m1te = tagmuons[0].te();
              TEName = (Trig::getTEName(*m1te).c_str());
              cout << "   MC test: TE name for muon " << tag_index << " is: " << TEName << endl;
              //cout << "   MC test: TE ID = " << (*m1te).getId() << endl;
              cout << "   MC test: getActiveState: " << (*m1te).getActiveState() << endl;
              //m1roiID = (*m_tdt)->ancestor<TrigRoiDescriptor>(tagmuons[0], "forID").cptr()->roiId();
              //cout << "   MC test: RoI ID (mu1): " << m1roiID << " (" << tag_index << ")" << endl;

              //Trig::Feature< TrigMuonEFInfoContainer > trackFeature1 = tagmuons.at(0);
              const TrigMuonEFInfoContainer* trigMuon = tagmuons.at(0).cptr();
              //TrigMuonEFInfoContainer::const_iterator muItr1  = trigMuon->begin();
                    const TrigMuonEFInfoTrackContainer* tagCon = (*(trigMuon->begin()))->TrackContainer();
	            TrigMuonEFInfoTrackContainer::const_iterator trackItr = tagCon->begin();
	            TrigMuonEFInfoTrackContainer::const_iterator trackItrEnd = tagCon->end();
              //int nCons = int(trackItrEnd - trackItr);
              //cout << "Number of trackcontainer elements: " << nCons << endl;
              TrigMuonEFCbTrack* tagCB = 0;
              for(; trackItr != trackItrEnd; ++trackItr) {
	              const TrigMuonEFInfoTrack* muonInfo = (*trackItr);
                bool pass = muonInfo->hasCombinedTrack();
                cout << "hasCombinedTrack: " << pass << endl;
                if(pass)  tagCB = muonInfo->CombinedTrack();
              }
              if(!tagCB) { cout << "TAG IGNORED." << endl; continue; }
              //const TrigMuonEFInfoContainer* trigMuon = tagmuons[0].cptr();
	            //TrigMuonEFInfoTrackContainer* tagCon = trigMuon->at(0)->TrackContainer();
	            //TrigMuonEFCbTrack* tagCB = tagCon->at(0)->CombinedTrack(); 
	            //TrigMuonEFCbTrack* tagCB = muonInfo->CombinedTrack(); 
			        TrigMuonEFTrack* tag = tagCon->at(0)->ExtrapolatedTrack(); 
			        //TrigMuonEFTrack* tag   = tagmuons[0].cptr()->at(0)->TrackContainer()->at(0)->ExtrapolatedTrack(); 

              cout << "   MC test: Loop over all combinations including the deactived ones ..." << endl;
              //vector<Trig::Combination>::const_iterator deacCombo_itr = incComboDeac_itr;
              //vector<Trig::Combination>::const_iterator deacCombo_itr = incCombo_itr + 1;
              incComboDeac_itr = incComboDeac.begin();
              double minMass = 0.;
              for(int deac_index = 0; deac_index < nComb; ++deac_index) {
              //deacCombo_itr < incComboDeac_itrEnd; ++deacCombo_itr) {
                cout << "MC test: Indices for current combination: " << tag_index
                     << ", " << deac_index  << " (ignore if equal)" << endl;
                cout << "      MC test: current combination: " << deac_index << endl;
                vector< Trig::Feature< TrigMuonEFInfoContainer > > combimuons = incComboDeac_itr->get< TrigMuonEFInfoContainer >("MuonEFInfo", TrigDefs::alsoDeactivateTEs, tagTE);
                ++incComboDeac_itr;
                cout << "      MC test: number of muons in current combination: " << combimuons.size() << endl;
                assert(combimuons.size() == 1 && "WARNING -- wrong number of muons in this combination!!!");
                const HLT::TriggerElement* m2te = combimuons[0].te();
                TEName = (Trig::getTEName(*m2te).c_str());
                cout << "      MC test: TE name for muon " << deac_index << " is: " << TEName << endl;
                //cout << "      MC test: TE ID = " << (*m2te).getId() << endl;
                cout << "      MC test: getActiveState: " << (*m2te).getActiveState() << endl;
                //m2roiID = (*m_tdt)->ancestor<TrigRoiDescriptor>(combimuons[0], "forID").cptr()->roiId();
                //cout << "      MC test: RoI ID (mu2): " << m2roiID << " (" << deac_index << ")" << endl;
                //cout << "      MC test: compare RoIs: (mu1, mu2) = (" << m1roiID << ", " << m2roiID << ")" << endl;

                const TrigMuonEFInfoContainer* trigMu = combimuons.at(0).cptr();
                const TrigMuonEFInfoTrackContainer* probeCon = (*(trigMu->begin()))->TrackContainer();
                TrigMuonEFInfoTrackContainer::const_iterator trackItr = probeCon->begin();
                TrigMuonEFInfoTrackContainer::const_iterator trackItrEnd = probeCon->end();
			          TrigMuonEFTrack* probe = 0; 
                for(; trackItr != trackItrEnd; ++trackItr) {
                  const TrigMuonEFInfoTrack* muonInfo = (*trackItr);
                  bool pass = muonInfo->hasExtrapolatedTrack();
                  cout << "hasExtrapolatedTrack: " << pass << endl;
                  if(pass)  probe = muonInfo->ExtrapolatedTrack();
                }
                if(!probe) { cout << "PROBE IGNORED." << endl; continue; }
			          //TrigMuonEFTrack* probe = combimuons[0].cptr()->at(0)->TrackContainer()->at(0)->ExtrapolatedTrack(); 
                if( (*tag) == (*probe) || !tagCB )  { cout << "IGNORED." << endl; continue; }
                int tagRoiID = (*m_tdt)->ancestor<TrigRoiDescriptor>(tagmuons[0], "").cptr()->roiId();
                int probeRoiID = (*m_tdt)->ancestor<TrigRoiDescriptor>(combimuons[0], "").cptr()->roiId();
                cout << "tag pt, eta, phi: "   << tag->pt()   << " " << tag->eta()   << " " << tag->phi()   << endl;
                cout << "probe pt, eta, phi: " << probe->pt() << " " << probe->eta() << " " << probe->phi() << endl;
                cout << ">>> TEST 1: " << tag << " " << (*tag) << endl;
                TLorentzVector v1, v2;
                double energy = sqrt(pow(m_muonMass,2) + pow(tag->px(),2) + pow(tag->py(),2) + pow(tag->pz(),2));
                v1.SetPtEtaPhiE(tag->pt(), tag->eta(), tag->phi(), energy);
                energy = sqrt(pow(m_muonMass,2) + pow(probe->px(),2) + pow(probe->py(),2) + pow(probe->pz(),2));
                v2.SetPtEtaPhiE(probe->pt(), probe->eta(), probe->phi(), energy);
                double mass = (v1 + v2).Mag();
                cout << "      MC test: invariant mass: " << mass << endl;
                if(!minMass) {
                  minMass = mass;
                  used_combis.push_back( 100 + 10 * tag_index + deac_index );
                  muonExtrapolatedTracks.push_back(probe);
                  muonCombinedTracks.push_back(tagCB);
                  probeRois.push_back(probeRoiID);
                  tagRois.push_back(tagRoiID);
                }
                else if(mass < minMass) {
                  minMass = mass;
                  used_combis[used_combis.size() - 1] = (100 + 10 * tag_index + deac_index );
                  muonExtrapolatedTracks[muonExtrapolatedTracks.size() - 1] = probe;
                  //muonCombinedTracks[muonCombinedTracks.size() - 1] = tagCB;
                  probeRois[probeRois.size() - 1] = probeRoiID;
                  //tagRois[tagRois.size() - 1] = tagRoiID;
                }
              } // loop over deactivatedTEs
            } // loop over physics
            cout << "MC test: combinations being used:" << endl;
            for(int ii = 0; ii < int(used_combis.size()); ++ii) { cout << " " << used_combis[ii] << endl; }
            cout << "MC test: END.\n" << endl;
            //return;

            /*m_provider->msg(MSG::DEBUG) << "Get features... " << endmsg;
            Trig::FeatureContainer fconto = (*m_tdt)->features(m_refChainName, m_trigDefs);
            m_provider->msg(MSG::DEBUG) << "Get combinations... " << endmsg;
            const vector<Trig::Combination>& combo = fconto.getCombinations();
            vector<Trig::Combination>::const_iterator c_itro = combo.begin();
            m_provider->msg(MSG::DEBUG) << "combo.end() - combo.begin(): " << (combo.end() - c_itro) << endmsg;
            if(c_itro == combo.end()) return;
            m_provider->msg(MSG::DEBUG) << "m_remover->MuonMaker(...)" << endmsg;
            m_remover->muonMaker(c_itro, m_refChainName, m_tdt, m_trigDefs);
            muonExtrapolatedTracks = m_remover->getExTrks();
            muonCombinedTracks = m_remover->getCbTrks();*/

            //cout << "Number of combined tracks before making J/Psi = " << muonCombinedTracks.size()
            //     << " extrp tracks = " << muonExtrapolatedTracks.size() << endl;
          }
          //temp
          if(m_provider->evtStore()->contains<Analysis::MuonContainer>(m_muonCollection)) {
            StatusCode scm = m_provider->evtStore()->retrieve(Muons,m_muonCollection);
            if(scm.isFailure()) {
              if(m_events==1) {
                m_provider->msg(MSG::WARNING) <<"No Collection with name "<<m_muonCollection<<" found in StoreGate" << endmsg;
              }
              else if(m_provider->msgLvl(MSG::DEBUG)) {
                m_provider->msg(MSG::DEBUG)<< "No Collection with name "<<m_muonCollection<<" found in StoreGate" << endmsg;
              }
	      Muons = 0;
            }
          } 
          else {
            if(m_provider->msgLvl(MSG::DEBUG)) {
              m_provider->msg(MSG::DEBUG) << "StoreGate does not contain Collection with name " << m_muonCollection << " " << endmsg;
            }
          }
          if(!m_provider->evtStore()->contains<LVL1_ROI>("LVL1_ROI"))  m_useTrigger = false;
          // iterators over the muon container and retrieve ID tracks only
	  if (Muons){
	    Analysis::MuonContainer::const_iterator MuonItr = Muons->begin(), MuonItrEnd = Muons->end();
	    for(; MuonItr != MuonItrEnd; ++MuonItr) {
	      if((*MuonItr)->isCombinedMuon()&&(*MuonItr)->hasInDetTrackParticle())  offlineInDet.push_back((*MuonItr)->inDetTrackParticle());
	      if((*MuonItr)->hasMuonExtrapolatedTrackParticle())  offlineMSOnly.push_back((*MuonItr)->muonExtrapolatedTrackParticle());
	    }
	  }
          ////temp
        }
        else {
          m_provider->msg(MSG::DEBUG) << "EF_egamma stuff" << endmsg;
          Trig::FeatureContainer f = (*m_tdt)->features(m_refChainName, TrigDefs::Physics);
          const vector<Trig::Combination> comb_container_EF = f.getCombinations();
          Trig::FeatureContainer::combination_const_iterator c(comb_container_EF.begin()); 
          Trig::FeatureContainer::combination_const_iterator cEnd(comb_container_EF.end());
          if(comb_container_EF.size()) {
            int counter_egamma  = -1;
            int counter_cluster = -1;
            for(Trig::FeatureContainer::combination_const_iterator my_it = c; my_it != cEnd; ++my_it) {
              vector< Trig::Feature<TrigRoiDescriptor> > vecRoi = my_it->get<TrigRoiDescriptor>("");
              //vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roiItr = vecRoi.begin();
              //vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roiLast = vecRoi.end();
              /*for( ; (roiItr != roiLast); ++roiItr) {
                const HLT::TriggerElement* a = (*roiItr).te();
                a = const_cast<HLT::TriggerElement*>(a);
                string TEName = "";
                TEName = (Trig::getTEName((*a))).c_str();
                m_provider->msg(MSG::DEBUG) << "TE Name: " << TEName << endmsg;
              }*/
              //const vector<Trig::Feature<CaloClusterContainer> > cluster_cont = my_it->get<CaloClusterContainer>("");
              m_provider->msg(MSG::DEBUG) << "Get calo cluster..." << endmsg;
              std::string teName = "";
              //if(m_refChainName.find("Zee") != string::npos)   teName = "EF_e15_NoCutcalo";
              //cout << "try TE name: " << teName << endl;
              const vector<Trig::Feature<CaloClusterContainer> > cluster_cont = my_it->get<CaloClusterContainer>("",TrigDefs::Physics,teName);
              m_provider->msg(MSG::DEBUG) << "egamma container cluster_cont size: " << cluster_cont.size() << endmsg;
              if(cluster_cont.size() != 0) {
                vector<Trig::Feature<CaloClusterContainer> >::const_iterator tempEnd = cluster_cont.end();
                for(vector<Trig::Feature<CaloClusterContainer> >::const_iterator contit = cluster_cont.begin(); contit != tempEnd; ++contit) {
                  ++counter_cluster;
                  if(!(counter_cluster%2 == 0)) {
                    //cout << "There are " << ((*contit).cptr())->size() << " loose cluster(s) in this event" << endl; 
                    Trig::Feature<TrigRoiDescriptor> trackroi = (*m_tdt)->ancestor<TrigRoiDescriptor>(*contit, "");
                    const TrigRoiDescriptor* roiId = trackroi.cptr();
                    DataVector<CaloCluster>::const_iterator temp2End = ((*contit).cptr())->end();
                    for(DataVector<CaloCluster>::const_iterator clusItr = ((*contit).cptr())->begin(); clusItr!= temp2End; ++clusItr) {
                      const CaloCluster* tempCalo = *clusItr;
                      //if(TMath::Abs(tempCalo->eta())<1.37 || TMath::Abs(tempCalo->eta())>1.52) {
                        ClusterContainer_EF.push_back(tempCalo);
                        clusterRoiIDs.push_back(roiId->roiId());
                        m_provider->msg(MSG::DEBUG) << "cluster egamma roiId: " << roiId->roiId() << endmsg;
                      //}
                    }
                  }
                }
              }
              //const vector< Trig::Feature<egammaContainer> > temp = my_it->get<egammaContainer>("");
              m_provider->msg(MSG::DEBUG) << "calocluster size: " << cluster_cont.size() << endmsg;
              m_provider->msg(MSG::DEBUG) << "get tight electrons ..." << endmsg;
              teName = "";
              //if(m_refChainName.find("Zee") != string::npos)  teName = "EF_e24vh_tight1";
              //cout << "try TE name: " << teName << endl;
              const vector<Trig::Feature<egammaContainer> > temp = my_it->get<egammaContainer>("",TrigDefs::Physics,teName);
              m_provider->msg(MSG::DEBUG) << "egamma container temp size: " << temp.size() << endmsg;
              if(temp.size() != 0) {
                vector<Trig::Feature<egammaContainer> >::const_iterator t0 = temp.begin();
                vector<Trig::Feature<egammaContainer> >::const_iterator tEnd = temp.end();
                for(vector< Trig::Feature<egammaContainer> >::const_iterator contit = t0; contit != tEnd; ++contit){
                  ++counter_egamma;
                  if(counter_egamma%2 == 0) {
                    Trig::Feature<TrigRoiDescriptor> trackroi = (*m_tdt)->ancestor<TrigRoiDescriptor>(*contit, "");
                    const TrigRoiDescriptor* roiId = trackroi.cptr();
                    DataVector<egamma>::const_iterator e0   = ((*contit).cptr())->begin();
                    DataVector<egamma>::const_iterator eEnd = ((*contit).cptr())->end();
                    for( DataVector<egamma>::const_iterator elecItr = e0; elecItr != eEnd; ++elecItr) {
                      const egamma* tempEl = *elecItr; 
                      //if(TMath::Abs(tempEl->eta())<1.37 ||TMath::Abs(tempEl->eta())>1.52) {
                        tightElectrons_EF.push_back(tempEl);
                        tightElecsRoiIDs.push_back(roiId->roiId());
                        m_provider->msg(MSG::DEBUG) << "tight egamma roiId: " << roiId->roiId() << endmsg;
                        //egamma_tightEl_EF.push_back(tempEl->trackParticle());
                      //}
                    }
                  }
                }
              }
              m_provider->msg(MSG::DEBUG) << "tightelectrons size: " << tightElectrons_EF.size() << endmsg;
              /*vector< Trig::Feature<Rec::TrackParticleContainer> > feature_EF = my_it->get<Rec::TrackParticleContainer>("InDetTrigParticleCreation_Electron_EFID");
              vector< Trig::Feature<Rec::TrackParticleContainer> >::const_iterator contit    = feature_EF.begin();
              vector< Trig::Feature<Rec::TrackParticleContainer> >::const_iterator contitEnd = feature_EF.end();
              for( ; contit != contitEnd; ++contit) {
                const Rec::TrackParticleContainer *cont = (*contit).cptr();
                Rec::TrackParticleContainer::const_iterator track    = cont->begin();
                Rec::TrackParticleContainer::const_iterator trackEnd = cont->end();
                for( ; track != trackEnd; ++track) {
                  //cout << "NEWEST_TEST" << endl;
                  //const Rec::TrackParticle *temp = *track;
                  offlineInDet2.push_back(*track);
                }
              }*/
            }
          }
          m_provider->msg(MSG::DEBUG) << "Start offline bit ..." << endmsg;
          vector<int> tagRoIs;
          const DataHandle<ElectronContainer> ElectronCollection;
          string electronCollection = "ElectronAODCollection";
          StatusCode sc_electrons = m_provider->evtStore()->retrieve( ElectronCollection, electronCollection);
          if(!sc_electrons.isFailure())  m_provider->msg(MSG::DEBUG) << "OFFLINE -- number of electrons: " << ElectronCollection->size() << endmsg;
          if(ElectronCollection && ElectronCollection->size() != 0) {
            ElectronContainer::const_iterator elecItrEnd = ElectronCollection->end();
            for(ElectronContainer::const_iterator elecItr = ElectronCollection->begin(); elecItr != elecItrEnd; ++elecItr) { 
              const Analysis::Electron* temp = *elecItr;
              /*cout << "isem(0) = " << temp->isem(egammaPID::ElectronMediumPP) << "  passID(1) = "
                   << temp->passID(egammaPID::ElectronIDMediumPP) << endl;*/
              if(temp->isem(egammaPIDObs::ElectronMediumPP)==0 || 1==temp->passID(egammaPIDObs::ElectronIDMediumPP)) {
              //if(temp->isem(egammaPID::ElectronTightPP)==0 || 1==temp->passID(egammaPID::ElectronIDTightPP)) {
                //if(TMath::Abs(temp->eta())<1.37 || TMath::Abs(temp->eta())>1.52) {
                  m_provider->msg(MSG::DEBUG) << "OFFLINE -- electron passed [ pT = " << temp->pt()
                                              << ", eta = " << temp->eta() << ", phi = " << temp->phi()
                                              << ", charge = " << temp->charge() << " ]" << endmsg;
                  cout << "OFFLINE -- electron passed [ pT = " << temp->pt()
                       << ", eta = " << temp->eta() << ", phi = " << temp->phi()
                       << ", charge = " << temp->charge() << " ]" << endl;
                if(temp->charge())  offTightElecs.push_back(temp); // tight
                //}       
              }
            } 
            //cout << "OFFLINE -- Number of electrons after filtering: " << offlineInDet2.size() << " candidates!!" << endl;
          }
          //cout << "OFFLINE_END_OF_LOOP" << endl;
        } // end of electron bit
        if(m_jpsi) {
          cout << "prepare making dilepton pairs..." << endl;
          m_jpsi->initializeMembers();
          m_jpsi->doTagAndProbe(m_TagAndProbe);
          m_jpsi->useTrigger(m_useTrigger); //tag a combined muon passing L1 muon ROI; not necessary for EF muons
          m_jpsi->setTrackMass(m_muonMass);
          m_jpsi->setUpperInvMassLimit(m_jpsiPDGmass+m_plusMass);
          m_jpsi->setLowerInvMassLimit(m_jpsiPDGmass-m_minusMass);
          cout << "making dilepton pairs..." << endl;
          if(m_isMuon) {
            cout << "set tags..." << endl;
            m_jpsi->setTags(muonCombinedTracks, tagRois); //m_remover->getTagRoIs());
            cout << "set probes..." << endl;
            m_jpsi->setProbes(muonExtrapolatedTracks, probeRois); //m_remover->getProbeRoIs());
            //m_jpsi->setProbes(muonExtrapolatedTracks, m_remover->getMatches(), m_remover->getRoIs());
          }
          else {
            m_jpsi->setTags(tightElectrons_EF,tightElecsRoiIDs);
            m_jpsi->setProbes(ClusterContainer_EF,clusterRoiIDs);
          }
          cout << "finish off making dilepton pairs..." << endl;
          m_jpsi->setPtcut(m_ptcut);
          m_jpsi->setTagPtcut(m_tagptcut);
          m_jpsi->setEtacut(m_etacut);
          m_jpsi->setTagEtacut(m_tagetacut);
          cout << "get the pairs..." << endl;

          //cout << "AC_Jpsi extrapolated tracks " << muonExtrapolatedTracks.size() << " combined tracks " << muonCombinedTracks.size() << endl;
          vector<DiMuonTool*> mypairs = m_jpsi->execute();
          m_provider->msg(MSG::DEBUG) << "jpsi candidates: " << mypairs.size() << endmsg;
          /*for(unsigned int q = 0; q < mypairs.size(); q++) {
            m_provider->msg(MSG::DEBUG) << "pair: " << q << " mypairs[q]: " << mypairs[q] << endmsg;
            const Wrapper::MuonTrack *p = mypairs[q]->ProbedTrack();
            m_provider->msg(MSG::DEBUG) << "probed track -- pt = " << p->pt() << " eta = " << p->eta() << " phi = " << p->phi() << endmsg;
            const Wrapper::MuonTrack *t = mypairs[q]->TaggedTrack();
            m_provider->msg(MSG::DEBUG) << "tagged track -- pt = " << t->pt() << " eta = " << t->eta() << " phi = " << t->phi() << endmsg;
          }*/
	        //to protect against > 1 J/Psi's being made with 1 tag and > 1 similar probes
          //take the pair with invariant mass closest to the J/Psi
          //for 1 J/Psi, this will be seen as > 2 J/Psi candidates...
          /*if(mypairs.size() > 2){
            double PI=3.14159265;
            for(unsigned i=0; i<mypairs.size(); i++) {
              for(unsigned j = i+1; j<mypairs.size(); ) {
                double deta = mypairs[i]->ProbedTrack()->eta() - mypairs[j]->ProbedTrack()->eta();
                double dphi = mypairs[i]->ProbedTrack()->phi() - mypairs[j]->ProbedTrack()->phi();
                if(dphi >  PI)  dphi -= 2*PI;
                if(dphi < -PI)  dphi += 2*PI;
                double dr = sqrt(pow(deta,2) + pow(dphi,2));
                if(dr < 0.5) {
                  if(fabs(m_jpsiPDGmass - mypairs[i]->InvariantMass()) < fabs(m_jpsiPDGmass - mypairs[j]->InvariantMass())) {
                    mypairs.erase(mypairs.begin()+j); 
                    break;
                  }
                  else {
                    mypairs.erase(mypairs.begin()+i);
                    break;
                  }
                }
                else j++;
              }
            }
          }*/
          m_selectorJ->addJpsi(mypairs);
          m_provider->msg(MSG::DEBUG) << "Pair added." << endmsg;
          cout << "SKIM CANDIDATES -- event #: " << eventNumber << " run #: " << runNumber << endl;
        } // if( m_jpsi )
      } // end of else -> if( doTruth )
	  
      m_provider->msg(MSG::DEBUG) << "Call m_selectorJ->tracks()" << endmsg;
      vector<TIDA::Track*> probeTracks = m_selectorJ->tracks();
      unsigned int trackSize = probeTracks.size();
      m_provider->msg(MSG::DEBUG) << "probeTracks size: " << trackSize << endmsg;
      
      // perform dR match between offline extrapolated tracks and EF probes
      map<unsigned int, const Rec::TrackParticle*> offlineProbes, offlineTags;
      vector<float> pOffDr(trackSize),   tOffDr(trackSize),     pOffDeta(trackSize),
                    pOffDphi(trackSize), pOffCharge(trackSize), tOffCharge(trackSize),
                    pOffEt(trackSize);
      float ptCut = 4000., tagPtCut = 5000.;
      if(!m_isJpsi) {
        ptCut = 15000.;
        tagPtCut = 24000.;
      }

      m_provider->msg(MSG::DEBUG) << "loop over probeTracks." << endmsg;
      for(unsigned int j=0; j< trackSize; j++) {

        const Rec::TrackParticle* closestProbe = 0, *closestTag = 0;
        TIDA::JpsiTrack* probe = 0;
        const Wrapper::MuonTrack *tag = 0;
        const CaloCluster *offClust = 0;

        // Make sure there is a default value of 0 if there is no offline match
        offlineProbes[j] = offlineTags[j] = 0;
        pOffDr[j] = tOffDr[j] = pOffDeta[j] = pOffDphi[j] = 0;
        pOffCharge[j] = tOffCharge[j] = pOffEt[j] = 0;

        m_provider->msg(MSG::DEBUG) << "probeTracks[j] --> " << probeTracks[j] << endmsg;
        if(probeTracks[j]) {
          m_provider->msg(MSG::DEBUG) << "Get probe ..."  << endmsg;
          probe = dynamic_cast<TIDA::JpsiTrack*>(probeTracks[j]);
          m_provider->msg(MSG::DEBUG) << "probe: " << probe << " and *probe: " << *probe << endmsg;
          m_provider->msg(MSG::DEBUG) << "Get tag ..."  << endmsg;
          tag = probe->getTag();
          m_provider->msg(MSG::DEBUG) << "tag: " << tag << endmsg;
          m_provider->msg(MSG::DEBUG) << "... done." << endmsg;
        }
        else throw std::runtime_error("*** TrigJpsiMonTool *** Probe not casting, aborting!!! *** TrigJpsiMonTool ***");


        float tempP = 999., tempT = 999., detaP = 0., dphiP = 0., etP = 0.;
        m_provider->msg(MSG::DEBUG) << "Loop over offline objects..." << endmsg;
        unsigned int offSize = m_isMuon? offlineInDet.size() : offTightElecs.size();
        for(unsigned int i = 0; i < offSize; i++) {

          if(!m_isMuon) {
            unsigned int author = offTightElecs[i]->author();
            m_provider->msg(MSG::DEBUG) << "author: " << author << endmsg;
            if(author != 1 && author != 3)  continue;
            offClust = offTightElecs[i]->cluster();
            //cout << "off electron pT: " << offTightElecs[i]->pt() << endl;
            //cout << "off cluster pT: " << offClust->pt() << endl;
            //cout << "off track pT: " << offTightElecs[i]->trackParticle()->pt() << endl;
          }

          float offEta = m_isMuon? offlineInDet[i]->eta() : offClust->eta();
          float offPhi = m_isMuon? offlineInDet[i]->phi() : offClust->phi();
          float offPt  = m_isMuon? offlineInDet[i]->pt()  : offClust->pt();
          float deltar = AnalysisConfig_Jpsi::deltaR(offEta, offPhi, probe->eta(), probe->phi());
          m_provider->msg(MSG::DEBUG) << "OFFLINE PROBE [ pt = " <<  offPt << " eta = " << offEta
                                      << " phi = " << offPhi << " ]" << endmsg;
          if(deltar < tempP && offPt > ptCut) {
            tempP = deltar;
            detaP = offEta - probe->eta();
            dphiP = AnalysisConfig_Jpsi::deltaPhi(offPhi, probe->phi());
            closestProbe = m_isMuon? offlineInDet[i] : offTightElecs[i]->trackParticle();
            etP = offPt; // cluster mass always zero, so I guess ET = pT in this case?
          }

          if(!m_isMuon) {
            offEta = offTightElecs[i]->eta();
            offPhi = offTightElecs[i]->phi();
            offPt  = offTightElecs[i]->pt();
          }
          deltar = AnalysisConfig_Jpsi::deltaR(offEta, offPhi, tag->eta(), tag->phi());
          m_provider->msg(MSG::DEBUG) << "OFFLINE TAG [ pt = " <<  offPt << " eta = " << offEta
                                      << " phi = " << offPhi << " ]" << endmsg;
          //cout << deltar << endl;
          if(deltar < tempT && offPt > tagPtCut) {
            tempT = deltar;
            closestTag = m_isMuon? offlineInDet[i] : offTightElecs[i]->trackParticle();
          }
        }
       
        if(closestProbe) {
          const Trk::TrackSummary *summary = closestProbe->trackSummary();
          int nBlayerHits = 2*summary->get(Trk::numberOfInnermostPixelLayerHits);
          int nPixelHits  = 2*summary->get(Trk::numberOfPixelHits);  
          int nSctHits    = summary->get(Trk::numberOfSCTHits); 
          //int nStrawHits  = summary->get(Trk::numberOfTRTHits);
          //int nTrHits     = summary->get(Trk::numberOfTRTHighThresholdHits);

          if(tempP < 0.15 && nBlayerHits > 0 && nPixelHits > 1 && nSctHits > 5) {
            offlineProbes[j] = closestProbe;
            pOffDr[j] = tempP;
            pOffDeta[j] = detaP;
            pOffDphi[j] = dphiP;
            pOffCharge[j] = closestProbe->charge();
            pOffEt[j] = etP;
          }
        }
        if(closestTag) {
          const Trk::TrackSummary *summary = closestTag->trackSummary();
          int nBlayerHits = 2*summary->get(Trk::numberOfInnermostPixelLayerHits);
          int nPixelHits  = 2*summary->get(Trk::numberOfPixelHits);  
          int nSctHits    = summary->get(Trk::numberOfSCTHits); 
          //int nStrawHits  = summary->get(Trk::numberOfTRTHits);
          //int nTrHits     = summary->get(Trk::numberOfTRTHighThresholdHits);

          if(tempT < 0.15 && nBlayerHits > 0 && nPixelHits > 1 && nSctHits > 5) {
            offlineTags[j] = closestTag;
            tOffDr[j] = tempT;
            tOffCharge[j] = closestTag->charge();
          }
        }
      }

      m_provider->msg(MSG::DEBUG) << "Set probes and tags for m_analysisJ..." << endmsg;
      m_analysisJ->setProbeOfflineDr(pOffDr);
      m_analysisJ->setProbeOfflineEt(pOffEt);
      m_analysisJ->setProbeOfflineDEta(pOffDeta);
      m_analysisJ->setProbeOfflineDPhi(pOffDphi);
      m_analysisJ->setProbeOfflineCharge(pOffCharge);
      m_analysisJ->setTagOfflineDr(tOffDr);
      m_analysisJ->setTagOfflineCharge(tOffCharge);
      m_analysisJ->setOfflineProbes(offlineProbes);
      m_analysisJ->setOfflineTags(offlineTags);

      // Fill vector of test tracks from the TDT feature 
      vector<int> sizes;
      m_provider->msg(MSG::DEBUG) << "Start to select tracks with " << m_testChainKey << " / " << m_refChainName << endmsg;
      bool check  = m_testChainKey.find("IDSCAN")        != string::npos;
      check      |= m_testChainKey.find("SegmentFinder") != string::npos;
      check      |= m_testChainKey.find("SiTrack_")      != string::npos;
      check      |= m_testChainKey.find("SegFinder")     != string::npos;  
      if(check)  selectTracks<TrigInDetTrackCollection>(m_testChainKey, 0, m_refChainName);

      if(m_testChainKey.find("SiTrackFinder") != string::npos) { 
        if(m_isMuon) {
          if(m_refChainName.find("_mu20") != string::npos) {
            selectTracks<TrigInDetTrackCollection>("TrigSiTrack_Muon", 0, m_refChainName);
          }
          else if(m_testChainKey.find("MuonA") != string::npos) {
            selectTracks<TrigInDetTrackCollection>("TrigL2SiTrackFinder_Muon", 0, m_refChainName);
          }
          else if(m_testChainKey.find("MuonB") != string::npos) {
            selectTracks<TrigInDetTrackCollection>("TrigL2SiTrackFinder_Muon", 1, m_refChainName);
          }
          else if(m_testChainKey.find("MuonC") != string::npos) {
            selectTracks<TrigInDetTrackCollection>("TrigL2SiTrackFinder_Muon", 2, m_refChainName);
          }
        }
        else {
          if(m_testChainKey.find("eGammaA") != string::npos) {
            selectTracks<TrigInDetTrackCollection>("TrigL2SiTrackFinder_eGamma", 0, m_refChainName);
          }
          else if(m_testChainKey.find("eGammaB") != string::npos) {
            selectTracks<TrigInDetTrackCollection>("TrigL2SiTrackFinder_eGamma", 1, m_refChainName);
          }
          else if(m_testChainKey.find("eGammaC") != string::npos) {
            selectTracks<TrigInDetTrackCollection>("TrigL2SiTrackFinder_eGamma", 2, m_refChainName);
          }
        }
      }

      if(m_testChainKey.find("InDetTrig") != string::npos) {
        selectTracks0<Rec::TrackParticleContainer>(m_testChainKey, 0, m_refChainName);
      }
      
      //const vector<Track*> &testtracksA = m_selectorTestA->tracks();
      const vector<TIDA::Track*> testtracksA = m_trigTracks;
      sizes.push_back(testtracksA.size()); 

      m_provider->msg(MSG::DEBUG) << "... done. Ntracks = " << testtracksA.size() << " ( " << m_testChainKey << " / " << m_refChainName << " )" << endmsg;

      m_analysisJ->setTIDARois(m_trackRois);
      //m_analysisJ->setEtaAtCalo(m_etaAtCalo);
      //m_analysisJ->setPhiAtCalo(m_phiAtCalo);

      if(m_associator) m_associatorJ = dynamic_cast<Associator_BestDeltaRMatcher*>(m_associator);

      //match the 2 sets of tracks ... this is no longer necessary, since we match by mass confirmation
      //m_associatorJ->match(probeTracks, testtracks);
      m_analysis->setBeamRef(xbeam,  ybeam);
      m_analysis->setBeamTest(xbeam, ybeam);

      m_analysisJ->setVertices(vertices);
      m_analysisJ->setCase(m_isMuon, m_isJpsi);

      m_analysis->execute(probeTracks, testtracksA, m_associator);

      m_provider->msg(MSG::DEBUG) << "Final clean up ..." << endmsg;
      for(vector<TIDA::Vertex*>::const_iterator it = vertices.begin(); it != vertices.end(); it++)  delete (*it);
      m_analysisJ->clearOffline();
      // m_analysisJ->clearSuper();
      //m_selectorTestB->clear();
      
      m_provider->msg(MSG::DEBUG) << "END EVENT." << endmsg;
     }//end of complete result condition
  }//end of trigger condition
}

/// setup the analysis the analysis, retrieve the tools etc 

void AnalysisConfig_Jpsi::book() { 
  
  m_events = m_count_before = m_count_after = 0;

  m_isJpsi  = (m_refChainName.find("Zmumu") == string::npos);
  m_isJpsi &= (m_refChainName.find("mu24i") == string::npos);
  m_isJpsi &= (m_refChainName.find("mu20")  == string::npos);
  m_isJpsi &= m_refChainName.find("Zee")    == string::npos;

  m_isMuon = (m_refChainName.find("mu") != string::npos);
  m_trigDefs = m_isMuon? TrigDefs::alsoDeactivateTEs : TrigDefs::Physics;
  //m_trigDefs = TrigDefs::Physics;

  //get combined muons
  m_muonCollection = "MuidMuonCollection";
  m_checkrate      = 1000;
  m_etacut         = 2.5;
  m_tagetacut      = 2.5;
  m_muonMass       = m_isMuon? 105.66*CLHEP::MeV : 0.511*CLHEP::MeV;
  m_TagAndProbe    = true;
  m_useTrigger     = false;    
  m_runOnline      = true;

/* MEEEEH   m_RCAL = 1470.0E+3;
  m_ZCAL = 3800.0E+3;*/

  if(m_isJpsi){
    m_jpsiPDGmass = 3.097*CLHEP::GeV;
    m_jpsiwidth   = 2.*CLHEP::GeV;
    m_ptcut       = 4.0*CLHEP::GeV;
    m_tagptcut    = (m_isMuon? 4.0 : 5.0) * CLHEP::GeV;
    //m_plusMass    = 1.203*GeV;
    //m_plusMass    = 0.603*GeV;
    //m_minusMass   = 0.597*GeV;
    m_plusMass    = (m_isMuon? 0.303 : 2.903) * CLHEP::GeV;
    m_minusMass   = (m_isMuon? 0.597 : 2.097) * CLHEP::GeV;
  }
  else {
    m_jpsiPDGmass = 90.097*CLHEP::GeV;
    m_jpsiwidth   = 2.*CLHEP::GeV;
    m_ptcut       = 15.0*CLHEP::GeV;   
    m_tagptcut    = 24.0*CLHEP::GeV;   
    //m_plusMass    = 19.903*CLHEP::GeV; // 110 GeV
    m_plusMass    = 309.903*CLHEP::GeV; // 400 GeV
    //m_minusMass   = 20.097*CLHEP::GeV; // 70 GeV  
    m_minusMass   = 55.097*CLHEP::GeV; // 35 GeV 
  }
  
  ///temp

  if(m_toolSvc.retrieve().isFailure()) m_provider->msg(MSG::FATAL) << "Failed to retrieve service " << m_toolSvc << endmsg;
  else  m_provider->msg(MSG::INFO) << "Retrieved service " << m_toolSvc << endmsg;
  
  // get the beam condition services
  if(m_iBeamCondSvc.retrieve().isFailure())  m_provider->msg(MSG::ERROR) << " failed to retrieve BeamCondSvc " << endmsg;
  else  m_provider->msg(MSG::INFO) << " retrieved BeamCondSvc " << endmsg;
  
  /*m_iOnlineBeamCondSvc = 0;
  if ( m_provider->service( "InDetBeamSpotOnline", m_iOnlineBeamCondSvc ).isFailure() )  { 
    m_provider->msg(MSG::ERROR) << " failed to retrieve Online BeamCondSvc" << endmsg;
  }
  else {
    m_provider->msg(MSG::INFO) << " retrieved OnlineBeamCondSvc " << endmsg;
  }*/

  // get the TriggerDecisionTool

  if(m_tdt->retrieve().isFailure()) {
    m_provider->msg(MSG::ERROR) << "Unable to retrieve the TrigDecisionTool: Please check job options file" << endmsg;
    //return StatusCode::FAILURE;
  }

  if(m_isMuon){
    if(m_testChainKey.find("SiTrackFinder") != string::npos){
      if(m_testChainKey.find("eGammaA") != string::npos) m_testChainKey = "TrigL2SiTrackFinder_MuonA";
      if(m_testChainKey.find("eGammaB") != string::npos) m_testChainKey = "TrigL2SiTrackFinder_MuonB";
      if(m_testChainKey.find("eGammaC") != string::npos) m_testChainKey = "TrigL2SiTrackFinder_MuonC";
    }
    if(m_testChainKey.find("InDetTrig") != string::npos) {
      if(m_testChainKey.find("Electron") != string::npos) m_testChainKey = "InDetTrigParticleCreation_Muon_EFID";
    }
  }

#ifdef ManagedMonitorToolBase_Uses_API_201401 
  m_provider->addMonGroup( new ManagedMonitorToolBase::MonGroup( m_provider,
                               "HLT/IDJpsiMon/" + m_analysis->name() + "/" + m_testChainKey,
                               ManagedMonitorToolBase::run ) );
                               // ManagedMonitorToolBase::shift, ManagedMonitorToolBase::run ) );
#else
  m_provider->addMonGroup( new ManagedMonitorToolBase::MonGroup( m_provider,
                               "HLT/IDJpsiMon/" + m_analysis->name() + "/" + m_testChainKey,
                               ManagedMonitorToolBase::shift, ManagedMonitorToolBase::run ) );
#endif

  m_analysis->initialise();

  m_analysisJ = dynamic_cast<JpsiAnalysis*> (m_analysis);

  m_analysisJ->setChainName(m_refChainName);
  m_analysisJ->setPlusMass(m_plusMass);
  m_analysisJ->setMinusMass(m_minusMass);
      
  map<string, TH1F*>::const_iterator hitr = m_analysisJ->THbegin();
  map<string, TH1F*>::const_iterator hend = m_analysisJ->THend();

  while ( hitr!=hend ) { 
    m_provider->addHistogram( hitr->second,"HLT/IDJpsiMon/" +  m_analysis->name() + "/" + m_testChainKey ); 
    hitr++; 
  }

  map<string, TProfile*>::const_iterator tphitr = m_analysisJ->TPbegin();
  map<string, TProfile*>::const_iterator tphend = m_analysisJ->TPend();

  while(tphitr != tphend) { 
    m_provider->addHistogram( tphitr->second,"HLT/IDJpsiMon/" +  m_analysis->name() + "/" + m_testChainKey ); 
    tphitr++; 
  }

  map<string, TH2F*>::const_iterator hitr2d = m_analysisJ->TH2begin();
  map<string, TH2F*>::const_iterator hend2d = m_analysisJ->TH2end();

  while(hitr2d != hend2d) { 
    m_provider->addHistogram(hitr2d->second,"HLT/IDJpsiMon/" +  m_analysis->name() + "/" + m_testChainKey); 
    hitr2d++; 
  }

  ///temp
  m_analysisJ->setSpeaker(m_provider);
  m_analysisJ->setTool(m_tdt);

}

/// finalise the analysis - take ratios for efficiencies etc
void AnalysisConfig_Jpsi::finalize() { 
  cout << ">>> trigger: "        << m_refChainName << std::endl;
  cout << ">>> before trigger: " << m_count_before << std::endl;
  cout << ">>> after trigger : " << m_count_after  << std::endl;
  m_analysis->finalise();
}

// ###########################################################################
 
float AnalysisConfig_Jpsi::deltaR(float eta1, float phi1, float eta2, float phi2) {
  float deta = eta1-eta2;
  float dphi = deltaPhi(phi1,phi2);
  return TMath::Sqrt(deta*deta+dphi*dphi);
}
 
// ###########################################################################

float AnalysisConfig_Jpsi::deltaPhi(float phi1, float phi2) {
  if(TMath::IsNaN(phi1)||TMath::IsNaN(phi2)) {
    cout << "> > > > > WTF!? deltaPhi(phi1,phi2) called with NaN" << endl;
  }
  return fixPhi(phi1-phi2);
}

// ###########################################################################
 
float AnalysisConfig_Jpsi::fixPhi(float phi) {
  while(phi >=  M_PI) phi -= 2.0*M_PI;
  while(phi <  -M_PI) phi += 2.0*M_PI;
  return phi;
}
 
// ###########################################################################

