/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ELECTRONTRACKVERTEXASSOCIATIONTOOL_H
#define ELECTRONTRACKVERTEXASSOCIATIONTOOL_H

#include "TrackVertexAssociationTool/BaseTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/CheckExc.h"

#include <string>

namespace CP {

/// Track-vertex association tool for electron tracks.  This applies the recommendations from
/// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/InDetTrackingPerformanceGuidelines#Track_to_Vertex_Association
class ElectronTrackVertexAssociationTool : public BaseTrackVertexAssociationTool
{
  ASG_TOOL_CLASS(ElectronTrackVertexAssociationTool, ITrackVertexAssociationTool)

public:
  ElectronTrackVertexAssociationTool(std::string name)
      : BaseTrackVertexAssociationTool(name)
  {
    CHECK_EXC(setProperty("d0sig_cut", 5.0));
    CHECK_EXC(setProperty("dzSinTheta_cut", 0.5));
  }
};

} // namespace CP

#endif // ELECTRONTRACKVERTEXASSOCIATIONTOOL_H
// vim: expandtab tabstop=8 shiftwidth=2 softtabstop=2
