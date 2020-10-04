/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class DQTBackgroundMon
 * @author C. D. Burton <burton@utexas.edu>
 * @date 2019/07/31
 * @brief Monitor background processes in the detector.
 */

#ifndef DQTBACKGROUNDMON_H
#define DQTBACKGROUNDMON_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

#include "TagEvent/RawInfoSummaryForTag.h"
#include "LArRecEvent/LArCollisionTime.h"
#include "TileEvent/MBTSCollisionTime.h"
#include "TileEvent/TileContainer.h"
#include "LUCID_RawEvent/LUCID_RawDataContainer.h"
#include "RecBackgroundEvent/BeamBackgroundData.h"

class DQTBackgroundMon : public AthMonitorAlgorithm {
public:
    DQTBackgroundMon(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~DQTBackgroundMon();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
private:
    template <class T> using RH = SG::ReadHandle<T>;
    template <class T> using RHK = SG::ReadHandleKey<T>;
    template <class T> using GP = Gaudi::Property<T>;

	// Activates trigger and muon parts of the algorithm
    GP<bool> m_doMuons {this, "Muons", true};

    // StoreGate keys
    RHK<RawInfoSummaryForTag> m_RawInfoSummaryForTagKey{this, "RawInfoSummaryForTagKey", "RawInfoSummaryForTag", ""};
    RHK<LArCollisionTime> m_LArCollisionTimeKey {this, "LArCollisionTimeKey", "LArCollisionTime", ""};
    RHK<MBTSCollisionTime> m_MBTSCollisionTimeKey {this, "MBTSCollisionTimeKey", "MBTSCollisionTime", ""};
    RHK<TileCellContainer> m_TileCellContainerKey {this, "MBTSContainerName", "MBTSContainer", ""};
    RHK<LUCID_RawDataContainer> m_LUCID_RawDataContainerKey {this, "Lucid_RawDataKey","Lucid_RawData", ""};
    RHK<Trk::SegmentCollection> m_SegmentCollectionKey {this, "MuonSegmentsKey", "MuonSegments", ""};
    RHK<BeamBackgroundData> m_BeamBackgroundDataKey {this, "BeamBackgroundDataKey", "BeamBackgroundData", ""};
    RHK<xAOD::VertexContainer> m_VertexContainerKey {this, "PrimaryVerticesKey", "PrimaryVertices", ""};

    // For parsing the int returned by TrigDecTool's getBGCode()
    GP<int> m_filledBG {this, "FilledBGIndex", 1};
    GP<int> m_emptyBG {this, "EmptyBGIndex", 3};
    GP<int> m_unpairIsoBG {this, "UnpairIsoBGIndex", 4};
    GP<int> m_unpairNonIsoBG {this, "UnpairNonIsoBGIndex", 5};

    // Various cuts used in fillHistograms()
    GP<int> m_upPixSP {this, "UpPixSP", 5000};
    GP<int> m_upSctSP {this, "UpSctSP", 10000};
    GP<int> m_LArECSideCut {this, "LArECSideCut", 1};
    GP<int> m_MBTSSideCut {this, "MBTSSideCut", 2};
    GP<float> m_MBTSTimeCut {this, "MBTSTimeCut", 15};
    GP<float> m_MBTSThresholdCut {this, "MBTSThresholdCut", 40.0/222.0};
    GP<int> m_MBTSMask {this, "MBTSMask", TileCell::MASK_BADCH | TileCell::MASK_OVER | TileCell::MASK_TIME };
    GP<int> m_MBTSPattern {this, "MBTSPattern", TileCell::MASK_TIME };
    GP<int> m_nBkgWords {this, "NBkgWords", 32};
};
#endif
