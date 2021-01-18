#ifndef INDET__PERF__NTUPLE_TRUTHTORECO__H
#define INDET__PERF__NTUPLE_TRUTHTORECO__H

#include "InDetPerfNtuple.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <cmath> // std::isnan()

class InDetPerfNtuple_TruthToReco: public InDetPerfNtuple {
public: 
    InDetPerfNtuple_TruthToReco(InDetPlotBase* pParent, const std::string& dirName, const std::string & treeName); 
    
    void fillTree(); 
    void fillTrack(const xAOD::TrackParticle& track, const int truthMatchRanking = -1); 
    void fillTruth(const xAOD::TruthParticle& truth);

    SG::AuxElement::Accessor<bool> m_acc_passedTruthSelection{"passedTruthSelection"};
	SG::AuxElement::Accessor<bool> m_acc_passedTrackSelection{"passedTrackSelection"};
    SG::AuxElement::Accessor<bool> m_acc_selectedByPileupSwitch{"selectedByPileupSwitch"};

private:
    const float m_undefinedValue{-9999};
    
    //Truth variables
    InDetPerfNtupleBranch<int>   m_truth_selectedByPileupSwitch; 
    InDetPerfNtupleBranch<int>   m_truth_pdgId; 
    InDetPerfNtupleBranch<int>   m_truth_barcode;
    InDetPerfNtupleBranch<float> m_truth_charge;
    InDetPerfNtupleBranch<float> m_truth_eta;
    InDetPerfNtupleBranch<float> m_truth_pt; 
    InDetPerfNtupleBranch<float> m_truth_d0; 
    InDetPerfNtupleBranch<float> m_truth_z0; 
    InDetPerfNtupleBranch<float> m_truth_phi; 
    InDetPerfNtupleBranch<float> m_truth_theta; 
    InDetPerfNtupleBranch<float> m_truth_qOverP; 
    InDetPerfNtupleBranch<float> m_truth_qOverPt;     
    InDetPerfNtupleBranch<float> m_truth_z0sin; 

    //Track variables
    InDetPerfNtupleBranch<int>   m_track_truthMatchRanking;
    InDetPerfNtupleBranch<float> m_track_truthMatchProb;
    InDetPerfNtupleBranch<float> m_track_charge;
    InDetPerfNtupleBranch<float> m_track_eta;
    InDetPerfNtupleBranch<float> m_track_pt; 
    InDetPerfNtupleBranch<float> m_track_d0; 
    InDetPerfNtupleBranch<float> m_track_z0; 
    InDetPerfNtupleBranch<float> m_track_phi; 
    InDetPerfNtupleBranch<float> m_track_theta; 
    InDetPerfNtupleBranch<float> m_track_qOverP; 
    InDetPerfNtupleBranch<float> m_track_qOverPt; 
    InDetPerfNtupleBranch<float> m_track_z0sin; 

    //Track fit errors
    InDetPerfNtupleBranch<float> m_trackErr_pt; 
    InDetPerfNtupleBranch<float> m_trackErr_d0; 
    InDetPerfNtupleBranch<float> m_trackErr_z0; 
    InDetPerfNtupleBranch<float> m_trackErr_phi; 
    InDetPerfNtupleBranch<float> m_trackErr_theta; 
    InDetPerfNtupleBranch<float> m_trackErr_qOverP; 
    InDetPerfNtupleBranch<float> m_trackErr_qOverPt; 
    InDetPerfNtupleBranch<float> m_trackErr_z0sin; 

    //More track variables
    InDetPerfNtupleBranch<float> m_track_chiSquared;
    InDetPerfNtupleBranch<float> m_track_nDoF;
    
    //Flags
    InDetPerfNtupleBranch<int> m_hasTruth; 
    InDetPerfNtupleBranch<int> m_hasTrack; 
    InDetPerfNtupleBranch<int> m_passedTruthSelection;
    InDetPerfNtupleBranch<int> m_passedTrackSelection;
};

#endif // INDET__PERF__NTUPLE_TRUTHTORECO__H