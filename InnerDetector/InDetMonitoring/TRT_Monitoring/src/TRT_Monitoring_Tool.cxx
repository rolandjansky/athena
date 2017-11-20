/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_Monitoring/TRT_Monitoring_Tool.h"

#include "TRT_TrackHoleSearch/TRTTrackHoleSearchTool.h"
#include "DataModel/DataVector.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"//may be obsolete, TRT_ID includes this
#include "InDetIdentifier/TRT_ID.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"
#include "TRT_ConditionsServices/ITRT_DCS_ConditionsSvc.h"
#include "TRT_ConditionsServices/ITRT_DAQ_ConditionsSvc.h"
#include "TRT_ConditionsServices/ITRT_ByteStream_ConditionsSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"
#include "xAODTrigger/TrigDecision.h"

//#include "TrkParameters/TrackParameters.h"
//#include "TrkTrack/TrackStateOnSurface.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
//#include "VxVertex/VxContainer.h"

#include "StoreGate/ReadHandle.h"

#include "TProfile.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TH1D_LW.h"
#include "LWHists/LWHist1D.h"

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
#define CAN_REBIN(hist)  hist->SetCanExtend(TH1::kAllAxes)
#else
#define CAN_REBIN(hist)  hist->SetBit(TH1::kCanRebin)
#endif


#include <sstream>
#include <iomanip>
#include <memory>

using namespace std;
//Private Static Const data member initialization
const int TRT_Monitoring_Tool::s_numberOfBarrelStacks = 32;
const int TRT_Monitoring_Tool::s_numberOfEndCapStacks = 32;
const int TRT_Monitoring_Tool::s_Straw_max[2] = {1642, 3840};
const int TRT_Monitoring_Tool::s_iStack_max[2] = {32, 64};
const int TRT_Monitoring_Tool::s_iChip_max[2] = {104, 240};
const int TRT_Monitoring_Tool::s_numberOfStacks[2] = {32, 32};
const int TRT_Monitoring_Tool::s_moduleNum[2] = {96, 64};


// TODO: Clean up the rest of the package
// TODO: Fix naming conventions

//------------------------------------------------------------------------------------------------//
TRT_Monitoring_Tool::TRT_Monitoring_Tool(const std::string &type, const std::string &name, const IInterface *parent) :
	ManagedMonitorToolBase(type, name, parent),
	m_lastLumiBlock(-99),
	m_evtLumiBlock(0),
	m_good_bcid(0),
	m_nTotalTracks(0),
	m_nBSErrors(),
	m_nRobErrors(),
	m_passEventBurst(),
	m_idHelper(0),
	p_toolSvc("IToolSvc", name), // NOTE: is this service neccessary?
	m_sumSvc("TRT_StrawStatusSummarySvc", name),
	m_DCSSvc("TRT_DCS_ConditionsSvc", name), // NOTE: not used anywhere?
	m_DAQSvc("TRT_DAQ_ConditionsSvc", name), // NOTE: not used anywhere?
	m_BSSvc("TRT_ByteStream_ConditionsSvc", name),
	m_condSvc_BS("TRT_ByteStream_ConditionsSvc", name), // NOTE: not used anywhere?
	m_TRTStrawNeighbourSvc("TRT_StrawNeighbourSvc", name),
	m_TRTCalDbSvc("TRT_CalDbSvc", name),
	m_pTRTHelper(0),
	m_mgr(0),
	m_TrackSummaryTool("Trk::TrackSummaryTool/InDetTrackSummaryTool"),
	m_drifttool("TRT_DriftFunctionTool"),
	//	m_rdoContainer(0),
	//	m_trkCollection(0),
	//	m_TRT_BCIDColl(0),
	//	m_theComTime(0),
	//	eventInfo(0),
	//	m_comTimeObjectName("ComTime"),
	m_rbins(40),
	m_rmin(0.0),
	m_rmax(2.0),
	m_tbins(68),
	m_tmin(-12.5),
	m_tmax(75.0),
	m_fitmin(-5.0),
	m_fitmax(30.0),
	m_HTfraconTrack_B(),
	m_LonTrack_B(),
	m_nTrack_B(),
	m_nTrackwithHL_B(),
	m_HTfraconTrack_E(),
	m_LonTrack_E(),
	m_nTrack_E(),
	m_nTrackwithHL_E(),
	nTRTHits(),
	m_doDCS(false),
	EventPhaseScale(1.0),
	m_nStrawHits_B(),
	m_nStrawHits_E(),
	DriftTimeonTrkDistScale_B(),
	HLhitOnTrackScale_B(),
	HtoLRatioOnTrackScale_B(),
	HtoLRatioOnTrackScale_B_Ar(),
	HtoLRatioOnTrackScale_B_Xe(),
	NumSwLLWoTScale_B(),
	WireToTrkPositionScale_B(),
	WireToTrkPositionScale_B_Ar(),
	TronTDistScale_B(),
	ResidualScale_B(),
	ResidualScale_B_20GeV(),
	TimeResidualScale_B(),
	DriftTimeonTrkDistScale_B_Ar(),
	TronTDistScale_B_Ar(),
	ResidualScale_B_Ar(),
	ResidualScale_B_Ar_20GeV(),
	TimeResidualScale_B_Ar(),
	nTrkvPhiScale_B(),
	DriftTimeonTrkDistScale_E(),
	HLhitOnTrackScale_E(),
	HtoLRatioOnTrackScale_E(),
	HtoLRatioOnTrackScale_E_Ar(),
	HtoLRatioOnTrackScale_E_Xe(),
	NumSwLLWoTScale_E(),
	WireToTrkPositionScale_E(),
	WireToTrkPositionScale_E_Ar(),
	TronTDistScale_E(),
	ResidualScale_E(),
	ResidualScale_E_20GeV(),
	TimeResidualScale_E(),
	DriftTimeonTrkDistScale_E_Ar(),
	ResidualScale_E_Ar(),
	ResidualScale_E_Ar_20GeV(),
	TimeResidualScale_E_Ar(),
	nTrkvPhiScale_E(),
	//m_propagator(0),
	//m_extrapolator(0),
	DEBUG(false),
	m_longToTCut(9.375),
	m_EventBurstCut(200),
	m_lumiBlocksToResetOcc(20),
	m_isCosmics(false),
	m_minTRThits(10),
	m_minP(0),
	m_trt_hole_finder("TRTTrackHoleSearchTool"),
	m_track_pt(0),
	m_track_eta(0),
	m_track_phi(0),
	m_track_d0(0),
	m_track_z0(0),
	m_min_tracks_straw(10),
	m_lumiTool("LuminosityTool")
//-----------------------------------------------------------------------------------------------//
// NOTE: check up on obsolete properties
{
	declareProperty("ToolSvc",                  p_toolSvc);
	declareProperty("InDetTRTStrawStatusSummarySvc", m_sumSvc);
	declareProperty("InDetTRT_DAQ_ConditionsSvc", m_DAQSvc);
	declareProperty("InDetTRT_DCS_ConditionsSvc", m_DCSSvc);
	declareProperty("TRT_ByteStream_ConditionsSvc", m_BSSvc);
	declareProperty("TRT_StrawNeighbourSvc",   m_TRTStrawNeighbourSvc);
	declareProperty("DriftFunctionTool",        m_drifttool);
	declareProperty("DoTRT_DCS",                m_doDCS);
	declareProperty("DoRDOsMon",                m_doRDOsMon           = true);
	declareProperty("DoGeoMon",                 m_doGeoMon            = false);//obsolete
	//	declareProperty("TRTRawDataObjectName",     m_rawDataObjectName   = "TRT_RDOs");
	declareProperty("NumberOfEvents",           m_usedEvents             = -1);
	declareProperty("DoTracksMon",              m_doTracksMon         = true);
	declareProperty("TRTTracksObjectName",      m_tracksObjectName    = "Tracks");
	declareProperty("doAside",                  m_doASide               = true);//obsolete
	declareProperty("doCside",                  m_doCSide               = true);//obsolete
	declareProperty("doStraws",                 m_doStraws              = true);
	declareProperty("doChips",                  m_doChips               = false);
	declareProperty("doExpert",                 m_doExpert              = false);
	declareProperty("doEfficiency",             m_doEfficiency          = true);
	declareProperty("doMaskStraws",             m_doMaskStraws          = true);
	declareProperty("doShift",                  m_doShift               = true);
	declareProperty("doDiagnostic",             m_doDiagnostic          = false);//obsolete	
	//	declareProperty("ComTimeObjectName",        m_comTimeObjectName   = "TRT_Phase");
	declareProperty("TrkSummaryTool",           m_TrackSummaryTool);
	declareProperty("DistanceToStraw",          m_DistToStraw         = 0.4);
	declareProperty("Geo_Summary_Provider",     m_geo_summary_provider);//probably obsolete
	declareProperty("Map_Path",                 m_mapPath); // obsolete
	declareProperty("maxDistToStraw",           m_maxDistToStraw      = 2.0);//obsolete
	declareProperty("is_TRT_only_tracks",       m_trt_only_trks       = true);//obsolete
	declareProperty("is_zero_mag_field",        m_zero_field          = true);//obsolete
	//
	//   Tunable parameters for TRT monitoring histograms
	//
	declareProperty("LE_TimeWindow_MIN",        m_LE_timeWindow_MIN   = 0);//obsolete
	declareProperty("LE_TimeWindow_MAX",        m_LE_timeWindow_MAX   = 24);//obsolete
	declareProperty("LL_TimeWindow_MIN",        m_LL_timeWindow_MIN   = 0);//obsolete
	declareProperty("LL_TimeWindow_MAX",        m_LL_timeWindow_MAX   = 24);//obsolete
	declareProperty("HL_TimeWindow_MIN",        m_HL_timeWindow_MIN   = 0);//obsolete
	declareProperty("HL_TimeWindow_MAX",        m_HL_timeWindow_MAX   = 3);//obsolete
	declareProperty("MIN_N_LL_Hits",            m_MIN_N_LL_Hits       = 10);//obsolete
	declareProperty("MIN_TOT_Hits",             m_MIN_TOT_Hits        = 2);//obsolete
	declareProperty("NoiseSuppressionLevel_30pc", m_NoiseSuppressionLevel_30pc = false);//obsolete
	declareProperty("NoiseSuppressionMap",      m_NoiseSuppressionMap = false);//obsolete
	declareProperty("Debug",                    DEBUG);//obsolete
	declareProperty("PrintEventInfo",           m_printEventInfo);//obsolete
	declareProperty("ITRT_CalDbSvc",            m_TRTCalDbSvc);
	declareProperty("LongToTCut",               m_longToTCut);
	declareProperty("NPhiBins",                 m_nphi_bins           = 360);
	declareProperty("EventBurstCut",            m_EventBurstCut       = 200);
	declareProperty("LumiBlocksToResetOcc",     m_lumiBlocksToResetOcc = 20);
	declareProperty("IsCosmics",                m_isCosmics           = false);
	declareProperty("MinTRTHitCut",             m_minTRThits          = 10);
	declareProperty("trt_hole_finder",          m_trt_hole_finder);
	declareProperty("useHoleFinder",            m_useHoleFinder = true);
	//	declareProperty("track_collection_hole_finder", m_track_collection_hole_finder = "CombinedInDetTracks");
	declareProperty("max_abs_d0",               m_max_abs_d0          = 10 * CLHEP::mm);
	declareProperty("max_abs_z0",               m_max_abs_z0          = 300 * CLHEP::mm);
	declareProperty("max_abs_eta",              m_max_abs_eta         = 2.5);
	declareProperty("MinTrackP",                m_minP                = 0.0 * CLHEP::GeV);
	declareProperty("min_pT",                   m_min_pT              = 0.5 * CLHEP::GeV);
	declareProperty("min_si_hits",              m_min_si_hits         = 1);
	declareProperty("min_pixel_hits",           m_min_pixel_hits      = 0);
	declareProperty("min_sct_hits",             m_min_sct_hits        = 0);
	declareProperty("min_trt_hits",             m_min_trt_hits        = 10);
	declareProperty("min_tracks_straw",         m_min_tracks_straw    = 10);
	declareProperty("every_xth_track",          m_every_xth_track     = 25);
	declareProperty("whatdatatype",             m_datatype);//obsolete
	declareProperty("doArgonXenonSeparation",   m_ArgonXenonSplitter  = true); // Argon Histograms won't be created if this is set to false.
	declareProperty("LuminosityTool", m_lumiTool, "Luminosity Tool");
	m_flagforscale = 1; //Added for a fix
	m_totalEvents = 0;
	m_hSummary = 0;
	m_IntLum = 0;//
	m_LBvsLum = 0; // coverity 25098
	m_LBvsTime = 0; //
	m_hEvtPhaseDetPhi_B = 0;
	m_hEvtPhase = 0;
	m_hEvtPhaseVsTrig = 0;
	m_hOccAll = 0;
	m_hefficiency_eta = 0;
	m_hefficiency_phi = 0;
	m_hefficiency_pt = 0;
	m_hefficiency_z0 = 0;

	for (int iside = 0; iside < 2; iside++) { // A-side(iside=0), C-side(iside=1)
		m_nTracksB[iside] = 0;
		m_nTracksEC[iside] = 0;
		m_nTracksEC_B[iside] = 0;
	}

	for (int ibe = 0; ibe < 2; ibe++) {
		m_hBCIDvsOcc[ibe] = 0;
		m_hHitsOnTrack_Scatter[ibe] = 0;
		m_hLLOcc_Scatter[ibe] = 0;
		m_hHightoLowRatioOnTrack_Scatter[ibe] = 0;
		m_hefficiencyMap[ibe] = 0;

		for (int iside = 0; iside < 2; iside++) {
			m_hChipBSErrorsVsLB[ibe][iside] = 0;
			m_hRobBSErrorsVsLB[ibe][iside] = 0;
			m_hAvgHLOcc_side[ibe][iside] = 0;
			m_hAvgLLOcc_side[ibe][iside] = 0;
			m_hAvgLLOccMod_side[ibe][iside] = 0;
			m_hAvgHLOccMod_side[ibe][iside] = 0;
			m_hefficiency[ibe][iside] = 0;
			m_hefficiencyIntegral[ibe][iside] = 0;
		}// for (int iside=0; iside<2; iside++)

		for (int i = 0; i < 64; i++) {
			m_hChipOcc[ibe][i] = 0;
			m_hStrawOcc[ibe][i] = 0;
			m_hStrawsEff[ibe][i] = 0;
			m_hChipsEff[ibe][i] = 0;
			m_hHitOnTrackVsAllS[ibe][i] = 0;
			m_hHitOnTrackVsAllC[ibe][i] = 0;
			m_hHitWMapS[ibe][i] = 0;
			m_hHitTrWMapS[ibe][i] = 0;
			m_hHitTrMapS[ibe][i] = 0;
			m_hHitAMapS[ibe][i] = 0;
			m_hHitAWMapS[ibe][i] = 0;
			m_hHtoLMapS[ibe][i] = 0;
			m_hHitToTMapS[ibe][i] = 0;
			m_hHitToTLongMapS[ibe][i] = 0;
			m_hHitToTLongTrMapS[ibe][i] = 0;
			m_hHitHMapS[ibe][i] = 0;
			m_hHitHWMapS[ibe][i] = 0;
			m_hHitWonTMapS[ibe][i] = 0;
			m_hHitTronTMapS[ibe][i] = 0;
			m_hHitAonTMapS[ibe][i] = 0;
			m_hHitAWonTMapS[ibe][i] = 0;
			m_hHitHonTMapS[ibe][i] = 0;
			m_hHitHWonTMapS[ibe][i] = 0;
			m_hHtoLonTMapS[ibe][i] = 0;
			m_hHtoLWonTMapS[ibe][i] = 0;
			m_hHitToTonTMapS[ibe][i] = 0;
			m_hHitTronTwEPCMapS[ibe][i] = 0;
			m_hValidRawDriftTimeonTrk[ibe][i] = 0;
			m_hValidRawDriftTimeonTrkC[ibe][i] = 0;
			m_hHitWMapC[ibe][i] = 0;
			m_hHitTrMapC[ibe][i] = 0;
			m_hHitTrWMapC[ibe][i] = 0;
			m_hHitAMapC[ibe][i] = 0;
			m_hHitAWMapC[ibe][i] = 0;
			m_hHtoLMapC[ibe][i] = 0;
			m_hHtoBCMapC[ibe][i] = 0;
			m_hHtoBCMapB[ibe][i] = 0;
			m_hHitToTMapC[ibe][i] = 0;
			m_hHitHMapC[ibe][i] = 0;
			m_hHitHWMapC[ibe][i] = 0;
			m_hHitWonTMapC[ibe][i] = 0;
			m_hHitTronTMapC[ibe][i] = 0;
			m_hHitAonTMapC[ibe][i] = 0;
			m_hHitAWonTMapC[ibe][i] = 0;
			m_hHitHonTMapC[ibe][i] = 0;
			m_hHitHWonTMapC[ibe][i] = 0;
			m_hHtoLonTMapC[ibe][i] = 0;
			m_hHtoLWonTMapC[ibe][i] = 0;
			m_hHitToTonTMapC[ibe][i] = 0;
			m_hHitTronTwEPCMapC[ibe][i] = 0;
			m_hefficiencyS[ibe][i] = 0;
			m_hefficiencyC[ibe][i] = 0;

			if (ibe == 1) m_hHitToTrkDistanceMapS_E[i] = 0;
		} //for (int i = 0; i < 64; i++)
	} // for (int ibe=0; ibe<2; ibe++)

	nTrksperLB_B = 0;
	nHitsperLB_B = 0;
	nHLHitsperLB_B = 0;
	m_hNumTrksDetPhi_B = 0;
	m_hNumHoTDetPhi_B = 0;
	m_hAvgTroTDetPhi_B = 0;
	m_hAvgTroTDetPhi_B_Ar = 0;
	m_hStrawEffDetPhi_B = 0;
	m_hNumSwLLWoT_B = 0;
	m_hHitWMap_B = 0;
	m_hHitWonTMap_B = 0;
	m_Pull_Biased_Barrel = 0;
	m_hResidual_B = 0;
	m_hResidual_B_20GeV = 0;
	m_hTimeResidual_B = 0;
	m_hDriftTimeonTrkDist_B = 0;
	m_hTronTDist_B = 0;
	m_hrtRelation_B = 0;
	m_hHLhitOnTrack_B = 0;
	m_hHtoLRatioOnTrack_B = 0;
	m_hWireToTrkPosition_B = 0;
	m_hWireToTrkPosition_B_Ar = 0;
	m_hHtoLRatioOnTrack_B_Ar = 0;
	m_hHtoLRatioOnTrack_B_Xe = 0;
	m_hResVsDetPhi_B = 0;
	m_hNHitsperLB_B = 0;
	m_hNTrksperLB_B = 0;
	m_hNHLHitsperLB_B = 0;
	m_hefficiencyBarrel_locR = 0;
	m_hefficiencyBarrel_locR_Ar = 0;
	m_hHitWMap_B_Ar = 0;
	m_hResidual_B_Ar = 0;
	m_hResidual_B_Ar_20GeV = 0;
	m_hTimeResidual_B_Ar = 0;
	m_hrtRelation_B_Ar = 0;
	m_hDriftTimeonTrkDist_B_Ar = 0;
	m_hTronTDist_B_Ar = 0;
	m_Pull_Biased_EndCap = 0;

	for (int iside = 0; iside < 2; iside++) {
		nTrksperLB_E[iside] = 0;
		nHitsperLB_E[iside] = 0;
		nHLHitsperLB_E[iside] = 0;
		m_hNumTrksDetPhi_E[iside] = 0;
		m_hNumHoTDetPhi_E[iside] = 0;
		m_hAvgTroTDetPhi_E[iside] = 0;
		m_hAvgTroTDetPhi_E_Ar[iside] = 0;
		m_hStrawEffDetPhi_E[iside] = 0;
		m_hNumSwLLWoT_E[iside] = 0;
		m_hEvtPhaseDetPhi_E[iside] = 0;
		m_hHitWMap_E[iside] = 0;
		m_hHitWonTMap_E[iside] = 0;
		m_hResidual_E[iside] = 0;
		m_hResidual_E_20GeV[iside] = 0;
		m_hTimeResidual_E[iside] = 0;
		m_hDriftTimeonTrkDist_E[iside] = 0;
		m_hTronTDist_E[iside] = 0;
		m_hrtRelation_E[iside] = 0;
		m_hHLhitOnTrack_E[iside] = 0;
		m_hHtoLRatioOnTrack_E[iside] = 0;
		m_hHtoLRatioOnTrack_E_Ar[iside] = 0;
		m_hHtoLRatioOnTrack_E_Xe[iside] = 0;
		m_hWireToTrkPosition_E[iside] = 0;
		m_hWireToTrkPosition_E_Ar[iside] = 0;
		m_hResVsDetPhi_E[iside] = 0;
		m_hNHitsperLB_E[iside] = 0;
		m_hNTrksperLB_E[iside] = 0;
		m_hNHLHitsperLB_E[iside] = 0;
		m_hefficiencyEndCap_locR[iside] = 0;
		m_hefficiencyEndCap_locR_Ar[iside] = 0;
		m_hHitWMap_E_Ar[iside] = 0;
		m_hResidual_E_Ar[iside] = 0;
		m_hResidual_E_Ar_20GeV[iside] = 0;
		m_hTimeResidual_E_Ar[iside] = 0;
		m_hrtRelation_E_Ar[iside] = 0;
		m_hTronTDist_E_Ar[iside] = 0;
		m_hDriftTimeonTrkDist_E_Ar[iside] = 0;
	}
}

//-------------------------------------------------------------------------//
TRT_Monitoring_Tool::~TRT_Monitoring_Tool() {}
//-------------------------------------------------------------------------//

//------------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::initialize() {
//------------------------------------------------------------------------------------//
	ATH_MSG_VERBOSE("Initializing TRT Monitoring");
	//	StatusCode sc = StatusCode::SUCCESS;

	ATH_CHECK( ManagedMonitorToolBase::initialize() );
	ATH_CHECK( AlgTool::initialize() );

	IToolSvc *p_toolSvc;

	ATH_CHECK( service("ToolSvc", p_toolSvc) );
	// Retrieve detector manager.
	ATH_CHECK( detStore()->retrieve(m_mgr, "TRT") );
	// Get ID helper for TRT to access various detector components like straw, straw_layer, layer_or_wheel, phi_module, etc...
	ATH_CHECK( detStore()->retrieve(m_pTRTHelper, "TRT_ID") );
	ATH_CHECK( detStore()->retrieve(m_idHelper, "AtlasID") );

	if (m_doExpert) {
		// Retrieve the TRT_Straw Status Service.
		if (m_sumSvc.name().empty()) {
			ATH_MSG_WARNING("TRT_StrawStatusSvc not given.");
		} else {
			ATH_CHECK( m_sumSvc.retrieve() );
		}

		// NOTE: is this used anywhere?
		if (m_doDCS) {
			// Retrieve the TRT_DCS Conditions Service.
			if (m_DCSSvc.name().empty()) {
				ATH_MSG_WARNING("TRT_DCSConditionsSvc not given.");
			} else {
				ATH_CHECK( m_DCSSvc.retrieve() );
			}
		}// if do dcs

		// NOTE: is this used anywhere?
		// Retrieve the TRT_Conditions Service.
		if (m_condSvc_BS.name().empty()) {
			ATH_MSG_WARNING("TRT_ConditionsSvc not given.");
		} else {
			ATH_CHECK( m_condSvc_BS.retrieve() );
		}

		// Retrieve the TRT TRTTrackHoleSearchTool.
		ATH_CHECK( m_trt_hole_finder.retrieve() );

		// NOTE: is this used anywhere?
		// Retrieve the TRT_DAQConditions Service.
		if (m_DAQSvc.name().empty()) {
			ATH_MSG_WARNING("TRT_DAQConditionsSvc not given.");
		} else {
			ATH_CHECK( m_DAQSvc.retrieve() );
		}

		// Retrieve the TRT_ByteStreamService.
		if (m_BSSvc.name().empty()) {
			ATH_MSG_WARNING("TRT_ByteStreamSvc not given.");
		} else {
			ATH_CHECK( m_BSSvc.retrieve() );
		}

		// Test out the TRT_ConditionsSummaryTool.
		//Identifier ident = m_trtid->straw_id(1,1,1,1,1);
		Identifier ident;

		if (m_sumSvc.name() != "") {
			ATH_MSG_VERBOSE("Trying " << m_sumSvc << " isGood");
			ATH_MSG_VERBOSE("TRT_StrawStatusSvc reports status = " << m_sumSvc->getStatus(ident));
		}
	}//If do expert

	//Retrieve TRT_StrawNeighbourService.
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
	if (m_TRTCalDbSvc.name().empty()) {
		ATH_MSG_WARNING("TRT_CalDbSvc not given.");
	} else {
		if (m_TRTCalDbSvc.retrieve().isFailure()) {
			ATH_MSG_ERROR("Cannot get TRTCalDBSvc.");
		}
	}

	// retrieve TRTTrackHoleSearchTool
	if (m_doEfficiency) {
		ATH_CHECK( m_trt_hole_finder.retrieve() );
	}

	// Initialize arrays
	// These arrays store information about each entry to the HitMap histograms

	// if (m_environment==AthenaMonManager::online)
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
								mat_chip_B[idPhiModule][tempStrawNumber]    = i_chip;
								mat_chip_B[idPhiModule + 32][tempStrawNumber] = i_chip;
							}
						} else if (ibe == 1) { //endcap
							//              i_chip -= 104;
							++i_chip -= 104;
							int tempStrawNumber = strawNumberEndCap(istraw, idStrawLayer, idLayerWheel, idPhiModule, idSide);

							if (tempStrawNumber < 0 || tempStrawNumber > (s_Straw_max[ibe] - 1)) {
								ATH_MSG_WARNING("Found tempStrawNumber = " << tempStrawNumber << " out of range.");
							} else {
								mat_chip_E[idPhiModule][tempStrawNumber]    = i_chip;
								mat_chip_E[idPhiModule + 32][tempStrawNumber] = i_chip;
							}
						}
					}
				}
			}//for (unsigned int index = 0; index < maxHash; index++)
		} //for (int ibe=0; ibe<2; ibe++)
	}

	// some initializaton
	// TODO: ugly way of doing this, so we probably want to clean it up a bit.
	if (m_doShift) {
		// Barrel
		int ibe = 0;

		for (int iStack = 0; iStack < s_iStack_max[ibe]; iStack++) {
			m_LLOcc[ibe][iStack] = 0;
			m_LLOcc[ibe][iStack + 32] = 0;
			m_HTfraconTrack_B[iStack] = 0;
			m_LonTrack_B[iStack] = 0;
			m_nTrack_B[iStack] = 0;
		}

		for (int i = 0; i < s_Straw_max[ibe]; i++) {
			m_nStrawHits_B[i] = 0;
		}

		// Endcap
		ibe = 1;

		for (int iStack = 0; iStack < s_iStack_max[ibe]; iStack++) {
			m_LLOcc[ibe][iStack] = 0;
			m_HTfraconTrack_E[iStack] = 0;
			m_LonTrack_E[iStack] = 0;
			m_nTrack_E[iStack] = 0;
		}

		for (int i = 0; i < s_Straw_max[ibe]; i++) {
			m_nStrawHits_E[0][i] = 0; // A-side
			m_nStrawHits_E[1][i] = 0; // C-side
		}
	} // doshift for phi bin init

	// Initialization of VarHandleKeys
	ATH_CHECK( m_rdoContainerKey.initialize() );
	ATH_CHECK( m_trackCollectionKey.initialize() );
	ATH_CHECK( m_combTrackCollectionKey.initialize() );
	ATH_CHECK( m_eventInfoKey.initialize() );
	ATH_CHECK( m_xAODEventInfoKey.initialize() );
	ATH_CHECK( m_TRT_BCIDCollectionKey.initialize() );
	ATH_CHECK( m_comTimeObjectKey.initialize() );
	ATH_CHECK( m_trigDecisionKey.initialize() );

	ATH_CHECK( m_lumiTool.retrieve() );
	ATH_MSG_INFO("My TRT_DAQ_ConditionsSvc is " << m_DAQSvc);

	// This exists only for the sake of testing online monitoring
	ATH_MSG_WARNING("Online athena environment was turned on by force!!!");
	m_environment = AthenaMonManager::online;
	
	return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::bookHistogramsRecurrent() {
//------------------------------------------------------------------------------------//
	ATH_MSG_VERBOSE("Booking TRT histograms");

	if (newLumiBlockFlag()) ATH_MSG_VERBOSE("newLumiBlock");

	if (newRunFlag()) ATH_MSG_VERBOSE("newRun");

	// NOTE
	// evtStore()->contains() was used to set up histogram booking
	// ReadHandle isValid() is doing this now, but it is (probably) the best to remove RH from here
	StatusCode sc = StatusCode::SUCCESS;

	//If it is a new run check rdo and track containers.
	if (newRunFlag()) {
		SG::ReadHandle<TRT_RDO_Container> rdoContainer(m_rdoContainerKey);
		SG::ReadHandle<TrackCollection> trackCollection(m_trackCollectionKey);

		if (!rdoContainer.isValid()) {
			ATH_MSG_WARNING("No TRT_RDO_Container by the name of " <<
			                m_rdoContainerKey.key() << " in StoreGate. Skipping TRT RDO Monitoring.");
			m_doRDOsMon = false;
		}

		if (!trackCollection.isValid()) {
			ATH_MSG_WARNING("No TrackCollection by the name of " <<
			                m_trackCollectionKey.key() << " in StoreGate. Skipping TRT Track Monitoring.");
			m_doTracksMon = false;
		}
	}

	// NOTE: This is already retrieved during initialization
	// Is this needed?
	if (m_lumiTool.retrieve().isFailure()) {
		ATH_MSG_ERROR("Unable to retrieve Luminosity Tool");
		return StatusCode::FAILURE;
	} else {
		ATH_MSG_DEBUG("Successfully retrieved Luminosity Tool");
	}

	//Book_TRT_RDOs registers all raw data histograms
	if (m_doRDOsMon) {
		ATH_CHECK( bookTRTRDOs(newLumiBlockFlag(), newRunFlag()) );
	}

	//Book_TRT_Tracks registers all tracking histograms
	if (m_doTracksMon) {
		sc = bookTRTTracks(newLumiBlockFlag(), newRunFlag());

		if (m_doShift) {
			sc = bookTRTShiftTracks(newLumiBlockFlag(), newRunFlag());

			if (sc == StatusCode::FAILURE) {
				ATH_MSG_ERROR("Unable to book trt shift tracks histograms");
			}//if sc== failure
		}//if m_doShift
	}//if do tracks mon

	if (m_doEfficiency) {
		sc = bookTRTEfficiency (newLumiBlockFlag(), newRunFlag());

		if (sc == StatusCode::FAILURE) {
			ATH_MSG_ERROR("Unable to book trt efficiency");
		}//if sc== failure
	}

	if (newRunFlag()) {
		ATH_MSG_DEBUG("Begin of run");
	}

	if (sc == StatusCode::FAILURE) {
		ATH_MSG_ERROR("No histograms booked");
	}

	return StatusCode::SUCCESS;
}

