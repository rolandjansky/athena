///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef THINNINGUTILS_ThinNegativeEnergyCaloClustersAlg_H
#define THINNINGUTILS_ThinNegativeEnergyCaloClustersAlg_H 1

/**
 @class ThinNegativeEnergyCaloClustersAlg
*/

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ThinningHandleKey.h"
// STL includes
#include <atomic>
#include <string>

#include "xAODCaloEvent/CaloClusterContainer.h"

class ThinNegativeEnergyCaloClustersAlg final : public AthReentrantAlgorithm
{
public:
  /// Constructor with parameters:
  ThinNegativeEnergyCaloClustersAlg(const std::string& name,
                                    ISvcLocator* pSvcLocator);

  /// Destructor:
  virtual ~ThinNegativeEnergyCaloClustersAlg() = default;

  /// Athena algorithm's initalize hook
  virtual StatusCode initialize() override final;

  /// Athena algorithm's execute hook
  StatusCode execute(const EventContext& ctx) const override final;

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
    "ThinNegativeEnergyCaloClusters",
    true,
    "Should the thinning of negative energy calo clusters be run?"
  };

  /// Names of the containers to thin
  SG::ThinningHandleKey<xAOD::CaloClusterContainer> m_caloClustersKey{
    this,
    "CaloClustersKey",
    "CaloCalTopoClusters",
    "StoreGate key for the CaloClustersContainer to be thinned"
  };

  /// Counters
  mutable std::atomic<unsigned long> m_nEventsProcessed;
  mutable std::atomic<unsigned long> m_nCaloClustersProcessed;
  mutable std::atomic<unsigned long> m_nCaloClustersThinned;
};

#endif //> !THINNINGUTILS_ThinNegativeEnergyCaloClustersAlg_H
