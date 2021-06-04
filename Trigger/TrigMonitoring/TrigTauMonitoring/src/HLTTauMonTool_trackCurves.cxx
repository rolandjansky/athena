/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
//  @file HLTTauMonTool_trackCurves.cxx
//  created by Milena Bajic <milena.bajic@cern.ch>
#include "TProfile.h"
#include "HLTTauMonTool.h"
#include "AthenaKernel/Units.h"
using namespace std;
using Athena::Units::GeV;
StatusCode HLTTauMonTool::trackCurves(const std::string & trigItem, const std::string & goodTauRefType){
   
    ATH_MSG_DEBUG ("HLTTauMonTool::doTrackCurves");

    std::string trigItemShort=trigItem;
    if(trigItem.find("tau25")!=string::npos && trigItem.find("L1TAU")!=string::npos){
      size_t posit=trigItem.rfind("_");
      if(posit<31)trigItemShort=trigItem.substr(0,posit);
    }

    setCurrentMonGroup("HLT/TauMon/Expert/"+trigItemShort+"/trackCurves");
   
    std::string trig_item_EF = "HLT_"+trigItem;
    std::string trig_item_L1(LowerChain( trig_item_EF ) );
          std::vector<const xAOD::TauJet *> taus_here;
                if (goodTauRefType == "RNN") {
                        taus_here = m_taus_RNN;
                } else {
                        taus_here = m_taus_BDT;
                }
    // Loop over selected offline taus  
    for(unsigned int t=0;t<taus_here.size();t++){
 
        if(m_truth){
            // apply truth matching if doTruth is true
            bool matched(false);
            for(unsigned int truth=0;truth<m_true_taus.size();truth++){
                if(taus_here.at(t)->p4().DeltaR(m_true_taus.at(truth))<0.2) matched = true;
            }
            if(!matched) continue;
        }
                
        bool presmatch = false; //true if reco+truth is matched to preselection
        const xAOD::TauJet *offlineTau = taus_here.at(t);
        const xAOD::TauJet* matchedTau = 0 ;
       
        if (not getTDT()->isPassed(trig_item_EF,m_HLTTriggerCondition)) {
            continue; //presmatch will never be true otherwise anyway
        }   
        Trig::FeatureContainer f = ( getTDT()->features(trig_item_EF,m_HLTTriggerCondition) );
        for(const auto& comb: f.getCombinations()) {
            // get HLT perselection taus:        
            const std::vector< Trig::Feature<xAOD::TauJetContainer> >  vec_preseltau = comb.get<xAOD::TauJetContainer>("TrigTauRecPreselection", m_HLTTriggerCondition);
           
            for(const auto& preselTau : vec_preseltau) {
                const auto *preselTauJetVec = preselTau.cptr();
                if(not preselTauJetVec) {
                    continue;
                }
                for (unsigned int tauit = 0; tauit< (*preselTauJetVec).size();++tauit){
                    const auto *preselTauJet = (*preselTauJetVec)[tauit];
                    double dR_recopres = deltaR( preselTauJet->eta(), offlineTau->eta(), preselTauJet->phi(), offlineTau->phi() );
                    if  (dR_recopres < 0.2) {
                        presmatch = true;
                        matchedTau = preselTauJet;
                        break;
                        }
                    }
                 }
        }
       
        // If reco is not matched to preselection tau, skip this tau and skip the filling of all of the next plots
        if (!presmatch) { // skip this reco tau and all the rest if it is not matched to preselection
            continue;
        }
        // TODO: we should also loop and fill plots for precision taus
        int EFnTrack(-1),EFnWideTrack(-1);
        int OffnTrack(-1),OffnWideTrack(-1);
        #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
        EFnTrack = matchedTau->nTracks();
        EFnWideTrack = matchedTau->nWideTracks();
        OffnTrack = offlineTau->nTracks();
        OffnWideTrack = offlineTau->nWideTracks();
        #else
        matchedTau->detail(xAOD::TauJetParameters::nChargedTracks, EFnTrack);
        offlineTau->detail(xAOD::TauJetParameters::nChargedTracks, OffnTrack);
        matchedTau->detail(xAOD::TauJetParameters::nIsolatedTracks, EFnWideTrack);
        offlineTau->detail(xAOD::TauJetParameters::nIsolatedTracks, OffnWideTrack);
        #endif
   
        hist2("hreco_vs_pres_coreTracks")->Fill(OffnTrack, EFnTrack);
        hist2("hreco_vs_pres_isoTracks")->Fill(OffnWideTrack,EFnWideTrack);
        // loop over the core (dr<0.2) offline tau tracks, maybe for now good enough, we may add wide tracks too
        if(OffnTrack==0) continue;
        bool validLink(true);
        std::vector<ElementLink< xAOD::TrackParticleContainer> > trackLinks;
        #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
        trackLinks = offlineTau->trackLinks();
        #else
        trackLinks = offlineTau->track(0)->trackLinks();
        #endif 
        for(const auto& trklink : trackLinks) {
            if(!trklink.isValid()){ ATH_MSG_WARNING("Invalid tau track link"); validLink = false;}
        }
        if(!validLink) continue;
        if(m_isData) continue; // track links not working in data!!
        else {
          #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
          trackLinks = offlineTau->trackLinks();
          #else
          if(EFnTrack>0) trackLinks = offlineTau->track(0)->trackLinks();
          #endif
          for(const auto& trklink : trackLinks) {
            if(!trklink.isValid()){ ATH_MSG_WARNING("Invalid tau track link"); validLink = false;}
          }
          if(!validLink) continue;
        }
        for (int recoTrack = 0; recoTrack < OffnTrack; ++recoTrack){
            const xAOD::TrackParticle* recotau_trk = 0;
            float recotau_trk_pt(-99.);
            float recotau_trk_eta(-99.);
            float recotau_trk_phi(-99.);
            float recotau_trk_d0(-99.);
            float recotau_trk_z0(-99.);
            const xAOD::TrackParticle* pstau_trk_clos = 0;
            float pstau_trk_pt(-99.);
            float pstau_trk_eta(-99.);
            float pstau_trk_phi(-99.);
            float pstau_trk_d0(-99.);
            float pstau_trk_z0(-99.);
            #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
            recotau_trk = offlineTau->track(recoTrack);
            #else
            recotau_trk = offlineTau->track(recoTrack)->track();
            #endif
            if(recotau_trk){
               recotau_trk_pt = recotau_trk->pt()/GeV;
               recotau_trk_eta = recotau_trk->eta();
               recotau_trk_phi = recotau_trk->phi();
               recotau_trk_d0 = recotau_trk->d0();
               recotau_trk_z0 = recotau_trk->z0();
            }
            //Now find the closest track
            double dRtracksmin = 9999; 
            int trackIndex(-1);
            for (unsigned int i = 0; i < matchedTau->nTracks(); ++i){
                pstau_trk_eta = matchedTau->track(i)->eta();
                pstau_trk_phi = matchedTau->track(i)->phi();
                pstau_trk_pt  = matchedTau->track(i)->pt(); 
                double dRtracks = deltaR (pstau_trk_eta, recotau_trk_eta, pstau_trk_phi, recotau_trk_phi);
                if (dRtracks < dRtracksmin) {
                   dRtracksmin = dRtracks;
                   trackIndex = i;
                } 
            } 
            bool tracksMatched = false; 
            if ( dRtracksmin  < 0.1 ) {tracksMatched = true;}
            if (tracksMatched == false) {
                 //Fill TProfile
                
                profile("TProfPresVsRecoPtEfficiency")->Fill(recotau_trk_pt, 0);
                profile("TProfPresVsRecoEtaEfficiency")->Fill(recotau_trk_eta, 0);
                profile("TProfPresVsRecoPhiEfficiency")->Fill(recotau_trk_phi, 0);
                profile("TProfPresVsRecod0Efficiency")->Fill(recotau_trk_d0, 0);
                profile("TProfPresVsRecoz0Efficiency")->Fill(recotau_trk_z0, 0);
               
                continue; // skip the filling of the next plots if the tracks are not matched
                }
            else {
                // All the next plots are not filled if the tracks are not matched
           
                #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
                pstau_trk_clos = matchedTau->track(trackIndex);
                #else 
                bool checklink = true;
                std::vector<ElementLink< xAOD::TrackParticleContainer> > onlinetrackLinks;
                onlinetrackLinks = matchedTau->track(trackIndex)->trackLinks(); 
                for(const auto& onlinetrklink : onlinetrackLinks) {
                   if(!onlinetrklink.isValid()){ checklink=false;}
                }       
                if (!checklink){
                   continue;
                }
                pstau_trk_clos = matchedTau->track(trackIndex)->track();
                #endif
                if(pstau_trk_clos){
                  pstau_trk_d0 = pstau_trk_clos->d0();
                  pstau_trk_z0 = pstau_trk_clos->z0();
                }
                // TODO: these 6 plots below are biased because they are filled only for the online tracks matching an offline track.
                // they should be fill for all the online tracks            
                hist("hpstau_trk_pt")->Fill(pstau_trk_pt);
                hist("hpstau_trk_eta")->Fill(pstau_trk_eta);
                hist("hpstau_trk_phi")->Fill(pstau_trk_phi);
                hist("hpstau_trk_d0")->Fill(pstau_trk_d0);
                hist("hpstau_trk_z0")->Fill(pstau_trk_z0);
                hist2("hpstau_trk_etaphi")->Fill(pstau_trk_eta, pstau_trk_phi);
           
                hist("hpstau_trkres_pt")->Fill((pstau_trk_pt - recotau_trk_pt)/recotau_trk_pt);  //this is not filled if the tracks are not matched
                hist("hpstau_trkres_eta")->Fill((pstau_trk_eta - recotau_trk_eta)/recotau_trk_eta);
                hist("hpstau_trkres_phi")->Fill((pstau_trk_phi - recotau_trk_phi)/recotau_trk_phi);
                hist("hpstau_trkres_d0")->Fill((pstau_trk_d0 - recotau_trk_d0)/recotau_trk_d0);
                hist("hpstau_trkres_z0")->Fill((pstau_trk_z0 - recotau_trk_z0)/recotau_trk_z0);
                 //2d plots of Reco vs preselection tracks
                hist2("hpstautrk_vs_recotrk_pt")->Fill(pstau_trk_pt,recotau_trk_pt);
                hist2("hpstautrk_vs_recotrk_eta")->Fill(pstau_trk_eta,recotau_trk_eta);
                hist2("hpstautrk_vs_recotrk_phi")->Fill(pstau_trk_phi,recotau_trk_phi);
                hist2("hpstautrk_vs_recotrk_d0")->Fill(pstau_trk_d0,recotau_trk_d0);
                hist2("hpstautrk_vs_recotrk_z0")->Fill(pstau_trk_z0,recotau_trk_z0);
                profile("TProfPresVsRecoPtEfficiency")->Fill(recotau_trk_pt, 1);
                profile("TProfPresVsRecoEtaEfficiency")->Fill(recotau_trk_eta, 1);
                profile("TProfPresVsRecoPhiEfficiency")->Fill(recotau_trk_phi, 1);
                profile("TProfPresVsRecod0Efficiency")->Fill(recotau_trk_d0, 1);
                profile("TProfPresVsRecoz0Efficiency")->Fill(recotau_trk_z0, 1);
            }
        }
    } //end of the reco loop
   
    return StatusCode::SUCCESS;
}
