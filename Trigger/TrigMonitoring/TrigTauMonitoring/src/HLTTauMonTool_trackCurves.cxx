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


StatusCode HLTTauMonTool::trackCurves(const std::string & trigItem){
    
    ATH_MSG_DEBUG ("HLTTauMonTool::doTrackCurves");
    setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/trackCurves");
    
    std::string trig_item_EF = "HLT_"+trigItem;
    std::string trig_item_L1(LowerChain( trig_item_EF ) );
    
    const xAOD::TauJetContainer* reco_cont = 0;
    if( evtStore()->retrieve(reco_cont, "TauJets").isFailure() ){
        ATH_MSG_WARNING("Failed to retrieve TauJets container. Exiting!");
        return StatusCode::FAILURE;
    }
    
    xAOD::TauJetContainer::const_iterator recoItr, reco_cont_end = reco_cont->end();
    for(recoItr=reco_cont->begin(); recoItr!=reco_cont_end; ++recoItr) {
        if(!Selection(*recoItr)) { continue; }
        if(!TruthMatched(*recoItr)) { continue; }
        
        bool presmatch = false; //true if reco+truth is matched to preselection
        const xAOD::TauJet* matchedTau = 0 ;
        
        if (not getTDT()->isPassed(trig_item_EF,m_HLTTriggerCondition)) {
            continue; //presmatch will never be true otherwise anyway
        }   

        Trig::FeatureContainer f = ( getTDT()->features(trig_item_EF,m_HLTTriggerCondition) );

        for(const auto& comb: f.getCombinations()) {
            
            const std::vector< Trig::Feature<xAOD::TauJetContainer> >  vec_preseltau = comb.get<xAOD::TauJetContainer>("TrigTauRecPreselection", m_HLTTriggerCondition);
            
            for(const auto& preselTau : vec_preseltau) {
                const auto *preselTauJetVec = preselTau.cptr();
                if(not preselTauJetVec) {
                    continue;
                }
		for (unsigned int tauit = 0; tauit< (*preselTauJetVec).size();++tauit){
               	    const auto *preselTauJet = (*preselTauJetVec)[tauit];
                    double dR_recopres = deltaR( preselTauJet->eta(), (*recoItr)->eta(), preselTauJet->phi(), preselTauJet->phi() );
               	    if  (dR_recopres < 0.2) { 
			presmatch = true;
			matchedTau = preselTauJet;
                        break;
			}
		    }
           	 }
	}
        
	// If reco is not matched to preselection tau,skip this tau and skip the filling of all of the next plots
        if (!presmatch) { // skip this reco tau and all the rest if it is not matched to preselection
            continue;
        }

        int EFnTrack(-1);
        #ifndef XAODTAU_VERSIONS_TAUJET_V3_H 
        EFnTrack = matchedTau->nTracks();
        #else
        matchedTau->detail(xAOD::TauJetParameters::nChargedTracks, EFnTrack);
        #endif
    
	if(EFnTrack < 1) { 
	    continue; 
	}

	hist2("hreco_vs_pres_coreTracks")->Fill((*recoItr)->nTracks(), EFnTrack);
        #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
	hist2("hreco_vs_pres_isoTracks")->Fill( (*recoItr)->nWideTracks(), 
						(*recoItr)->nWideTracks() );
        #else
        hist2("hreco_vs_pres_isoTracks")->Fill( (*recoItr)->nTracksIsolation(),
                                                (*recoItr)->nTracksIsolation() );
	#endif
        for (unsigned int recoTrack = 0; recoTrack < (*recoItr)->nTracks(); ++recoTrack){
            const xAOD::TrackParticle* recotau_trk = 0;
            #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
            recotau_trk = (*recoItr)->track(recoTrack);
            #else
            recotau_trk = (*recoItr)->track(recoTrack)->track();
            #endif
            float recotau_trk_pt = recotau_trk->pt()/GeV;
            float recotau_trk_eta = recotau_trk->eta();
            float recotau_trk_phi = recotau_trk->phi();
            float recotau_trk_d0 = recotau_trk->d0();
            float recotau_trk_z0 = recotau_trk->z0();

  	    //Those plots are used in the efficiency plots, as denominator
            hist("hrecotau_trk_pt")->Fill(recotau_trk_pt);
            hist("hrecotau_trk_eta")->Fill(recotau_trk_eta);
            hist("hrecotau_trk_phi")->Fill(recotau_trk_phi);
            hist("hrecotau_trk_d0")->Fill(recotau_trk_d0);
            hist("hrecotau_trk_z0")->Fill(recotau_trk_z0);
            hist2("hrecotau_trk_etaphi")->Fill(recotau_trk_eta, recotau_trk_phi);
            
            //Now find the closest track
            double dRtracksmin = 9999;
            const xAOD::TrackParticle* pstau_trk_clos = 0;        

            for (unsigned int i = 0; i < matchedTau->nTracks(); ++i){
                const xAOD::TrackParticle* pstau_trk = 0;
                #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
                pstau_trk = matchedTau->track(i);
                #else
                pstau_trk = matchedTau->track(i)->track();
                #endif
                double dRtracks = deltaR (pstau_trk->eta(), recotau_trk_eta, pstau_trk->phi(), recotau_trk_phi);
                if (dRtracks < dRtracksmin) {
                    dRtracksmin = dRtracks;
                    pstau_trk_clos = pstau_trk;
                }
            }
            
            double dRtracks_min = deltaR (pstau_trk_clos->eta(), recotau_trk_eta, pstau_trk_clos->phi(), recotau_trk_phi); //equal to dRtracksmin
	    bool tracksMatched = false;
	    if ( dRtracks_min< 0.1 ) {tracksMatched = true;}
	
	    if (tracksMatched == false) {
 		 //Fill TProfile
		std::cout<<" Track curves: filling TProfile, starting pt, line 185 "<<std::endl;
	    	profile("TProfPresVsRecoPtEfficiency")->Fill(recotau_trk_pt, 0); 
            	profile("TProfPresVsRecoEtaEfficiency")->Fill(recotau_trk_eta, 0);
            	profile("TProfPresVsRecoPhiEfficiency")->Fill(recotau_trk_phi, 0);
            	profile("TProfPresVsRecod0Efficiency")->Fill(recotau_trk_d0, 0);
            	profile("TProfPresVsRecoz0Efficiency")->Fill(recotau_trk_z0, 0);
		std::cout<<" Track curves: finishing Profile, line 192 "<<std::endl;
		continue; // skip the filling of the next plots if the tracks are not matched
		}

	    if (tracksMatched == true){
	    // All the next plots are not filled if the tracks are not matched
            	hist("hrecotauNum_trk_pt")->Fill(recotau_trk_pt); //Num plots are taken in the efficency plots, as numenator
            	hist("hrecotauNum_trk_eta")->Fill(recotau_trk_eta);
            	hist("hrecotauNum_trk_phi")->Fill(recotau_trk_phi);
            	hist("hrecotauNum_trk_d0")->Fill(recotau_trk_d0);
            	hist("hrecotauNum_trk_z0")->Fill(recotau_trk_z0);
            	hist2("hrecotauNum_trk_etaphi")->Fill(recotau_trk_eta, recotau_trk_phi);
            
            	float pstau_trk_pt = pstau_trk_clos->pt()/GeV;
            	float pstau_trk_eta = pstau_trk_clos->eta();
            	float pstau_trk_phi = pstau_trk_clos->phi();
            	float pstau_trk_d0 = pstau_trk_clos->d0();
            	float pstau_trk_z0 = pstau_trk_clos->z0();
            
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


bool HLTTauMonTool::TruthMatched( const xAOD::TauJet * tau){
    // the function returns true if tau is matched to truth tau
    bool truthmatch = false;
    
    const xAOD::TruthParticleContainer* truth_cont = 0;
    
    if( evtStore()->retrieve(truth_cont, "TruthParticles" ).isFailure() ){
        ATH_MSG_WARNING("Failed to retrieve TruthParticle container. Exiting.");
        return StatusCode::FAILURE;
    }
    
    xAOD::TruthParticleContainer::const_iterator truthItr,truth_cont_end = truth_cont->end();
    for (truthItr=truth_cont->begin(); truthItr != truth_cont_end; ++truthItr){
        if (abs((*truthItr)->pdgId()) == 15  && (*truthItr)->status() == 2) {
            examineTruthTau(**truthItr);
            double pt  = (*truthItr)->auxdata<double>("pt_vis");
            double eta = (*truthItr)->auxdata<double>("eta_vis");
            double phi = (*truthItr)->auxdata<double>("phi_vis");
            //double m   = (*truthItr)->auxdata<double>("m_vis");
            bool lep = (*truthItr)->auxdata<bool>("IsLeptonicTau");
            int ntracks = (*truthItr)->auxdata<int>("nTracks");
            if(pt < m_effOffTauPtCut || lep || fabs(eta) > 2.47 ) continue;
            if(m_selection_nTrkMax>-1 && ntracks>m_selection_nTrkMax) continue;
            if(m_selection_nTrkMin>-1 && ntracks<m_selection_nTrkMin) continue;
            if(m_selection_ptMax>-1 && pt>m_selection_ptMax) continue;
            if(m_selection_ptMin>-1 && pt<m_selection_ptMin) continue;
            if(m_selection_absEtaMax>-1 && fabs(eta)>m_selection_absEtaMax) continue;
            if(m_selection_absEtaMin>-1 && fabs(eta)<m_selection_absEtaMin) continue;
            if(m_selection_absPhiMax>-1 && fabs(phi)>m_selection_absPhiMax) continue;
   	        if(m_selection_absPhiMin>-1 && fabs(phi)<m_selection_absPhiMin) continue;
            float dR = deltaR(eta, tau->eta(), phi, tau->phi());
            if ( dR<0.2 ) truthmatch = true;
        }}
    return truthmatch;
}






