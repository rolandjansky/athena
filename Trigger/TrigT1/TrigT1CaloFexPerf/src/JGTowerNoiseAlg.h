/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JGTOWERNOISEALG_H
#define TRIGT1CALOFEXPERF_JGTOWERNOISEALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "TProfile.h"
#include "TH1.h"
#include "TFile.h"

namespace LVL1
{
  class JGTowerNoiseAlg : public AthReentrantAlgorithm
  {
  public:
    JGTowerNoiseAlg(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~JGTowerNoiseAlg() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    SG::ReadHandleKey<xAOD::JGTowerContainer> m_inputKey;
    std::string m_noiseFile;
    bool m_doJFex;
    float m_defaultNoise;

    // Detector eta binning
    std::vector<float> m_etaBins;
    // Noise values for the full detector (for gFEX)
    std::vector<float> m_noiseValues;
    // Split detector noise values (for jFEX)
    std::vector<float> m_noiseValuesEM;
    std::vector<float> m_noiseValuesHAD;
    std::map<std::size_t, float> m_noiseValuesFCAL;
  }; //> end class JGTowerNoiseAlg
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_JGTOWERNOISEALG_H
