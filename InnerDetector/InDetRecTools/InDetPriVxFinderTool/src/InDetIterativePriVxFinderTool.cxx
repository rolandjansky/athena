/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/***************************************************************************
                         InDetIterativePriVxFinderTool.cxx  -  Description
                             -------------------
    begin   : 23-02-2010
    authors : Giacinto Piacquadio (CERN PH-ADE-ID)
    changes :
              2016-04-26   David Shope <david.richard.shope@cern.ch>
              EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex
              findVertex will now always return an xAOD::VertexContainer,
              even when using a TrackCollection.
              as input.
***************************************************************************/
#include "InDetPriVxFinderTool/InDetIterativePriVxFinderTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"

#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrkVertexFitterInterfaces/IImpactPoint3dEstimator.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"

#include "AthContainers/DataVector.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrackLink/ITrackLink.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexContainer.h"

#include <utility>
#include <vector>

namespace InDet {
InDetIterativePriVxFinderTool::InDetIterativePriVxFinderTool(
  const std::string& t,
  const std::string& n,
  const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_iVertexFitter("Trk::AdaptiveVertexFitter")
  , m_trkFilter("InDet::InDetTrackSelection")
  , m_SeedFinder("Trk::ZScanSeedFinder")
  , m_ImpactPoint3dEstimator("Trk::ImpactPoint3dEstimator")
  , m_useBeamConstraint(false)
  , m_significanceCutSeeding(10)
  , m_maximumChi2cutForSeeding(6. * 6.)
  , m_maxVertices(25)
  , m_createSplitVertices(false)
  , m_splitVerticesTrkInvFraction(2)
  , m_reassignTracksAfterFirstFit(false)
  , m_doMaxTracksCut(false)
  , m_maxTracks(5000)
{
  declareInterface<IVertexFinder>(this);

  declareProperty("VertexFitterTool", m_iVertexFitter);
  declareProperty("TrackSelector", m_trkFilter);
  declareProperty("SeedFinder", m_SeedFinder);
  declareProperty("ImpactPoint3dEstimator", m_ImpactPoint3dEstimator);
  declareProperty("LinearizedTrackFactory", m_LinearizedTrackFactory);
  declareProperty("useBeamConstraint", m_useBeamConstraint);
  declareProperty("significanceCutSeeding", m_significanceCutSeeding);
  declareProperty("maximumChi2cutForSeeding", m_maximumChi2cutForSeeding);
  declareProperty("maxVertices", m_maxVertices);
  declareProperty("createSplitVertices", m_createSplitVertices);
  declareProperty("splitVerticesTrkInvFraction",
                  m_splitVerticesTrkInvFraction,
                  "inverse fraction to split tracks (1:N)");
  declareProperty("reassignTracksAfterFirstFit", m_reassignTracksAfterFirstFit);
  declareProperty("doMaxTracksCut", m_doMaxTracksCut);
  declareProperty("MaxTracks", m_maxTracks);
}

InDetIterativePriVxFinderTool::~InDetIterativePriVxFinderTool() = default;

StatusCode
InDetIterativePriVxFinderTool::initialize()
{
  if (m_createSplitVertices && m_useBeamConstraint) {
    ATH_MSG_FATAL(" Split vertices cannot be obtained if beam spot constraint "
                  "is true! Change settings...");
    return StatusCode::FAILURE;
  }

  /* Get the right vertex fitting tool from ToolSvc */
  if (m_iVertexFitter.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_iVertexFitter);
    return StatusCode::FAILURE;
  }

  if (m_SeedFinder.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_SeedFinder);
    return StatusCode::FAILURE;
  }

  if (m_LinearizedTrackFactory.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_LinearizedTrackFactory);
    return StatusCode::FAILURE;
  }

  if (m_ImpactPoint3dEstimator.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_ImpactPoint3dEstimator);
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_beamSpotKey.initialize());

  if (m_trkFilter.retrieve().isFailure()) {
    ATH_MSG_ERROR(" Unable to retrieve " << m_trkFilter);
    return StatusCode::FAILURE;
  }

  // since some parameters special to an inherited class this method
  // will be overloaded by the inherited class
  printParameterSettings();

  return StatusCode::SUCCESS;
}

