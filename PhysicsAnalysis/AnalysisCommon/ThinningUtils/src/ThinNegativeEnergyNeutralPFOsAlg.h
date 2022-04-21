///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef THINNINGUTILS_ThinNegativeEnergyNeutralPFOsAlg_H
#define THINNINGUTILS_ThinNegativeEnergyNeutralPFOsAlg_H 1

/**
 @class ThinNegativeEnergyNeutralPFOsAlg
*/

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ThinningHandleKey.h"

#include "xAODPFlow/FlowElementContainer.h"

class ThinNegativeEnergyNeutralPFOsAlg final : public AthReentrantAlgorithm
{
public:
  /// Constructor with parameters:
  ThinNegativeEnergyNeutralPFOsAlg(const std::string& name,
                                   ISvcLocator* pSvcLocator);

  /// Destructor:
  virtual ~ThinNegativeEnergyNeutralPFOsAlg() = default;

  /// Athena algorithm's initalize hook
  virtual StatusCode initialize() override final;

  /// Athena algorithm's execute hook
  virtual StatusCode execute(const EventContext& ctx) const override final;

  /// Athena algorithm's finalize hook
  virtual StatusCode finalize() override final;

private:
  StringProperty m_streamName{
    this,
    "StreamName",
    "",
    "Name of the stream for which thinning is being done."
  };

  /// Should the thinning run?
  BooleanProperty m_doThinning{
    this,
    "ThinNegativeEnergyNeutralPFOs",
    true,
    "Should the thinning of negative energy neutral PFOs be run?"
  };

  /// Names of the containers to thin
  SG::ThinningHandleKey<xAOD::FlowElementContainer> m_neutralPFOsKey{
    this,
    "NeutralPFOsKey",
    "",
    "StoreGate key for the FlowElementContainer to be thinned"
  };
  SG::ThinningHandleKey<xAOD::FlowElementContainer> m_LCNeutralPFOsKey{
    this,
    "LCNeutralPFOsKey",
    "",
    "StoreGate key for the LC FlowElementContainer to be thinned (if any)"
  };

  /// Counters
  mutable std::atomic<unsigned long> m_nEventsProcessed;
  mutable std::atomic<unsigned long> m_nNeutralPFOsProcessed;
  mutable std::atomic<unsigned long> m_nNeutralPFOsThinned;
};

#endif //> !THINNINGUTILS_ThinNegativeEnergyNeutralPFOsAlg_H
