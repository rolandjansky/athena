#include "InDetPerfNtuple_TruthToReco.h" 

InDetPerfNtuple_TruthToReco::InDetPerfNtuple_TruthToReco(InDetPlotBase* pParent, const std::string & dirName, const std::string & treeName):
    InDetPerfNtuple(pParent, dirName, treeName), 
    m_truth_selectedByPileupSwitch("truth_selectedByPileupSwitch",0,*this),
    m_truth_pdgId("truth_pdgId",m_undefinedValue,*this),
    m_truth_barcode("truth_barcode",m_undefinedValue,*this),
    m_truth_charge("truth_charge",m_undefinedValue,*this),
    m_truth_eta("truth_eta",m_undefinedValue,*this),
    m_truth_pt("truth_pt",m_undefinedValue,*this),
    m_truth_d0("truth_d0",m_undefinedValue,*this),
    m_truth_z0("truth_z0",m_undefinedValue,*this),
    m_truth_phi("truth_phi",m_undefinedValue,*this),
    m_truth_theta("truth_theta",m_undefinedValue,*this),
    m_truth_qOverP("truth_qOverP",m_undefinedValue,*this),
    m_truth_qOverPt("truth_qOverPt",m_undefinedValue,*this),    
    m_truth_z0sin("truth_z0sin",m_undefinedValue,*this),
    m_track_truthMatchRanking("track_truthMatchRanking",-1,*this),
    m_track_truthMatchProb("track_truthMatchProb",m_undefinedValue,*this),
    m_track_charge("track_charge",m_undefinedValue,*this),
    m_track_eta("track_eta",m_undefinedValue,*this),
    m_track_pt("track_pt",m_undefinedValue,*this),    
    m_track_d0("track_d0",m_undefinedValue,*this),
    m_track_z0("track_z0",m_undefinedValue,*this),
    m_track_phi("track_phi",m_undefinedValue,*this),
    m_track_theta("track_theta",m_undefinedValue,*this),
    m_track_qOverP("track_qOverP",m_undefinedValue,*this),
    m_track_qOverPt("track_qOverPt",m_undefinedValue,*this),
    m_track_z0sin("track_z0sin",m_undefinedValue,*this),
    m_trackErr_pt("trackErr_pt",m_undefinedValue,*this),
    m_trackErr_d0("trackErr_d0",m_undefinedValue,*this),
    m_trackErr_z0("trackErr_z0",m_undefinedValue,*this),
    m_trackErr_phi("trackErr_phi",m_undefinedValue,*this),
    m_trackErr_theta("trackErr_theta",m_undefinedValue,*this),
    m_trackErr_qOverP("trackErr_qOverP",m_undefinedValue,*this),
    m_trackErr_qOverPt("trackErr_qOverPt",m_undefinedValue,*this),
    m_trackErr_z0sin("trackErr_z0sin",m_undefinedValue,*this),
    m_track_chiSquared("track_chiSquared",m_undefinedValue,*this),
    m_track_nDoF("track_nDoF",m_undefinedValue,*this),
    m_numberOfContribPixelLayers("numberOfContribPixelLayers",m_undefinedValue,*this),
    m_numberOfBLayerHits("numberOfBLayerHits",m_undefinedValue,*this),
    m_numberOfBLayerOutliers("numberOfBLayerOutliers",m_undefinedValue,*this),
    m_numberOfBLayerSharedHits("numberOfBLayerSharedHits",m_undefinedValue,*this),
    m_numberOfBLayerSplitHits("numberOfBLayerSplitHits",m_undefinedValue,*this),
    m_expectBLayerHit("expectBLayerHit",m_undefinedValue,*this),
    m_expectInnermostPixelLayerHit("expectInnermostPixelLayerHit",m_undefinedValue,*this),
    m_numberOfInnermostPixelLayerHits("numberOfInnermostPixelLayerHits",m_undefinedValue,*this),
    m_numberOfInnermostPixelLayerOutliers("numberOfInnermostPixelLayerOutliers",m_undefinedValue,*this),
    m_numberOfInnermostPixelLayerSharedHits("numberOfInnermostPixelLayerSharedHits",m_undefinedValue,*this),
    m_numberOfInnermostPixelLayerSplitHits("numberOfInnermostPixelLayerSplitHits",m_undefinedValue,*this),
    m_expectNextToInnermostPixelLayerHit("expectNextToInnermostPixelLayerHit",m_undefinedValue,*this),
    m_numberOfNextToInnermostPixelLayerHits("numberOfNextToInnermostPixelLayerHits",m_undefinedValue,*this),
    m_numberOfNextToInnermostPixelLayerOutliers("numberOfNextToInnermostPixelLayerOutliers",m_undefinedValue,*this),
    m_numberOfNextToInnermostPixelLayerSharedHits("numberOfNextToInnermostPixelLayerSharedHits",m_undefinedValue,*this),
    m_numberOfNextToInnermostPixelLayerSplitHits("numberOfNextToInnermostPixelLayerSplitHits",m_undefinedValue,*this),
    m_numberOfDBMHits("numberOfDBMHits",m_undefinedValue,*this),
    m_numberOfPixelHits("numberOfPixelHits",m_undefinedValue,*this),
    m_numberOfPixelOutliers("numberOfPixelOutliers",m_undefinedValue,*this),
    m_numberOfPixelHoles("numberOfPixelHoles",m_undefinedValue,*this),
    m_numberOfPixelSharedHits("numberOfPixelSharedHits",m_undefinedValue,*this),
    m_numberOfPixelSplitHits("numberOfPixelSplitHits",m_undefinedValue,*this),
    m_numberOfGangedPixels("numberOfGangedPixels",m_undefinedValue,*this),
    m_numberOfGangedFlaggedFakes("numberOfGangedFlaggedFakes",m_undefinedValue,*this),
    m_numberOfPixelDeadSensors("numberOfPixelDeadSensors",m_undefinedValue,*this),
    m_numberOfPixelSpoiltHits("numberOfPixelSpoiltHits",m_undefinedValue,*this),
    m_numberOfSCTHits("numberOfSCTHits",m_undefinedValue,*this),
    m_numberOfSCTOutliers("numberOfSCTOutliers",m_undefinedValue,*this),
    m_numberOfSCTHoles("numberOfSCTHoles",m_undefinedValue,*this),
    m_numberOfSCTDoubleHoles("numberOfSCTDoubleHoles",m_undefinedValue,*this),
    m_numberOfSCTSharedHits("numberOfSCTSharedHits",m_undefinedValue,*this),
    m_numberOfSCTDeadSensors("numberOfSCTDeadSensors",m_undefinedValue,*this),
    m_numberOfSCTSpoiltHits("numberOfSCTSpoiltHits",m_undefinedValue,*this),
    m_numberOfTRTHits("numberOfTRTHits",m_undefinedValue,*this),
    m_numberOfTRTOutliers("numberOfTRTOutliers",m_undefinedValue,*this),
    m_numberOfTRTHoles("numberOfTRTHoles",m_undefinedValue,*this),
    m_numberOfTRTHighThresholdHits("numberOfTRTHighThresholdHits",m_undefinedValue,*this),
    m_numberOfTRTHighThresholdHitsTotal("numberOfTRTHighThresholdHitsTotal",m_undefinedValue,*this),
    m_numberOfTRTHighThresholdOutliers("numberOfTRTHighThresholdOutliers",m_undefinedValue,*this),
    m_numberOfTRTDeadStraws("numberOfTRTDeadStraws",m_undefinedValue,*this),
    m_numberOfTRTTubeHits("numberOfTRTTubeHits",m_undefinedValue,*this),
    m_numberOfTRTXenonHits("numberOfTRTXenonHits",m_undefinedValue,*this),
    m_numberOfTRTSharedHits("numberOfTRTSharedHits",m_undefinedValue,*this),
    m_hasTruth("hasTruth",0,*this),
    m_hasTrack("hasTrack",0,*this),
    m_passedTruthSelection("passedTruthSelection",0,*this),
    m_passedTrackSelection("passedTrackSelection",0,*this) {
}

