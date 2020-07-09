/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTMonitoringRun3/TRTMonitoringRun3RAW_Alg.h"

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
const int TRTMonitoringRun3RAW_Alg::s_numberOfBarrelStacks = 32;
const int TRTMonitoringRun3RAW_Alg::s_numberOfEndCapStacks = 32;
const int TRTMonitoringRun3RAW_Alg::s_Straw_max[2] = {1642, 3840};

TRTMonitoringRun3RAW_Alg::TRTMonitoringRun3RAW_Alg( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_idHelper(0)
,m_pTRTHelper(0)
,m_mgr(0)
,m_minTRThits(10)
,m_minP(0)
,m_sumTool("TRT_StrawStatusSummaryTool", this)
,m_isCosmics(false)
,m_EventBurstCut(-1)
{
    declareProperty("InDetTRTStrawStatusSummaryTool", m_sumTool);
    declareProperty("doStraws",                       m_doStraws         = true);
    declareProperty("doExpert",                       m_doExpert         = true);
    declareProperty("doChips",                        m_doChips          = true);
    declareProperty("doRDOsMon",                      m_doRDOsMon        = true);
    declareProperty("doShift",                        m_doShift          = true);
    declareProperty("doEfficiency",                   m_doEfficiency     = true);
    declareProperty("doMaskStraws",                   m_doMaskStraws     = true);
    declareProperty("useHoleFinder",                  m_useHoleFinder    = true);
    declareProperty("DoHitsMon",                      m_doHitsMon        = true);
    declareProperty("DistanceToStraw",                m_DistToStraw      = 0.4); 
    declareProperty("min_si_hits",                    m_min_si_hits      = 1);
    declareProperty("min_pixel_hits",                 m_min_pixel_hits   = 0);
    declareProperty("min_sct_hits",                   m_min_sct_hits     = 0);
    declareProperty("min_trt_hits",                   m_min_trt_hits     = 10);
    declareProperty("MinTRTHitCut",                   m_minTRThits       = 10);
    declareProperty("every_xth_track",                m_every_xth_track  = 1);
    declareProperty("max_abs_eta",                    m_max_abs_eta      = 2.5);
    declareProperty("max_abs_d0",                     m_max_abs_d0       = 10  * CLHEP::mm);
    declareProperty("max_abs_z0",                     m_max_abs_z0       = 300 * CLHEP::mm);
    declareProperty("MinTrackP",                      m_minP             = 0.0 * CLHEP::GeV);
    declareProperty("min_pT",                         m_min_pT           = 0.5 * CLHEP::GeV);
}
 
 
TRTMonitoringRun3RAW_Alg::~TRTMonitoringRun3RAW_Alg() {}

StatusCode TRTMonitoringRun3RAW_Alg::initialize() {
    using namespace Monitored;
    
    ATH_MSG_VERBOSE("Initializing TRT Monitoring");

    // initialize superclass
    ATH_CHECK( AthMonitorAlgorithm::initialize() );
    
    
    // Retrieve detector manager.
    ATH_CHECK( detStore()->retrieve(m_mgr, "TRT") );
    // Get ID helper for TRT to access various detector components like straw, straw_layer, layer_or_wheel, phi_module, etc...
    ATH_CHECK( detStore()->retrieve(m_pTRTHelper, "TRT_ID") );
    ATH_CHECK( detStore()->retrieve(m_idHelper, "AtlasID") );
    
    if (m_doExpert) {
        // Retrieve the TRT_Straw Status Service.
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
    }//If do expert

    // Get Track summary tool
    if (m_TrackSummaryTool.retrieve().isFailure())
        ATH_MSG_ERROR("Cannot get TrackSummaryTool");
    else
        ATH_MSG_DEBUG("Retrieved succesfully the track summary tool" << m_TrackSummaryTool);

    // Retrieve TRTTrackHoleSearchTool
	if (m_doEfficiency || m_doExpert) {
	  ATH_CHECK( m_trt_hole_finder.retrieve() );
	}
	else {
	  m_trt_hole_finder.disable();
	}

    // Initialization of VarHandleKeys
    ATH_CHECK( m_rdoContainerKey.initialize() );
    ATH_CHECK( m_xAODEventInfoKey.initialize() );
    ATH_CHECK( m_TRT_BCIDCollectionKey.initialize() );
    ATH_CHECK( m_combTrackCollectionKey.initialize() );
    ATH_CHECK( m_trackCollectionKey.initialize() );
    ATH_CHECK( m_bsErrContKey.initialize(SG::AllowEmpty) );

    return StatusCode::SUCCESS;
}


//-------------------------------------------------------------------------------------------------//
StatusCode TRTMonitoringRun3RAW_Alg::checkTRTReadoutIntegrity(const xAOD::EventInfo& eventInfo) const {
//-------------------------------------------------------------------------------------------------//
    StatusCode sc = StatusCode::SUCCESS;

    const TRT_BSErrContainer emptyErrCont;//Empty dummy instance for MC
    const TRT_BSErrContainer* bsErrCont=&emptyErrCont;
	
    if (!m_bsErrContKey.empty()) { 
      //Regular real-data case, get the byte-stream errors from SG
      SG::ReadHandle<TRT_BSErrContainer> bsErrContHdl{m_bsErrContKey};
      bsErrCont=bsErrContHdl.cptr();
    }
    else {
      ATH_MSG_DEBUG("MC case, using dummy TRT_BSErrContainer");
    }

    const unsigned int lumiBlock = eventInfo.lumiBlock();
    ATH_MSG_VERBOSE("This is lumiblock : " << lumiBlock);

//    if ((int)lumiBlock != m_lastLumiBlock) {
//        m_lastLumiBlock = lumiBlock;
//    }

    //Get BSConversion errors
    const std::set<std::pair<uint32_t, uint32_t> > &L1IDErrorSet      = bsErrCont->getL1ErrorSet();
    const std::set<std::pair<uint32_t, uint32_t> > &BCIDErrorSet      = bsErrCont->getBCIDErrorSet();
    const std::set<uint32_t>                       &MissingErrorSet   = bsErrCont->getMissingErrorSet();
    const std::set<uint32_t>                       &SidErrorSet       = bsErrCont->getSidErrorSet();
    const std::set<std::pair<uint32_t, uint32_t> > &RobStatusErrorSet = bsErrCont->getRobErrorSet();
    const unsigned int rod_id_base[2][2] = { { 0x310000, 0x320000 }, { 0x330000, 0x340000 } };
    const unsigned int nChipsTotal[2][2] = { {     3328,     3328 }, {     7680,     7680 } };
    const unsigned int nRobsTotal[2][2]  = { {       32,       32 }, {       64,       64 } };
    float nBSErrors[2][2]  = { { 0, 0 }, { 0, 0 } };
    float nRobErrors[2][2] = { { 0, 0 }, { 0, 0 } };
    const std::set<std::pair<uint32_t, uint32_t> > *errorset1[2] = { &BCIDErrorSet, &L1IDErrorSet };

    for (int iset = 0; iset < 2; ++iset) {
        for (auto setIt = errorset1[iset]->begin(); setIt != errorset1[iset]->end(); ++setIt) {
            for (int ibe = 0; ibe < 2; ++ibe) {
                for (int iside = 0; iside < 2; ++iside) {
                    if (((setIt->first >> 8) & 0xFF0000) == rod_id_base[ibe][iside]) {
                        nBSErrors[ibe][iside] += 1. / nChipsTotal[ibe][iside];
                    }
                }
            }
        }
    }

    const std::set<uint32_t> *errorset2[2] = { &MissingErrorSet, &SidErrorSet };

    for (int iset = 0; iset < 2; ++iset) {
        for (auto setIt = errorset2[iset]->begin(); setIt != errorset2[iset]->end(); ++setIt) {
            for (int ibe = 0; ibe < 2; ++ibe) {
                for (int iside = 0; iside < 2; ++iside) {
                    if (((*setIt >> 8) & 0xFF0000) == rod_id_base[ibe][iside]) {
                        nBSErrors[ibe][iside] += 1. / nChipsTotal[ibe][iside];
                    }
                }
            }
        }
    }

    for (int ibe = 0; ibe < 2; ++ibe) {
        for (int iside = 0; iside < 2; ++iside) {
//            m_hChipBSErrorsVsLB[ibe][iside]->Fill(lumiBlock, nBSErrors[ibe][iside]);
//            m_hChipBSErrorsVsLB[ibe][iside]->SetEntries(lumiBlock); // we need this so the LastBinThreshold algorithm can find the last bin
        }
    }

    for (auto setIt = RobStatusErrorSet.begin(); setIt != RobStatusErrorSet.end(); ++setIt) {
        for (int ibe = 0; ibe < 2; ++ibe) {
            for (int iside = 0; iside < 2; ++iside) {
                if (setIt->first % rod_id_base[ibe][iside] < 0xffff) {
                    nRobErrors[ibe][iside] += 1. / nRobsTotal[ibe][iside];
                }
            }
        }
    }

    for (int ibe = 0; ibe < 2; ++ibe) {
        for (int iside = 0; iside < 2; ++iside) {
//            m_hRobBSErrorsVsLB[ibe][iside]->Fill(lumiBlock, nRobErrors[ibe][iside]);
//            m_hRobBSErrorsVsLB[ibe][iside]->SetEntries(lumiBlock); // we need this so the LastBinThreshold algorithm can find the last bin
        }
    }

    return sc;
}


//----------------------------------------------------------------------------------//
int TRTMonitoringRun3RAW_Alg::chipToBoard(int chip) const {
//----------------------------------------------------------------------------------//
    // return logical board index:
    // 0 for Board 1S (has 10 chips)  0 -  9
    // 1 for 1L (11)                 10 - 20
    // 2 for 2S (15)                 21 - 35
    // 3 for 2L, first 9 chips       36 - 44
    // 4 for 2L, second 9 chips      45 - 53
    // 5 for 3S, first 11            54 - 64
    // 6 for 3S, second 12           65 - 76
    // 7 for 3L, first 13            77 - 89
    // 8 for 3L, second 14           90 - 103
    const int list[] = {10, 11, 15, 9, 9, 11, 12, 13, 14};
    int count = 0;
    chip--;

    for (int i = 0; i < 9; i++) {
        count += list[i];

        if (chip < count) return i + 1;
        else if (chip == 104) return 9;
    }

    assert(count == 104);
    assert(false); // should never come this far
    return -1;
}


//----------------------------------------------------------------------------------//
int TRTMonitoringRun3RAW_Alg::chipToBoard_EndCap(int chip) const {
//----------------------------------------------------------------------------------//
    const int remainder = (chip - 1) % 12;
    const int Board = int(((chip - 1) - remainder) / 12);
    return Board + 1;
}



// Check for EventBurst: Counts high level hits and returns true if the count is less than m_passEventBurstCut.
// If m_EventBurstCut is less than zero, returns allways true
//----------------------------------------------------------------------------------//
bool TRTMonitoringRun3RAW_Alg::checkEventBurst(const TRT_RDO_Container& rdoContainer) const {
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



// Returns the straw number (0-1641) given the layerNumber, strawlayerNumber, and strawNumber, all gotten from Athena IDHelper
//----------------------------------------------------------------------------------//
int TRTMonitoringRun3RAW_Alg::strawNumber(int strawNumber, int strawlayerNumber, int LayerNumber) const {
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
int TRTMonitoringRun3RAW_Alg::strawNumberEndCap(int strawNumber, int strawLayerNumber, int LayerNumber, int phi_stack, int side) const {
//----------------------------------------------------------------------------------//
    // Before perfoming map, corrections need to be perfomed.
    //  apply special rotations for endcap mappings
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
            //Change straw number from 0-23 in straw layer to 0-192
            if (strawLayerNumber < 8)strawNumber = strawNumber + 24 * strawLayerNumber;

            if (strawLayerNumber > 7)strawNumber = strawNumber + 24 * (strawLayerNumber - 8);

            strawNumber = (192 - 1) * TripletOrientation[side][phi1] + strawNumber * (1 - 2 * TripletOrientation[side][phi1]); //actual rotation

            //take strawNumber back to 0-23
            if (strawLayerNumber < 8) strawLayerNumber = int(strawNumber / 24);

            if (strawLayerNumber > 7) strawLayerNumber = int(strawNumber / 24) + 8;

            strawNumber = strawNumber % 24;
        }

        //Finish rotation
        //Flip straw in layer.

        if (side == 0) strawNumber = 23 - strawNumber;

        //Finish Flipping
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
int TRTMonitoringRun3RAW_Alg::strawLayerNumber(int strawLayerNumber, int LayerNumber) const{
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
float TRTMonitoringRun3RAW_Alg::radToDegrees(float radValue) const{
//----------------------------------------------------------------------------------//
	float degreeValue = radValue / M_PI * 180;

	if (degreeValue < 0) degreeValue += 360;

	return degreeValue;
}


//Fill the TRT RDO Histograms
//----------------------------------------------------------------------------------//
StatusCode TRTMonitoringRun3RAW_Alg::fillTRTRDOs(const TRT_RDO_Container& rdoContainer,
                                            const xAOD::EventInfo& eventInfo,
                                            const InDetTimeCollection* trtBCIDCollection) const {
//----------------------------------------------------------------------------------//
    ATH_MSG_DEBUG("Filling TRT RDO Histograms");
    
    // TProfile
    auto HitTrWMapS_x         = Monitored::Scalar<float>("HitTrWMapS_x", 0.0);
    auto HitTrWMapS_y         = Monitored::Scalar<float>("HitTrWMapS_y", 0.0);
    auto HitTrWMapC_x         = Monitored::Scalar<float>("HitTrWMapC_x", 0.0);
    auto HitTrWMapC_y         = Monitored::Scalar<float>("HitTrWMapC_y", 0.0);     
    auto HitTrMapC_x          = Monitored::Scalar<float>("HitTrMapC_x", 0.0);
    auto HitTrMapC_y          = Monitored::Scalar<float>("HitTrMapC_y", 0.0);
    auto HitToTLongTrMapS_x   = Monitored::Scalar<float>("HitToTLongTrMapS_x", 0.0);
    auto HitToTLongTrMapS_y   = Monitored::Scalar<float>("HitToTLongTrMapS_y", 0.0);
    auto HitToTMapC_x         = Monitored::Scalar<float>("HitToTMapC_x", 0.0);
    auto HitToTMapC_y         = Monitored::Scalar<float>("HitToTMapC_y", 0.0);
    auto HitTrMapS_x          = Monitored::Scalar<float>("HitTrMapS_x", 0.0);
    auto HitTrMapS_y          = Monitored::Scalar<float>("HitTrMapS_y", 0.0);
    auto HitToTLongMapS_x     = Monitored::Scalar<float>("HitToTLongMapS_x", 0.0);
    auto HitToTLongMapS_y     = Monitored::Scalar<float>("HitToTLongMapS_y", 0.0);
    auto HitToTMapS_x         = Monitored::Scalar<float>("HitToTMapS_x", 0.0);
    auto HitToTMapS_y         = Monitored::Scalar<float>("HitToTMapS_y", 0.0);
    auto BCIDvsOcc_x          = Monitored::Scalar<float>("BCIDvsOcc_x", 0.0);
    auto BCIDvsOcc_y          = Monitored::Scalar<float>("BCIDvsOcc_y", 0.0);
    auto AvgHLOcc_side_x      = Monitored::Scalar<float>("AvgHLOcc_side_x", 0.0);
    auto AvgHLOcc_side_y      = Monitored::Scalar<float>("AvgHLOcc_side_y", 0.0);
    auto AvgLLOcc_side_x      = Monitored::Scalar<float>("AvgLLOcc_side_x", 0.0);
    auto AvgLLOcc_side_y      = Monitored::Scalar<float>("AvgLLOcc_side_y", 0.0);
    auto AvgLLOccMod_side_x   = Monitored::Scalar<float>("AvgLLOccMod_side_x", 0.0);
    auto AvgLLOccMod_side_y   = Monitored::Scalar<float>("AvgLLOccMod_side_y", 0.0);
    auto AvgHLOccMod_side_x   = Monitored::Scalar<float>("AvgHLOccMod_side_x", 0.0);
    auto AvgHLOccMod_side_y   = Monitored::Scalar<float>("AvgHLOccMod_side_y", 0.0);
    auto ChipBSErrorsVsLB_x   = Monitored::Scalar<float>("ChipBSErrorsVsLB_x", 0.0);
    auto ChipBSErrorsVsLB_y   = Monitored::Scalar<float>("ChipBSErrorsVsLB_y", 0.0);
    auto RobBSErrorsVsLB_x    = Monitored::Scalar<float>("RobBSErrorsVsLB_x", 0.0);
    auto RobBSErrorsVsLB_y    = Monitored::Scalar<float>("RobBSErrorsVsLB_y", 0.0);
    
    // TH1F
    auto OccAll               = Monitored::Scalar<float>("OccAll", 0.0);
    auto Summary              = Monitored::Scalar<float>("Summary", 0.0);
    auto SummaryWeight        = Monitored::Scalar<float>("SummaryWeight", 0.0);
    
    // TH2F
    auto HtoBCMapC_x          = Monitored::Scalar<float>("HtoBCMapC_x", 0.0);
    auto HtoBCMapC_y          = Monitored::Scalar<float>("HtoBCMapC_y", 0.0);
    auto HtoBCMapB_x          = Monitored::Scalar<float>("HtoBCMapB_x", 0.0);
    auto HtoBCMapB_y          = Monitored::Scalar<float>("HtoBCMapB_y", 0.0);
    
    // TEfficiency
    auto HitWMapS             = Monitored::Scalar<float>("HitWMapS", 0.0);
    auto HitWMapS_passed      = Monitored::Scalar<bool>("HitWMapS_passed", false);
    auto HitAWMapS            = Monitored::Scalar<float>("HitAWMapS", 0.0);
    auto HitAWMapS_passed     = Monitored::Scalar<bool>("HitAWMapS_passed", false);
    auto HitAMapS             = Monitored::Scalar<float>("HitAMapS", 0.0);
    auto HitAMapS_passed      = Monitored::Scalar<bool>("HitAMapS_passed", false);
    auto HitHMapS             = Monitored::Scalar<float>("HitHMapS", 0.0);
    auto HitHMapS_passed      = Monitored::Scalar<bool>("HitHMapS_passed", false);
    auto HitHWMapS            = Monitored::Scalar<float>("HitHWMapS", 0.0);
    auto HitHWMapS_passed     = Monitored::Scalar<bool>("HitHWMapS_passed", false);
    auto HtoLMapS             = Monitored::Scalar<float>("HtoLMapS", 0.0);
    auto HtoLMapS_passed      = Monitored::Scalar<bool>("HtoLMapS_passed", false);
    auto HitWMapC             = Monitored::Scalar<float>("HitWMapC", 0.0);
    auto HitWMapC_passed      = Monitored::Scalar<bool>("HitWMapC_passed", false);
    auto HitAWMapC            = Monitored::Scalar<float>("HitAWMapC", 0.0);
    auto HitAWMapC_passed     = Monitored::Scalar<bool>("HitAWMapC_passed", false);
    auto HitAMapC             = Monitored::Scalar<float>("HitAMapC", 0.0);
    auto HitAMapC_passed      = Monitored::Scalar<bool>("HitAMapC_passed", false);
    auto HitWMap_A            = Monitored::Scalar<float>("HitWMap_A", 0.0);
    auto HitWMap_A_passed     = Monitored::Scalar<bool>("HitWMap_A_passed", false);
    auto HitWMap_C            = Monitored::Scalar<float>("HitWMap_C", 0.0);
    auto HitWMap_C_passed     = Monitored::Scalar<bool>("HitWMap_C_passed", false);
    auto HitWMap_Ar_A         = Monitored::Scalar<float>("HitWMap_Ar_A", 0.0);
    auto HitWMap_Ar_A_passed  = Monitored::Scalar<bool>("HitWMap_Ar_A_passed", false);
    auto HitWMap_Ar_C         = Monitored::Scalar<float>("HitWMap_Ar_C", 0.0);
    auto HitWMap_Ar_C_passed  = Monitored::Scalar<bool>("HitWMap_Ar_C_passed", false);
    auto HitHMapC             = Monitored::Scalar<float>("HitHMapC", 0.0);
    auto HitHMapC_passed      = Monitored::Scalar<bool>("HitHMapC_passed", false);
    auto HitHWMapC            = Monitored::Scalar<float>("HitHWMapC", 0.0);
    auto HitHWMapC_passed     = Monitored::Scalar<bool>("HitHWMapC_passed", false);
    auto HtoLMapC             = Monitored::Scalar<float>("HtoLMapC", 0.0);
    auto HtoLMapC_passed      = Monitored::Scalar<bool>("HtoLMapC_passed", false);
    auto HitWMap              = Monitored::Scalar<float>("HitWMap", 0.0);
    auto HitWMap_passed       = Monitored::Scalar<bool>("HitWMap_passed", false);
    auto HitWMap_Ar           = Monitored::Scalar<float>("HitWMap_Ar", 0.0);
    auto HitWMap_Ar_passed    = Monitored::Scalar<bool>("HitWMap_Ar_passed", false);
    auto OccupancyC           = Monitored::Scalar<float>("OccupancyC", 0.0);
    auto OccupancyC_passed    = Monitored::Scalar<bool>("OccupancyC_passed", false);
    auto IntLum               = Monitored::Scalar<float>("IntLum", 0.0);
    auto IntLum_passed        = Monitored::Scalar<bool>("IntLum_passed_passed", false);
    auto LBvsLum              = Monitored::Scalar<float>("LBvsLum", 0.0);
    auto LBvsLum_passed       = Monitored::Scalar<bool>("LBvsLum_passed", false);
    auto LBvsTime             = Monitored::Scalar<float>("LBvsTime", 0.0);
    auto LBvsTime_passed      = Monitored::Scalar<bool>("LBvsTime_passed", false);
    auto StrawOcc             = Monitored::Scalar<float>("StrawOcc", 0.0);
    auto StrawOcc_passed      = Monitored::Scalar<bool>("StrawOcc_passed", false);
    
    const unsigned int lumiBlock = eventInfo.lumiBlock();
    ATH_MSG_VERBOSE("This is lumiblock : " << lumiBlock);
    auto good_bcid = eventInfo.bcid();
    
    TRT_RDO_Container::const_iterator RDO_CollectionBegin = rdoContainer.begin();
    TRT_RDO_Container::const_iterator RDO_CollectionEnd   = rdoContainer.end();
    //Check readout Integrity of TRT
    ATH_CHECK( checkTRTReadoutIntegrity(eventInfo) );
    int numberOfStacks_b[2]; //Total stack number of barrel and endcap
    numberOfStacks_b[0] = s_numberOfBarrelStacks * 3;
    numberOfStacks_b[1] = s_numberOfEndCapStacks * 2;
    Identifier TRT_Identifier;
    int numberOfStrawsMod[3]; // For barrel(number if straw in module)
    numberOfStrawsMod[0] = 329;
    numberOfStrawsMod[1] = 520;
    numberOfStrawsMod[2] = 793;
    int numberOfStrawsWheel[2]; // For endcap
    numberOfStrawsWheel[0] = 2304; //6 layers (6*16=96) 96*24=2304 straws in wheel type A
    numberOfStrawsWheel[1] = 1536; //8 layers (8*8=64) 64*24=1536 straws in wheel type B
    int moduleHits_B[192];
    int moduleHits_E[128];
    int HLmoduleHits_B[192];
    int HLmoduleHits_E[128];
    

    for (int i = 0; i < 192; i++) {
        moduleHits_B[i] = 0;
        HLmoduleHits_B[i] = 0;
    }

    for (int i = 0; i < 128; i++) {
        moduleHits_E[i] = 0;
        HLmoduleHits_E[i] = 0;
    }

    int goodid_status = 0;
    int prev_bcid = 0;

    if (trtBCIDCollection) {
        InDetTimeCollection::const_iterator itrt_bcid = trtBCIDCollection->begin();

        while (goodid_status == 0 && itrt_bcid != trtBCIDCollection->end()) {

            const unsigned int trt_bcid = (*itrt_bcid).second;

            if (itrt_bcid > trtBCIDCollection->begin() && prev_bcid - trt_bcid == 0) {
                goodid_status = 1;
            } else if (itrt_bcid > trtBCIDCollection->begin() && prev_bcid - trt_bcid != 0) {
                ATH_MSG_WARNING("TRT BCID is not consistent.  TRT RODID is " <<
                                std::hex << (*itrt_bcid).first << " trt bcid from ROD is " <<
                                std::hex << trt_bcid);
            }

            prev_bcid = trt_bcid;
            ++itrt_bcid;
        }
    }

    // Test out the TRT_StrawStatusSummarySvc.
    if (!m_sumTool.name().empty() && m_doExpert) {
        ATH_MSG_VERBOSE("Trying " << m_sumTool << " isGood");
        ATH_MSG_VERBOSE("TRT_StrawStatusTool reports status = " << m_sumTool->getStatus(TRT_Identifier));
    }
    
    // ibe = 0 (Barrel), ibe = 1 (Endcap)
    int nTRTHits[2];
    for (int ibe = 0; ibe < 2; ibe++) {
        nTRTHits[ibe] = 0;

    // Insert here
    // Scaling for online environment
    
    }

    int nhitsall = 0;

    for (; RDO_CollectionBegin != RDO_CollectionEnd; ++RDO_CollectionBegin) {
        const InDetRawDataCollection<TRT_RDORawData> *TRT_Collection(*RDO_CollectionBegin);

        if (!TRT_Collection) continue;

        DataVector<TRT_RDORawData>::const_iterator p_rdo = TRT_Collection->begin();

        for (; p_rdo != TRT_Collection->end(); ++p_rdo) {
            int middleHTbit       = (*p_rdo)->getWord() & 0x00020000;
            //0x00020000 = 0000 0000 0000 0000 0000 0010 0000 0000 0000 0000
            int hitinvaliditygate = (*p_rdo)->getWord() & 0x000DFE80;
            //0x000DFE80 = 0000 0000 0000 0000 0000 1101 1111 1110 1000 0000 //
            bool is_middleHTbit_high   = (middleHTbit != 0);
            bool is_anybininVgate_high = (hitinvaliditygate != 0);
            TRT_Identifier = (*p_rdo)->identify();

            if (m_doMaskStraws && m_sumTool->get_status(TRT_Identifier)) continue;

            int barrel_ec = m_pTRTHelper->barrel_ec(TRT_Identifier);
            //ToDo: Check TRT_LoLumRawData object
            const TRT_LoLumRawData *p_lolum = dynamic_cast<const TRT_LoLumRawData *>(*p_rdo);

            if (!p_lolum) continue;

            nhitsall++;
            int ibe = abs(barrel_ec) - 1;
            int iside = barrel_ec > 0 ? 0 : 1;

            //if barrel_ec is outof range go to next measurement in rdo_collection
            if (ibe != 1 && ibe != 0) {
                ATH_MSG_DEBUG("TRT part retrieved from TRT Identifier is not a barrel or an endcap");
                continue;
            }

            int moduleNumber_barrel1[2];
            int moduleNumber_barrel2[2];
            int moduleNumber_barrel3[2];
            int moduleNumber_endcapA[2];
            int moduleNumber_endcapB[2];
            // Get TRT Identifier
            // Need to know phi module, module layer, straw layer, and straw # within the layer
            // To get proper straw numbering
            TRT_Identifier = p_lolum->identify();
            //inline function checks m_ArgonXenonSplitter
            const bool isArgonStraw = (Straw_Gastype( m_sumTool->getStatusHT(TRT_Identifier) ) == GasType::Ar);
            int phi_module     = m_pTRTHelper->phi_module(TRT_Identifier);
            int layer_or_wheel = m_pTRTHelper->layer_or_wheel(TRT_Identifier);
            int straw_layer    = m_pTRTHelper->straw_layer(TRT_Identifier);
            int straw          = m_pTRTHelper->straw(TRT_Identifier);
            int thisStrawNumber;
            int chip = 0;
            int board = -1;
            //ToDo: Check if that is really neccessary
            bool is_barrel = m_pTRTHelper->is_barrel(TRT_Identifier);

            //check straw number and find the correct chip and m_ board values
            if ( is_barrel && ibe == 0 ) {
                thisStrawNumber = strawNumber(straw, straw_layer, layer_or_wheel);

                if (thisStrawNumber >= 0 && thisStrawNumber < s_Straw_max[ibe]) {
                    chip = m_mat_chip_B[phi_module][thisStrawNumber];
                }

                board = chipToBoard(chip);

            } else if ( !is_barrel && ibe == 1 ) {
                thisStrawNumber = strawNumberEndCap(straw, straw_layer, layer_or_wheel, phi_module, barrel_ec);

                if (thisStrawNumber >= 0 && thisStrawNumber < s_Straw_max[ibe]) {
                    chip = m_mat_chip_E[phi_module][thisStrawNumber];
                }

                board = chipToBoard_EndCap(chip);
            } else {
                thisStrawNumber = -1;
            }

            if (thisStrawNumber < 0 || thisStrawNumber >= s_Straw_max[ibe]) {
                ATH_MSG_WARNING("Found m_strawNumber = " << thisStrawNumber << " out of range.");
                continue;
            }
            const int driftTimeBin  = p_lolum->driftTimeBin();
            const int trailingEdge  = p_lolum->trailingEdge();
            const bool highlevel    = is_middleHTbit_high;//Hardcoded Middle Bit
            const bool firstBinHigh = p_lolum->firstBinHigh(); // if the first time bin is up then the hit is out of time window
            const bool lastBinHigh  = p_lolum->lastBinHigh(); // if the last bin is up then the hit is out of time window.
            const float timeOverThreshold = p_lolum->timeOverThreshold();
            moduleNumber_barrel1[0] = phi_module;
            moduleNumber_barrel1[1] = phi_module + 96;
            moduleNumber_barrel2[0] = phi_module + s_numberOfBarrelStacks;
            moduleNumber_barrel2[1] = phi_module + s_numberOfBarrelStacks + 96;
            moduleNumber_barrel3[0] = phi_module + 2 * s_numberOfBarrelStacks;
            moduleNumber_barrel3[1] = phi_module + 2 * s_numberOfBarrelStacks + 96;
            moduleNumber_endcapA[0] = phi_module;
            moduleNumber_endcapA[1] = phi_module + 64;
            moduleNumber_endcapB[0] = phi_module + s_numberOfEndCapStacks;
            moduleNumber_endcapB[1] = phi_module + s_numberOfEndCapStacks + 64;
            int iphi_module = -999;

            if (iside == 0) {
                iphi_module = phi_module;
            } else if (iside == 1) {
                iphi_module = phi_module + 32;
            }
            if (ibe == 0) {
                nTRTHits[ibe]++;

                if (m_doStraws) {
                    if (isArgonStraw) {
                        HitWMap_Ar = thisStrawNumber;
                        fill("RDOHistograms0", HitWMap_Ar_passed, HitWMap_Ar);
                    } else {
                        HitWMap = thisStrawNumber;
                        fill("RDOHistograms0", HitWMap_passed, HitWMap);
                    }
                }

                if (m_doShift) {
//                    m_nHitsperLB_B++;

                    if (highlevel) {
//                        m_nHLHitsperLB_B++;
                    }
                }
            } else if (ibe == 1) {
                nTRTHits[ibe]++;

                if (m_doStraws) {
                    if (isArgonStraw) {
                        HitWMap_Ar = thisStrawNumber;
                        fill("RDOHistograms1"+std::to_string(iside), HitWMap_Ar_passed, HitWMap_Ar);
                    } else {
                        HitWMap = thisStrawNumber;
                        fill("RDOHistograms1"+std::to_string(iside), HitWMap_passed, HitWMap);
                    }
                }

                if (m_doShift) {
//                    m_nHitsperLB_E[iside]++;

                    if (highlevel) {
//                        m_nHLHitsperLB_E[iside]++;
                    }
                }
            }

            if (m_doExpert) {
                if (m_doStraws) { // Experimental
                    HitWMapS = thisStrawNumber;
                    HitWMapS_passed = 0.0;
                    fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitWMapS_passed, HitWMapS);
                }
                if (m_doChips) { // Experimental
                    HitWMapC = chip - 1;
                    HitWMapC_passed = 0.0;
                    fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitWMapC_passed, HitWMapC);
                }
                if ( (driftTimeBin > 2) && (driftTimeBin < 17) ) {
                    if (m_doStraws) {
                        HitWMapS = thisStrawNumber;
                        HitWMapS_passed = 1.0;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitWMapS_passed, HitWMapS);
                    }
                    if (m_doChips) {
                        HitWMapC = chip - 1;
                        HitWMapC_passed = 1.0;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitWMapC_passed, HitWMapC);
                    }
                }

                float trailingEdgeScaled = (trailingEdge + 1)*3.125;

                if ((trailingEdge < 23) && !lastBinHigh && !firstBinHigh) {
                    if (m_doStraws) {
                        HitTrWMapS_x = thisStrawNumber;
                        HitTrWMapS_y = trailingEdgeScaled;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitTrWMapS_x, HitTrWMapS_y);
                    }
                    if (m_doChips) {
                        HitTrWMapC_x = chip - 1;
                        HitTrWMapC_y = trailingEdgeScaled;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitTrWMapC_x, HitTrWMapC_y);
                    }
                }
                if (m_doStraws) {
                    HitTrMapS_x = thisStrawNumber;
                    HitTrMapS_y = trailingEdgeScaled;
                    fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitTrMapS_x, HitTrMapS_y);
                }
                if (m_doChips) {
                        HitTrMapC_x = chip - 1;
                        HitTrMapC_y = trailingEdgeScaled;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitTrMapC_x, HitTrMapC_y);
                    }

                if (m_doStraws) { // Experimental
                    HitHMapS = thisStrawNumber;
                    HitHMapS_passed = 0.0;
                    fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHMapS_passed, HitHMapS);
                }
                if (m_doChips) { // Experimental
                    HitHMapC = chip - 1;
                    HitHMapC_passed = 0.0;
                    fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHMapC_passed, HitHMapC);
                }
                if (highlevel) {
                    if (m_doStraws) {
                        HitHMapS = thisStrawNumber;
                        HitHMapS_passed = 1.0;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHMapS_passed, HitHMapS);
                    }
                    if (m_doChips) {
                        HitHMapC = chip - 1;
                        HitHMapC_passed = 1.0;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHMapC_passed, HitHMapC);
                    }

                    if (m_doStraws) { // Experimental
                        HitHWMapS = thisStrawNumber;
                        HitHWMapS_passed = 0.0;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHWMapS_passed, HitHWMapS);
                    }
                    if (m_doChips) { // Experimental
                        HitHWMapC = chip - 1;
                        HitHWMapC_passed = 0.0;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHWMapC_passed, HitHWMapC);
                    }
                    if (is_middleHTbit_high) {
                        if (m_doStraws) {
                            HitHWMapS = thisStrawNumber;
                            HitHWMapS_passed = 1.0;
                            fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHWMapS_passed, HitHWMapS);
                        }
                    if (m_doChips) {
                        HitHWMapC = chip - 1;
                        HitHWMapC_passed = 1.0;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHWMapC_passed, HitHWMapC);
                    }
                    }
                }
                
                if (m_doStraws) { // Experimental
                    HitAMapS = thisStrawNumber;
                    HitAMapS_passed = 0.0;
                    fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitAMapS_passed, HitAMapS);
                }
                if (m_doChips) {// Experimental
                    HitAMapC = chip - 1;
                    HitAMapC_passed = 0.0;
                    fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitAMapC_passed, HitAMapC);
                }

                if (firstBinHigh || lastBinHigh || driftTimeBin > 0 || trailingEdge < 23) {
                    if (m_doStraws) {
                        HitAMapS = thisStrawNumber;
                        HitAMapS_passed = 1.0;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitAMapS_passed, HitAMapS);
                    }
                    if (m_doChips) {
                        HitAMapC = chip - 1;
                        HitAMapC_passed = 1.0;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitAMapC_passed, HitAMapC);
                    }
                }

                if (m_doStraws) {// Experimental
                    HitAWMapS = thisStrawNumber;
                    HitAWMapS_passed = 0.0;
                    fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitAWMapS_passed, HitAWMapS);
                }
                if (m_doChips) {// Experimental
                    HitAWMapC = chip - 1;
                    HitAWMapC_passed = 0.0;
                    fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitAWMapC_passed, HitAWMapC);
                }

                if ( is_anybininVgate_high) {
                    if (m_doStraws) {
                        HitAWMapS = thisStrawNumber;
HitAWMapS_passed = 1.0;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitAWMapS_passed, HitAWMapS);
                    }
                    if (m_doChips) {
                        HitAWMapC = chip - 1;
HitAWMapC_passed = 1.0;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitAWMapC_passed, HitAWMapC);
                    }
                }
        
                if (m_doStraws) {
                    HitToTMapS_x = thisStrawNumber;
                    HitToTMapS_y = timeOverThreshold;
                    fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitToTMapS_x, HitToTMapS_y);

                    if (timeOverThreshold > m_longToTCut) {
                        HitToTLongMapS_x = thisStrawNumber;
                        HitToTLongMapS_y = timeOverThreshold;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitToTLongMapS_x, HitToTLongMapS_y);
                        HitToTLongTrMapS_x = thisStrawNumber;
                        HitToTLongTrMapS_y = trailingEdgeScaled;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitToTLongTrMapS_x, HitToTLongTrMapS_y);
                    }
                }

                if (m_doChips) {
                    HitToTMapC_x = chip - 1;
                    HitToTMapC_y = timeOverThreshold;
                    fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitToTMapC_x, HitToTMapC_y);
                }

                if (m_doChips) {
                    if (p_lolum->highLevel(1)) {
                        HtoBCMapC_x = 0.;
                        HtoBCMapC_y = chip - 1;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HtoBCMapC_x, HtoBCMapC_y);
                        HtoBCMapB_x = 0.;
                        HtoBCMapB_y = board - 1;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HtoBCMapB_x, HtoBCMapB_y);
                    }

                    if (p_lolum->highLevel(2)) {
                        HtoBCMapC_x = 1.;
                        HtoBCMapC_y = chip - 1;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HtoBCMapC_x, HtoBCMapC_y);
                        HtoBCMapB_x = 1.;
                        HtoBCMapB_y = board - 1;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HtoBCMapB_x, HtoBCMapB_y);
                    }

                    if (p_lolum->highLevel(3)) {
                        HtoBCMapC_x = 2.;
                        HtoBCMapC_y = chip - 1;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HtoBCMapC_x, HtoBCMapC_y);
                        HtoBCMapB_x = 2.;
                        HtoBCMapB_y = board - 1;
                        fill("RDOStackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HtoBCMapB_x, HtoBCMapB_y);
                    }
                }
            }

            //Set Module Numbers.
            int moduleNumber = -1;

            if (ibe == 0) {
                if (layer_or_wheel == 0) {
                    moduleNumber = moduleNumber_barrel1[iside];
                    moduleHits_B[moduleNumber]++;
                } else if (layer_or_wheel == 1) {
                    moduleNumber = moduleNumber_barrel2[iside];
                    moduleHits_B[moduleNumber]++;
                } else if (layer_or_wheel == 2) {
                    moduleNumber = moduleNumber_barrel3[iside];
                    moduleHits_B[moduleNumber]++;
                }

                if (highlevel) {
                    if (layer_or_wheel == 0) {
                        moduleNumber = moduleNumber_barrel1[iside];
                        HLmoduleHits_B[moduleNumber]++;
                    } else if (layer_or_wheel == 1) {
                        moduleNumber = moduleNumber_barrel2[iside];
                        HLmoduleHits_B[moduleNumber]++;
                    } else if (layer_or_wheel == 2) {
                        moduleNumber = moduleNumber_barrel3[iside];
                        HLmoduleHits_B[moduleNumber]++;
                    }
                }
            } else if (ibe == 1) {
                if (layer_or_wheel < 6) {
                    moduleNumber = moduleNumber_endcapA[iside];
                    moduleHits_E[moduleNumber]++;
                } else if (layer_or_wheel > 5) {
                    moduleNumber = moduleNumber_endcapB[iside];
                    moduleHits_E[moduleNumber]++;
                }

                if (highlevel) {
                    if (layer_or_wheel < 6) {
                        moduleNumber = moduleNumber_endcapA[iside];
                        HLmoduleHits_E[moduleNumber]++;
                    } else if (layer_or_wheel > 5) {
                        moduleNumber = moduleNumber_endcapB[iside];
                        HLmoduleHits_E[moduleNumber]++;
                    }
                }
            }
        }
    }

    OccAll = nhitsall/350848.;
    fill("RDOHistograms0", OccAll);

    //ToDo Explain this
    for (int ibe = 0; ibe < 2; ibe++) {
        if (m_doShift) {
            if (ibe == 0) {
                BCIDvsOcc_x = good_bcid;
                BCIDvsOcc_y = nTRTHits[ibe]/105088.0;
                fill("RDOHistograms0", BCIDvsOcc_x, BCIDvsOcc_y);
            } else if (ibe == 1) {
                BCIDvsOcc_x = good_bcid;
                BCIDvsOcc_y = nTRTHits[ibe]/245760.0;
                fill("RDOHistograms1", BCIDvsOcc_x, BCIDvsOcc_y);
            }

            for (int iside = 0; iside < 2; iside++) {
                for (int i = 1; i <= numberOfStacks_b[ibe]; i++) {
                    int index_tmp = 0;
                    int modulenum_tmp = 0;

                    if (iside == 0) {
                        index_tmp = i - 1;
                        modulenum_tmp = i - 1;
                    } else if (iside == 1) {
                        index_tmp = i + 31;

                        if (ibe == 0) modulenum_tmp = (i - 1) + 96;
                        else if (ibe == 1) modulenum_tmp = (i - 1) + 64;
                    }

                    int nclass = -1;

                    if (i <= s_numberOfBarrelStacks) {
                        nclass = 0;
                    } else if (i <= 2 * s_numberOfBarrelStacks && i > s_numberOfBarrelStacks) {
                        nclass = 1;
                    } else if (i > 2 * s_numberOfBarrelStacks) {
                        nclass = 2;
                    }
                    int LLocc_index = index_tmp - 32 * nclass;

                    if (nclass >= 0) {
                        if (ibe == 0) {
                            float occLL = float(moduleHits_B[modulenum_tmp]) / float(numberOfStrawsMod[nclass]);
                            float occHL = float(HLmoduleHits_B[modulenum_tmp]) / float(numberOfStrawsMod[nclass]);
//                            m_LLOcc[ibe][LLocc_index] += occLL;
                            AvgLLOcc_side_x = i - (32 * nclass);
                            AvgLLOcc_side_y = occLL;
                            fill("RDOLLHLOccHistograms"+std::to_string(ibe)+std::to_string(iside), AvgLLOcc_side_x, AvgLLOcc_side_y);
                            AvgHLOcc_side_x = i - (32 * nclass);
                            AvgHLOcc_side_y = occHL;
                            fill("RDOLLHLOccHistograms"+std::to_string(ibe)+std::to_string(iside), AvgHLOcc_side_x, AvgHLOcc_side_y);
                            AvgLLOccMod_side_x = i;
                            AvgLLOccMod_side_y = occLL;
                            fill("RDOLLHLOccHistograms"+std::to_string(ibe)+std::to_string(iside), AvgLLOccMod_side_x, AvgLLOccMod_side_y);
                            AvgHLOccMod_side_x = i;
                            AvgHLOccMod_side_y = occHL;
                            fill("RDOLLHLOccHistograms"+std::to_string(ibe)+std::to_string(iside), AvgHLOccMod_side_x, AvgHLOccMod_side_y);
                        } else if (ibe == 1) {
                            float occLL = float(moduleHits_E[modulenum_tmp]) / float(numberOfStrawsWheel[nclass]);
                            float occHL = float(HLmoduleHits_E[modulenum_tmp]) / float(numberOfStrawsWheel[nclass]);

                            if (LLocc_index < 64) {
//                                m_LLOcc[ibe][LLocc_index] += occLL;
                            } else {
                                ATH_MSG_WARNING("m_LLOcc index out of bounds!"); // To satisfy Coverity defect CID 16514 which we believe is a false report.
                            }
                            AvgLLOcc_side_x = i - (32 * nclass);
                            AvgLLOcc_side_y = occLL;
                            fill("RDOLLHLOccHistograms"+std::to_string(ibe)+std::to_string(iside), AvgLLOcc_side_x, AvgLLOcc_side_y);
                            AvgHLOcc_side_x = i - (32 * nclass);
                            AvgHLOcc_side_y = occHL;
                            fill("RDOLLHLOccHistograms"+std::to_string(ibe)+std::to_string(iside), AvgHLOcc_side_x, AvgHLOcc_side_y);
                            AvgLLOccMod_side_x = i;
                            AvgLLOccMod_side_y = occLL;
                            fill("RDOLLHLOccHistograms"+std::to_string(ibe)+std::to_string(iside), AvgLLOccMod_side_x, AvgLLOccMod_side_y);
                            AvgHLOccMod_side_x = i;
                            AvgHLOccMod_side_y = occHL;
                            fill("RDOLLHLOccHistograms"+std::to_string(ibe)+std::to_string(iside), AvgHLOccMod_side_x, AvgHLOccMod_side_y);
                        }
                    }
                }
            }
        }
        // Normalization for online environmenmet
        // Insert here
        
    }
  
    if (m_environment == Environment_t::online) {
        if (m_doShift) {
            Summary = 0;
            SummaryWeight = m_totalEvents;
            fill("RDOShiftSmryHistograms", SummaryWeight, Summary);
        }
    }

    ATH_MSG_VERBOSE("Leaving Fill TRT RDO Histograms");
    return StatusCode::SUCCESS;
}





