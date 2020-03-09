/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  virtual ~MergeGenericMuonSimHitColl() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }

private:
  ToolHandle<IPileUpTool> m_mergeTool{this, "MergeGenericMuonSimHitCollTool", "MergeGenericMuonSimHitCollTool", ""};
};
#endif
