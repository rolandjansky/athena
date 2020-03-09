/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  virtual ~MergeTrackRecordCollection() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }
private:
  ToolHandle<IPileUpTool> m_mergeTool{this, "MergeTrackRecordCollTool", "MergeTrackRecordCollTool", ""};
};
#endif //MCTRUTHSIMALGS_MERGETRACKRECORDCOLLECTION_H
