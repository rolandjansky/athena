/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRACKSELECTION_H_
#define TRACKSELECTION_H_

#include "TopObjectSelectionTools/TrackSelectionBase.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"


namespace top {
  class TrackSelection: public TrackSelectionBase {
  public:
    TrackSelection(const double ptcut, const double etamax);
    virtual ~TrackSelection();
    virtual bool passSelection(const xAOD::TrackParticle&, const xAOD::Vertex&) override;
    virtual void print(std::ostream& os) const override;
  private:
    double m_ptcut;
    double m_etamax;

    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkseltool;

  };
}
#endif