// Fill the TRT Efficiency Histograms
//----------------------------------------------------------------------------------//
StatusCode TRTMonitoringRun3RAW_Alg::fillTRTEfficiency(const TrackCollection& combTrackCollection) const {
//----------------------------------------------------------------------------------//
	ATH_MSG_VERBOSE("Filling TRT Efficiency Histograms");
    
    // TEfficiency
    auto Efficiency_eta                  = Monitored::Scalar<float>("Efficiency_eta", 0.0);
    auto Efficiency_phi                  = Monitored::Scalar<float>("Efficiency_phi", 0.0);
    auto Efficiency_pt                   = Monitored::Scalar<float>("Efficiency_pt", 0.0);
    auto Efficiency_z0                   = Monitored::Scalar<float>("Efficiency_z0", 0.0);
    auto EfficiencyBarrel_locR           = Monitored::Scalar<float>("EfficiencyBarrel_locR", 0.0);
    auto EfficiencyBarrel_locR_Ar        = Monitored::Scalar<float>("EfficiencyBarrel_locR_Ar", 0.0);
    auto EfficiencyBarrelMap             = Monitored::Scalar<float>("EfficiencyBarrelMap", 0.0);
    auto EfficiencyEndCapMap             = Monitored::Scalar<float>("EfficiencyEndCapMap", 0.0);
    auto EfficiencyEndCap_locR           = Monitored::Scalar<float>("EfficiencyEndCap_locR", 0.0);
    auto EfficiencyEndCap_locR_Ar        = Monitored::Scalar<float>("EfficiencyEndCap_locR_Ar", 0.0);
    auto EfficiencyS                     = Monitored::Scalar<float>("EfficiencyS", 0.0);
    auto EfficiencyC                     = Monitored::Scalar<float>("EfficiencyC", 0.0);
    auto Efficiency_eta_passed           = Monitored::Scalar<bool>("Efficiency_eta_passed", false);
    auto Efficiency_phi_passed           = Monitored::Scalar<bool>("Efficiency_phi_passed", false);
    auto Efficiency_pt_passed            = Monitored::Scalar<bool>("Efficiency_pt_passed", false);
    auto Efficiency_z0_passed            = Monitored::Scalar<bool>("Efficiency_z0_passed", false);
    auto EfficiencyBarrel_locR_passed    = Monitored::Scalar<bool>("EfficiencyBarrel_locR_passed", false);
    auto EfficiencyBarrel_locR_Ar_passed = Monitored::Scalar<bool>("EfficiencyBarrel_locR_Ar_passed", false);
    auto EfficiencyBarrelMap_passed      = Monitored::Scalar<bool>("EfficiencyBarrelMap_passed", false);
    auto EfficiencyEndCapMap_passed      = Monitored::Scalar<bool>("EfficiencyEndCapMap_passed", false);
    auto EfficiencyEndCap_locR_passed    = Monitored::Scalar<bool>("EfficiencyEndCap_locR_passed", false);
    auto EfficiencyEndCap_locR_Ar_passed = Monitored::Scalar<bool>("EfficiencyEndCap_locR_Ar_passed", false);
    auto EfficiencyS_passed              = Monitored::Scalar<bool>("EfficiencyS_passed", false);
    auto EfficiencyC_passed              = Monitored::Scalar<bool>("EfficiencyC_passed", false);

	// Reduce unnecessary divisions
	const float invGeV = 1. / CLHEP::GeV;
	const float invmm = 1. / CLHEP::mm;
	int itrack = 0;
    float track_pt  = 0;
    float track_eta = 0;
    float track_phi = 0;
    float track_d0  = 0;
    float track_z0  = 0;

	for (auto track = combTrackCollection.begin(); track != combTrackCollection.end(); ++track) {
		// Online: use all tracks, offline: use only every xth track, skip the rest
        if (m_environment != Environment_t::online && (itrack % m_every_xth_track) != 0) continue;
 
		++itrack;
		// Get perigee
		const Trk::Perigee *perigee = (*track)->perigeeParameters();

		if (perigee) {
			track_pt  = perigee->pT();
			track_eta = perigee->eta();
			track_phi = perigee->parameters()[Trk::phi0];
			track_d0  = perigee->parameters()[Trk::d0];
			track_z0  = perigee->parameters()[Trk::z0];
			ATH_MSG_DEBUG("This track has perigee parameters:\n"
			              << "                 pT     = " << track_pt * invGeV  << " GeV" << "\n"
			              << "                 eta =    " << track_eta << "\n"
			              << "                 phi0 =   " << track_phi << "\n"
			              << "                 d0 =     " << track_d0 * invmm << "\n"
			              << "                 z0 =     " << track_z0 * invmm << "\n"
			              << "                 theta =  " << perigee->parameters()[Trk::theta] << "\n"
			              << "                 qOverP = " << perigee->parameters()[Trk::qOverP]);
		} else {
			ATH_MSG_DEBUG("This track has null perigeeParameters.");
			continue;
		}

		const DataVector<const Trk::TrackStateOnSurface> *track_states = (*track)->trackStateOnSurfaces();

		if (track_states) {
			ATH_MSG_DEBUG("This track has " << track_states->size() << " track states on surface.");
		} else {
			ATH_MSG_DEBUG("This track has null track states on surface.");
			continue;
		}

		const std::unique_ptr<const Trk::TrackSummary> summary(m_TrackSummaryTool->summary(*(*track)));
		int n_trt_hits = summary->get(Trk::numberOfTRTHits);
		int n_sct_hits = summary->get(Trk::numberOfSCTHits);
		int n_pixel_hits = summary->get(Trk::numberOfPixelHits);
		float p = 1.0e+08;

		if (perigee) {
			p = (perigee->parameters()[Trk::qOverP] != 0.) ? fabs(1. / (perigee->parameters()[Trk::qOverP])) : 1.0e+08;
		}

		float min_pt_new = m_min_pT;

		if (m_isCosmics == false) {
			min_pt_new = 2.0 * CLHEP::GeV;
		}
		// Preselect tracks
		const bool passed_track_preselection =
			(fabs(perigee->parameters()[Trk::d0]) < m_max_abs_d0) &&
			(fabs(perigee->parameters()[Trk::z0]) < m_max_abs_z0) &&
			(perigee->pT() > min_pt_new) &&
			(p > m_minP) &&
			(fabs(perigee->eta()) < m_max_abs_eta) &&
			(n_pixel_hits >= m_min_pixel_hits) &&
			(n_sct_hits >= m_min_sct_hits) &&
			(n_trt_hits >= m_min_trt_hits);
		ATH_MSG_DEBUG("track has ntrt = " << n_trt_hits
		              << " and nsct = " << n_sct_hits
		              << " and npix = " << n_pixel_hits);

		if (!passed_track_preselection) {
			ATH_MSG_DEBUG("This track failed preselection.");
			continue;
		}
  
		ATH_MSG_DEBUG("This track passed preselection.");

		for (auto it = track_states->begin(); it != track_states->end(); it++) {
			if ( !((*it)->type(Trk::TrackStateOnSurface::Measurement)) ) continue;

			const Trk::TrackParameters *track_parameters = (*it)->trackParameters();

			if (!track_parameters) continue;

			Identifier id = track_parameters->associatedSurface().associatedDetectorElementIdentifier();

			if ( !((m_pTRTHelper->is_trt(id)) )) continue;

			float locR = track_parameters->parameters()[Trk::driftRadius];
			int barrel_ec       = m_pTRTHelper->barrel_ec(id);
			int layer_or_wheel  = m_pTRTHelper->layer_or_wheel(id);
			int phi_module      = m_pTRTHelper->phi_module(id);
			int straw_layer     = m_pTRTHelper->straw_layer(id);
			int straw           = m_pTRTHelper->straw(id);
			const bool isArgonStraw = (Straw_Gastype( m_sumTool->getStatusHT(id) ) == GasType::Ar);
			// Assume always Xe if m_ArgonXenonSplitter is not enabled, otherwise check the straw status (good is Xe, non-good is Ar)
			int ibe   = abs(barrel_ec) - 1;     // ibe   = 0 (Barrel), ibe   = 1 (Endcap)
			int iside = barrel_ec > 0 ? 0 : 1;  // iside = 0 (Side A), iside = 1 (Side C)
 
			if (ibe == 0) {
				if (isArgonStraw) {
                    EfficiencyBarrel_locR_Ar = locR;
                    EfficiencyBarrel_locR_Ar_passed = 1.0;
                    fill("TRTEfficiencyHistogramsBarrel", EfficiencyBarrel_locR_Ar_passed, EfficiencyBarrel_locR_Ar);
				} else {
                    EfficiencyBarrel_locR = locR;
                    EfficiencyBarrel_locR_passed = 1.0;
                    fill("TRTEfficiencyHistogramsBarrel", EfficiencyBarrel_locR_passed, EfficiencyBarrel_locR);
				}
			} else if (ibe == 1) {
				if (isArgonStraw) {
                    EfficiencyBarrel_locR_Ar = locR;
                    EfficiencyBarrel_locR_Ar_passed = 1.0;
                    fill("TRTEfficiencyHistogramsEndCap"+std::to_string(iside), EfficiencyBarrel_locR_Ar_passed, EfficiencyBarrel_locR_Ar);
				} else {
                    EfficiencyEndCap_locR = locR;
                    EfficiencyEndCap_locR_passed = 1.0;
                    fill("TRTEfficiencyHistogramsEndCap"+std::to_string(iside), EfficiencyEndCap_locR_passed, EfficiencyEndCap_locR);
				}
			}

			if (fabs(locR) >= 1.3) continue;

			int thisStrawNumber = 0;
			int chip = 0;

			if (ibe == 0) {
				thisStrawNumber = strawNumber(straw, straw_layer, layer_or_wheel);

				if (thisStrawNumber >= 0 && thisStrawNumber < s_Straw_max[ibe])
					chip = m_mat_chip_B[phi_module][thisStrawNumber];
			} else if (ibe == 1) {
				thisStrawNumber = strawNumberEndCap(straw, straw_layer, layer_or_wheel, phi_module, barrel_ec);

				if (thisStrawNumber >= 0 && thisStrawNumber < s_Straw_max[ibe])
					chip = m_mat_chip_E[phi_module][thisStrawNumber];
			}

            if (ibe == 0) {
                EfficiencyBarrelMap = thisStrawNumber;
                EfficiencyBarrelMap_passed = 1.0;
                fill("TRTEfficiencyHistogramsBarrel", EfficiencyBarrelMap_passed, EfficiencyBarrelMap);
            } else if (ibe == 1) {
                EfficiencyEndCapMap = thisStrawNumber;
                EfficiencyEndCapMap_passed = 1.0;
                fill("TRTEfficiencyHistogramsEndCap", EfficiencyEndCapMap_passed, EfficiencyEndCapMap);
            }

			if (m_doExpert) {
				if (iside == 0) {
                    EfficiencyS = thisStrawNumber;
                    EfficiencyS_passed = 1.0;
                    fill("TRTEfficiencyHistograms"+std::to_string(ibe)+std::to_string(phi_module), EfficiencyS_passed, EfficiencyS);
                    EfficiencyC = chip;
                    EfficiencyC_passed = 1.0;
                    fill("TRTEfficiencyHistograms"+std::to_string(ibe)+std::to_string(phi_module), EfficiencyC_passed, EfficiencyC);
				} else if (iside == 1) {
                    EfficiencyS = thisStrawNumber;
                    EfficiencyS_passed = 1.0;
                    fill("TRTEfficiencyHistograms"+std::to_string(ibe)+std::to_string(phi_module + 32), EfficiencyS_passed, EfficiencyS);
                    EfficiencyC = chip;
                    EfficiencyC_passed = 1.0;
                    fill("TRTEfficiencyHistograms"+std::to_string(ibe)+std::to_string(phi_module + 32), EfficiencyC_passed, EfficiencyC);
				}
			}

            Efficiency_eta = track_eta;
            Efficiency_eta_passed = 1.0;
            fill("TRTEfficiencyHistograms", Efficiency_eta_passed, Efficiency_eta);
            Efficiency_phi = track_phi;
            Efficiency_phi_passed = 1.0;
            fill("TRTEfficiencyHistograms", Efficiency_phi_passed, Efficiency_phi);
            Efficiency_pt = track_pt*invGeV;
            Efficiency_pt_passed = 1.0;
            fill("TRTEfficiencyHistograms", Efficiency_pt_passed, Efficiency_pt);
            Efficiency_z0 = track_z0;
            Efficiency_z0_passed = 1.0;
            fill("TRTEfficiencyHistograms", Efficiency_z0_passed, Efficiency_z0);
		}

		// Use hole finder to find holes on this track
		if (m_useHoleFinder) {
			const DataVector<const Trk::TrackStateOnSurface> *holes = m_trt_hole_finder->getHolesOnTrack(**track);

			if (!holes) {
				ATH_MSG_WARNING("TRTTrackHoleSearchTool returned null results.");
				continue;
			} else {
				for (auto it = holes->begin(); it != holes->end(); ++it) {
					if ( !((*it)->type(Trk::TrackStateOnSurface::Hole)) ) continue;

					const Trk::TrackParameters *track_parameters = (*it)->trackParameters();

					if (!track_parameters) continue;

					Identifier id = track_parameters->associatedSurface().associatedDetectorElementIdentifier();

					if ( !(m_pTRTHelper->is_trt(id)) ) continue;

					float locR = track_parameters->parameters()[Trk::driftRadius];
					int barrel_ec = m_pTRTHelper->barrel_ec(id);
					int layer_or_wheel = m_pTRTHelper->layer_or_wheel(id);
					int phi_module = m_pTRTHelper->phi_module(id);
					int straw_layer = m_pTRTHelper->straw_layer(id);
					int straw = m_pTRTHelper->straw(id);
					const bool isArgonStraw = Straw_Gastype( m_sumTool->getStatusHT(id) ) == GasType::Ar;
					// Assume always Xe if m_ArgonXenonSplitter is not enabled, otherwise check the straw status (good is Xe, non-good is Ar)
					int ibe = abs(barrel_ec) - 1;      // ibe   = 0 (Barrel), ibe   = 1 (Endcap)
					int iside = barrel_ec > 0 ? 0 : 1; // iside = 0 (Side A), iside = 1 (Side C)

					if (ibe == 0) {
						if (isArgonStraw) {
                            EfficiencyBarrel_locR_Ar = locR;
                            EfficiencyBarrel_locR_Ar_passed = 0.0;
                            fill("TRTEfficiencyHistograms", EfficiencyBarrel_locR_Ar_passed, EfficiencyBarrel_locR_Ar);
						} else {
                            EfficiencyBarrel_locR = locR;
                            EfficiencyBarrel_locR_passed = 0.0;
                            fill("TRTEfficiencyHistograms", EfficiencyBarrel_locR_passed, EfficiencyBarrel_locR);
						}
					} else if (ibe == 1) {
						if (isArgonStraw) {
                            EfficiencyEndCap_locR_Ar = locR;
                            EfficiencyEndCap_locR_Ar_passed = 0.0;
                            fill("TRTEfficiencyHistogramsEndCap"+std::to_string(iside), EfficiencyEndCap_locR_Ar_passed, EfficiencyEndCap_locR_Ar);
						} else {
                            EfficiencyEndCap_locR = locR;
                            EfficiencyEndCap_locR_passed = 0.0;
                            fill("TRTEfficiencyHistogramsEndCap"+std::to_string(iside), EfficiencyEndCap_locR_passed, EfficiencyEndCap_locR);
						}
					}

					if (fabs(locR) >= 1.3) continue;

					int thisStrawNumber = 0;
					int chip = 0;

					if (ibe == 0) {
						thisStrawNumber = strawNumber(straw, straw_layer, layer_or_wheel);

						if (thisStrawNumber >= 0 && thisStrawNumber < s_Straw_max[ibe]) {
							chip = m_mat_chip_B[phi_module][thisStrawNumber];
						}
					} else if (ibe == 1) {
						thisStrawNumber = strawNumberEndCap(straw, straw_layer, layer_or_wheel, phi_module, barrel_ec);

						if (thisStrawNumber >= 0 && thisStrawNumber < s_Straw_max[ibe]) {
							chip = m_mat_chip_E[phi_module][thisStrawNumber];
						}
					}
                    
                    if (ibe == 0) {
                        EfficiencyBarrelMap = thisStrawNumber;
                        EfficiencyBarrelMap_passed = 0.0;
                        fill("TRTEfficiencyHistogramsBarrel", EfficiencyBarrelMap_passed, EfficiencyBarrelMap);
                    } else if (ibe == 1) {
                        EfficiencyEndCapMap = thisStrawNumber;
                        EfficiencyEndCapMap_passed = 0.0;
                        fill("TRTEfficiencyHistogramsEndCap", EfficiencyEndCapMap_passed, EfficiencyEndCapMap);
                    }

					if (m_doExpert) {
						if (iside == 0) {
                            EfficiencyS = thisStrawNumber;
                            EfficiencyS_passed = 0.0;
                            fill("TRTEfficiencyHistograms"+std::to_string(ibe)+std::to_string(phi_module), EfficiencyS_passed, EfficiencyS);
                            EfficiencyC = chip;
                            EfficiencyC_passed = 0.0;
                            fill("TRTEfficiencyHistograms"+std::to_string(ibe)+std::to_string(phi_module), EfficiencyC_passed, EfficiencyC);
						} else if (iside == 1) {
                            EfficiencyS = thisStrawNumber;
                            EfficiencyS_passed = 0.0;
                            fill("TRTEfficiencyHistograms"+std::to_string(ibe)+std::to_string(phi_module + 32), EfficiencyS_passed, EfficiencyS);
                            EfficiencyC = chip;
                            EfficiencyC_passed = 0.0;
                            fill("TRTEfficiencyHistograms"+std::to_string(ibe)+std::to_string(phi_module + 32), EfficiencyC_passed, EfficiencyC);
						}
					}
                    Efficiency_eta = track_eta;
                    Efficiency_eta_passed = 0.0;
                    fill("TRTEfficiencyHistograms", Efficiency_eta_passed, Efficiency_eta);
                    Efficiency_phi = track_phi;
                    Efficiency_phi_passed = 0.0;
                    fill("TRTEfficiencyHistograms", Efficiency_phi_passed, Efficiency_phi);
                    Efficiency_pt = track_pt*invGeV;
                    Efficiency_pt_passed = 0.0;
                    fill("TRTEfficiencyHistograms", Efficiency_pt_passed, Efficiency_pt);
                    Efficiency_z0 = track_z0;
                    Efficiency_z0_passed = 0.0;
                    fill("TRTEfficiencyHistograms", Efficiency_z0_passed, Efficiency_z0);	
				}

				delete holes;
			}
		}
	}

    // Efficiency calculations
    // Insert here

	return StatusCode::SUCCESS;
}


