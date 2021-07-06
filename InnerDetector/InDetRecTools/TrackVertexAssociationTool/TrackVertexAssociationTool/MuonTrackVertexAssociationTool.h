/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONTRACKVERTEXASSOCIATIONTOOL_H
#define MUONTRACKVERTEXASSOCIATIONTOOL_H

#include "TrackVertexAssociationTool/BaseTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/CheckExc.h"

#include <string>

namespace CP {

/// Track-vertex association tool for muon tracks.  This applies the recommendations from
/// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/InDetTrackingPerformanceGuidelines#Track_to_Vertex_Association
class MuonTrackVertexAssociationTool : public BaseTrackVertexAssociationTool
{
  ASG_TOOL_CLASS(MuonTrackVertexAssociationTool, ITrackVertexAssociationTool)

public:
  MuonTrackVertexAssociationTool(std::string name)
      : BaseTrackVertexAssociationTool(name)
  {
    CHECK_EXC(setProperty("d0sig_cut", 3.0));
    CHECK_EXC(setProperty("dzSinTheta_cut", 0.5));
  }
};

} // namespace CP

#endif // MUONTRACKVERTEXASSOCIATIONTOOL_H
// vim: expandtab tabstop=8 shiftwidth=2 softtabstop=2
