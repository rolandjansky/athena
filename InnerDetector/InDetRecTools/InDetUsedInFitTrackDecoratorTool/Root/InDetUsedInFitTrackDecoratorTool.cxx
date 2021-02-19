/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "InDetUsedInFitTrackDecoratorTool/InDetUsedInFitTrackDecoratorTool.h"

// FrameWork include(s):
#include "AthLinks/ElementLink.h"
#include "AsgTools/CurrentContext.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/WriteDecorHandle.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// STL include(s):
#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>

// Helper functions, let's put them in a nameless namespace:
namespace {

  std::vector<const xAOD::TrackParticle*> convertLinksToTracks(const std::vector<ElementLink<xAOD::TrackParticleContainer>>& trackLinks) {
    std::vector<const xAOD::TrackParticle*> outTracks;
    outTracks.reserve(trackLinks.size());
    for (const auto& trkLink : trackLinks) {
        if (trkLink.isValid()) {
            outTracks.push_back((const xAOD::TrackParticle*)(*trkLink));
        }
    }
    return outTracks;
  }

  inline int positionInVector(const xAOD::TrackParticle* track, const std::vector<const xAOD::TrackParticle*>& trackVec) {
    auto it = std::find(trackVec.begin(), trackVec.end(), track);
    return ((it != trackVec.end()) ? it - trackVec.begin() : -1);
  }

  // Functions for helping sort 2 vectors according to a comparison using only one of the vectors
  // See: https://stackoverflow.com/a/17074810
  template <typename T, typename Compare>
  std::vector<std::size_t> sort_permutation(const std::vector<T>& vec, const Compare& compare) {
    std::vector<std::size_t> p(vec.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(), [&](std::size_t i, std::size_t j){ return compare(vec[i], vec[j]); });
    return p;
  }

  template <typename T>
  std::vector<T> apply_permutation(const std::vector<T>& vec, const std::vector<std::size_t>& p) {
    std::vector<T> sorted_vec(vec.size());
    std::transform(p.begin(), p.end(), sorted_vec.begin(), [&](std::size_t i){ return vec[i]; });
    return sorted_vec;
  }

} // end: namespace

// Constructor
InDet::InDetUsedInFitTrackDecoratorTool::InDetUsedInFitTrackDecoratorTool(const std::string& name) :
  asg::AsgTool(name),
  m_vtxDecoName("TTVA_AMVFVertices"),
  m_wgtDecoName("TTVA_AMVFWeights"),
  m_trkContKey("InDetTrackParticles"),
  m_vtxContKey("PrimaryVertices")
{
  // Property declarations
  declareProperty("AMVFVerticesDecoName", m_vtxDecoName, "Name of the per-track AMVF vertices decoration");
  declareProperty("AMVFWeightsDecoName",  m_wgtDecoName, "Name of the per-track AMVF weights decoration");
  declareProperty("TrackContainer",       m_trkContKey,  "Name of the track particle container");
  declareProperty("VertexContainer",      m_vtxContKey,  "Name of the primary vertex container");
}

// Destructor
InDet::InDetUsedInFitTrackDecoratorTool::~InDetUsedInFitTrackDecoratorTool()
{}