void InDetPerfNtuple_TruthToReco::fillTruth(const xAOD::TruthParticle& truth) {
    m_hasTruth = (int)true;
    m_passedTruthSelection         = (m_acc_passedTruthSelection.isAvailable(truth)   ? (int)m_acc_passedTruthSelection(truth)   : 0);
    m_truth_selectedByPileupSwitch = (m_acc_selectedByPileupSwitch.isAvailable(truth) ? (int)m_acc_selectedByPileupSwitch(truth) : 0);

    m_truth_pdgId   = truth.pdgId(); 
    m_truth_barcode = truth.barcode(); 
    m_truth_charge  = truth.charge();
    m_truth_eta     = truth.eta();
    m_truth_pt      = truth.pt();
    m_truth_phi     = truth.phi();
    m_truth_d0      = (truth.isAvailable<float>("d0")     ? truth.auxdata<float>("d0")     : m_undefinedValue);
    m_truth_z0      = (truth.isAvailable<float>("z0")     ? truth.auxdata<float>("z0")     : m_undefinedValue);
    m_truth_theta   = (truth.isAvailable<float>("theta")  ? truth.auxdata<float>("theta")  : m_undefinedValue);
    m_truth_qOverP  = (truth.isAvailable<float>("qOverP") ? truth.auxdata<float>("qOverP") : m_undefinedValue);
    m_truth_qOverPt = (((m_truth_theta() != m_undefinedValue) && (m_truth_qOverP() != m_undefinedValue)) ? m_truth_qOverP() * (1 / std::sin(m_truth_theta())) : m_undefinedValue);
    m_truth_z0sin   = (((m_truth_theta() != m_undefinedValue) && (m_truth_z0()     != m_undefinedValue)) ? m_truth_z0() * std::sin(m_truth_theta())           : m_undefinedValue);
}

