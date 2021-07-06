/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGT1CALOFEXPERF_SCEMULATION_H
#define TRIGT1CALOFEXPERF_SCEMULATION_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloConditions/CaloNoiseSigmaDiff.h"
#include "CaloEvent/CaloBCIDAverage.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"

#include <map>
#include <tuple>
#include <TH1F.h>

namespace LVL1
{
  class SCEmulation : public AthReentrantAlgorithm
  {
  public:
    SCEmulation(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~SCEmulation() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext &ctx) const override;

  private:
    SG::ReadHandleKey<CaloCellContainer> m_inputCellsKey;
    SG::ReadHandleKey<xAOD::EventInfo> m_evtInfoKey;
    SG::ReadHandleKey<CaloBCIDAverage> m_caloBCIDAverageKey{
        this, "CaloBCIDAverage", "CaloBCIDAverage"};
    SG::ReadCondHandleKey<CaloNoiseSigmaDiff> m_caloNoiseSigmaDiffKey{
        this, "CaloNoiseSigmaDiff", "CaloNoiseSigmaDiff"};
    SG::WriteHandleKey<CaloCellContainer> m_outputSuperCellsKey;

    /// Offline<->supercell mapping tool
    ToolHandle<ICaloSuperCellIDTool> m_scidTool{"CaloSuperCellIDTool"};
    /// Use noise values
    bool m_useNoise;
    /// Compensate for BCIDs
    bool m_useBCID;
    /// The cell timing file
    std::string m_cellTimingFile;

    // Internals
    /// eta binning read from timing file
    std::map<CaloSampling::CaloSample, std::vector<float>> m_etaBins;
    /// Et binning read from timing file
    std::map<CaloSampling::CaloSample, std::vector<float>> m_etBins;
    /// Timing distributions
    // Map keys are sampling, eta bin, phi bin
    std::map<std::tuple<CaloSampling::CaloSample, std::size_t, std::size_t>, std::map<float, float>> m_timingSamples;

    /// Super cell manager
    const CaloSuperCellDetDescrManager *m_scellMgr{nullptr};
    /// Calo ID helpers
    const CaloIdManager *m_caloIdMgr{nullptr};

    std::size_t getEtaIndex(CaloSampling::CaloSample sample, float eta) const;
    std::size_t getEtIndex(CaloSampling::CaloSample sample, float et) const;

  }; //> end class SCEmulation
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_SCEMULATION_H