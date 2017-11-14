// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_MONITORING_TOOL_H
#define TRT_MONITORING_TOOL_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "GaudiKernel/StatusCode.h"

#include "LumiBlockComps/ILuminosityTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "TrkTrack/TrackCollection.h"
#include "EventInfo/EventInfo.h"
#include "CommissionEvent/ComTime.h"
#include "xAODTrigger/TrigDecision.h"
#include "xAODEventInfo/EventInfo.h"

#include <string>
#include <vector>
#include <set>

class TProfile;
class TH1F_LW;
class TH2F_LW;
class TProfile_LW;
class TH1D_LW;
class LWHist1D;

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
class ITRT_CalDbSvc;
class ITRT_StrawStatusSummarySvc;
class ITRT_DCS_ConditionsSvc;
class ITRT_DAQ_ConditionsSvc;
class ITRT_ByteStream_ConditionsSvc;
class ITRT_ConditionsSvc;
class ITRT_StrawNeighbourSvc;
class ITRT_DriftFunctionTool;

class TRT_Monitoring_Tool : public ManagedMonitorToolBase {
public:
	TRT_Monitoring_Tool(const std::string &type, const std::string &name, const IInterface *parent);
	virtual ~TRT_Monitoring_Tool();
	virtual StatusCode initialize();
	virtual StatusCode bookHistogramsRecurrent();
	virtual StatusCode fillHistograms();
	virtual StatusCode procHistograms();

private:
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

	const AtlasDetectorID * m_idHelper;
	StatusCode Retrieve_TRT_RDOs(); // NOTE: will be removed
	StatusCode Retrieve_TRT_Tracks(); // NOTE: will be removed

	StatusCode bookTRTRDOs(bool isNewLumiBlock, bool isNewRun);
	StatusCode bookTRTTracks(bool isNewLumiBlock, bool isNewRun);
	StatusCode bookTRTShiftTracks(bool isNewLumiBlock, bool isNewRun);
	StatusCode bookTRTEfficiency(bool isNewLumiBlock, bool isNewRun);
	StatusCode checkEventBurst(const TRT_RDO_Container& rdoContainer);
	StatusCode fillTRTRDOs(const TRT_RDO_Container& rdoContainer,
	                       const InDetTimeCollection& trtBCIDCollection,
	                       const xAOD::EventInfo& eventInfo);
	// ComTime might be missing from file, have to use const pointer
	StatusCode fillTRTTracks(const TrackCollection& trackCollection,
	                         const xAOD::TrigDecision& trigDecision,
	                         const ComTime* comTimeObject);
	StatusCode fillTRTEfficiency(const TrackCollection& combTrackCollection);
	StatusCode fillTRTHighThreshold(const TrackCollection& trackCollection,
	                                const xAOD::EventInfo& eventInfo);
	StatusCode checkTRTReadoutIntegrity(const xAOD::EventInfo& eventInfo);

	int strawLayerNumber(int strawLayerNumber, int LayerNumber);


	// Returns the straw number (0-1641) given the layerNumber, strawlayerNumber, and strawNumber, all gotten from Athena IDHelper
	int strawNumber(int strawNumber, int strawlayerNumber, int LayerNumber);
	// Returns Barrel Board Number
	int chipToBoard(int chip);
	// Returns EndCap Board Number
	int chipToBoard_EndCap(int chip);

	// Returns Degrees, converted from radians (Athena Standard units)
	float radToDegrees(float radValue);
	int strawNumber_reverse (int inp_strawnumber,  int* strawNumber, int* strawlayerNumber, int* LayerNumber);
	int strawLayerNumber_reverse(int strawLayerNumInp,int* strawLayerNumber, int* LayerNumber);

	TH1F_LW* bookTH1F_LW(MonGroup& mongroup, const std::string &hName, const std::string &hTitle, int bins, double lowbin, double highbin, const std::string &xTitle, const std::string &yTitle, StatusCode &scode);
	TH2F_LW* bookTH2F_LW(MonGroup& mongroup, const std::string &hName, const std::string &hTitle, int xbins, double lowxbins, double highxbins, int ybins, double lowybins, double highybins, const std::string &xTitle, const std::string &yTitle, StatusCode &scode);
	TProfile_LW* bookTProfile_LW(MonGroup& mongroup, const std::string &hName, const std::string &hTitle, int bins, double lowbin, double highbin, double ymin, double ymax, const std::string &xTitle, const std::string &yTitle, StatusCode &scode);
	TProfile*       bookTProfile(MonGroup& mongroup, const std::string &hName, const std::string &hTitle, int bins, double lowbin, double highbin, double ymin, double ymax, const std::string &xTitle, const std::string &yTitle, StatusCode &scode);
	TH1D_LW* bookTH1D_LW(MonGroup& mongroup, const std::string &hName, const std::string &hTitle, int bins, double lowbin, double highbin, const std::string &xTitle, const std::string &yTitle, StatusCode &scode);