// Fill TRT Hits histograms
//----------------------------------------------------------------------------------//
StatusCode TRTMonitoringRun3RAW_Alg::fillTRTHits(const TrackCollection& trackCollection) const {
//----------------------------------------------------------------------------------//
    ATH_MSG_VERBOSE("Filling TRT Tracks Histos");
   
    // TH1F
    auto HLhitOnTrack_B                 = Monitored::Scalar<float>("HLhitOnTrack_B", 0.0);
    auto HLhitOnTrack_E                 = Monitored::Scalar<float>("HLhitOnTrack_E", 0.0);
    auto HitWonTMap_B                   = Monitored::Scalar<float>("HitWonTMap_B", 0.0);
    auto HitWonTMap_E                   = Monitored::Scalar<float>("HitWonTMap_E", 0.0);
    auto HtoLRatioOnTrack_B_Ar          = Monitored::Scalar<float>("HtoLRatioOnTrack_B_Ar", 0.0);
    auto HtoLRatioOnTrack_B_Xe          = Monitored::Scalar<float>("HtoLRatioOnTrack_B_Xe", 0.0);
    auto HtoLRatioOnTrack_B             = Monitored::Scalar<float>("HtoLRatioOnTrack_B", 0.0);
    auto HtoLRatioOnTrack_E             = Monitored::Scalar<float>("HtoLRatioOnTrack_E", 0.0);
    auto HtoLRatioOnTrack_E_Ar          = Monitored::Scalar<float>("HtoLRatioOnTrack_E_Ar", 0.0);
    auto HtoLRatioOnTrack_E_Xe          = Monitored::Scalar<float>("HtoLRatioOnTrack_E_Xe", 0.0);
    auto NumSwLLWoT_E                   = Monitored::Scalar<float>("NumSwLLWoT_E", 0.0);
    auto NumSwLLWoT_B                   = Monitored::Scalar<float>("NumSwLLWoT_B", 0.0);
    
    // TEfficiency
    auto StrawEffDetPhi_B               = Monitored::Scalar<float>("StrawEffDetPhi_B", 0.0);
    auto StrawEffDetPhi_B_passed        = Monitored::Scalar<bool>("StrawEffDetPhi_B_passed", false);
    auto StrawEffDetPhi_E               = Monitored::Scalar<float>("StrawEffDetPhi_E", 0.0);
    auto StrawEffDetPhi_E_passed        = Monitored::Scalar<bool>("StrawEffDetPhi_E_passed", false);
    auto EfficiencyS                    = Monitored::Scalar<float>("EfficiencyS", 0.0);
    auto EfficiencyS_passed             = Monitored::Scalar<bool>("EfficiencyS_passed", false);
    auto EfficiencyC                    = Monitored::Scalar<float>("EfficiencyC", 0.0);
    auto EfficiencyC_passed             = Monitored::Scalar<bool>("EfficiencyC_passed", false);
    auto HitHonTMapS                    = Monitored::Scalar<float>("HitHonTMapS", 0.0);
    auto HitHonTMapS_passed             = Monitored::Scalar<bool>("HitHonTMapS_passed", false);
    auto HitHWonTMapS                   = Monitored::Scalar<float>("HitHWonTMapS", 0.0);
    auto HitHWonTMapS_passed            = Monitored::Scalar<bool>("HitHWonTMapS_passed", false);
    auto HitHWonTMapC                   = Monitored::Scalar<float>("HitHWonTMapC", 0.0);
    auto HitHWonTMapC_passed            = Monitored::Scalar<bool>("HitHWonTMapC_passed", false);
    auto HitHonTMapC                    = Monitored::Scalar<float>("HitHonTMapC", 0.0);
    auto HitHonTMapC_passed             = Monitored::Scalar<bool>("HitHonTMapC_passed", false);
    auto HitWonTMapS                    = Monitored::Scalar<float>("HitWonTMapS", 0.0);
    auto HitWonTMapS_passed             = Monitored::Scalar<bool>("HitWonTMapS_passed", false);
    auto HitWonTMapC                    = Monitored::Scalar<float>("HitWonTMapC", 0.0);
    auto HitWonTMapC_passed             = Monitored::Scalar<bool>("HitWonTMapC_passed", false);
    auto HitAonTMapS                    = Monitored::Scalar<float>("HitAonTMapS", 0.0);
    auto HitAonTMapS_passed             = Monitored::Scalar<bool>("HitAonTMapS_passed", false);
    auto HitAonTMapC                    = Monitored::Scalar<float>("HitAonTMapC", 0.0);
    auto HitAonTMapC_passed             = Monitored::Scalar<bool>("HitAonTMapC_passed", false);
    auto HitAWonTMapS                   = Monitored::Scalar<float>("HitAWonTMapS", 0.0);
    auto HitAWonTMapS_passed            = Monitored::Scalar<bool>("HitAWonTMapS_passed", false);
    auto HitAWonTMapC                   = Monitored::Scalar<float>("HitAWonTMapC", 0.0);
    auto HitAWonTMapC_passed            = Monitored::Scalar<bool>("HitAWonTMapC_passed", false);

    auto p_trk = trackCollection.begin();

    const Trk::Perigee *mPer = nullptr;
    const DataVector<const Trk::TrackParameters> *AllTrkPar(0);
    DataVector<const Trk::TrackParameters>::const_iterator p_trkpariter;

	int ntrackstack[2][64];

	for (int ibe = 0; ibe < 2; ibe++) {
		std::fill(ntrackstack[ibe], ntrackstack[ibe] + 64, 0);
	}

	for (; p_trk != trackCollection.end(); ++p_trk) {
		const std::unique_ptr<const Trk::TrackSummary> summary(m_TrackSummaryTool->summary(*(*p_trk)));
		int nTRTHits = summary->get(Trk::numberOfTRTHits);

		if (nTRTHits < m_minTRThits) continue;

		AllTrkPar = (*p_trk)->trackParameters();

		// Search of MeasuredPerigee in TrackParameters
		// The following algorithm only finds the First perigee measurement.
		// As there should be one and only one perigee measurement then this assumption should be valid.
		// But no check is done to see if there is more than one perigee measurement.
		for (p_trkpariter = AllTrkPar->begin(); p_trkpariter != AllTrkPar->end(); ++p_trkpariter) {
			//if track parameter does have a measured perigee then the track parameter is a keeper and break out of the loop
			if ((mPer = dynamic_cast<const Trk::Perigee *>(*p_trkpariter))) break;
		}

		if (!mPer) continue;

		float theta   =  mPer->parameters()[Trk::theta];
		float p       =  (mPer->parameters()[Trk::qOverP] != 0.) ? fabs(1. / (mPer->parameters()[Trk::qOverP])) : 10e7;
		float pT      =  (p * sin(theta));
		pT = pT * 1e-3;  // GeV

		if (p < m_minP) continue;

		const DataVector<const Trk::TrackStateOnSurface> *trackStates = (**p_trk).trackStateOnSurfaces();

		if (trackStates == 0) continue;

		DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItBegin0    = trackStates->begin();
		DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItBegin     = trackStates->begin();
		DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItBeginTemp = trackStates->begin();
		DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItEnd       = trackStates->end();
		int n_trt_hits = summary->get(Trk::numberOfTRTHits);
		int n_sct_hits = summary->get(Trk::numberOfSCTHits);
		int n_pixel_hits = summary->get(Trk::numberOfPixelHits);
		const int n_si_hits = n_pixel_hits + n_sct_hits;

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

        int checkB[2] = {0, 0};
        int checkEC[2] = {0, 0};
        int checkEC_B[2] = {0, 0};
        int nTRTHitsW[2][2];
        int nTRTHitsW_Ar[2][2];
        int nTRTHitsW_Xe[2][2];
        int nTRTHLHitsW[2][2];
        int nTRTHLHitsW_Ar[2][2];
        int nTRTHLHitsW_Xe[2][2];
        int nTRTHits_side[2][2];
        int nTRTHitsW_perwheel[2][18];
        int hitontrack[2] = {0, 0};
        int hitontrack_E_side[2] = {0, 0};

        for (int ibe = 0; ibe < 2; ibe++) {
            for (int iside = 0; iside < 2; iside++) {
                nTRTHits_side[ibe][iside] = -1;
                nTRTHitsW[ibe][iside] = 0;
                nTRTHitsW_Ar[ibe][iside] = 0;
                nTRTHitsW_Xe[ibe][iside] = 0;
                nTRTHLHitsW[ibe][iside] = 0;
                nTRTHLHitsW_Ar[ibe][iside] = 0;
                nTRTHLHitsW_Xe[ibe][iside] = 0;
            }
            std::fill(nTRTHitsW_perwheel[ibe], nTRTHitsW_perwheel[ibe] + 18, 0);
        }

        bool isBarrelOnly = true;
        bool ECAhit = false;
        bool ECChit = false;
        bool Bhit = false;
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
            if (checkB[iside] == 0 && ibe == 0) {
                checkB[iside] = 1;
            }

            if (checkEC[iside] == 0 && ibe == 1) {
                checkEC[iside] = 1;
            }

            if (checkEC_B[iside] == 0 && checkB[iside] == 1 && ibe == 1 ) {
                checkEC_B[iside] = 1;
            } // ToDo: be sure about this approach

            if (ibe == 0) {
                Bhit = true;
            } else if (barrel_ec == 2) {
                isBarrelOnly = false;
                ECAhit = true;
            } else if (barrel_ec == -2) {
                isBarrelOnly = false;
                ECChit = true;
            }

            Identifier surfaceID;
            surfaceID = trtCircle->identify();
            const bool isArgonStraw = ( Straw_Gastype( m_sumTool->getStatusHT(surfaceID) ) == GasType::Ar );
            // Assume always Xe if m_ArgonXenonSplitter is not enabled, otherwise check the straw status (good is Xe, non-good is Ar)
            float temp_locr = aTrackParam->parameters()[Trk::driftRadius];
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

                        if (m_doShift) {
                            StrawEffDetPhi_B = phi_module;
                            StrawEffDetPhi_B_passed = 1.0;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), StrawEffDetPhi_B_passed, StrawEffDetPhi_B);
                        }

                    } else if (ibe == 1) {
                        hitontrack[ibe]++;
                        hitontrack_E_side[iside]++;

                        if (m_doShift) {
                            StrawEffDetPhi_E = phi_module;
                            StrawEffDetPhi_E_passed = 1.0;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), StrawEffDetPhi_E_passed, StrawEffDetPhi_E);
                        }
                    }

                    if (m_doStraws && m_doExpert) {
                        EfficiencyS = thisStrawNumber[ibe];
                        EfficiencyS_passed = 1.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), EfficiencyS_passed, EfficiencyS);
                    }

                    if (m_doChips && m_doExpert) {
                        EfficiencyC = chip[ibe] - 1;
                        EfficiencyC_passed = 1.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), EfficiencyC_passed, EfficiencyC);
                    }
                }
            } else {
                if (m_idHelper->is_trt(DCoTId)) { // ToDo: Is this really needed?
                    if (ibe == 0) {
                        if (m_doShift) {
                            StrawEffDetPhi_B = phi_module;
                            StrawEffDetPhi_B_passed = 0.0;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), StrawEffDetPhi_B_passed, StrawEffDetPhi_B);
                        }

                        if (m_doStraws && m_doShift) {
                        }
                    } else if (ibe == 1) {
                        if (m_doShift) {
                            StrawEffDetPhi_E = phi_module;
                            StrawEffDetPhi_E_passed = 0.0;
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), StrawEffDetPhi_E_passed, StrawEffDetPhi_E);
                        }
                    }

                    if (m_doStraws && m_doExpert) {
                        EfficiencyS = thisStrawNumber[ibe];
                        EfficiencyS_passed = 0.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), EfficiencyS_passed, EfficiencyS);
                    }

                    if (m_doChips  && m_doExpert) {
                        EfficiencyC = chip[ibe] - 1;
                        EfficiencyC_passed = 0.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), EfficiencyC_passed, EfficiencyC);
                    }
                }
            }
            const InDet::TRT_DriftCircle *RawDriftCircle = dynamic_cast<const InDet::TRT_DriftCircle *>(trtCircle->prepRawData());
            if (RawDriftCircle) {
                nTRTHits_side[ibe][iside]++;
                int middleHTbit       = RawDriftCircle->getWord() & 0x00020000;
                //0x00020000 = 0000 0000 0000 0000 0000 0010 0000 0000 0000 0000
                int hitinvaliditygate = RawDriftCircle->getWord() & 0x000DFE80;
                //0x000DFE80 = 0000 0000 0000 0000 0000 1101 1111 1110 1000 0000
                bool is_middleHTbit_high = (middleHTbit != 0);
                bool is_anybininVgate_high = (hitinvaliditygate != 0);

                if (m_doExpert && m_doStraws) {
                    HitHonTMapS = thisStrawNumber[ibe];
                    HitHonTMapS_passed = 0.0;
                    fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHonTMapS_passed, HitHonTMapS); // Experimental
                    HitHWonTMapS = thisStrawNumber[ibe];
                    HitHWonTMapS_passed = 0.0;
                    fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHWonTMapS_passed, HitHWonTMapS); // Experimental

                    if (is_middleHTbit_high) {
                        HitHonTMapS = thisStrawNumber[ibe];
                        HitHonTMapS_passed = 1.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHonTMapS_passed, HitHonTMapS);
                        HitHWonTMapS = thisStrawNumber[ibe];
                        HitHWonTMapS_passed = 1.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHWonTMapS_passed, HitHWonTMapS);
                    }
                }
                
                if (m_doExpert && m_doChips) { // Experimental
                    HitHWonTMapC = chip[ibe] - 1;
                    HitHWonTMapC_passed = 0.0;
                    fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHWonTMapC_passed, HitHWonTMapC);
                    HitHonTMapC = chip[ibe] - 1;
                    HitHonTMapC_passed = 0.0;
                    fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHonTMapC_passed, HitHonTMapC);
                }
                if (m_doExpert && m_doChips) {
                    if (is_middleHTbit_high) {
                        HitHWonTMapC = chip[ibe] - 1;
                        HitHWonTMapC_passed = 1.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHWonTMapC_passed, HitHWonTMapC);
                        HitHonTMapC = chip[ibe] - 1;
                        HitHonTMapC_passed = 1.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitHonTMapC_passed, HitHonTMapC);
                    }
                }

                const int driftTimeBin = RawDriftCircle->driftTimeBin();

                if ( (driftTimeBin < 24) &&
                     !(RawDriftCircle->lastBinHigh()) &&
                     !(RawDriftCircle->firstBinHigh()) ) {
                    if (m_doStraws) {
                        if (ibe == 0) {
                            HitWonTMap_B = thisStrawNumber[ibe];
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), HitWonTMap_B);
                        } else if (ibe == 1) {
                            HitWonTMap_E = thisStrawNumber[ibe];
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), HitWonTMap_E);
                        }
                    }
                }

                if (m_doExpert && m_doStraws) { // Experimental
                    HitWonTMapS = thisStrawNumber[ibe];
                    HitWonTMapS_passed = 0.0;
                    fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitWonTMapS_passed, HitWonTMapS);
                }
                if (m_doExpert && m_doChips) { // Experimental
                    HitWonTMapC = chip[ibe] - 1;
                    HitWonTMapC_passed = 0.0;
                    fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitWonTMapC_passed, HitWonTMapC);
                }
                
                if ((driftTimeBin > 2) && (driftTimeBin < 17)) {
                    if (m_doExpert && m_doStraws) {
                        HitWonTMapS = thisStrawNumber[ibe];
                        HitWonTMapS_passed = 1.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitWonTMapS_passed, HitWonTMapS);
                    }
                    if (m_doExpert && m_doChips) {
                        HitWonTMapC = chip[ibe] - 1;
                        HitWonTMapC_passed = 1.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitWonTMapC_passed, HitWonTMapC);
                    }
                }

                const int trailingEdge = RawDriftCircle->trailingEdge();

                const bool firstBinHigh = RawDriftCircle->firstBinHigh();
                const bool lastBinHigh = RawDriftCircle->lastBinHigh();

                if (firstBinHigh || lastBinHigh || driftTimeBin > 0 || trailingEdge < 23) {
                    if (m_doExpert && m_doStraws) {
                        HitAonTMapS = thisStrawNumber[ibe];
                        HitAonTMapS_passed = 1.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitAonTMapS_passed, HitAonTMapS);
                    }

                    if (m_doExpert && m_doChips) {
                        HitAonTMapC = chip[ibe] - 1;
                        HitAonTMapC_passed = 1.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitAonTMapC_passed, HitAonTMapC);
                    }

                    nTRTHitsW[ibe][iside]++;

                    if (isArgonStraw) nTRTHitsW_Ar[ibe][iside]++;
                    else nTRTHitsW_Xe[ibe][iside]++;


                    nTRTHitsW_perwheel[iside][layer_or_wheel]++;

                    if (is_middleHTbit_high) {
                        nTRTHLHitsW[ibe][iside]++;
                        if (isArgonStraw) nTRTHLHitsW_Ar[ibe][iside]++;
                        else nTRTHLHitsW_Xe[ibe][iside]++; 
                    }
                }

                if (is_anybininVgate_high) {
                    if (m_doExpert && m_doStraws) {
                        HitAWonTMapS = thisStrawNumber[ibe];
                        HitAWonTMapS_passed = 1.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitAWonTMapS_passed, HitAWonTMapS);
                    }

                    if (m_doExpert && m_doChips) {
                        HitAWonTMapC = chip[ibe] - 1;
                        HitAWonTMapC_passed = 1.0;
                        fill("TRTTrackHistograms"+std::to_string(ibe)+std::to_string(iphi_module), HitAWonTMapC_passed, HitAWonTMapC);
                    }
                }
            }
        }

        // ToDo: work on the part below
        for (int ibe = 0; ibe < 2; ibe++) {
            for (int i = 0; i < 64; i++)
                if (trackfound[ibe][i])
                    ntrackstack[ibe][i]++;

            if (phi2D[ibe] < 0) continue;


            if (m_doShift) {
                if (ibe == 0) {
                    if (isBarrelOnly) {
                        if (nTRTHitsW[ibe][0] > 0) {
                            if (nTRTHitsW[ibe][1] > 0) {
                                NumSwLLWoT_B = nTRTHitsW[ibe][0] + nTRTHitsW[ibe][1];
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe), NumSwLLWoT_B);
                            }
                            else {
                                NumSwLLWoT_B = nTRTHitsW[ibe][0];
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe), NumSwLLWoT_B);
                            }
                        } else if (nTRTHitsW[ibe][1] > 0) {
                            NumSwLLWoT_B = nTRTHitsW[ibe][1];
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), NumSwLLWoT_B);
                        }
                    }

                    if (nTRTHLHitsW[ibe][0] > 0) {
                        if (nTRTHLHitsW[ibe][1] > 0) {
                            HLhitOnTrack_B = nTRTHLHitsW[ibe][0] + nTRTHLHitsW[ibe][1];
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), HLhitOnTrack_B);
                        } else {
                            HLhitOnTrack_B = nTRTHLHitsW[ibe][0];
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe), HLhitOnTrack_B);
                        }
                    } else if (nTRTHLHitsW[ibe][1] > 0) {
                        HLhitOnTrack_B = nTRTHLHitsW[ibe][1];
                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe), HLhitOnTrack_B);
                    }

                    if (nTRTHitsW[ibe][0] + nTRTHitsW[ibe][1] > 0) {
                        HtoLRatioOnTrack_B = (float)(nTRTHLHitsW[ibe][0] + nTRTHLHitsW[ibe][1]) / (nTRTHitsW[ibe][0] + nTRTHitsW[ibe][1]);
                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe), HtoLRatioOnTrack_B);
                    }

                    if (nTRTHitsW_Ar[ibe][0] + nTRTHitsW_Ar[ibe][1] > 0) {
                        HtoLRatioOnTrack_B_Ar = (float)(nTRTHLHitsW_Ar[ibe][0] + nTRTHLHitsW_Ar[ibe][1]) / (nTRTHitsW_Ar[ibe][0] + nTRTHitsW_Ar[ibe][1]);
                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe), HtoLRatioOnTrack_B_Ar);
                    }

                    if (nTRTHitsW_Xe[ibe][0] + nTRTHitsW_Xe[ibe][1] > 0) {
                        HtoLRatioOnTrack_B_Xe = (float)(nTRTHLHitsW_Xe[ibe][0] + nTRTHLHitsW_Xe[ibe][1]) / (nTRTHitsW_Xe[ibe][0] + nTRTHitsW_Xe[ibe][1]);
                        fill("ShiftTRTTrackHistograms"+std::to_string(ibe), HtoLRatioOnTrack_B_Xe);
                    }
                } else if (ibe == 1) {
                    if (nTRTHitsW[ibe][0] > 0) {
                        if (nTRTHitsW[ibe][1] > 0) {
                            if (ECAhit && !ECChit && !Bhit) {
                                NumSwLLWoT_E = nTRTHitsW[ibe][0];
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+"0", NumSwLLWoT_E);
                            }

                            if (ECChit && !ECAhit && !Bhit) {
                                NumSwLLWoT_E = nTRTHitsW[ibe][1];
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+"1", NumSwLLWoT_E);
                            }
                        }

                        if (ECAhit && !ECChit && !Bhit) {
                            NumSwLLWoT_E = nTRTHitsW[ibe][0];
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+"0", NumSwLLWoT_E);
                        }
                    } else if (nTRTHitsW[ibe][1] > 0) {
                        if (ECChit && !ECAhit && !Bhit) {
                            NumSwLLWoT_E = nTRTHitsW[ibe][1];
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+"0", NumSwLLWoT_E);
                        }
                    }

                    for (int iside = 0; iside < 2; iside++) {
                        if (nTRTHLHitsW[ibe][iside] > 0) {
                            HLhitOnTrack_E = nTRTHLHitsW[ibe][iside];
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), HLhitOnTrack_E);
                        }

                        if ((nTRTHitsW[ibe][iside]) > 0) {
                            HtoLRatioOnTrack_E = (float)(nTRTHLHitsW[ibe][iside]) / nTRTHitsW[ibe][iside];
                            fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), HtoLRatioOnTrack_E);

                            if ((nTRTHitsW_Ar[ibe][iside]) > 0) {
                                HtoLRatioOnTrack_E_Ar = (float)(nTRTHLHitsW_Ar[ibe][iside]) / nTRTHitsW_Ar[ibe][iside];
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), HtoLRatioOnTrack_E_Ar);
                            }

                            if ((nTRTHitsW_Xe[ibe][iside]) > 0) {
                                HtoLRatioOnTrack_E_Xe = (float)(nTRTHLHitsW_Xe[ibe][iside]) / nTRTHitsW_Xe[ibe][iside];
                                fill("ShiftTRTTrackHistograms"+std::to_string(ibe)+std::to_string(iside), HtoLRatioOnTrack_E_Xe);
                            }
                        }
                    }
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}


