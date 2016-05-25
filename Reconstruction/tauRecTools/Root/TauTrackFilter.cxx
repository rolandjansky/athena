/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauTrackFilter.cxx
// package:     Reconstruction/tauRec
// authors:     Robert Clarke, Blake Burghgrave
// date:        2014-01-04
//
//
//-----------------------------------------------------------------------------

//#include <GaudiKernel/IToolSvc.h>
//#include <GaudiKernel/ListItem.h>

#include "FourMomUtils/P4Helpers.h"
//#include "FourMom/P4EEtaPhiM.h"

#include "tauRecTools/TauEventData.h"

#include "tauRecTools/TauTrackFilter.h"
#include "tauRecTools/TauTrackFilterUtils.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTau/TauxAODHelpers.h"

#include "TLorentzVector.h"

void TrackFilterAlg(TLorentzVector tau,
                    std::vector<TLorentzVector>& inputtracks20,
                    std::vector<int>& inputtracks20charge,
                    std::vector<TLorentzVector>& inputtracks40,
                    std::vector<int>& inputtracks40charge,
                    std::vector<TLorentzVector>& outputtracksgood,
                    std::vector<int>& outputtracksgoodcharge,
                    std::vector<TLorentzVector>& outputtracksbad,
                    std::vector<int>& outputtracksbadcharge,
                    int& nProng,
                    int& flag);

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TauTrackFilter::TauTrackFilter(const std::string &name ) :
  TauRecToolBase(name),
  m_nProng(0),
  m_flag(0)
{
    declareProperty("ConfigPath", m_configPath);
    declareProperty("TrackContainerName", m_trackContainerName = "InDetTrackParticles");
    declareProperty("TauTrackContainerName", m_tauTrackConName = "TauTracks");
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------

TauTrackFilter::~TauTrackFilter() {
}


//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------

StatusCode TauTrackFilter::initialize() {
    ATH_MSG_VERBOSE("TauTrackFilter Initialising");

    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------

StatusCode TauTrackFilter::finalize() {
    ATH_MSG_VERBOSE("TauTrackFilter Finalizing");

    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauTrackFilter::execute(xAOD::TauJet& pTau) {
    ATH_MSG_VERBOSE("TauTrackFilter Executing");

    StatusCode sc;

    const xAOD::TrackParticleContainer *trackContainer;

    //TODO: trigger uses getObject
    sc = evtStore()->retrieve(trackContainer, m_trackContainerName);
    if (sc.isFailure() || !trackContainer) {
        ATH_MSG_DEBUG(" No track container found in TDS !!");
        return StatusCode::SUCCESS;
    }

    TLorentzVector tau;
    tau.SetPtEtaPhiE(pTau.pt()/1000, //GeV
                     pTau.eta(),
                     pTau.phi(),
                     pTau.e()/1000); //GeV

    std::vector<TLorentzVector> inputtracks20;
    std::vector<TLorentzVector> inputtracks40;
    std::vector<int> inputtracks20charge;
    std::vector<int> inputtracks40charge;
    std::vector<TLorentzVector> outputtracksgood;
    std::vector<TLorentzVector> outputtracksbad;
    std::vector<int> outputtracksgoodcharge;
    std::vector<int> outputtracksbadcharge;
    int nProng = 0;
    int flag = 0;

    std::vector<unsigned int> inputtracks20index;
    std::vector<unsigned int> inputtracks40index;

    m_TrkPass.clear();

    for(unsigned int j=0; j<pTau.nTracks(); j++ ) {
      const xAOD::TrackParticle *TauJetTrack = pTau.track(j)->track();
        TLorentzVector inputTrack;
        inputTrack.SetPtEtaPhiE(TauJetTrack->pt()/1000, //GeV
                                TauJetTrack->eta(),
                                TauJetTrack->phi(),
                                TauJetTrack->e()/1000); //GeV
        float dR = tau.DeltaR(inputTrack);
        if (dR < 0.2) {
            inputtracks20.push_back(inputTrack);
            inputtracks20charge.push_back(TauJetTrack->charge());
            inputtracks20index.push_back(j);
        }
        else if (dR < 0.4) {
            inputtracks40.push_back(inputTrack);
            inputtracks40charge.push_back(TauJetTrack->charge());
            inputtracks40index.push_back(j);
        }

        // Add default status to track pass/fail member.
        m_TrkPass.push_back(false);
    }

    // Run the main algorithm
    TrackFilterAlg(tau,
                   inputtracks20,
                   inputtracks20charge,
                   inputtracks40,
                   inputtracks40charge,
                   outputtracksgood,
                   outputtracksgoodcharge,
                   outputtracksbad,
                   outputtracksbadcharge,
                   nProng,
                   flag);

    // Store results
    for (unsigned int j=0; j<outputtracksgood.size(); j++ ) {
        for (unsigned int k=0; k<inputtracks20.size(); k++ ) {
            if (outputtracksgood.at(j) == inputtracks20.at(k)) {
                m_TrkPass.at(inputtracks20index.at(k)) = true;
            }
        }
        for (unsigned int k=0; k<inputtracks40.size(); k++ ) {
            if (outputtracksgood.at(j) == inputtracks40.at(k)) {
                m_TrkPass.at(inputtracks40index.at(k)) = true;
            }
        }
    }
    m_nProng = nProng;
    m_flag = flag;

    // Set values in EDM
    xAOD::TauTrackContainer* tauTracks = 0;
    ATH_CHECK(evtStore()->retrieve(tauTracks, m_tauTrackConName));
    for (unsigned int numTrack=0; numTrack<m_TrkPass.size(); numTrack++) {
      xAOD::TauTrack* tauTrk = xAOD::TauHelpers::tauTrackNonConst(&pTau, tauTracks, numTrack); //pTau.trackNonConst(numTrack);
      tauTrk->setFlag(xAOD::TauJetParameters::failTrackFilter, !m_TrkPass.at(numTrack));
    }
    pTau.setTrackFilterProngs(m_nProng);
    pTau.setTrackFilterQuality(m_flag);

    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Main algorithm
//-----------------------------------------------------------------------------
void TrackFilterAlg(TLorentzVector tau,
                    std::vector<TLorentzVector>& inputtracks20,
                    std::vector<int>& inputtracks20charge,
                    std::vector<TLorentzVector>& inputtracks40,
                    std::vector<int>& inputtracks40charge,
                    std::vector<TLorentzVector>& outputtracksgood,
                    std::vector<int>& outputtracksgoodcharge,
                    std::vector<TLorentzVector>& outputtracksbad,
                    std::vector<int>& outputtracksbadcharge,
                    int& nProng,
                    int& flag) {

   std::vector<TauTrackFilterUtils::TrackInfo> unsorted,tracks,SScombination;
   TauTrackFilterUtils::TrackInfo track;
   unsigned int tracknum = inputtracks20.size();
   unsigned int widetracknum = inputtracks40.size();
   for(unsigned int i=0;i<tracknum;i++){
      track.p4 = (inputtracks20)[i];
      track.charge = (inputtracks20charge)[i];
      unsorted.push_back(track);
   }
   while (unsorted.size() > 0){
      float trackP = unsorted[0].p4.P();
      int index = 0;
      for(unsigned int i=1;i<unsorted.size();i++){
         if(unsorted[i].p4.P() > trackP){
            index = i;
            trackP = unsorted[i].p4.P();
         }
      }
      tracks.push_back(unsorted[index]);
      tracks[tracks.size()-1].index = tracks.size()-1;
      unsorted.erase(unsorted.begin()+index);            
   }
//Step 2: Test 3 Prong Hypothesis
//Step 2a: Arrange combinations of tracks for testing
   
   bool test3prong = true, test2prong = true, test1prong = true;
   for(unsigned int i=0;i<widetracknum;i++){
   	outputtracksbad.push_back((inputtracks40)[i]);
   	outputtracksbadcharge.push_back((inputtracks40charge)[i]);
   }
   if(tracknum > 4){ //Anything with more than 4 tracks is a fake.
      flag = 0;
      test3prong = false;
      test2prong = false;
      test1prong = false;
   }
   if(tracknum < 3) test3prong = false; //Don't test 3 prong if fewer than 3 tracks
   if(tracknum < 2) test2prong = false; //Don't test 2 prong if fewer than 2 tracks
   if(tracknum < 1){
      flag = 0;
      test1prong = false; //Don't test 1 prong if no tracks within dR < 0.2 of tau
   }
   if(test3prong){
		//Test 3 Highest pT Tracks
      bool isSS = false;
      std::vector<TauTrackFilterUtils::TrackInfo> combination;
      int charge = 0; TLorentzVector threetrack;
      for(unsigned int i=0;i<3;i++){
      	combination.push_back(tracks[i]); //Only Care about 3 Highest pT Tracks
         charge+=tracks[i].charge;
         threetrack+=tracks[i].p4;
      }
      if((tracknum == 3) && (abs(charge)!=1)) isSS = true; //Reject all same-sign combinations
//Step 2b: Check kinematics of track combinations against shrinking cones and mass boundaries
//         for(unsigned int i=0;i<combinations.size();i++){
		bool goodcombo = false;
		if(tracknum == 4){
      	char eqn[] = "pol2";
   		float a[] = {1.51673, -0.000150534, 2.64226e-06};
			float mass99 = TauTrackFilterUtils::Compute1dim(tau.P(),a,3,eqn);
			if((threetrack.M() < mass99)&&(TauTrackFilterUtils::pass3prong(combination,tau))){
				goodcombo=true;
				flag = 2;
			}
         else flag = 0;
         test1prong=false;
         test2prong=false;
		}
      else if(TauTrackFilterUtils::pass3prong(combination, tau)){
         goodcombo=true;
         flag = 1;
      }
      if(goodcombo){  //A Combination is found which passes 3prong hypothesis
         for(unsigned int i=0;i<combination.size();i++){
            if (isSS) SScombination.push_back(combination[i]);
            outputtracksgood.push_back(combination[i].p4);
            outputtracksgoodcharge.push_back(combination[i].charge);
         }
         if(isSS) flag = 0;
         else flag = 1;
         nProng = 3;
         test1prong = false;
         test2prong = false;
         if(!test1prong){ //Fill Bad Track in the Case of 4 trk taus
            if(tracknum == 4){
            	outputtracksbad.push_back(tracks[3].p4);
            	outputtracksbadcharge.push_back(tracks[3].charge);
            }
         }
      }
   }//End 3 Prong Test Conditional
   if (test2prong){
      std::vector<TauTrackFilterUtils::TrackInfo> pair;
      for(unsigned int i=0;i<2;i++) pair.push_back(tracks[i]);
      if(TauTrackFilterUtils::pass2prong(pair,tau)){
      	nProng = 2;
         for(unsigned int i=0;i<pair.size();i++){ //Fill Good Tracks
            outputtracksgood.push_back(pair[i].p4);
            outputtracksgoodcharge.push_back(pair[i].charge);
         }
         test1prong = false;
         if(tracknum == 3){
            flag = 2;
         	outputtracksbad.push_back(tracks[2].p4); //Fill Bad Track in Case of 3 trk Taus
         	outputtracksbadcharge.push_back(tracks[2].charge);
         }
         else flag = 1; //Good 2 Prong if only 2 trks
      }
   }//End 2 Prong Test Conditional
//Step 4: Check tracks that don't pass 2 prong hypothesis against 1 prong hypothesis
     if (test1prong){
         char eqn[] = "([0]*exp([1]*x))*pol6(2)+[9]";
         float a[10];
         a[0] = 0.079586;
         a[1] = -0.0289929;
         a[2] = 7.06684;
         a[3] = -0.158835;
         a[4] = 0.000607181;
         a[5] = 6.8445e-05;
         a[6] = -6.79205e-07;
         a[7] = 2.13158e-09;
         a[8] = -5.11643e-13;
         a[9] = 0.030376;
         float ratio10 = TauTrackFilterUtils::Compute1dim(tau.P(),a,10,eqn);
         bool goodcase = false;
         if(tracknum == 1) goodcase = true;
         if(tracknum == 2){
            if(tracks[1].p4.Pt()/tracks[0].p4.Pt() < ratio10) goodcase = true; //Test 2trk taus most likely to actually be 1pngs
         }
         if((TauTrackFilterUtils::pass1prong(tracks[0].p4,tau))&&(goodcase)){ //A track is found which passes 1prong hypothesis
            outputtracksgood.push_back(tracks[0].p4);
            outputtracksgoodcharge.push_back(tracks[0].charge);
            nProng = 1;
            if (tracknum == 2){
               flag = 2;
            	outputtracksbad.push_back(tracks[1].p4); //Fill Bad Track in Case of 3 trk Taus
            	outputtracksbadcharge.push_back(tracks[1].charge);
            }
            else flag = 1;
         }
         else flag = 0; //Fake Tau
     }//End 1 Prong Test Conditional

     return;
}

