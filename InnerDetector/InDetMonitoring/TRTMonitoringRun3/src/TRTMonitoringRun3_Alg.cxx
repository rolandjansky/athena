/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTMonitoringRun3/TRTMonitoringRun3_Alg.h"

#include "AthContainers/DataVector.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
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
#include "TRT_ConditionsServices/ITRT_ByteStream_ConditionsSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include <cmath>


const int TRTMonitoringRun3_Alg::s_numberOfBarrelStacks = 32;
const int TRTMonitoringRun3_Alg::s_numberOfEndCapStacks = 32;
const int TRTMonitoringRun3_Alg::s_Straw_max[2] = {1642, 3840};
const int TRTMonitoringRun3_Alg::s_iStack_max[2] = {32, 64};
const int TRTMonitoringRun3_Alg::s_iChip_max[2] = {104, 240};
const int TRTMonitoringRun3_Alg::s_numberOfStacks[2] = {32, 32};
const int TRTMonitoringRun3_Alg::s_moduleNum[2] = {96, 64};

TRTMonitoringRun3_Alg::TRTMonitoringRun3_Alg( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_doRandom(false)
,m_pTRTHelper(0)

{
    declareProperty("DoRDOsMon",                m_doRDOsMon             = false);
    declareProperty("NumberOfEvents",           m_usedEvents            = -1);
    declareProperty("DoTracksMon",              m_doTracksMon           = true);
    declareProperty("doStraws",                 m_doStraws              = true);
    declareProperty("doChips",                  m_doChips               = true);
    declareProperty("doExpert",                 m_doExpert              = true);
    declareProperty("doEfficiency",             m_doEfficiency          = false);
    declareProperty("doMaskStraws",             m_doMaskStraws          = true);
    declareProperty("doShift",                  m_doShift               = true); 
    declareProperty("DistanceToStraw",          m_DistToStraw           = 0.4);
    declareProperty("MinTrackP",                m_minP                  = 0.0 * CLHEP::GeV);
}

 
TRTMonitoringRun3_Alg::~TRTMonitoringRun3_Alg() {}


StatusCode TRTMonitoringRun3_Alg::initialize() { 
    using namespace Monitored;
    
    // Initialization of VarHandleKeys
    ATH_CHECK( m_trackCollectionKey.initialize() );
    ATH_CHECK( m_combTrackCollectionKey.initialize() );
    ATH_CHECK( m_xAODEventInfoKey.initialize() );
    ATH_CHECK( m_TRT_BCIDCollectionKey.initialize(m_doRDOsMon) );
    
    ATH_CHECK(detStore()->retrieve(m_idHelper, "AtlasID"));
    
    // Detector manager
    ATH_CHECK(detStore()->retrieve(m_mgr,"TRT"));

    // TRT ID helper
    ATH_CHECK(detStore()->retrieve(m_pTRTHelper,"TRT_ID"));

    return AthMonitorAlgorithm::initialize();
}


//----------------------------------------------------------------------------------//
int TRTMonitoringRun3_Alg::strawLayerNumber(int strawLayerNumber, int LayerNumber) const {
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
int TRTMonitoringRun3_Alg::strawNumber(int strawNumber, int strawlayerNumber, int LayerNumber) const {
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
int TRTMonitoringRun3_Alg::strawNumberEndCap(int strawNumber, int strawLayerNumber, int LayerNumber, int phi_stack, int side) const {
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
float TRTMonitoringRun3_Alg::radToDegrees(float radValue) const {
//----------------------------------------------------------------------------------//
    float degreeValue = radValue / M_PI * 180;

    if (degreeValue < 0) degreeValue += 360;

    return degreeValue;
}



//Fill the TRT Track level histograms
//----------------------------------------------------------------------------------//
StatusCode TRTMonitoringRun3_Alg::fillTestTRTTrackHistograms(const TrackCollection& trackCollection) const {
//----------------------------------------------------------------------------------//
    ATH_MSG_VERBOSE("Filling Test TRT Tracks Histos");
    
    auto DriftTimeonTrkDist_B = Monitored::Scalar<float>("DriftTimeonTrkDist_B",0.0);
    auto WireToTrkPosition_E = Monitored::Scalar<float>("WireToTrkPosition_E",0.0);
    auto WireToTrkPosition_B = Monitored::Scalar<float>("WireToTrkPosition_B",0.0);
    auto TronTDist_B = Monitored::Scalar<float>("TronTDist_B",0.0);
    auto TronTDist_E = Monitored::Scalar<float>("TronTDist_E",0.0);
	
    
    //Initialize a bunch of stuff before looping over the track collection. Fill some basic histograms.
    auto p_trk = trackCollection.begin();
    const Trk::Perigee *mPer = nullptr;
    const DataVector<const Trk::TrackParameters> *AllTrkPar(0);
    DataVector<const Trk::TrackParameters>::const_iterator p_trkpariter;

    int ntrackstack[2][64];

    for (int ibe = 0; ibe < 2; ibe++) {
        std::fill(ntrackstack[ibe], ntrackstack[ibe] + 64, 0);
    }

    for (; p_trk != trackCollection.end(); ++p_trk) {

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
        float p       =  (mPer->parameters()[Trk::qOverP] != 0.) ? std::abs(1. / (mPer->parameters()[Trk::qOverP])) : 10e7;
        float pT      =  (p * std::sin(theta));
        pT = pT * 1e-3;  // GeV

        if (p < m_minP) continue;

        const DataVector<const Trk::TrackStateOnSurface> *trackStates = (**p_trk).trackStateOnSurfaces();

        if (trackStates == 0) continue;

        DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItBegin0    = trackStates->begin();
        DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItBegin     = trackStates->begin();
        DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItBeginTemp = trackStates->begin();
        DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItEnd       = trackStates->end();
       
        int checkB[2] = {0, 0};
        int checkEC[2] = {0, 0};
        int checkEC_B[2] = {0, 0};
        int nTRTHits_side[2][2];
        int nTRTHitsW_perwheel[2][18];

        for (int ibe = 0; ibe < 2; ibe++) {
            for (int iside = 0; iside < 2; iside++) {
                nTRTHits_side[ibe][iside] = -1;
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

            //restrict ourselves to the inner most TRT layers To get detector phi.
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
            //select a TSOS which is non-empty, measurement type and contains  both drift circle and track parameters informations
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
            int ibe = std::abs(barrel_ec) - 1;
            int iside = barrel_ec > 0 ? 0 : 1;
            int thisStrawNumber[2] = {-1, -1};

            if (ibe == 0) {
                thisStrawNumber[ibe] = strawNumber(straw, straw_layer, layer_or_wheel);

                if (thisStrawNumber[ibe] >= 0 && thisStrawNumber[ibe] < s_Straw_max[ibe]) {
                    thisStrawNumber[ibe] = thisStrawNumber[ibe];
                }
            } else if (ibe == 1) {
                thisStrawNumber[ibe] = strawNumberEndCap(straw, straw_layer, layer_or_wheel, phi_module, barrel_ec);

                if (thisStrawNumber[ibe] >= 0 && thisStrawNumber[ibe] < s_Straw_max[ibe]) {
                    thisStrawNumber[ibe] = thisStrawNumber[ibe];
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
            }

            

            Identifier surfaceID;
            const Trk::MeasurementBase *mesb = (*TSOSItBegin)->measurementOnTrack();
            surfaceID = trtCircle->identify();
            // assume always Xe if m_ArgonXenonSplitter is not enabled, otherwise check the straw status (good is Xe, non-good is Ar)
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
            }
            
            const InDet::TRT_DriftCircle *RawDriftCircle = dynamic_cast<const InDet::TRT_DriftCircle *>(trtCircle->prepRawData());
            bool isTubeHit = (mesb->localCovariance()(Trk::locX, Trk::locX) > 1.0) ? 1 : 0;

            if (RawDriftCircle) {
                nTRTHits_side[ibe][iside]++;

                if (m_doShift && m_doStraws) {
                    if (ibe == 0) {
                        DriftTimeonTrkDist_B = RawDriftCircle->rawDriftTime();
                        fill("testTracksHistograms" + std::to_string(ibe),DriftTimeonTrkDist_B);
                    }
                }
 
                float locR = aTrackParam->parameters()[Trk::driftRadius];

                if (isTubeHit) {
                    if (m_doShift) {
                        if (ibe == 0) {
                            WireToTrkPosition_B = locR;
                            fill("testTracksHistograms" + std::to_string(ibe),WireToTrkPosition_B);
                        } else if (ibe == 1) {
                            WireToTrkPosition_E = locR;
                            fill("testTracksHistogramsIside" + std::to_string(ibe) + std::to_string(iside),WireToTrkPosition_E);
                        }
                    }
                }
                
                const int trailingEdge = RawDriftCircle->trailingEdge();
                float trailingEdgeScaled = (trailingEdge + 1) * 3.125;
                
                if (m_doShift && m_doStraws) {
                        if (RawDriftCircle->driftTimeValid()) {
                            if (ibe == 0) {
                                    TronTDist_B = trailingEdgeScaled;
                                    fill("testTracksHistograms" + std::to_string(ibe),TronTDist_B);
                            } else if (ibe == 1) {
                                    TronTDist_E = trailingEdgeScaled;
                                    fill("testTracksHistogramsIside" + std::to_string(ibe) + std::to_string(iside),TronTDist_E);
                            }
                        }
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}


StatusCode TRTMonitoringRun3_Alg::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored

    // Set the values of the monitored variables for the event

    ATH_MSG_VERBOSE("Monitoring Histograms being filled");

    // Retrieve containers/objects only once per event
    // Dereference handles to pass them to methods
    SG::ReadHandle<TrackCollection>     trackCollection{m_trackCollectionKey, ctx};
    SG::ReadHandle<TrackCollection>     combTrackCollection{m_combTrackCollectionKey, ctx};
    SG::ReadHandle<xAOD::EventInfo>     xAODEventInfo{m_xAODEventInfoKey, ctx};

    if (!xAODEventInfo.isValid()) {
        ATH_MSG_ERROR("Could not find event info object " << m_xAODEventInfoKey.key() <<
                      " in store");
        return StatusCode::FAILURE;
    }

    if (m_doRDOsMon) {
      SG::ReadHandle<InDetTimeCollection> trtBCIDCollection{m_TRT_BCIDCollectionKey, ctx};
        if (!trtBCIDCollection.isValid()) {
			ATH_MSG_INFO("Could not find BCID collection " << m_TRT_BCIDCollectionKey.key() << " in store");
        }
    }
	
    if (m_doTracksMon) {
        if (!trackCollection.isValid()) {
            ATH_MSG_ERROR("Could not find track collection " << m_trackCollectionKey.key() << " in store");
            return StatusCode::FAILURE;
        }

        ATH_CHECK( fillTestTRTTrackHistograms(*trackCollection) );
    }

    if (m_doEfficiency) {
        if (!combTrackCollection.isValid()) {
            ATH_MSG_ERROR("Could not find track collection " << m_combTrackCollectionKey.key() <<
                          " in store");
            return StatusCode::FAILURE;
        }
    }

    return StatusCode::SUCCESS;
}
