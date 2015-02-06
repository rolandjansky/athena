/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGEGENERICMUONSIMHITCOLL_H
#define MCTRUTHSIMALGS_MERGEGENERICMUONSIMHITCOLL_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

/** @class MergeGenericMuonSimHitColl
 *  @brief an algorithm to merge truth jet collections in the overlay store
 */
class MergeGenericMuonSimHitColl : public AthAlgorithm {
public:
  MergeGenericMuonSimHitColl(const std::string& name, ISvcLocator* svcLoc);
  StatusCode initialize() override final;
  StatusCode execute() override final;
  StatusCode finalize() override final;
private:
  ToolHandle<IPileUpTool> m_mergeTool;
};
#endif
