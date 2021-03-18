/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "InDetHardScatterSelectionTool/InDetHardScatterSelectionTool.h"

// FrameWork include(s):
#include "AsgTools/CurrentContext.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/ReadDecorHandle.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// Tool include(s)
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

// Helper classes
namespace {
  using Sum = InDet::InDetHardScatterSelectionTool::Sum;
  class SumPt2 : public Sum {
  public:
    virtual float add(const float a, const float b) const override final {
      return a + (b * b);
    }
  };
  class SumPt : public Sum {
  public:
    virtual float add(const float a, const float b) const override final {
      return a + b;
    }
  };
} // end: namespace

// Constructor
InDet::InDetHardScatterSelectionTool::InDetHardScatterSelectionTool(const std::string& name) :
  asg::AsgTool(name),
  m_redoHardScatter(false),
  m_mode(InDet::InDetHardScatterSelectionTool::Mode::SumPt2),
  m_trkSelectTool(nullptr),
  m_returnDeco(false),
  m_hardScatterDeco("hardScatterVertexLink"),
  m_vtxContName("PrimaryVertices"),
  m_doTrkSelection(false)
{
  // Property declarations
  declareProperty("RedoHardScatter",     m_redoHardScatter, "If true, redo the hardscatter selection using sumpt2 or sumpt");
  declareProperty("SelectionMode",       m_mode,            "Mode for selecting the hardscatter: sumpt2 or sumpt");
  declareProperty("TrackSelectionTool",  m_trkSelectTool,   "InDetTrackSelectionTool, to optionally select on a vertex's used-in-fit tracks");
  declareProperty("ReturnDeco",          m_returnDeco,      "If true, return the hardscatter as the vertex decorated with some boolean");
  declareProperty("HardScatterLinkDeco", m_hardScatterDeco, "The decoration name of the ElementLink to the hardscatter vertex (applied to xAOD::EventInfo)");
  declareProperty("VertexContainer",     m_vtxContName,     "Name of the primary vertex container");
}

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
  ATH_MSG_DEBUG("Using HardScatterLinkDeco: " << m_hardScatterDeco);
  ATH_MSG_DEBUG("Using VertexContainer: "     << m_vtxContName);

  if (m_redoHardScatter && m_returnDeco) {
    ATH_MSG_ERROR("RedoHardScatter and ReturnDeco cannot both be set to true.");
    return StatusCode::FAILURE;
  }

  // If we are re-doing the hardscatter
  if (m_redoHardScatter) {

    // Determine how we will sum the track pt
    switch (m_mode) {
      case Mode::SumPt2: {
        m_sum = std::unique_ptr<::SumPt2>(new ::SumPt2());
        break;
      }
      case Mode::SumPt: {
        m_sum = std::unique_ptr<::SumPt>(new ::SumPt());
        break;
      }
      default: {
        ATH_MSG_ERROR("Unknown running mode: "  << m_mode);
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
  m_hardScatterDecoKey = SG::ReadDecorHandleKey<xAOD::EventInfo>(m_hardScatterDeco);
  ATH_CHECK(m_hardScatterDecoKey.initialize());

  // Initialize our vertex container read
  m_vtxContKey = SG::ReadHandleKey<xAOD::VertexContainer>(m_vtxContName);
  ATH_CHECK(m_vtxContKey.initialize());

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
    SG::ReadDecorHandle<xAOD::EventInfo, ElementLink<xAOD::VertexContainer>> hardScatterDeco(m_hardScatterDeco, ctx);
    ElementLink<xAOD::VertexContainer> vtxLink = hardScatterDeco(*evtInfo);
    if (!vtxLink.isValid()) {
      ATH_MSG_ERROR("Hardscatter vertex link is not valid, returning nullptr!");
      return nullptr;
    }
    return *vtxLink;
  }

  float sum;
  float maxsum = -999.0;
  const xAOD::Vertex* hardscatter = nullptr;
  for (const auto& vtx : *vtxCont) {
    // If we are redoing the hardscatter determination:
    if (m_redoHardScatter) {
      sum = 0.;
      for (const auto& trkLink : vtx->trackParticleLinks()) {
        if (trkLink.isValid()) {
          if (m_doTrkSelection && !static_cast<bool>(m_trkSelectTool->accept(**trkLink, vtx))) continue;
          sum = m_sum->add(sum, (*trkLink)->pt());
        }
      }
      if (sum > maxsum) {
        maxsum = sum;
        hardscatter = vtx;
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
    return ElementLink<xAOD::VertexContainer>();
  }
  return getHardScatterLink(vtxCont.get());
}
