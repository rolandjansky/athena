/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// VertexThinning.cxx, (c) ATLAS Detector software
// Keeps only the vertex with the highest track sum pT2
///////////////////////////////////////////////////////////////////

// My header
#include "VertexThinning.h"

// Members
#include "AthenaKernel/IThinningSvc.h"

// EDM includes
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"

// STL includes
#include <vector>
#include <string>

// Constructor
DerivationFramework::VertexThinning::VertexThinning(const std::string& t,
                                                    const std::string& n,
                                                    const IInterface* p )
  : AthAlgTool(t,n,p)
  , m_thinningSvc("ThinningSvc",n)
  , m_vertexSGKey("PrimaryVertices")
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("ThinningService", m_thinningSvc);
  declareProperty("VertexKey", m_vertexSGKey, "Vertex collection for thinning");
}

// Destructor
DerivationFramework::VertexThinning::~VertexThinning() {
}

// The thinning itself
StatusCode DerivationFramework::VertexThinning::doThinning() const
{
  const xAOD::VertexContainer* primaryVertices(nullptr);
  ATH_CHECK( evtStore()->retrieve( primaryVertices, m_vertexSGKey ) );

  // Set up a mask for the vertices
  std::vector<bool> mask(primaryVertices->size(),false);

  int keep_index = -1;
  double pT2 = -1.;
  for(size_t n_v=0;n_v<primaryVertices->size();++n_v){
    const xAOD::Vertex* vtx = primaryVertices->at(n_v);
    if( vtx->vertexType() != xAOD::VxType::PriVtx ) continue;
    double my_pT2 = 0.;
    for (size_t n_t=0;n_t<vtx->nTrackParticles();++n_t){
      if (!vtx->trackParticle(n_t)) continue; // null ptr protection
      my_pT2 += vtx->trackParticle(n_t)->pt()*vtx->trackParticle(n_t)->pt();
    }
    if (my_pT2>pT2){
      // Our new highest sum pT2 vertex
      keep_index = n_v;
      pT2 = my_pT2;
    }
  }
  if (keep_index<0 && primaryVertices->size()>0){
    ATH_MSG_WARNING("Did not find a primary vertex in the " << primaryVertices->size() << " vertices in " << m_vertexSGKey);
  } else {
    mask[keep_index] = true;
  }

  ATH_CHECK( m_thinningSvc->filter(*primaryVertices, mask, IThinningSvc::Operator::And) );
  return StatusCode::SUCCESS;
}

