/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTMONITORINGRUN3_TOOL_H
#define TRTMONITORINGRUN3_TOOL_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/HistogramDef.h"

#include "TRandom3.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/StatusCode.h"

// Data handles
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkTrack/TrackCollection.h"
#include "CommissionEvent/ComTime.h"
#include "xAODTrigger/TrigDecision.h"
#include "xAODEventInfo/EventInfo.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

// Tool interfaces
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"
#include "TRT_ConditionsServices/ITRT_CalDbTool.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

// STDLIB
#include <string>
#include <vector>
#include <set>

namespace Trk {
	class ITrackHoleSearchTool;
    class Track;
    class TrackStateOnSurface;
    class ITrackSummaryTool;
}

namespace InDetDD {
	class TRT_DetectorManager;
}
 
class AtlasDetectorID;
class TRT_ID;
class Identifier;
class ComTime;
class IInDetConditionsSvc;
class ITRT_CalDbTool;
class ITRT_StrawStatusSummaryTool;
class ITRT_ConditionsSvc;
class ITRT_DAQ_ConditionsSvc;
class ITRT_ByteStream_ConditionsSvc;
class ITRT_StrawNeighbourSvc;
class ITRT_DriftFunctionTool;

class TRTMonitoringRun3_Tool : public AthMonitorAlgorithm {
public:
    TRTMonitoringRun3_Tool( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TRTMonitoringRun3_Tool();
    virtual StatusCode initialize() override;
	virtual StatusCode fillTestTRTTrackHistograms( const TrackCollection& trackCollection) const;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
private:
	static const int s_numberOfBarrelStacks;
	static const int s_numberOfEndCapStacks;

	static const int s_Straw_max[2];
	static const int s_iStack_max[2];
	static const int s_iChip_max[2];
	static const int s_numberOfStacks[2];
	static const int s_moduleNum[2];
	
		
	int m_lastLumiBlock;
	int m_evtLumiBlock;
	int m_good_bcid;
	int m_nTotalTracks;
	int m_nTracksB[2];
	int m_nTracksEC[2];
	int m_nTracksEC_B[2];
	float m_nBSErrors[195];
	float m_nRobErrors[195];
	std::vector<unsigned int> m_rodMap;
	bool m_passEventBurst;
	bool m_ArgonXenonSplitter;
	enum GasType{ Xe = 0, Ar = 1, Kr = 2 };
	
private:	
	
	float m_HTfraconTrack_B[32];
	float m_LonTrack_B[32];
	int m_nTrack_B[32];
	int m_nTrackwithHL_B[32];//obsolete

	float m_HTfraconTrack_E[64];
	float m_LonTrack_E[64];
	int m_nTrack_E[64];
	int m_nTrackwithHL_E[64];//obsolete

	bool m_doRDOsMon;
	bool m_doGeoMon;
	bool m_doTracksMon;
	int m_usedEvents;
	int m_nTRTHits[2];
	int m_totalEvents;

	bool m_doDCS;
	bool m_doASide;
	bool m_doCSide;
	bool m_doStraws;
	bool m_doChips;
	bool m_doShift;
	bool m_doExpert;
	bool m_doMaskStraws;
	bool m_doEfficiency;
	bool m_doDiagnostic;
	bool m_useHoleFinder;//switch for hole finder

	int m_LE_timeWindow_MIN;
	int m_LE_timeWindow_MAX;

	int m_LL_timeWindow_MIN;
	int m_LL_timeWindow_MAX;

	int m_HL_timeWindow_MIN;
	int m_HL_timeWindow_MAX;

	bool m_NoiseSuppressionLevel_30pc;
	int m_MIN_N_LL_Hits;
	int m_MIN_TOT_Hits;
	bool m_NoiseSuppressionMap;

	float m_EventPhaseScale;


	unsigned char m_mat_chip_B[64][1642];
	int m_nStrawHits_B[1642];

	unsigned char m_mat_chip_E[64][3840];
	int m_nStrawHits_E[2][3840];

	float m_DriftTimeonTrkDistScale_B;
	float m_HLhitOnTrackScale_B;
	float m_HtoLRatioOnTrackScale_B;
	float m_HtoLRatioOnTrackScale_B_Ar;
	float m_HtoLRatioOnTrackScale_B_Xe;
	float m_NumSwLLWoTScale_B;
	float m_WireToTrkPositionScale_B;
	float m_WireToTrkPositionScale_B_Ar;
	float m_TronTDistScale_B;
	float m_ResidualScale_B;
	float m_ResidualScale_B_20GeV;
	float m_TimeResidualScale_B;
	float m_DriftTimeonTrkDistScale_B_Ar;
	float m_TronTDistScale_B_Ar;
	float m_ResidualScale_B_Ar;
	float m_ResidualScale_B_Ar_20GeV;
	float m_TimeResidualScale_B_Ar;
	float m_nTrkvPhiScale_B;//obsolete
	int m_nTrksperLB_B;
	int m_nHitsperLB_B;
	int m_nHLHitsperLB_B;

	float m_DriftTimeonTrkDistScale_E[2];
	float m_HLhitOnTrackScale_E[2];
	float m_HtoLRatioOnTrackScale_E[2];
	float m_HtoLRatioOnTrackScale_E_Ar[2];
	float m_HtoLRatioOnTrackScale_E_Xe[2];
	float m_NumSwLLWoTScale_E[2];
	float m_WireToTrkPositionScale_E[2];
	float m_WireToTrkPositionScale_E_Ar[2];
	float m_TronTDistScale_E[2];
	float m_ResidualScale_E[2];
	float m_ResidualScale_E_20GeV[2];
	float m_TimeResidualScale_E[2];
	float m_DriftTimeonTrkDistScale_E_Ar[2];
	float m_TronTDistScale_E_Ar[2];
	float m_ResidualScale_E_Ar[2];
	float m_ResidualScale_E_Ar_20GeV[2];
	float m_TimeResidualScale_E_Ar[2];
	float m_nTrkvPhiScale_E[2];//obsolete
	int m_nTrksperLB_E[2];
	int m_nHitsperLB_E[2];
	int m_nHLHitsperLB_E[2];


	float m_maxDistToStraw;
	float m_DistToStraw;
	bool m_trt_only_trks;
	bool m_zero_field;
	bool m_DEBUG;//obsolete
	bool m_printEventInfo;//obsolete
	float m_longToTCut;
	int m_nphi_bins;
	int m_EventBurstCut;
	int m_lumiBlocksToResetOcc;
	bool m_isCosmics;
	int m_minTRThits;
	float m_minP;
	int m_flagforscale;

	///// Additional stuff for efficiency measurements, online only for now
	std::string m_track_collection_hole_finder;
	float m_max_abs_d0;
	float m_max_abs_z0;
	float m_min_pT;
	float m_max_abs_eta;

	int m_min_si_hits;
	int m_min_pixel_hits;
	int m_min_sct_hits;
	int m_min_trt_hits;

	float m_track_pt;
	float m_track_eta;
	float m_track_phi;
	float m_track_d0;
	float m_track_z0;
	int m_min_tracks_straw;

	int m_every_xth_track;
	std::string m_datatype;
	

	const AtlasDetectorID* m_idHelper;

	Gaudi::Property<bool> m_doRandom {this,"RandomHist",false}; //< Trigger chain string pulled from the job option and parsed into a vector?

	int strawLayerNumber(int strawLayerNumber, int LayerNumber) const;
	int strawNumber(int strawNumber, int strawlayerNumber, int LayerNumber) const;
	int strawNumberEndCap(int strawNumber, int strawLayerNumber, int LayerNumber, int phi_stack, int side) const;
	float radToDegrees(float radValue) const;

    // Services
	ToolHandle<ITRT_StrawStatusSummaryTool>      m_sumTool;
    
	const TRT_ID *m_pTRTHelper;
	const InDetDD::TRT_DetectorManager* m_mgr;

	// Data handles
    SG::ReadHandleKey<TRT_RDO_Container>   m_rdoContainerKey       {this, "TRTRawDataObjectName", "TRT_RDOs", "Name of TRT RDOs container"};
	SG::ReadHandleKey<TrackCollection>     m_trackCollectionKey    {this, "TRTTracksObjectName", "Tracks", "Name of tracks container"};
	SG::ReadHandleKey<TrackCollection>     m_combTrackCollectionKey{this, "track_collection_hole_finder", "CombinedInDetTracks", "Name of tracks container used for hole finder"};
	
    SG::ReadHandleKey<xAOD::EventInfo>     m_xAODEventInfoKey      {this, "xAODEventInfo", "EventInfo", "Name of EventInfo object"};
	SG::ReadHandleKey<InDetTimeCollection> m_TRT_BCIDCollectionKey {this, "TRTBCIDCollectionName",        "TRT_BCID",            "Name of TRT BCID collection"};

};
#endif
