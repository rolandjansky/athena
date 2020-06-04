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

    //    declareProperty("TrkSelTool", m_trkseltool);

    //    //std::cout << "In track selection 1" << std::endl;

    /*if(m_trkseltool.empty()) {

      //      //std::cout << "In track selection 1a" << std::endl;

      InDet::InDetTrackSelectionTool *selTool = new InDet::InDetTrackSelectionTool( "TrkSelTool" , m_config->trackQuality());
      top::check(selTool -> initialize(), "Failed to initialize InDetTrackSelectionTool");

      //      //std::cout << "In track selection 1b" << std::endl;
      m_trkseltool = ToolHandle<InDet::IInDetTrackSelectionTool>(selTool);

      //      //std::cout << "In track selection 1c" << std::endl;
      }*/

    // //std::cout << "In track selection 2" << std::endl;

    top::check(m_trkseltool.retrieve(), "Failed to retrieve InDetTrackSelectionTool");

    // //std::cout << "In track selection 3" << std::endl;

  }

  TrackSelection::~TrackSelection() {
  }

  bool TrackSelection::passSelection(const xAOD::TrackParticle& track, const xAOD::Vertex& primaryVertex) {
    if (track.pt() < m_ptcut) return false;

    if (std::fabs(track.eta()) > m_etamax) return false;

    //std::cout << "In track selection 4" << std::endl;

    if ( !m_trkseltool->accept(track, &primaryVertex ) ) return false;

    //std::cout << "In track selection 5" << std::endl;

    return true;
  }

  void TrackSelection::print(std::ostream& os) const {
    os << "Track \n"
       << "    * pT > " << m_ptcut << "\n"
       << "    * |eta| < " << m_etamax;
  }
}
