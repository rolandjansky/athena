/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef METRECONSTRUCTION_METBADTRACKEVENTFILTER_H
#define METRECONSTRUCTION_METBADTRACKEVENTFILTER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "AsgTools/ToolHandle.h"

class METBadTrackEventFilter : public AthAlgorithm {

public:
  METBadTrackEventFilter(const std::string& name, ISvcLocator* pSvcLocator);
  ~METBadTrackEventFilter() {}

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
private:
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trkseltool;
};
#endif
