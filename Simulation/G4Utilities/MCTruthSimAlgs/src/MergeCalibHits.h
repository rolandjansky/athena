/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGECALIBHITS_H
#define MCTRUTHSIMALGS_MERGECALIBHITS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

/** @class MergeCalibHits
 *  @brief an algorithm to propagate CaloCalibrationHitContainers into the overlay store
 */
class MergeCalibHits : public AthAlgorithm {
public:
  MergeCalibHits(const std::string& name, ISvcLocator* svcLoc);
  virtual ~MergeCalibHits() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }
private:
  ToolHandle<IPileUpTool> m_mergeTool{this, "MergeCalibHitsTool", "MergeCalibHitsTool", ""};
};
#endif //MCTRUTHSIMALGS_MERGECALIBHITS_H
