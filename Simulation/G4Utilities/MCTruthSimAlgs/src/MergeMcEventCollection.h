/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGEMCEVENTCOLLECTION_H
#define MCTRUTHSIMALGS_MERGEMCEVENTCOLLECTION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

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
  StatusCode initialize() override final;
  StatusCode execute() override final;
  StatusCode finalize() override final;

private:
  ToolHandle<IPileUpTool> m_mergeTool;
};
#endif //MCTRUTHSIMALGS_MERGEMCEVENTCOLLECTION_H