// Initialize
StatusCode InDet::InDetUsedInFitTrackDecoratorTool::initialize()
{
  // Print configuration
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_MSG_DEBUG("Using AMVFVerticesDecoName: " << m_vtxDecoName);
  ATH_MSG_DEBUG("Using AMVFWeightsDecoName: "  << m_wgtDecoName);
  ATH_MSG_DEBUG("Using TrackContainer: "       << m_trkContKey);
  ATH_MSG_DEBUG("Using VertexContainer: "      << m_vtxContKey);

  // Instantiate and initialize our container reads
  // For tracks:
  ATH_CHECK(m_trkContKey.initialize());
  // For vertices:
  ATH_CHECK(m_vtxContKey.initialize());

  // Instantiate and initialize our decorator writes
  // For vertices:
  m_vtxDecoKey = SG::WriteDecorHandleKey<xAOD::TrackParticleContainer>(m_trkContKey.key() + "." + m_vtxDecoName);
  this->declare(m_vtxDecoKey);
  m_vtxDecoKey.setOwner(&(*this));
  ATH_CHECK(m_vtxDecoKey.initialize());
  // For weights:
  m_wgtDecoKey = SG::WriteDecorHandleKey<xAOD::TrackParticleContainer>(m_trkContKey.key() + "." + m_wgtDecoName);
  this->declare(m_wgtDecoKey);
  m_wgtDecoKey.setOwner(&(*this));
  ATH_CHECK(m_wgtDecoKey.initialize());

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode InDet::InDetUsedInFitTrackDecoratorTool::finalize()
{
  ATH_MSG_DEBUG("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

void InDet::InDetUsedInFitTrackDecoratorTool::decorate(const xAOD::TrackParticleContainer* trkCont, const xAOD::VertexContainer* vtxCont) const
{
  ATH_MSG_DEBUG("In decorate(...) for " << name() <<  "...");

  const EventContext& ctx = Gaudi::Hive::currentContext();

  // Decor handles for the AMVF vertices and weights
  SG::WriteDecorHandle<xAOD::TrackParticleContainer, std::vector<ElementLink<xAOD::VertexContainer>>> vtxDeco(m_vtxDecoKey, ctx);
  SG::WriteDecorHandle<xAOD::TrackParticleContainer, std::vector<float>> wgtDeco(m_wgtDecoKey, ctx);

  // A few declarations
  std::vector<ElementLink<xAOD::VertexContainer>> AMVFVertices;
  std::vector<float> AMVFWeights;
  std::vector<const xAOD::TrackParticle*> fitTracks;
  int posInFitTrks;
  ElementLink<xAOD::VertexContainer> vtxLink;

  // Iterate over our tracks:
  for (const auto& trk : *trkCont) {
    if (!trk) continue;

    // Don't expect more than 5 vertex fits per track
    AMVFVertices.clear();
    AMVFVertices.reserve(5);
    AMVFWeights.clear();
    AMVFWeights.reserve(5);

    // Iterate over our vertices
    for (const auto& vtx : *vtxCont) {
      if (!vtx) continue;

      // Get our fit tracks and weights (inefficient? done for **each** vertex trkCont->size() times)
      fitTracks = ::convertLinksToTracks(vtx->trackParticleLinks());
      posInFitTrks = ::positionInVector(trk, fitTracks);
      if (posInFitTrks != -1) {
        vtxLink.reset();
        vtxLink.toContainedElement(*vtxCont, vtx, ctx);
        AMVFVertices.push_back(vtxLink);
        AMVFWeights.push_back(vtx->trackWeights().at(posInFitTrks));
      }

    } // end, loop over vertices

    // Sort our vertices and weights in order of highest to lowest weight
    std::vector<std::size_t> p = ::sort_permutation(AMVFWeights, [](const float& w1, const float& w2){ return w1 > w2; });
    AMVFVertices = ::apply_permutation(AMVFVertices, p);
    AMVFWeights  = ::apply_permutation(AMVFWeights, p);

    // Actually perform the decoration
    vtxDeco(*trk) = AMVFVertices;
    wgtDeco(*trk) = AMVFWeights;

  } // end, loop over tracks

}

void InDet::InDetUsedInFitTrackDecoratorTool::decorate() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  // Open our track container
  SG::ReadHandle<xAOD::TrackParticleContainer> trkCont(m_trkContKey, ctx);
  if (!trkCont.isValid()) {
    ATH_MSG_WARNING("Unable to retrieve xAOD::TrackParticleContainer, \"" << m_trkContKey.key() << "\", returning without applying decorations!");
    return;
  }

  // Open our vertex container
  SG::ReadHandle<xAOD::VertexContainer> vtxCont(m_vtxContKey, ctx);
  if (!vtxCont.isValid()) {
    ATH_MSG_WARNING("Unable to retrieve xAOD::VertexContainer, \"" << m_vtxContKey.key() << "\", returning without applying decorations!");
    return;
  }

  // Perform the decoration
  decorate(trkCont.get(), vtxCont.get());

}
