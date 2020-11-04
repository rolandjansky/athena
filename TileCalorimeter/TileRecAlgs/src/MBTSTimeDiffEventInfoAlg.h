/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECALGS_MBTSTIMEDIFFEVENTINFOALG_
#define TILERECALGS_MBTSTIMEDIFFEVENTINFOALG_

// Tile includes
#include "TileEvent/TileContainer.h"
#include "TileEvent/MBTSCollisionTime.h"

// Atlas includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class TileTBID;

class MBTSTimeDiffEventInfoAlg: public AthReentrantAlgorithm {

  public:
    /// Algorithm constructor
    using AthReentrantAlgorithm::AthReentrantAlgorithm;
    virtual ~MBTSTimeDiffEventInfoAlg() override = default;

    /// Algorithm initialize at begin of job
    virtual StatusCode initialize() override;

    /// Algorithm execute once per event
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:

    Gaudi::Property<float> m_timeDiffThreshold{this,
        "TimeDiffThreshold", 10.0, "Time difference threshold to mark event as background"};
    Gaudi::Property<unsigned int> m_minHitsPerSide{this,
        "MinHitsPerSide", 2, "Minimum nubmer of hits per side required to distinguish halo from collision event"};
    Gaudi::Property<float> m_mbts_threshold{this,
        "MBTS_Threshold", 40.0 / 222.0, "Only MBTS hits with apmplitude above threshold in pC will be used"};

    SG::ReadHandleKey<TileCellContainer> m_mbtsContainerKey{this,"MBTSContainer","MBTSContainer","Input MBTS container key"};
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfo", "EventInfo", "Input Event info key"};
    SG::WriteHandleKey<MBTSCollisionTime> m_mbtsCollisionTimeKey{this,"MBTSCollisionTime","MBTSCollisionTime","Output MBTS collision time key"};
    SG::WriteDecorHandleKey<xAOD::EventInfo> m_eventInfoDecorKey{this, "EventInfoDecorKey", "EventInfo.MBTSBackground", "EventInfo decoration key"};

    const TileTBID* m_tileTBID{nullptr};

    const uint8_t m_mask{TileCell::MASK_BADCH | TileCell::MASK_OVER | TileCell::MASK_TIME};
    const uint8_t m_pattern{TileCell::MASK_TIME};
};

#endif
