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
    
    void fillTrackSummaryInfo(const xAOD::TrackParticle& track);

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

    //Track summary variables
    InDetPerfNtupleBranch<int> m_numberOfContribPixelLayers;
    InDetPerfNtupleBranch<int> m_numberOfBLayerHits;
    InDetPerfNtupleBranch<int> m_numberOfBLayerOutliers;
    InDetPerfNtupleBranch<int> m_numberOfBLayerSharedHits;
    InDetPerfNtupleBranch<int> m_numberOfBLayerSplitHits;
    InDetPerfNtupleBranch<int> m_expectBLayerHit;
    InDetPerfNtupleBranch<int> m_expectInnermostPixelLayerHit;
    InDetPerfNtupleBranch<int> m_numberOfInnermostPixelLayerHits;
    InDetPerfNtupleBranch<int> m_numberOfInnermostPixelLayerOutliers;
    InDetPerfNtupleBranch<int> m_numberOfInnermostPixelLayerSharedHits;
    InDetPerfNtupleBranch<int> m_numberOfInnermostPixelLayerSplitHits;
    InDetPerfNtupleBranch<int> m_expectNextToInnermostPixelLayerHit;
    InDetPerfNtupleBranch<int> m_numberOfNextToInnermostPixelLayerHits;
    InDetPerfNtupleBranch<int> m_numberOfNextToInnermostPixelLayerOutliers;
    InDetPerfNtupleBranch<int> m_numberOfNextToInnermostPixelLayerSharedHits;
    InDetPerfNtupleBranch<int> m_numberOfNextToInnermostPixelLayerSplitHits;
    InDetPerfNtupleBranch<int> m_numberOfDBMHits;
    InDetPerfNtupleBranch<int> m_numberOfPixelHits;
    InDetPerfNtupleBranch<int> m_numberOfPixelOutliers;
    InDetPerfNtupleBranch<int> m_numberOfPixelHoles;
    InDetPerfNtupleBranch<int> m_numberOfPixelSharedHits;
    InDetPerfNtupleBranch<int> m_numberOfPixelSplitHits;
    InDetPerfNtupleBranch<int> m_numberOfGangedPixels;
    InDetPerfNtupleBranch<int> m_numberOfGangedFlaggedFakes;
    InDetPerfNtupleBranch<int> m_numberOfPixelDeadSensors;
    InDetPerfNtupleBranch<int> m_numberOfPixelSpoiltHits;
    InDetPerfNtupleBranch<int> m_numberOfSCTHits;
    InDetPerfNtupleBranch<int> m_numberOfSCTOutliers;
    InDetPerfNtupleBranch<int> m_numberOfSCTHoles;
    InDetPerfNtupleBranch<int> m_numberOfSCTDoubleHoles;
    InDetPerfNtupleBranch<int> m_numberOfSCTSharedHits;
    InDetPerfNtupleBranch<int> m_numberOfSCTDeadSensors;
    InDetPerfNtupleBranch<int> m_numberOfSCTSpoiltHits;
    InDetPerfNtupleBranch<int> m_numberOfTRTHits;
    InDetPerfNtupleBranch<int> m_numberOfTRTOutliers;
    InDetPerfNtupleBranch<int> m_numberOfTRTHoles;
    InDetPerfNtupleBranch<int> m_numberOfTRTHighThresholdHits;
    InDetPerfNtupleBranch<int> m_numberOfTRTHighThresholdHitsTotal;
    InDetPerfNtupleBranch<int> m_numberOfTRTHighThresholdOutliers;
    InDetPerfNtupleBranch<int> m_numberOfTRTDeadStraws;
    InDetPerfNtupleBranch<int> m_numberOfTRTTubeHits;
    InDetPerfNtupleBranch<int> m_numberOfTRTXenonHits;
    InDetPerfNtupleBranch<int> m_numberOfTRTSharedHits;

    //Flags
    InDetPerfNtupleBranch<int> m_hasTruth; 
    InDetPerfNtupleBranch<int> m_hasTrack; 
    InDetPerfNtupleBranch<int> m_passedTruthSelection;
    InDetPerfNtupleBranch<int> m_passedTrackSelection;
};

#endif // INDET__PERF__NTUPLE_TRUTHTORECO__H