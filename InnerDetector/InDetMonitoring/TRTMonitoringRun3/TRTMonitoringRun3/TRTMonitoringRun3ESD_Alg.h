/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTMONITORINGRUN3ESD_ALG_H
#define TRTMONITORINGRUN3ESD_ALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

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
    class Track;
    class TrackStateOnSurface;
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
class ITRT_StrawNeighbourSvc;

class TRTMonitoringRun3ESD_Alg : public AthMonitorAlgorithm {
public:
    TRTMonitoringRun3ESD_Alg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TRTMonitoringRun3ESD_Alg();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
private:
    BooleanProperty m_ArgonXenonSplitter{this, "doArgonXenonSeparation", true};
    enum GasType{ Xe = 0, Ar = 1, Kr = 2 };

    const AtlasDetectorID * m_idHelper;

    StatusCode fillTRTTracks(const xAOD::TrackParticleContainer& trackCollection,
                         const xAOD::TrigDecision* trigDecision,
                         const ComTime* comTimeObject) const;                             
    StatusCode fillTRTHighThreshold(const xAOD::TrackParticleContainer& trackCollection,
                                const xAOD::EventInfo& eventInfo) const;
    int strawLayerNumber(int strawLayerNumber, int LayerNumber) const;


    // Returns the straw number (0-1641) given the layerNumber, strawlayerNumber, and strawNumber, all gotten from Athena IDHelper
    int strawNumber(int strawNumber, int strawlayerNumber, int LayerNumber) const;
    // Returns Barrel Board Number
    int chipToBoard(int chip) const;
    // Returns EndCap Board Number
    int chipToBoard_EndCap(int chip) const;

    // Returns Degrees, converted from radians (Athena Standard units)
    float radToDegrees(float radValue) const;
    int strawNumber_reverse (int inp_strawnumber,  int* strawNumber, int* strawlayerNumber, int* LayerNumber) const;
    int strawLayerNumber_reverse(int strawLayerNumInp,int* strawLayerNumber, int* LayerNumber) const;
    int strawNumberEndCap(int strawNumber, int strawLayerNumber, int LayerNumber, int phi_stack, int side) const;
    bool checkEventBurst(const TRT_RDO_Container& rdoContainer) const;

private:
    static const int s_numberOfBarrelStacks;
    static const int s_numberOfEndCapStacks;
    static const int s_Straw_max[2];
    static const int s_iStack_max[2];
    static const int s_iChip_max[2];
    static const int s_numberOfStacks[2];
    static const int s_moduleNum[2];

    // Services
    ToolHandle<ITRT_StrawStatusSummaryTool> m_sumTool;
    ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc;
    ToolHandle<ITRT_CalDbTool> m_TRTCalDbTool;

    // Data handles
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackCollectionKey{this, "TrackParticleContainerKeys", "InDetTrackParticles", "Keys for TrackParticle Container"};
    SG::ReadHandleKey<xAOD::EventInfo> m_xAODEventInfoKey{this, "xAODEventInfo", "EventInfo", "Name of EventInfo object"};
    SG::ReadHandleKey<InDetTimeCollection> m_TRT_BCIDCollectionKey{this, "TRTBCIDCollectionName", "TRT_BCID", "Name of TRT BCID collection"};
    SG::ReadHandleKey<ComTime> m_comTimeObjectKey{this, "ComTimeObjectName", "TRT_Phase", "Name of ComTime object"};
    SG::ReadHandleKey<xAOD::TrigDecision> m_trigDecisionKey{this, "TrigDecisionObjectName", "xTrigDecision", "Name of trigger decision object"};

    // Tools
    ToolHandle<Trk::ITrackSummaryTool> m_TrackSummaryTool{this, "TrackSummaryTool", "InDetTrackSummaryTool", "Track summary tool name"};
    ToolHandle<ITRT_DriftFunctionTool> m_drifttool;

    const TRT_ID* m_pTRTHelper;
    const InDetDD::TRT_DetectorManager *m_mgr;

    bool m_doTracksMon;
    bool m_doStraws;
    bool m_doChips;
    bool m_doShift;
    bool m_doExpert;

    unsigned char m_mat_chip_B[64][1642];
    unsigned char m_mat_chip_E[64][3840];

    float m_DistToStraw;
    bool m_isCosmics;

    int m_min_si_hits;
    int m_min_pixel_hits;
    int m_min_sct_hits;
    int m_min_trt_hits;
    int m_minTRThits;
    float m_minP;
	float m_min_pT;


    // Deciphers status HT to  GasType Enumerator
    inline GasType Straw_Gastype(int stat) const {
        // getStatusHT returns enum {Undefined, Dead, Good, Xenon, Argon, Krypton}.
        // Our representation of 'GasType' is 0:Xenon, 1:Argon, 2:Krypton
        GasType Gas = Xe; // Xenon is default
        if (m_ArgonXenonSplitter) {
            //      int stat=m_sumSvc->getStatusHT(TRT_Identifier);
            if       ( stat==2 || stat==3 ) { Gas = Xe; } // Xe
            else if  ( stat==1 || stat==4 ) { Gas = Ar; } // Ar
            else if  ( stat==5 )            { Gas = Kr; } // Kr
            else if  ( stat==6 )            { Gas = Xe; } // Emulate Ar (so treat as Xe here)
            else if  ( stat==7 )            { Gas = Xe; } // Emulate Kr (so treat as Xe here)
            else { ATH_MSG_FATAL ("getStatusHT = " << stat << ", must be 'Good(2)||Xenon(3)' or 'Dead(1)||Argon(4)' or 'Krypton(5)!' or 6 or 7 for emulated types!");
                throw std::exception();
            }
        }
        return Gas;
    }
    
    int  m_EventBurstCut;
};
#endif
