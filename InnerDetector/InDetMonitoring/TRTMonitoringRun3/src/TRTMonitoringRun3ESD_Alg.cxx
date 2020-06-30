/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTMonitoringRun3/TRTMonitoringRun3ESD_Alg.h"

#include "AthContainers/DataVector.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TRT_ConditionsServices/ITRT_CalDbTool.h"
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummaryTool.h"
#include "TRT_ConditionsServices/ITRT_DAQ_ConditionsSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"

#include <sstream>
#include <iomanip>
#include <memory> 
#include <cmath>
using namespace std;

//Private Static Const data member initialization
const int TRTMonitoringRun3ESD_Alg::s_numberOfBarrelStacks = 32;
const int TRTMonitoringRun3ESD_Alg::s_numberOfEndCapStacks = 32;
const int TRTMonitoringRun3ESD_Alg::s_Straw_max[2] = {1642, 3840};
const int TRTMonitoringRun3ESD_Alg::s_iStack_max[2] = {32, 64};
const int TRTMonitoringRun3ESD_Alg::s_iChip_max[2] = {104, 240};
const int TRTMonitoringRun3ESD_Alg::s_numberOfStacks[2] = {32, 32};
const int TRTMonitoringRun3ESD_Alg::s_moduleNum[2] = {96, 64};