namespace {
struct xAODVertex_pair
{
  double first;
  xAOD::Vertex* second;
  xAODVertex_pair(double p1, xAOD::Vertex* p2)
    : first(p1)
    , second(p2)
  {}
  bool operator<(const xAODVertex_pair& other) const
  {
    return first > other.first;
  }
};
} // anonymous namespace

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
InDetIterativePriVxFinderTool::findVertex(const EventContext& ctx,
                                          const TrackCollection* trackTES) const
{

  ATH_MSG_DEBUG(
    " Number of input tracks before track selection: " << trackTES->size());

  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{ m_beamSpotKey, ctx };
  const InDet::BeamSpotData* beamSpot = *beamSpotHandle;

  std::vector<Trk::ITrackLink*> selectedTracks;

  typedef DataVector<Trk::Track>::const_iterator TrackDataVecIter;

  bool selectionPassed;
  for (TrackDataVecIter itr = (*trackTES).begin(); itr != (*trackTES).end();
       itr++) {

    if (m_useBeamConstraint && beamSpot != nullptr) {
      Trk::RecVertex beamPosition{ beamSpot->beamVtx() };
      selectionPassed =
        static_cast<bool>(m_trkFilter->accept(**itr, &beamPosition));
    } else {
      Trk::Vertex null(Amg::Vector3D(0, 0, 0));
      selectionPassed = static_cast<bool>(m_trkFilter->accept(**itr, &null));
    }
    if (selectionPassed) {
      ElementLink<TrackCollection> link;
      link.setElement(*itr);
      Trk::LinkToTrack* linkTT = new Trk::LinkToTrack(link);
      linkTT->setStorableObject(*trackTES);
      selectedTracks.push_back(linkTT);
    }
  }

  ATH_MSG_DEBUG("Of " << trackTES->size() << " tracks " << selectedTracks.size()
                      << " survived the preselection.");

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
    returnContainers = findVertex(ctx,selectedTracks);

  return returnContainers;
}

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
InDetIterativePriVxFinderTool::findVertex(
  const EventContext& ctx,
  const xAOD::TrackParticleContainer* trackParticles) const
{
  ATH_MSG_DEBUG(" Number of input tracks before track selection: "
                << trackParticles->size());

  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{ m_beamSpotKey, ctx };
  const InDet::BeamSpotData* beamSpot = *beamSpotHandle;

  std::vector<Trk::ITrackLink*> selectedTracks;

  typedef DataVector<xAOD::TrackParticle>::const_iterator
    TrackParticleDataVecIter;

  bool selectionPassed;
  for (TrackParticleDataVecIter itr = trackParticles->begin();
       itr != trackParticles->end();
       ++itr) {

    if (m_useBeamConstraint && beamSpot != nullptr) {
      xAOD::Vertex beamPosition;
      beamPosition.makePrivateStore();
      beamPosition.setPosition(beamSpot->beamVtx().position());
      beamPosition.setCovariancePosition(
        beamSpot->beamVtx().covariancePosition());
      selectionPassed =
        static_cast<bool>(m_trkFilter->accept(**itr, &beamPosition));
    } else {

      xAOD::Vertex null;
      null.makePrivateStore();
      null.setPosition(Amg::Vector3D(0, 0, 0));
      AmgSymMatrix(3) vertexError;
      vertexError.setZero();
      null.setCovariancePosition(vertexError);
      selectionPassed = static_cast<bool>(m_trkFilter->accept(**itr, &null));
    }

    if (selectionPassed) {
      ElementLink<xAOD::TrackParticleContainer> link;
      link.setElement(*itr);
      Trk::LinkToXAODTrackParticle* linkTT =
        new Trk::LinkToXAODTrackParticle(link);
      linkTT->setStorableObject(*trackParticles);
      selectedTracks.push_back(linkTT);
    }
  }

  ATH_MSG_DEBUG("Of " << trackParticles->size() << " tracks "
                      << selectedTracks.size()
                      << " survived the preselection.");

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
    returnContainers = findVertex(ctx, selectedTracks);

  return returnContainers;
}

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
InDetIterativePriVxFinderTool::findVertex(
  const EventContext& ctx,
  const std::vector<Trk::ITrackLink*>& trackVector) const
{
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{ m_beamSpotKey, ctx };
  const InDet::BeamSpotData* beamSpot = *beamSpotHandle;
  // two things need to be added
  // 1) the seeding mechanism
  // 2) the iterative removal of tracks

  std::vector<Trk::ITrackLink*> origTracks = trackVector;
  std::vector<Trk::ITrackLink*> seedTracks = trackVector;

  // TODO: xAODVertex_pair never seems to be used?
  std::vector<xAODVertex_pair> myxAODVertices;

  xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer;
  xAOD::VertexAuxContainer* theVertexAuxContainer =
    new xAOD::VertexAuxContainer;
  theVertexContainer->setStore(theVertexAuxContainer);

  // bail out early with only Dummy vertex if multiplicity cut is applied and
  // exceeded

  if (m_doMaxTracksCut && (trackVector.size() > m_maxTracks)) {
    ATH_MSG_WARNING(trackVector.size()
                    << " tracks - exceeds maximum (" << m_maxTracks
                    << "), skipping vertexing and returning only dummy...");
    xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
    theVertexContainer->push_back(
      dummyxAODVertex); // have to add vertex to container here first so it can
                        // use its aux store
    dummyxAODVertex->setPosition(beamSpot->beamVtx().position());
    dummyxAODVertex->setCovariancePosition(
      beamSpot->beamVtx().covariancePosition());
    dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
    dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
    return std::make_pair(theVertexContainer, theVertexAuxContainer);
  }

  int iterations = -1;
  unsigned int seedtracknumber = seedTracks.size();

  // used to store seed info
  Amg::Vector3D actualVertex;

  // prepare iterators for tracks only necessary for seeding
  std::vector<Trk::ITrackLink*>::iterator seedBegin;
  std::vector<Trk::ITrackLink*>::iterator seedEnd;

  do {

    seedBegin = seedTracks.begin();
    seedEnd = seedTracks.end();

    if (seedtracknumber == 0) {
      ATH_MSG_DEBUG(" New iteration. No tracks available after track selection "
                    "for seeding. No finding done.");
      break;
    }

    iterations += 1;
    ATH_MSG_DEBUG("ITERATION NUMBER " << iterations);

    // now find a new SEED

    std::vector<const Trk::TrackParameters*> perigeeList;
    for (std::vector<Trk::ITrackLink*>::iterator seedtrkAtVtxIter = seedBegin;
         seedtrkAtVtxIter != seedEnd;
         ++seedtrkAtVtxIter) {
      perigeeList.push_back((*seedtrkAtVtxIter)->parameters());
    }

    xAOD::Vertex theconstraint;
    if (m_useBeamConstraint && beamSpot != nullptr) {
      theconstraint =
        xAOD::Vertex(); // Default constructor creates a private store
      theconstraint.setPosition(beamSpot->beamVtx().position());
      theconstraint.setCovariancePosition(
        beamSpot->beamVtx().covariancePosition());
      theconstraint.setFitQuality(
        beamSpot->beamVtx().fitQuality().chiSquared(),
        beamSpot->beamVtx().fitQuality().doubleNumberDoF());
      actualVertex = m_SeedFinder->findSeed(perigeeList, &theconstraint);
    } else {
      actualVertex = m_SeedFinder->findSeed(perigeeList);
      Amg::MatrixX looseConstraintCovariance(3, 3);
      looseConstraintCovariance.setIdentity();
      looseConstraintCovariance = looseConstraintCovariance * 1e+8;
      theconstraint = xAOD::Vertex();
      theconstraint.setPosition(actualVertex);
      theconstraint.setCovariancePosition(looseConstraintCovariance);
      theconstraint.setFitQuality(0., -3.);
    }

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << " seed at x: " << actualVertex.x()
                      << " at y: " << actualVertex.y()
                      << " at z: " << actualVertex.z() << endmsg;
    }

    if (actualVertex.z() == 0.) {
      break;
    }

    // now question (remove tracks which are too far away??? I think so...)
    std::vector<const Trk::TrackParameters*> perigeesToFit;
    std::vector<const Trk::TrackParameters*> perigeesToFitSplitVertex;

    int numberOfTracks(perigeeList.size());

    std::vector<const Trk::TrackParameters*>::const_iterator perigeeListBegin =
      perigeeList.begin();
    std::vector<const Trk::TrackParameters*>::const_iterator perigeeListEnd =
      perigeeList.end();

    int counter = 0;

    for (std::vector<const Trk::TrackParameters*>::const_iterator
           perigeeListIter = perigeeListBegin;
         perigeeListIter != perigeeListEnd;
         ++perigeeListIter) {
      if (numberOfTracks <= 2) {
        perigeesToFit.push_back(*perigeeListIter);
        counter += 1;
      } else if (numberOfTracks <= 4 && !m_createSplitVertices) {
        perigeesToFit.push_back(*perigeeListIter);
        counter += 1;
      } else if (numberOfTracks <= 4 * m_splitVerticesTrkInvFraction &&
                 m_createSplitVertices) {
        // few tracks are left, put them into the fit regardless their position!
        if (counter % m_splitVerticesTrkInvFraction == 0) {
          perigeesToFit.push_back(*perigeeListIter);
          counter += 1;
        } else {
          perigeesToFitSplitVertex.push_back(*perigeeListIter);
          counter += 1;
        }
      } else { // check first whether it is not too far away!
        double distance = 0.;
        try {
          std::unique_ptr<Trk::PlaneSurface> mySurface =
            m_ImpactPoint3dEstimator->Estimate3dIP(
              *perigeeListIter, &actualVertex, distance);
        } catch (error::ImpactPoint3dEstimatorProblem err) {
          msg(MSG::WARNING) << " ImpactPoint3dEstimator failed to find minimum "
                               "distance between track and vertex seed: "
                            << err.p << endmsg;
        }

        if (distance < 0) {
          msg(MSG::WARNING)
            << " Distance between track and seed vtx is negative: " << distance
            << endmsg;
        }

        const Trk::TrackParameters* myPerigee = (*perigeeListIter);

        // very approximate error
        double error = 0.;

        if (myPerigee && myPerigee->covariance()) {
          error = std::sqrt((*myPerigee->covariance())(Trk::d0, Trk::d0) +
                            (*myPerigee->covariance())(Trk::z0, Trk::z0));
        } // end of the security check

        if (error == 0.) {
          msg(MSG::ERROR) << " Error is zero! " << distance << endmsg;
          error = 1.;
        }

        if (distance / error < m_significanceCutSeeding) {
          if (counter % m_splitVerticesTrkInvFraction == 0 ||
              !m_createSplitVertices) {
            perigeesToFit.push_back(*perigeeListIter);
            counter += 1;
          } else {
            perigeesToFitSplitVertex.push_back(*perigeeListIter);
            counter += 1;
          }
        }
      }
    }

    if (perigeesToFit.empty()) {
      ATH_MSG_DEBUG(" No good seed found. Exiting search for vertices...");
      break;
    }

    // now you have perigeeToFit and perigeeToFitSplitVertices
    // AND HERE YOU DO THE FIT
    // to reassign vertices you look ino what is already in myVxCandidate
    // you do it only ONCE!

    xAOD::Vertex* myxAODVertex = nullptr;
    xAOD::Vertex* myxAODSplitVertex = nullptr;

    if (m_useBeamConstraint && !perigeesToFit.empty()) {
      myxAODVertex = m_iVertexFitter->fit(perigeesToFit, theconstraint);
    } else if (!m_useBeamConstraint && perigeesToFit.size() > 1) {
      myxAODVertex = m_iVertexFitter->fit(perigeesToFit);
    }
    if (m_createSplitVertices && perigeesToFitSplitVertex.size() > 1) {
      myxAODSplitVertex = m_iVertexFitter->fit(perigeesToFitSplitVertex);
    }

    double ndf = 0.;
    int ntracks = 0;
    countTracksAndNdf(myxAODVertex, ndf, ntracks);

    double ndfSplitVertex = 0.;
    int ntracksSplitVertex = 0;
    countTracksAndNdf(myxAODSplitVertex, ndfSplitVertex, ntracksSplitVertex);

    bool goodVertex = myxAODVertex != nullptr &&
                      ((!m_useBeamConstraint && ndf > 0 && ntracks >= 2) ||
                       (m_useBeamConstraint && ndf > 3 && ntracks >= 2));

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << " xAOD::Vertex is pointer: " << myxAODVertex
                      << " ndf = " << ndf << " ntracks (with weight>0.01) "
                      << ntracks << " beam constraint "
                      << (m_useBeamConstraint ? "yes" : "no") << endmsg;
    }

    if (!goodVertex) {
      removeAllFrom(perigeesToFit, seedTracks);
    } else {
      if (m_reassignTracksAfterFirstFit && (!m_createSplitVertices)) {
        // now you HAVE a good vertex
        // but you want to add the tracks which you missed...

        int numberOfAddedTracks = 0;

        // iterate on remaining vertices and cross-check if tracks need to be
        // attached to new vertex
        xAOD::VertexContainer::iterator vxBegin = theVertexContainer->begin();
        xAOD::VertexContainer::iterator vxEnd = theVertexContainer->end();

        for (xAOD::VertexContainer::iterator vxIter = vxBegin; vxIter != vxEnd;
             ++vxIter) {
          // now iterate on tracks at vertex

          std::vector<Trk::VxTrackAtVertex>* myVxTracksAtVtx =
            (&(*vxIter)->vxTrackAtVertex());

          if (!myVxTracksAtVtx)
            continue;

          std::vector<Trk::VxTrackAtVertex>::iterator tracksBegin =
            myVxTracksAtVtx->begin();
          std::vector<Trk::VxTrackAtVertex>::iterator tracksEnd =
            myVxTracksAtVtx->end();

          for (std::vector<Trk::VxTrackAtVertex>::iterator tracksIter =
                 tracksBegin;
               tracksIter != tracksEnd;) {
            // only try with tracks which are not too tightly assigned to
            // another vertex
            if ((*tracksIter).weight() > 0.01) {
              ++tracksIter;
              continue;
            }

            const Trk::TrackParameters* trackPerigee =
              (*tracksIter).initialPerigee();

            if (trackPerigee == nullptr) {
              msg(MSG::ERROR) << " Cast to perigee gives 0 pointer " << endmsg;
            }

            double chi2_newvtx = compatibility(*trackPerigee, *myxAODVertex);
            double chi2_oldvtx = compatibility(*trackPerigee, *(*vxIter));

            bool remove = false;

            if (chi2_newvtx < chi2_oldvtx) {
              ATH_MSG_DEBUG(" Found track of old vertex (chi2= "
                            << chi2_oldvtx
                            << ") more compatible to new one (chi2= "
                            << chi2_newvtx << ")");

              perigeesToFit.push_back(trackPerigee);
              // but you need to re-add it to the seedTracks too...

              bool isFound = false;

              std::vector<Trk::ITrackLink*>::iterator origBegin =
                origTracks.begin();
              std::vector<Trk::ITrackLink*>::iterator origEnd =
                origTracks.end();

              for (std::vector<Trk::ITrackLink*>::iterator origIter = origBegin;
                   origIter != origEnd;
                   ++origIter) {
                if ((*origIter)->parameters() == trackPerigee) {
                  isFound = true;
                  seedTracks.push_back(*origIter);
                  break;
                }
              }
              if (!isFound) {
                ATH_MSG_WARNING(" Cannot find old perigee to re-add back to "
                                "seed tracks... ");
              }

              numberOfAddedTracks += 1;

              remove = true;
            }

            if (remove) {
              // now you have to delete the track from the old vertex...
              // easy???
              tracksIter = myVxTracksAtVtx->erase(tracksIter);
              tracksBegin = myVxTracksAtVtx->begin();
              tracksEnd = myVxTracksAtVtx->end();
            } else {
              ++tracksIter;
            }
          } // end of iterating on tracks
        }   // end of iterating on already found vertices in event

        // now you have to delete the previous xAOD::Vertex, do a new fit, then
        // check if you still have a good vertex

        if (numberOfAddedTracks > 0) {
          delete myxAODVertex;
          myxAODVertex = nullptr;

          if (m_useBeamConstraint && !perigeesToFit.empty()) {
            myxAODVertex = m_iVertexFitter->fit(perigeesToFit, theconstraint);
          } else if (!m_useBeamConstraint && perigeesToFit.size() > 1) {
            myxAODVertex = m_iVertexFitter->fit(perigeesToFit);
          }

          ndf = 0.;
          ntracks = 0;
          countTracksAndNdf(myxAODVertex, ndf, ntracks);

          goodVertex = myxAODVertex != nullptr &&
                       ((!m_useBeamConstraint && ndf > 0 && ntracks >= 2) ||
                        (m_useBeamConstraint && ndf > 3 && ntracks >= 2));

          ATH_MSG_DEBUG(" Refitted xAODVertex is pointer: " << myxAODVertex
                        << " ndf = " << ndf << " ntracks (with weight>0.01) "
                        << ntracks << " beam constraint "
                        << (m_useBeamConstraint ? "yes" : "no"));

          if (!goodVertex) {
            removeAllFrom(perigeesToFit, seedTracks);
            ATH_MSG_DEBUG(" Adding tracks resulted in an invalid vertex. "
                          "Should be rare. ");
          }
        } // end if tracks were added...
      }   // end reassign tracks from previous vertices and refitting if needed

      // need to re-ask goodVertex since it can be changed in the mean time
      if (goodVertex) {
        removeCompatibleTracks(myxAODVertex, perigeesToFit, seedTracks);
      }
    } // end else case on if not good Vertex

    bool goodSplitVertex = false;

    if (m_createSplitVertices) {
      goodSplitVertex = myxAODSplitVertex != nullptr && ndfSplitVertex > 0 &&
                        ntracksSplitVertex >= 2;

      ATH_MSG_DEBUG(" xAODSplitVertex is pointer: "
                    << myxAODSplitVertex << " ndf = " << ndfSplitVertex
                    << " ntracks (with weight>0.01) " << ntracksSplitVertex);

      if (!goodSplitVertex) {
        removeAllFrom(perigeesToFitSplitVertex, seedTracks);
      } else {
        removeCompatibleTracks(
          myxAODSplitVertex, perigeesToFitSplitVertex, seedTracks);

      } // end else if not good Vertex
    }   // end if create split vertices

    if (!m_createSplitVertices) {
      if (goodVertex) {
        theVertexContainer->push_back(myxAODVertex);
      } else {
        if (myxAODVertex) {
          delete myxAODVertex;
          myxAODVertex = nullptr;
        }
      }
    } else {
      if (goodVertex) {
        // type does not seem to be set earlier
        myxAODVertex->setVertexType(xAOD::VxType::PriVtx);
        theVertexContainer->push_back(myxAODVertex);
      } else {
        if (myxAODVertex) {
          delete myxAODVertex;
          myxAODVertex = nullptr;
        }

        xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
        theVertexContainer->push_back(
          dummyxAODVertex); // have to add vertex to container here first so it
                            // can use its aux store
        dummyxAODVertex->setPosition(beamSpot->beamVtx().position());
        dummyxAODVertex->setCovariancePosition(
          beamSpot->beamVtx().covariancePosition());
        dummyxAODVertex->vxTrackAtVertex() =
          std::vector<Trk::VxTrackAtVertex>();
        dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
      }
      if (goodSplitVertex) {
        // type does not seem to be set earlier
        myxAODSplitVertex->setVertexType(xAOD::VxType::PriVtx);
        theVertexContainer->push_back(myxAODSplitVertex);
      } else {
        if (myxAODSplitVertex) {
          delete myxAODSplitVertex;
          myxAODSplitVertex = nullptr;
        }

        xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
        theVertexContainer->push_back(
          dummyxAODVertex); // have to add vertex to container here first so it
                            // can use its aux store
        dummyxAODVertex->setPosition(beamSpot->beamVtx().position());
        dummyxAODVertex->setCovariancePosition(
          beamSpot->beamVtx().covariancePosition());
        dummyxAODVertex->vxTrackAtVertex() =
          std::vector<Trk::VxTrackAtVertex>();
        dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
      }
    }
  } while (seedTracks.size() > 1 && iterations < m_maxVertices);

  if (iterations >= m_maxVertices) {
    ATH_MSG_WARNING("Reached maximum iterations ");
  }

  // unfortunately you have still a problem with the track to links!!!

  //---- add dummy vertex at the end
  //------------------------------------------------------//
  //---- if one or more vertices are already there: let dummy have same position
  //as primary vertex
  if (!m_createSplitVertices) {
    if (!theVertexContainer->empty()) {
      xAOD::Vertex* primaryVtx = theVertexContainer->front();
      if (!primaryVtx->vxTrackAtVertex().empty()) {
        primaryVtx->setVertexType((xAOD::VxType::VertexType)Trk::PriVtx);
        xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
        theVertexContainer->push_back(
          dummyxAODVertex); // have to add vertex to container here first so it
                            // can use its aux store
        dummyxAODVertex->setPosition(primaryVtx->position());
        dummyxAODVertex->setCovariancePosition(
          primaryVtx->covariancePosition());
        dummyxAODVertex->vxTrackAtVertex() =
          std::vector<Trk::VxTrackAtVertex>();
        dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
      } else {
        primaryVtx->setVertexType(xAOD::VxType::NoVtx);
      }
    }
    //---- if no vertex is there let dummy be at beam spot

    else if (theVertexContainer->empty()) {
      xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
      theVertexContainer->push_back(
        dummyxAODVertex); // have to add vertex to container here first so it
                          // can use its aux store
      dummyxAODVertex->setPosition(beamSpot->beamVtx().position());
      dummyxAODVertex->setCovariancePosition(
        beamSpot->beamVtx().covariancePosition());
      dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
      dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
    }
    // loop over the pile up to set it as pile up (EXCLUDE first and last
    // vertex, do not do that in split mode)
    for (unsigned int i = 0; i < theVertexContainer->size() - 1; i++) {

      ATH_MSG_DEBUG(
        " Vtx: " << i << " x= " << (*theVertexContainer)[i]->position().x()
                 << " y= " << (*theVertexContainer)[i]->position().y() << " z= "
                 << (*theVertexContainer)[i]->position().z() << " ntracks= "
                 << (*theVertexContainer)[i]->vxTrackAtVertex().size()
                 << " chi2= " << (*theVertexContainer)[i]->chiSquared()
                 << " ndf = " << (*theVertexContainer)[i]->numberDoF());
      if (i > 0) {
        (*theVertexContainer)[i]->setVertexType(xAOD::VxType::PileUp);
      }
    }
  }

  xAOD::VertexContainer::iterator vxBegin = theVertexContainer->begin();
  xAOD::VertexContainer::iterator vxEnd = theVertexContainer->end();

  // prepare iterators for tracks only necessary for seeding
  std::vector<Trk::ITrackLink*>::iterator origtrkbegin = origTracks.begin();
  std::vector<Trk::ITrackLink*>::iterator origtrkend = origTracks.end();

  for (xAOD::VertexContainer::iterator vxIter = vxBegin; vxIter != vxEnd;
       ++vxIter) {
    std::vector<Trk::VxTrackAtVertex>* myVxTracksAtVtx =
      &((*vxIter)->vxTrackAtVertex());

    if (!myVxTracksAtVtx)
      continue;

    std::vector<Trk::VxTrackAtVertex>::iterator tracksBegin =
      myVxTracksAtVtx->begin();
    std::vector<Trk::VxTrackAtVertex>::iterator tracksEnd =
      myVxTracksAtVtx->end();

    bool found = false;

    for (std::vector<Trk::VxTrackAtVertex>::iterator tracksIter = tracksBegin;
         tracksIter != tracksEnd;
         ++tracksIter) {

      // now look for corresponding ITrackLink
      for (std::vector<Trk::ITrackLink*>::iterator origtrkiter = origtrkbegin;
           origtrkiter != origtrkend;
           ++origtrkiter) {
        if ((*origtrkiter)->parameters() == (*tracksIter).initialPerigee()) {
          found = true;

          // assigning the input track to the fitted vertex through
          // VxTrackAtVertices vector
          (*tracksIter).setOrigTrack(*origtrkiter);

          // See if the trklink is to an xAOD::TrackParticle
          Trk::LinkToXAODTrackParticle* linkToXAODTP = nullptr;
          Trk::ITrackLink* tmpLink = (*tracksIter).trackOrParticleLink();
          if (tmpLink->type() == Trk::ITrackLink::ToxAODTrackParticle) {
            linkToXAODTP = static_cast<Trk::LinkToXAODTrackParticle*>(tmpLink);
          }

          // If track is an xAOD::TrackParticle, set directly in xAOD::Vertex
          if (linkToXAODTP) {
            (*vxIter)->addTrackAtVertex(*linkToXAODTP, (*tracksIter).weight());
          } 

          origTracks.erase(origtrkiter);
          origtrkbegin = origTracks.begin();
          origtrkend = origTracks.end();
          break;
        }
      }
      if (!found) {
        ATH_MSG_ERROR(" Cannot find vector element to fix links (step 4)! ");
      }
    } // end iterate on tracks at vtx
  }   // end iterate on vertices

  for (std::vector<Trk::ITrackLink*>::iterator origtrkiter = origtrkbegin;
       origtrkiter != origtrkend;
       ++origtrkiter) {
    if ((*origtrkiter) != 0) {
      delete *origtrkiter;
      *origtrkiter = 0;
    }
  }

  return std::make_pair(theVertexContainer, theVertexAuxContainer);
}