	template<typename T>
	StatusCode trtRegHist(T *hist, MonGroup &mongrp, const char *hName) {
		StatusCode scode = mongrp.regHist(hist);
		if (scode == StatusCode::FAILURE) {
			ATH_MSG_FATAL("Failed to register histogram " << hName);
		}
		return scode;
	}

private:
	static const int s_numberOfBarrelStacks;
	static const int s_numberOfEndCapStacks;

	static const int s_Straw_max[2];
	static const int s_iStack_max[2];
	static const int s_iChip_max[2];
	static const int s_numberOfStacks[2];
	static const int s_moduleNum[2];

	ServiceHandle<IToolSvc> p_toolSvc;
	ServiceHandle<ITRT_StrawStatusSummarySvc> m_sumSvc;
	ServiceHandle<ITRT_DCS_ConditionsSvc> m_DCSSvc;
	ServiceHandle<ITRT_DAQ_ConditionsSvc> m_DAQSvc;
	ServiceHandle<ITRT_ByteStream_ConditionsSvc> m_BSSvc;
	ServiceHandle<ITRT_ConditionsSvc> m_condSvc_BS;
	ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc;
	ServiceHandle<ITRT_CalDbSvc> m_TRTCalDbSvc;

	SG::ReadHandleKey<TRT_RDO_Container> m_rdoContainerKey{this, "TRTRawDataObjectName", "TRT_RDOs", "Name of TRT RDOs container"};
	SG::ReadHandleKey<TrackCollection> m_trackCollectionKey{this, "TRTTrackObjectName", "Tracks", "Name of tracks container"};
	// NOTE: this property is not used anywhere, is it ok to change its name?
	SG::ReadHandleKey<TrackCollection> m_combTrackCollectionKey{this, "track_collection_hole_finder", "CombinedInDetTracks", "Name of tracks container used for hole finder"};
	SG::ReadHandleKey<EventInfo> m_eventInfoKey{this, "EventInfo", "ByteStreamEventInfo", "Name of EventInfo object"};
	SG::ReadHandleKey<xAOD::EventInfo> m_xAODEventInfoKey{this, "xAODEventInfo", "EventInfo", "Name of EventInfo object"};
	SG::ReadHandleKey<InDetTimeCollection> m_TRT_BCIDCollectionKey{this, "TRTBCIDCollectionName", "TRT_BCID", "Name of TRT BCID collection"};
	SG::ReadHandleKey<ComTime> m_comTimeObjectKey{this, "ComTimeObjectName", "TRT_Phase", "Name of ComTime object"};
	SG::ReadHandleKey<xAOD::TrigDecision> m_trigDecisionKey{this, "TrigDecisionObjectName", "xTrigDecision", "Name of trigger decision object"};

	const TRT_ID* m_pTRTHelper;
	const InDetDD::TRT_DetectorManager *m_mgr;

	ToolHandle<Trk::ITrackSummaryTool> m_TrackSummaryTool;
	ToolHandle<ITRT_DriftFunctionTool> m_drifttool;

	const TRT_RDO_Container* m_rdoContainer;
	std::string m_rawDataObjectName;

	const DataVector<Trk::Track> *m_trkCollection;
	std::string m_tracksObjectName;

	const InDetTimeCollection *m_TRT_BCIDColl;

	const ComTime *m_theComTime;
	const EventInfo* m_eventInfo;

	std::string m_comTimeObjectName;
	std::string m_geo_summary_provider;//obsolete
	std::string m_mapPath;

	int m_rbins;
	float m_rmin;
	float m_rmax;
	float m_tbins;
	float m_tmin;
	float m_tmax;
	float m_fitmin;
	float m_fitmax;