TRTMonitoringRun3ESD_Alg::TRTMonitoringRun3ESD_Alg( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_idHelper(0)
,m_sumTool("TRT_StrawStatusSummaryTool", this)
,m_TRTStrawNeighbourSvc("TRT_StrawNeighbourSvc", name)
,m_TRTCalDbTool("TRT_CalDbTool", this)
,m_drifttool("TRT_DriftFunctionTool")
,m_pTRTHelper(0)
,m_mgr(0)
,m_isCosmics(false)
,m_minTRThits(10)
,m_minP(0)
,m_EventBurstCut(-1)
{
    declareProperty("InDetTRTStrawStatusSummaryTool",                 m_sumTool);
    declareProperty("ITRT_CalDbTool",                                 m_TRTCalDbTool);
    declareProperty("DriftFunctionTool",                              m_drifttool);
    declareProperty("DoTracksMon",              m_doTracksMon         = true);
    declareProperty("doStraws",                 m_doStraws            = true);
    declareProperty("doChips",                  m_doChips             = true);
    declareProperty("doExpert",                 m_doExpert            = true);
    declareProperty("doShift",                  m_doShift             = true); 
    declareProperty("DistanceToStraw",          m_DistToStraw         = 0.4);  
    declareProperty("min_si_hits",              m_min_si_hits         = 1);
    declareProperty("min_pixel_hits",           m_min_pixel_hits      = 0);
    declareProperty("min_sct_hits",             m_min_sct_hits        = 0);
    declareProperty("min_trt_hits",             m_min_trt_hits        = 10);
    declareProperty("MinTRTHitCut",             m_minTRThits          = 10);
    declareProperty("MinTrackP",                m_minP                = 0.0 * CLHEP::GeV);
    declareProperty("min_pT",                   m_min_pT              = 0.5 * CLHEP::GeV);
}
 
TRTMonitoringRun3ESD_Alg::~TRTMonitoringRun3ESD_Alg() {}

StatusCode TRTMonitoringRun3ESD_Alg::initialize() { 
    using namespace Monitored;
    
    ATH_MSG_VERBOSE("Initializing TRT Monitoring");

    // Initialize superclass
    ATH_CHECK( AthMonitorAlgorithm::initialize() );
    
    // Retrieve detector manager
    ATH_CHECK( detStore()->retrieve(m_mgr, "TRT") );
    // Get ID helper for TRT to access various detector components like straw, straw_layer, layer_or_wheel, phi_module, etc.
    ATH_CHECK( detStore()->retrieve(m_pTRTHelper, "TRT_ID") );
    ATH_CHECK( detStore()->retrieve(m_idHelper, "AtlasID") );

    if (m_doExpert) {
        // Retrieve the TRT_Straw Status Service
        if (m_sumTool.name().empty()) {
            ATH_MSG_WARNING("TRT_StrawStatusTool not given.");
        } else {
            ATH_CHECK( m_sumTool.retrieve() );
        }

        Identifier ident;

        if (m_sumTool.name() != "") {
            ATH_MSG_VERBOSE("Trying " << m_sumTool << " isGood");
            ATH_MSG_VERBOSE("TRT_StrawStatusTool reports status = " << m_sumTool->getStatus(ident));
        }
    } //If do expert

    // Retrieve TRT_StrawNeighbourService
    if (m_TRTStrawNeighbourSvc.name().empty()) {
        ATH_MSG_WARNING("TRT_StrawNeighbourSvc not given.");
    } else {
        if (m_TRTStrawNeighbourSvc.retrieve().isFailure()) {
            ATH_MSG_FATAL("Could not get StrawNeighbourSvc.");
        }
    }

    // Get Track summary tool
    if (m_TrackSummaryTool.retrieve().isFailure())
        ATH_MSG_ERROR("Cannot get TrackSummaryTool");
    else
        ATH_MSG_DEBUG("Retrieved succesfully the track summary tool" << m_TrackSummaryTool);

    //Get TRTCalDbTool
    if (m_TRTCalDbTool.name().empty()) {
        ATH_MSG_WARNING("TRT_CalDbTool not given.");
    } else {
        if (m_TRTCalDbTool.retrieve().isFailure()) {
            ATH_MSG_ERROR("Cannot get TRTCalDBTool.");
        }
    }

    ATH_CHECK(m_drifttool.retrieve());

    // Initialize arrays
    // These arrays store information about each entry to the HitMap histograms

    if (true) {
        //loop over straw hash index to create straw number mapping for TRTViewer
        unsigned int maxHash = m_pTRTHelper->straw_layer_hash_max();

        for (int ibe = 0; ibe < 2; ibe++) { // ibe=0(barrel), ibe=1(endcap)
            for (unsigned int index = 0; index < maxHash; index++) {
                IdentifierHash idHash = index;
                Identifier id = m_pTRTHelper->layer_id(idHash);
                int idBarrelEndcap = m_pTRTHelper->barrel_ec(id);
                int idLayerWheel   = m_pTRTHelper->layer_or_wheel(id);
                int idPhiModule    = m_pTRTHelper->phi_module(id);
                int idStrawLayer   = m_pTRTHelper->straw_layer(id);
                bool isBarrel = m_pTRTHelper->is_barrel(id);
                int idSide;
                int sectorflag = 0;
                const InDetDD::TRT_BaseElement *element = nullptr;

                if (ibe == 0) { // barrel
                    idSide = idBarrelEndcap ? 1 : -1;

                    if (isBarrel && (idBarrelEndcap == -1)) {
                        sectorflag = 1;
                        element = m_mgr->getBarrelElement(idSide, idLayerWheel, idPhiModule, idStrawLayer);
                    }
                } else if (ibe == 1) { // endcap
                    idSide = idBarrelEndcap ? 1 : 0;

                    if (!isBarrel && ((idBarrelEndcap == -2) || (idBarrelEndcap == 2))) {
                        sectorflag = 1;
                        element = m_mgr->getEndcapElement(idSide, idLayerWheel, idStrawLayer, idPhiModule);//, idStrawLayer_ec);
                    }
                }

                if (sectorflag == 1) {
                    if (!element) continue;

                    for (unsigned int istraw = 0; istraw < element->nStraws(); istraw++) {
                        std::vector<Identifier> neighbourIDs;
                        Identifier strawID = m_pTRTHelper->straw_id(id, int(istraw));
                        int i_chip, i_pad;
                        m_TRTStrawNeighbourSvc->getChip(strawID, i_chip);
                        m_TRTStrawNeighbourSvc->getPad(id, i_pad);

                        if (ibe == 0) { //barrel
                            if (idLayerWheel == 1) i_chip += 21;

                            if (idLayerWheel == 2) i_chip += 54;

                            int tempStrawNumber = strawNumber(istraw, idStrawLayer, idLayerWheel);

                            if (tempStrawNumber < 0 || tempStrawNumber > (s_Straw_max[ibe] - 1)) {
                                ATH_MSG_WARNING("Found tempStrawNumber = " << tempStrawNumber << " out of range.");
                            } else {
                                m_mat_chip_B[idPhiModule][tempStrawNumber]    = i_chip;
                                m_mat_chip_B[idPhiModule + 32][tempStrawNumber] = i_chip;
                            }
                        } else if (ibe == 1) { //endcap
                            ++i_chip -= 104;
                            int tempStrawNumber = strawNumberEndCap(istraw, idStrawLayer, idLayerWheel, idPhiModule, idSide);

                            if (tempStrawNumber < 0 || tempStrawNumber > (s_Straw_max[ibe] - 1)) {
                                ATH_MSG_WARNING("Found tempStrawNumber = " << tempStrawNumber << " out of range.");
                            } else {
                                m_mat_chip_E[idPhiModule][tempStrawNumber]    = i_chip;
                                m_mat_chip_E[idPhiModule + 32][tempStrawNumber] = i_chip;
                            }
                        }
                    }
                }
            }
        }
    }

    // Initialization of VarHandleKeys
    ATH_CHECK( m_trackCollectionKey.initialize() );
    ATH_CHECK( m_xAODEventInfoKey.initialize() );
    ATH_CHECK( m_TRT_BCIDCollectionKey.initialize() );
    ATH_CHECK( m_comTimeObjectKey.initialize() );
    ATH_CHECK( m_trigDecisionKey.initialize() );

    return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------------//
int TRTMonitoringRun3ESD_Alg::strawLayerNumber(int strawLayerNumber, int LayerNumber) const {
//----------------------------------------------------------------------------------//
    switch (LayerNumber) {
    case 0:
        return strawLayerNumber;

    case 1:
        return strawLayerNumber + 19;

    case 2:
        return strawLayerNumber + 43;

    default:
        return strawLayerNumber;
    }
}

//----------------------------------------------------------------------------------//
int TRTMonitoringRun3ESD_Alg::strawNumber(int strawNumber, int strawlayerNumber, int LayerNumber) const {
//----------------------------------------------------------------------------------//
    int addToStrawNumber = 0;
    int addToStrawNumberNext = 0;
    int i = 0;
    const int numberOfStraws[75] = {
        0,
        15,
        16, 16, 16, 16,
        17, 17, 17, 17, 17,
        18, 18, 18, 18, 18,
        19, 19, 19,
        18,
        19,
        20, 20, 20, 20, 20,
        21, 21, 21, 21, 21,
        22, 22, 22, 22, 22,
        23, 23, 23, 23, 23,
        24, 24,
        23, 23,
        24, 24, 24, 24,
        25, 25, 25, 25, 25,
        26, 26, 26, 26, 26,
        27, 27, 27, 27, 27,
        28, 28, 28, 28, 28,
        29, 29, 29, 29,
        28,
        0
    };

    do {
        i++;
        addToStrawNumber += numberOfStraws[i - 1];
        addToStrawNumberNext = addToStrawNumber + numberOfStraws[i];
    } while (strawLayerNumber(strawlayerNumber, LayerNumber) != i - 1);

    strawNumber = addToStrawNumberNext - strawNumber - 1;

    if (strawNumber < 0 || strawNumber > s_Straw_max[0] - 1) {
        ATH_MSG_WARNING("strawNumber = " << strawNumber << " out of range. Will set to 0.");
        strawNumber = 0;
    }

    return strawNumber;
}


//----------------------------------------------------------------------------------//
int TRTMonitoringRun3ESD_Alg::strawNumberEndCap(int strawNumber, int strawLayerNumber, int LayerNumber, int phi_stack, int side) const {
//----------------------------------------------------------------------------------//
    // Before perfoming map, corrections need to be perfomed.
    // apply special rotations for endcap mappings
    // for eca, rotate triplets by 180 for stacks 9-16, and 25-32.
    static const int TripletOrientation[2][32] = {
        {
            1, 1, 1, 1, 1, 1, 1, 1,
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 1,
            0, 0, 0, 0, 0, 0, 0, 0
        },
        {
            1, 1, 1, 1, 1, 1, 1, 1,
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 1,
            0, 0, 0, 0, 0, 0, 0, 0
        }
    };
    int phi1 = -1;

    if (side == 2) phi1 = phi_stack, side = 1;
    else if (side == -2) phi1 = 31 - phi_stack, side = 0;

    if (phi1 > -1) {
        if (TripletOrientation[side][phi1]) {
            // Change straw number from 0-23 in straw layer to 0-192
            if (strawLayerNumber < 8)strawNumber = strawNumber + 24 * strawLayerNumber;

            if (strawLayerNumber > 7)strawNumber = strawNumber + 24 * (strawLayerNumber - 8);

            strawNumber = (192 - 1) * TripletOrientation[side][phi1] + strawNumber * (1 - 2 * TripletOrientation[side][phi1]); //actual rotation

            // Take strawNumber back to 0-23
            if (strawLayerNumber < 8) strawLayerNumber = int(strawNumber / 24);

            if (strawLayerNumber > 7) strawLayerNumber = int(strawNumber / 24) + 8;

            strawNumber = strawNumber % 24;
        }

        // Finish rotation
        // Flip straw in layer.

        if (side == 0) strawNumber = 23 - strawNumber;

        // Finish Flipping
    }

    // Done with corrections
    // Start mapping from athena identifiers to TRTViewer maps
    int strawNumberNew = 0;

    if (LayerNumber < 6 && strawLayerNumber > 7) {
        strawNumberNew = strawNumberNew + (384 * LayerNumber);
        strawNumberNew = strawNumberNew + 192 + (strawLayerNumber % 8) + (strawNumber * 8);
    } else if (LayerNumber < 6 && strawLayerNumber < 8) {
        strawNumberNew = strawNumberNew + (384 * LayerNumber);
        strawNumberNew = strawNumberNew + (strawLayerNumber % 8) + (strawNumber * 8);
    } else if (LayerNumber > 5 && strawLayerNumber > 7) {
        strawNumberNew = strawNumberNew + 2304 + 192 * (LayerNumber - 6);
        strawNumberNew = strawNumberNew + 192 + (strawLayerNumber % 8) + (8 * strawNumber);
    } else if (LayerNumber > 5 && strawLayerNumber < 8) {
        strawNumberNew = strawNumberNew + 2304 + 192 * (LayerNumber - 6);
        strawNumberNew = strawNumberNew + (strawLayerNumber % 8) + (8 * strawNumber);
    }

    strawNumber = strawNumberNew;

    if (strawNumber < 0 || strawNumber > s_Straw_max[1] - 1) {
        ATH_MSG_WARNING("strawNumber = " << strawNumber << " out of range. Will set to 0.");
        strawNumber = 0;
    }

    return strawNumber;
}

//----------------------------------------------------------------------------------//
float TRTMonitoringRun3ESD_Alg::radToDegrees(float radValue) const {
//----------------------------------------------------------------------------------//
    float degreeValue = radValue / M_PI * 180;

    if (degreeValue < 0) degreeValue += 360;

    return degreeValue;
}


// Check for EventBurst: Counts high level hits and returns true if the count is less than m_passEventBurstCut.
// If m_EventBurstCut is less than zero, returns allways true
//----------------------------------------------------------------------------------//
bool TRTMonitoringRun3ESD_Alg::checkEventBurst(const TRT_RDO_Container& rdoContainer) const {
//----------------------------------------------------------------------------------//
    if (m_EventBurstCut <= 0) return true;

    int nHLHits = 0;
    TRT_RDO_Container::const_iterator RDO_CollectionBegin = rdoContainer.begin();
    TRT_RDO_Container::const_iterator RDO_CollectionEnd   = rdoContainer.end();

    for (; RDO_CollectionBegin != RDO_CollectionEnd; ++RDO_CollectionBegin) {
        const InDetRawDataCollection<TRT_RDORawData> *TRT_Collection(*RDO_CollectionBegin);

        if (!TRT_Collection) continue;
        else {
            DataVector<TRT_RDORawData>::const_iterator p_rdo = TRT_Collection->begin();

            for (; p_rdo != TRT_Collection->end(); ++p_rdo) {
                const TRT_LoLumRawData *p_lolum = dynamic_cast<const TRT_LoLumRawData *>(*p_rdo);

                if (!p_lolum) continue;

                if (p_lolum->highLevel()) nHLHits++;
            }
        }
    }

    if (nHLHits > m_EventBurstCut) return false;
    else return true;
}


// Fill the TRT Track level histograms
//----------------------------------------------------------------------------------//
StatusCode TRTMonitoringRun3ESD_Alg::fillTRTTracks(const xAOD::TrackParticleContainer& trackCollection,
                                              const xAOD::TrigDecision* trigDecision,
                                              const ComTime* comTimeObject) const {
//----------------------------------------------------------------------------------//
    ATH_MSG_VERBOSE("Filling TRT Tracks Histos");
    
    // TProfile
    auto HitToTonTMapS_x                = Monitored::Scalar<float>("HitToTonTMapS_x", 0.0);
    auto HitToTonTMapS_y                = Monitored::Scalar<float>("HitToTonTMapS_y", 0.0);
    auto HitToTonTMapC_x                = Monitored::Scalar<float>("HitToTonTMapC_x", 0.0);
    auto HitToTonTMapC_y                = Monitored::Scalar<float>("HitToTonTMapC_y", 0.0);
    auto ValidRawDriftTimeonTrkS_x      = Monitored::Scalar<float>("ValidRawDriftTimeonTrkS_x", 0.0);
    auto ValidRawDriftTimeonTrkS_y      = Monitored::Scalar<float>("ValidRawDriftTimeonTrkS_y", 0.0);
    auto ValidRawDriftTimeonTrkC_x      = Monitored::Scalar<float>("ValidRawDriftTimeonTrkC_x", 0.0);
    auto ValidRawDriftTimeonTrkC_y      = Monitored::Scalar<float>("ValidRawDriftTimeonTrkC_y", 0.0);
    auto HitTronTMapS_x                 = Monitored::Scalar<float>("HitTronTMapS_x", 0.0);
    auto HitTronTMapS_y                 = Monitored::Scalar<float>("HitTronTMapS_y", 0.0);
    auto HitTronTMapC_x                 = Monitored::Scalar<float>("HitTronTMapC_x", 0.0);
    auto HitTronTMapC_y                 = Monitored::Scalar<float>("HitTronTMapC_y", 0.0);
    auto HitTronTwEPCMapS_x             = Monitored::Scalar<float>("HitTronTwEPCMapS_x", 0.0);
    auto HitTronTwEPCMapS_y             = Monitored::Scalar<float>("HitTronTwEPCMapS_y", 0.0);
    auto HitTronTwEPCMapC_x             = Monitored::Scalar<float>("HitTronTwEPCMapC_x", 0.0);
    auto HitTronTwEPCMapC_y             = Monitored::Scalar<float>("HitTronTwEPCMapC_y", 0.0);
    auto AvgTroTDetPhi_B_Ar_x           = Monitored::Scalar<float>("AvgTroTDetPhi_B_Ar_x", 0.0);
    auto AvgTroTDetPhi_B_Ar_y           = Monitored::Scalar<float>("AvgTroTDetPhi_B_Ar_y", 0.0);
    auto AvgTroTDetPhi_B_x              = Monitored::Scalar<float>("AvgTroTDetPhi_B_x", 0.0);
    auto AvgTroTDetPhi_B_y              = Monitored::Scalar<float>("AvgTroTDetPhi_B_y", 0.0);
    auto AvgTroTDetPhi_E_Ar_x           = Monitored::Scalar<float>("AvgTroTDetPhi_E_Ar_x", 0.0);
    auto AvgTroTDetPhi_E_Ar_y           = Monitored::Scalar<float>("AvgTroTDetPhi_E_Ar_y", 0.0);
    auto AvgTroTDetPhi_E_x              = Monitored::Scalar<float>("AvgTroTDetPhi_E_x", 0.0);
    auto AvgTroTDetPhi_E_y              = Monitored::Scalar<float>("AvgTroTDetPhi_E_y", 0.0);
    auto NumHoTDetPhi_B_x               = Monitored::Scalar<float>("NumHoTDetPhi_B_x", 0.0);
    auto NumHoTDetPhi_B_y               = Monitored::Scalar<float>("NumHoTDetPhi_B_y", 0.0);
    auto NumHoTDetPhi_E_x               = Monitored::Scalar<float>("NumHoTDetPhi_E_x", 0.0);
    auto NumHoTDetPhi_E_y               = Monitored::Scalar<float>("NumHoTDetPhi_E_y", 0.0);
    auto EvtPhaseDetPhi_B_x             = Monitored::Scalar<float>("EvtPhaseDetPhi_B_x", 0.0);
    auto EvtPhaseDetPhi_B_y             = Monitored::Scalar<float>("EvtPhaseDetPhi_B_y", 0.0);
    auto EvtPhaseDetPhi_E_x             = Monitored::Scalar<float>("EvtPhaseDetPhi_E_x", 0.0);
    auto EvtPhaseDetPhi_E_y             = Monitored::Scalar<float>("EvtPhaseDetPhi_E_y", 0.0);
    
    // TH1F
    auto DriftTimeonTrkDist_B           = Monitored::Scalar<float>("DriftTimeonTrkDist_B", 0.0);
    auto DriftTimeonTrkDist_B_Ar        = Monitored::Scalar<float>("DriftTimeonTrkDist_B_Ar", 0.0);
    auto DriftTimeonTrkDist_E_Ar        = Monitored::Scalar<float>("DriftTimeonTrkDist_E_Ar", 0.0);
    auto DriftTimeonTrkDist_E           = Monitored::Scalar<float>("DriftTimeonTrkDist_E", 0.0);
    auto NumTrksDetPhi_B                = Monitored::Scalar<float>("NumTrksDetPhi_B", 0.0);
    auto NumTrksDetPhi_E                = Monitored::Scalar<float>("NumTrksDetPhi_E", 0.0);
    auto Pull_Biased_Barrel             = Monitored::Scalar<float>("Pull_Biased_Barrel", 0.0);
    auto Pull_Biased_EndCap             = Monitored::Scalar<float>("Pull_Biased_EndCap", 0.0);
    auto Residual_B                     = Monitored::Scalar<float>("Residual_B", 0.0);
    auto Residual_B_Ar                  = Monitored::Scalar<float>("Residual_B_Ar", 0.0);
    auto Residual_B_20GeV               = Monitored::Scalar<float>("Residual_B_20GeV", 0.0);
    auto Residual_B_Ar_20GeV            = Monitored::Scalar<float>("Residual_B_Ar_20GeV", 0.0);
    auto Residual_E                     = Monitored::Scalar<float>("Residual_E", 0.0);
    auto Residual_E_Ar                  = Monitored::Scalar<float>("Residual_E_Ar", 0.0);
    auto Residual_E_20GeV               = Monitored::Scalar<float>("Residual_E_20GeV", 0.0);
    auto Residual_E_Ar_20GeV            = Monitored::Scalar<float>("Residual_E_Ar_20GeV", 0.0);
    auto TimeResidual_B                 = Monitored::Scalar<float>("TimeResidual_B", 0.0);
    auto TimeResidual_B_Ar              = Monitored::Scalar<float>("TimeResidual_B_Ar", 0.0);
    auto TimeResidual_E                 = Monitored::Scalar<float>("TimeResidual_E", 0.0);
    auto TimeResidual_E_Ar              = Monitored::Scalar<float>("TimeResidual_E_Ar", 0.0);
    auto TronTDist_E                    = Monitored::Scalar<float>("TronTDist_E", 0.0);
    auto TronTDist_B                    = Monitored::Scalar<float>("TronTDist_B", 0.0);
    auto TronTDist_B_Ar                 = Monitored::Scalar<float>("TronTDist_B_Ar", 0.0);
    auto TronTDist_E_Ar                 = Monitored::Scalar<float>("TronTDist_E_Ar", 0.0);
    auto WireToTrkPosition_B_Ar         = Monitored::Scalar<float>("WireToTrkPosition_B_Ar", 0.0);
    auto WireToTrkPosition_B            = Monitored::Scalar<float>("WireToTrkPosition_B", 0.0);
    auto WireToTrkPosition_E_Ar         = Monitored::Scalar<float>("WireToTrkPosition_E_Ar", 0.0);
    auto WireToTrkPosition_E            = Monitored::Scalar<float>("WireToTrkPosition_E", 0.0);
    auto EvtPhase                       = Monitored::Scalar<float>("EvtPhase", 0.0);

    // TH2F
    auto RtRelation_B_Ar_x              = Monitored::Scalar<float>("RtRelation_B_Ar_x", 0.0);
    auto RtRelation_B_Ar_y              = Monitored::Scalar<float>("RtRelation_B_Ar_y", 0.0);
    auto RtRelation_B_x                 = Monitored::Scalar<float>("RtRelation_B_x", 0.0);
    auto RtRelation_B_y                 = Monitored::Scalar<float>("RtRelation_B_y", 0.0);
    auto RtRelation_E_Ar_x              = Monitored::Scalar<float>("RtRelation_E_Ar_x", 0.0);
    auto RtRelation_E_Ar_y              = Monitored::Scalar<float>("RtRelation_E_Ar_y", 0.0);
    auto RtRelation_E_x                 = Monitored::Scalar<float>("RtRelation_E_x", 0.0);
    auto RtRelation_E_y                 = Monitored::Scalar<float>("RtRelation_E_y", 0.0);
    auto EvtPhaseVsTrig_x               = Monitored::Scalar<float>("EvtPhaseVsTrig_x", 0.0);
    auto EvtPhaseVsTrig_y               = Monitored::Scalar<float>("EvtPhaseVsTrig_y", 0.0);
    
    // Initialize a bunch of stuff before looping over the track collection. Fill some basic histograms.
    const float timeCor =  comTimeObject ? comTimeObject->getTime() : 0;

    auto p_trk = trackCollection.begin();

    const Trk::Perigee *mPer = nullptr;
    const DataVector<const Trk::TrackParameters> *AllTrkPar(0);
    DataVector<const Trk::TrackParameters>::const_iterator p_trkpariter;

    int ntrackstack[2][64];

    for (int ibe = 0; ibe < 2; ibe++) {
        std::fill(ntrackstack[ibe], ntrackstack[ibe] + 64, 0);
    }

for (; p_trk != trackCollection.end(); ++p_trk) {

        uint8_t tempHitsVariable(0);
        (*p_trk)->summaryValue(tempHitsVariable, xAOD::SummaryType::numberOfTRTHits);
        int nTRTHits = unsigned(tempHitsVariable);


        if (nTRTHits < m_minTRThits) continue;

        AllTrkPar = ((*p_trk)->track())->trackParameters();

        // Search of MeasuredPerigee in TrackParameters
        // The following algorithm only finds the First perigee measurement.
        // As there should be one and only one perigee measurement then this assumption should be valid.
        // But no check is done to see if there is more than one perigee measurement.
        for (p_trkpariter = AllTrkPar->begin(); p_trkpariter != AllTrkPar->end(); ++p_trkpariter) {
            // If track parameter does have a measured perigee then the track parameter is a keeper and break out of the loop
            if ((mPer = dynamic_cast<const Trk::Perigee *>(*p_trkpariter))) break;
        }

        if (!mPer) continue;

        float theta   =  mPer->parameters()[Trk::theta];
        float p       =  (mPer->parameters()[Trk::qOverP] != 0.) ? std::abs(1. / (mPer->parameters()[Trk::qOverP])) : 10e7;
        float pT      =  (p * std::sin(theta));
        pT = pT * 1e-3;  // GeV

        if (p < m_minP) continue;

        const DataVector<const Trk::TrackStateOnSurface> *trackStates = ((*p_trk)->track())->trackStateOnSurfaces();

        if (trackStates == 0) continue;

        DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItBegin0    = trackStates->begin();
        DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItBegin     = trackStates->begin();
        DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItBeginTemp = trackStates->begin();
        DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItEnd       = trackStates->end();
        
        (*p_trk)->summaryValue(tempHitsVariable, xAOD::SummaryType::numberOfTRTHits);
        int n_trt_hits = unsigned(tempHitsVariable);
        (*p_trk)->summaryValue(tempHitsVariable, xAOD::SummaryType::numberOfSCTHits);
        int n_sct_hits = unsigned(tempHitsVariable);
        (*p_trk)->summaryValue(tempHitsVariable, xAOD::SummaryType::numberOfPixelHits);
        int n_pixel_hits = unsigned(tempHitsVariable);

        const int n_si_hits = n_pixel_hits + n_sct_hits;
        bool is_pT_over_20GeV = false;

        if (mPer->pT() > 20 * CLHEP::GeV) {
            is_pT_over_20GeV = true;
        } else {
            is_pT_over_20GeV = false;
        }

        const bool cnst_is_pT_over_20GeV = is_pT_over_20GeV;
        /// Hardcoded cut for pT 2.0 GeV for collision setup
        float min_pt_new = m_min_pT;

        if (m_isCosmics == false) {
            min_pt_new = 2.0 * CLHEP::GeV;
        }

        const bool passed_track_preselection =
            (mPer->pT() > min_pt_new) &&
            (p > m_minP) &&
            (n_si_hits >= m_min_si_hits) &&
            (n_pixel_hits >= m_min_pixel_hits) &&
            (n_sct_hits >= m_min_sct_hits) &&
            (n_trt_hits >= m_min_trt_hits);

        if (!passed_track_preselection) continue;

        int nTRTHitsW[2][2];
        int nTRTHits_side[2][2];
        int nTRTHitsW_perwheel[2][18];
        int hitontrack[2] = {0, 0};
        int hitontrack_E_side[2] = {0, 0};

        for (int ibe = 0; ibe < 2; ibe++) {
            for (int iside = 0; iside < 2; iside++) {
                nTRTHits_side[ibe][iside] = -1;
                nTRTHitsW[ibe][iside] = 0;
            }
            std::fill(nTRTHitsW_perwheel[ibe], nTRTHitsW_perwheel[ibe] + 18, 0);
        }

        int barrel_ec = 0;
        int layer_or_wheel = 0;
        int phi_module = 0;
        int straw_layer = 0;
        int straw = 0;
        int nearest_straw_layer[2] = {100, 100};
        int nearest_straw[2] = {0, 0};
        int testLayer[2] = {100, 100};
        float phi2D[2] = {-100, -100};

        for (TSOSItBeginTemp = TSOSItBegin0; TSOSItBeginTemp != TSOSItEnd; ++TSOSItBeginTemp) {
            if ((*TSOSItBeginTemp) == 0) continue;

            if (! ((*TSOSItBeginTemp)->type(Trk::TrackStateOnSurface::Measurement)) ) continue;
            const InDet::TRT_DriftCircleOnTrack *trtCircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>((*TSOSItBeginTemp)->measurementOnTrack());

            if (!trtCircle) continue;
            const Trk::TrackParameters *aTrackParam = dynamic_cast<const Trk::TrackParameters *>((*TSOSItBeginTemp)->trackParameters());

            if (!aTrackParam) continue;
            Identifier DCoTId = trtCircle->identify();
            barrel_ec = m_pTRTHelper->barrel_ec(DCoTId);
            int ibe = std::abs(barrel_ec) - 1;
            layer_or_wheel = m_pTRTHelper->layer_or_wheel (DCoTId);
            straw_layer = m_pTRTHelper->straw_layer(DCoTId);
            straw = m_pTRTHelper->straw(DCoTId);

            // Restrict ourselves to the inner most TRT layers To get detector phi.
            if (layer_or_wheel >= testLayer[ibe]) continue;
            testLayer[ibe] = layer_or_wheel;

            if (straw_layer < nearest_straw_layer[ibe]) {
                nearest_straw_layer[ibe] = straw_layer;
                nearest_straw[ibe] = straw;
                const InDetDD::TRT_BaseElement *circleElement = nullptr;
                circleElement = trtCircle->detectorElement();
                phi2D[ibe] = radToDegrees(circleElement->strawCenter(nearest_straw[ibe]).phi());
                circleElement = nullptr;
            }
        }

        if (phi2D[0] == -999) {
            ATH_MSG_DEBUG("Track did not go through inner layer of Barrel.");
        } else {
            ATH_MSG_VERBOSE("Track's closest approach is m_layer_or_wheel: " <<
                            testLayer[0] << " m_straw_layer: " <<
                            nearest_straw_layer[0] << " (in the Barrel).");
        }

        if (phi2D[1] == -999) {
            ATH_MSG_DEBUG("Track did not go through any inner layer of EndCap A or C.");
        } else {
            ATH_MSG_VERBOSE("Track's closest approach is m_layer_or_wheel: " <<
                            testLayer[1] << " m_straw_layer: " <<
                            nearest_straw_layer[1] << " (in the EndCaps).");
        }

        bool trackfound[2][64];

        for (int i = 0; i < 2; i++) {
            std::fill(trackfound[i], trackfound[i] + 64, false);
        }
        
        for (TSOSItBegin = TSOSItBegin0; TSOSItBegin != TSOSItEnd; ++TSOSItBegin) {
            // Select a TSOS which is non-empty, measurement type and contains  both drift circle and track parameters informations
            if ((*TSOSItBegin) == 0) continue;

            if ( !((*TSOSItBegin)->type(Trk::TrackStateOnSurface::Measurement)) ) continue;

            const InDet::TRT_DriftCircleOnTrack *trtCircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>((*TSOSItBegin)->measurementOnTrack());

            if (!trtCircle) continue;

            const Trk::TrackParameters *aTrackParam = dynamic_cast<const Trk::TrackParameters *>((*TSOSItBegin)->trackParameters());

            if (!aTrackParam) continue;

            Identifier DCoTId = trtCircle->identify();
            barrel_ec = m_pTRTHelper->barrel_ec(DCoTId);
            layer_or_wheel = m_pTRTHelper->layer_or_wheel(DCoTId);
            phi_module = m_pTRTHelper->phi_module(DCoTId);
            straw_layer = m_pTRTHelper->straw_layer(DCoTId);
            straw = m_pTRTHelper->straw(DCoTId);
            int ibe = std::abs(barrel_ec) - 1;     // ibe   = 0 (Barrel), ibe   = 1 (Endcap)
            int iside = barrel_ec > 0 ? 0 : 1;     // iside = 0 (Side A), iside = 1 (Side C)
            int thisStrawNumber[2] = {-1, -1};
            int chip[2] = {0, 0};

            if (ibe == 0) {
                thisStrawNumber[ibe] = strawNumber(straw, straw_layer, layer_or_wheel);

                if (thisStrawNumber[ibe] >= 0 && thisStrawNumber[ibe] < s_Straw_max[ibe]) {
                    chip[ibe] = m_mat_chip_B[phi_module][thisStrawNumber[ibe]];
                }
            } else if (ibe == 1) {
                thisStrawNumber[ibe] = strawNumberEndCap(straw, straw_layer, layer_or_wheel, phi_module, barrel_ec);

                if (thisStrawNumber[ibe] >= 0 && thisStrawNumber[ibe] < s_Straw_max[ibe]) {
                    chip[ibe] = m_mat_chip_E[phi_module][thisStrawNumber[ibe]];
                }
            } else {
                thisStrawNumber[ibe] = -1;
            }

            if (thisStrawNumber[ibe] < 0 || thisStrawNumber[ibe] >= s_Straw_max[ibe]) continue;

            Identifier surfaceID;
            const Trk::MeasurementBase *mesb = (*TSOSItBegin)->measurementOnTrack();
            surfaceID = trtCircle->identify();
            const bool isArgonStraw = ( Straw_Gastype( m_sumTool->getStatusHT(surfaceID) ) == GasType::Ar );
            // Assume always Xe if m_ArgonXenonSplitter is not enabled, otherwise check the straw status (good is Xe, non-good is Ar)
            float temp_locr = aTrackParam->parameters()[Trk::driftRadius];
            TRTCond::RtRelation const *rtr = m_TRTCalDbTool->getRtRelation(surfaceID);
            int iphi_module = -9999;

            if (iside == 0) iphi_module = phi_module;
            else if (iside == 1) iphi_module = phi_module + 32;

            trackfound[ibe][iphi_module] = true;

            if (((ibe == 0) && (temp_locr < m_DistToStraw)) ||
                ((ibe == 1) && ((*TSOSItBegin)->type(Trk::TrackStateOnSurface::Measurement) ||
                                (*TSOSItBegin)->type(Trk::TrackStateOnSurface::Outlier) ||
                                (*TSOSItBegin)->type(Trk::TrackStateOnSurface::Hole)) &&
                 (temp_locr < m_DistToStraw))) {
                if (m_idHelper->is_trt(DCoTId)) {
                    if (ibe == 0) {
                        hitontrack[ibe]++;
                    } else if (ibe == 1) {
                        hitontrack[ibe]++;
                        hitontrack_E_side[iside]++;
                    }
                }
            } 
            const InDet::TRT_DriftCircle *RawDriftCircle = dynamic_cast<const InDet::TRT_DriftCircle *>(trtCircle->prepRawData());
            bool isTubeHit = (mesb->localCovariance()(Trk::locX, Trk::locX) > 1.0) ? 1 : 0;
            if (RawDriftCircle) {
                nTRTHits_side[ibe][iside]++;
                float timeOverThreshold = RawDriftCircle->timeOverThreshold();
                double t0 = m_TRTCalDbTool->getT0(DCoTId, TRTCond::ExpandedIdentifier::STRAW);

                if (m_doExpert && m_doChips) {
                    HitToTonTMapC_x = chip[ibe] - 1;
                    HitToTonTMapC_y = timeOverThreshold;
                    fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitToTonTMapC_x, HitToTonTMapC_y);
                }

                const bool driftTimeValid = RawDriftCircle->driftTimeValid();

                if (driftTimeValid) {
                    const float validRawDriftTime = RawDriftCircle->rawDriftTime();

                    if (m_doExpert && m_doStraws) {
                        ValidRawDriftTimeonTrkS_x = thisStrawNumber[ibe];
                        ValidRawDriftTimeonTrkS_y = validRawDriftTime;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), ValidRawDriftTimeonTrkS_x, ValidRawDriftTimeonTrkS_y);
                    }

                    if (m_doExpert && m_doChips) {
                        ValidRawDriftTimeonTrkC_x = chip[ibe] - 1;
                        ValidRawDriftTimeonTrkC_y = validRawDriftTime;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), ValidRawDriftTimeonTrkC_x, ValidRawDriftTimeonTrkC_y);
                    }
                }

                if (m_doShift && m_doStraws) {
                    if (ibe == 0) {
                        if (isArgonStraw) {
                            DriftTimeonTrkDist_B_Ar = RawDriftCircle->rawDriftTime();
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), DriftTimeonTrkDist_B_Ar);
                        }
                        else {
                            DriftTimeonTrkDist_B = RawDriftCircle->rawDriftTime();
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), DriftTimeonTrkDist_B);
                        }
                    } else if (ibe == 1) {
                        if (isArgonStraw) {
                            DriftTimeonTrkDist_E_Ar = RawDriftCircle->rawDriftTime();
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), DriftTimeonTrkDist_E_Ar);
                        }
                        else {
                            DriftTimeonTrkDist_E = RawDriftCircle->rawDriftTime();
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), DriftTimeonTrkDist_E);
                        }
                    }
                }

                float locR_err = 0.0;
                const AmgSymMatrix(5)* b_err = aTrackParam->covariance();

                if (b_err) {
                    locR_err = Amg::error(*b_err, Trk::locR);
                } else {
                    ATH_MSG_ERROR("Track parameters have no covariance attached.");
                }

                float loc_err = sqrt(Amg::error(trtCircle->localCovariance(), Trk::driftRadius));
                float locR = aTrackParam->parameters()[Trk::driftRadius];
                float loc = trtCircle->localParameters()[Trk::driftRadius];

                if (isTubeHit) {
                    bool isOK = false;
                    loc = m_drifttool->driftRadius(RawDriftCircle->rawDriftTime(), DCoTId, t0, isOK);

                    if ((loc * locR) < 0) loc = -loc;
                }

                // Calculate Residuals for hit
                if (m_doShift && m_doStraws) {
                    const double pull_b =
                        ((loc - locR) /
                         sqrt((loc_err * loc_err * loc_err * loc_err) -
                              (locR_err * locR_err * locR_err * locR_err)));
                    const double thist0 = m_TRTCalDbTool->getT0(surfaceID);
                    const double trkdrifttime = (!rtr) ? 0 : rtr->drifttime(fabs(locR));
                    const double timeresidual = RawDriftCircle->rawDriftTime() - thist0 - trkdrifttime;

                    if (ibe == 0) {
                        if (!isTubeHit) {
                            Pull_Biased_Barrel = pull_b;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), Pull_Biased_Barrel);
                        }

                        if (isArgonStraw) {
                            Residual_B_Ar = loc - locR;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), Residual_B_Ar);

                            if (cnst_is_pT_over_20GeV) {
                                Residual_B_Ar_20GeV = loc - locR;
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe), Residual_B_Ar_20GeV);
                            }
                            TimeResidual_B_Ar = timeresidual;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), TimeResidual_B_Ar);
                        } else {
                            Residual_B = loc - locR;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), Residual_B);
                            TimeResidual_B = timeresidual;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), TimeResidual_B);

                            if (cnst_is_pT_over_20GeV) {
                                Residual_B_20GeV = loc - locR;
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe), Residual_B_20GeV);
                            }
                        }
                    } else if (ibe == 1) {
                        if (!isTubeHit) {
                            Pull_Biased_EndCap = pull_b;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), Pull_Biased_EndCap);
                        }

                        if (isArgonStraw) {
                            Residual_E_Ar = loc - locR;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), Residual_E_Ar);
                            TimeResidual_E_Ar = timeresidual;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), TimeResidual_E_Ar);

                            if (cnst_is_pT_over_20GeV) {
                                Residual_E_Ar_20GeV = loc - locR;
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), Residual_E_Ar_20GeV);
                            }
                        } else {
                            Residual_E = loc - locR;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), Residual_E);
                            TimeResidual_E = timeresidual;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), TimeResidual_E);

                            if (cnst_is_pT_over_20GeV) {
                                Residual_E_20GeV = loc - locR;
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), Residual_E_20GeV);
                            }
                        }
                    }
                }

                if (m_doShift) {
                    if (ibe == 0) {
                        if (isArgonStraw) {
                            WireToTrkPosition_B_Ar = locR;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), WireToTrkPosition_B_Ar);
                        } else {
                            WireToTrkPosition_B = locR;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), WireToTrkPosition_B);
                        }
                    } else if (ibe == 1) {
                        if (isArgonStraw) {
                            WireToTrkPosition_E_Ar = locR;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), WireToTrkPosition_E_Ar);
                        } else {
                            WireToTrkPosition_E = locR;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), WireToTrkPosition_E);
                        }
                    }
                }

                const float LE = (RawDriftCircle->driftTimeBin()) * 3.125;
                const float EP = timeCor;

                if (m_doShift && m_doStraws) {
                    if (ibe == 0) {
                        if (isArgonStraw) {
                            if (m_isCosmics) {
                                RtRelation_B_Ar_x = LE - EP - t0;
                                RtRelation_B_Ar_y = fabs(locR);
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe), RtRelation_B_Ar_x, RtRelation_B_Ar_y);
                            } else {
                                RtRelation_B_Ar_x = LE - t0;
                                RtRelation_B_Ar_y = fabs(locR);
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe), RtRelation_B_Ar_x, RtRelation_B_Ar_y);
                            }
                        } else {
                            if (m_isCosmics) {
                                RtRelation_B_x = LE - EP - t0;
                                RtRelation_B_y = fabs(locR);
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe), RtRelation_B_x, RtRelation_B_y);
                            } else {
                                RtRelation_B_x = LE - t0;
                                RtRelation_B_y = fabs(locR);
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe), RtRelation_B_x, RtRelation_B_y);
                            }
                        }
                    } else if (ibe == 1) {
                        if (isArgonStraw) {
                            if (m_isCosmics) {
                                RtRelation_E_Ar_x = LE - EP - t0;
                                RtRelation_E_Ar_y = fabs(locR);
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), RtRelation_E_Ar_x, RtRelation_E_Ar_y);
                            } else {
                                RtRelation_E_Ar_x = LE - t0;
                                RtRelation_E_Ar_y = fabs(locR);
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), RtRelation_E_Ar_x, RtRelation_E_Ar_y);
                            }
                        } else {
                            if (m_isCosmics) {
                                RtRelation_E_x = LE - EP - t0;
                                RtRelation_E_y = fabs(locR);
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), RtRelation_E_x, RtRelation_E_y);
                            } else {
                                RtRelation_E_x = LE - t0;
                                RtRelation_E_y = fabs(locR);
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), RtRelation_E_x, RtRelation_E_y);
                            }
                        }
                    }
                }

                const int trailingEdge = RawDriftCircle->trailingEdge();
                float trailingEdgeScaled = (trailingEdge + 1) * 3.125;

                if ((trailingEdge < 23) &&
                    !(RawDriftCircle->lastBinHigh()) &&
                    !(RawDriftCircle->firstBinHigh())) {
                        if (m_doExpert && m_doStraws) {
                            HitTronTMapS_x = thisStrawNumber[ibe];
                            HitTronTMapS_y = trailingEdgeScaled;
                            fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitTronTMapS_x, HitTronTMapS_y);
                        }
    
                        if (m_doExpert && m_doChips) {
                            HitTronTMapC_x = chip[ibe] - 1;
                            HitTronTMapC_y = trailingEdgeScaled;
                            fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitTronTMapC_x, HitTronTMapC_y);
                        }
    
                        if (m_doExpert && m_doStraws) {
                            HitTronTwEPCMapS_x = thisStrawNumber[ibe];
                            HitTronTwEPCMapS_y = trailingEdgeScaled - timeCor;
                            fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitTronTwEPCMapS_x, HitTronTwEPCMapS_y);
                        }
    
                        if (m_doExpert && m_doChips) {
                            HitTronTwEPCMapC_x = chip[ibe] - 1;
                            HitTronTwEPCMapC_y = trailingEdgeScaled - timeCor;
                            fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitTronTwEPCMapC_x, HitTronTwEPCMapC_y);
                        }
    
                        if (m_doShift && m_doStraws) {
                            if (RawDriftCircle->driftTimeValid()) {
                                if (ibe == 0) {
                                    if (isArgonStraw) {
                                        TronTDist_B_Ar = trailingEdgeScaled;
                                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe), TronTDist_B_Ar);
                                        AvgTroTDetPhi_B_Ar_x = phi2D[ibe];
                                        AvgTroTDetPhi_B_Ar_y = trailingEdgeScaled;
                                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe), AvgTroTDetPhi_B_Ar_x, AvgTroTDetPhi_B_Ar_y);
                                    } else {
                                        TronTDist_B_Ar = trailingEdgeScaled;
                                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe), TronTDist_B_Ar);
                                        AvgTroTDetPhi_B_x = phi2D[ibe];
                                        AvgTroTDetPhi_B_y = trailingEdgeScaled;
                                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe), AvgTroTDetPhi_B_x, AvgTroTDetPhi_B_y);
                                    }
                                } else if (ibe == 1) {
                                    if (isArgonStraw) {
                                        TronTDist_E_Ar = trailingEdgeScaled;
                                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), TronTDist_E_Ar);
                                        AvgTroTDetPhi_E_Ar_x = phi2D[ibe];
                                        AvgTroTDetPhi_E_Ar_y = trailingEdgeScaled;
                                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), AvgTroTDetPhi_E_Ar_x, AvgTroTDetPhi_E_Ar_y);
                                    } else {
                                        TronTDist_E = trailingEdgeScaled;
                                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), TronTDist_E);
                                        AvgTroTDetPhi_E_x = phi2D[ibe];
                                        AvgTroTDetPhi_E_y = trailingEdgeScaled;
                                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), AvgTroTDetPhi_E_x, AvgTroTDetPhi_E_y);
                                    }
                                }
                            }
                        }
                }
            }
        }

        // ToDo: work on the part below
        for (int ibe = 0; ibe < 2; ibe++) {
            for (int i = 0; i < 64; i++)
                if (trackfound[ibe][i])
                    ntrackstack[ibe][i]++;

            if (m_doShift) {
                if (ibe == 0) {
                    if (hitontrack[ibe] >= m_minTRThits) {
                        NumHoTDetPhi_B_x = phi2D[ibe];
                        NumHoTDetPhi_B_y = hitontrack[ibe];
                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe), NumHoTDetPhi_B_x, NumHoTDetPhi_B_y);
                    }
                }

                if (ibe == 1) {
                    if (hitontrack_E_side[0] >= m_minTRThits) {
                        NumHoTDetPhi_E_x = phi2D[ibe];
                        NumHoTDetPhi_E_y = hitontrack_E_side[0];
                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+"0", NumHoTDetPhi_E_x, NumHoTDetPhi_E_y);
                    }

                    if (hitontrack_E_side[1] >= m_minTRThits) {
                        NumHoTDetPhi_E_x = phi2D[ibe];
                        NumHoTDetPhi_E_y = hitontrack_E_side[1];
                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+"1", NumHoTDetPhi_E_x, NumHoTDetPhi_E_y);
                    }
                }
            }

            if (phi2D[ibe] < 0) continue;

            if (m_doShift) {
                if (ibe == 0) {
                    if (nTRTHitsW[ibe][0] + nTRTHitsW[ibe][1] > 0) {
                        NumTrksDetPhi_B = phi2D[ibe];
                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe), NumTrksDetPhi_B);
                    }
                } else if (ibe == 1) {
                    if (nTRTHitsW[ibe][0] > 0) {
                        NumTrksDetPhi_E = phi2D[ibe];
                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+"0", NumTrksDetPhi_E);
                    }

                    if (nTRTHitsW[ibe][1] > 0) {
                        NumTrksDetPhi_E = phi2D[ibe];
                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+"1", NumTrksDetPhi_E);
                    }
                }
            }

            if (ibe == 0) {
                if ((nTRTHitsW[ibe][0] + nTRTHitsW[ibe][1]) > 0) {
                }
                if (comTimeObject) {
                    if (m_doShift && (phi2D[ibe] > 0) && (std::fabs(timeCor) > 1e-8)) {
                        EvtPhaseDetPhi_B_x = phi2D[ibe];
                        EvtPhaseDetPhi_B_y = timeCor;
                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe), EvtPhaseDetPhi_B_x, EvtPhaseDetPhi_B_y);
                    }
                }
            } else if (ibe == 1) {
                for (int iside = 0; iside < 2; iside++) {
                    if (nTRTHitsW[ibe][iside] > 0) {
                    }
                    if (comTimeObject) {
                        if (nTRTHits_side[ibe][iside] > 5 && (std::fabs(timeCor)
                                                                > 1e-8)) {
                            if (m_doShift) {
                                EvtPhaseDetPhi_E_x = phi2D[ibe];
                                EvtPhaseDetPhi_E_y = timeCor;
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), EvtPhaseDetPhi_E_x, EvtPhaseDetPhi_E_y);
                            }
                        }
                    }
                }
            }
        }
    }

    if (comTimeObject) {
        if (std::fabs(timeCor) > 1e-8) {
            if (m_doShift) {
                EvtPhase = timeCor;
                fill("ShiftTRTTrackHistograms0", EvtPhase);
            }

            if (m_doShift && trigDecision) {
                std::vector<int> trigid;
                trigid.clear(); // Trigger ID
                // Get bits for trigger after veto
                std::vector<unsigned int> level1TAV = trigDecision->tav();

                for (unsigned int j = 0; j < 8 && j < level1TAV.size(); ++j) {
                    for (unsigned int i = 0; i < 32; ++i) {
                        if ((level1TAV[j] >> i) & 0x1) {
                            trigid.push_back(i + (j % 8) * 32); // Found the ID
                        }
                    }
                }

                for (unsigned int j = 0; j < trigid.size(); ++j) {
                    EvtPhaseVsTrig_x = timeCor;
                    EvtPhaseVsTrig_y = trigid[j];
                    fill("ShiftTRTTrackHistograms0", EvtPhaseVsTrig_x, EvtPhaseVsTrig_y);
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------------//
StatusCode TRTMonitoringRun3ESD_Alg::fillTRTHighThreshold(const xAOD::TrackParticleContainer& trackCollection,
                                                     const xAOD::EventInfo& eventInfo) const {
//----------------------------------------------------------------------------------//
    ATH_MSG_VERBOSE("Filling TRT Aging Histos");
    
    auto Trackr_HT  = Monitored::Scalar<float>("Trackr_HT", 0.0);
    auto Trackr_All = Monitored::Scalar<float>("Trackr_All", 0.0);
    auto Trackz_HT  = Monitored::Scalar<float>("Trackz_HT", 0.0);
    auto Trackz_All = Monitored::Scalar<float>("Trackz_All", 0.0);
  
    auto p_trk = trackCollection.begin();
    const Trk::Perigee *perigee = NULL;
    const DataVector<const Trk::TrackParameters> *AllTrkPar(0);
    DataVector<const Trk::TrackParameters>::const_iterator p_trkpariter;

    int runNumber;
    runNumber = eventInfo.runNumber();

    for (; p_trk != trackCollection.end(); ++p_trk) {
        AllTrkPar = ((*p_trk)->track())->trackParameters();

        for (p_trkpariter = AllTrkPar->begin(); p_trkpariter != AllTrkPar->end(); ++p_trkpariter) {
            if ((perigee = dynamic_cast<const Trk::Perigee *>(*p_trkpariter))) break;
        }

        // If you went through all of the track parameters and found no perigee mearsurement
        // then something is wrong with the track and so don't use the track.
        // i.e. continue to the next track.
        if (!perigee) {
            ATH_MSG_DEBUG("No perigee mearsurement found for the track. This entry will not be propogated to aging histograms.");
            continue;
        }

        float track_eta  = perigee->eta();
        float track_p    = (perigee->parameters()[Trk::qOverP] != 0.) ? fabs(1. / (perigee->parameters()[Trk::qOverP])) : 10e7;
        const DataVector<const Trk::TrackStateOnSurface> *trackStates = ((*p_trk)->track())->trackStateOnSurfaces();

        if (trackStates == 0) continue;

        DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItBegin     = trackStates->begin();
        DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItEnd       = trackStates->end();
        uint8_t tempHitsVariable(0);
        (*p_trk)->summaryValue(tempHitsVariable, xAOD::SummaryType::numberOfTRTHits);
        int trt_hits = unsigned(tempHitsVariable);
        (*p_trk)->summaryValue(tempHitsVariable, xAOD::SummaryType::numberOfSCTHits);
        int sct_hits = unsigned(tempHitsVariable);
        (*p_trk)->summaryValue(tempHitsVariable, xAOD::SummaryType::numberOfPixelHits);
        int pixel_hits = unsigned(tempHitsVariable);

        if (fabs(track_eta) > 2.5) continue;
        if (fabs(track_p) < 5000.) continue;
        if (pixel_hits < 1.) continue;
        if (sct_hits < 6.) continue;
        if (trt_hits < 6.) continue;

        // Now we have hit informations
        const DataVector<const Trk::TrackStateOnSurface> *track_states = ((*p_trk)->track())->trackStateOnSurfaces();

        if (track_states) {
            ATH_MSG_DEBUG("This track has " << track_states->size() << " track states on surface.");
        } else {
            ATH_MSG_DEBUG("This track has null track states on surface.");
            continue;
        }

        int barrel_ec_side = 0;
        int layer_or_wheel = 0;
        int phi_module     = 0;
        int straw_layer    = 0;

        for (; TSOSItBegin != TSOSItEnd; ++TSOSItBegin) {
            if ((*TSOSItBegin) == 0) continue;
            if ( !((*TSOSItBegin)->type(Trk::TrackStateOnSurface::Measurement)) ) continue;

            const InDet::TRT_DriftCircleOnTrack *trtCircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>((*TSOSItBegin)->measurementOnTrack());
            const Trk::TrackParameters *aTrackParam = dynamic_cast<const Trk::TrackParameters *>((*TSOSItBegin)->trackParameters());

            if (!trtCircle) continue;
            if (!aTrackParam) continue;


            Identifier DCoTId = trtCircle->identify();
            barrel_ec_side  = m_pTRTHelper->barrel_ec(DCoTId);
            layer_or_wheel  = m_pTRTHelper->layer_or_wheel(DCoTId);
            phi_module      = m_pTRTHelper->phi_module(DCoTId);
            straw_layer     = m_pTRTHelper->straw_layer(DCoTId);
            int Ba_Ec = abs(barrel_ec_side) - 1;               // Ba_Ec:  0 is barrel, 1 is Endcap
            int Side  = barrel_ec_side > 0 ? 0 : 1;            // Side :  0 is side_A, 1 is side_C
            double xPos = trtCircle->globalPosition().x();     // Global x coordinate
            double yPos = trtCircle->globalPosition().y();     // Global y coordinate
            double zPos = trtCircle->globalPosition().z();     // Global z coordinate
            double RPos = sqrt(xPos * xPos + yPos * yPos);
            Identifier surfaceID;
            surfaceID = trtCircle->identify();
            // Assume always Xe if m_ArgonXenonSplitter is not enabled, otherwise check the straw status (good is Xe, non-good is Ar)
            const InDet::TRT_DriftCircle *RawDriftCircle = dynamic_cast<const InDet::TRT_DriftCircle *>(trtCircle->prepRawData());

            if (!RawDriftCircle) { //coverity 25097
                // This shouldn't happen in normal conditions because trtCircle is a TRT_DriftCircleOnTrack object
                ATH_MSG_WARNING("RawDriftCircle object returned null");
                continue;
            }

            int middleHTbit       = RawDriftCircle->getWord() & 0x00020000;
            //0x00020000 = 0000 0000 0000 0000 0000 0010 0000 0000 0000 0000
            bool is_middleHTbit_high   = (middleHTbit != 0);
            // bool isHighLevel= RawDriftCircle->highLevel();
            bool isHighLevel = is_middleHTbit_high; // Hardcoded HT Middle Bit
            bool shortStraw = false;
            int InputBar = 0;

            if (fabs(track_eta) < 2. && Ba_Ec == 0.) {
                if ((layer_or_wheel == 0) && (phi_module < 4 || (phi_module > 7 && phi_module < 12) || (phi_module > 15 && phi_module < 20) || (phi_module > 23 && phi_module < 28))) InputBar = 1;
                else if ((runNumber >= 296939) && (layer_or_wheel == 0) && (phi_module > 27)) InputBar = 1;
                else if (layer_or_wheel == 0)
                    InputBar = 0;
                else if ((layer_or_wheel == 1) && ((phi_module > 1 && phi_module < 6) || (phi_module > 9 && phi_module < 14) || (phi_module > 17 && phi_module < 22) || (phi_module > 25 && phi_module < 30)))
                    InputBar = 1;
                else if (layer_or_wheel == 1)
                    InputBar = 0;
                else if (layer_or_wheel == 2 && phi_module % 2 != 0)
                    InputBar = 1;
                else if (layer_or_wheel == 2)
                    InputBar = 0;
                else {
                    ATH_MSG_WARNING("Should not pass here");
                    continue;
                }

                if ((layer_or_wheel == 0) && straw_layer < 9.)
                    shortStraw = true;
            }

            // Fill Barrel Plots
            if ((!shortStraw) && (Ba_Ec == 0)) {
                Trackz_All = zPos;
                fill("TRTAgingHistograms0"+std::to_string(layer_or_wheel)+std::to_string(InputBar), Trackz_All);
                if (isHighLevel) {
                    Trackz_HT = zPos;
                fill("TRTAgingHistograms"+std::to_string(Ba_Ec)+std::to_string(layer_or_wheel)+std::to_string(InputBar), Trackz_HT);
                }
            }

            if (shortStraw) {
                if (zPos > 0.) {
                    Trackz_All = zPos;
                    fill("TRTAgingHistograms"+std::to_string(Ba_Ec)+"3"+std::to_string(InputBar), Trackz_All);
                    if (isHighLevel) {
                        Trackz_HT = zPos;
                        fill("TRTAgingHistograms"+std::to_string(Ba_Ec)+"3"+std::to_string(InputBar), Trackz_HT);
                    }
                } else {
                    Trackz_All = zPos;
                    fill("TRTAgingHistograms"+std::to_string(Ba_Ec)+"4"+std::to_string(InputBar), Trackz_All);

                    if (isHighLevel) {
                        Trackz_HT = zPos;
                        fill("TRTAgingHistograms"+std::to_string(Ba_Ec)+"4"+std::to_string(InputBar), Trackz_HT);
                    }
                }
            }

            // End of Barrel plots, moving to Endcap plots
            int WType = -1;

            if ((Ba_Ec == 1) && (layer_or_wheel < 6) &&
                ((straw_layer > 3 && straw_layer < 8) ||
                 (straw_layer > 11))) {
                WType = 0;
            }
            if ((Ba_Ec == 1) && (layer_or_wheel >= 6) &&
                (straw_layer > 3)) {
                WType = 3;
            }
            if ((Ba_Ec == 1) && (layer_or_wheel < 6) &&
                ((straw_layer > -1 && straw_layer < 4) ||
                 (straw_layer > 7 && straw_layer < 12))) {
                WType = 2;
            }
            if ((Ba_Ec == 1) && (layer_or_wheel >= 6) &&
                ((straw_layer > -1 && straw_layer < 4))) {
                WType = 1;
            }

            if (WType < 0 && Ba_Ec == 1) { // Coverity CID 25096
                ATH_MSG_WARNING("The variable  \"WType\" is less than zero!.");
                continue;
            }

            if (Ba_Ec == 1) {
                Trackr_All = RPos;
                fill("TRTAgingHistograms"+std::to_string(Ba_Ec)+std::to_string(WType)+std::to_string(Side), Trackr_All);
                if (isHighLevel) {
                    Trackr_HT = RPos;
                    fill("TRTAgingHistograms"+std::to_string(Ba_Ec)+std::to_string(WType)+std::to_string(Side), Trackr_HT);
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}


StatusCode TRTMonitoringRun3ESD_Alg::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;
    bool passEventBurst = true;

    // Declare the quantities which should be monitored

    // Set the values of the monitored variables for the event

    ATH_MSG_VERBOSE("Monitoring Histograms being filled");

    SG::ReadHandle<xAOD::TrackParticleContainer> trackCollection(m_trackCollectionKey, ctx);
    SG::ReadHandle<xAOD::EventInfo>     xAODEventInfo(m_xAODEventInfoKey, ctx);
    SG::ReadHandle<InDetTimeCollection> trtBCIDCollection(m_TRT_BCIDCollectionKey, ctx);
    SG::ReadHandle<ComTime>             comTimeObject(m_comTimeObjectKey, ctx);
    SG::ReadHandle<xAOD::TrigDecision>  trigDecision(m_trigDecisionKey, ctx);
    
    if (!xAODEventInfo.isValid()) {
        ATH_MSG_ERROR("Could not find event info object " << m_xAODEventInfoKey.key() <<
                      " in store");
        return StatusCode::FAILURE;
    }

    if (m_doTracksMon) {
        if (!trackCollection.isValid()) {
            ATH_MSG_ERROR("Could not find track collection " << m_trackCollectionKey.key() <<
                          " in store");
            return StatusCode::FAILURE;
        }
        if (!trigDecision.isValid()) {
            ATH_MSG_INFO("Could not find trigger decision object " << m_trigDecisionKey.key() <<
                          " in store");
        }
        // NOTE: failing to retrieve ComTime from store for some reason
        if (!comTimeObject.isValid()) {
            ATH_MSG_INFO("Could not find com time object " << m_comTimeObjectKey.key() <<
                         " in store");
        }
        ATH_CHECK( fillTRTTracks(*trackCollection, trigDecision.cptr(), comTimeObject.cptr()) );
    }

    if (!m_doTracksMon) {
        if (!trackCollection.isValid()) {
            ATH_MSG_ERROR("Could not find track collection " << m_trackCollectionKey.key() <<
                          " in store");
            return StatusCode::FAILURE;
        }
    }

    if (passEventBurst) { // ESD files does not have an RDO container to pass event burst, what to do?
		ATH_CHECK( fillTRTHighThreshold(*trackCollection, *xAODEventInfo) );
	}
    


    return StatusCode::SUCCESS;
}