StatusCode
InDetIterativePriVxFinderTool::finalize()
{
  return StatusCode::SUCCESS;
}

void
InDetIterativePriVxFinderTool::printParameterSettings()
{
  ATH_MSG_INFO("VxPrimary initialize(): Parametersettings "
               << '\n'
               << "VertexFitter " << m_iVertexFitter << '\n');
}

void
InDetIterativePriVxFinderTool::SGError(const std::string& errService)
{
  msg(MSG::FATAL) << errService << " not found. Exiting !" << endmsg;
}

double
InDetIterativePriVxFinderTool::compatibility(
  const Trk::TrackParameters& measPerigee,
  const xAOD::Vertex& vertex) const
{
  Trk::LinearizedTrack* myLinearizedTrack =
    m_LinearizedTrackFactory->linearizedTrack(&measPerigee, vertex.position());

  AmgMatrix(2, 2) weightReduced =
    myLinearizedTrack->expectedCovarianceAtPCA().block<2, 2>(0, 0);

  AmgMatrix(2, 2) errorVertexReduced =
    (myLinearizedTrack->positionJacobian() *
     (vertex.covariancePosition() *
      myLinearizedTrack->positionJacobian().transpose()))
      .block<2, 2>(0, 0);

  weightReduced += errorVertexReduced;

  weightReduced = weightReduced.inverse().eval();
  Amg::Vector2D trackParameters2D =
    myLinearizedTrack->expectedParametersAtPCA().block<2, 1>(0, 0);
  double returnValue = trackParameters2D.dot(weightReduced * trackParameters2D);

  delete myLinearizedTrack;
  myLinearizedTrack = nullptr;

  return returnValue;
}

