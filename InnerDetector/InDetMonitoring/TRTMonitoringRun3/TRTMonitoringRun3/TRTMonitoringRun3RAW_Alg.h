/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTMONITORINGRUN3RAW_ALG_H
#define TRTMONITORINGRUN3RAW_ALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

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

#include "InDetByteStreamErrors/TRT_BSErrContainer.h"

// STDLIB
#include <string>
#include <vector>
#include <set>

namespace InDetDD {
    class TRT_DetectorManager;
}
 
class AtlasDetectorID;
class TRT_ID;
class Identifier;
class ITRT_StrawStatusSummaryTool;

class TRTMonitoringRun3RAW_Alg : public AthMonitorAlgorithm {
public:
    TRTMonitoringRun3RAW_Alg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TRTMonitoringRun3RAW_Alg();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
    
private:
    const AtlasDetectorID * m_idHelper;
    const TRT_ID* m_pTRTHelper;
    const InDetDD::TRT_DetectorManager *m_mgr;
    
    unsigned char m_mat_chip_E[64][3840];
    unsigned char m_mat_chip_B[64][1642];
    
    enum GasType{ Xe = 0, Ar = 1, Kr = 2 };
    
    static const int s_Straw_max[2];
    
    static const int s_numberOfBarrelStacks;
	static const int s_numberOfEndCapStacks;
    
    bool m_doStraws;
    bool m_doExpert;
    bool m_doChips;
    bool m_doRDOsMon;
    bool m_doShift;
    bool m_doMaskStraws;
    bool m_doEfficiency;
    bool m_useHoleFinder;
    bool m_doHitsMon;
    float m_DistToStraw;
    
    BooleanProperty m_ArgonXenonSplitter{this, "doArgonXenonSeparation", true};
    
    int m_totalEvents;
    float m_longToTCut;
    
    int m_min_si_hits;
    int m_min_pixel_hits;
    int m_min_sct_hits;
    int m_min_trt_hits;
    int m_minTRThits;
    int m_every_xth_track;
    float m_max_abs_d0;
	float m_max_abs_z0;
    float m_max_abs_eta;
    float m_minP;
    float m_min_pT;
    
    StatusCode fillTRTRDOs(const TRT_RDO_Container& rdoContainer,
	                       const xAOD::EventInfo& eventInfo,
	                       const InDetTimeCollection* trtBCIDCollection) const;
    StatusCode fillTRTEfficiency(const TrackCollection& combTrackCollection) const;
    StatusCode fillTRTHits(const TrackCollection& trackCollection) const;       
    
    int chipToBoard(int chip) const;
    int chipToBoard_EndCap(int chip) const;
    StatusCode checkTRTReadoutIntegrity(const xAOD::EventInfo& eventInfo) const;
    bool checkEventBurst(const TRT_RDO_Container& rdoContainer) const;
    int strawNumberEndCap(int strawNumber, int strawLayerNumber, int LayerNumber, int phi_stack, int side) const;
    int strawNumber(int strawNumber, int strawlayerNumber, int LayerNumber) const;
    int strawLayerNumber(int strawLayerNumber, int LayerNumber) const;
    float radToDegrees(float radValue) const;

    // Services
    ToolHandle<ITRT_StrawStatusSummaryTool> m_sumTool;

    // Data handles
    SG::ReadHandleKey<TRT_RDO_Container>   m_rdoContainerKey{this,       "TRTRawDataObjectName",   "TRT_RDOs",      "Name of TRT RDOs container"};
    SG::ReadHandleKey<InDetTimeCollection> m_TRT_BCIDCollectionKey{this, "TRTBCIDCollectionName",  "TRT_BCID",      "Name of TRT BCID collection"};
    SG::ReadHandleKey<xAOD::EventInfo>     m_xAODEventInfoKey{this,      "xAODEventInfo",          "EventInfo",     "Name of EventInfo object"};
    SG::ReadHandleKey<TrackCollection>     m_combTrackCollectionKey{this, "track_collection_hole_finder", "CombinedInDetTracks", "Name of tracks container used for hole finder"};
    SG::ReadHandleKey<TrackCollection> m_trackCollectionKey{this, "TRTTracksObjectName", "Tracks", "Name of tracks container"};

    SG::ReadHandleKey<TRT_BSErrContainer> m_bsErrContKey{this,"ByteStreamErrors","TRT_ByteStreamErrs","SG key of TRT ByteStream Error container"};

    // Tools
    ToolHandle<Trk::ITrackHoleSearchTool>  m_trt_hole_finder{this, "trt_hole_search", "TRTTrackHoleSearchTool", "Track hole search tool name"};
    ToolHandle<Trk::ITrackSummaryTool>     m_TrackSummaryTool{this, "TrackSummaryTool", "InDetTrackSummaryTool", "Track summary tool name"};
    
    //Deciphers status HT to  GasType Enumerator
	inline GasType Straw_Gastype(int stat) const {
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
    
    bool m_isCosmics;
    int  m_EventBurstCut;
   
};
#endif
