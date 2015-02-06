/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGETRACKRECORDCOLLECTION_H
#define MCTRUTHSIMALGS_MERGETRACKRECORDCOLLECTION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

/** @class MergeTrackRecordCollection
 *  @brief an algorithm to propagate TrackRecordCollections into the overlay store
 */
class MergeTrackRecordCollection : public AthAlgorithm {
public:
  MergeTrackRecordCollection(const std::string& name, ISvcLocator* svcLoc);
  StatusCode initialize() override final;
  StatusCode execute() override final;
  StatusCode finalize() override final;
private:
  ToolHandle<IPileUpTool> m_mergeTool;
};
#endif //MCTRUTHSIMALGS_MERGETRACKRECORDCOLLECTION_H
