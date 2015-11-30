/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MBTSTIMEDIFFEVENTINFOALG
#define MBTSTIMEDIFFEVENTINFOALG

#include "AthenaBaseComps/AthAlgorithm.h"

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
    std::string m_mbtsContainerName;
    const uint8_t m_mask, m_pattern;
};

#endif
