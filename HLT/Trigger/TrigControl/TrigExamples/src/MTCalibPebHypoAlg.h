/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEXPARTIALEB_MTCALIBPEBHYPOALG_H
#define TRIGEXPARTIALEB_MTCALIBPEBHYPOALG_H

// Trigger includes
#include "MTCalibPebHypoTool.h"
#include "DecisionHandling/HypoBase.h"

/** @class MTCalibPebHypoAlg
 *  @brief Hypo algorithm for framework testing
 **/
class MTCalibPebHypoAlg : public HypoBase {
public:
  /// Standard constructor
  MTCalibPebHypoAlg(const std::string& name, ISvcLocator* svcLoc);
  /// Standard destructor
  virtual ~MTCalibPebHypoAlg();

  // ------------------------- AthReentrantAlgorithm methods -------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute(const EventContext& eventContext) const override;
private:
  ToolHandleArray<MTCalibPebHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"};
};

#endif // TRIGEXPARTIALEB_MTCALIBPEBHYPOALG_H
