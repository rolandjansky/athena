/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  virtual ~MergeRecoTimingObj() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }
private:
  ToolHandle<IPileUpTool> m_mergeTool{this, "MergeRecoTimingObjTool", "MergeRecoTimingObjTool", ""};
};
#endif
