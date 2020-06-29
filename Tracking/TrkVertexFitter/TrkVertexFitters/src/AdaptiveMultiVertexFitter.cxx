/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
              AdaptiveMultiVertexFitter.cxx  -  Description in header file
 ***************************************************************************/

#include "TrkVertexFitters/AdaptiveMultiVertexFitter.h"
//
#include "TrkVertexFitterInterfaces/IImpactPoint3dEstimator.h"
#include "TrkVertexFitterInterfaces/IVertexAnnealingMaker.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "TrkVertexFitterInterfaces/IVertexSmoother.h"
#include "TrkVertexFitterInterfaces/IVertexTrackCompatibilityEstimator.h"
#include "TrkVertexFitterInterfaces/IVertexUpdator.h"
//
#include "VxMultiVertex/MVFVxTrackAtVertex.h"
#include "VxMultiVertex/MvfFitInfo.h"
#include "VxMultiVertex/TrackToVtxLink.h"
#include "VxVertex/LinearizedTrack.h"
//
// xAOD Includes
#include "xAODTracking/Vertex.h"
//
// STL
#include <algorithm> //std::find
#include <limits>
#include <numeric> //accumulate, reduce etc

namespace Trk {

AdaptiveMultiVertexFitter::AdaptiveMultiVertexFitter(const std::string& t,
                                                     const std::string& n,
                                                     const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_maxIterations(30)
  , m_maxDistToLinPoint(0.5)
  , m_initialError(0.0001)
  , m_doSmoothing(false)
  , m_minweight(0.001)
  , m_maxRelativeShift(0.01)
  , m_LinearizedTrackFactory("Trk::FullLinearizedTrackFactory")
  , m_TrackCompatibilityEstimator("Trk::Chi2TrackCompatibilityEstimator")
  , m_ImpactPoint3dEstimator(
      "Trk::ImpactPoint3dEstimator/ImpactPoint3dEstimator")
  , m_VertexUpdator("Trk::KalmanVertexUpdator")
  , m_VertexSmoother("Trk::SequentialVertexSmoother")
  , m_AnnealingMaker("Trk::DetAnnealingMaker")
{
  declareProperty("MaxIterations", m_maxIterations);
  declareProperty("MaxDistToLinPoint", m_maxDistToLinPoint);
  declareProperty("InitialError", m_initialError);
  declareProperty("LinearizedTrackFactory", m_LinearizedTrackFactory);
  declareProperty("TrackCompatibilityEstimator", m_TrackCompatibilityEstimator);
  declareProperty("ImpactPoint3dEstimator", m_ImpactPoint3dEstimator);
  declareProperty("VertexUpdator", m_VertexUpdator);
  declareProperty("VertexSmoother", m_VertexSmoother);
  declareProperty("AnnealingMaker", m_AnnealingMaker);
  declareProperty("DoSmoothing", m_doSmoothing);
  declareProperty("MinWeight", m_minweight);
  declareProperty("MaxRelativeShift", m_maxRelativeShift);
  declareInterface<AdaptiveMultiVertexFitter>(this);
}

AdaptiveMultiVertexFitter::~AdaptiveMultiVertexFitter() = default;

StatusCode
AdaptiveMultiVertexFitter::initialize()
{
  ATH_CHECK(m_LinearizedTrackFactory.retrieve());
  ATH_CHECK(m_TrackCompatibilityEstimator.retrieve());
  ATH_CHECK(m_ImpactPoint3dEstimator.retrieve());
  ATH_CHECK(m_VertexUpdator.retrieve());
  // loading smoother in case required
  if (m_doSmoothing)
    ATH_CHECK(m_VertexSmoother.retrieve());
  ATH_CHECK(m_AnnealingMaker.retrieve());
  ATH_MSG_VERBOSE("Initialize successful");
  return StatusCode::SUCCESS;
}

StatusCode
AdaptiveMultiVertexFitter::finalize()
{
  ATH_MSG_VERBOSE("Finalize successful");
  return StatusCode::SUCCESS;
}

void
AdaptiveMultiVertexFitter::fit(std::vector<xAOD::Vertex*>& allVertices) const
{
  // TODO: put this in a better place
  // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
  // For optimization of access speed
  static const xAOD::Vertex::Decorator<MvfFitInfo*> MvfFitInfo("MvfFitInfo");
  static const xAOD::Vertex::Decorator<bool> isInitialized("isInitialized");
  static const xAOD::Vertex::Decorator<std::vector<Trk::VxTrackAtVertex*>> VTAV(
    "VTAV");
  ATH_MSG_DEBUG(" Now fitting all vertices ");
  // create a vector of vertices, to store the old position...
  std::map<xAOD::Vertex*, Amg::Vector3D> oldpositions;
  std::map<xAOD::Vertex*, bool> relinearizations;
  // count number of steps
  int num_steps(0);
  // reset the annealing
  Trk::IVertexAnnealingMaker::AnnealingState astate;
  m_AnnealingMaker->reset(astate);
  ATH_MSG_DEBUG("Entering cycle of AdaptiveMultiVertexFitter");
  bool shiftIsSmall(true);
  // now start to iterate
  do {
    for (const auto& pThisVertex : allVertices) {
      // now store all the "old positions"; if vertex is added for the first
      // time this corresponds to the seed (at the same time fitted vertex will
      // be updated with the constraint information) check if you need to
      // reestimate compatibility + linearization
      ATH_MSG_DEBUG("Now considering candidate with ptr " << pThisVertex);
      relinearizations[pThisVertex] = false;
      if (isInitialized(*pThisVertex)) {
        ATH_MSG_DEBUG("vertex position z: " << (*pThisVertex).position()[2]);
        oldpositions[pThisVertex] = pThisVertex->position();
      } else {
        isInitialized(*pThisVertex) = true;
        ATH_MSG_DEBUG("Candidate has no position so far: using as old position "
                      "the seedVertex");
        if (MvfFitInfo(*pThisVertex)->seedVertex() ==
            nullptr) { // TODO: Is there a way of checking whether a decoration
                       // exists on an object?
          ATH_MSG_ERROR("Candidate has no seed...CRASHING now!!!");
        }
        oldpositions[pThisVertex] = *(MvfFitInfo(*pThisVertex)->seedVertex());
      }
      if (MvfFitInfo(*pThisVertex)->linearizationVertex() ==
          nullptr) { // TODO: Is there a way of checking whether a decoration
                     // exists on an object?
        ATH_MSG_ERROR(
          " Candidate has no linearization point...CRASHING now!!! ");
      }
      if ((oldpositions[pThisVertex] -
           *MvfFitInfo(*pThisVertex)->linearizationVertex())
            .perp() > m_maxDistToLinPoint) {
        ATH_MSG_DEBUG("Candidate has to be relinearized ");
        relinearizations[pThisVertex] = true;
        prepareCompatibility(pThisVertex);
      }
      ATH_MSG_DEBUG("Setting the Vertex to the initial constraint");
      // reput everything to the constraint level
      pThisVertex->setPosition(
        MvfFitInfo(*pThisVertex)->constraintVertex()->position());
      pThisVertex->setCovariancePosition(
        MvfFitInfo(*pThisVertex)->constraintVertex()->covariancePosition());
      pThisVertex->setFitQuality(
        MvfFitInfo(*pThisVertex)->constraintVertex()->chiSquared(),
        MvfFitInfo(*pThisVertex)->constraintVertex()->numberDoF());
      pThisVertex->setCovariancePosition(
        pThisVertex->covariancePosition() * 1. /
        float(m_AnnealingMaker->getWeight(astate, 1.)));
      ATH_MSG_DEBUG("Running TrackCompatibilityEstimator on each track");
      // prepare the iterators for the tracks
      const auto& theseTrackPointersAtVtx = VTAV(*pThisVertex);
      // iterate and update the vertex with the track information
      for (const auto& pThisTrack : theseTrackPointersAtVtx) {
        ATH_MSG_DEBUG("Adding compatibility info to a track of "
                      << theseTrackPointersAtVtx.size());
        // now recover from cases where the linearization position is !=0, but
        // you added more tracks later on...
        if (not pThisTrack->ImpactPoint3dAtaPlane()) {
          const bool success = m_ImpactPoint3dEstimator->addIP3dAtaPlane(
            *pThisTrack, *MvfFitInfo(*pThisVertex)->linearizationVertex());
          if (!success) {
            ATH_MSG_WARNING(
              "Adding compatibility to vertex information failed. Newton "
              "distance finder didn't converge...");
          }
        }
        // first -> estimate the compatibility of the track to the vertex
        m_TrackCompatibilityEstimator->estimate(*pThisTrack,
                                                oldpositions[pThisVertex]);
        ATH_MSG_DEBUG("End of compatibility for a track");
      }
    }
    ATH_MSG_DEBUG("Finished first candidates cycle");
    // after having estimated the compatibility of all the vertices, you have to
    // run again on all vertices, to compute the weights
    for (const auto& pThisVertex : allVertices) {
      // TODO: crude and quite possibly time consuming, but best solution I
      // could think of...
      //      updated VxTrackAtVertices are stored in VTAV decoration:
      //      so each time a vertex is to be updated with its tracks in this
      //      loop, delete VxTrackAtVertex vector and add correctly updated
      //      VxTrackAtVertex (from VTAV) to the vector just before calling
      //      the vertex updator
      std::vector<Trk::VxTrackAtVertex>* tracksOfVertex =
        &(pThisVertex->vxTrackAtVertex());
      tracksOfVertex->clear();
      // prepare the iterators for the tracks
      const auto& theseTrackPointersAtVtx = VTAV(*pThisVertex);
      ATH_MSG_VERBOSE(
        "Beginning lin&update of vertex with pointer: " << pThisVertex);
      for (const auto& pThisTrack : theseTrackPointersAtVtx) {
        // set the weight according to all other track's weight
        ATH_MSG_DEBUG("Calling collect weight for track " << pThisTrack);
        const std::vector<double>& allweights(
          collectWeights(*(static_cast<Trk::MVFVxTrackAtVertex*>(pThisTrack))
                            ->linkToVertices()));
        ATH_MSG_DEBUG("The vtxcompatibility for the track is: "
                      << pThisTrack->vtxCompatibility());
        pThisTrack->setWeight(m_AnnealingMaker->getWeight(
          astate, pThisTrack->vtxCompatibility(), allweights));
        ATH_MSG_DEBUG("The resulting weight for the track is "
                      << m_AnnealingMaker->getWeight(
                           astate, pThisTrack->vtxCompatibility(), allweights));
        if (pThisTrack->weight() > m_minweight) {
          ATH_MSG_DEBUG("check passed");
          // now take care if linearization has been done at least once
          if (not pThisTrack->linState()) {
            // linearization never done so far: do it now!
            ATH_MSG_VERBOSE("Linearizing track for the first time");
            m_LinearizedTrackFactory->linearize(*pThisTrack,
                                                oldpositions[pThisVertex]);
          } else if (relinearizations[pThisVertex]) {
            ATH_MSG_VERBOSE("Relinearizing track ");
            m_LinearizedTrackFactory->linearize(*pThisTrack,
                                                oldpositions[pThisVertex]);
            MvfFitInfo(*pThisVertex)
              ->setLinearizationVertex(
                new Amg::Vector3D(oldpositions[pThisVertex]));
          }
          // now you can proceed with the update
          ATH_MSG_DEBUG("Update of the track "
                        << pThisTrack << " to the vertex " << pThisVertex);
          // TODO: obviously not ideal that I have to do this
          tracksOfVertex->push_back(*pThisTrack);
          // TODO: add() returns an xAOD::Vertex* - is it really ok to just
          // have this line without *iter = m_VertexUpdator->add() ? Must
          // be...
          m_VertexUpdator->add(*pThisVertex, *pThisTrack);
        }
      } // iterator on tracks
      // show some info about the position
      ATH_MSG_DEBUG("Vertex pointer " << pThisVertex << " New position x: "
                                      << pThisVertex->position().x()
                                      << " y: " << pThisVertex->position().y()
                                      << " z: " << pThisVertex->position().z());
    } // iterator on Vertices
    // call now one more step of annealing
    if (!(m_AnnealingMaker->isEquilibrium(astate))) {
      m_AnnealingMaker->anneal(astate);
    }
    // August 2009: sometimes the fitter has not converged when the annealing
    // has finished iterate on all vertex candidates and check whether they moved
    // significantly from last iteration
    shiftIsSmall = true;
    Amg::Vector3D vrtpos;
    for (const auto& pThisVertex : allVertices) {
      vrtpos[0] = oldpositions[pThisVertex][0] - pThisVertex->position()[0];
      vrtpos[1] = oldpositions[pThisVertex][1] - pThisVertex->position()[1];
      vrtpos[2] = oldpositions[pThisVertex][2] - pThisVertex->position()[2];
      AmgSymMatrix(3) weightMatrixVertex;
      weightMatrixVertex = pThisVertex->covariancePosition().inverse();
      double relativeShift = vrtpos.dot(weightMatrixVertex * vrtpos);
      if (relativeShift > m_maxRelativeShift) {
        shiftIsSmall = false;
        break;
      }
    }
    num_steps += 1;
  } while (num_steps < m_maxIterations &&
           (!(m_AnnealingMaker->isEquilibrium(astate)) || !shiftIsSmall));

  if (num_steps >= m_maxIterations) {
    ATH_MSG_DEBUG("Didn't converge fully after " << num_steps);
  }
  ATH_MSG_VERBOSE(
    "In principle the big multivertex fit step is finished now...");
  // TODO: I don't think that I need to fiddle with updating
  // MVFVxTracksAtVertex - vector of VxTrackAtVertex should be available in
  // the usual way now and be enough
  if (m_doSmoothing) {
    auto trackAvailable = [](const xAOD::Vertex* pV) {
      return pV->vxTrackAtVertexAvailable() and
             not(pV->vxTrackAtVertex()).empty();
    };
    for (const auto& pThisVertex : allVertices) {
      if (trackAvailable(pThisVertex)) {
        m_VertexSmoother->smooth(*pThisVertex);
      }
    }
  } else { // TODO: I added this during xAOD migration
    for (const auto& pThisVertex : allVertices) {
      const auto& theseTrackPointersAtVtx = VTAV(*pThisVertex);
      for (const auto& pTrack : theseTrackPointersAtVtx) {
        if (pTrack->initialPerigee())
          pTrack->setPerigeeAtVertex((pTrack->initialPerigee())->clone());
      }
    }
  }
}

std::vector<double>
AdaptiveMultiVertexFitter::collectWeights(
  const Trk::TrackToVtxLink& tracklink) const
{
  ATH_MSG_DEBUG("Collecting weights for tracklink " << &tracklink);
  // TODO: put this in a better place
  // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
  // For optimization of access speed
  static const xAOD::Vertex::Decorator<std::vector<Trk::VxTrackAtVertex*>> VTAV(
    "VTAV");
  const auto& theseVertices = *(tracklink.vertices());
  std::vector<double> myvector;
  myvector.reserve(theseVertices.size());
  for (const auto& pThisVertex : theseVertices) {
    // really stupid, now you have still to find the weight value (right track
    // in this vertex) but if you later remove tracks from candidate, you cannot
    // do much better (maybe update on demand...)
    const auto& trackPointersForThisVertex = VTAV(*pThisVertex);
    for (const auto& pThisTrack : trackPointersForThisVertex) {
      if ((static_cast<Trk::MVFVxTrackAtVertex*>(pThisTrack))
            ->linkToVertices() == &tracklink) {
        ATH_MSG_DEBUG("found one weight: it's : "
                      << pThisTrack->vtxCompatibility() << " for track "
                      << pThisTrack);
        myvector.push_back(pThisTrack->vtxCompatibility());
        break; // gain time in avoiding to look for other tracks if you already
               // found the right one
      }
    }
  }
  return myvector;
}

void
AdaptiveMultiVertexFitter::addVtxToFit(xAOD::Vertex* newvertex) const
{
  ATH_MSG_VERBOSE(" Now entered addVtxToFit ");
  // TODO: put this in a better place
  // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
  // For optimization of access speed
  static const xAOD::Vertex::Decorator<std::vector<Trk::VxTrackAtVertex*>> VTAV(
    "VTAV");

  if (VTAV(*newvertex).empty()) {
    ATH_MSG_ERROR(
      "The candidate you're adding has no tracks: please fix the problem");
  }
  std::vector<xAOD::Vertex*>
    allVerticesToFit;           // how many vertices do you expect?
  allVerticesToFit.reserve(10); // try 10
  prepareCompatibility(newvertex);
  //
  ATH_MSG_VERBOSE("Iterating on tracks");
  std::vector<xAOD::Vertex*> addedVerticesLastIteration;
  std::vector<xAOD::Vertex*> addedVerticesNewIteration;
  addedVerticesLastIteration.push_back(newvertex);
  do {
    for (const auto& vertexIterator : addedVerticesLastIteration) {
      // now you have to check what are the other vertices which still have to
      // do with your new one
      const auto& vertexTracksAtVertex = VTAV(*vertexIterator);
      for (const auto& thisTrack : vertexTracksAtVertex) {
        const auto& pTheseVertices =
          (static_cast<Trk::MVFVxTrackAtVertex*>(thisTrack))
            ->linkToVertices()
            ->vertices();
        for (const auto& thisVertex : *pTheseVertices) {
          // add the vertex if it's still not there
          if (not findAmongVertices(thisVertex, allVerticesToFit)) {
            allVerticesToFit.push_back(thisVertex);
            if (thisVertex != vertexIterator) {
              addedVerticesNewIteration.push_back(thisVertex);
            }
          }
        }
      } // iterate on tracks at considered vertex
    }   // end iteration on addedVerticesLastIteration

    addedVerticesLastIteration = addedVerticesNewIteration;
    addedVerticesNewIteration.clear();
  } while (not addedVerticesLastIteration.empty());
  //
  // now fitting everything together
  fit(allVerticesToFit);
}

void
AdaptiveMultiVertexFitter::prepareCompatibility(xAOD::Vertex* newvertex) const
{
  ATH_MSG_VERBOSE("Entered prepareCompatibility() ");
  // TODO: put this in a better place
  // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
  // For optimization of access speed
  static const xAOD::Vertex::Decorator<MvfFitInfo*> MvfFitInfo("MvfFitInfo");
  static const xAOD::Vertex::Decorator<std::vector<Trk::VxTrackAtVertex*>> VTAV(
    "VTAV");
  const Amg::Vector3D* seedPoint = MvfFitInfo(*newvertex)->seedVertex();
  ATH_MSG_VERBOSE("Now adding compatibility info to the track");
  // lambda adds impact point and 'ANDs' with previous success result
  auto addImpactPoint = [this, seedPoint](const auto& thisVxTrack) {
    return this->m_ImpactPoint3dEstimator->addIP3dAtaPlane(*thisVxTrack,
                                                           *seedPoint);
  };
  const auto& vertexTracksAtVertex = VTAV(*newvertex);
  // std::reduce might be quicker, if compiler implemented it
  const bool success = std::all_of(
    vertexTracksAtVertex.begin(), vertexTracksAtVertex.end(), addImpactPoint);
  if (not success) {
    ATH_MSG_WARNING("Adding compatibility to vertex information failed. Newton "
                    "distance finder didn't converge...");
  }
}

bool
AdaptiveMultiVertexFitter::findAmongVertices(
  const xAOD::Vertex* vertex,
  const std::vector<xAOD::Vertex*>& previousVertices) const
{
  return (std::find(previousVertices.begin(), previousVertices.end(), vertex) !=
          previousVertices.end());
}
}
