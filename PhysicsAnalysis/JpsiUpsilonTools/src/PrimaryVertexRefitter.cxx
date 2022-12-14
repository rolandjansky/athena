/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// PrimaryVertexRefitter
// James Catmore <James.Catmore@cern.ch> 
// Evelina Bouhova-Thacker <e.bouhova@cern.ch> 
// Returns a refitted primary vertex having removed requested tracks
// ----------------------------------------------------------------------------
// ****************************************************************************

#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

namespace Analysis {
 
StatusCode PrimaryVertexRefitter::initialize() {

  CHECK( m_trackToVertexIPEstimator.retrieve() );

  ATH_MSG_DEBUG("Initialize successful");
  
  return StatusCode::SUCCESS;

}

PrimaryVertexRefitter::PrimaryVertexRefitter(const std::string& t, const std::string& n, const IInterface* p)  : 
AthAlgTool(t,n,p)
{
  declareInterface<PrimaryVertexRefitter>(this);
}

PrimaryVertexRefitter::~PrimaryVertexRefitter() { }

// -------------------------------------------------------------------------------------------------
xAOD::Vertex* PrimaryVertexRefitter::refitVertex(const xAOD::Vertex* vertex, const xAOD::Vertex* excludeVertex, bool returnCopy, int* exitcode) const
{

  if (vertex == 0) {
    ATH_MSG_DEBUG("Empty original xAOD::Vertex pointer passed: returning 0");
    if(exitcode) *exitcode = -1;
    return 0;
  }
  unsigned int ntrk_pv = vertex->nTrackParticles();
  if (ntrk_pv == 0) {
    ATH_MSG_DEBUG("Input vertex has no associated tracks: returning 0");
    if(exitcode) *exitcode =-2;
    return 0;
  }
  if (excludeVertex == 0) {
    ATH_MSG_DEBUG("Empty exclude xAOD::Vertex pointer passed: returning original vertex");
    if(exitcode) *exitcode = 10;
    return returnCopy ? new xAOD::Vertex(*vertex) : nullptr;
  }
  std::vector<const xAOD::TrackParticle*> tps; tps.clear();
  unsigned int ntrk = excludeVertex->nTrackParticles();
  if (ntrk == 0) {
    ATH_MSG_DEBUG("Exclude vertex has no associated tracks: returning original vertex");
    if(exitcode) *exitcode = 11;
    return returnCopy ? new xAOD::Vertex(*vertex) : nullptr;
  }

  for (unsigned int i = 0; i < ntrk ; ++i) {
    const xAOD::TrackParticle* tp = excludeVertex->trackParticle(i);
    if (tp==0) continue;
    tps.push_back(tp);
  }
  return refitVertex(vertex,tps, returnCopy, exitcode);
}

// -------------------------------------------------------------------------------------------------
xAOD::Vertex* PrimaryVertexRefitter::refitVertex(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*> &tps, bool returnCopy, int* exitcode) const
{

  if (vertex == 0) {
    ATH_MSG_DEBUG("Empty original xAOD::Vertex pointer passed: returning 0");
    if(exitcode) *exitcode =-1;
    return 0;
  }
  unsigned int ntrk_pv = vertex->nTrackParticles();
  if (ntrk_pv == 0) {
    ATH_MSG_DEBUG("Input vertex has no associated tracks: returning 0");
    if(exitcode) *exitcode = -2;
    return 0;
  }
  if (ntrk_pv <= m_ntrk_min) {
    ATH_MSG_DEBUG("The number of tracks " << ntrk_pv << " in the original vertex is already <= the allowed minimum number of tracks " << m_ntrk_min << ", returning original vertex");
    if(exitcode) *exitcode = 2;
    return returnCopy ? new xAOD::Vertex(*vertex) : nullptr;
  }
  unsigned int ntrk = tps.size();
  if (ntrk == 0) {
    ATH_MSG_DEBUG("No tracks requested to be removed: returning original vertex");
    if(exitcode) *exitcode = 3;
    return returnCopy ? new xAOD::Vertex(*vertex) : nullptr;
  }

  xAOD::Vertex* reducedVertex(0);
  const xAOD::Vertex* tmpVert = vertex;
  std::vector <const xAOD::TrackParticle*>::const_iterator pb = tps.begin();
  std::vector <const xAOD::TrackParticle*>::const_iterator pe = tps.end();
  for (;pb!=pe;++pb) {
    const xAOD::TrackParticle* tp = (*pb);
    reducedVertex = m_trackToVertexIPEstimator->getUnbiasedVertex(tp,tmpVert);
    if (tmpVert != vertex) delete tmpVert;
    if (reducedVertex == 0) {
      ATH_MSG_DEBUG("Refit failed: returning original vertex");
      if(exitcode) *exitcode = -4;
      return returnCopy ? new xAOD::Vertex(*vertex) : nullptr;
    }
    tmpVert = reducedVertex;
    if (tmpVert->nTrackParticles() <= m_ntrk_min-1) {
      ATH_MSG_DEBUG("The number of tracks in the refitted vertex would be less than the allowed minimum number of tracks: " << m_ntrk_min << ", returning original vertex");
      delete reducedVertex;
      if(exitcode) *exitcode = 5;
      return returnCopy ? new xAOD::Vertex(*vertex) : nullptr;
    }
  }
  if(exitcode) *exitcode = 1;
  return reducedVertex;
}

}