void InDetPerfNtuple_TruthToReco::fillTrack(const xAOD::TrackParticle& track, const int truthMatchRanking) {
    m_hasTrack = (int)true;
    m_passedTrackSelection = (m_acc_passedTrackSelection.isAvailable(track) ? (int)m_acc_passedTrackSelection(track) : 0);

    // Record truth match ranking to handle cases in which multiple tracks share the same truth association
    // Integer value to be interpreted as:
    // n = -1        : No associated truth available (i.e. track-only entry)
    // n =  0        : Associated truth available and this is the 'best matched' (highest probability) associated track
    //                 This is the most likely case when there is a linked truth because most associations are one-to-one
    // n = 1, 2, ... : Assoicated truth available and this is the second, third, ... 'best matched' associated track
    //                 This handles rare cases where there are multiple tracks associated to the same truth
    m_track_truthMatchRanking = truthMatchRanking;

    float prob = (track.isAvailable<float>("truthMatchProbability") ? track.auxdata<float>("truthMatchProbability") : m_undefinedValue);
    m_track_truthMatchProb = (!std::isnan(prob) ? prob : m_undefinedValue);

    m_track_charge  = track.charge();
    m_track_eta     = track.eta();
    m_track_pt      = track.pt(); 
    m_track_d0      = track.d0();  
    m_track_z0      = track.z0(); 
    m_track_phi     = track.phi(); 
    m_track_theta   = track.theta(); 
    m_track_qOverP  = track.qOverP(); 
    m_track_qOverPt = track.qOverP() * (1 / std::sin(track.theta()));
    m_track_z0sin   = track.z0() * std::sin(track.theta()); 

    if (track.qOverP() == 0) m_trackErr_pt = m_undefinedValue;
    else {
        float dqOverP = -1 * track.pt() / std::abs(track.qOverP());
        float dtheta  =      track.pt() / std::tan(track.theta());
        const std::vector<float> & cov = track.definingParametersCovMatrixVec();
        m_trackErr_pt = std::sqrt(dqOverP * (dqOverP * cov[14] + dtheta * cov[13]) + dtheta * dtheta * cov[9]);
    }

    m_trackErr_d0      = std::sqrt(track.definingParametersCovMatrix()(0, 0));
    m_trackErr_z0      = std::sqrt(track.definingParametersCovMatrix()(1, 1));
    m_trackErr_phi     = std::sqrt(track.definingParametersCovMatrix()(2, 2));
    m_trackErr_theta   = std::sqrt(track.definingParametersCovMatrix()(3, 3));
    m_trackErr_qOverP  = std::sqrt(track.definingParametersCovMatrix()(4, 4));
    m_trackErr_qOverPt = m_trackErr_qOverP() * (1 / std::sin(track.theta()));
    m_trackErr_z0sin   = std::sqrt(std::pow(m_trackErr_z0() * std::sin(track.theta()), 2) + 
                                   std::pow(track.z0() * m_trackErr_theta() * std::cos(track.theta()), 2));

    m_track_chiSquared = track.chiSquared();
    m_track_nDoF       = track.numberDoF();

    // Fill track summary info (e.g. hits/holes/outliers on track)
    fillTrackSummaryInfo(track);
}

