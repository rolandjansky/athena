/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "VxVertex/ExtendedVxCandidate.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "TrkVertexFitterInterfaces/IVertexUpdator.h"
#include "Particle/TrackParticle.h"
#include "GaudiKernel/ToolFactory.h"
#include "DataModel/ElementLink.h"

namespace Analysis {
 
StatusCode PrimaryVertexRefitter::initialize() {

  // uploading the V0 tools
  if ( m_V0Tools.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_V0Tools);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_V0Tools);
  }

  // updator 
  if ( m_Updator.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Failed to retrieve tool " << m_Updator);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_Updator);
  }

  // linearized track factory
  if ( m_linFactory.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Failed to retrieve tool " << m_linFactory);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_linFactory);
  }

  ATH_MSG_INFO("Initialize successful");
  
  return StatusCode::SUCCESS;

}

StatusCode PrimaryVertexRefitter::finalize() {

  ATH_MSG_INFO("Finalize successful");
  return StatusCode::SUCCESS;

}

PrimaryVertexRefitter::PrimaryVertexRefitter(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
   m_V0Tools("Trk::V0Tools"),
   m_linFactory("Trk::FullLinearizedTrackFactory"), 
   m_Updator("Trk::KalmanVertexUpdator")            

{
  declareInterface<PrimaryVertexRefitter>(this);
  declareProperty("V0Tools", m_V0Tools);
  declareProperty("LinearizedTrackFactory", m_linFactory);  
  declareProperty("VertexUpdator", m_Updator);              

}

PrimaryVertexRefitter::~PrimaryVertexRefitter() { }

// -------------------------------------------------------------------------------------------------
Trk::VxCandidate* PrimaryVertexRefitter::refitVertex(Trk::VxCandidate* vertex, Trk::VxCandidate* excludeVertex) {

  std::vector<Trk::VxTrackAtVertex*>* tracksAtVertex(vertex->vxTrackAtVertex());
  if (tracksAtVertex->size() == 0) {
    ATH_MSG_DEBUG("Input vertex has no associated tracks: cannot remove any");
    return vertex;
  }
  std::vector<const Rec::TrackParticle*> tps; tps.clear();
  int ntrk = excludeVertex->vxTrackAtVertex()->size();
  if (ntrk == 0) {
    ATH_MSG_DEBUG("Exclude vertex has no associated tracks: returning input vertex");
    return vertex;
  }
  for (int i = 0; i < ntrk ; ++i) {
    const Trk::TrackParticleBase* pb = m_V0Tools->origTrackPB(excludeVertex,i);
    if (pb==0) continue;
    const Rec::TrackParticle* tp = dynamic_cast<const Rec::TrackParticle*>(pb);
    if (tp==0) continue;
    tps.push_back(tp);
  }
  return refitVertex(vertex,tps);
}

// -------------------------------------------------------------------------------------------------
Trk::VxCandidate* PrimaryVertexRefitter::refitVertex(Trk::VxCandidate* vertex, std::vector<const Rec::TrackParticle*> tps) {

  std::vector<Trk::VxTrackAtVertex*>* tracksAtVertex(vertex->vxTrackAtVertex());
  if (tracksAtVertex->size() == 0) {
    ATH_MSG_DEBUG("Input vertex has no associated tracks: cannot remove any");
    return vertex;
  }
  Trk::VxCandidate* clonedVertex = new Trk::VxCandidate(*vertex);

  int ntrk = tracksAtVertex->size();
  Trk::VxCandidate* reducedVertex = NULL;
  bool first = true;
  unsigned int i=0;
  std::vector<Trk::VxTrackAtVertex*>::iterator lintracksBegin  = tracksAtVertex->begin();
  std::vector<Trk::VxTrackAtVertex*>::iterator lintracksEnd    = tracksAtVertex->end();
  for (std::vector<Trk::VxTrackAtVertex*>::iterator iter=lintracksBegin;iter!=lintracksEnd;++iter) {
    const Trk::TrackParticleBase* tmpPB = m_V0Tools->origTrackPB(vertex,i);
    if (tmpPB==0) continue;
    const Rec::TrackParticle* tmpTP = dynamic_cast<const Rec::TrackParticle*>(tmpPB);
    if (tmpTP==0) continue;

    std::vector <const Rec::TrackParticle*>::const_iterator pb = tps.begin();
    std::vector <const Rec::TrackParticle*>::const_iterator pe = tps.end();
    for(;pb!=pe;++pb) {
      if (tmpTP == *pb) {
        if (ntrk > 2) {
          if(!(*iter)->linState()) m_linFactory->linearize( **iter, clonedVertex->recVertex());
          reducedVertex = m_Updator->remove(*clonedVertex,**iter);
          ntrk--;
          first = false;
        } else {
          ATH_MSG_DEBUG("cannot remove all tracks - there won't be enough tracks left to fit: returning input vertex");
          delete clonedVertex;
          return vertex;
        }
      }
    } // loop over tracks to be excluded
    i++;
  } // loop over tracks in PV

  if (first) {
    ATH_MSG_DEBUG("No refit attempted: returning input vertex");
    delete clonedVertex;
    return vertex;
  }
  return reducedVertex;
}

}