StatusCode TRTMonitoringRun3RAW_Alg::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;
    bool passEventBurst;
    
    ATH_MSG_VERBOSE("Monitoring Histograms being filled");
    
    SG::ReadHandle<TRT_RDO_Container>   rdoContainer(m_rdoContainerKey, ctx);
    SG::ReadHandle<xAOD::EventInfo>     xAODEventInfo(m_xAODEventInfoKey, ctx);
    SG::ReadHandle<InDetTimeCollection> trtBCIDCollection(m_TRT_BCIDCollectionKey, ctx);
    SG::ReadHandle<TrackCollection>     combTrackCollection(m_combTrackCollectionKey, ctx);
    SG::ReadHandle<TrackCollection>     trackCollection(m_trackCollectionKey, ctx);

    if (m_doRDOsMon) {
        if (!rdoContainer.isValid()) {
            ATH_MSG_ERROR("Could not find TRT Raw Data Object " << m_rdoContainerKey.key() <<
                          " in store");
            return StatusCode::FAILURE;
        }

        passEventBurst = checkEventBurst(*rdoContainer);
        if (passEventBurst) {
            if (!trtBCIDCollection.isValid()) {
                ATH_MSG_INFO("Could not find BCID collection " << m_TRT_BCIDCollectionKey.key() <<
                             " in store");
            }

            ATH_CHECK( fillTRTRDOs(*rdoContainer, *xAODEventInfo, trtBCIDCollection.ptr()) );
        }
    } else passEventBurst = true;
    
    if (m_doEfficiency) {
		if (!combTrackCollection.isValid()) {
			ATH_MSG_ERROR("Could not find track collection " << m_combTrackCollectionKey.key() <<
			              " in store");
			return StatusCode::FAILURE;
		}
		ATH_CHECK( fillTRTEfficiency(*combTrackCollection) );
	}
    
    if (m_doHitsMon) {
        if (!trackCollection.isValid()) {
            ATH_MSG_ERROR("Could not find track collection " << m_trackCollectionKey.key() <<
                        " in store");
            return StatusCode::FAILURE;
        }
	    if (passEventBurst) {
            ATH_CHECK( fillTRTHits(*trackCollection) );
	    }
    }

    return StatusCode::SUCCESS;
}