	TH1F_LW* m_hSummary;
	TProfile* m_hChipBSErrorsVsLB[2][2];
	TProfile* m_hRobBSErrorsVsLB[2][2];

	TProfile_LW* m_hAvgHLOcc_side[2][2];
	TProfile_LW* m_hAvgLLOcc_side[2][2];

	TH1F_LW* m_hNumTrksDetPhi_B;//hNTrksvsPhi
	TProfile_LW* m_hNumHoTDetPhi_B;//hNumHoTPhi
	TProfile_LW* m_hAvgTroTDetPhi_B;//hAvgTrEdgeonTrack
	TProfile_LW* m_hAvgTroTDetPhi_B_Ar;
	TH1F_LW* m_hNumSwLLWoT_B;//hLLHitsonTrk
	TProfile_LW* m_hStrawEffDetPhi_B;//hStrawEffDetPhi
	TH1F_LW* m_hHitWMap_B;
	TH1F_LW* m_hHitWonTMap_B;
	TH1F_LW* m_Pull_Biased_Barrel;
	TH1F_LW* m_hResidual_B;//hHitToTrkDistance
	TH1F_LW* m_hResidual_B_20GeV;//hHitToTrkDistance
	TH1F_LW* m_hTimeResidual_B;//hHitToTrkDistance
	TH1F_LW* m_hDriftTimeonTrkDist_B;//hDriftTimeonTrk
	TH1F_LW* m_hTronTDist_B;//hTronT
	TH2F_LW* m_hrtRelation_B;//hrt
	TH1F_LW* m_hHLhitOnTrack_B;
	TH1F_LW* m_hHtoLRatioOnTrack_B;
	TH1F_LW* m_hHtoLRatioOnTrack_B_Ar;
	TH1F_LW* m_hHtoLRatioOnTrack_B_Xe;
	TH1F_LW* m_hWireToTrkPosition_B;//hWiretoTrkPosition
	TH1F_LW* m_hWireToTrkPosition_B_Ar;
	TH1F_LW* m_hResVsDetPhi_B;
	TProfile* m_hNHitsperLB_B;
	TProfile* m_hNTrksperLB_B;
	TProfile* m_hNHLHitsperLB_B;

	TH1F_LW* m_hDriftTimeonTrkDist_B_Ar;//hDriftTimeonTrk
	TH1F_LW* m_hTronTDist_B_Ar;//hTronT
	TH1F_LW* m_hHitWMap_B_Ar;
	TH1F_LW* m_hResidual_B_Ar;
	TH1F_LW* m_hResidual_B_Ar_20GeV;
	TH1F_LW* m_hTimeResidual_B_Ar;
	TH2F_LW* m_hrtRelation_B_Ar;//hrt

	TH1F_LW* m_hNumTrksDetPhi_E[2];//hNTrksvsPhi
	TProfile_LW* m_hNumHoTDetPhi_E[2];//hNumHoTPhi
	TProfile_LW* m_hAvgTroTDetPhi_E[2];//hAvgTrEdgeonTrack
	TProfile_LW* m_hAvgTroTDetPhi_E_Ar[2];
	TH1F_LW* m_hNumSwLLWoT_E[2];//hLLHitsonTrk
	TProfile_LW* m_hStrawEffDetPhi_E[2];//hStrawEffDetPhi
	TH1F_LW* m_hHitWMap_E[2];
	TH1F_LW* m_hHitWonTMap_E[2];
	TH1F_LW* m_Pull_Biased_EndCap;
	TH1F_LW* m_hResidual_E[2];//hHitToTrkDistance
	TH1F_LW* m_hResidual_E_20GeV[2];//hHitToTrkDistance
	TH1F_LW* m_hTimeResidual_E[2];//hHitToTrkDistance
	TH1F_LW* m_hDriftTimeonTrkDist_E[2];//hDriftTimeonTrk
	TH1F_LW* m_hTronTDist_E[2];//hTronT
	TH2F_LW* m_hrtRelation_E[2];//hrt
	TH1F_LW* m_hHLhitOnTrack_E[2];
	TH1F_LW* m_hHtoLRatioOnTrack_E[2];
	TH1F_LW* m_hHtoLRatioOnTrack_E_Ar[2];
	TH1F_LW* m_hHtoLRatioOnTrack_E_Xe[2];
	TH1F_LW* m_hWireToTrkPosition_E[2];//hWiretoTrkPosition
	TH1F_LW* m_hWireToTrkPosition_E_Ar[2];
	TH1F_LW* m_hResVsDetPhi_E[2];
	TProfile* m_hNHitsperLB_E[2];
	TProfile* m_hNTrksperLB_E[2];
	TProfile* m_hNHLHitsperLB_E[2];

