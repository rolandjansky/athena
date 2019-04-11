/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGESTGCSIMHITCOLL_H
#define MCTRUTHSIMALGS_MERGESTGCSIMHITCOLL_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

/** @class MergesTGCSimHitColl
 *  @brief an algorithm to merge truth jet collections in the overlay store
 */
class MergesTGCSimHitColl : public AthAlgorithm {
public:
  MergesTGCSimHitColl(const std::string& name, ISvcLocator* svcLoc);
  StatusCode initialize() override final;
  StatusCode execute() override final;
  StatusCode finalize() override final;
private:
  ToolHandle<IPileUpTool> m_mergeTool;
};
#endif
