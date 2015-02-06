/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGERECTIMINGOBJ_H
#define MCTRUTHSIMALGS_MERGERECTIMINGOBJ_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

/** @class MergeRecoTimingObj
 *  @brief an algorithm to merge truth jet collections in the overlay store
 */
class MergeRecoTimingObj : public AthAlgorithm {
public:
  MergeRecoTimingObj(const std::string& name, ISvcLocator* svcLoc);
  StatusCode initialize() override final;
  StatusCode execute() override final;
  StatusCode finalize() override final;
private:
  ToolHandle<IPileUpTool> m_mergeTool;
};
#endif