	TH1F_LW* m_hTronTDist_E_Ar[2];//hTronT
	TH1F_LW* m_hDriftTimeonTrkDist_E_Ar[2];//hDriftTimeonTrk
	TH1F_LW* m_hHitWMap_E_Ar[2];
	TH1F_LW* m_hResidual_E_Ar[2];
	TH1F_LW* m_hResidual_E_Ar_20GeV[2];
	TH1F_LW* m_hTimeResidual_E_Ar[2];
	TH2F_LW* m_hrtRelation_E_Ar[2];//hrt

	TProfile_LW* m_hStrawsEff[2][64];

	TProfile_LW* m_hEvtPhaseDetPhi_B;
	TProfile_LW* m_hEvtPhaseDetPhi_E[2];
	TH1F_LW* m_hEvtPhase;
	TH2F_LW* m_hEvtPhaseVsTrig;// evt phase vs trig item
	TProfile_LW* m_hChipsEff[2][64];

	TH1F_LW* m_hHitOnTrackVsAllS[2][64];
	TH1F_LW* m_hHitOnTrackVsAllC[2][64];

	/** Leading Edge in time Window: Straws.
	 * Any hit where the leading edge (driftTimeBin()) is less than 24.
	 * In Time window means ((driftTimeBin<24) && !lastBinHigh && !firstBinHigh)
	 * This is an RDO level histogram.
	 */
	TH1F_LW* m_hHitWMapS[2][64];

	/** TE in Time Window: Straws.
	 * If hit trailing edge is in time window.
	 * In Time window means ((trailingEdge<23)&& !lastBinHigh && !firstBinHigh)
	 * m_hHitTrWMapS[m_phi_module]->Fill(m_strawNumber, ((m_trailingEdge+1)*3.125));
	 */
	TProfile_LW* m_hHitTrWMapS[2][64];

	/** Mean TE: Straws.
	 * Average of Trailing Edge bin where the trailing edge (trailingEdge()) is less than 23.
	 * This an RDO level histogram.
	 */
	TProfile_LW* m_hHitTrMapS[2][64];

	/** Any LL bit on: Straws
	 * if any low threshold bit was set.
	 * if leading edge is > 0, or if trailing edge < 23, or if first bin is high, or if last bin is high
	 * This is an RDO level histogram.
	 */
	TH1F_LW* m_hHitAMapS[2][64];

	/** LL in time window: Straws
	 * Any low level hit in time window by straw.
	 * ((m_driftTimeBin>0 || m_trailingEdge<23)&& !m_firstBinHigh && !m_lastBinHigh)
	 * This is an RDO level histogram.
	 */
	TH1F_LW* m_hHitAWMapS[2][64];

	/** HL/LL: Straws
	 * The ratio of High Level hits to Low Level hits per straw.
	 * This is an RDO level histogram.
	 */
	TH1F_LW* m_hHtoLMapS[2][64]; // not filled

	/** Mean ToT (ns): Straws.
	 * Average Time over Threshold (ToT) in nano seconds per straw.
	 * m_hHitToTMapS[m_phi_module]->Fill(m_strawNumber, m_timeOverThreshold);
	 * m_timeOverThreshold = (p_lolum)->timeOverThreshold();
	 * This is an RDO level histogram.
	 */
	TProfile_LW* m_hHitToTMapS[2][64];

	/** Mean ToT (ns) for straws with ToT > LongToTCut: Straws.
	 * Average Time over Threshold (ToT) in nano seconds per straw for straws with ToT > LongToTCut.
	 * m_hHitToTLongMapS[m_phi_module]->Fill(m_strawNumber, m_timeOverThreshold);
	 * m_timeOverThreshold = (p_lolum)->timeOverThreshold();
	 * This is an RDO level histogram.
	 */
	TProfile_LW* m_hHitToTLongMapS[2][64];