void
InDetIterativePriVxFinderTool::removeAllFrom(
  std::vector<const Trk::TrackParameters*>& perigeesToFit,
  std::vector<Trk::ITrackLink*>& seedTracks) const
{
  // remove all perigeesToFit and go on...

  std::vector<Trk::ITrackLink*>::iterator seedBegin = seedTracks.begin();
  std::vector<Trk::ITrackLink*>::iterator seedEnd = seedTracks.end();

  std::vector<const Trk::TrackParameters*>::iterator perigeesToFitBegin =
    perigeesToFit.begin();
  std::vector<const Trk::TrackParameters*>::iterator perigeesToFitEnd =
    perigeesToFit.end();

  for (std::vector<const Trk::TrackParameters*>::iterator perigeesToFitIter =
         perigeesToFitBegin;
       perigeesToFitIter != perigeesToFitEnd;
       ++perigeesToFitIter) {

    bool found = false;

    for (std::vector<Trk::ITrackLink*>::iterator seedIter = seedTracks.begin();
         seedIter != seedEnd;
         ++seedIter) {
      if ((*seedIter)->parameters() == *perigeesToFitIter) {
        found = true;
        seedTracks.erase(seedIter);
        seedBegin = seedTracks.begin();
        seedEnd = seedTracks.end();
        break;
      }
    }
    if (!found) {
      ATH_MSG_ERROR(
        " Cannot find vector element to delete when removing BAD vertex! ");
    }
  } // for cycle
}

