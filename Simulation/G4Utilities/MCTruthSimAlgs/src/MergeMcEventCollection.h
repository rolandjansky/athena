/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGEMCEVENTCOLLECTION_H
#define MCTRUTHSIMALGS_MERGEMCEVENTCOLLECTION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "PileUpTools/PileUpMergeSvc.h"


/** @class MergeMcEventCollection
 *  @brief an algorithm to merge MC truth collection in the overlay store
 *
 *  $Id:
 *  @author pcalafiura@lbl.gov
 *
 */
class MergeMcEventCollection : public AthAlgorithm {
public:
  MergeMcEventCollection(const std::string& name, ISvcLocator *svcLoc);
  virtual ~MergeMcEventCollection() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }

private:
  ToolHandle<IPileUpTool> m_mergeTool{this, "MergeMcEventCollTool", "MergeMcEventCollTool", ""};
};
#endif //MCTRUTHSIMALGS_MERGEMCEVENTCOLLECTION_H
