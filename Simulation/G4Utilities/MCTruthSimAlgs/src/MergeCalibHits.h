/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  StatusCode initialize() override final;
  StatusCode execute() override final;
  StatusCode finalize() override final;
private:
  ToolHandle<IPileUpTool> m_mergeTool;
};
#endif //MCTRUTHSIMALGS_MERGECALIBHITS_H
