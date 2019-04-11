/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGEMMSIMHITCOLL_H
#define MCTRUTHSIMALGS_MERGEMMSIMHITCOLL_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

/** @class MergeMMSimHitColl
 *  @brief an algorithm to merge truth jet collections in the overlay store
 */
class MergeMMSimHitColl : public AthAlgorithm {
public:
  MergeMMSimHitColl(const std::string& name, ISvcLocator* svcLoc);
  StatusCode initialize() override final;
  StatusCode execute() override final;
  StatusCode finalize() override final;
private:
  ToolHandle<IPileUpTool> m_mergeTool;
};
#endif
