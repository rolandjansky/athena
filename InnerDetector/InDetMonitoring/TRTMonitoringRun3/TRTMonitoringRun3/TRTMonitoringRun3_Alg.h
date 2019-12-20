/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTMONITORINGRUN3_ALG_H
#define TRTMONITORINGRUN3_ALG_H

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

class TRTMonitoringRun3_Alg : public AthMonitorAlgorithm {
public:
    TRTMonitoringRun3_Alg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TRTMonitoringRun3_Alg();
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
	
		
	std::vector<unsigned int> m_rodMap;
	enum GasType{ Xe = 0, Ar = 1, Kr = 2 };
	
private:	
	
	bool m_doRDOsMon;
	bool m_doTracksMon;
	int m_usedEvents;

	bool m_doStraws;
	bool m_doChips;
	bool m_doShift;
	bool m_doExpert;
	bool m_doMaskStraws;
	bool m_doEfficiency;

	float m_DistToStraw;
	float m_minP;

	///// Additional stuff for efficiency measurements, online only for now


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
	// Will come back when raw monitoring added
	//    SG::ReadHandleKey<TRT_RDO_Container>   m_rdoContainerKey       {this, "TRTRawDataObjectName", "TRT_RDOs", "Name of TRT RDOs container"};
	SG::ReadHandleKey<TrackCollection>     m_trackCollectionKey    {this, "TRTTracksObjectName", "Tracks", "Name of tracks container"};
	SG::ReadHandleKey<TrackCollection>     m_combTrackCollectionKey{this, "track_collection_hole_finder", "CombinedInDetTracks", "Name of tracks container used for hole finder"};
	
    SG::ReadHandleKey<xAOD::EventInfo>     m_xAODEventInfoKey      {this, "xAODEventInfo", "EventInfo", "Name of EventInfo object"};
	SG::ReadHandleKey<InDetTimeCollection> m_TRT_BCIDCollectionKey {this, "TRTBCIDCollectionName",        "TRT_BCID",            "Name of TRT BCID collection"};

};
#endif
