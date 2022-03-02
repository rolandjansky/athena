/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include <memory>

// FrameWork include(s):
#include "AsgTools/CurrentContext.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/ReadDecorHandle.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// Tool include(s)
#include "InDetHardScatterSelectionTool/InDetHardScatterSelectionTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "FourMomUtils/xAODP4Helpers.h"
// Helper classes
namespace {
  using Sum = InDet::InDetHardScatterSelectionTool::Sum;
  class SumPt2 : public Sum {
  public:
    virtual float add(const float a, const float b, const float, const float) const override final {
      return a + (b * b);
    }
  };
  class SumPt : public Sum {
  public:
    virtual float add(const float a, const float b, const float, const float ) const override final {
      return a + b;
    }
  };
  class SumPtw : public Sum {
  public:
    virtual float add(const float a, const float b, const float jet_pt, const float mindR ) const override final {
      return a + (b * b * jet_pt *jet_pt )/(mindR+0.0001);
    }
  };
} // end: namespace

// Constructor
InDet::InDetHardScatterSelectionTool::InDetHardScatterSelectionTool(const std::string& name) :
  asg::AsgTool(name)
{}

// Destructor
InDet::InDetHardScatterSelectionTool::~InDetHardScatterSelectionTool()
{}

// Initialize
StatusCode InDet::InDetHardScatterSelectionTool::initialize()
{
  // Print configuration
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_MSG_DEBUG("Using RedoHardScatter: "     << m_redoHardScatter);
  ATH_MSG_DEBUG("Using SelectionMode: "       << m_mode);
  ATH_MSG_DEBUG("Using TrackSelectionTool: "  << m_trkSelectTool);
  ATH_MSG_DEBUG("Using ReturnDeco: "          << m_returnDeco);
  ATH_MSG_DEBUG("Using HardScatterLinkDeco: " << m_hardScatterDeco.toString());
  ATH_MSG_DEBUG("Using VertexContainer: "     << m_vtxContKey);
  ATH_MSG_DEBUG("Using JetContainer: "        << m_jetContKey);

  if (m_redoHardScatter && m_returnDeco) {
    ATH_MSG_ERROR("RedoHardScatter and ReturnDeco cannot both be set to true.");
    return StatusCode::FAILURE;
  }

  // If we are re-doing the hardscatter
  if (m_redoHardScatter) {
    ATH_MSG_DEBUG("Setting the hard scatter strategy to: " << m_mode);

    // Determine how we will sum the track pt
    switch (m_mode) {
      case InDet::InDetHardScatterSelectionTool::Mode::SumPt2: {
        m_sum = std::make_unique<::SumPt2>();
        break;
      }
      case InDet::InDetHardScatterSelectionTool::Mode::SumPt: {
        m_sum = std::make_unique<::SumPt>();
        break;
      }
      case InDet::InDetHardScatterSelectionTool::Mode::SumPtw: {
        m_sum = std::make_unique<::SumPtw>();
        break;
      }
      default: {
        ATH_MSG_ERROR("Unknown running mode  : "  << m_mode);
        return StatusCode::FAILURE;
      }
    }

    // Fetch our InDetTrackSelectionTool
    if (m_redoHardScatter && !m_trkSelectTool.empty()) {
      m_doTrkSelection = true;
      ATH_CHECK(m_trkSelectTool.retrieve());
    }

  }

  // If we are returning a decoration
  else if (m_returnDeco) {
    if (m_hardScatterDeco.empty()) {
      ATH_MSG_ERROR("Hardscatter vertex decoration cannot be empty!");
      return StatusCode::FAILURE;
    }
  }

  // Initialize our EventInfo container and decoration reads
  ATH_CHECK(m_evtInfoKey.initialize());
  m_hardScatterDecoKey = SG::ReadDecorHandleKey<xAOD::EventInfo>(m_hardScatterDeco.toString());
  ATH_CHECK(m_hardScatterDecoKey.initialize());

  // Initialize our vertex container read
  ATH_CHECK(m_vtxContKey.initialize());

  // Initialize our jet container read if it is non empty
  ATH_CHECK(m_jetContKey.initialize(!m_jetContKey.empty()));

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode InDet::InDetHardScatterSelectionTool::finalize()
{
  ATH_MSG_DEBUG("Finalizing " << name() << "...");

  // Release our tool
  if (m_redoHardScatter && !m_trkSelectTool.empty()) ATH_CHECK(m_trkSelectTool.release());

  return StatusCode::SUCCESS;
}

const xAOD::Vertex* InDet::InDetHardScatterSelectionTool::getHardScatter(const xAOD::VertexContainer* vtxCont) const
{
  ATH_MSG_DEBUG("In getHardScatter(...) for " << name() <<  "...");

  const EventContext& ctx = Gaudi::Hive::currentContext();

  // If we are simply returning a decoration on the EventInfo:
  if (m_returnDeco) {
    SG::ReadHandle<xAOD::EventInfo> evtInfo(m_evtInfoKey, ctx);
    if (!evtInfo.isValid()) {
      ATH_MSG_ERROR("Could not open event info, returning nullptr!");
      return nullptr;
    }
    SG::ReadDecorHandle<xAOD::EventInfo, ElementLink<xAOD::VertexContainer>> hardScatterDeco(m_hardScatterDecoKey, ctx);
    ElementLink<xAOD::VertexContainer> vtxLink = hardScatterDeco(*evtInfo);
    if (!vtxLink.isValid()) {
      ATH_MSG_ERROR("Hardscatter vertex link is not valid, returning nullptr!");
      return nullptr;
    }
    return *vtxLink;
  }
  //::get the jet container if the mode of HS selection is sumptw
  const xAOD::JetContainer* jetCont{nullptr};
  if ( m_mode == InDet::InDetHardScatterSelectionTool::Mode::SumPtw ){
    //::First get the jet containers
    SG::ReadHandle<xAOD::JetContainer> jetContHandle(m_jetContKey, ctx);
    if (!jetContHandle.isValid()){  
      ATH_MSG_ERROR ("HardScatterSelectionTool configured to use jet collection "<<m_jetContKey.key()<<", but collection is not found!"); 
    }
    jetCont = jetContHandle.cptr(); 
  }

  float maxsum = -999.0;
  const xAOD::Vertex* hardscatter = nullptr;
  for (const auto& vtx : *vtxCont) {
    float sum = 0.;
    // If we are redoing the hardscatter determination:
    if (m_redoHardScatter) {
      if ( m_mode == InDet::InDetHardScatterSelectionTool::Mode::SumPtw ){
        for (const auto& trkLink : vtx->trackParticleLinks()) {
          if (!trkLink.isValid()) continue;
          if (m_doTrkSelection && !static_cast<bool>(m_trkSelectTool->accept(**trkLink, vtx))) continue;
          if ( (*trkLink)->pt() >m_trkMaxPt) continue;
          float jetPt=0.0;
          float minDr=10.0;

          for (const xAOD::Jet* jet : *jetCont) {
            if (jet->pt() < m_jetMinPt) {continue;} //skip jets below pT threshold
            float DR = xAOD::P4Helpers::deltaR(*trkLink, jet);
            if(DR<minDr) { 
              minDr = DR;
              jetPt = jet->pt();
            }
          }
          if(minDr <= m_jetTrkMaxDr){//check if jet is within dR 1.
            sum = m_sum->add(sum, (*trkLink)->pt(),jetPt,minDr );
          }
        }
        if (sum > maxsum) {
          maxsum = sum;
          hardscatter = vtx;
        }
      }  
      //::keeping Matt's (old) method for sumpt & sumpt2
      else {
        for (const auto& trkLink : vtx->trackParticleLinks()) {
          if (trkLink.isValid()) {
            if (m_doTrkSelection && !static_cast<bool>(m_trkSelectTool->accept(**trkLink, vtx))) continue;
            sum = m_sum->add(sum, (*trkLink)->pt(),0.,0.);
          }
        }
        if (sum > maxsum) {
          maxsum = sum;
          hardscatter = vtx;
        }
      } 
    }
    // Else, return the PV:
    else {
      if (vtx->vertexType() == xAOD::VxType::PriVtx) {
        hardscatter = vtx;
        break;
      }
    }
  }

  return hardscatter;
}

const xAOD::Vertex* InDet::InDetHardScatterSelectionTool::getHardScatter() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<xAOD::VertexContainer> vtxCont(m_vtxContKey, ctx);
  if (!vtxCont.isValid()) {
    ATH_MSG_ERROR("Could not open primary vertex container, returning nullptr!");
    return nullptr;
  }
  return getHardScatter(vtxCont.get());
}

ElementLink<xAOD::VertexContainer> InDet::InDetHardScatterSelectionTool::getHardScatterLink(const xAOD::VertexContainer* vtxCont) const
{
  ATH_MSG_DEBUG("In getHardScatterLink(...) for " << name() <<  "...");

  const EventContext& ctx = Gaudi::Hive::currentContext();

  // Get our hardscatter vertex
  const xAOD::Vertex* hardscatter = getHardScatter(vtxCont);

  // Create our element link
  ElementLink<xAOD::VertexContainer> hardscatterLink;
  if (hardscatter) {
    hardscatterLink.toContainedElement(*vtxCont, hardscatter, ctx);
  }

  return hardscatterLink;
}

ElementLink<xAOD::VertexContainer> InDet::InDetHardScatterSelectionTool::getHardScatterLink() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<xAOD::VertexContainer> vtxCont(m_vtxContKey, ctx);
  if (!vtxCont.isValid()) {
    ATH_MSG_ERROR("Could not open primary vertex container, returning empty ElementLink!");
    return {};
  }
  return getHardScatterLink(vtxCont.get());
}