//Book TRT Raw Data Object info (all TRT Hits).
//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::bookTRTRDOs(bool newLumiBlock, bool newRun) {
//----------------------------------------------------------------------------------//
	ATH_MSG_VERBOSE("Booking TRT RDO Histograms");

	if (newLumiBlock) ATH_MSG_VERBOSE("newLumiBlock");

	if (newRun) ATH_MSG_VERBOSE("newRun");

	StatusCode scode = StatusCode::SUCCESS;
	std::string hName, hTitle;
	const std::string barrel_or_endcap[2] = { "Barrel", "EndCap" };
	const std::string be_id[2] = { "B", "E" };
	const std::string side_id[2] = { "A", "C" };

	//Booking of some expert monitoring histograms
	// ibe = 0 (Barrel), ibe = 1 (Endcap)
	for (int ibe = 0; ibe < 2; ibe++) {
		for (int i = 0; i < s_numberOfStacks[ibe] * 2; i++) {
			std::ostringstream oss;

			if (ibe == 0) {
				if (i <  s_numberOfStacks[ibe]) {
					oss << "TRT/Barrel/Stack"   << i + 1 << "A";
				} else if (i >= s_numberOfStacks[ibe]) {
					oss << "TRT/Barrel/Stack"   << i + 1 - 32 << "C";
				}
			} else if (ibe == 1) {
				if (i <  s_numberOfStacks[ibe]) {
					oss << "TRT/EndcapA/Sector" << i + 1;
				} else if (i >= s_numberOfStacks[ibe]) {
					oss << "TRT/EndcapC/Sector" << i + 1 - 32;
				}
			}

			const std::string hPath = oss.str();
			MonGroup rdoStackWeighted(this, hPath, run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED, "", "weightedEff");

			if (newRun && m_doExpert && m_doStraws) {
				m_hHitWMapS[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hHitWMapS", "Leading Edge in Time Window: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability per Event", scode);
				m_hHitTrWMapS[ibe][i] = bookTProfile_LW(rdoStackWeighted, "hHitTrWMapS", "Mean Trailing Edge in Time Window: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], 0, 75, "Straw Number in Stack", "Time (ns)", scode);
				m_hHitTrMapS[ibe][i] = bookTProfile_LW(rdoStackWeighted, "hHitTrMapS", "Mean Trailing Edge: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], 0, 75, "Straw Number in Stack", "Time (ns)", scode);
				m_hHitAWMapS[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hHitAWMapS", "LL in Time Window: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
				m_hHitAMapS[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hHitAMapS", "Any LL Bit: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
				m_hStrawOcc[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hOccupancyS", "Straw Occupancy Distribution: Straws", 201, 0, 1.005, "Occupancy", "Number of Straws", scode);
				m_hHitToTMapS[ibe][i] = bookTProfile_LW(rdoStackWeighted, "hHitToTMapS", "Mean ToT: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], 0, 75, "Straw Number in Stack", "Time (ns)", scode);
				m_hHitToTLongMapS[ibe][i] = bookTProfile_LW(rdoStackWeighted, "hHitToTLongMapS", "Mean ToT for Straws with ToT > LongToTCut: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], 0, 75, "Straw Number in Stack", "Time (ns)", scode);
				m_hHitToTLongTrMapS[ibe][i] = bookTProfile_LW(rdoStackWeighted, "hHitToTLongTrMapS", "Mean Trailing Edge for Straws with ToT > LongToTCut: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], 0, 75, "Straw Number in Stack", "Time (ns)", scode);
				m_hHitHMapS[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hHitHMapS", "Any HL Bit: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
				m_hHitHWMapS[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hHitHWMapS", "HL in Time Window: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
				m_hHtoLMapS[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hHtoLMapS", "HL/LL Ratio: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
			}

			if (newRun && m_doExpert && m_doChips) {
				m_hHitWMapC[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hHitWMapC", "Leading Edge in Time Window: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hHitTrMapC[ibe][i] = bookTProfile_LW(rdoStackWeighted, "hHitTrMapC", "Mean Trailing Edge: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], 0, 75, "Chip Number in Stack", "Time (ns)", scode);
				m_hHitTrWMapC[ibe][i] = bookTProfile_LW(rdoStackWeighted, "hHitTrWMapC", "Mean Trailing Edge in Time Window: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], 0, 75, "Chip Number in Stack", "Time (ns)", scode);
				m_hHitAWMapC[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hHitAWMapC", "LL in Time Window: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hHitAMapC[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hHitAMapC", "Any LL Bit: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hChipOcc[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hOccupancyC", "Chip Occupancy Distribution", 201, 0, 1.005, "Occupancy", "Number of Chips", scode);
				m_hHitToTMapC[ibe][i] = bookTProfile_LW(rdoStackWeighted, "hHitToTMapC", "Mean ToT: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], 0, 75, "Chip Number in Stack", "Time (ns)", scode);
				m_hHitHMapC[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hHitHMapC", "Any HL Bit: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hHitHWMapC[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hHitHWMapC", "HL in Time Window: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hHtoLMapC[ibe][i] = bookTH1F_LW(rdoStackWeighted, "hHtoLMapC", "HL/LL Ratio: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hHtoBCMapC[ibe][i] = bookTH2F_LW(rdoStackWeighted, "hHtoBCMapC", "HL in BC: Chips", 3, 0, 3, s_iChip_max[ibe], 0, s_iChip_max[ibe], "Bunch Crossing ID", "Chip Number in Stack", scode);

				if (ibe == 0) {
					m_hHtoBCMapB[ibe][i] = bookTH2F_LW(rdoStackWeighted, "hHtoBCMapB", "HL in BC: Boards", 3, 0, 3, 9, 0, 9, "Bunch Crossing ID", "Board Number in Stack", scode);
				} else if (ibe == 1) {
					m_hHtoBCMapB[ibe][i] = bookTH2F_LW(rdoStackWeighted, "hHtoBCMapB", "HL in BC: Boards", 3, 0, 3, 20, -0.5, 19.5, "Bunch Crossing ID", "Board Number in Stack", scode);
				}
			}
		}
	}

	//Registering Collisions Histograms
	if (m_doShift) {
		MonGroup rdoShiftSmry (this, "TRT/Shift/Summary", run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED);
		MonGroup rdoShiftSmryRebinned (this, "TRT/Shift/Summary", run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED, "", "mergeRebinned");

		if (newRun) {
			m_hSummary = bookTH1F_LW(rdoShiftSmry, "hSummary", "Run Summary", 8, 0, 8, "", "Entries", scode);
			m_hSummary->GetXaxis()->SetBinLabel(1, "Events");
			m_hSummary->GetXaxis()->SetBinLabel(2, "Tracks Total");
			m_hSummary->GetXaxis()->SetBinLabel(3, "Tracks BA");
			m_hSummary->GetXaxis()->SetBinLabel(4, "Tracks BC");
			m_hSummary->GetXaxis()->SetBinLabel(5, "Tracks EA");
			m_hSummary->GetXaxis()->SetBinLabel(6, "Tracks EC");
			m_hSummary->GetXaxis()->SetBinLabel(7, "Transition Side A");
			m_hSummary->GetXaxis()->SetBinLabel(8, "Transition Side C");
			//lumi summary histograms
			m_IntLum = bookTH1F_LW(rdoShiftSmry, "hIntLum", "Luminosity", 1, 0., 1., " ", "Luminosity [#mub^{1}]", scode);
			m_LBvsLum = bookTH1F_LW(rdoShiftSmry, "hLBvsLum", "Luminosity", 2000, 0., 2000., "Luminosity Bin", "Luminosity [#mub^{1}]", scode);
			m_LBvsTime = bookTH1F_LW(rdoShiftSmry, "hLBvsTime", "Time", 2000, 0., 2000., " Luminosity Bin", "Time [s]", scode);
			const unsigned int maxLumiBlock = 200;

			// ibe = 0 (Barrel), ibe = 1 (Endcap)
			// iside = 0 (A-side), iside = 1 (C-side)
			for (int ibe = 0; ibe < 2; ibe++) {
				for (int iside = 0; iside < 2; iside++) {
					const std::string regionTag = " (" + be_id[ibe] + side_id[iside] + ")";
					m_hChipBSErrorsVsLB[ibe][iside] = bookTProfile(rdoShiftSmryRebinned, "hChipBSErrorsVsLB_" + be_id[ibe] + side_id[iside], "Chip Bytestream Errors vs LB" + regionTag, maxLumiBlock + 1, -0.5, maxLumiBlock + 0.5, 0, 150.0, "Luminosity Block", "Fraction of Chips with Errors", scode);
					CAN_REBIN(m_hChipBSErrorsVsLB[ibe][iside]);
					m_hRobBSErrorsVsLB[ibe][iside] = bookTProfile(rdoShiftSmryRebinned, "hRobBSErrorsVsLB_" + be_id[ibe] + side_id[iside], "Rob Bytestream Errors vs LB" + regionTag, maxLumiBlock + 1, -0.5, maxLumiBlock + 0.5, 0, 150.0, "Luminosity Block", "Fraction of RODs with Errors", scode);
					CAN_REBIN(m_hRobBSErrorsVsLB[ibe][iside]);
				}
			}

			// Barrel/Endcap Histograms
			const std::string module_or_wheel[2] = { "Module", "Wheel" };
			const std::string stack_or_sector[2] = { "Barrel Stack", "Endcap Sector" };
			const std::string modulenum_assign2[2] = { "Modules Type 1 (1-32), Type 2 (33-64), Type 3 (65-96)", "Wheels A (1-32), B (33-64)" };

			for (int ibe = 0; ibe < 2; ibe++) {
				const std::string regionTag = " (" + barrel_or_endcap[ibe] + ")";
				MonGroup rdo(this, "TRT/" + barrel_or_endcap[ibe] + "/Expert",  run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED);
				MonGroup rdoShift (this, "TRT/Shift/" + barrel_or_endcap[ibe], run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED);
				MonGroup rdoShiftTH1(this, "TRT/Shift/" + barrel_or_endcap[ibe], run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED, "", "weightedEff");

				if (ibe == 0) {
					m_hHitWMap_B = bookTH1F_LW(rdoShiftTH1, "hHitWMap", "Leading Edge in Time Window: Xenon Straws" " (Barrel)", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);

					if (m_ArgonXenonSplitter) {
						m_hHitWMap_B_Ar = bookTH1F_LW(rdoShiftTH1, "hHitWMap_Ar", "Leading Edge in Time Window: Argon Straws (Barrel)", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
					}

					m_hOccAll = bookTH1F_LW(rdoShift, "hOccAll", "Occupancy per Event", 400, 0.0, 1.0, "Occupancy", "Events", scode);
				} else if (ibe == 1) {
					m_hHitWMap_E[0] = bookTH1F_LW(rdoShiftTH1, "hHitWMap_A", "Leading Edge in Time Window: Xenon Straws" " (EA)", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
					m_hHitWMap_E[1] = bookTH1F_LW(rdoShiftTH1, "hHitWMap_C", "Leading Edge in Time Window: Xenon Straws" " (EC)", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);

					if (m_ArgonXenonSplitter) {
						m_hHitWMap_E_Ar[0] = bookTH1F_LW(rdoShiftTH1, "hHitWMap_Ar_A", "Leading Edge in Time Window: Argon Straws" " (EA)", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
						m_hHitWMap_E_Ar[1] = bookTH1F_LW(rdoShiftTH1, "hHitWMap_Ar_C", "Leading Edge in Time Window: Argon Straws" " (EC)", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
					}
				}

				m_hBCIDvsOcc[ibe] = bookTProfile_LW(rdo, "hBCIDvsOcc", "Avg. Occupancy vs BCID" + regionTag, 3565, 0, 3564, 0, 1, "Bunch Crossing ID", "Occupancy", scode);

				for (int iside = 0; iside < 2; iside++) {
					const std::string regionTag = " (" + be_id[ibe] + side_id[iside] + ")";
					const std::string regionMarker = (m_environment == AthenaMonManager::online) ? (be_id[ibe] + side_id[iside]) : (side_id[iside]); // for historical reasons ...
					m_hAvgHLOcc_side[ibe][iside] = bookTProfile_LW(rdoShift, "hAvgHLOcc_" + regionMarker, "Avg. HL Occupancy" + regionTag, 32, 1, 33, 0, 1, stack_or_sector[ibe], "Occupancy", scode);
					m_hAvgLLOcc_side[ibe][iside] = bookTProfile_LW(rdoShift, "hAvgLLOcc_" + regionMarker, "Avg. LL Occupancy" + regionTag, 32, 1, 33, 0, 1, stack_or_sector[ibe], "Occupancy", scode);
					m_hAvgLLOccMod_side[ibe][iside] = bookTProfile_LW(rdo, "hAvgLLOccMod_" + regionMarker, "Avg. LL Occupancy: " + module_or_wheel[ibe] + "s" + regionTag, s_moduleNum[ibe], 0, s_moduleNum[ibe], 0, 1, modulenum_assign2[ibe], "Occupancy", scode);
					m_hAvgHLOccMod_side[ibe][iside] = bookTProfile_LW(rdo, "hAvgHLOccMod_" + regionMarker, "Avg. HL Occupancy: " + module_or_wheel[ibe] + "s" + regionTag, s_moduleNum[ibe], 0, s_moduleNum[ibe], 0, 1, modulenum_assign2[ibe], "Occupancy", scode);
				}
			}
		}
	}

	ATH_MSG_VERBOSE("Finished Booking TRT RDO Histograms");
	return scode;
}


//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::bookTRTTracks(bool newLumiBlock, bool newRun) {
//----------------------------------------------------------------------------------//
	ATH_MSG_VERBOSE("Booking TRT Track Histograms");

	if (newLumiBlock) ATH_MSG_VERBOSE("newLumiBlock");

	if (newRun) ATH_MSG_VERBOSE("newRun");

	StatusCode scode = StatusCode::SUCCESS;
	std::string hName, hTitle;
	const std::string barrel_or_endcap[2] = { "Barrel", "EndCap" };

	for (int ibe = 0; ibe < 2; ibe++) {
		std::ostringstream oss_distance;
		oss_distance << std::setprecision(3) << std::fixed << m_DistToStraw;
		const std::string distance = oss_distance.str();
		const std::string hPathGen = "TRT/" + barrel_or_endcap[ibe] + "/Expert";
		const std::string regionTag = " (" + barrel_or_endcap[ibe] + ")";

		for (int i = 0; i < s_numberOfStacks[ibe] * 2; i++) {
			std::ostringstream oss;

			if (ibe == 0) {
				if (i <  s_numberOfStacks[ibe]) {
					oss << "TRT/Barrel/Stack"   << i + 1 << "A";
				} else if (i >= s_numberOfStacks[ibe]) {
					oss << "TRT/Barrel/Stack"   << i + 1 - 32 << "C";
				}
			} else if (ibe == 1) {
				if (i <  s_numberOfStacks[ibe]) {
					oss << "TRT/EndcapA/Sector" << i + 1;
				} else if (i >= s_numberOfStacks[ibe]) {
					oss << "TRT/EndcapC/Sector" << i + 1 - 32;
				}
			}

			const std::string hPath = oss.str();
			MonGroup trackStackWeighted(this, hPath, run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED, "", "weightedEff");

			if (newRun && m_doExpert && m_doStraws) {
				m_hHitWonTMapS[ibe][i]  = bookTH1F_LW(trackStackWeighted, "hHitWonTMapS", "Leading Edge on Track in Time Window: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
				m_hHitTronTMapS[ibe][i] = bookTProfile_LW(trackStackWeighted, "hHitTronTMapS", "Mean Trailing Edge on Track: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], 0, 75.0, "Straw Number in Stack", "Time (ns)", scode);
				m_hHitAonTMapS[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHitAonTMapS", "Any LL Bit on Track: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
				m_hStrawsEff[ibe][i] = bookTProfile_LW(trackStackWeighted, "hEfficiencyS", "Straw Efficiency with " + distance + " mm Cut", s_Straw_max[ibe], 0, s_Straw_max[ibe], 0.0, 1.0, "Straw Number in Stack", "Efficiency", scode);
				m_hHitAWonTMapS[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHitAWonTMapS", "Any LL Bit on Track in Time Window: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
				m_hHitHonTMapS[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHitHonTMapS", "HL Hit on Track: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
				m_hHitHWonTMapS[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHitHWonTMapS", "HL Hit(In Time Window) on Track: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
				m_hHitAWonTMapS[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHitAWonTMapS", "Any LL Bit on Track in Time Window: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
				m_hHitToTonTMapS[ibe][i] = bookTProfile_LW(trackStackWeighted, "hHitToTonTMapS", "Mean ToT on Track: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], 0, 75, "Straw Number in Stack", "Time (ns)", scode);
				m_hValidRawDriftTimeonTrk[ibe][i] = bookTProfile_LW(trackStackWeighted, "hValidRawDriftTimeonTrkS", "Valid Raw Drift Time on Track: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], 0, 75, "Straw Number in Stack", "Time (ns)", scode);
				m_hHtoLonTMapS[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHtoLonTMapS", "HL/LL Ratio on Track: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
				m_hHtoLWonTMapS[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHtoLWonTMapS", "HL/LL (In Time Window) Ratio on Track: Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Probability", scode);
				m_hHitTronTwEPCMapS[ibe][i] = bookTProfile_LW(trackStackWeighted, "hHitTronTwEPCMapS", "Mean Trailing Edge on Track (with Event Phase Correction): Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], -50, 75, "Straw Number in Stack", "Time (ns)", scode);
				m_hHitOnTrackVsAllS[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHitonTrackVAllS", "(Hit on Track) / (Any LL Bit): Straws", s_Straw_max[ibe], 0, s_Straw_max[ibe], "Straw Number in Stack", "Ratio", scode);
			}

			if (newRun && m_doExpert && m_doChips) {
				m_hHitWonTMapC[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHitWonTMapC", "Leading Edge on Track in Time Window: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hHitTronTMapC[ibe][i] = bookTProfile_LW(trackStackWeighted, "hHitTronTMapC", "Mean Trailing Edge on Track: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], 0, 75, "Chip Number in Stack", "Time (ns)", scode);
				m_hHitAonTMapC[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHitAonTMapC", "Any LL Bit on Track: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hChipsEff[ibe][i] = bookTProfile_LW(trackStackWeighted, "hEfficiencyC", "Chip Efficiency with " + distance + " mm Cut", s_iChip_max[ibe], 0, s_iChip_max[ibe], 0.0, 1.0, "Chip Number in Stack", "Efficiency", scode);
				m_hHitAWonTMapC[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHitAWonTMapC", "Any LL Bit on Track in Time Window: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hHitHonTMapC[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHitHonTMapC", "HL Hit on Track: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hHitHWonTMapC[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHitHWonTMapC", "HL Hit(In time Window) on Track: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hHitToTonTMapC[ibe][i] = bookTProfile_LW(trackStackWeighted, "hHitToTonTMapC", "Mean ToT on Track: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], 0, 75, "Chip Number in Stack", "Time (ns)", scode);
				m_hValidRawDriftTimeonTrkC[ibe][i] = bookTProfile_LW(trackStackWeighted, "hValidRawDriftTimeonTrkC", "Valid Raw Drift Time on Track: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], 0, 75, "Chip Number in Stack", "Time (ns)", scode);
				m_hHtoLonTMapC[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHtoLonTMapC", "HL/LL Ratio on Track: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hHtoLWonTMapC[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHtoLWonTMapC", "HL/LL(In Time Window) Ratio on Track: Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Probability", scode);
				m_hHitTronTwEPCMapC[ibe][i] = bookTProfile_LW(trackStackWeighted, "hHitTronTwEPCMapC", "Mean Trailing Edge on Track (with Event Phase Correction): Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], -50, 75, "Chip Number in Stack", "Time (ns)", scode);
				m_hHitOnTrackVsAllC[ibe][i] = bookTH1F_LW(trackStackWeighted, "hHitonTrackVAllC", "(Hit on Track) / (Any LL Bit): Chips", s_iChip_max[ibe], 0, s_iChip_max[ibe], "Chip Number in Stack", "Ratio", scode);
			}
		}
	}

	ATH_MSG_VERBOSE("Booked TRT Track Histograms successfully");
	return scode;
}


//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::bookTRTEfficiency(bool newLumiBlock, bool newRun) {
//----------------------------------------------------------------------------------//
	StatusCode scode = StatusCode::SUCCESS;

	if (newLumiBlock) {}

	if (newRun) {
		std::string hName, hTitle;
		const std::string barrel_or_endcap[2] = { "Barrel", "EndCap" };
		const std::string be_id[2] = { "B", "E" };
		const std::string side_id[2] = { "A", "C" };
		MonGroup trackShiftEff(this, "TRT/Efficiency", run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED);
		m_hefficiency_eta = bookTProfile_LW(trackShiftEff, "hEfficiency_eta", "Efficiency vs #eta", 50, -2.8, 2.8, 0, 1, "#eta", "Efficiency", scode);
		m_hefficiency_phi = bookTProfile_LW(trackShiftEff, "hEfficiency_phi", "Efficiency vs #phi", 50, -3.2, 3.2, 0, 1, "#phi (deg)", "Efficiency", scode);
		m_hefficiency_pt = bookTProfile_LW(trackShiftEff, "hEfficiency_pt", "Efficiency vs pT", 50, 0, 10, 0, 1, "pT (GeV)", "Efficiency", scode);
		m_hefficiency_z0 = bookTProfile_LW(trackShiftEff, "hEfficiency_z0", "Efficiency vs z0", 50, -200, 200, 0, 1, "z0", "Efficiency", scode);
		m_hefficiencyBarrel_locR = bookTProfile_LW(trackShiftEff, "hEfficiencyBarrel_locR", "Efficiency vs Track-to-Wire Distance for Xenon Straws" " (Barrel)", 50, -2.5, 2.5, 0, 1, "Track-to-Wire Distance (mm)", "Efficiency", scode);
		m_hefficiencyBarrel_locR_Ar = bookTProfile_LW(trackShiftEff, "hEfficiencyBarrel_locR_Ar", "Efficiency vs Track-to-Wire Distance for Argon Straws" " (Barrel)", 50, -2.5, 2.5, 0, 1, "Track-to-Wire Distance (mm)", "Efficiency", scode);
		m_hefficiencyMap[0] = bookTProfile_LW(trackShiftEff, "hEfficiencyBarrelMap", "Straw Efficiency Map" " (Barrel)", s_Straw_max[0], 0, s_Straw_max[0], 0, 1, "Straw Number", "Efficiency", scode);
		m_hefficiencyMap[1] = bookTProfile_LW(trackShiftEff, "hEfficiencyEndCapMap", "Straw Efficiency Map" " (Endcap)", s_Straw_max[1], 0, s_Straw_max[1], 0, 1, "Straw Number", "Efficiency", scode);

		for (int iside = 0; iside < 2; iside++) {
			const std::string regionTag = " (" + be_id[1]   + side_id[iside] + ")";
			m_hefficiencyEndCap_locR[iside] = bookTProfile_LW(trackShiftEff, "hEfficiencyEndCap" + side_id[iside] + "_locR", "Efficiency vs Track-to-Wire Distance for Xenon Straws" + regionTag, 50, -2.5, 2.5, 0, 1, "Track-to-Wire Distance (mm)", "Efficiency", scode);
			m_hefficiencyEndCap_locR_Ar[iside] = bookTProfile_LW(trackShiftEff, "hEfficiencyEndCap" + side_id[iside] + "_locR_Ar", "Efficiency vs Track-to-Wire Distance for Argon Straws" + regionTag, 50, -2.5, 2.5, 0, 1, "Track-to-Wire Distance (mm)", "Efficiency", scode);
		}

		for (int ibe = 0; ibe < 2; ibe++) {
			for (int iside = 0; iside < 2; iside++) {
				const std::string regionTag = " (" + be_id[ibe] + side_id[iside] + ")";
				MonGroup trackShiftEffWeighted(this, "TRT/Efficiency", run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED, "", "weightedEff");
				m_hefficiency[ibe][iside] = bookTH1F_LW(trackShiftEffWeighted, "hEfficiency" + barrel_or_endcap[ibe] + side_id[iside], "Straw Efficiency" + regionTag, 500, -0.01, 1.01, "Efficiency", "Number of Straws", scode);
				m_hefficiencyIntegral[ibe][iside] = bookTH1F_LW(trackShiftEffWeighted, "hEfficiencyIntegral" + barrel_or_endcap[ibe] + side_id[iside], "Straw Efficiency Integral" + regionTag, 500, -0.01, 1.01, "Efficiency", "Fraction of Straws", scode);

				if (m_doExpert) {
					int imintmp = s_numberOfBarrelStacks * iside;
					int imaxtmp = s_numberOfBarrelStacks * (iside + 1);

					for (int i = imintmp; i < imaxtmp; i++) {
						std::ostringstream oss;

						if (ibe == 0) {
							oss << "TRT/Barrel/Stack" << (i + 1 - 32 * iside) << side_id[iside];
						} else if (ibe == 1) {
							oss << "TRT/Endcap" << side_id[iside] << "/Sector" << (i + 1 - 32 * iside);
						}

						const std::string hPath = oss.str();
						MonGroup trackStack(this, hPath, run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED);
						m_hefficiencyS[ibe][i] = bookTProfile_LW(trackStack, "hHitEfficiencyS", "Straw Efficiency Map", s_Straw_max[ibe], 0.5, s_Straw_max[ibe] + 0.5, 0, 1, "Straw Number", "Efficiency", scode);
						m_hefficiencyC[ibe][i] = bookTProfile_LW(trackStack, "hHitEfficiencyC", "Chip Efficiency Map",  s_iChip_max[ibe], 0.5, s_iChip_max[ibe] + 0.5, 0, 1, "Chip Number", "Efficiency", scode);
					}
				}
			}
		}
	}

	return scode;
}

//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::bookTRTShiftTracks(bool newLumiBlock, bool newRun) {
//----------------------------------------------------------------------------------//
	ATH_MSG_VERBOSE("Booking TRT Track Histograms");

	if (newLumiBlock)   ATH_MSG_VERBOSE("newLumiBlock");

	if (newRun)         ATH_MSG_VERBOSE("newRun");

	std::string hName, hTitle;
	std::ostringstream oss_distance;
	oss_distance << std::setprecision(3) << std::fixed << m_DistToStraw;
	const std::string distance = oss_distance.str();
	StatusCode scode = StatusCode::SUCCESS;
	//create several histogram directories.
	MonGroup trackBarrelShiftTProf(this, "TRT/Shift/Barrel", run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED);
	MonGroup trackBarrelDiag(this, "TRT/Barrel/Diagnostics", run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED);
	const std::string barrel_or_endcap[2] = { "Barrel", "EndCap" };
	const std::string be_id[2] = { "B", "E" };
	const std::string side_id[2] = { "A", "C" };
	const int maxLumiblock = 720;
	//Arrays for Aging
	const std::string gas[4] = { "in_A", "in_B", "out_A", "out_B"};
	const std::string Mod[5] = {"1", "2", "3", "shortP", "shortN"};

	for (int ibe = 0; ibe < 2; ibe++) {
		const std::string regionTag = " (" + barrel_or_endcap[ibe] + ")";
		MonGroup trackShift(this, "TRT/Shift/" + barrel_or_endcap[ibe],  run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED);
		MonGroup trackShiftRebinned(this, "TRT/Shift/" + barrel_or_endcap[ibe],  run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED, "", "mergeRebinned");
		MonGroup trackShiftTH1(this, "TRT/Shift/" + barrel_or_endcap[ibe],  run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED, "", "weightedEff");
		MonGroup trackShiftTH1_lowStat(this, "TRT/Shift/" + barrel_or_endcap[ibe],  lowStat, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED, "", "weightedEff");
		MonGroup trackAging(this, "TRT/Aging/" + barrel_or_endcap[ibe], lowStat, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED);

		if (newRun && m_doShift) {
			if (ibe == 0) {
				m_hEvtPhase = bookTH1F_LW(trackShift, "hEvtPhase", "Event Phase Correction Factor", 200, -50, 50, "Event Phase (ns)", "Entries", scode);
				m_hEvtPhaseVsTrig = bookTH2F_LW(trackShift, "hEvtPhaseVsTrig", "Event Phase vs L1 Trigger Item", 300, -200, 100, 256, -0.5, 255.5, "Event Phase (ns)", "L1 Trigger Item", scode);
				m_hEvtPhaseDetPhi_B = bookTProfile_LW(trackShift, "hEvtPhaseDetPhi", "Event Phase vs #phi (2D)" + regionTag, m_nphi_bins, 0, 360, -50, 100., "#phi (deg)", "Event Phase from Tracks per Event", scode);
				m_hrtRelation_B = bookTH2F_LW(trackShift, "hrtRelation", "R(t) Relation for Xenon Straws" + regionTag, 30, -12.5, 81.25, 50, 0, 2.5, "Measured Leading Edge (ns)", "Track-to-Wire Distance (mm)", scode);
				m_hNumHoTDetPhi_B = bookTProfile_LW(trackShift, "hNumHoTDetPhi", "Number of Hits per Track with " + distance + " mm Cut vs #phi" + regionTag, m_nphi_bins, 0., 360, 0, 150, "#phi (deg)", Form("Hits per Track, TRT Hits >= %d", m_minTRThits), scode);
				m_hTronTDist_B = bookTH1F_LW(trackShiftTH1, "hTronTDist", "Trailing Edge Distribution on Track for Xenon Straws" + regionTag, 26, -0.5, 80.75, "Trailing Edge (ns)", "Norm. Entries", scode);
				m_hDriftTimeonTrkDist_B = bookTH1F_LW(trackShiftTH1, "hDriftTimeonTrkDist", "Drift Time Distribution on Track for Xenon Straws" + regionTag, 32, 0, 100., "Drift Time (ns)", "Norm. Entries", scode);
				m_hNumTrksDetPhi_B = bookTH1F_LW(trackShift, "hNumTrksDetPhi", "Number of Reconstructed Tracks vs #phi (2D)" + regionTag, 60, 0, 360, "#phi (deg)", "Number of Tracks", scode);

				if (m_ArgonXenonSplitter) {
					m_hDriftTimeonTrkDist_B_Ar = bookTH1F_LW(trackShiftTH1, "hDriftTimeonTrkDist_Ar", "Drift Time Distribution on Track for Argon Straws" + regionTag, 32, 0, 100., "Drift Time (ns)", "Norm. Entries", scode);
					m_hTronTDist_B_Ar = bookTH1F_LW(trackShiftTH1, "hTronTDist_Ar", "Trailing Edge Distribution on Track for Argon Straws" + regionTag, 26, -0.5, 80.75, "Trailing Edge (ns)", "Norm. Entries", scode);
					m_hrtRelation_B_Ar = bookTH2F_LW(trackShift, "hrtRelation_Ar", "R(t) Relation for Argon Straws" + regionTag, 30, -12.5, 81.25, 50, 0, 2.5, "Measured Leading Edge (ns)", "Track-to-Wire Distance (mm)", scode);
					m_Pull_Biased_Barrel  = bookTH1F_LW(trackShift, "hPull_Biased_Barrel", "Biased Track Pulls for Barrel Hits", 200, -2.5, 2.5, "Pulls", "Entries", scode);
					m_hResidual_B_Ar = bookTH1F_LW(trackShiftTH1_lowStat, "hResidual_Ar", "Residuals for Argon Straws" + regionTag, 200, -2.5, 2.5, "Hit-to-Track Distance (mm)", "Norm. Entries", scode);
					m_hResidual_B_Ar_20GeV = bookTH1F_LW(trackShiftTH1, "hResidual_Ar_20GeV", "Residuals for Argon Straws" + regionTag + "(After 20GeV pT cut)", 200, -2.5, 2.5, "Hit-to-Track Distance (mm)", "Norm. Entries", scode);
					m_hAvgTroTDetPhi_B_Ar = bookTProfile_LW(trackShift, "hAvgTroTDetPhi_Ar", "Avg. Trailing Edge on Track vs #phi (2D) for Argon" + regionTag, m_nphi_bins, 0, 360, 0, 75., "#phi (deg)", "Trailing Edge (ns)", scode);
					m_hTimeResidual_B_Ar  = bookTH1F_LW(trackShiftTH1, "hTimeResidual_Ar", "Time Residuals for Argon Straws" + regionTag, 200, -20, 20, "Time Residual (ns)", "Norm. Entries", scode);
					m_hWireToTrkPosition_B_Ar = bookTH1F_LW(trackShiftTH1, "hWireToTrkPosition_Ar", "Track-to-Wire Distance for Argon" + regionTag, 100, -5., 5, "Track-to-Wire Distance (mm)", "Norm. Entries", scode);
					m_hHtoLRatioOnTrack_B_Ar = bookTH1F_LW(trackShiftTH1, "hHtoLRatioOnTrack_Ar", "HL/LL Ratio per Reconstructed Track for Argon" + regionTag, 50, 0, 1, "HL/LL Ratio", "Norm. Entries", scode); //for argon
				}

				m_hHtoLRatioOnTrack_B_Xe = bookTH1F_LW(trackShiftTH1, "hHtoLRatioOnTrack_Xe", "HL/LL Ratio per Reconstructed Track for Xenon" + regionTag, 50, 0, 1, "HL/LL Ratio", "Norm. Entries", scode); //for xenon
				m_hResidual_B = bookTH1F_LW(trackShiftTH1_lowStat, "hResidual", "Residuals for Xenon Straws" + regionTag, 200, -2.5, 2.5, "Hit-to-Track Distance (mm)", "Norm. Entries", scode);
				m_hResidual_B_20GeV   = bookTH1F_LW(trackShiftTH1, "hResidual_20GeV", "Residuals for Xenon Straws" + regionTag + "(After 20GeV pT cut)", 200, -2.5, 2.5, "Hit-to-Track Distance (mm)", "Norm. Entries", scode);
				m_hTimeResidual_B = bookTH1F_LW(trackShiftTH1, "hTimeResidual", "Time Residuals for Xenon Straws" + regionTag, 200, -20, 20, "Time Residual (ns)", "Norm. Entries", scode);
				m_hWireToTrkPosition_B  = bookTH1F_LW(trackShiftTH1, "hWireToTrkPosition", "Track-to-Wire Distance for Xenon" + regionTag, 100, -5., 5, "Track-to-Wire Distance (mm)", "Norm. Entries", scode);
				m_hNumSwLLWoT_B = bookTH1F_LW(trackShiftTH1, "hNumSwLLWoT", "Number of Straws with Hits on Track in Time Window" + regionTag, 150, 0, 150, "Number of LL Hits per Track", "Norm. Entries", scode);
				m_hAvgTroTDetPhi_B = bookTProfile_LW(trackShift, "hAvgTroTDetPhi", "Avg. Trailing Edge on Track vs #phi (2D) for Xenon" + regionTag, m_nphi_bins, 0, 360, 0, 75., "#phi (deg)", "Trailing Edge (ns)", scode);
				m_hNTrksperLB_B = bookTProfile(trackShiftRebinned, "hNTrksperLB", "Avg. Number of Reconstructed Tracks per Event" + regionTag, maxLumiblock, -0.5, maxLumiblock - 0.5, 0, 150.0, "Luminosity Block", "Number of Tracks", scode);
				CAN_REBIN(m_hNTrksperLB_B);
				m_hNHitsperLB_B = bookTProfile(trackShiftRebinned, "hNHitsperLB", "Avg. Occupancy" + regionTag, maxLumiblock, -0.5, maxLumiblock - 0.5, 0, 150.0, "Luminosity Block", "Occupancy", scode);
				CAN_REBIN(m_hNHitsperLB_B);
				m_hNHLHitsperLB_B = bookTProfile(trackShiftRebinned, "hNHLHitsperLB", "Avg. HL Occupancy" + regionTag, maxLumiblock, -0.5, maxLumiblock - 0.5, 0, 150.0, "Luminosity Block", "Occupancy", scode);
				CAN_REBIN(m_hNHLHitsperLB_B);
				m_hHLhitOnTrack_B = bookTH1F_LW(trackShiftTH1, "hHLhitOnTrack", "Number of HL Hits per Reconstructed Track" + regionTag, 50, 0, 50, "Number of HL Hits per Track", "Norm. Entries", scode);
				m_hHtoLRatioOnTrack_B = bookTH1F_LW(trackShiftTH1, "hHtoLRatioOnTrack", "HL/LL Ratio per Reconstructed Track for All" + regionTag, 50, 0, 1, "HL/LL Ratio", "Norm. Entries", scode);
				m_hHitWonTMap_B = bookTH1F_LW(trackShiftTH1, "hHitWonTMap", "Leading Edge in Time Window per Reconstructed Track" + regionTag, s_Straw_max[0], 0, s_Straw_max[0], "Straw Number", "Norm. Entries", scode);
				m_hStrawEffDetPhi_B = bookTProfile_LW(trackShift, "hStrawEffDetPhi", "Straw Efficiency on Track with " + distance + " mm Cut vs #phi(2D)" + regionTag, 32, 0, 32, 0, 1.2, "Stack", "Avg. Straw Efficiency", scode);
			} else if (ibe == 1) {
				m_Pull_Biased_EndCap =  bookTH1F_LW(trackShift, "hPull_Biased_EndCap", "Biased Track Pulls for EndCap Hits", 200, -2.5, 2.5, "Pulls", "Entries", scode);

				for (int iside = 0; iside < 2; iside++) {
					const std::string regionTag = " (" + be_id[ibe] + side_id[iside] + ")";
					m_hEvtPhaseDetPhi_E[iside] = bookTProfile_LW(trackShift, "hEvtPhaseDetPhi_" + side_id[iside], "Event Phase vs #phi (2D)" + regionTag, m_nphi_bins, 0, 360, -50, 100, "#phi (deg)", "Event Phase from Tracks per Event", scode);
					m_hrtRelation_E[iside] = bookTH2F_LW(trackShift, "hrtRelation_" + side_id[iside], "R(t) Relation for Xenon Straws" + regionTag, 30, -12.5, 81.25, 50, 0, 2.5, "Measured Leading Edge (ns)", "Track-to-Wire Distance (mm)", scode);
					m_hNumHoTDetPhi_E[iside] = bookTProfile_LW(trackShift, "hNumHoTDetPhi_" + side_id[iside], "Number of Hits per Track with " + distance + " mm Cut vs #phi" + regionTag, m_nphi_bins, 0., 360, 0, 150, "#phi (deg)", Form("Hits per Track, TRT Hits> = %d", m_minTRThits), scode);
					m_hTronTDist_E[iside] = bookTH1F_LW(trackShiftTH1, "hTronTDist_" + side_id[iside], "Trailing Edge Distribution on Track for Xenon Straws" + regionTag, 26, -0.5, 80.75,  "Trailing Edge (ns)", "Norm. Entries", scode);
					m_hDriftTimeonTrkDist_E[iside] = bookTH1F_LW(trackShiftTH1, "hDriftTimeonTrkDist_" + side_id[iside], "Drift Time Distribution on Track for Xenon Straws" + regionTag, 32, 0, 100, "Drift Time (ns)", "Norm. Entries", scode);
					m_hNumTrksDetPhi_E[iside] = bookTH1F_LW(trackShift, "hNumTrksDetPhi_" + side_id[iside], "Number of Reconstructed Tracks vs #phi (2D)" + regionTag, 60, 0, 360, "#phi (deg)", "Number of Tracks", scode);
					m_hResidual_E[iside] = bookTH1F_LW(trackShiftTH1_lowStat, "hResidual_" + side_id[iside], "Residuals for Xenon Straws" + regionTag, 200, -2.5, 2.5, "Hit-to-Track Distance (mm)", "Norm. Entries", scode);
					m_hResidual_E_20GeV[iside] = bookTH1F_LW(trackShiftTH1, "hResidual_" + side_id[iside] + "_20GeV", "Residuals for Xenon Straws" + regionTag + "(After 20GeV pT cut)", 200, -2.5, 2.5, "Hit-to-Track Distance (mm)", "Norm. Entries", scode);
					m_hTimeResidual_E[iside] = bookTH1F_LW(trackShiftTH1, "hTimeResidual_" + side_id[iside], "Time Residuals for Xenon Straws" + regionTag, 200, -20, 20, "Time Residual (ns)", "Norm. Entries", scode);

					if (m_ArgonXenonSplitter) {
						m_hTronTDist_E_Ar[iside] = bookTH1F_LW(trackShiftTH1, "hTronTDist_Ar_" + side_id[iside], "Trailing Edge Distribution on Track for Argon Straws" + regionTag, 26, -0.5, 80.75,  "Trailing Edge (ns)", "Norm. Entries", scode);
						m_hAvgTroTDetPhi_E_Ar[iside] = bookTProfile_LW(trackShift, "hAvgTroTDetPhi_Ar_" + side_id[iside], "Avg. Trailing Edge on Track vs #phi (2D) for Argon" + regionTag, m_nphi_bins, 0, 360, 0, 75., "#phi (deg)", "Trailing Edge (ns)", scode);
						m_hrtRelation_E_Ar[iside] = bookTH2F_LW(trackShift, "hrtRelation_Ar_" + side_id[iside], "R(t) Relation for Argon Straws" + regionTag, 30, -12.5, 81.25, 50, 0, 2.5, "Measured Leading Edge (ns)", "Track-to-Wire Distance (mm)", scode);
						m_hDriftTimeonTrkDist_E_Ar[iside] = bookTH1F_LW(trackShiftTH1, "hDriftTimeonTrkDist_Ar_" + side_id[iside], "Drift Time Distribution on Track for Argon Straws" + regionTag, 32, 0, 100, "Drift Time (ns)", "Norm. Entries", scode);
						m_hResidual_E_Ar[iside] = bookTH1F_LW(trackShiftTH1_lowStat, "hResidual_Ar_" + side_id[iside], "Residuals for Argon Straws" + regionTag, 200, -2.5, 2.5, "Hit-to-Track Distance (mm)", "Norm. Entries", scode);
						m_hResidual_E_Ar_20GeV[iside] = bookTH1F_LW(trackShiftTH1, "hResidual_Ar_" + side_id[iside] + "_20GeV", "Residuals for Argon Straws" + regionTag + "(After 20GeV pT cut)", 200, -2.5, 2.5, "Hit-to-Track Distance (mm)", "Norm. Entries", scode);
						m_hTimeResidual_E_Ar[iside] = bookTH1F_LW(trackShiftTH1, "hTimeResidual_Ar_" + side_id[iside], "Time Residuals for Argon Straws" + regionTag, 200, -20, 20, "Time Residual (ns)", "Norm. Entries", scode);
						m_hWireToTrkPosition_E_Ar[iside] = bookTH1F_LW(trackShiftTH1, "hWireToTrkPosition_Ar_" + side_id[iside], "Track-to-Wire Distance for Argon" + regionTag, 100, -5., 5, "Track-to-Wire Distance (mm)", "Norm. Entries", scode);
						m_hHtoLRatioOnTrack_E_Ar[iside] = bookTH1F_LW(trackShiftTH1, "hHtoLRatioOnTrack_Ar_" + side_id[iside], "HL/LL Ratio per Reconstructed Track for Argon" + regionTag, 50, 0, 1.0, "HL/LL Ratio", "Norm. Entries", scode);
					}

					m_hHtoLRatioOnTrack_E_Xe[iside] = bookTH1F_LW(trackShiftTH1, "hHtoLRatioOnTrack_Xe_" + side_id[iside], "HL/LL Ratio per Reconstructed Track for Xenon" + regionTag, 50, 0, 1.0, "HL/LL Ratio", "Norm. Entries", scode);
					m_hWireToTrkPosition_E[iside] = bookTH1F_LW(trackShiftTH1, "hWireToTrkPosition_" + side_id[iside], "Track-to-Wire Distance for Xenon" + regionTag, 100, -5., 5, "Track-to-Wire Distance (mm)", "Norm. Entries", scode);
					m_hNumSwLLWoT_E[iside] = bookTH1F_LW(trackShiftTH1, "hNumSwLLWoT_" + side_id[iside], "Number of Straws with Hits on Track in Time Window" + regionTag, 150, 0, 150, "Number of LL Hits per Track", "Norm. Entries", scode);
					m_hAvgTroTDetPhi_E[iside] = bookTProfile_LW(trackShift, "hAvgTroTDetPhi_" + side_id[iside], "Avg. Trailing Edge on Track vs #phi (2D) for Xenon" + regionTag, m_nphi_bins, 0, 360, 0, 75., "#phi (deg)", "Trailing Edge (ns)", scode);
					m_hNTrksperLB_E[iside] = bookTProfile(trackShiftRebinned, "hNTrksperLB_" + side_id[iside], "Avg. Number of Reconstructed Tracks per Event" + regionTag, maxLumiblock, -0.5, maxLumiblock - 0.5, 0, 150.0, "Luminosity Block", "Number of Tracks", scode);
					CAN_REBIN(m_hNTrksperLB_E[iside]);
					m_hNHitsperLB_E[iside] = bookTProfile(trackShiftRebinned, "hNHitsperLB_" + side_id[iside], "Avg. Occupancy" + regionTag, maxLumiblock, -0.5, maxLumiblock - 0.5, 0, 150.0, "Luminosity Block", "Occupancy", scode);
					CAN_REBIN(m_hNHitsperLB_E[iside]);
					m_hNHLHitsperLB_E[iside] = bookTProfile(trackShiftRebinned, "hNHLHitsperLB_" + side_id[iside], "Avg. HL Occupancy" + regionTag, maxLumiblock, -0.5, maxLumiblock - 0.5, 0, 150.0, "Luminosity Block", "Occupancy", scode);
					CAN_REBIN(m_hNHLHitsperLB_E[iside]);
					m_hHLhitOnTrack_E[iside] = bookTH1F_LW(trackShiftTH1, "hHLhitOnTrack_" + side_id[iside], "Number of HL Hits per Reconstructed Track" + regionTag, 50, 0, 50, "Number of HL Hits per Track", "Norm. Entries", scode);
					m_hHtoLRatioOnTrack_E[iside] = bookTH1F_LW(trackShiftTH1, "hHtoLRatioOnTrack_" + side_id[iside], "HL/LL Ratio per Reconstructed Track for All" + regionTag, 50, 0, 1.0, "HL/LL Ratio", "Norm. Entries", scode);
					m_hHitWonTMap_E[iside] = bookTH1F_LW(trackShiftTH1, "hHitWonTMap_" + side_id[iside], "Leading Edge in Time Window per Reconstructed Track" + regionTag, s_Straw_max[1], 0, s_Straw_max[1], "Straw Number", "Norm. Entries", scode);
					m_hStrawEffDetPhi_E[iside] = bookTProfile_LW(trackShift, "hStrawEffDetPhi_" + side_id[iside], "Straw Efficiency on Track with " + distance + " mm Cut vs #phi(2D)" + regionTag, 32, 0, 32, 0, 1.2, "Stack", "Avg. Straw Efficiency", scode);
				}
			}

			m_hHitsOnTrack_Scatter[ibe] = bookTH2F_LW(trackShift, "m_hHitsOnTrack_Scatter", "Hits per Track in Time Window in Stacks" + regionTag, 1440, -0.5, 1440 - 0.5, 80, 0, 80, "Luminosity Block (mod 1440)", "Number of Hits per Track in Stacks", scode);
			m_hLLOcc_Scatter[ibe] = bookTH2F_LW(trackShift, "m_hLLOcc_Scatter", "LL Occupancy in Stacks" + regionTag, 1440, -0.5, 1440 - 0.5, 400, 0.0, 1.0, "Luminosity Block (mod 1440)", "LL Occupancy in Stacks", scode);
			m_hHightoLowRatioOnTrack_Scatter[ibe] = bookTH2F_LW(trackShift, "m_hHightoLowRatioOnTrack_Scatter", "HL/LL Ratio on Track in Stacks" + regionTag, 1440, -0.5, 1440 - 0.5, 40, 0.0, 0.5, "Luminosity Block (mod 1440)", "HL/LL Ratio in Stacks", scode);
		}

		//Initialize Aging plots
		if (newLumiBlock && m_doShift) {
			for (int iL = 0; iL < 5; iL++) {
				for (int iSide = 0; iSide < 2; iSide++) {
					if (ibe == 0) {
						if (iL < 3) {
							m_trackz_All[iL][iSide] = bookTH1F_LW(trackAging, "trackz_m" + Mod[iL] + "_" + side_id[iSide] + "_All", "Number All Hits side " + side_id[iSide] + " Layer " + Mod[iL], 30, -750., 750., "z [mm]", "Number of Hits", scode);
							m_trackz_HT[iL][iSide] = bookTH1F_LW(trackAging, "trackz_m" + Mod[iL] + "_" + side_id[iSide] + "_HT", "Number HT Hits side " + side_id[iSide] + " Layer " + Mod[iL], 30, -750., 750., "z [mm]", "Number of HT Hits", scode);
						}

						if (iL == 3) {
							m_trackz_All[iL][iSide] = bookTH1F_LW(trackAging, "trackz_m1_" + side_id[iSide] + "_All_" + Mod[iL], "Number All Hits side " + side_id[iSide] + " Layer 1 " + Mod[iL], 30, 0., 725., "z [mm]", "Number of Hits", scode);
							m_trackz_HT[iL][iSide] = bookTH1F_LW(trackAging, "trackz_m1_" + side_id[iSide] + "_HT_" + Mod[iL], "Number HT Hits side " + side_id[iSide] + " Layer 1 " + Mod[iL], 30, 0., 725., "z [mm]", "Number of HT Hits", scode);
						}

						if (iL == 4) {
							m_trackz_All[iL][iSide] = bookTH1F_LW(trackAging, "trackz_m1_" + side_id[iSide] + "_All_" + Mod[iL], "Number All Hits side " + side_id[iSide] + " Layer 1 " + Mod[iL], 30, -725., 0., "z [mm]", "Number of Hits", scode);
							m_trackz_HT[iL][iSide] = bookTH1F_LW(trackAging, "trackz_m1_" + side_id[iSide] + "_HT_" + Mod[iL], "Number HT Hits side " + side_id[iSide] + " Layer 1 " + Mod[iL], 30, -725., 0., "z [mm]", "Number of HT Hits", scode);
						}
					} else if (ibe == 1) {
						// prevent double booking of histograms here
						if (iL < 4) {
							m_trackr_All[iL][iSide] = bookTH1F_LW(trackAging, "trackr_E" + side_id[iSide] + "_" + gas[iL] + "_All", "Number All Hits E" + side_id[iSide] + " " + gas[iL], 30, 644., 1004., "r [mm]", "Number of Hits", scode);
							m_trackr_HT[iL][iSide] = bookTH1F_LW(trackAging, "trackr_E" + side_id[iSide] + "_" + gas[iL] + "_HT", "Number HT Hits E" + side_id[iSide] + " " + gas[iL], 30, 644., 1004., "r [mm]", "Number of HT Hits", scode);
						}
					}
				}
			}
		}
	}

	return scode;
}

//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::fillHistograms() {
//----------------------------------------------------------------------------------//
	ATH_MSG_VERBOSE("Monitoring Histograms being filled");

	// Retrieve containers/objects only once per event
	// Dereference handles to pass them to methods
	SG::ReadHandle<TRT_RDO_Container>   rdoContainer(m_rdoContainerKey);
	SG::ReadHandle<TrackCollection>     trackCollection(m_trackCollectionKey);
	SG::ReadHandle<TrackCollection>     combTrackCollection(m_combTrackCollectionKey);
	SG::ReadHandle<xAOD::EventInfo>     xAODEventInfo(m_xAODEventInfoKey);
	SG::ReadHandle<InDetTimeCollection> trtBCIDCollection(m_TRT_BCIDCollectionKey);
	SG::ReadHandle<ComTime>             comTimeObject(m_comTimeObjectKey);
	SG::ReadHandle<xAOD::TrigDecision>  trigDecision(m_trigDecisionKey);

	if (!xAODEventInfo.isValid()) {
		ATH_MSG_ERROR("Could not find event info object " << m_xAODEventInfoKey.key() <<
		              " in store");
		return StatusCode::FAILURE;
	}

	if (m_doRDOsMon) {
		if (!rdoContainer.isValid()) {
			ATH_MSG_ERROR("Could not find TRT Raw Data Object " << m_rdoContainerKey.key() <<
			              " in store");
			return StatusCode::FAILURE;
		}

		ATH_CHECK( checkEventBurst(*rdoContainer) );

		if (m_passEventBurst) {
			m_totalEvents++;
			m_evtLumiBlock++;
			if (!trtBCIDCollection.isValid()) {
				ATH_MSG_ERROR("Could not find BCID collection " << m_TRT_BCIDCollectionKey.key() <<
				              " in store");
				return StatusCode::FAILURE;
			}

			ATH_CHECK( fillTRTRDOs(*rdoContainer, *trtBCIDCollection, *xAODEventInfo) );
		}
	} else {
		m_totalEvents++;
		m_passEventBurst = true;
		m_evtLumiBlock++;
	}

	if (m_doTracksMon) {
		if (!trackCollection.isValid()) {
			ATH_MSG_ERROR("Could not find track collection " << m_trackCollectionKey.key() <<
			              " in store");
			return StatusCode::FAILURE;
		}
		if (!trigDecision.isValid()) {
			ATH_MSG_ERROR("Could not find trigger decision object " << m_trigDecisionKey.key() <<
			              " in store");
			return StatusCode::FAILURE;
		}
		if (!comTimeObject.isValid()) {
			ATH_MSG_DEBUG("Could not find com time object " << m_comTimeObjectKey.key() <<
			              " in store");
		}
		if (m_passEventBurst) {
			ATH_CHECK( fillTRTTracks(*trackCollection, *trigDecision, comTimeObject.ptr()) );
		}
	}

	if (m_doEfficiency) {
		if (!combTrackCollection.isValid()) {
			ATH_MSG_ERROR("Could not find track collection " << m_combTrackCollectionKey.key() <<
			              " in store");
			return StatusCode::FAILURE;
		}

		ATH_CHECK( fillTRTEfficiency(*combTrackCollection) );
	}

	if (!m_doTracksMon) {
		if (!trackCollection.isValid()) {
			ATH_MSG_ERROR("Could not find track collection " << m_trackCollectionKey.key() <<
			              " in store");
			return StatusCode::FAILURE;
		}
	}

	if (m_passEventBurst) {
		ATH_CHECK( fillTRTHighThreshold(*trackCollection, *xAODEventInfo) );
	}

	return StatusCode::SUCCESS;
}

// Process all of the Histrograms.  ie divide, multiply..etc and write them to file.
//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::procHistograms() {
//----------------------------------------------------------------------------------//
	double n_BorE[2][2], total_BorE[2][2];
	double nfill[2] = {3.0, 2.0};  // [0]:barrel, [1]:endcap

	//proccesing of online histograms
	if (m_environment != AthenaMonManager::online) {
		if (m_doShift && m_doRDOsMon) {
			m_hSummary->SetBinContent(1, m_totalEvents);

			if (m_doTracksMon) {
				m_hSummary->SetBinContent(2, m_nTotalTracks);
				m_hSummary->SetBinContent(3, m_nTracksB[0]);
				m_hSummary->SetBinContent(4, m_nTracksB[1]);
				m_hSummary->SetBinContent(5, m_nTracksEC[0]);
				m_hSummary->SetBinContent(6, m_nTracksEC[1]);
				m_hSummary->SetBinContent(7, m_nTracksEC_B[0]);
				m_hSummary->SetBinContent(8, m_nTracksEC_B[1]);
			}
		}

		if (m_totalEvents < m_usedEvents || m_usedEvents < 0) {
			m_usedEvents = m_totalEvents;
		}

		// ibe = 0 (Barrel), ibe = 1 (Endcap)
		for (int ibe = 0; ibe < 2; ibe++) {
			//Loop over stack histograms and normalize to number of events processed.
			if (m_doChips && m_doExpert && endOfRunFlag()) {
				for (int i = 0; i < 64; i++) {
					if (m_doTracksMon && m_doExpert) {
						divide_LWHist(m_hHitOnTrackVsAllC[ibe][i], m_hHitAonTMapC[ibe][i], m_hHitAMapC[ibe][i]);
					}

					if (m_doRDOsMon) {
						float scale = (float)m_usedEvents * 16;

						if (scale > 0) {
							scale = 1. / scale;
							scale_LWHist(m_hHitHWMapC[ibe][i], scale);
							scale_LWHist(m_hHitWMapC[ibe][i], scale);
							scale_LWHist(m_hHitAMapC[ibe][i], scale);
							scale_LWHist(m_hHitAWMapC[ibe][i], scale);
							scale_LWHist(m_hHitHMapC[ibe][i], scale);
						}
					}

					for (int j = 0; j < s_iChip_max[ibe]; j++) {
						if (m_doRDOsMon) m_hChipOcc[ibe][i]->Fill(m_hHitAMapC[ibe][i]->GetBinContent(j + 1));

						if (m_doTracksMon) {
							float scale = m_hChipsEff[ibe][i]->GetBinEntries(j + 1);

							if (scale > 0) {
								scale = 1. / scale;
								m_hHitAonTMapC[ibe][i]->SetBinContent(j + 1, m_hHitAonTMapC[ibe][i]->GetBinContent(j + 1) * scale);
								m_hHitWonTMapC[ibe][i]->SetBinContent(j + 1, m_hHitWonTMapC[ibe][i]->GetBinContent(j + 1) * scale);
								m_hHitAWonTMapC[ibe][i]->SetBinContent(j + 1, m_hHitAWonTMapC[ibe][i]->GetBinContent(j + 1) * scale);
								m_hHitHonTMapC[ibe][i]->SetBinContent(j + 1, m_hHitHonTMapC[ibe][i]->GetBinContent(j + 1) * scale);
								m_hHitHWonTMapC[ibe][i]->SetBinContent(j + 1, m_hHitHWonTMapC[ibe][i]->GetBinContent(j + 1) * scale);
							} else {
								m_hHitAonTMapC[ibe][i]->SetBinContent(j + 1, 0);
								m_hHitWonTMapC[ibe][i]->SetBinContent(j + 1, 0);
								m_hHitAWonTMapC[ibe][i]->SetBinContent(j + 1, 0);
								m_hHitHonTMapC[ibe][i]->SetBinContent(j + 1, 0);
								m_hHitHWonTMapC[ibe][i]->SetBinContent(j + 1, 0);
							}
						}
					}

					if (m_doRDOsMon && m_doExpert) {
						divide_LWHist(m_hHtoLMapC[ibe][i], m_hHitHMapC[ibe][i], m_hHitAMapC[ibe][i]);
					}

					if (m_doTracksMon && m_doExpert) {
						divide_LWHist(m_hHtoLonTMapC[ibe][i], m_hHitHonTMapC[ibe][i], m_hHitAonTMapC[ibe][i]);
						divide_LWHist(m_hHtoLWonTMapC[ibe][i], m_hHitHWonTMapC[ibe][i], m_hHitAWonTMapC[ibe][i]);
					}
				}
			}

			if (m_doStraws && endOfRunFlag()) {
				if (m_doRDOsMon && m_usedEvents > 0) {
					if (ibe == 0) {
						//fix for leading edge in time window probability vs straw number(Barrel) histograms
						m_initScaleVectors();
						vector<float> scalevector;
						vector<float> scalevector_Ar;

						for (int k = 0; k < s_Straw_max[0]; k++) {
							try {
								if (m_scale_hHitWMap_B.at(k) == 0.) {
									scalevector.push_back(0.);
								} else {
									scalevector.push_back(1. / (m_usedEvents * m_scale_hHitWMap_B.at(k)));
								}

								if (m_scale_hHitWMap_B_Ar.at(k) == 0.) {
									scalevector_Ar.push_back(0.);
								} else {
									scalevector_Ar.push_back(1. / (m_usedEvents * m_scale_hHitWMap_B_Ar.at(k)));
								}
							} catch (out_of_range &e) {
								ATH_MSG_ERROR("Index " << k << " out of range in scaling for hHitWMap");
							}
						}

						scale_LWHistWithScaleVector(m_hHitWMap_B, scalevector);

						if (m_ArgonXenonSplitter) {
							scale_LWHistWithScaleVector(m_hHitWMap_B_Ar, scalevector_Ar);
						}
					} else if (ibe == 1) {
						float eventScale = 1. / (m_usedEvents * 32);
						scale_LWHist(m_hHitWMap_E[0], eventScale);
						scale_LWHist(m_hHitWMap_E[1], eventScale);

						if (m_ArgonXenonSplitter) {
							scale_LWHist(m_hHitWMap_E_Ar[0], eventScale);
							scale_LWHist(m_hHitWMap_E_Ar[1], eventScale);
						}
					}
				}

				for (int i = 0; i < 64; i++) {
					if (m_doTracksMon && m_doExpert) {
						divide_LWHist(m_hHitOnTrackVsAllS[ibe][i], m_hHitAonTMapS[ibe][i], m_hHitAMapS[ibe][i]);
					}

					if (m_doRDOsMon && m_doExpert && m_usedEvents > 0) {
						float scale = 1. / m_usedEvents;
						scale_LWHist(m_hHitHWMapS[ibe][i], scale);
						scale_LWHist(m_hHitWMapS[ibe][i], scale);
						scale_LWHist(m_hHitAMapS[ibe][i], scale);
						scale_LWHist(m_hHitAWMapS[ibe][i], scale);
						scale_LWHist(m_hHitHMapS[ibe][i], scale);
					}

					for (int j = 0; j < s_Straw_max[ibe]; j++) {
						if (m_doRDOsMon) {
							if (m_doExpert) {
								m_hStrawOcc[ibe][i]->Fill(m_hHitAMapS[ibe][i]->GetBinContent(j + 1));
							}
						}

						if (m_doTracksMon) {
							if (i == 0) {
								if (ibe == 0) {
									if (m_nStrawHits_B[j] > 0) {
										m_hHitWonTMap_B->SetBinContent(j + 1, m_hHitWonTMap_B->GetBinContent(j + 1) / ((Float_t)m_nStrawHits_B[j]));
									}
								} else if (ibe == 1) {
									if (m_nStrawHits_E[0][j] > 0) {
										m_hHitWonTMap_E[0]->SetBinContent(j + 1, (float)m_hHitWonTMap_E[0]->GetBinContent(j + 1) / (m_nStrawHits_E[0][j]));
									}

									if (m_nStrawHits_E[1][j] > 0) {
										m_hHitWonTMap_E[1]->SetBinContent(j + 1, (float)m_hHitWonTMap_E[1]->GetBinContent(j + 1) / (m_nStrawHits_E[1][j]));
									}
								}
							}

							if (m_doExpert) {
								float scale = m_hStrawsEff[ibe][i]->GetBinEntries(j + 1);

								if (scale > 0 && m_hStrawsEff[ibe][i]->GetBinEntries(j + 1) > 0) {
									scale = 1. / scale;
									m_hHitAWonTMapS[ibe][i]->SetBinContent(j + 1, m_hHitAWonTMapS[ibe][i]->GetBinContent(j + 1) * scale);
									m_hHitAonTMapS[ibe][i]->SetBinContent(j + 1, m_hHitAonTMapS[ibe][i]->GetBinContent(j + 1) * scale);
									m_hHitHonTMapS[ibe][i]->SetBinContent(j + 1, m_hHitHonTMapS[ibe][i]->GetBinContent(j + 1) * scale);
									m_hHitWonTMapS[ibe][i]->SetBinContent(j + 1, m_hHitWonTMapS[ibe][i]->GetBinContent(j + 1) * scale);
								} else {
									m_hHitAWonTMapS[ibe][i]->SetBinContent(j + 1, 0);
									m_hHitAonTMapS[ibe][i]->SetBinContent(j + 1, 0);
									m_hHitHonTMapS[ibe][i]->SetBinContent(j + 1, 0);
									m_hHitWonTMapS[ibe][i]->SetBinContent(j + 1, 0);
								}
							}
						}
					}

					if (m_doRDOsMon && m_doExpert) divide_LWHist(m_hHtoLMapS[ibe][i], m_hHitHMapS[ibe][i], m_hHitAMapS[ibe][i]);

					if (m_doTracksMon && m_doExpert) {
						divide_LWHist(m_hHtoLonTMapS[ibe][i], m_hHitHonTMapS[ibe][i], m_hHitAonTMapS[ibe][i]);
						divide_LWHist(m_hHtoLWonTMapS[ibe][i], m_hHitHWonTMapS[ibe][i], m_hHitAWonTMapS[ibe][i]);
					}
				}
			}

			if (m_doShift && endOfRunFlag()) {
				if (m_doTracksMon) {
					if (ibe == 0) {
						EventPhaseScale = m_hEvtPhase->GetEntries() * 3.125;

						if (EventPhaseScale > 0) {
							scale_LWHist(m_hEvtPhase, 1. / EventPhaseScale);
						}

						DriftTimeonTrkDistScale_B = m_hDriftTimeonTrkDist_B->GetEntries() * 3.125;

						if (DriftTimeonTrkDistScale_B > 0) {
							scale_LWHist(m_hDriftTimeonTrkDist_B, 1. / DriftTimeonTrkDistScale_B);
						}

						HLhitOnTrackScale_B = m_hHLhitOnTrack_B->GetEntries();

						if (HLhitOnTrackScale_B > 0) {
							scale_LWHist(m_hHLhitOnTrack_B, 1. / HLhitOnTrackScale_B);
						}

						HtoLRatioOnTrackScale_B = m_hHtoLRatioOnTrack_B->GetEntries() * 0.02;

						if (HtoLRatioOnTrackScale_B > 0) {
							scale_LWHist(m_hHtoLRatioOnTrack_B, 1. / HtoLRatioOnTrackScale_B);
						}

						NumSwLLWoTScale_B = m_hNumSwLLWoT_B->GetEntries();

						if (NumSwLLWoTScale_B > 0) {
							scale_LWHist(m_hNumSwLLWoT_B, 1. / NumSwLLWoTScale_B);
						}

						WireToTrkPositionScale_B = m_hWireToTrkPosition_B->GetEntries() * 0.1;

						if (WireToTrkPositionScale_B > 0) {
							scale_LWHist(m_hWireToTrkPosition_B, 1. / WireToTrkPositionScale_B);
						}

						TronTDistScale_B = m_hTronTDist_B->GetEntries() * 3.125;

						if (TronTDistScale_B > 0) {
							scale_LWHist(m_hTronTDist_B, 1. / TronTDistScale_B);
						}

						ResidualScale_B = m_hResidual_B->GetEntries() * 0.025;

						if (ResidualScale_B > 0) {
							scale_LWHist(m_hResidual_B, 1. / ResidualScale_B);
						}

						ResidualScale_B_20GeV = m_hResidual_B_20GeV->GetEntries() * 0.025;

						if (ResidualScale_B_20GeV > 0) {
							scale_LWHist(m_hResidual_B_20GeV, 1. / ResidualScale_B_20GeV);
						}

						TimeResidualScale_B = m_hTimeResidual_B->GetEntries() * 0.2;

						if (TimeResidualScale_B > 0) {
							scale_LWHist(m_hTimeResidual_B, 1. / TimeResidualScale_B);
						}

						if (m_ArgonXenonSplitter) {
							DriftTimeonTrkDistScale_B_Ar = m_hDriftTimeonTrkDist_B_Ar->GetEntries() * 3.125;

							if (DriftTimeonTrkDistScale_B_Ar > 0) {
								scale_LWHist(m_hDriftTimeonTrkDist_B_Ar, 1. / DriftTimeonTrkDistScale_B_Ar);
							}

							WireToTrkPositionScale_B_Ar = m_hWireToTrkPosition_B_Ar->GetEntries() * 0.1;

							if (WireToTrkPositionScale_B_Ar > 0) {
								scale_LWHist(m_hWireToTrkPosition_B_Ar, 1. / WireToTrkPositionScale_B_Ar);
							}

							HtoLRatioOnTrackScale_B_Ar = m_hHtoLRatioOnTrack_B_Ar->GetEntries() * 0.02;

							if (HtoLRatioOnTrackScale_B_Ar > 0) {
								scale_LWHist(m_hHtoLRatioOnTrack_B_Ar, 1. / HtoLRatioOnTrackScale_B_Ar);
							}

							TronTDistScale_B_Ar = m_hTronTDist_B_Ar->GetEntries() * 3.125;

							if (TronTDistScale_B_Ar > 0) {
								scale_LWHist(m_hTronTDist_B_Ar, 1. / TronTDistScale_B_Ar);
							}

							ResidualScale_B_Ar = m_hResidual_B_Ar->GetEntries() * 0.025;

							if (ResidualScale_B_Ar > 0) {
								scale_LWHist(m_hResidual_B_Ar, 1. / ResidualScale_B_Ar);
							}

							ResidualScale_B_Ar_20GeV = m_hResidual_B_Ar_20GeV->GetEntries() * 0.025;

							if (ResidualScale_B_Ar_20GeV > 0) {
								scale_LWHist(m_hResidual_B_Ar_20GeV, 1. / ResidualScale_B_Ar_20GeV);
							}

							TimeResidualScale_B_Ar = m_hTimeResidual_B_Ar->GetEntries() * 0.2;

							if (TimeResidualScale_B_Ar > 0) {
								scale_LWHist(m_hTimeResidual_B_Ar, 1. / TimeResidualScale_B_Ar);
							}
						}

						HtoLRatioOnTrackScale_B_Xe = m_hHtoLRatioOnTrack_B_Xe->GetEntries() * 0.02;

						if (HtoLRatioOnTrackScale_B_Xe > 0) {
							scale_LWHist(m_hHtoLRatioOnTrack_B_Xe, 1. / HtoLRatioOnTrackScale_B_Xe);
						}
					} else if (ibe == 1) {
						for (int iside = 0; iside < 2; iside++) {
							DriftTimeonTrkDistScale_E[iside] = m_hDriftTimeonTrkDist_E[iside]->GetEntries() * 3.125;

							if (DriftTimeonTrkDistScale_E[iside] > 0) {
								scale_LWHist(m_hDriftTimeonTrkDist_E[iside], 1. / DriftTimeonTrkDistScale_E[iside]);
							}

							HLhitOnTrackScale_E[iside] = m_hHLhitOnTrack_E[iside]->GetEntries();

							if (HLhitOnTrackScale_E[iside] > 0) {
								scale_LWHist(m_hHLhitOnTrack_E[iside], 1. / HLhitOnTrackScale_E[iside]);
							}

							HtoLRatioOnTrackScale_E[iside] = m_hHtoLRatioOnTrack_E[iside]->GetEntries() * 0.02;

							if (HtoLRatioOnTrackScale_E[iside] > 0) {
								scale_LWHist(m_hHtoLRatioOnTrack_E[iside], 1. / HtoLRatioOnTrackScale_E[iside]);
							}

							NumSwLLWoTScale_E[iside] = m_hNumSwLLWoT_E[iside]->GetEntries();

							if (NumSwLLWoTScale_E[iside] > 0) {
								scale_LWHist(m_hNumSwLLWoT_E[iside], 1. / NumSwLLWoTScale_E[iside]);
							}

							WireToTrkPositionScale_E[iside] = m_hWireToTrkPosition_E[iside]->GetEntries() * 0.1;

							if (WireToTrkPositionScale_E[iside] > 0) {
								scale_LWHist(m_hWireToTrkPosition_E[iside], 1. / WireToTrkPositionScale_E[iside]);
							}

							TronTDistScale_E[iside] = m_hTronTDist_E[iside]->GetEntries() * 3.125;

							if (TronTDistScale_E[iside] > 0) {
								scale_LWHist(m_hTronTDist_E[iside], 1. / TronTDistScale_E[iside]);
							}

							ResidualScale_E[iside] = m_hResidual_E[iside]->GetEntries() * 0.025;

							if (ResidualScale_E[iside] > 0) {
								scale_LWHist(m_hResidual_E[iside], 1. / ResidualScale_E[iside]);
							}

							ResidualScale_E_20GeV[iside] = m_hResidual_E_20GeV[iside]->GetEntries() * 0.025;

							if (ResidualScale_E_20GeV[iside] > 0) {
								scale_LWHist(m_hResidual_E_20GeV[iside], 1. / ResidualScale_E_20GeV[iside]);
							}

							TimeResidualScale_E[iside] = m_hTimeResidual_E[iside]->GetEntries() * 0.2;

							if (TimeResidualScale_E[iside] > 0) {
								scale_LWHist(m_hTimeResidual_E[iside], 1. / TimeResidualScale_E[iside]);
							}

							HtoLRatioOnTrackScale_E_Xe[iside] = m_hHtoLRatioOnTrack_E_Xe[iside]->GetEntries() * 0.02;

							if (HtoLRatioOnTrackScale_E_Xe[iside] > 0) {
								scale_LWHist(m_hHtoLRatioOnTrack_E_Xe[iside], 1. / HtoLRatioOnTrackScale_E_Xe[iside]);
							}

							if (m_ArgonXenonSplitter) {
								DriftTimeonTrkDistScale_E_Ar[iside] = m_hDriftTimeonTrkDist_E_Ar[iside]->GetEntries() * 3.125;

								if (DriftTimeonTrkDistScale_E_Ar[iside] > 0) {
									scale_LWHist(m_hDriftTimeonTrkDist_E_Ar[iside], 1. / DriftTimeonTrkDistScale_E_Ar[iside]);
								}

								WireToTrkPositionScale_E_Ar[iside] = m_hWireToTrkPosition_E_Ar[iside]->GetEntries() * 0.1;

								if (WireToTrkPositionScale_E_Ar[iside] > 0) {
									scale_LWHist(m_hWireToTrkPosition_E_Ar[iside], 1. / WireToTrkPositionScale_E_Ar[iside]);
								}

								HtoLRatioOnTrackScale_E_Ar[iside] = m_hHtoLRatioOnTrack_E_Ar[iside]->GetEntries() * 0.02;

								if (HtoLRatioOnTrackScale_E_Ar[iside] > 0) {
									scale_LWHist(m_hHtoLRatioOnTrack_E_Ar[iside], 1. / HtoLRatioOnTrackScale_E_Ar[iside]);
								}

								TronTDistScale_E_Ar[iside] = m_hTronTDist_E_Ar[iside]->GetEntries() * 3.125;

								if (TronTDistScale_E_Ar[iside] > 0) {
									scale_LWHist(m_hTronTDist_E_Ar[iside], 1. / TronTDistScale_E_Ar[iside]);
								}

								ResidualScale_E_Ar[iside] = m_hResidual_E_Ar[iside]->GetEntries() * 0.025;

								if (ResidualScale_E_Ar[iside] > 0) {
									scale_LWHist(m_hResidual_E_Ar[iside], 1. / ResidualScale_E_Ar[iside]);
								}

								ResidualScale_E_Ar_20GeV[iside] = m_hResidual_E_Ar_20GeV[iside]->GetEntries() * 0.025;

								if (ResidualScale_E_Ar_20GeV[iside] > 0) {
									scale_LWHist(m_hResidual_E_Ar_20GeV[iside], 1. / ResidualScale_E_Ar_20GeV[iside]);
								}

								TimeResidualScale_E_Ar[iside] = m_hTimeResidual_E_Ar[iside]->GetEntries() * 0.2;

								if (TimeResidualScale_E_Ar[iside] > 0) {
									scale_LWHist(m_hTimeResidual_E_Ar[iside], 1. / TimeResidualScale_E_Ar[iside]);
								}
							}
						}
					}
				}
			}

			if (m_doEfficiency && endOfRunFlag()) {
				for (int iside = 0; iside < 2; iside++) {
					for (int i = 0; i < 32; i++) {
						for (int ibin = 0; ibin <= s_Straw_max[ibe]; ibin++) {
							if (m_doExpert) {
								if (m_hefficiencyS[ibe][i + (32 * iside)]->GetBinEntries(ibin) > m_min_tracks_straw) {
									m_hefficiency[ibe][iside]->Fill(m_hefficiencyS[ibe][i + (32 * iside)]->GetBinContent(ibin));
								}
							}
						}
					}

					n_BorE[ibe][iside] = m_hefficiency[ibe][iside]->GetEntries();
					total_BorE[ibe][iside] = 0.0;

					for (unsigned int ibin = 0; ibin <= m_hefficiency[ibe][iside]->GetXaxis()->GetNbins(); ibin++) {
						total_BorE[ibe][iside] += m_hefficiency[ibe][iside]->GetBinContent(ibin);
						m_hefficiencyIntegral[ibe][iside]->SetBinContent(ibin, n_BorE[ibe][iside] != 0.0 ? total_BorE[ibe][iside] / n_BorE[ibe][iside] : 0);
					}
				}
			}
		}
	}

	if (endOfLumiBlockFlag() || endOfRunFlag()) {
		if (m_doShift) {
			Int_t lumiblock1440 = m_lastLumiBlock % 1440;
			float lumiBlockScale = (m_evtLumiBlock > 0) ? (1. / m_evtLumiBlock) : 0;
			const float barrelConst = 1. / 105088;
			const float endcapConst = 1. / 122880;

			if (m_doTracksMon && m_evtLumiBlock > 0) {
				m_hNHitsperLB_B->Fill(m_lastLumiBlock,   (float)nHitsperLB_B * lumiBlockScale * barrelConst);
				m_hNTrksperLB_B->Fill(m_lastLumiBlock,   (float)nTrksperLB_B * lumiBlockScale);
				m_hNHLHitsperLB_B->Fill(m_lastLumiBlock, (float)nHLHitsperLB_B * lumiBlockScale * barrelConst);

				for (int iside = 0; iside < 2; iside++) {
					m_hNHitsperLB_E[iside]->Fill(m_lastLumiBlock, (float)nHitsperLB_E[iside] * lumiBlockScale * endcapConst);
					m_hNTrksperLB_E[iside]->Fill(m_lastLumiBlock,  (float)nTrksperLB_E[iside] * lumiBlockScale);
					m_hNHLHitsperLB_E[iside]->Fill(m_lastLumiBlock, (float)nHLHitsperLB_E[iside] * lumiBlockScale * endcapConst);
				}

				nTrksperLB_B = 0;
				nHitsperLB_B = 0;
				nHLHitsperLB_B = 0;

				for (int iside = 0; iside < 2; iside++) {
					nTrksperLB_E[iside] = 0;
					nHitsperLB_E[iside] = 0;
					nHLHitsperLB_E[iside] = 0;
				}

				// ibe = 0 (Barrel), ibe = 1 (Endcap)
				for (int ibe = 0; ibe < 2; ibe++) {
					for (int i = 0; i < s_iStack_max[ibe]; i++) {
						if (ibe == 0) {
							if (m_evtLumiBlock > 0) {
								float occ = (m_LLOcc[ibe][i] * lumiBlockScale) / nfill[ibe];
								m_hLLOcc_Scatter[ibe]->Fill(lumiblock1440, occ);
								occ = (m_LLOcc[ibe][i + 32] * lumiBlockScale) / nfill[ibe];
								m_hLLOcc_Scatter[ibe]->Fill(lumiblock1440, occ);
							}

							m_LLOcc[ibe][i] = 0;
							m_LLOcc[ibe][i + 32] = 0;

							if (m_nTrack_B[i]) {
								float ratio = m_HTfraconTrack_B[i] / m_nTrack_B[i];
								m_hHightoLowRatioOnTrack_Scatter[ibe]->Fill(lumiblock1440, ratio);
								m_hHitsOnTrack_Scatter[ibe]->Fill(lumiblock1440, m_LonTrack_B[i] / m_nTrack_B[i]);
							}

							m_LonTrack_B[i] = 0;
							m_HTfraconTrack_B[i] = 0;
							m_nTrack_B[i] = 0;
						} else if (ibe == 1) {
							if (m_evtLumiBlock > 0) {
								float occ = (m_LLOcc[ibe][i] * lumiBlockScale) / nfill[ibe];
								m_hLLOcc_Scatter[ibe]->Fill(lumiblock1440, occ);
							}

							m_LLOcc[ibe][i] = 0;

							if (m_nTrack_E[i]) {
								float ratio = m_HTfraconTrack_E[i] / m_nTrack_E[i];
								m_hHightoLowRatioOnTrack_Scatter[ibe]->Fill(lumiblock1440, ratio);
								m_hHitsOnTrack_Scatter[ibe]->Fill(lumiblock1440, m_LonTrack_E[i] / m_nTrack_E[i]);
							}

							m_LonTrack_E[i] = 0;
							m_HTfraconTrack_E[i] = 0;
							m_nTrack_E[i] = 0;
						}
					}
				}
			}
		}

		//Resetting Occupuncy histograms for online environment
		if (m_doShift && m_environment == AthenaMonManager::online &&
		    (m_lastLumiBlock % m_lumiBlocksToResetOcc) == 0) {
			for (int ibe = 0; ibe < 2; ibe++) {
				for (int iside = 0; iside < 2; iside++) {
					m_hAvgHLOcc_side[ibe][iside]->Reset();
					m_hAvgLLOcc_side[ibe][iside]->Reset();
					m_hAvgHLOccMod_side[ibe][iside]->Reset();
					m_hAvgLLOccMod_side[ibe][iside]->Reset();
				}
			}
		}

		ATH_MSG_DEBUG("end of event and lumi block");
		//number of events in lumiblock counter setted to zero since it is end of the run or the lumiblock
		m_evtLumiBlock = 0;
	}

	if (endOfRunFlag()) {
		ATH_MSG_DEBUG("end of run");
	}

	return StatusCode::SUCCESS;
}

// Will be removed as m_rdoContainer will be changed to ReadHandle
// Get TRT Raw Data Objects (all TRT Hits)
//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::Retrieve_TRT_RDOs() {
//----------------------------------------------------------------------------------//
	ATH_MSG_VERBOSE("Retrieving RDO Container from StoreGate");
	StatusCode sc = StatusCode::SUCCESS;

	if (evtStore()->contains<TRT_RDO_Container>(m_rawDataObjectName)) {
		sc = evtStore()->retrieve(m_rdoContainer, m_rawDataObjectName);

		if (sc.isFailure() || !m_rdoContainer) {
			ATH_MSG_FATAL("Could not find the data object");
			return StatusCode::FAILURE;
		} else {
			ATH_MSG_DEBUG("Data Object " << m_rawDataObjectName << " found");
		}
	} else {
		ATH_MSG_WARNING("No TRT_RDO_Container by the name of " << m_rawDataObjectName << " in storegate");
		return StatusCode::FAILURE;
	}

	ATH_MSG_VERBOSE("Leaving Retrieve_TRT_RDOs()");
	return sc;
}//Retrieve_TRT_RDOs()


//Check for EventBurst: Counts highlevelhits and returns m_passEventBurst flag true if the count is less than m_m_passEventBurstCut,returns allways succes
//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::checkEventBurst(const TRT_RDO_Container& rdoContainer) {
//----------------------------------------------------------------------------------//
	m_passEventBurst = true;

	if (m_EventBurstCut <= 0) return StatusCode::SUCCESS;

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

	if (nHLHits > m_EventBurstCut) m_passEventBurst = false;

	return StatusCode::SUCCESS;
}

//Now Fill the TRT RDO Histograms
//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::fillTRTRDOs(const TRT_RDO_Container& rdoContainer,
                                            const InDetTimeCollection& trtBCIDCollection,
                                            const xAOD::EventInfo& eventInfo) {
//----------------------------------------------------------------------------------//
	ATH_MSG_DEBUG("Filling TRT RDO Histograms");
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

	InDetTimeCollection::const_iterator itrt_bcid = trtBCIDCollection.begin();

	while (goodid_status == 0 && itrt_bcid != trtBCIDCollection.end()) {
		if (!(*itrt_bcid)) continue;

		const unsigned int trt_bcid = (*itrt_bcid)->second;

		if (itrt_bcid > trtBCIDCollection.begin() && prev_bcid - trt_bcid == 0) {
			goodid_status = 1;
		} else if (itrt_bcid > trtBCIDCollection.begin() && prev_bcid - trt_bcid != 0) {
			ATH_MSG_WARNING("TRT BCID is not consistent.  TRT RODID is " <<
			                std::hex << (*itrt_bcid)->first << " trt bcid from ROD is " <<
			                std::hex << trt_bcid);
		}

		prev_bcid = trt_bcid;
		++itrt_bcid;
	}

	// Test out the TRT_StrawStatusSummarySvc.
	if (!m_sumSvc.name().empty() && m_doExpert) {
		ATH_MSG_VERBOSE("Trying " << m_sumSvc << " isGood");
		ATH_MSG_VERBOSE("TRT_StrawStatusSvc reports status = " << m_sumSvc->getStatus(TRT_Identifier));
	}

	// ibe = 0 (Barrel), ibe = 1 (Endcap)
	for (int ibe = 0; ibe < 2; ibe++) {
		nTRTHits[ibe] = 0;

		//Take out normalization from previous event for online environment
		//Explanation: While online monitoring running we need to present out histograms repeatedly so we need to pay attention to normalization.
		//before adding any information from new event to normalized histograms we need to take out the normalization of the previous event by scaling histograms back.
		//After  we are done with filling those histograms we will normalize them again
		if (m_environment == AthenaMonManager::online && m_totalEvents > 0) {
			//Loop over stack histograms and normalize to number of events processed.
			if (m_doChips && m_doExpert) {
				for (int i = 0; i < 64; i++) {
					float scale = (m_totalEvents - 1) * 16;
					scale_LWHist(m_hHitHWMapC[ibe][i], scale);
					scale_LWHist(m_hHitWMapC[ibe][i], scale);
					scale_LWHist(m_hHitAMapC[ibe][i], scale);
					scale_LWHist(m_hHitAWMapC[ibe][i], scale);
					scale_LWHist(m_hHitHMapC[ibe][i], scale);
				}
			}

			if (m_doStraws) {
				if (ibe == 0) {
					m_initScaleVectors();
					vector<float> scalevector;
					vector<float> scalevector_Ar;

					for (int k = 0; k < s_Straw_max[0]; k++) {
						try {
							if (m_scale_hHitWMap_B.at(k) == 0.) {
								scalevector.push_back(0.);
							} else {
								scalevector.push_back((m_totalEvents - 1) * m_scale_hHitWMap_B.at(k));
							}
							if (m_scale_hHitWMap_B_Ar.at(k) == 0.) {
								scalevector_Ar.push_back(0.);
							} else {
								scalevector_Ar.push_back((m_totalEvents - 1) * m_scale_hHitWMap_B_Ar.at(k));
							}
						} catch (out_of_range &e) {
							ATH_MSG_ERROR("Index " << k << " out of range in scaling for hHitWMap");
						}
					}

					scale_LWHistWithScaleVector(m_hHitWMap_B, scalevector);

					if (m_ArgonXenonSplitter) {
						scale_LWHistWithScaleVector(m_hHitWMap_B_Ar, scalevector_Ar);
					}
				} else if (ibe == 1) {
					float scale = (m_totalEvents - 1) * 32;
					scale_LWHist(m_hHitWMap_E[0], scale);
					scale_LWHist(m_hHitWMap_E[1], scale);

					if (m_ArgonXenonSplitter) {
						scale_LWHist(m_hHitWMap_E_Ar[0], scale);
						scale_LWHist(m_hHitWMap_E_Ar[1], scale);
					}
				}

				if (m_doExpert) {
					for (int i = 0; i < 64; i++) {
						float scale = m_totalEvents - 1;
						scale_LWHist(m_hHitHWMapS[ibe][i], scale);
						scale_LWHist(m_hHitWMapS[ibe][i], scale);
						scale_LWHist(m_hHitAMapS[ibe][i], scale);
						scale_LWHist(m_hHitAWMapS[ibe][i], scale);
						scale_LWHist(m_hHitHMapS[ibe][i], scale);
					}
				}
			}
		}
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

			if (m_doMaskStraws && m_sumSvc->get_status(TRT_Identifier)) continue;

			int m_barrel_ec = m_pTRTHelper->barrel_ec(TRT_Identifier);
			//ToDo: Check TRT_LoLumRawData object
			const TRT_LoLumRawData *p_lolum = dynamic_cast<const TRT_LoLumRawData *>(*p_rdo);

			if (!p_lolum) continue;

			nhitsall++;
			int ibe = abs(m_barrel_ec) - 1;
			int iside = m_barrel_ec > 0 ? 0 : 1;

			//if m_barrel_ec is outof range go to next measurement in rdo_collection
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
			const bool isArgonStraw = (Straw_Gastype( m_sumSvc->getStatusHT(TRT_Identifier) ) == GasType::Ar);
			int m_phi_module     = m_pTRTHelper->phi_module(TRT_Identifier);
			int m_layer_or_wheel = m_pTRTHelper->layer_or_wheel(TRT_Identifier);
			int m_straw_layer    = m_pTRTHelper->straw_layer(TRT_Identifier);
			int m_straw          = m_pTRTHelper->straw(TRT_Identifier);
			int m_strawNumber;
			int m_chip = 0;
			int m_board = -1;
			//ToDo: Check if that is really neccessary
			bool is_barrel = m_pTRTHelper->is_barrel(TRT_Identifier);

			//check straw number and find the correct m_chip and m_ board values
			if ( is_barrel && ibe == 0 ) {
				m_strawNumber = strawNumber(m_straw, m_straw_layer, m_layer_or_wheel);

				if (m_strawNumber >= 0 && m_strawNumber < s_Straw_max[ibe]) {
					m_chip = mat_chip_B[m_phi_module][m_strawNumber];
				}

				m_board = chipToBoard(m_chip);
			} else if ( !is_barrel && ibe == 1 ) {
				m_strawNumber = strawNumberEndCap(m_straw, m_straw_layer, m_layer_or_wheel, m_phi_module, m_barrel_ec);

				if (m_strawNumber >= 0 && m_strawNumber < s_Straw_max[ibe]) {
					m_chip = mat_chip_E[m_phi_module][m_strawNumber];
				}

				m_board = chipToBoard_EndCap(m_chip);
			} else {
				m_strawNumber = -1;
			}

			if (m_strawNumber < 0 || m_strawNumber >= s_Straw_max[ibe]) {
				ATH_MSG_WARNING("Found m_strawNumber = " << m_strawNumber << " out of range.");
				continue;
			}

			//ToDo: rename them (They are clearly not member but have "m_" prefix)
			const int m_driftTimeBin  = p_lolum->driftTimeBin();
			const int m_trailingEdge  = p_lolum->trailingEdge();
			const bool m_highlevel    = is_middleHTbit_high;//Hardcoded Middle Bit
			const bool m_firstBinHigh = p_lolum->firstBinHigh(); // if the first time bin is up then the hit is out of time window
			const bool m_lastBinHigh  = p_lolum->lastBinHigh(); // if the last bin is up then the hit is out of time window.
			const float m_timeOverThreshold = p_lolum->timeOverThreshold();
			moduleNumber_barrel1[0] = m_phi_module;
			moduleNumber_barrel1[1] = m_phi_module + 96;
			moduleNumber_barrel2[0] = m_phi_module + s_numberOfBarrelStacks;
			moduleNumber_barrel2[1] = m_phi_module + s_numberOfBarrelStacks + 96;
			moduleNumber_barrel3[0] = m_phi_module + 2 * s_numberOfBarrelStacks;
			moduleNumber_barrel3[1] = m_phi_module + 2 * s_numberOfBarrelStacks + 96;
			moduleNumber_endcapA[0] = m_phi_module;
			moduleNumber_endcapA[1] = m_phi_module + 64;
			moduleNumber_endcapB[0] = m_phi_module + s_numberOfEndCapStacks;
			moduleNumber_endcapB[1] = m_phi_module + s_numberOfEndCapStacks + 64;
			int iphi_module = -999;

			if (iside == 0) {
				iphi_module = m_phi_module;
			} else if (iside == 1) {
				iphi_module = m_phi_module + 32;
			}

			if (ibe == 0) {
				nTRTHits[ibe]++;

				if (m_doStraws) {
					if (isArgonStraw) {
						m_hHitWMap_B_Ar->Fill(m_strawNumber);
					} else {
						m_hHitWMap_B->Fill(m_strawNumber);
					}
				}

				if (m_doShift) {
					nHitsperLB_B++;

					if (m_highlevel) {
						nHLHitsperLB_B++;
					}
				}
			} else if (ibe == 1) {
				nTRTHits[ibe]++;

				if (m_doStraws) {
					if (isArgonStraw) {
						m_hHitWMap_E_Ar[iside]->Fill(m_strawNumber);
					} else {
						m_hHitWMap_E[iside]->Fill(m_strawNumber);
					}
				}

				if (m_doShift) {
					nHitsperLB_E[iside]++;

					if (m_highlevel) {
						nHLHitsperLB_E[iside]++;
					}
				}
			}

			if (m_doExpert) {
				if ( (m_driftTimeBin > 2) && (m_driftTimeBin < 17) ) {
					if (m_doStraws) m_hHitWMapS[ibe][iphi_module]->Fill(m_strawNumber);

					if (m_doChips) m_hHitWMapC[ibe][iphi_module]->Fill(m_chip - 1);
				}

				float trailingEdgeScaled = (m_trailingEdge + 1) * 3.125;

				if ((m_trailingEdge < 23) && !m_lastBinHigh && !m_firstBinHigh) {
					if (m_doStraws) m_hHitTrWMapS[ibe][iphi_module]->Fill(m_strawNumber, trailingEdgeScaled);

					if (m_doChips) m_hHitTrWMapC[ibe][iphi_module]->Fill((m_chip - 1), trailingEdgeScaled);
				}

				if (m_doStraws) m_hHitTrMapS[ibe][iphi_module]->Fill(m_strawNumber, trailingEdgeScaled);

				if (m_doChips) m_hHitTrMapC[ibe][iphi_module]->Fill((m_chip - 1), trailingEdgeScaled);

				if (m_highlevel) {
					if (m_doStraws) m_hHitHMapS[ibe][iphi_module]->Fill(m_strawNumber);

					if (m_doChips) m_hHitHMapC[ibe][iphi_module]->Fill(m_chip - 1);

					if (is_middleHTbit_high) {
						if (m_doStraws) m_hHitHWMapS[ibe][iphi_module]->Fill(m_strawNumber);

						if (m_doChips) m_hHitHWMapC[ibe][iphi_module]->Fill(m_chip - 1);
					}
				}

				if (m_firstBinHigh || m_lastBinHigh || m_driftTimeBin > 0 || m_trailingEdge < 23) {
					if (m_doStraws) m_hHitAMapS[ibe][iphi_module]->Fill(m_strawNumber);

					if (m_doChips) m_hHitAMapC[ibe][iphi_module]->Fill(m_chip - 1);
				}

				if ( is_anybininVgate_high) {
					if (m_doStraws) m_hHitAWMapS[ibe][iphi_module]->Fill(m_strawNumber);

					if (m_doChips) m_hHitAWMapC[ibe][iphi_module]->Fill(m_chip - 1);
				}

				if (m_doStraws) {
					m_hHitToTMapS[ibe][iphi_module]->Fill(m_strawNumber, m_timeOverThreshold);

					if (m_timeOverThreshold > m_longToTCut) {
						m_hHitToTLongMapS[ibe][iphi_module]->Fill(m_strawNumber, m_timeOverThreshold);
						m_hHitToTLongTrMapS[ibe][iphi_module]->Fill(m_strawNumber, trailingEdgeScaled);
					}
				}

				if (m_doChips) m_hHitToTMapC[ibe][iphi_module]->Fill((m_chip - 1), m_timeOverThreshold);

				if (m_doChips) {
					if (p_lolum->highLevel(1)) {
						m_hHtoBCMapC[ibe][iphi_module]->Fill(0., m_chip - 1);
						m_hHtoBCMapB[ibe][iphi_module]->Fill(0., m_board - 1);
					}

					if (p_lolum->highLevel(2)) {
						m_hHtoBCMapC[ibe][iphi_module]->Fill(1., m_chip - 1);
						m_hHtoBCMapB[ibe][iphi_module]->Fill(1., m_board - 1);
					}

					if (p_lolum->highLevel(3)) {
						m_hHtoBCMapC[ibe][iphi_module]->Fill(2., m_chip - 1);
						m_hHtoBCMapB[ibe][iphi_module]->Fill(2., m_board - 1);
					}
				}
			}

			//Set Module Numbers.
			int moduleNumber = -1;

			if (ibe == 0) {
				if (m_layer_or_wheel == 0) {
					moduleNumber = moduleNumber_barrel1[iside];
					moduleHits_B[moduleNumber]++;
				} else if (m_layer_or_wheel == 1) {
					moduleNumber = moduleNumber_barrel2[iside];
					moduleHits_B[moduleNumber]++;
				} else if (m_layer_or_wheel == 2) {
					moduleNumber = moduleNumber_barrel3[iside];
					moduleHits_B[moduleNumber]++;
				}

				if (m_highlevel) {
					if (m_layer_or_wheel == 0) {
						moduleNumber = moduleNumber_barrel1[iside];
						HLmoduleHits_B[moduleNumber]++;
					} else if (m_layer_or_wheel == 1) {
						moduleNumber = moduleNumber_barrel2[iside];
						HLmoduleHits_B[moduleNumber]++;
					} else if (m_layer_or_wheel == 2) {
						moduleNumber = moduleNumber_barrel3[iside];
						HLmoduleHits_B[moduleNumber]++;
					}
				}
			} else if (ibe == 1) {
				if (m_layer_or_wheel < 6) {
					moduleNumber = moduleNumber_endcapA[iside];
					moduleHits_E[moduleNumber]++;
				} else if (m_layer_or_wheel > 5) {
					moduleNumber = moduleNumber_endcapB[iside];
					moduleHits_E[moduleNumber]++;
				}

				if (m_highlevel) {
					if (m_layer_or_wheel < 6) {
						moduleNumber = moduleNumber_endcapA[iside];
						HLmoduleHits_E[moduleNumber]++;
					} else if (m_layer_or_wheel > 5) {
						moduleNumber = moduleNumber_endcapB[iside];
						HLmoduleHits_E[moduleNumber]++;
					}
				}
			}
		}
	}

	m_hOccAll->Fill(nhitsall / 350848.);

	//ToDo Explain this
	for (int ibe = 0; ibe < 2; ibe++) {
		if (m_doShift) {
			if (ibe == 0) {
				m_hBCIDvsOcc[ibe]->Fill(m_good_bcid, nTRTHits[ibe] / 105088.0);
			} else if (ibe == 1) {
				m_hBCIDvsOcc[ibe]->Fill(m_good_bcid, nTRTHits[ibe] / 245760.0);
			}

			for (int iside = 0; iside < 2; iside++) {
				for (int i = 1; i <= numberOfStacks_b[ibe]; i++) {
					int index_tmp, modulenum_tmp;

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
							m_LLOcc[ibe][LLocc_index] += occLL;
							m_hAvgLLOcc_side[ibe][iside]->Fill(i - (32 * nclass), occLL);
							m_hAvgHLOcc_side[ibe][iside]->Fill(i - (32 * nclass), occHL);
							m_hAvgLLOccMod_side[ibe][iside]->Fill(i, occLL);
							m_hAvgHLOccMod_side[ibe][iside]->Fill(i, occHL);
						} else if (ibe == 1) {
							float occLL = float(moduleHits_E[modulenum_tmp]) / float(numberOfStrawsWheel[nclass]);
							float occHL = float(HLmoduleHits_E[modulenum_tmp]) / float(numberOfStrawsWheel[nclass]);

							if (LLocc_index < 64) {
								m_LLOcc[ibe][LLocc_index] += occLL;
							} else {
								ATH_MSG_WARNING("m_LLOcc index out of bounds!"); // To satisfy Coverity defect CID 16514 which we believe is a false report.
							}

							m_hAvgLLOcc_side[ibe][iside]->Fill(i - (32 * nclass), occLL);
							m_hAvgHLOcc_side[ibe][iside]->Fill(i - (32 * nclass), occHL);
							m_hAvgLLOccMod_side[ibe][iside]->Fill(i, occLL);
							m_hAvgHLOccMod_side[ibe][iside]->Fill(i, occHL);
						}
					}
				}
			}
		}

		//Normalization for online environmenmet
		if (m_environment == AthenaMonManager::online) {
			//Loop over stack histograms and normalize to number of events processed.
			if (m_doChips && m_doExpert) {
				for (int i = 0; i < 64; i++) {
					for (int j = 0; j < s_iChip_max[ibe]; j++) {
						m_hChipOcc[ibe][i]->Fill(m_hHitAMapC[ibe][i]->GetBinContent(j + 1));
					}

					if (m_totalEvents > 0) {
						const float scale = 1. / (16 * m_totalEvents);
						scale_LWHist(m_hHitHWMapC[ibe][i], scale);
						scale_LWHist(m_hHitWMapC[ibe][i], scale);
						scale_LWHist(m_hHitAMapC[ibe][i], scale);
						scale_LWHist(m_hHitAWMapC[ibe][i], scale);
						scale_LWHist(m_hHitHMapC[ibe][i], scale);
					}
				}
			}

			if (m_doStraws && m_doShift && m_totalEvents > 0) {
				if (ibe == 0) {
					m_initScaleVectors();
					vector<float> scalevector;
					vector<float> scalevector_Ar;

					for (int k = 0; k < s_Straw_max[0]; k++) {
						try {
							if (m_scale_hHitWMap_B.at(k) == 0.) {
								scalevector.push_back(0.);
							} else {
								scalevector.push_back(1. / (m_totalEvents * m_scale_hHitWMap_B.at(k)));
							}

							if (m_scale_hHitWMap_B_Ar.at(k) == 0.) {
								scalevector_Ar.push_back(0.);
							} else {
								scalevector_Ar.push_back(1. / (m_totalEvents * m_scale_hHitWMap_B_Ar.at(k)));
							}
						} catch (out_of_range &e) {
							ATH_MSG_ERROR("Index " << k << " out of range in scaling for hHitWMap");
						}
					}

					scale_LWHistWithScaleVector(m_hHitWMap_B, scalevector);

					if (m_ArgonXenonSplitter) {
						scale_LWHistWithScaleVector(m_hHitWMap_B_Ar, scalevector_Ar);
					}
				} else if (ibe == 1) {
					float eventScale = 1. / (32 * m_totalEvents);
					scale_LWHist(m_hHitWMap_E[0], eventScale);
					scale_LWHist(m_hHitWMap_E[1], eventScale);

					if (m_ArgonXenonSplitter) {
						scale_LWHist(m_hHitWMap_E_Ar[0], eventScale);
						scale_LWHist(m_hHitWMap_E_Ar[1], eventScale);
					}
				}
			}

			if (m_doStraws) {
				for (int i = 0; i < 64; i++) {
					if (m_doExpert && m_totalEvents > 0) {
						const float scale = 1. / m_totalEvents;
						scale_LWHist(m_hHitHWMapS[ibe][i], scale);
						scale_LWHist(m_hHitWMapS[ibe][i], scale);
						scale_LWHist(m_hHitAMapS[ibe][i], scale);
						scale_LWHist(m_hHitAWMapS[ibe][i], scale);
						scale_LWHist(m_hHitHMapS[ibe][i], scale);
					}

					for (int j = 0; j < s_iChip_max[ibe]; j++) {
						if (m_doExpert) m_hStrawOcc[ibe][i]->Fill(m_hHitAMapS[ibe][i]->GetBinContent(j + 1));
					}
				}
			}
		}
	}

	if (m_environment == AthenaMonManager::online) {
		if (m_doShift) m_hSummary->SetBinContent(1, m_totalEvents);
	}

	ATH_MSG_VERBOSE("Leaving Fill TRT RDO Histograms");
	return StatusCode::SUCCESS;
}

// Will be removed later
//Get the TRT Track Collections from store gate
//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::Retrieve_TRT_Tracks() {
//----------------------------------------------------------------------------------//
	ATH_MSG_VERBOSE("Retrieving TRT Tracks Container from StoreGate");

	if (evtStore()->contains<TrackCollection>(m_tracksObjectName)) {
		if (evtStore()->retrieve(m_trkCollection, m_tracksObjectName).isFailure()) {
			ATH_MSG_ERROR("Could not find Tracks Collection");
			return StatusCode::FAILURE;
		} else {
			ATH_MSG_VERBOSE("Tracks retrieved from StoreGate");
		}
	} else {
		ATH_MSG_WARNING("No TrackCollection by the name of " << m_tracksObjectName << " in storegate");
		return StatusCode::FAILURE;
	}

	if (evtStore()->contains<ComTime>(m_comTimeObjectName)) {
		if (evtStore()->retrieve(m_theComTime, m_comTimeObjectName).isFailure()) {
			ATH_MSG_DEBUG("ComTime object not found with name " << m_comTimeObjectName << ".");
			m_theComTime = 0; // protection for later on
		} else {
			ATH_MSG_DEBUG("ComTime object found successfully");
		}
	} else {
		ATH_MSG_DEBUG("No ComTime object found in storegate.");
		m_theComTime = 0;
	}

	return StatusCode::SUCCESS;
}

//Fill the TRT Track level histograms
//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::fillTRTTracks(const TrackCollection& trackCollection,
                                              const xAOD::TrigDecision& trigDecision,
                                              const ComTime* comTimeObject) {
//----------------------------------------------------------------------------------//
	ATH_MSG_VERBOSE("Filling TRT Tracks Histos");
	//Initialize a bunch of stuff before looping over the track collection. Fill some basic histograms.
	const float timeCor =  comTimeObject ? comTimeObject->getTime() : 0;
	auto p_trk = trackCollection.begin();
	const Trk::Perigee *m_mPer = nullptr;
	const DataVector<const Trk::TrackParameters> *AllTrkPar(0);
	DataVector<const Trk::TrackParameters>::const_iterator p_trkpariter;

	//Take out normalization of previous event for online environment
	//Explanation: While online monitoring running we need to present out histograms repeatedly so we need to pay attention to normalization.
	//before adding any information from new event to normalized histograms we need to take out the normalization of the previous event by scaling histograms back.
	//After  we are done with filling those histograms we will normalize them again
	if (m_environment == AthenaMonManager::online) {
		// ibe = 0 (Barrel), ibe = 1 (Endcap)
		for (int ibe = 0; ibe < 2; ibe++) {
			if (m_doChips && m_doExpert) {
				for (int i = 0; i < 64; i++) {
					for (int j = 0; j < s_iChip_max[ibe]; j++) {
						if (m_hChipsEff[ibe][i]->GetBinEntries(j + 1) > 0) {
							m_hHitAonTMapC[ibe][i]->SetBinContent(j + 1, m_hHitAonTMapC[ibe][i]->GetBinContent(j + 1) * m_hChipsEff[ibe][i]->GetBinEntries(j + 1));
							m_hHitWonTMapC[ibe][i]->SetBinContent(j + 1, m_hHitWonTMapC[ibe][i]->GetBinContent(j + 1) * m_hChipsEff[ibe][i]->GetBinEntries(j + 1));
							m_hHitAWonTMapC[ibe][i]->SetBinContent(j + 1, m_hHitAWonTMapC[ibe][i]->GetBinContent(j + 1) * m_hChipsEff[ibe][i]->GetBinEntries(j + 1));
							m_hHitHonTMapC[ibe][i]->SetBinContent(j + 1, m_hHitHonTMapC[ibe][i]->GetBinContent(j + 1) * m_hChipsEff[ibe][i]->GetBinEntries(j + 1));
							m_hHitHWonTMapC[ibe][i]->SetBinContent(j + 1, m_hHitHWonTMapC[ibe][i]->GetBinContent(j + 1) * m_hChipsEff[ibe][i]->GetBinEntries(j + 1));
						}
					}
				}
			}

			if (m_doStraws) {
				for (int i = 0; i < 64; i++) {
					for (int j = 0; j < s_Straw_max[ibe]; j++) {
						if (ibe == 0) {
							if (i == 0 && m_nStrawHits_B[j] > 0) {
								m_hHitWonTMap_B->SetBinContent(j + 1, m_hHitWonTMap_B->GetBinContent(j + 1) * m_nStrawHits_B[j]);
							}
						} else if (ibe == 1) {
							for (int iside = 0; iside < 2; iside++) {
								if (i == 0 && m_nStrawHits_E[iside][j] > 0) {
									m_hHitWonTMap_E[iside]->SetBinContent(j + 1, m_hHitWonTMap_E[iside]->GetBinContent(j + 1) * m_nStrawHits_E[iside][j]);
								}
							}
						}

						if (m_doExpert) {
							if (m_hStrawsEff[ibe][i]->GetBinEntries(j + 1) > 0) {
								m_hHitAWonTMapS[ibe][i]->SetBinContent(j + 1, m_hHitAWonTMapS[ibe][i]->GetBinContent(j + 1) * m_hStrawsEff[ibe][i]->GetBinEntries(j + 1));
								m_hHitAonTMapS[ibe][i]->SetBinContent(j + 1, m_hHitAonTMapS[ibe][i]->GetBinContent(j + 1) * m_hStrawsEff[ibe][i]->GetBinEntries(j + 1));
								m_hHitHonTMapS[ibe][i]->SetBinContent(j + 1, m_hHitHonTMapS[ibe][i]->GetBinContent(j + 1) * m_hStrawsEff[ibe][i]->GetBinEntries(j + 1));
								m_hHitWonTMapS[ibe][i]->SetBinContent(j + 1, m_hHitWonTMapS[ibe][i]->GetBinContent(j + 1) * m_hStrawsEff[ibe][i]->GetBinEntries(j + 1));
							}
						}
					}
				}
			}
		}

		if (m_doShift) {
			scale_LWHist(m_hEvtPhase, EventPhaseScale);
			scale_LWHist(m_hDriftTimeonTrkDist_B, DriftTimeonTrkDistScale_B);
			scale_LWHist(m_hHLhitOnTrack_B, HLhitOnTrackScale_B);
			scale_LWHist(m_hHtoLRatioOnTrack_B, HtoLRatioOnTrackScale_B);
			scale_LWHist(m_hNumSwLLWoT_B, NumSwLLWoTScale_B);
			scale_LWHist(m_hWireToTrkPosition_B, WireToTrkPositionScale_B);
			scale_LWHist(m_hTronTDist_B, TronTDistScale_B);
			scale_LWHist(m_hResidual_B, ResidualScale_B);
			scale_LWHist(m_hResidual_B_20GeV, ResidualScale_B_20GeV);
			scale_LWHist(m_hTimeResidual_B, TimeResidualScale_B);

			if (m_ArgonXenonSplitter) {
				scale_LWHist(m_hDriftTimeonTrkDist_B_Ar, DriftTimeonTrkDistScale_B_Ar);
				scale_LWHist(m_hWireToTrkPosition_B_Ar, WireToTrkPositionScale_B_Ar);
				scale_LWHist(m_hTronTDist_B_Ar, TronTDistScale_B_Ar);
				scale_LWHist(m_hResidual_B_Ar, ResidualScale_B_Ar);
				scale_LWHist(m_hResidual_B_Ar_20GeV, ResidualScale_B_Ar_20GeV);
				scale_LWHist(m_hTimeResidual_B_Ar, TimeResidualScale_B_Ar);
			}

			for (int iside = 0; iside < 2; iside++) {
				scale_LWHist(m_hDriftTimeonTrkDist_E[iside], DriftTimeonTrkDistScale_E[iside]);
				scale_LWHist(m_hHLhitOnTrack_E[iside], HLhitOnTrackScale_E[iside]);
				scale_LWHist(m_hHtoLRatioOnTrack_E[iside], HtoLRatioOnTrackScale_E[iside]);
				scale_LWHist(m_hNumSwLLWoT_E[iside], NumSwLLWoTScale_E[iside]);
				scale_LWHist(m_hWireToTrkPosition_E[iside], WireToTrkPositionScale_E[iside]);
				scale_LWHist(m_hTronTDist_E[iside], TronTDistScale_E[iside]);
				scale_LWHist(m_hResidual_E[iside], ResidualScale_E[iside]);
				scale_LWHist(m_hResidual_E_20GeV[iside], ResidualScale_E_20GeV[iside]);
				scale_LWHist(m_hTimeResidual_E[iside], TimeResidualScale_E[iside]);

				if (m_ArgonXenonSplitter) {
					scale_LWHist(m_hDriftTimeonTrkDist_E_Ar[iside], DriftTimeonTrkDistScale_E_Ar[iside]);
					scale_LWHist(m_hWireToTrkPosition_E_Ar[iside], WireToTrkPositionScale_E_Ar[iside]);
					scale_LWHist(m_hTronTDist_E_Ar[iside], TronTDistScale_E_Ar[iside]);
					scale_LWHist(m_hResidual_E_Ar[iside], ResidualScale_E_Ar[iside]);
					scale_LWHist(m_hResidual_E_Ar_20GeV[iside], ResidualScale_E_Ar_20GeV[iside]);
					scale_LWHist(m_hTimeResidual_E_Ar[iside], TimeResidualScale_E_Ar[iside]);
				}
			}
		}
	}

	int ntrackstack[2][64];

	for (int ibe = 0; ibe < 2; ibe++) {
		std::fill(ntrackstack[ibe], ntrackstack[ibe] + 64, 0);
	}

	for (; p_trk != trackCollection.end(); ++p_trk) {
		const std::auto_ptr<const Trk::TrackSummary> summary(m_TrackSummaryTool->createSummary(*(*p_trk)));
		int m_nTRTHits = summary->get(Trk::numberOfTRTHits);

		if (m_nTRTHits < m_minTRThits) continue;

		AllTrkPar = (*p_trk)->trackParameters();

		// Search of MeasuredPerigee in TrackParameters
		// The following algorithm only finds the First perigee measurement.
		// As there should be one and only one perigee measurement then this assumption should be valid.
		// But no check is done to see if there is more than one perigee measurement.
		for (p_trkpariter = AllTrkPar->begin(); p_trkpariter != AllTrkPar->end(); ++p_trkpariter) {
			//if track parameter does have a measured perigee then the track parameter is a keeper and break out of the loop
			if ((m_mPer = dynamic_cast<const Trk::Perigee *>(*p_trkpariter))) break;
		}

		if (!m_mPer) continue;

		float m_theta   =  m_mPer->parameters()[Trk::theta];
		float m_p       =  (m_mPer->parameters()[Trk::qOverP] != 0.) ? fabs(1. / (m_mPer->parameters()[Trk::qOverP])) : 10e7;
		float m_pT      =  (m_p * sin(m_theta));
		m_pT = m_pT * 1e-3;  // GeV

		if (m_p < m_minP) continue;

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
		bool is_pT_over_20GeV = false;

		if (m_mPer->pT() > 20 * CLHEP::GeV) {
			is_pT_over_20GeV = true;
		} else {
			is_pT_over_20GeV = false;
		}

		const bool cnst_is_pT_over_20GeV = is_pT_over_20GeV;
		///hardcoded cut for pT 2.0 GeV for collision setup
		float min_pt_new = m_min_pT;

		if (m_isCosmics == false) {
			min_pt_new = 2.0 * CLHEP::GeV;
		}

		const bool passed_track_preselection =
			(m_mPer->pT() > min_pt_new) &&
			(m_p > m_minP) &&
			(n_si_hits >= m_min_si_hits) &&
			(n_pixel_hits >= m_min_pixel_hits) &&
			(n_sct_hits >= m_min_sct_hits) &&
			(n_trt_hits >= m_min_trt_hits);

		if (!passed_track_preselection) continue;

		m_nTotalTracks++;
		int checkB[2] = {0, 0};
		int checkEC[2] = {0, 0};
		int checkEC_B[2] = {0, 0};
		int m_nTRTHitsW[2][2];
		int m_nTRTHitsW_Ar[2][2];
		int m_nTRTHitsW_Xe[2][2];
		int m_nTRTHLHitsW[2][2];
		int m_nTRTHLHitsW_Ar[2][2];
		int m_nTRTHLHitsW_Xe[2][2];
		int m_nTRTHits_side[2][2];
		int m_nTRTHitsW_perwheel[2][18];
		int hitontrack[2] = {0, 0};
		int hitontrack_E_side[2] = {0, 0};

		for (int ibe = 0; ibe < 2; ibe++) {
			for (int iside = 0; iside < 2; iside++) {
				m_nTRTHits_side[ibe][iside] = -1;
				m_nTRTHitsW[ibe][iside] = 0;
				m_nTRTHitsW_Ar[ibe][iside] = 0;
				m_nTRTHitsW_Xe[ibe][iside] = 0;
				m_nTRTHLHitsW[ibe][iside] = 0;
				m_nTRTHLHitsW_Ar[ibe][iside] = 0;
				m_nTRTHLHitsW_Xe[ibe][iside] = 0;
			}

			std::fill(m_nTRTHitsW_perwheel[ibe], m_nTRTHitsW_perwheel[ibe] + 18, 0);
		}

		bool isBarrelOnly = true;
		bool ECAhit = false;
		bool ECChit = false;
		bool Bhit = false;
		int m_barrel_ec = 0;
		int m_layer_or_wheel = 0;
		int m_phi_module = 0;
		int m_straw_layer = 0;
		int m_straw = 0;
		int m_nearest_straw_layer[2] = {100, 100};
		int m_nearest_straw[2] = {0, 0};
		int testLayer[2] = {100, 100};
		int innerstack[2] = {-999, -999};
		float m_phi2D[2] = {-100, -100};

		for (TSOSItBeginTemp = TSOSItBegin0; TSOSItBeginTemp != TSOSItEnd; ++TSOSItBeginTemp) {
			if ((*TSOSItBeginTemp) == 0) continue;

			if (! ((*TSOSItBeginTemp)->type(Trk::TrackStateOnSurface::Measurement)) ) continue;

			const InDet::TRT_DriftCircleOnTrack *trtCircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>((*TSOSItBeginTemp)->measurementOnTrack());

			if (!trtCircle) continue;

			const Trk::TrackParameters *aTrackParam = dynamic_cast<const Trk::TrackParameters *>((*TSOSItBeginTemp)->trackParameters());

			if (!aTrackParam) continue;

			Identifier DCoTId = trtCircle->identify();
			m_barrel_ec = m_pTRTHelper->barrel_ec(DCoTId);
			int ibe = abs(m_barrel_ec) - 1;
			m_layer_or_wheel = m_pTRTHelper->layer_or_wheel (DCoTId);
			m_straw_layer = m_pTRTHelper->straw_layer(DCoTId);
			m_straw = m_pTRTHelper->straw(DCoTId);

			//restrict ourselves to the inner most TRT layers To get detector phi.
			if (m_layer_or_wheel >= testLayer[ibe]) continue;

			testLayer[ibe] = m_layer_or_wheel;

			if (m_straw_layer < m_nearest_straw_layer[ibe]) {
				m_nearest_straw_layer[ibe] = m_straw_layer;
				m_nearest_straw[ibe] = m_straw;
				const InDetDD::TRT_BaseElement *circleElement = nullptr;
				circleElement = trtCircle->detectorElement();
				m_phi2D[ibe] = radToDegrees(circleElement->strawCenter(m_nearest_straw[ibe]).phi());
				circleElement = nullptr;
				innerstack[ibe] = m_pTRTHelper->phi_module(DCoTId);
			}
		}

		if (m_phi2D[0] == -999) {
			ATH_MSG_DEBUG("Track did not go through inner layer of Barrel.");
		} else {
			ATH_MSG_VERBOSE("Track's closest approach is m_layer_or_wheel: " <<
			                testLayer[0] << " m_straw_layer: " <<
			                m_nearest_straw_layer[0] << " (in the Barrel).");
		}

		if (m_phi2D[1] == -999) {
			ATH_MSG_DEBUG("Track did not go through any inner layer of EndCap A or C.");
		} else {
			ATH_MSG_VERBOSE("Track's closest approach is m_layer_or_wheel: " <<
			                testLayer[1] << " m_straw_layer: " <<
			                m_nearest_straw_layer[1] << " (in the EndCaps).");
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
			m_barrel_ec = m_pTRTHelper->barrel_ec(DCoTId);
			m_layer_or_wheel = m_pTRTHelper->layer_or_wheel(DCoTId);
			m_phi_module = m_pTRTHelper->phi_module(DCoTId);
			m_straw_layer = m_pTRTHelper->straw_layer(DCoTId);
			m_straw = m_pTRTHelper->straw(DCoTId);
			// ibe = 0 (Barrel), ibe = 1 (Endcap)
			int ibe = abs(m_barrel_ec) - 1;
			// iside = 0 (Side A), iside = 1 (Side C)
			int iside = m_barrel_ec > 0 ? 0 : 1;
			int m_strawNumber[2] = {-1, -1};
			int m_chip[2] = {0, 0};

			if (ibe == 0) {
				m_strawNumber[ibe] = strawNumber(m_straw, m_straw_layer, m_layer_or_wheel);

				if (m_strawNumber[ibe] >= 0 && m_strawNumber[ibe] < s_Straw_max[ibe]) {
					m_chip[ibe] = mat_chip_B[m_phi_module][m_strawNumber[ibe]];
				}
			} else if (ibe == 1) {
				m_strawNumber[ibe] = strawNumberEndCap(m_straw, m_straw_layer, m_layer_or_wheel, m_phi_module, m_barrel_ec);

				if (m_strawNumber[ibe] >= 0 && m_strawNumber[ibe] < s_Straw_max[ibe]) {
					m_chip[ibe] = mat_chip_E[m_phi_module][m_strawNumber[ibe]];
				}
			} else {
				m_strawNumber[ibe] = -1;
			}

			if (m_strawNumber[ibe] < 0 || m_strawNumber[ibe] >= s_Straw_max[ibe]) continue;

			if (checkB[iside] == 0 && ibe == 0) {
				m_nTracksB[iside]++;
				checkB[iside] = 1;
			}

			if (checkEC[iside] == 0 && ibe == 1) {
				m_nTracksEC[iside]++;
				checkEC[iside] = 1;
			}

			if (checkEC_B[iside] == 0 && checkB[iside] == 1 && ibe == 1 ) {
				m_nTracksEC_B[iside]++;
				checkEC_B[iside] = 1;
			}//ToDo: be sure about this approach

			if (ibe == 0) {
				Bhit = true;
			} else if (m_barrel_ec == 2) {
				isBarrelOnly = false;
				ECAhit = true;
			} else if (m_barrel_ec == -2) {
				isBarrelOnly = false;
				ECChit = true;
			}

			Identifier surfaceID;
			const Trk::MeasurementBase *mesb = (*TSOSItBegin)->measurementOnTrack();
			surfaceID = trtCircle->identify();
			const bool isArgonStraw = ( Straw_Gastype( m_sumSvc->getStatusHT(surfaceID) ) == GasType::Ar );
			// assume always Xe if m_ArgonXenonSplitter is not enabled, otherwise check the straw status (good is Xe, non-good is Ar)
			float temp_locr = aTrackParam->parameters()[Trk::driftRadius];
			TRTCond::RtRelation const *rtr = m_TRTCalDbSvc->getRtRelation(surfaceID);
			int iphi_module = -9999;

			if (iside == 0) iphi_module = m_phi_module;
			else if (iside == 1) iphi_module = m_phi_module + 32;

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
							m_hStrawEffDetPhi_B->Fill(m_phi_module, 1.0);
						}

						if (m_doStraws && m_doShift) {
							m_nStrawHits_B[m_strawNumber[ibe]]++;
						}
					} else if (ibe == 1) {
						hitontrack[ibe]++;
						hitontrack_E_side[iside]++;

						if (m_doShift) {
							m_hStrawEffDetPhi_E[iside]->Fill(m_phi_module, 1.0);
						}

						if (m_doStraws && m_doShift) {
							m_nStrawHits_E[iside][m_strawNumber[ibe]]++;
						}
					}

					if (m_doStraws && m_doExpert) {
						m_hStrawsEff[ibe][iphi_module]->Fill(m_strawNumber[ibe], 1.0);
					}

					if (m_doChips && m_doExpert) {
						m_hChipsEff[ibe][iphi_module]->Fill((m_chip[ibe] - 1), 1.0);
					}
				}
			} else {
				if (m_idHelper->is_trt(DCoTId)) { //ToDo:Is this really needed
					if (ibe == 0) {
						if (m_doShift) {
							m_hStrawEffDetPhi_B->Fill(m_phi_module, 0.0);
						}

						if (m_doStraws && m_doShift) {
							m_nStrawHits_B[m_strawNumber[ibe]]++;
						}
					} else if (ibe == 1) {
						if (m_doShift) {
							m_hStrawEffDetPhi_E[iside]->Fill(m_phi_module, 0.0);
						}

						if (m_doStraws && m_doShift) {
							m_nStrawHits_E[iside][m_strawNumber[ibe]]++;
						}
					}

					if (m_doStraws && m_doExpert) {
						m_hStrawsEff[ibe][iphi_module]->Fill(m_strawNumber[ibe], 0.0);
					}

					if (m_doChips  && m_doExpert) {
						m_hChipsEff[ibe][iphi_module]->Fill((m_chip[ibe] - 1), 0.0);
					}
				}
			}

			const InDet::TRT_DriftCircle *RawDriftCircle = dynamic_cast<const InDet::TRT_DriftCircle *>(trtCircle->prepRawData());
			bool isTubeHit = (mesb->localCovariance()(Trk::locX, Trk::locX) > 1.0) ? 1 : 0;

			if (RawDriftCircle) {
				m_nTRTHits_side[ibe][iside]++;
				int middleHTbit       = RawDriftCircle->getWord() & 0x00020000;
				//0x00020000 = 0000 0000 0000 0000 0000 0010 0000 0000 0000 0000
				int hitinvaliditygate = RawDriftCircle->getWord() & 0x000DFE80;
				//0x000DFE80 = 0000 0000 0000 0000 0000 1101 1111 1110 1000 0000 //
				bool is_middleHTbit_high = (middleHTbit != 0);
				bool is_anybininVgate_high = (hitinvaliditygate != 0);
				float m_timeOverThreshold = RawDriftCircle->timeOverThreshold();
				double t0 = m_TRTCalDbSvc->getT0(DCoTId, TRTCond::ExpandedIdentifier::STRAW);

				if (m_doExpert && m_doStraws) {
					m_hHitToTonTMapS[ibe][iphi_module]->Fill(m_strawNumber[ibe], m_timeOverThreshold);
					// NOTE: this looks redundant
					m_hHitHonTMapS[ibe][iphi_module]->Fill(m_strawNumber[ibe], 1.0);

					if (is_middleHTbit_high) {
						m_hHitHonTMapS[ibe][iphi_module]->Fill(m_strawNumber[ibe], 1.0);
						m_hHitHWonTMapS[ibe][iphi_module]->Fill(m_strawNumber[ibe], 1.0);
					}
				}

				if (m_doExpert && m_doChips) {
					m_hHitToTonTMapC[ibe][iphi_module]->Fill((m_chip[ibe] - 1), m_timeOverThreshold);

					if (is_middleHTbit_high) {
						m_hHitHWonTMapC[ibe][iphi_module]->Fill(m_chip[ibe] - 1);
						m_hHitHonTMapC[ibe][iphi_module]->Fill(m_chip[ibe] - 1);
					}
				}

				const bool m_driftTimeValid = RawDriftCircle->driftTimeValid();

				if (m_driftTimeValid) {
					const float m_validRawDriftTime = RawDriftCircle->rawDriftTime();

					if (m_doExpert && m_doStraws)
						m_hValidRawDriftTimeonTrk[ibe][iphi_module]->Fill(m_strawNumber[ibe], m_validRawDriftTime);

					if (m_doExpert && m_doChips)
						m_hValidRawDriftTimeonTrkC[ibe][iphi_module]->Fill((m_chip[ibe] - 1), m_validRawDriftTime);
				}

				if (m_doShift && m_doStraws) {
					if (ibe == 0) {
						if (isArgonStraw)
							m_hDriftTimeonTrkDist_B_Ar->Fill(RawDriftCircle->rawDriftTime());
						else
							m_hDriftTimeonTrkDist_B->Fill(RawDriftCircle->rawDriftTime());
					} else if (ibe == 1) {
						if (isArgonStraw)
							m_hDriftTimeonTrkDist_E_Ar[iside]->Fill(RawDriftCircle->rawDriftTime());
						else
							m_hDriftTimeonTrkDist_E[iside]->Fill(RawDriftCircle->rawDriftTime());
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
					const double thist0 = m_TRTCalDbSvc->getT0(surfaceID);
					const double trkdrifttime = rtr->drifttime(fabs(locR));
					const double timeresidual = RawDriftCircle->rawDriftTime() - thist0 - trkdrifttime;

					if (ibe == 0) {
						if (!isTubeHit) {
							m_Pull_Biased_Barrel->Fill(pull_b);
						}

						if (isArgonStraw) {
							m_hResidual_B_Ar->Fill(loc - locR);

							if (cnst_is_pT_over_20GeV) {
								m_hResidual_B_Ar_20GeV->Fill(loc - locR);
							}

							m_hTimeResidual_B_Ar->Fill(timeresidual);
						} else {
							m_hResidual_B->Fill(loc - locR);
							m_hTimeResidual_B->Fill(timeresidual);

							if (cnst_is_pT_over_20GeV) {
								m_hResidual_B_20GeV->Fill(loc - locR);
							}
						}
					} else if (ibe == 1) {
						if (!isTubeHit) {
							m_Pull_Biased_EndCap->Fill(pull_b);
						}

						if (isArgonStraw) {
							m_hResidual_E_Ar[iside]->Fill(loc - locR);
							m_hTimeResidual_E_Ar[iside]->Fill(timeresidual);

							if (cnst_is_pT_over_20GeV) {
								m_hResidual_E_Ar_20GeV[iside]->Fill(loc - locR);
							}
						} else {
							m_hResidual_E[iside]->Fill(loc - locR);
							m_hTimeResidual_E[iside]->Fill(timeresidual);

							if (cnst_is_pT_over_20GeV) {
								m_hResidual_E_20GeV[iside]->Fill(loc - locR);
							}
						}
					}
				}

				if (m_doShift) {
					if (ibe == 0) {
						if (isArgonStraw) {
							m_hWireToTrkPosition_B_Ar->Fill(locR);
						} else {
							m_hWireToTrkPosition_B->Fill(locR);
						}
					} else if (ibe == 1) {
						if (isArgonStraw) {
							m_hWireToTrkPosition_E_Ar[iside]->Fill(locR);
						} else {
							m_hWireToTrkPosition_E[iside]->Fill(locR);
						}
					}
				}

				const float LE = (RawDriftCircle->driftTimeBin()) * 3.125;
				const float EP = timeCor;

				if (m_doShift && m_doStraws) {
					if (ibe == 0) {
						if (isArgonStraw) {
							if (m_isCosmics) {
								m_hrtRelation_B_Ar->Fill(LE - EP - t0, fabs(locR));
							} else {
								m_hrtRelation_B_Ar->Fill(LE - t0, fabs(locR));
							}
						} else {
							if (m_isCosmics) {
								m_hrtRelation_B->Fill(LE - EP - t0, fabs(locR));
							} else {
								m_hrtRelation_B->Fill(LE - t0, fabs(locR));
							}
						}
					} else if (ibe == 1) {
						if (isArgonStraw) {
							if (m_isCosmics) {
								m_hrtRelation_E_Ar[iside]->Fill(LE - EP - t0, fabs(locR));
							} else {
								m_hrtRelation_E_Ar[iside]->Fill(LE - t0, fabs(locR));
							}
						} else {
							if (m_isCosmics) {
								m_hrtRelation_E[iside]->Fill(LE - EP - t0, fabs(locR));
							} else {
								m_hrtRelation_E[iside]->Fill(LE - t0, fabs(locR));
							}
						}
					}
				}

				const int m_driftTimeBin = RawDriftCircle->driftTimeBin();

				if ( (m_driftTimeBin < 24) &&
				     !(RawDriftCircle->lastBinHigh()) &&
				     !(RawDriftCircle->firstBinHigh()) ) {
					if (m_doStraws) {
						if (ibe == 0) {
							m_hHitWonTMap_B->Fill(m_strawNumber[ibe]);
						} else if (ibe == 1) {
							m_hHitWonTMap_E[iside]->Fill(m_strawNumber[ibe]);
						}
					}
				}

				if ((m_driftTimeBin > 2) && (m_driftTimeBin < 17)) {
					if (m_doExpert && m_doStraws)
						m_hHitWonTMapS[ibe][iphi_module]->Fill(m_strawNumber[ibe], 1.0);

					if (m_doExpert && m_doChips)
						m_hHitWonTMapC[ibe][iphi_module]->Fill(m_chip[ibe] - 1, 1.0);
				}

				const int m_trailingEdge = RawDriftCircle->trailingEdge();
				float trailingEdgeScaled = (m_trailingEdge + 1) * 3.125;

				if ((m_trailingEdge < 23) &&
				    !(RawDriftCircle->lastBinHigh()) &&
				    !(RawDriftCircle->firstBinHigh())) {
					if (m_doExpert && m_doStraws)
						m_hHitTronTMapS[ibe][iphi_module]->Fill(m_strawNumber[ibe], trailingEdgeScaled);

					if (m_doExpert && m_doChips)
						m_hHitTronTMapC[ibe][iphi_module]->Fill(m_chip[ibe] - 1, trailingEdgeScaled);

					if (m_doExpert && m_doStraws)
						m_hHitTronTwEPCMapS[ibe][iphi_module]->Fill(m_strawNumber[ibe], trailingEdgeScaled - timeCor);// Mean TE on Track (with Event Phase correction): Straws

					if (m_doExpert && m_doChips)
						m_hHitTronTwEPCMapC[ibe][iphi_module]->Fill((m_chip[ibe] - 1), trailingEdgeScaled - timeCor); // Mean TE on Track (with Event Phase correction): Chips

					if (m_doShift && m_doStraws) {
						if (RawDriftCircle->driftTimeValid()) {
							if (ibe == 0) {
								if (isArgonStraw) {
									m_hTronTDist_B_Ar->Fill(trailingEdgeScaled);
									m_hAvgTroTDetPhi_B_Ar->Fill(m_phi2D[ibe], trailingEdgeScaled);
								} else {
									m_hTronTDist_B->Fill(trailingEdgeScaled);
									m_hAvgTroTDetPhi_B->Fill(m_phi2D[ibe], trailingEdgeScaled);
								}
							} else if (ibe == 1) {
								if (isArgonStraw) {
									m_hTronTDist_E_Ar[iside]->Fill(trailingEdgeScaled);
									m_hAvgTroTDetPhi_E_Ar[iside]->Fill(m_phi2D[ibe], trailingEdgeScaled);
								} else {
									m_hTronTDist_E[iside]->Fill(trailingEdgeScaled);
									m_hAvgTroTDetPhi_E[iside]->Fill(m_phi2D[ibe], trailingEdgeScaled);
								}
							}
						}
					}
				}

				const bool m_firstBinHigh = RawDriftCircle->firstBinHigh();
				const bool m_lastBinHigh = RawDriftCircle->lastBinHigh();

				if (m_firstBinHigh || m_lastBinHigh || m_driftTimeBin > 0 || m_trailingEdge < 23) {
					if (m_doExpert && m_doStraws)
						m_hHitAonTMapS[ibe][iphi_module]->Fill(m_strawNumber[ibe]);

					if (m_doExpert && m_doChips)
						m_hHitAonTMapC[ibe][iphi_module]->Fill(m_chip[ibe] - 1);

					m_nTRTHitsW[ibe][iside]++;

					if (isArgonStraw) {
						m_nTRTHitsW_Ar[ibe][iside]++;
					} else {
						m_nTRTHitsW_Xe[ibe][iside]++;
					}

					m_nTRTHitsW_perwheel[iside][m_layer_or_wheel]++;

					if (is_middleHTbit_high) {
						m_nTRTHLHitsW[ibe][iside]++;

						if (isArgonStraw) {
							m_nTRTHLHitsW_Ar[ibe][iside]++;
						} else {
							m_nTRTHLHitsW_Xe[ibe][iside]++;
						}
					}
				}

				if (is_anybininVgate_high) {
					if (m_doExpert && m_doStraws)
						m_hHitAWonTMapS[ibe][iphi_module]->Fill(m_strawNumber[ibe]);

					if (m_doExpert && m_doChips)
						m_hHitAWonTMapC[ibe][iphi_module]->Fill(m_chip[ibe] - 1);
				}
			}
		}

		//ToDo: work on the part below
		for (int ibe = 0; ibe < 2; ibe++) {
			for (int i = 0; i < 64; i++)
				if (trackfound[ibe][i])
					ntrackstack[ibe][i]++;

			if (m_doShift) {
				if (ibe == 0) {
					if (hitontrack[ibe] >= m_minTRThits)
						m_hNumHoTDetPhi_B->Fill(m_phi2D[ibe], hitontrack[ibe]);
				}

				if (ibe == 1) {
					if (hitontrack_E_side[0] >= m_minTRThits)
						m_hNumHoTDetPhi_E[0]->Fill(m_phi2D[ibe], hitontrack_E_side[0]);

					if (hitontrack_E_side[1] >= m_minTRThits)
						m_hNumHoTDetPhi_E[1]->Fill(m_phi2D[ibe], hitontrack_E_side[1]);
				}
			}

			if (m_phi2D[ibe] < 0) continue;

			if (m_doShift) {
				if (ibe == 0) {
					if (m_nTRTHitsW[ibe][0] + m_nTRTHitsW[ibe][1] > 0)
						m_hNumTrksDetPhi_B->Fill(m_phi2D[ibe]);
				} else if (ibe == 1) {
					if (m_nTRTHitsW[ibe][0] > 0)
						m_hNumTrksDetPhi_E[0]->Fill(m_phi2D[ibe]);

					if (m_nTRTHitsW[ibe][1] > 0)
						m_hNumTrksDetPhi_E[1]->Fill(m_phi2D[ibe]);
				}
			}

			if (m_doShift) {
				if (innerstack[ibe] >= 0 && innerstack[ibe] < s_iStack_max[ibe]) {
					if (ibe == 0) {
						m_LonTrack_B[innerstack[ibe]] += m_nTRTHitsW[ibe][0] + m_nTRTHitsW[ibe][1];

						if (m_nTRTHitsW[ibe][0] + m_nTRTHitsW[ibe][1] > 0) {
							m_HTfraconTrack_B[innerstack[ibe]] += (float)(m_nTRTHLHitsW[ibe][0] + m_nTRTHLHitsW[ibe][1]) / (m_nTRTHitsW[ibe][0] + m_nTRTHitsW[ibe][1]);
							m_nTrack_B[innerstack[ibe]] = m_nTrack_B[innerstack[ibe]] + 1;
						}
					} else if (ibe == 1) {
						m_LonTrack_E[innerstack[ibe]] += m_nTRTHitsW[ibe][0] + m_nTRTHitsW[ibe][1];

						if ((m_nTRTHitsW[ibe][0] + m_nTRTHitsW[ibe][1]) > 0) {
							m_HTfraconTrack_E[innerstack[ibe]] += (float)(m_nTRTHLHitsW[ibe][0] + m_nTRTHLHitsW[ibe][1]) / (m_nTRTHitsW[ibe][0] + m_nTRTHitsW[ibe][1]);
							m_nTrack_E[innerstack[ibe]] = m_nTrack_E[innerstack[ibe]] + 1;
						}
					}
				}
			}

			if (m_doShift) {
				if (ibe == 0) {
					if (isBarrelOnly) {
						if (m_nTRTHitsW[ibe][0] > 0) {
							if (m_nTRTHitsW[ibe][1] > 0) m_hNumSwLLWoT_B->Fill(m_nTRTHitsW[ibe][0] + m_nTRTHitsW[ibe][1]);
							else m_hNumSwLLWoT_B->Fill(m_nTRTHitsW[ibe][0]);
						} else if (m_nTRTHitsW[ibe][1] > 0) {
							m_hNumSwLLWoT_B->Fill(m_nTRTHitsW[ibe][1]);
						}
					}

					if (m_nTRTHLHitsW[ibe][0] > 0) {
						if (m_nTRTHLHitsW[ibe][1] > 0) {
							m_hHLhitOnTrack_B->Fill(m_nTRTHLHitsW[ibe][0] + m_nTRTHLHitsW[ibe][1]);
						} else {
							m_hHLhitOnTrack_B->Fill(m_nTRTHLHitsW[ibe][0]);
						}
					} else if (m_nTRTHLHitsW[ibe][1] > 0) {
						m_hHLhitOnTrack_B->Fill(m_nTRTHLHitsW[ibe][1]);
					}

					if (m_nTRTHitsW[ibe][0] + m_nTRTHitsW[ibe][1] > 0) {
						m_hHtoLRatioOnTrack_B->Fill((float)(m_nTRTHLHitsW[ibe][0] + m_nTRTHLHitsW[ibe][1]) / (m_nTRTHitsW[ibe][0] + m_nTRTHitsW[ibe][1]));
					}

					if (m_nTRTHitsW_Ar[ibe][0] + m_nTRTHitsW_Ar[ibe][1] > 0) {
						m_hHtoLRatioOnTrack_B_Ar->Fill((float)(m_nTRTHLHitsW_Ar[ibe][0] + m_nTRTHLHitsW_Ar[ibe][1]) / (m_nTRTHitsW_Ar[ibe][0] + m_nTRTHitsW_Ar[ibe][1]));
					}

					if (m_nTRTHitsW_Xe[ibe][0] + m_nTRTHitsW_Xe[ibe][1] > 0) {
						m_hHtoLRatioOnTrack_B_Xe->Fill((float)(m_nTRTHLHitsW_Xe[ibe][0] + m_nTRTHLHitsW_Xe[ibe][1]) / (m_nTRTHitsW_Xe[ibe][0] + m_nTRTHitsW_Xe[ibe][1]));
					}
				} else if (ibe == 1) {
					if (m_nTRTHitsW[ibe][0] > 0) {
						if (m_nTRTHitsW[ibe][1] > 0) {
							if (ECAhit && !ECChit && !Bhit) {
								m_hNumSwLLWoT_E[0]->Fill(m_nTRTHitsW[ibe][0]);
							}

							if (ECChit && !ECAhit && !Bhit) {
								m_hNumSwLLWoT_E[1]->Fill(m_nTRTHitsW[ibe][1]);
							}
						}

						if (ECAhit && !ECChit && !Bhit) {
							m_hNumSwLLWoT_E[0]->Fill(m_nTRTHitsW[ibe][0]);
						}
					} else if (m_nTRTHitsW[ibe][1] > 0) {
						if (ECChit && !ECAhit && !Bhit) {
							m_hNumSwLLWoT_E[1]->Fill(m_nTRTHitsW[ibe][1]);
						}
					}

					for (int iside = 0; iside < 2; iside++) {
						if (m_nTRTHLHitsW[ibe][iside] > 0) {
							m_hHLhitOnTrack_E[iside]->Fill(m_nTRTHLHitsW[ibe][iside]);
						}

						if ((m_nTRTHitsW[ibe][iside]) > 0) {
							m_hHtoLRatioOnTrack_E[iside]->Fill((float)(m_nTRTHLHitsW[ibe][iside]) / m_nTRTHitsW[ibe][iside]);

							if ((m_nTRTHitsW_Ar[ibe][iside]) > 0) {
								m_hHtoLRatioOnTrack_E_Ar[iside]->Fill((float)(m_nTRTHLHitsW_Ar[ibe][iside]) / m_nTRTHitsW_Ar[ibe][iside]);
							}

							if ((m_nTRTHitsW_Xe[ibe][iside]) > 0) {
								m_hHtoLRatioOnTrack_E_Xe[iside]->Fill((float)(m_nTRTHLHitsW_Xe[ibe][iside]) / m_nTRTHitsW_Xe[ibe][iside]);
							}
						}
					}
				}
			}

			if (ibe == 0) {
				if ((m_nTRTHitsW[ibe][0] + m_nTRTHitsW[ibe][1]) > 0) {
					nTrksperLB_B++;
				}
				if (comTimeObject) {
					if (m_doShift && (m_phi2D[ibe] > 0) && (std::fabs(timeCor) > 1e-8)) {
						m_hEvtPhaseDetPhi_B->Fill(m_phi2D[ibe], timeCor);
					}
				}
			} else if (ibe == 1) {
				for (int iside = 0; iside < 2; iside++) {
					if (m_nTRTHitsW[ibe][iside] > 0) nTrksperLB_E[iside]++;
					if (comTimeObject) {
						if (m_nTRTHits_side[ibe][iside] > 5 && (std::fabs(timeCor)
						                                        > 1e-8)) {
							if (m_doShift) m_hEvtPhaseDetPhi_E[iside]->Fill(m_phi2D[ibe], timeCor);
						}
					}
				}
			}
		}
	}

	if (comTimeObject) {
		if (std::fabs(timeCor) > 1e-8) {
			if (m_doShift) {
				m_hEvtPhase->Fill(timeCor);
			}

			if (m_doShift) {
				std::vector<int> trigid;
				trigid.clear(); // Trigger ID
				// std::vector<unsigned int> level1TriggerInfo_t = eventInfo.trigger_info()->level1TriggerInfo();
				// get bits for trigger after veto
				std::vector<unsigned int> level1TAV = trigDecision.tav();

				for (unsigned int j = 0; j < 8 && j < level1TAV.size(); ++j) {
					for (unsigned int i = 0; i < 32; ++i) {
						//if ((level1TriggerInfo_t[j] >> i) & 0x1) trigid.push_back(i + (j % 8) * 32); // Found the ID
						if ((level1TAV[j] >> i) & 0x1) {
							trigid.push_back(i + (j % 8) * 32); // Found the ID
						}
					}
				}

				for (unsigned int j = 0; j < trigid.size(); ++j) {
					m_hEvtPhaseVsTrig->Fill(timeCor, trigid[j]);// Event Phase vs. Trigger Item
				}
			}
		}
	}

	for (int ibe = 0; ibe < 2; ibe++) {
		if (m_environment == AthenaMonManager::online && m_totalEvents > 0) {
			//Loop over stack histograms and normalize to number of events processed.
			if (m_doChips && m_doExpert) {
				for (int i = 0; i < 64; i++) {
					divide_LWHist(m_hHitOnTrackVsAllC[ibe][i], m_hHitAonTMapC[ibe][i], m_hHitAMapC[ibe][i]);

					for (int j = 0; j < s_iChip_max[ibe]; j++) {
						if (m_hChipsEff[ibe][i]->GetBinEntries(j + 1) > 0) {
							float effScale = 1. / m_hChipsEff[ibe][i]->GetBinEntries(j + 1);
							int binContent = m_hHitAonTMapC[ibe][i]->GetBinContent(j + 1);
							m_hHitAonTMapC[ibe][i]->SetBinContent(j + 1, binContent * effScale);
							binContent = m_hHitAWonTMapC[ibe][i]->GetBinContent(j + 1);
							m_hHitAWonTMapC[ibe][i]->SetBinContent(j + 1, binContent * effScale);
							binContent = m_hHitHonTMapC[ibe][i]->GetBinContent(j + 1);
							m_hHitHonTMapC[ibe][i]->SetBinContent(j + 1, binContent * effScale);
							binContent = m_hHitHWonTMapC[ibe][i]->GetBinContent(j + 1);
							m_hHitHWonTMapC[ibe][i]->SetBinContent(j + 1, binContent * effScale);
							binContent = m_hHitWonTMapC[ibe][i]->GetBinContent(j + 1);
							m_hHitWonTMapC[ibe][i]->SetBinContent(j + 1, binContent * effScale);
						} else {
							m_hHitAonTMapC[ibe][i]->SetBinContent(j + 1, 0);
							m_hHitHonTMapC[ibe][i]->SetBinContent(j + 1, 0);
							m_hHitWonTMapC[ibe][i]->SetBinContent(j + 1, 0);
							m_hHitAWonTMapC[ibe][i]->SetBinContent(j + 1, 0);
							m_hHitHWonTMapC[ibe][i]->SetBinContent(j + 1, 0);
						}
					}

					divide_LWHist(m_hHtoLMapC[ibe][i], m_hHitHMapC[ibe][i], m_hHitAMapC[ibe][i]);
					divide_LWHist(m_hHtoLonTMapC[ibe][i], m_hHitHonTMapC[ibe][i], m_hHitAonTMapC[ibe][i]);
					divide_LWHist(m_hHtoLWonTMapC[ibe][i], m_hHitHWonTMapC[ibe][i], m_hHitAWonTMapC[ibe][i]);
				}
			}

			if (m_doStraws) {
				for (int i = 0; i < 64; i++) {
					if (m_doExpert) {
						divide_LWHist(m_hHitOnTrackVsAllS[ibe][i], m_hHitAonTMapS[ibe][i], m_hHitAMapS[ibe][i]);
					}

					for (int j = 0; j < s_Straw_max[ibe]; j++) {
						if (i == 0) {
							if (ibe == 0) {
								if (m_nStrawHits_B[j] > 0) {
									m_hHitWonTMap_B->SetBinContent(j + 1, m_hHitWonTMap_B->GetBinContent(j + 1) / m_nStrawHits_B[j]);
								}
							} else if (ibe == 1) {
								if (m_nStrawHits_E[0][j] > 0) {
									m_hHitWonTMap_E[0]->SetBinContent(j + 1, m_hHitWonTMap_E[0]->GetBinContent(j + 1) / m_nStrawHits_E[0][j]);
								}

								if (m_nStrawHits_E[1][j] > 0) {
									m_hHitWonTMap_E[1]->SetBinContent(j + 1, m_hHitWonTMap_E[1]->GetBinContent(j + 1) / m_nStrawHits_E[1][j]);
								}
							}
						}

						if (m_doExpert) {
							if (m_hStrawsEff[ibe][i]->GetBinEntries(j + 1) > 0) {
								float effScale = 1. / m_hStrawsEff[ibe][i]->GetBinEntries(j + 1);
								int binContent = m_hHitAonTMapS[ibe][i]->GetBinContent(j + 1);
								m_hHitAonTMapS[ibe][i]->SetBinContent(j + 1, binContent * effScale);
								binContent = m_hHitAWonTMapS[ibe][i]->GetBinContent(j + 1);
								m_hHitAWonTMapS[ibe][i]->SetBinContent(j + 1, binContent * effScale);
								binContent = m_hHitHonTMapS[ibe][i]->GetBinContent(j + 1);
								m_hHitHonTMapS[ibe][i]->SetBinContent(j + 1, binContent * effScale);
								binContent = m_hHitHWonTMapS[ibe][i]->GetBinContent(j + 1);
								m_hHitHWonTMapS[ibe][i]->SetBinContent(j + 1, binContent * effScale);
								binContent = m_hHitWonTMapS[ibe][i]->GetBinContent(j + 1);
								m_hHitWonTMapS[ibe][i]->SetBinContent(j + 1, binContent * effScale);
							} else {
								m_hHitAWonTMapS[ibe][i]->SetBinContent(j + 1, 0);
								m_hHitAonTMapS[ibe][i]->SetBinContent(j + 1, 0);
								m_hHitHonTMapS[ibe][i]->SetBinContent(j + 1, 0);
								m_hHitHWonTMapS[ibe][i]->SetBinContent(j + 1, 0);
								m_hHitWonTMapS[ibe][i]->SetBinContent(j + 1, 0);
							}
						}
					}

					if (m_doExpert) {
						divide_LWHist(m_hHtoLMapS[ibe][i], m_hHitHMapS[ibe][i], m_hHitAMapS[ibe][i]);
						divide_LWHist(m_hHtoLonTMapS[ibe][i], m_hHitHonTMapS[ibe][i], m_hHitAonTMapS[ibe][i]);
						divide_LWHist(m_hHtoLWonTMapS[ibe][i], m_hHitHWonTMapS[ibe][i], m_hHitAWonTMapS[ibe][i]);
					}
				}
			}

			if (m_doShift) {
				if (ibe == 0) {
					m_hSummary->SetBinContent(2, m_nTotalTracks);
					m_hSummary->SetBinContent(3, m_nTracksB[0]);
					m_hSummary->SetBinContent(4, m_nTracksB[1]);
					m_hSummary->SetBinContent(5, m_nTracksEC[0]);
					m_hSummary->SetBinContent(6, m_nTracksEC[1]);
					m_hSummary->SetBinContent(7, m_nTracksEC_B[0]);
					m_hSummary->SetBinContent(8, m_nTracksEC_B[1]);
					EventPhaseScale = m_hEvtPhase->GetEntries() * 3.125;

					if (EventPhaseScale > 0) {
						scale_LWHist(m_hEvtPhase, 1. / EventPhaseScale);
					}

					DriftTimeonTrkDistScale_B = m_hDriftTimeonTrkDist_B->GetEntries() * 3.125;

					if (DriftTimeonTrkDistScale_B > 0) {
						scale_LWHist(m_hDriftTimeonTrkDist_B, 1. / DriftTimeonTrkDistScale_B);
					}

					HLhitOnTrackScale_B = m_hHLhitOnTrack_B->GetEntries();

					if (HLhitOnTrackScale_B > 0) {
						scale_LWHist(m_hHLhitOnTrack_B, 1. / HLhitOnTrackScale_B);
					}

					HtoLRatioOnTrackScale_B = m_hHtoLRatioOnTrack_B->GetEntries() * 0.02;

					if (HtoLRatioOnTrackScale_B > 0) {
						scale_LWHist(m_hHtoLRatioOnTrack_B, 1. / HtoLRatioOnTrackScale_B);
					}

					NumSwLLWoTScale_B = m_hNumSwLLWoT_B->GetEntries();

					if (NumSwLLWoTScale_B > 0) {
						scale_LWHist(m_hNumSwLLWoT_B, 1. / NumSwLLWoTScale_B);
					}

					WireToTrkPositionScale_B = m_hWireToTrkPosition_B->GetEntries() * 0.1;

					if (WireToTrkPositionScale_B > 0) {
						scale_LWHist(m_hWireToTrkPosition_B, 1. / WireToTrkPositionScale_B);
					}

					TronTDistScale_B = m_hTronTDist_B->GetEntries() * 3.125;

					if (TronTDistScale_B > 0) {
						scale_LWHist(m_hTronTDist_B, 1. / TronTDistScale_B);
					}

					ResidualScale_B = m_hResidual_B->GetEntries() * 0.025;

					if (ResidualScale_B > 0) {
						scale_LWHist(m_hResidual_B, 1. / ResidualScale_B);
					}

					ResidualScale_B_20GeV = m_hResidual_B_20GeV->GetEntries() * 0.025;

					if (ResidualScale_B_20GeV > 0) {
						scale_LWHist(m_hResidual_B_20GeV, 1. / ResidualScale_B_20GeV);
					}

					TimeResidualScale_B = m_hTimeResidual_B->GetEntries() * 0.2;

					if (TimeResidualScale_B > 0) {
						scale_LWHist(m_hTimeResidual_B, 1. / TimeResidualScale_B);
					}

					if (m_ArgonXenonSplitter) {
						DriftTimeonTrkDistScale_B_Ar = m_hDriftTimeonTrkDist_B_Ar->GetEntries() * 3.125;

						if (DriftTimeonTrkDistScale_B_Ar > 0) {
							scale_LWHist(m_hDriftTimeonTrkDist_B_Ar, 1. / DriftTimeonTrkDistScale_B_Ar);
						}

						WireToTrkPositionScale_B_Ar = m_hWireToTrkPosition_B_Ar->GetEntries() * 0.1;

						if (WireToTrkPositionScale_B_Ar > 0) {
							scale_LWHist(m_hWireToTrkPosition_B_Ar, 1. / WireToTrkPositionScale_B_Ar);
						}

						TronTDistScale_B_Ar = m_hTronTDist_B_Ar->GetEntries() * 3.125;

						if (TronTDistScale_B_Ar > 0) {
							scale_LWHist(m_hTronTDist_B_Ar, 1. / TronTDistScale_B_Ar);
						}

						ResidualScale_B_Ar = m_hResidual_B_Ar->GetEntries() * 0.025;

						if (ResidualScale_B_Ar > 0) {
							scale_LWHist(m_hResidual_B_Ar, 1. / ResidualScale_B_Ar);
						}

						ResidualScale_B_Ar_20GeV = m_hResidual_B_Ar_20GeV->GetEntries() * 0.025;

						if (ResidualScale_B_Ar_20GeV > 0) {
							scale_LWHist(m_hResidual_B_Ar_20GeV, 1. / ResidualScale_B_Ar_20GeV);
						}

						TimeResidualScale_B_Ar = m_hTimeResidual_B_Ar->GetEntries() * 0.2;

						if (TimeResidualScale_B_Ar > 0) {
							scale_LWHist(m_hTimeResidual_B_Ar, 1. / TimeResidualScale_B_Ar);
						}
					}
				} else if (ibe == 1) {
					for (int iside = 0; iside < 2; iside++) {
						DriftTimeonTrkDistScale_E[iside] = m_hDriftTimeonTrkDist_E[iside]->GetEntries() * 3.125;

						if (DriftTimeonTrkDistScale_E[iside] > 0) {
							scale_LWHist(m_hDriftTimeonTrkDist_E[iside], 1. / DriftTimeonTrkDistScale_E[iside]);
						}

						HLhitOnTrackScale_E[iside] = m_hHLhitOnTrack_E[iside]->GetEntries();

						if (HLhitOnTrackScale_E[iside] > 0) {
							scale_LWHist(m_hHLhitOnTrack_E[iside], 1. / HLhitOnTrackScale_E[iside]);
						}

						HtoLRatioOnTrackScale_E[iside] = m_hHtoLRatioOnTrack_E[iside]->GetEntries() * 0.02;

						if (HtoLRatioOnTrackScale_E[iside] > 0) {
							scale_LWHist(m_hHtoLRatioOnTrack_E[iside], 1. / HtoLRatioOnTrackScale_E[iside]);
						}

						NumSwLLWoTScale_E[iside] = m_hNumSwLLWoT_E[iside]->GetEntries();

						if (NumSwLLWoTScale_E[iside] > 0) {
							scale_LWHist(m_hNumSwLLWoT_E[iside], 1. / NumSwLLWoTScale_E[iside]);
						}

						WireToTrkPositionScale_E[iside] = m_hWireToTrkPosition_E[iside]->GetEntries() * 0.1;

						if (WireToTrkPositionScale_E[iside] > 0) {
							scale_LWHist(m_hWireToTrkPosition_E[iside], 1. / WireToTrkPositionScale_E[iside]);
						}

						TronTDistScale_E[iside] = m_hTronTDist_E[iside]->GetEntries() * 3.125;

						if (TronTDistScale_E[iside] > 0) {
							scale_LWHist(m_hTronTDist_E[iside], 1. / TronTDistScale_E[iside]);
						}

						ResidualScale_E[iside] = m_hResidual_E[iside]->GetEntries() * 0.025;

						if (ResidualScale_E[iside] > 0) {
							scale_LWHist(m_hResidual_E[iside], 1. / ResidualScale_E[iside]);
						}

						ResidualScale_E_20GeV[iside] = m_hResidual_E_20GeV[iside]->GetEntries() * 0.025;

						if (ResidualScale_E_20GeV[iside] > 0) {
							scale_LWHist(m_hResidual_E_20GeV[iside], 1. / ResidualScale_E_20GeV[iside]);
						}

						TimeResidualScale_E[iside] = m_hTimeResidual_E[iside]->GetEntries() * 0.2;

						if (TimeResidualScale_E[iside] > 0) {
							scale_LWHist(m_hTimeResidual_E[iside], 1. / TimeResidualScale_E[iside]);
						}

						if (m_ArgonXenonSplitter) {
							DriftTimeonTrkDistScale_E_Ar[iside] = m_hDriftTimeonTrkDist_E_Ar[iside]->GetEntries() * 3.125;

							if (DriftTimeonTrkDistScale_E_Ar[iside] > 0) {
								scale_LWHist(m_hDriftTimeonTrkDist_E_Ar[iside], 1. / DriftTimeonTrkDistScale_E_Ar[iside]);
							}

							WireToTrkPositionScale_E_Ar[iside] = m_hWireToTrkPosition_E_Ar[iside]->GetEntries() * 0.1;

							if (WireToTrkPositionScale_E_Ar[iside] > 0) {
								scale_LWHist(m_hWireToTrkPosition_E_Ar[iside], 1. / WireToTrkPositionScale_E_Ar[iside]);
							}

							TronTDistScale_E_Ar[iside] = m_hTronTDist_E_Ar[iside]->GetEntries() * 3.125;

							if (TronTDistScale_E_Ar[iside] > 0) {
								scale_LWHist(m_hTronTDist_E_Ar[iside], 1. / TronTDistScale_E_Ar[iside]);
							}

							ResidualScale_E_Ar[iside] = m_hResidual_E_Ar[iside]->GetEntries() * 0.025;

							if (ResidualScale_E_Ar[iside] > 0) {
								scale_LWHist(m_hResidual_E_Ar[iside], 1. / ResidualScale_E_Ar[iside]);
							}

							ResidualScale_E_Ar_20GeV[iside] = m_hResidual_E_Ar_20GeV[iside]->GetEntries() * 0.025;

							if (ResidualScale_E_Ar_20GeV[iside] > 0) {
								scale_LWHist(m_hResidual_E_Ar_20GeV[iside], 1. / ResidualScale_E_Ar_20GeV[iside]);
							}

							TimeResidualScale_E_Ar[iside] = m_hTimeResidual_E_Ar[iside]->GetEntries() * 0.2;

							if (TimeResidualScale_E_Ar[iside] > 0) {
								scale_LWHist(m_hTimeResidual_E_Ar[iside], 1. / TimeResidualScale_E_Ar[iside]);
							}
						}
					}
				}
			}
		}
	}

	return StatusCode::SUCCESS;
}

//Fill the TRT Eff histograms
//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::fillTRTEfficiency(const TrackCollection& combTrackCollection) {
//----------------------------------------------------------------------------------//
	ATH_MSG_VERBOSE("Filling TRT Eff Histos");
	// reduce unnecessary divisions
	const float invGeV = 1. / CLHEP::GeV;
	const float invmm = 1. / CLHEP::mm;
	int itrack = 0;

	for (auto track = combTrackCollection.begin(); track != combTrackCollection.end(); ++track) {
		// online: use all tracks, offline: use only every xth track, skip the rest
		if (m_environment != AthenaMonManager::online && (itrack % m_every_xth_track) != 0) continue;

		++itrack;
		// get perigee
		const Trk::Perigee *perigee = (*track)->perigeeParameters();

		if (perigee) {
			m_track_pt  = perigee->pT();
			m_track_eta = perigee->eta();
			m_track_phi = perigee->parameters()[Trk::phi0];
			m_track_d0  = perigee->parameters()[Trk::d0];
			m_track_z0  = perigee->parameters()[Trk::z0];
			ATH_MSG_DEBUG("This track has perigee parameters:\n"
			              << "                 pT     = " << m_track_pt * invGeV  << " GeV" << "\n"
			              << "                 eta =    " << m_track_eta << "\n"
			              << "                 phi0 =   " << m_track_phi << "\n"
			              << "                 d0 =     " << m_track_d0 * invmm << "\n"
			              << "                 z0 =     " << m_track_z0 * invmm << "\n"
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

		const std::auto_ptr<const Trk::TrackSummary> summary(m_TrackSummaryTool->createSummary(*(*track)));
		int n_trt_hits = summary->get(Trk::numberOfTRTHits);
		int n_sct_hits = summary->get(Trk::numberOfSCTHits);
		int n_pixel_hits = summary->get(Trk::numberOfPixelHits);
		float m_p = 1.0e+08;

		if (perigee) {
			m_p = (perigee->parameters()[Trk::qOverP] != 0.) ? fabs(1. / (perigee->parameters()[Trk::qOverP])) : 1.0e+08;
		}

		float min_pt_new = m_min_pT;

		if (m_isCosmics == false) {
			min_pt_new = 2.0 * CLHEP::GeV;
		}

		// preselect tracks
		const bool passed_track_preselection =
			(fabs(perigee->parameters()[Trk::d0]) < m_max_abs_d0) &&
			(fabs(perigee->parameters()[Trk::z0]) < m_max_abs_z0) &&
			(perigee->pT() > min_pt_new) &&
			(m_p > m_minP) &&
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
			int m_barrel_ec       = m_pTRTHelper->barrel_ec(id);
			int m_layer_or_wheel  = m_pTRTHelper->layer_or_wheel(id);
			int m_phi_module      = m_pTRTHelper->phi_module(id);
			int m_straw_layer     = m_pTRTHelper->straw_layer(id);
			int m_straw           = m_pTRTHelper->straw(id);
			const bool isArgonStraw = (Straw_Gastype( m_sumSvc->getStatusHT(id) ) == GasType::Ar);
			// assume always Xe if m_ArgonXenonSplitter is not enabled, otherwise check the straw status (good is Xe, non-good is Ar)
			// ibe = 0 (Barrel), ibe = 1 (Endcap)
			int ibe   = abs(m_barrel_ec) - 1;
			// iside = 0 (Side A), iside = 1 (Side C)
			int iside = m_barrel_ec > 0 ? 0 : 1;

			if (ibe == 0) {
				if (isArgonStraw) {
					m_hefficiencyBarrel_locR_Ar->Fill(locR, 1.0);
				} else {
					m_hefficiencyBarrel_locR->Fill(locR, 1.0);
				}
			} else if (ibe == 1) {
				if (isArgonStraw) {
					m_hefficiencyEndCap_locR_Ar[iside]->Fill(locR, 1.0);
				} else {
					m_hefficiencyEndCap_locR[iside]->Fill(locR, 1.0);
				}
			}

			if (fabs(locR) >= 1.3) continue;

			int m_strawNumber = 0;
			int m_chip = 0;

			if (ibe == 0) {
				m_strawNumber = strawNumber(m_straw, m_straw_layer, m_layer_or_wheel);

				if (m_strawNumber >= 0 && m_strawNumber < s_Straw_max[ibe])
					m_chip = mat_chip_B[m_phi_module][m_strawNumber];
			} else if (ibe == 1) {
				m_strawNumber = strawNumberEndCap(m_straw, m_straw_layer, m_layer_or_wheel, m_phi_module, m_barrel_ec);

				if (m_strawNumber >= 0 && m_strawNumber < s_Straw_max[ibe])
					m_chip = mat_chip_E[m_phi_module][m_strawNumber];
			}

			m_hefficiencyMap[ibe]->Fill(m_strawNumber, 1.0);

			if (m_doExpert) {
				if (iside == 0) {
					m_hefficiencyS[ibe][m_phi_module]->Fill(m_strawNumber, 1.0);
					m_hefficiencyC[ibe][m_phi_module]->Fill(m_chip, 1.0);
				} else if (iside == 1) {
					m_hefficiencyS[ibe][m_phi_module + 32]->Fill(m_strawNumber, 1.0);
					m_hefficiencyC[ibe][m_phi_module + 32]->Fill(m_chip, 1.0);
				}
			}

			m_hefficiency_eta->Fill(m_track_eta, 1.0);
			m_hefficiency_phi->Fill(m_track_phi, 1.0);
			m_hefficiency_pt->Fill(m_track_pt * invGeV, 1.0);
			m_hefficiency_z0->Fill(m_track_z0, 1.0);
		}

		//use hole finder to find holes on this track !

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
					int m_barrel_ec = m_pTRTHelper->barrel_ec(id);
					int m_layer_or_wheel = m_pTRTHelper->layer_or_wheel(id);
					int m_phi_module = m_pTRTHelper->phi_module(id);
					int m_straw_layer = m_pTRTHelper->straw_layer(id);
					int m_straw = m_pTRTHelper->straw(id);
					const bool isArgonStraw = Straw_Gastype( m_sumSvc->getStatusHT(id) ) == GasType::Ar;
					// assume always Xe if m_ArgonXenonSplitter is not enabled, otherwise check the straw status (good is Xe, non-good is Ar)
					// ibe = 0 (Barrel), ibe = 1 (Endcap)
					int ibe = abs(m_barrel_ec) - 1;
					// iside = 0 (Side A), iside = 1 (Side C)
					int iside = m_barrel_ec > 0 ? 0 : 1;

					if (ibe == 0) {
						if (isArgonStraw) {
							m_hefficiencyBarrel_locR_Ar->Fill(locR, 0.0);
						} else {
							m_hefficiencyBarrel_locR->Fill(locR, 0.0);
						}
					} else if (ibe == 1) {
						if (isArgonStraw) {
							m_hefficiencyEndCap_locR_Ar[iside]->Fill(locR, 0.0);
						} else {
							m_hefficiencyEndCap_locR[iside]->Fill(locR, 0.0);
						}
					}

					if (fabs(locR) >= 1.3) continue;

					int m_strawNumber = 0;
					int m_chip = 0;

					if (ibe == 0) {
						m_strawNumber = strawNumber(m_straw, m_straw_layer, m_layer_or_wheel);

						if (m_strawNumber >= 0 && m_strawNumber < s_Straw_max[ibe]) {
							m_chip = mat_chip_B[m_phi_module][m_strawNumber];
						}
					} else if (ibe == 1) {
						m_strawNumber = strawNumberEndCap(m_straw, m_straw_layer, m_layer_or_wheel, m_phi_module, m_barrel_ec);

						if (m_strawNumber >= 0 && m_strawNumber < s_Straw_max[ibe]) {
							m_chip = mat_chip_E[m_phi_module][m_strawNumber];
						}
					}

					m_hefficiencyMap[ibe]->Fill(m_strawNumber, 0.0);

					if (m_doExpert) {
						if (iside == 0) {
							m_hefficiencyS[ibe][m_phi_module]->Fill(m_strawNumber, 0.0);
							m_hefficiencyC[ibe][m_phi_module]->Fill(m_chip, 0.0);
						} else if (iside == 1) {
							m_hefficiencyS[ibe][m_phi_module + 32]->Fill(m_strawNumber, 0.0);
							m_hefficiencyC[ibe][m_phi_module + 32]->Fill(m_chip, 0.0);
						}
					}

					m_hefficiency_eta->Fill(m_track_eta, 0.0);
					m_hefficiency_phi->Fill(m_track_phi, 0.0);
					m_hefficiency_pt->Fill(m_track_pt * invGeV, 0.0);
					m_hefficiency_z0->Fill(m_track_z0, 0.0);
				}

				delete holes;
			}
		}
	}

	double n_BorE[2][2];
	double total_BorE[2][2];

	for (int ibe = 0; ibe < 2; ibe++) {
		for (int iside = 0; iside < 2; iside++) {
			m_hefficiency[ibe][iside]->Reset();
			m_hefficiencyIntegral[ibe][iside]->Reset();

			for (int i = 0; i < 32; i++) {
				for (int ibin = 0; ibin <= s_Straw_max[ibe]; ibin++) {
					if (m_doExpert) {
						if (m_hefficiencyS[ibe][i + (32 * iside)]->GetBinEntries(ibin) > m_min_tracks_straw)
							m_hefficiency[ibe][iside]->Fill(m_hefficiencyS[ibe][i + (32 * iside)]->GetBinContent(ibin));
					}
				}
			}

			n_BorE[ibe][iside] = m_hefficiency[ibe][iside]->GetEntries();
			total_BorE[ibe][iside] = 0.0;

			for (UInt_t ibin = 0; ibin <= m_hefficiency[ibe][iside]->GetXaxis()->GetNbins(); ibin++) {
				total_BorE[ibe][iside] += m_hefficiency[ibe][iside]->GetBinContent(ibin);
				m_hefficiencyIntegral[ibe][iside]->SetBinContent(ibin, n_BorE[ibe][iside] != 0.0 ? total_BorE[ibe][iside] / n_BorE[ibe][iside] : 0.0);
			}
		}
	}

	return StatusCode::SUCCESS;
}


int maxtimestamp = 0.;
//----------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::fillTRTHighThreshold(const TrackCollection& trackCollection,
                                                     const xAOD::EventInfo& eventInfo) {
//----------------------------------------------------------------------------------//
	ATH_MSG_VERBOSE("");
	DataVector<Trk::Track>::const_iterator p_trk;
	const Trk::Perigee *perigee = NULL;
	const DataVector<const Trk::TrackParameters> *AllTrkPar(0);
	DataVector<const Trk::TrackParameters>::const_iterator p_trkpariter;
	int lumiBlockNumber;
	int timeStamp;
	lumiBlockNumber = eventInfo.lumiBlock();
	timeStamp = eventInfo.timeStamp();

	if (timeStamp > maxtimestamp) {
		maxtimestamp = timeStamp;
	}

	int runNumber;
	runNumber = eventInfo.runNumber();
	// get Online Luminosity
	double intLum = (m_lumiTool->lbDuration() * m_lumiTool->lbAverageLuminosity());
	double timeStampAverage = (maxtimestamp - 0.5 * m_lumiTool->lbDuration());
	m_IntLum->SetBinContent(1, intLum);
	m_LBvsLum->SetBinContent(lumiBlockNumber, intLum);
	m_LBvsTime->SetBinContent(lumiBlockNumber, timeStampAverage);

	for (p_trk = trackCollection.begin(); p_trk != trackCollection.end(); ++p_trk) {
		AllTrkPar = (*p_trk)->trackParameters();

		for (p_trkpariter = AllTrkPar->begin(); p_trkpariter != AllTrkPar->end(); ++p_trkpariter) {
			if ((perigee = dynamic_cast<const Trk::Perigee *>(*p_trkpariter))) break;
		}

		//if you went through all of the track parameters and found no perigee mearsurement
		//then something is wrong with the track and so don't use the track.
		//i.e. continue to the next track.
		if (!perigee) continue;

		float track_eta  = perigee->eta();
		float track_p    = (perigee->parameters()[Trk::qOverP] != 0.) ? fabs(1. / (perigee->parameters()[Trk::qOverP])) : 10e7;
		const DataVector<const Trk::TrackStateOnSurface> *trackStates = (**p_trk).trackStateOnSurfaces();

		if (trackStates == 0) continue;

		DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItBegin     = trackStates->begin();
		DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItEnd       = trackStates->end();
		const std::auto_ptr<const Trk::TrackSummary> summary(m_TrackSummaryTool->createSummary(*(*p_trk)));
		int trt_hits = summary->get(Trk::numberOfTRTHits);
		int sct_hits = summary->get(Trk::numberOfSCTHits);
		int pixel_hits = summary->get(Trk::numberOfPixelHits);
		bool passCuts = true;

		if (fabs(track_eta) > 2.5) continue;
		if (fabs(track_p) < 5000.) continue;
		if (pixel_hits < 1.) continue;
		if (sct_hits < 6.) continue;
		if (trt_hits < 6.) continue;

		if (!passCuts) continue;

		//Now we have hit informations
		const DataVector<const Trk::TrackStateOnSurface> *track_states = (*p_trk)->trackStateOnSurfaces();

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
			//Ba_Ec:  0 is barrel 1 is Endcap
			//Side :  0 is side_A 1 is side_C
			int Ba_Ec = abs(barrel_ec_side) - 1;
			int Side  = barrel_ec_side > 0 ? 0 : 1;
			double xPos = trtCircle->globalPosition().x(); // global x coordinate
			double yPos = trtCircle->globalPosition().y(); // global y coordinate
			double zPos = trtCircle->globalPosition().z(); // global z coordinate
			double RPos = sqrt(xPos * xPos + yPos * yPos);
			Identifier surfaceID;
			surfaceID = trtCircle->identify();
			// assume always Xe if m_ArgonXenonSplitter is not enabled, otherwise check the straw status (good is Xe, non-good is Ar)
			const InDet::TRT_DriftCircle *RawDriftCircle = dynamic_cast<const InDet::TRT_DriftCircle *>(trtCircle->prepRawData());

			if (!RawDriftCircle) { //coverity 25097
				//This shouldn't happen in normal conditions  because trtCircle is a TRT_DriftCircleOnTrack object
				ATH_MSG_WARNING("RawDriftCircle object returned null");
				continue;
			}

			int middleHTbit       = RawDriftCircle->getWord() & 0x00020000;
			//0x00020000 = 0000 0000 0000 0000 0000 0010 0000 0000 0000 0000
			bool is_middleHTbit_high   = (middleHTbit != 0);
			//bool isHighLevel= RawDriftCircle->highLevel();
			bool isHighLevel = is_middleHTbit_high; //Hardcoded HT Middle Bit
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

			//Fill Barrel Plots
			if ((!shortStraw) && (Ba_Ec == 0)) {
				m_trackz_All[layer_or_wheel][InputBar]->Fill(zPos);
				if (isHighLevel)
					m_trackz_HT[layer_or_wheel][InputBar]->Fill(zPos);
			}

			if (shortStraw) {
				if (zPos > 0.) {
					m_trackz_All[3][InputBar]->Fill(zPos);
					if (isHighLevel)
						m_trackz_HT[3][InputBar]->Fill(zPos);
				} else {
					m_trackz_All[4][InputBar]->Fill(zPos);

					if (isHighLevel)m_trackz_HT[4][InputBar]->Fill(zPos);
				}
			}

			//End of Barrel Plots, begin EC plots
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

			if (WType < 0 && Ba_Ec == 1) { //Coverity CID 25096
				ATH_MSG_WARNING("The variable  \"WType\" is less than zero!.");
				continue;
			}

			if (Ba_Ec == 1) {
				m_trackr_All[WType][Side]->Fill(RPos);
				if (isHighLevel)m_trackr_HT[WType][Side]->Fill(RPos);
			}
		}
	}

	return StatusCode::SUCCESS;
}


//-------------------------------------------------------------------------------------------------//
StatusCode TRT_Monitoring_Tool::checkTRTReadoutIntegrity(const xAOD::EventInfo& eventInfo) {
//-------------------------------------------------------------------------------------------------//
	StatusCode sc = StatusCode::SUCCESS;

	const unsigned int m_lumiBlock = eventInfo.lumiBlock();
	ATH_MSG_VERBOSE("This is lumiblock : " << m_lumiBlock);
	m_good_bcid = eventInfo.bcid();

	if ((int)m_lumiBlock != m_lastLumiBlock) {
		m_lastLumiBlock = m_lumiBlock;
	}

	//Get BSConversion Errors from BSConditionsServices:
	std::set<std::pair<uint32_t, uint32_t> > *L1IDErrorSet      = m_BSSvc->getIdErrorSet(TRTByteStreamErrors::L1IDError);
	std::set<std::pair<uint32_t, uint32_t> > *BCIDErrorSet      = m_BSSvc->getIdErrorSet(TRTByteStreamErrors::BCIDError);
	std::set<uint32_t>                       *MissingErrorSet   = m_BSSvc->getErrorSet(TRTByteStreamErrors::MISSINGError);
	std::set<uint32_t>                       *SidErrorSet       = m_BSSvc->getErrorSet(TRTByteStreamErrors::SIDError);
	std::set<std::pair<uint32_t, uint32_t> > *RobStatusErrorSet = m_BSSvc->getRodRobErrorSet(TRTByteStreamErrors::RobStatusError);
	const unsigned int rod_id_base[2][2] = { { 0x310000, 0x320000 }, { 0x330000, 0x340000 } };
	const unsigned int nChipsTotal[2][2] = { {     3328,     3328 }, {     7680,     7680 } };
	const unsigned int nRobsTotal[2][2]  = { {       32,       32 }, {       64,       64 } };
	float m_nBSErrors[2][2]  = { { 0, 0 }, { 0, 0 } };
	float m_nRobErrors[2][2] = { { 0, 0 }, { 0, 0 } };
	const std::set<std::pair<uint32_t, uint32_t> > *errorset1[2] = { BCIDErrorSet, L1IDErrorSet };

	for (int iset = 0; iset < 2; ++iset) {
		for (auto setIt = errorset1[iset]->begin(); setIt != errorset1[iset]->end(); ++setIt) {
			for (int ibe = 0; ibe < 2; ++ibe) {
				for (int iside = 0; iside < 2; ++iside) {
					if (((setIt->first >> 8) & 0xFF0000) == rod_id_base[ibe][iside]) {
						m_nBSErrors[ibe][iside] += 1. / nChipsTotal[ibe][iside];
					}
				}
			}
		}
	}

	const std::set<uint32_t> *errorset2[2] = { MissingErrorSet, SidErrorSet };

	for (int iset = 0; iset < 2; ++iset) {
		for (auto setIt = errorset2[iset]->begin(); setIt != errorset2[iset]->end(); ++setIt) {
			for (int ibe = 0; ibe < 2; ++ibe) {
				for (int iside = 0; iside < 2; ++iside) {
					if (((*setIt >> 8) & 0xFF0000) == rod_id_base[ibe][iside]) {
						m_nBSErrors[ibe][iside] += 1. / nChipsTotal[ibe][iside];
					}
				}
			}
		}
	}

	for (int ibe = 0; ibe < 2; ++ibe) {
		for (int iside = 0; iside < 2; ++iside) {
			m_hChipBSErrorsVsLB[ibe][iside]->Fill(m_lumiBlock, m_nBSErrors[ibe][iside]);
			m_hChipBSErrorsVsLB[ibe][iside]->SetEntries(m_lumiBlock); // we need this so the LastBinThreshold algorithm can find the last bin
		}
	}

	for (auto setIt = RobStatusErrorSet->begin(); setIt != RobStatusErrorSet->end(); ++setIt) {
		for (int ibe = 0; ibe < 2; ++ibe) {
			for (int iside = 0; iside < 2; ++iside) {
				if (setIt->first % rod_id_base[ibe][iside] < 0xffff) {
					m_nRobErrors[ibe][iside] += 1. / nRobsTotal[ibe][iside];
				}
			}
		}
	}

	for (int ibe = 0; ibe < 2; ++ibe) {
		for (int iside = 0; iside < 2; ++iside) {
			m_hRobBSErrorsVsLB[ibe][iside]->Fill(m_lumiBlock, m_nRobErrors[ibe][iside]);
			m_hRobBSErrorsVsLB[ibe][iside]->SetEntries(m_lumiBlock); // we need this so the LastBinThreshold algorithm can find the last bin
		}
	}

	return sc;
}

//----------------------------------------------------------------------------------//
int TRT_Monitoring_Tool::strawNumber(int strawNumber, int strawlayerNumber, int LayerNumber) {
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
int TRT_Monitoring_Tool::strawNumber_reverse (int inp_strawnumber,  int *strawNumber, int *strawlayerNumber, int *LayerNumber) {
//----------------------------------------------------------------------------------//
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
	//ToDo check inp_strawnumber
	int i = 1;

	for (i = 1; inp_strawnumber >= 0; i++) {
		inp_strawnumber -= numberOfStraws[i];
	}

	i -= 2;
	strawLayerNumber_reverse(i, strawlayerNumber, LayerNumber);
	*strawNumber = -inp_strawnumber - 1;
	return 0;
}

//----------------------------------------------------------------------------------//
int TRT_Monitoring_Tool::strawNumberEndCap(int strawNumber, int strawLayerNumber, int LayerNumber, int phi_stack, int side) {
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
int TRT_Monitoring_Tool::strawLayerNumber(int strawLayerNumber, int LayerNumber) {
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
int TRT_Monitoring_Tool::strawLayerNumber_reverse(int strawLayerNumInp, int *strawLayerNumber, int *LayerNumber) {
//----------------------------------------------------------------------------------//
	//Danger? There are no checks on input
	//use with care
	if (strawLayerNumInp < 19) {
		*strawLayerNumber = strawLayerNumInp;
		*LayerNumber = 0;
	} else if (strawLayerNumInp < 43) {
		*strawLayerNumber = strawLayerNumInp - 19;
		*LayerNumber = 1;
	} else {
		*strawLayerNumber = strawLayerNumInp - 43;
		*LayerNumber = 2;
	}

	return 0;
}

//----------------------------------------------------------------------------------//
float TRT_Monitoring_Tool::radToDegrees(float radValue) {
//----------------------------------------------------------------------------------//
	float degreeValue = radValue / M_PI * 180;

	if (degreeValue < 0) degreeValue += 360;

	return degreeValue;
}

//----------------------------------------------------------------------------------//
int TRT_Monitoring_Tool::chipToBoard(int chip) {
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
int TRT_Monitoring_Tool::chipToBoard_EndCap(int chip) {
//----------------------------------------------------------------------------------//
	const int remainder = (chip - 1) % 12;
	const int Board = int(((chip - 1) - remainder) / 12);
	return Board + 1;
}

// Code to normalize a LW histogram. For now does it only of TH1F_LW.
// Return whether we could do it (ie integral was non-zero)
//----------------------------------------------------------------------------------//
void TRT_Monitoring_Tool::scale_LWHist(LWHist1D *hist, float scale) {
//----------------------------------------------------------------------------------//
	if (!hist) return;

	const unsigned int entries = hist->GetEntries();
	unsigned int index;
	double content, error;
	hist->resetActiveBinLoop();

	while (hist->getNextActiveBin(index, content, error)) {
		hist->SetBinContentAndError(index, scale * content, scale * error);
	}

	hist->SetEntries(entries);
}

//function for scaling a histogram with an vector
// checks that number of bins matches vector length
//----------------------------------------------------------------------------------//
void TRT_Monitoring_Tool::scale_LWHistWithScaleVector(LWHist1D *hist, const vector<float> &scale) {
//----------------------------------------------------------------------------------//
	if (!hist) return;

	if (hist->GetNbinsX() != scale.size()) return; // Could add an error message here

	const unsigned int entries = hist->GetEntries();
	unsigned int index;
	double content, error;
	hist->resetActiveBinLoop();

	while (hist->getNextActiveBin(index, content, error)) {
		// histogram bins run from 1 to n while array runs from 0 ro n-1
		try {
			hist->SetBinContentAndError(index, scale.at(index - 1) * content, scale.at(index - 1) * error);
		} catch (out_of_range &e) {
			ATH_MSG_ERROR("Index " << index << " out of range in scale_LWHistWithScaleVector");
		}
	}

	hist->SetEntries(entries);
}

// code to divide one histogram by another and save result
// we could add coefficients to this, but we're not going to
// we might also want to check that the bin ranges are the same, but for now
// all we check is that we have the same number of bins!
// Also, for now we don't care (I don't think) about the errors, so don't bother with them...
//----------------------------------------------------------------------------------//
void TRT_Monitoring_Tool::divide_LWHist(TH1F_LW *result, TH1F_LW *a, TH1F_LW *b) {
//----------------------------------------------------------------------------------//
	if (!result || !a || !b) return;

	if (result->GetXaxis()->GetNbins() != a->GetXaxis()->GetNbins() || b->GetXaxis()->GetNbins() != a->GetXaxis()->GetNbins()) return;

	result->Reset();
	unsigned index;
	double contentA, errorA;
	double contentB, errorB;
	a->resetActiveBinLoop();

	while (a->getNextActiveBin(index, contentA, errorA)) {
		b->GetBinContentAndError(index, contentB, errorB);

		if (contentB == 0)
			result->SetBinContentAndError(index, 0, 0);
		else
			result->SetBinContentAndError(index, contentA / contentB, 0);
	}

	result->SetEntries(a->GetEntries());
}

//----------------------------------------------------------------------------------//
TH1F_LW *TRT_Monitoring_Tool::bookTH1F_LW(MonGroup &mongroup, const std::string &hName, const std::string &hTitle, int nbins, double firstbin, double lastbin, const std::string &xTitle, const std::string &yTitle, StatusCode &scode) {
//----------------------------------------------------------------------------------//
	TH1F_LW *hist = TH1F_LW::create(hName.c_str(), hTitle.c_str(), nbins, firstbin, lastbin);
	scode = trtRegHist<TH1F_LW>(hist, mongroup, hName.c_str());
	hist->GetXaxis()->SetLabelSize(0.03);
	hist->GetYaxis()->SetLabelSize(0.03);
	hist->SetXTitle(xTitle.c_str());
	hist->SetYTitle(yTitle.c_str());
	return hist;
}

//Bens addition
//----------------------------------------------------------------------------------//
TH1D_LW *TRT_Monitoring_Tool::bookTH1D_LW(MonGroup &mongroup, const std::string &hName, const std::string &hTitle, int nbins, double firstbin, double lastbin, const std::string &xTitle, const std::string &yTitle, StatusCode &scode) {
//----------------------------------------------------------------------------------//
	TH1D_LW *hist = TH1D_LW::create(hName.c_str(), hTitle.c_str(), nbins, firstbin, lastbin);
	scode = trtRegHist<TH1D_LW>(hist, mongroup, hName.c_str());
	hist->GetXaxis()->SetLabelSize(0.03);
	hist->GetYaxis()->SetLabelSize(0.03);
	hist->SetXTitle(xTitle.c_str());
	hist->SetYTitle(yTitle.c_str());
	return hist;
}

//TProfile can be rebinned (unlike TProfile_LW) so we still need it.
//----------------------------------------------------------------------------------//
TProfile *TRT_Monitoring_Tool::bookTProfile(MonGroup &mongroup, const std::string &hName, const std::string &hTitle, int nbins, double firstbin, double lastbin, double ymin, double ymax, const std::string &xTitle, const std::string &yTitle, StatusCode &scode) {
//----------------------------------------------------------------------------------//
	TProfile *hist = new TProfile(hName.c_str(), hTitle.c_str(), nbins, firstbin, lastbin, ymin, ymax);
	scode = trtRegHist<TProfile>(hist, mongroup, hName.c_str());
	hist->SetMarkerSize(0.5);
	hist->SetMarkerStyle(2);
	hist->SetMarkerColor(2);
	hist->GetXaxis()->SetLabelSize(0.03);
	hist->GetYaxis()->SetLabelSize(0.03);
	hist->GetXaxis()->SetTitle(xTitle.c_str());
	hist->GetYaxis()->SetTitle(yTitle.c_str());
	return hist;
}

//----------------------------------------------------------------------------------//
TProfile_LW *TRT_Monitoring_Tool::bookTProfile_LW(MonGroup &mongroup, const std::string &hName, const std::string &hTitle, int nbins, double firstbin, double lastbin, double ymin, double ymax, const std::string &xTitle, const std::string &yTitle, StatusCode &scode) {
//----------------------------------------------------------------------------------//
	TProfile_LW *hist = TProfile_LW::create(hName.c_str(), hTitle.c_str(), nbins, firstbin, lastbin, ymin, ymax);
	scode = trtRegHist<TProfile_LW>(hist, mongroup, hName.c_str());
	hist->SetMarkerSize(0.5);
	hist->SetMarkerStyle(2);
	hist->SetMarkerColor(2);
	hist->GetXaxis()->SetLabelSize(0.03);
	hist->GetYaxis()->SetLabelSize(0.03);
	hist->GetXaxis()->SetTitle(xTitle.c_str());
	hist->GetYaxis()->SetTitle(yTitle.c_str());
	return hist;
}

//----------------------------------------------------------------------------------//
TH2F_LW *TRT_Monitoring_Tool::bookTH2F_LW(MonGroup &mongroup, const std::string &hName, const std::string &hTitle, int xnbins, double xfirstbin, double xlastbin, int ynbins, double yfirstbin, double ylastbin, const std::string &xTitle, const std::string &yTitle, StatusCode &scode) {
//----------------------------------------------------------------------------------//
	TH2F_LW *hist = TH2F_LW::create(hName.c_str(), hTitle.c_str(), xnbins, xfirstbin, xlastbin, ynbins, yfirstbin, ylastbin);
	scode = trtRegHist<TH2F_LW>(hist, mongroup, hName.c_str());
	hist->GetXaxis()->SetLabelSize(0.03);
	hist->GetYaxis()->SetLabelSize(0.03);
	hist->SetXTitle(xTitle.c_str());
	hist->SetYTitle(yTitle.c_str());
	return hist;
}


//----------------------------------------------------------------------------------//
int TRT_Monitoring_Tool::m_initScaleVectors() {
//----------------------------------------------------------------------------------//
	if (m_flagforscale == 0 ) return 0;

	m_scale_hHitWMap_B.clear();
	m_scale_hHitWMap_B_Ar.clear();

	for (int i = 0; i < s_Straw_max[0]; i++) {
		float countAr = 0;
		float countXe = 0;
		int sN, sLN, lN;
		strawNumber_reverse(i, &sN, &sLN, &lN);

		for (int side = -1 ; side < 2; side += 2 ) {
			for (int j = 0; j < 32; j++ ) {
				Identifier Dummy_Identifier;
				Dummy_Identifier = m_pTRTHelper->straw_id(side, j, lN, sLN, sN);
				bool isArgonStraw = (Straw_Gastype( m_sumSvc->getStatusHT(Dummy_Identifier) ) == GasType::Ar); 

				if (isArgonStraw)
					countAr += 1.0;
				else
					countXe += 1.0;
			}
		}

		m_scale_hHitWMap_B.push_back(countXe);
		m_scale_hHitWMap_B_Ar.push_back(countAr);
	}

	m_flagforscale = 0;
	return 0;
}
