/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef JETGHOSTTRACKSELECTION_H_
#define JETGHOSTTRACKSELECTION_H_

#include "TopObjectSelectionTools/JetGhostTrackSelectionBase.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"


namespace top {
  class JetGhostTrackSelection: public JetGhostTrackSelectionBase {
  public:
    JetGhostTrackSelection(const double ptcut, const double etamax, const std::string vertexassociation);
    ~JetGhostTrackSelection();
    virtual bool passSelection(const xAOD::TrackParticle&, const xAOD::Vertex&) override;
    virtual void print(std::ostream& os) const override;
  private:
    //Include selection on pt and eta  
    double m_ptcut;
    double m_etamax;
    
    //Requirement on the vertex association
    std::string m_vertexassociation;
    double m_d0;
    double m_z0sintheta;
    

    ToolHandle<InDet::InDetTrackSelectionTool> m_trkseltool;

  };
}
#endif
