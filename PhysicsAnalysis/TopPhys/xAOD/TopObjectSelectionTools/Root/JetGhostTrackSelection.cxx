/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/JetGhostTrackSelection.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

#include "TopEvent/EventTools.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "TopObjectSelectionTools/MsgCategory.h"
using namespace TopObjectSelectionTools;

namespace top {

  JetGhostTrackSelection::JetGhostTrackSelection(const double ptcut, const double etamax, const std::string vertexassociation, const double ptcut_jet, const double etacut_jet ) :
    m_ptcut(ptcut),
    m_etamax(etamax),
    m_ptcut_jet(ptcut_jet),
    m_etamax_jet(etacut_jet),
    m_vertexassociation(vertexassociation),
    m_trkseltool("top::GhostTrackCPTools::TrkSelTool")
  {

    top::check(m_trkseltool.retrieve(), "Failed to retrieve InDetTrackSelectionTool");
    
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackingCPRecsRun2Final
    if (m_vertexassociation == "nominal"){
        m_d0=2;
        m_z0sintheta=3;
    }
    else if(m_vertexassociation == "tight"){
        m_d0=0.5;
        m_z0sintheta=0.5;
    }
    else {
        ATH_MSG_INFO("Not applying any vertex association for GA tracks");
        m_d0=10000;
        m_z0sintheta=10000;
        
    }

  }

  JetGhostTrackSelection::~JetGhostTrackSelection() {
  }

  bool JetGhostTrackSelection::passSelection(const xAOD::TrackParticle& track, const xAOD::Vertex& primaryVertex) const {
      
   // Track quality requirements   
    if (track.pt() < m_ptcut) return false;

    if (std::fabs(track.eta()) > m_etamax) return false;

    if ( !m_trkseltool->accept(track, &primaryVertex ) ) return false;
   
   // Vertex association requirements
   float deltaz0 = track.z0() + track.vz() - primaryVertex.z();

   if ((std::fabs(track.d0()) > m_d0) || (std::fabs(sin(track.theta()) * deltaz0) > m_z0sintheta))
       return false;

    return true;
  }
  
  bool JetGhostTrackSelection::JetConsideredForGhostTrackSelection(double pt, const double eta) const{
    if ( (pt > m_ptcut_jet) && (std::abs(eta) < m_etamax_jet) ){
        return true;
    }
    else {
        return false;
    }
      
  }

  void JetGhostTrackSelection::print(std::ostream& os) const {
    os << "Ghost Associated Track \n"
       << "    * pT > " << m_ptcut << "\n"
       << "    * |eta| < " << m_etamax
       << "    * |d0| < " << m_d0
       << "    * |z0sintheta| < " << m_z0sintheta;
  }
  
  
}