void
InDetIterativePriVxFinderTool::countTracksAndNdf(xAOD::Vertex* myxAODVertex,
                                                 double& ndf,
                                                 int& ntracks) const
{
  if (myxAODVertex) {
    ndf = myxAODVertex->numberDoF();

    std::vector<Trk::VxTrackAtVertex> myVxTracksAtVtx =
      myxAODVertex->vxTrackAtVertex();

    std::vector<Trk::VxTrackAtVertex>::iterator tracksBegin =
      myVxTracksAtVtx.begin();
    std::vector<Trk::VxTrackAtVertex>::iterator tracksEnd =
      myVxTracksAtVtx.end();

    for (std::vector<Trk::VxTrackAtVertex>::iterator tracksIter = tracksBegin;
         tracksIter != tracksEnd;
         ++tracksIter) {
      if ((*tracksIter).weight() > 0.01) {
        ntracks += 1;
      }
    }
  }
}

void
InDetIterativePriVxFinderTool::removeCompatibleTracks(
  xAOD::Vertex* myxAODVertex,
  std::vector<const Trk::TrackParameters*>& perigeesToFit,
  std::vector<Trk::ITrackLink*>& seedTracks) const
{
  // now you have your new vertex with its new tracks
  // now you have to get the compatibility also of all tracks which DIDN'T
  // BELONG to the vertex!
  std::vector<Trk::VxTrackAtVertex>* tracksAtVertex =
    &(myxAODVertex->vxTrackAtVertex());

  std::vector<Trk::VxTrackAtVertex>::const_iterator tracksAtVertexBegin =
    tracksAtVertex->begin();
  std::vector<Trk::VxTrackAtVertex>::const_iterator tracksAtVertexEnd =
    tracksAtVertex->end();

  std::vector<Trk::ITrackLink*>::iterator seedBegin = seedTracks.begin();
  std::vector<Trk::ITrackLink*>::iterator seedEnd = seedTracks.end();

  std::vector<const Trk::TrackParameters*>::iterator perigeesToFitBegin =
    perigeesToFit.begin();
  std::vector<const Trk::TrackParameters*>::iterator perigeesToFitEnd =
    perigeesToFit.end();

  for (std::vector<Trk::VxTrackAtVertex>::const_iterator tracksAtVertexIter =
         tracksAtVertexBegin;
       tracksAtVertexIter != tracksAtVertexEnd;
       ++tracksAtVertexIter) {

    bool found = false;

    for (std::vector<Trk::ITrackLink*>::iterator seedIter = seedBegin;
         seedIter != seedEnd;
         ++seedIter) {
      if ((*seedIter)->parameters() == (*tracksAtVertexIter).initialPerigee()) {
        found = true;
        if ((*tracksAtVertexIter).weight() > 0.01) {
          seedTracks.erase(seedIter);
          seedBegin = seedTracks.begin();
          seedEnd = seedTracks.end();
        }
        break;
      }
    }

    if (!found) {
      ATH_MSG_ERROR(" Cannot find vector element to delete (step 1)! ");
    }

    found = false;
    for (std::vector<const Trk::TrackParameters*>::iterator perigeesToFitIter =
           perigeesToFitBegin;
         perigeesToFitIter != perigeesToFitEnd;
         ++perigeesToFitIter) {
      if (*perigeesToFitIter == (*tracksAtVertexIter).initialPerigee()) {
        found = true;
        if ((*tracksAtVertexIter).weight() > 0.01) {
          perigeesToFit.erase(perigeesToFitIter);
          perigeesToFitBegin = perigeesToFit.begin();
          perigeesToFitEnd = perigeesToFit.end();
        }
        break;
      }
    }

    if (!found) {
      ATH_MSG_ERROR(" Cannot find vector element to delete (step 2)! ");
    }
  } // finishing iterating on tracks at vertex

  std::vector<Trk::VxTrackAtVertex>* myVxTracksAtVertex =
    &(myxAODVertex->vxTrackAtVertex());

  std::vector<Trk::VxTrackAtVertex>::iterator tracksBegin =
    myVxTracksAtVertex->begin();
  std::vector<Trk::VxTrackAtVertex>::iterator tracksEnd =
    myVxTracksAtVertex->end();

  for (std::vector<const Trk::TrackParameters*>::iterator perigeesToFitIter =
         perigeesToFitBegin;
       perigeesToFitIter != perigeesToFitEnd;
       ++perigeesToFitIter) {
    bool found = false;

    // compute the chi2 with respect to the last fitted vertex!
    //(easy since track was NOT used in the last vertex fit...)

    const Trk::TrackParameters* myPerigee = (*perigeesToFitIter);

    if (myPerigee == nullptr) {
      ATH_MSG_ERROR(" Cast to perigee gives 0 pointer ");
    }

    double chi2 = compatibility(*myPerigee, *myxAODVertex);

    // check if still sufficiently compatible to previous vertex
    //(CAN BE VERY LOOSE TO BE CONSERVATIVE AGAINST FAR OUTLIERS)
    if (chi2 < m_maximumChi2cutForSeeding) {
      for (std::vector<Trk::ITrackLink*>::iterator seedIter =
             seedTracks.begin();
           seedIter != seedEnd;
           ++seedIter) {
        if ((*seedIter)->parameters() == *perigeesToFitIter) {
          found = true;
          seedTracks.erase(seedIter);
          seedBegin = seedTracks.begin();
          seedEnd = seedTracks.end();
          break;
        }
      }

      if (!found) {
        ATH_MSG_ERROR(" Cannot find vector element to delete (step 3)! ");
      }
    } else {
      // look if the track is attached to the vertex. If this is the case you
      // should delete the track from the vertex!
      for (std::vector<Trk::VxTrackAtVertex>::iterator tracksIter = tracksBegin;
           tracksIter != tracksEnd;
           ++tracksIter) {
        if ((*tracksIter).initialPerigee() == *perigeesToFitIter) {
          // delete *tracksIter;
          // delete has to happen BEFORE the erase (because the iterator will
          // point to the next object in the vector AFTER the erase!)
          myVxTracksAtVertex->erase(tracksIter);
          tracksBegin = myVxTracksAtVertex->begin();
          tracksEnd = myVxTracksAtVertex->end();
          found = true;
          break;
        }
      }
    }
  } // iterate on all perigeesToFit
}

} // end namespace InDet