	/** Mean Trailing Edge (ns) for straws with ToT > LongToTCut: Straws.
	 * Average Trailing Edge (Tr) in nano seconds per straw for straws with ToT > LongToTCut.
	 * m_hHitToTLongTrMapS[m_phi_module]->Fill(m_strawNumber, m_trailingEdge);
	 * m_trailingEdge = (p_lolum)->trailingEgde();
	 * This is an RDO level histogram.
	 */
	TProfile_LW* m_hHitToTLongTrMapS[2][64];

	/** High Level: Straws
	 * If a hit has any of the high threshold time bins up.
	 * m_hHitHMapS[m_phi_module]->Fill(m_strawNumber);
	 * This is an RDO level histogram
	 */
	TH1F_LW* m_hHitHMapS[2][64];

	/** HL in time window: Straws
	 * If a hit has any of the high threshold time bins up, and is in the time window. ((m_driftTimeBin<24) && !m_firstBinHight && !m_lastBinHight)
	 * This is an RDO level histogram
	 */
	TH1F_LW* m_hHitHWMapS[2][64];

	/** LE in time window on track: Straws.
	 * Leading Edge (LE) is within first 23 time bins of read out from a hit associated with a track.
	 * This is track level histogram.
	 */
	TH1F_LW* m_hHitWonTMapS[2][64];

	/** Mean TE on track: Straws
	 * Average Trailing Edge(TE) from a hit associated with a track.
	 * This is a track level histogram.
	 */
	TProfile_LW* m_hHitTronTMapS[2][64];

	/** Any LL bit on track: Straws
	 * Any low level bit is set from hit associated with a track.
	 * This is a track level hit
	 */
	TH1F_LW* m_hHitAonTMapS[2][64];

	/** Any LL bit in time window on track: Straws
	 * Any low level (LL) bit set and is in time window from hits associated with tracks
	 * This is a track level histogram.
	 */
	TH1F_LW* m_hHitAWonTMapS[2][64];

	/** Any HL hit on track: Straws
	 * Any straw with a High Threshold (HL) hit associated with a track.
	 * This is a track level histogram
	 */
	TH1F_LW* m_hHitHWonTMapS[2][64];
	//

	TH1F_LW* m_hHitHonTMapS[2][64];

	/** HL in time window on track: Straws
	 * Straws with a High Threshold hit associated with a track and the hit is in the time window.
	 * This is a track level histogram.
	 */
	TH1F_LW* m_hHtoLonTMapS[2][64]; //not filled

	//
	TH1F_LW* m_hHtoLWonTMapS[2][64]; //not filled

	/** Mean ToT (ns) on Track: Straws
	 * Average Time over Threshold (ToT) from a straw hit associated with a track.
	 * This is a track level histogram.
	 */
	TProfile_LW* m_hHitToTonTMapS[2][64];

	/** Mean TE on track (with Event Phase correction): Straws.
	 * Average trailing edge(TE) on track after correcting for event phase from a hit associated with a track.
	 * This is a track level histogram.
	 */
	TProfile_LW* m_hHitTronTwEPCMapS[2][64];

	/** Valid Raw Drift Time on Track.
	 * Staws with hits that have valid drift times and are associated with a track.
	 * This is a track level histogram.
	 */
	TProfile_LW* m_hValidRawDriftTimeonTrk[2][64];
	TProfile_LW* m_hValidRawDriftTimeonTrkC[2][64];
	TH1F_LW* m_hHitWMapC[2][64];
	TProfile_LW* m_hHitTrMapC[2][64];
	TProfile_LW* m_hHitTrWMapC[2][64];
	TH1F_LW* m_hHitAMapC[2][64];
	TH1F_LW* m_hHitAWMapC[2][64];
	TH1F_LW* m_hHtoLMapC[2][64]; // not filled

	TH2F_LW* m_hHtoBCMapC[2][64];
	TH2F_LW* m_hHtoBCMapB[2][64];
	TProfile_LW* m_hHitToTMapC[2][64];
	TH1F_LW* m_hHitHMapC[2][64];
	TH1F_LW* m_hHitHWMapC[2][64];

