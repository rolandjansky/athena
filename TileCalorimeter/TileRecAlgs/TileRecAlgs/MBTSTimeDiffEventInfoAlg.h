/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MBTSTIMEDIFFEVENTINFOALG
#define MBTSTIMEDIFFEVENTINFOALG

// Tile includes
#include "TileEvent/TileContainer.h"
#include "TileEvent/MBTSCollisionTime.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class TileTBID;

class MBTSTimeDiffEventInfoAlg: public AthAlgorithm {

  public:
    /// Algorithm constructor
    MBTSTimeDiffEventInfoAlg(const std::string &name, ISvcLocator *pSvcLocator);

    /// Algorithm initialize at begin of job
    virtual StatusCode initialize();

    /// Algorithm execute once per event
    virtual StatusCode execute();

    /// Algorithm finalize at end of job
    virtual StatusCode finalize();

  private:

    float m_timeDiffThreshold;
    unsigned m_minhitsperside;
    float m_mbts_threshold;
    const TileTBID* m_tileTBID;

    SG::ReadHandleKey<TileCellContainer> m_mbtsContainerKey{this,"MBTSContainer","MBTSContainer","Input MBTS container key"};
    SG::WriteHandleKey<MBTSCollisionTime> m_mbtsCollisionTimeKey{this,"MBTSCollisionTime","MBTSCollisionTime","Output MBTS collision time key"};

    const uint8_t m_mask, m_pattern;
};

#endif
