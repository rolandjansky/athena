/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/TrackSelection.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

#include "TopEvent/EventTools.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "TopObjectSelectionTools/MsgCategory.h"
using namespace TopObjectSelectionTools;

namespace top {

  TrackSelection::TrackSelection(const double ptcut, const double etamax) :
    m_ptcut(ptcut),
    m_etamax(etamax),
    m_trkseltool("TrkSelTool")
  {

    top::check(m_trkseltool.retrieve(), "Failed to retrieve InDetTrackSelectionTool");

  }

  TrackSelection::~TrackSelection() {
  }

  bool TrackSelection::passSelection(const xAOD::TrackParticle& track, const xAOD::Vertex& primaryVertex) {
    if (track.pt() < m_ptcut) return false;

    if (std::fabs(track.eta()) > m_etamax) return false;


    if (track.isAvailable<char>("passTruthFilter")) {
      if (!track.auxdecor<char>("passTruthFilter")) return false;
    }
    
    if ( !m_trkseltool->accept(track, &primaryVertex ) ) return false;

    return true;
  }

  void TrackSelection::print(std::ostream& os) const {
    os << "Track \n"
       << "    * pT > " << m_ptcut << "\n"
       << "    * |eta| < " << m_etamax;
  }
}