void InDetPerfNtuple_TruthToReco::fillTrackSummaryInfo(const xAOD::TrackParticle& track) {
    uint8_t value = 0;
    if (track.summaryValue(value, xAOD::numberOfContribPixelLayers)) {
        m_numberOfContribPixelLayers = value;
    }
    if (track.summaryValue(value, xAOD::numberOfBLayerHits)) {
        m_numberOfBLayerHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfBLayerOutliers)) {
        m_numberOfBLayerOutliers = value;
    }
    if (track.summaryValue(value, xAOD::numberOfBLayerSharedHits)) {
        m_numberOfBLayerSharedHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfBLayerSplitHits)) {
        m_numberOfBLayerSplitHits = value;
    }
    if (track.summaryValue(value, xAOD::expectBLayerHit)) {
        m_expectBLayerHit = value;
    }
    if (track.summaryValue(value, xAOD::expectInnermostPixelLayerHit)) {
        m_expectInnermostPixelLayerHit = value;
    }
    if (track.summaryValue(value, xAOD::numberOfInnermostPixelLayerHits)) {
        m_numberOfInnermostPixelLayerHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfInnermostPixelLayerOutliers)) {
        m_numberOfInnermostPixelLayerOutliers = value;
    }
    if (track.summaryValue(value, xAOD::numberOfInnermostPixelLayerSharedHits)) {
        m_numberOfInnermostPixelLayerSharedHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfInnermostPixelLayerSplitHits)) {
        m_numberOfInnermostPixelLayerSplitHits = value;
    }
    if (track.summaryValue(value, xAOD::expectNextToInnermostPixelLayerHit)) {
        m_expectNextToInnermostPixelLayerHit = value;
    }
    if (track.summaryValue(value, xAOD::numberOfNextToInnermostPixelLayerHits)) {
        m_numberOfNextToInnermostPixelLayerHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfNextToInnermostPixelLayerOutliers)) {
        m_numberOfNextToInnermostPixelLayerOutliers = value;
    }
    if (track.summaryValue(value, xAOD::numberOfNextToInnermostPixelLayerSharedHits)) {
        m_numberOfNextToInnermostPixelLayerSharedHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfNextToInnermostPixelLayerSplitHits)) {
        m_numberOfNextToInnermostPixelLayerSplitHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfDBMHits)) {
        m_numberOfDBMHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfPixelHits)) {
        m_numberOfPixelHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfPixelOutliers)) {
        m_numberOfPixelOutliers = value;
    }
    if (track.summaryValue(value, xAOD::numberOfPixelHoles)) {
        m_numberOfPixelHoles = value;
    }
    if (track.summaryValue(value, xAOD::numberOfPixelSharedHits)) {
        m_numberOfPixelSharedHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfPixelSplitHits)) {
        m_numberOfPixelSplitHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfGangedPixels)) {
        m_numberOfGangedPixels = value;
    }
    if (track.summaryValue(value, xAOD::numberOfGangedFlaggedFakes)) {
        m_numberOfGangedFlaggedFakes = value;
    }
    if (track.summaryValue(value, xAOD::numberOfPixelDeadSensors)) {
        m_numberOfPixelDeadSensors = value;
    }
    if (track.summaryValue(value, xAOD::numberOfPixelSpoiltHits)) {
        m_numberOfPixelSpoiltHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfSCTHits)) {
        m_numberOfSCTHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfSCTOutliers)) {
        m_numberOfSCTOutliers = value;
    }
    if (track.summaryValue(value, xAOD::numberOfSCTHoles)) {
        m_numberOfSCTHoles = value;
    }
    if (track.summaryValue(value, xAOD::numberOfSCTDoubleHoles)) {
        m_numberOfSCTDoubleHoles = value;
    }
    if (track.summaryValue(value, xAOD::numberOfSCTSharedHits)) {
        m_numberOfSCTSharedHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfSCTDeadSensors)) {
        m_numberOfSCTDeadSensors = value;
    }
    if (track.summaryValue(value, xAOD::numberOfSCTSpoiltHits)) {
        m_numberOfSCTSpoiltHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfTRTHits)) {
        m_numberOfTRTHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfTRTOutliers)) {
        m_numberOfTRTOutliers = value;
    }
    if (track.summaryValue(value, xAOD::numberOfTRTHoles)) {
        m_numberOfTRTHoles = value;
    }
    if (track.summaryValue(value, xAOD::numberOfTRTHighThresholdHits)) {
        m_numberOfTRTHighThresholdHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfTRTHighThresholdHitsTotal)) {
        m_numberOfTRTHighThresholdHitsTotal = value;
    }
    if (track.summaryValue(value, xAOD::numberOfTRTHighThresholdOutliers)) {
        m_numberOfTRTHighThresholdOutliers = value;
    }
    if (track.summaryValue(value, xAOD::numberOfTRTDeadStraws)) {
        m_numberOfTRTDeadStraws = value;
    }
    if (track.summaryValue(value, xAOD::numberOfTRTTubeHits)) {
        m_numberOfTRTTubeHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfTRTXenonHits)) {
        m_numberOfTRTXenonHits = value;
    }
    if (track.summaryValue(value, xAOD::numberOfTRTSharedHits)) {
        m_numberOfTRTSharedHits = value;
    }
}

void InDetPerfNtuple_TruthToReco::fillTree() {
    Fill();
}