	/** LE in time window on track: Chips.
	 * Leading Edge (LE) from a hit associated with a track is within first 23 time bins.
	 * Plotted as a function of DTMROC.
	 * This is a track level histogram.
	 */
	TH1F_LW* m_hHitWonTMapC[2][64];
	TProfile_LW* m_hHitTronTMapC[2][64];
	TH1F_LW* m_hHitAonTMapC[2][64];
	TH1F_LW* m_hHitAWonTMapC[2][64];
	TH1F_LW* m_hHitHonTMapC[2][64];
	TH1F_LW* m_hHitHWonTMapC[2][64];
	TH1F_LW* m_hHtoLonTMapC[2][64]; // not filled
	TH1F_LW* m_hHtoLWonTMapC[2][64]; //
	TProfile_LW* m_hHitToTonTMapC[2][64];
	TProfile_LW* m_hHitTronTwEPCMapC[2][64];

	TProfile_LW* m_hHitToTrkDistanceMapS_E[64];

	/** Avg. Occupancy: Modules (Side A&C)
	 * Average Occupancy per Phi Module on Side A(&C).
	 * This is an RDO level Histogram.
	 */
	TProfile_LW* m_hAvgLLOccMod_side[2][2];
	TProfile_LW* m_hAvgHLOccMod_side[2][2];
	TProfile_LW* m_hBCIDvsOcc[2];

	/** Avg. Occupancy: Modules (Side A and C)
	 * Average Occupancy per Phi Module on Side A and C.
	 * This is an RDO level Histogram.
	 */
	TH1F_LW* m_hChipOcc[2][64];
	TH1F_LW* m_hStrawOcc[2][64];

	/** Anatoli's "Scatter histograms" **
	 ** Monitor quantities as a function of lumi block. Fill per every stack
	 */
	TH2F_LW* m_hHitsOnTrack_Scatter[2];
	TH2F_LW* m_hLLOcc_Scatter[2];
	TH2F_LW* m_hHightoLowRatioOnTrack_Scatter[2];
	TH1F_LW* m_hOccAll;
	//Vector for normalizing probabilities of hHitWmap histograms (Leading Edge in Time Window probability per straw number)
	std::vector<float>  m_scale_hHitWMap_B;
	std::vector<float>  m_scale_hHitWMap_B_Ar;
	/* Helpers for the scatter histograms - 32 stacks (do same for both side for now) */
	float m_LLOcc[2][64]; // easy to keep occupancy separately for sides A&C, so let's do that
	float m_HLOcc[2][64]; // easy to keep occupancy separately for sides A&C, so let's do that

	/**Initialize Aging plots**
	 **HT, All, Barrel, EC, In/A, Out/C, etc...
	 */

	TH1F_LW* m_trackz_All[5][2];//({L1 Long, L2, L3, L1 short Pos, L2 Short Neg},{A,C})
	TH1F_LW* m_trackz_HT[5][2];

	TH1F_LW* m_trackr_All[4][2]; // ({In_A,In_B,Out_A,Out_B},{A,C})
	TH1F_LW* m_trackr_HT[4][2];
	TH1F_LW* m_IntLum;
	TH1F_LW* m_LBvsLum;
	TH1F_LW* m_LBvsTime;

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
	int nTRTHits[2];
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

	float EventPhaseScale;


	unsigned char mat_chip_B[64][1642];
	int m_nStrawHits_B[1642];

	unsigned char mat_chip_E[64][3840];
	int m_nStrawHits_E[2][3840];

	float DriftTimeonTrkDistScale_B;
	float HLhitOnTrackScale_B;
	float HtoLRatioOnTrackScale_B;
	float HtoLRatioOnTrackScale_B_Ar;
	float HtoLRatioOnTrackScale_B_Xe;
	float NumSwLLWoTScale_B;
	float WireToTrkPositionScale_B;
	float WireToTrkPositionScale_B_Ar;
	float TronTDistScale_B;
	float ResidualScale_B;
	float ResidualScale_B_20GeV;
	float TimeResidualScale_B;
	float DriftTimeonTrkDistScale_B_Ar;
	float TronTDistScale_B_Ar;
	float ResidualScale_B_Ar;
	float ResidualScale_B_Ar_20GeV;
	float TimeResidualScale_B_Ar;
	float nTrkvPhiScale_B;//obsolete
	int nTrksperLB_B;
	int nHitsperLB_B;
	int nHLHitsperLB_B;

