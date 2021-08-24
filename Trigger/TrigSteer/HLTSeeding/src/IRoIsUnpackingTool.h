/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_IROISUNPACKINGTOOL_H
#define HLTSEEDING_IROISUNPACKINGTOOL_H

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "GaudiKernel/IAlgTool.h"

namespace ROIB {
  class RoIBResult;
}

/**
 * @class IRoIsUnpackingTool
 * @brief Interface for RoI unpacking tools
 */
class IRoIsUnpackingTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IRoIsUnpackingTool, 1, 0);

  /**
   * Unpack RoIB result depending on implementaton (e.g. EM, Jet, ...)
   *
   * A collection of decision objects is created in event store with each decision tagged by the chain ID it relates to.
   * In addition, an implementation specific "RecRoI" collection is produced.
   */
  virtual StatusCode unpack(const EventContext& ctx,
                            const ROIB::RoIBResult& roib,
                            const HLT::IDSet& activeChains) const = 0;

  /**
   * Unpack specific sub-system RoIs (Muon, eFexEM, gFexJet, ...) linked to L1TriggerResult depending on implementation
   *
   * A collection of decision objects is created in event store with each decision tagged by the chain ID it relates to.
   */
  virtual StatusCode unpack(const EventContext& /*ctx*/,
                            const xAOD::TrigComposite& /*l1TriggerResult*/,
                            const HLT::IDSet& /*activeChains*/) const = 0;
};

#endif
