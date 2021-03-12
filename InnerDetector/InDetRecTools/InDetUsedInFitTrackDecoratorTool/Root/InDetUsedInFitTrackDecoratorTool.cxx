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

  //vector to collect results
  std::vector<std::pair<const xAOD::Vertex*,float> > vxWithWeight;

  // Iterate over our tracks:
  for (const xAOD::TrackParticle*  trk : *trkCont) {
    if (!trk) continue;
    
    vxWithWeight.clear();

    // Iterate over our vertices
    for (const auto vtx : *vtxCont) {
      if (!vtx) continue;

      //Search for vertex linked to this track
      const auto& trkLinks=vtx->trackParticleLinks();
      const size_t nTrackLinks=trkLinks.size();
      for (unsigned i=0;i<nTrackLinks;++i) {
        if (trkLinks[i].isValid() && *(trkLinks[i]) == trk) {//ptr comparison
          vxWithWeight.emplace_back(vtx,vtx->trackWeights()[i]);
          break; //Found pointer, quit loop
        }
      }
    }//end loop over vertices
    
    //sort by weight
    std::sort(vxWithWeight.begin(),vxWithWeight.end(),
        [](std::pair<const xAOD::Vertex*,float>& a, std::pair<const xAOD::Vertex*,float>& b){ return a.second > b.second; } );
  
    //split vector of pairs into two vectors in sync:
    
    std::vector<ElementLink<xAOD::VertexContainer>> AMVFVertices;
    std::vector<float> AMVFWeights;
    AMVFVertices.reserve(vxWithWeight.size());
    AMVFWeights.reserve(vxWithWeight.size());

    for (const auto& p : vxWithWeight) {
      AMVFVertices.emplace_back(p.first,*vtxCont,ctx);
      AMVFWeights.emplace_back(p.second);
    }

    // Actually perform the decoration
    vtxDeco(*trk) = std::move(AMVFVertices);
    wgtDeco(*trk) = std::move(AMVFWeights);

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