	float DriftTimeonTrkDistScale_E[2];
	float HLhitOnTrackScale_E[2];
	float HtoLRatioOnTrackScale_E[2];
	float HtoLRatioOnTrackScale_E_Ar[2];
	float HtoLRatioOnTrackScale_E_Xe[2];
	float NumSwLLWoTScale_E[2];
	float WireToTrkPositionScale_E[2];
	float WireToTrkPositionScale_E_Ar[2];
	float TronTDistScale_E[2];
	float ResidualScale_E[2];
	float ResidualScale_E_20GeV[2];
	float TimeResidualScale_E[2];
	float DriftTimeonTrkDistScale_E_Ar[2];
	float TronTDistScale_E_Ar[2];
	float ResidualScale_E_Ar[2];
	float ResidualScale_E_Ar_20GeV[2];
	float TimeResidualScale_E_Ar[2];
	float nTrkvPhiScale_E[2];//obsolete
	int nTrksperLB_E[2];
	int nHitsperLB_E[2];
	int nHLHitsperLB_E[2];

	/*
	  ToolHandle<Trk::IPropagator> m_propagatorTool;
	  Trk::IPropagator *m_propagator;
	  ToolHandle<Trk::IExtrapolator> m_extrapolatorTool;
	  Trk::IExtrapolator *m_extrapolator;
	*/ //obsolete
	float m_maxDistToStraw;
	float m_DistToStraw;
	bool m_trt_only_trks;
	bool m_zero_field;
	bool DEBUG;//obsolete
	bool m_printEventInfo;//obsolete
	float m_longToTCut;
	int m_nphi_bins;
	int m_EventBurstCut;
	int m_lumiBlocksToResetOcc;
	bool m_isCosmics;
	int m_minTRThits;
	float m_minP;
	void scale_LWHist(LWHist1D* hist, float scale);
	void scale_LWHistWithScaleVector(LWHist1D* hist, const std::vector<float>& scale);
	int m_initScaleVectors();
	int m_flagforscale;
	void divide_LWHist(TH1F_LW* result, TH1F_LW* a, TH1F_LW* b);

	///// Additional stuff for efficiency measurements, online only for now
	ToolHandle<Trk::ITrackHoleSearchTool>  m_trt_hole_finder;
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

	ToolHandle<ILuminosityTool>   m_lumiTool;


	TProfile_LW* m_hefficiency_eta;
	TProfile_LW* m_hefficiency_phi;
	TProfile_LW* m_hefficiency_pt;
	TProfile_LW* m_hefficiency_z0;
	TProfile_LW* m_hefficiencyBarrel_locR;
	TProfile_LW* m_hefficiencyBarrel_locR_Ar;
	TProfile_LW* m_hefficiencyEndCap_locR[2];
	TProfile_LW* m_hefficiencyEndCap_locR_Ar[2];

	TProfile_LW* m_hefficiencyMap[2]; // 0-barrel, 1-endcap
	TProfile_LW* m_hefficiencyS[2][64]; // 0-barrel, 1-endcap
	TProfile_LW* m_hefficiencyC[2][64]; // 0-barrel, 1-endcap
	TH1F_LW* m_hefficiency[2][2];
	TH1F_LW* m_hefficiencyIntegral[2][2];

	int strawNumberEndCap(int strawNumber, int strawLayerNumber, int LayerNumber, int phi_stack, int side);
	/////////
	//inline functions
	////////

	//it is taking lots of time to compile ?

	//Deciphers status HT to  GasType Enumerator
	inline GasType Straw_Gastype(int stat) {
		// getStatusHT returns enum {Undefined, Dead, Good, Xenon, Argon, Krypton}.
		// Our representation of 'GasType' is 0:Xenon, 1:Argon, 2:Krypton
		GasType Gas = Xe; // Xenon is default
		if (m_ArgonXenonSplitter) {
			//      int stat=m_sumSvc->getStatusHT(TRT_Identifier);
			if       ( stat==2 || stat==3 ) { Gas = Xe; } // Xe
			else if  ( stat==1 || stat==4 ) { Gas = Ar; } // Ar
			else if  ( stat==5 )            { Gas = Kr; } // Kr
			else if  ( stat==6 )            { Gas = Xe; } // emulate Ar (so treat as Xe here)
			else if  ( stat==7 )            { Gas = Xe; } // emulate Kr (so treat as Xe here)
			else { ATH_MSG_FATAL ("getStatusHT = " << stat << ", must be 'Good(2)||Xenon(3)' or 'Dead(1)||Argon(4)' or 'Krypton(5)!' or 6 or 7 for emulated types!");
				throw std::exception();
			}
		}
		return Gas;
	}


};

#endif
