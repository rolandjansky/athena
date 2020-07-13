/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetAdaptivePriVxFinderTool.cxx  -  Description
                             -------------------
    begin   : 29-05-2006
    authors : Giacinto Piacquadio (Freiburg Univ),
              this is a modified version of the adaptive primary vertex finder
to fit more vertexes at the same time changes : 06/12/2006
Kirill.Prokofiev@cern.ch EDM cleanup and switching to the FitQuality use
              2007-02-15  bug fix by scott s snyder  <snyder@bnl.gov>
              Fix memory leak.  Don't use a map for sorting.
              2007-10-xx  Giacinto Piacquadio
                          Many improvements (MultiVertexFinder v. 2)
              2016-03-09  D. Casper
                          Change default value of TracksMaxZinterval to 1 mm
                          (trial working point for high luminosities)

***************************************************************************/
#include "InDetPriVxFinderTool/InDetAdaptiveMultiPriVxFinderTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/Vertex.h"

#include "AthContainers/DataVector.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "TrkTrack/LinkToTrack.h"
#include "VxMultiVertex/MVFVxTrackAtVertex.h"
#include "VxMultiVertex/MvfFitInfo.h"
#include "VxMultiVertex/TrackToVtxLink.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexContainer.h"

#include <cmath>
#include <map>
#include <vector>

namespace InDet {
InDetAdaptiveMultiPriVxFinderTool::InDetAdaptiveMultiPriVxFinderTool(
  const std::string& t,
  const std::string& n,
  const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_useBeamConstraint(true)
  , m_TracksMaxZinterval(1.)
  , m_maxVertexChi2(18.42)
  , m_realMultiVertex(true)
  , m_useFastCompatibility(true)
  , m_selectiontype(0)
  , m_finalCutMaxVertexChi2(18.42)
  , m_cutVertexDependence(3.)
  , m_minweight(0.0001)
  , m_maxIterations(100)
  , m_addSingleTrackVertices(false)
  , m_do3dSplitting(false)
  , m_zBfieldApprox(0.60407)
  , m_maximumVertexContamination(0.5)
  , m_tracksMaxSignificance(5.)
  , m_useSeedConstraint(true)
{
  declareInterface<IVertexFinder>(
    this); // by GP: changed from InDetMultiAdaptivePriVxFinderTool to
           // IPriVxFinderTool
  // finder options
  declareProperty("TracksMaxZinterval", m_TracksMaxZinterval);
  declareProperty("maxVertexChi2", m_maxVertexChi2);
  declareProperty("finalCutMaxVertexChi2", m_finalCutMaxVertexChi2);
  declareProperty("cutVertexDependence", m_cutVertexDependence);
  declareProperty("MinWeight", m_minweight);
  declareProperty("realMultiVertex", m_realMultiVertex);
  declareProperty("useFastCompatibility", m_useFastCompatibility);
  declareProperty("useBeamConstraint", m_useBeamConstraint);
  declareProperty("addSingleTrackVertices", m_addSingleTrackVertices);
  declareProperty("tracksMaxSignificance", m_tracksMaxSignificance);
  declareProperty("m_useSeedConstraint", m_useSeedConstraint);
  //********* signal vertex selection (for pile up) ****
  declareProperty("selectiontype", m_selectiontype);
  //==0 for sum p_t^2
  //==1 for NN
  //==2 for min bias compatibility estimation (in the future)
  declareProperty("maxIterations", m_maxIterations);
  declareProperty("do3dSplitting", m_do3dSplitting);
  declareProperty("zBfieldApprox", m_zBfieldApprox);
  declareProperty("maximumVertexContamination", m_maximumVertexContamination);
}

StatusCode
InDetAdaptiveMultiPriVxFinderTool::initialize()
{
  /* Get the right vertex fitting tool */
  ATH_CHECK(m_MultiVertexFitter.retrieve());

  ATH_CHECK(m_analyticSeedFinder.retrieve());

  ATH_CHECK(m_beamSpotKey.initialize());

  ATH_CHECK(m_trkFilter.retrieve());

  // since some parameters special to an inherited class this method
  // will be overloaded by the inherited class
  if (msgLvl(MSG::DEBUG))
    printParameterSettings();

  ATH_MSG_INFO("Initialization successful");
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
InDetAdaptiveMultiPriVxFinderTool::findVertex(
  const EventContext& ctx,
  const TrackCollection* trackTES) const
{
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{ m_beamSpotKey, ctx };
  const Trk::RecVertex& beamposition(beamSpotHandle->beamVtx());

  std::vector<const Trk::ITrackLink*> selectedTracks;

  typedef DataVector<Trk::Track>::const_iterator TrackDataVecIter;

  bool selectionPassed;
  for (TrackDataVecIter itr = (*trackTES).begin(); itr != (*trackTES).end();
       itr++) {
    if (m_useBeamConstraint) {
      selectionPassed =
        static_cast<bool>(m_trkFilter->accept(**itr, &beamposition));
    } else {
      Trk::Vertex null(Amg::Vector3D(0, 0, 0));
      selectionPassed = static_cast<bool>(
        m_trkFilter->accept(**itr, &null)); // TODO: change trkFilter?
    }
    if (selectionPassed) {
      ElementLink<TrackCollection> link;
      link.setElement(const_cast<Trk::Track*>(*itr));
      Trk::LinkToTrack* linkTT = new Trk::LinkToTrack(link);
      linkTT->setStorableObject(*trackTES);
      selectedTracks.push_back(linkTT);
    }
  }

  ATH_MSG_DEBUG("Of " << trackTES->size() << " tracks " << selectedTracks.size()
                      << " survived the preselection.");

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
    returnContainers = findVertex(ctx, selectedTracks);

  std::vector<const Trk::ITrackLink*>::iterator ibegin = selectedTracks.begin();
  std::vector<const Trk::ITrackLink*>::iterator iend = selectedTracks.end();

  for (std::vector<const Trk::ITrackLink*>::iterator iiter = ibegin;
       iiter != iend;
       ++iiter) {
    if ((*iiter) != 0) {
      delete *iiter;
      *iiter = 0;
    }
  }

  return returnContainers;
}

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
InDetAdaptiveMultiPriVxFinderTool::findVertex(
  const EventContext& ctx,
  const xAOD::TrackParticleContainer* trackParticles) const
{
  std::vector<const Trk::ITrackLink*> selectedTracks;
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{ m_beamSpotKey, ctx };
  xAOD::Vertex beamposition;
  beamposition.makePrivateStore();
  beamposition.setPosition(beamSpotHandle->beamVtx().position());
  beamposition.setCovariancePosition(
    beamSpotHandle->beamVtx().covariancePosition());

  typedef DataVector<xAOD::TrackParticle>::const_iterator
    TrackParticleDataVecIter;

  bool selectionPassed;
  for (TrackParticleDataVecIter itr = (*trackParticles).begin();
       itr != (*trackParticles).end();
       itr++) {
    if (m_useBeamConstraint) {
      selectionPassed =
        static_cast<bool>(m_trkFilter->accept(**itr, &beamposition));
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
      link.setElement(const_cast<xAOD::TrackParticle*>(*itr));
      Trk::LinkToXAODTrackParticle* linkTT =
        new Trk::LinkToXAODTrackParticle(link);
      linkTT->setStorableObject(*trackParticles); //@TODO: really?!
      selectedTracks.push_back(linkTT);
    }
  }

  ATH_MSG_DEBUG("Of " << trackParticles->size() << " tracks "
                      << selectedTracks.size()
                      << " survived the preselection.");

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
    returnContainers = findVertex(ctx, selectedTracks);

  std::vector<const Trk::ITrackLink*>::iterator ibegin = selectedTracks.begin();
  std::vector<const Trk::ITrackLink*>::iterator iend = selectedTracks.end();

  // clean-up and return
  for (std::vector<const Trk::ITrackLink*>::iterator iiter = ibegin;
       iiter != iend;
       ++iiter) {
    if ((*iiter) != 0) {
      delete *iiter;
      *iiter = 0;
    }
  }

  return returnContainers;
}

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
InDetAdaptiveMultiPriVxFinderTool::findVertex(
  const EventContext& ctx,
  const std::vector<const Trk::ITrackLink*>& trackVector) const
{
  // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
  // For optimization of access speed
  static const xAOD::Vertex::Decorator<Trk::MvfFitInfo*> MvfFitInfo(
    "MvfFitInfo");
  static const xAOD::Vertex::Decorator<bool> isInitialized("isInitialized");
  static const xAOD::Vertex::Decorator<std::vector<Trk::VxTrackAtVertex*>> VTAV(
    "VTAV");

  if (m_selectiontype == 1) {
    ATH_MSG_WARNING("Only Selection Type 0 supported for MT");
  }

  //---- Start of preselection of tracks according to perigee parameters
  //---------------//
  std::vector<const Trk::ITrackLink*> origTracks = trackVector;
  std::vector<const Trk::ITrackLink*> seedTracks = trackVector;

  // now all tracks are in origTracks... std::vector<const Track*> origTracks;
  std::vector<xAODVertex_pair> myxAODVertices;

  std::vector<Trk::TrackToVtxLink*> myTrackToVtxLinks;

  xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer;
  xAOD::VertexAuxContainer* theVertexAuxContainer =
    new xAOD::VertexAuxContainer;
  theVertexContainer->setStore(theVertexAuxContainer);

  Amg::Vector3D actualVertex;

  std::map<const Trk::ITrackLink*, Trk::TrackToVtxLink*> TrackLinkOf;
  // create a map between ITrackLink* and TrackToVtxLink*
  std::vector<const Trk::ITrackLink*>::const_iterator trkbegin =
    origTracks.begin();
  std::vector<const Trk::ITrackLink*>::const_iterator trkend = origTracks.end();

  for (std::vector<const Trk::ITrackLink*>::const_iterator trkiter = trkbegin;
       trkiter != trkend;
       ++trkiter) {
    Trk::TrackToVtxLink* newTrkToVtxLink(
      new Trk::TrackToVtxLink(new std::vector<xAOD::Vertex*>));
    // makePrivateStore() is called for each vertex to add in iteration
    TrackLinkOf[*trkiter] = newTrkToVtxLink;
    myTrackToVtxLinks.push_back(newTrkToVtxLink);
  }

  // prepare iterators for tracks only necessary for seeding
  std::vector<const Trk::ITrackLink*>::iterator seedtrkbegin =
    seedTracks.begin();
  std::vector<const Trk::ITrackLink*>::iterator seedtrkend = seedTracks.end();
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{ m_beamSpotKey, ctx };
  int iteration = 0;

  int nNoCompatibleTracks(0);
  int nContamintationCut(0);
  int nWithin3sigma(0);

  unsigned int seedtracknumber = seedTracks.size();
  do {
    if (seedtracknumber == 0) {
      ATH_MSG_DEBUG("No tracks available after track selection for seeding. No "
                    "finding done.");
      break;
    }
    iteration += 1;
    ATH_MSG_DEBUG("Starting iteration number " << iteration << " with "
                                               << seedtracknumber
                                               << " seed tracks.");
    // now use all the perigee parameters you have so far
    if (m_realMultiVertex) {
      trkbegin = origTracks.begin();
      trkend = origTracks.end();
    } else {
      trkbegin = seedTracks.begin();
      trkend = seedTracks.end();
    }
    std::vector<const Trk::TrackParameters*> perigeeList;
    for (std::vector<const Trk::ITrackLink*>::iterator seedtrkAtVtxIter =
           seedtrkbegin;
         seedtrkAtVtxIter != seedtrkend;
         ++seedtrkAtVtxIter) {
      perigeeList.push_back((*seedtrkAtVtxIter)->parameters());
    }
    xAOD::Vertex* constraintVertex = nullptr;

    if (m_useBeamConstraint) {
      constraintVertex = new xAOD::Vertex();
      constraintVertex->makePrivateStore();
      constraintVertex->setPosition(beamSpotHandle->beamVtx().position());
      constraintVertex->setCovariancePosition(
        beamSpotHandle->beamVtx().covariancePosition());
      constraintVertex->setFitQuality(
        beamSpotHandle->beamVtx().fitQuality().chiSquared(),
        beamSpotHandle->beamVtx().fitQuality().doubleNumberDoF());
      std::pair<Amg::Vector3D, Amg::MatrixX> seedConstraintVertex =
        m_analyticSeedFinder->findAnalyticSeed(perigeeList, constraintVertex);
      actualVertex = seedConstraintVertex.first;
      if (m_useSeedConstraint) {
        constraintVertex->setPosition(seedConstraintVertex.first);
        constraintVertex->setCovariancePosition(seedConstraintVertex.second);
      }
    } else {
      actualVertex = m_analyticSeedFinder->findSeed(perigeeList);
      Amg::MatrixX looseConstraintCovariance(3, 3);
      looseConstraintCovariance.setIdentity();
      looseConstraintCovariance = looseConstraintCovariance * 1e+8;
      constraintVertex = new xAOD::Vertex();
      constraintVertex->makePrivateStore();
      constraintVertex->setPosition(actualVertex);
      constraintVertex->setCovariancePosition(looseConstraintCovariance);
      constraintVertex->setFitQuality(0., -3.);
    }
    // TODO: memory leak here with theconstraint when the loop breaks?
    if (actualVertex.z() == 0.) {
      ATH_MSG_DEBUG("No seed found: no further primary vertex finding "
                    "performed on this event");
      ATH_MSG_DEBUG(perigeeList.size()
                    << " tracks passed to seed finder, but no seed returned.");
      // TODO: Do I need this?
      delete constraintVertex;
      constraintVertex = nullptr;
      break;
    }
    // new xAOD::Vertex with this
    xAOD::Vertex* actualcandidate = new xAOD::Vertex;
    actualcandidate->makePrivateStore();
    actualcandidate->setVertexType(xAOD::VxType::NotSpecified);
    // to mimic the initialization present in the old EDM
    // constructor now add decorations!
    MvfFitInfo(*actualcandidate) =
      new Trk::MvfFitInfo(constraintVertex,
                          new Amg::Vector3D(actualVertex),
                          new Amg::Vector3D(actualVertex));
    isInitialized(*actualcandidate) = false;
    std::vector<Trk::VxTrackAtVertex*> vector_of_tracks(0);
    VTAV(*actualcandidate) =
      vector_of_tracks; // TODO: maybe needed before push_back?
    // now iterate on all tracks and find out if they are sufficiently close to
    // the found vertex


    for (std::vector<const Trk::ITrackLink*>::const_iterator trkiter = trkbegin;
         trkiter != trkend;
         ++trkiter) {
      if (std::fabs(estimateDeltaZ(*(*trkiter)->parameters(), actualVertex)) <
          m_TracksMaxZinterval) {
        const double thisTracksSignificance = ipSignificance(
          (*trkiter)->parameters(), &actualVertex); // calculate significance
        if (thisTracksSignificance < m_tracksMaxSignificance) {
          // accessing corresponding link to vertices
          Trk::TrackToVtxLink* actuallink = TrackLinkOf[*trkiter];
          std::vector<xAOD::Vertex*>* actualvtxlink = actuallink->vertices();
          // adding vertex to candidates of track
          actualvtxlink->push_back(actualcandidate);
          VTAV(*actualcandidate)
            .push_back(
              new Trk::MVFVxTrackAtVertex((*trkiter)->clone(), actuallink));
        }
      }
    }
    ATH_MSG_DEBUG(
      VTAV(*actualcandidate).size()
      << " tracks added to vertex candidate for IP significance less than "
      << m_tracksMaxSignificance << " within " << m_TracksMaxZinterval
      << " mm of seed position.");
    // now consider to recovery from the case where no tracks were added to the
    // vertex
    if (VTAV(*actualcandidate).empty()) {
      // you need to define a new seed (because the old one is probably in
      // between two ones...)
      double zdistance = 1e8;
      const Trk::ITrackLink* nearestTrack = nullptr;
      for (std::vector<const Trk::ITrackLink*>::const_iterator seedtrkiter =
             seedtrkbegin;
           seedtrkiter != seedtrkend;
           ++seedtrkiter) {
        if (std::fabs((*seedtrkiter)->parameters()->position()[Trk::z] -
                      actualVertex.z()) < zdistance) {
          zdistance =
            std::fabs((*seedtrkiter)->parameters()->position()[Trk::z] -
                      actualVertex.z());
          nearestTrack = *seedtrkiter;
        }
      }
      if (nearestTrack) {
        double newz = (nearestTrack->parameters())->position()[Trk::z];
        xAOD::Vertex* oldcandidate =
          actualcandidate; // to placehold old pointers

        actualVertex = Amg::Vector3D(0., 0., newz);
        actualcandidate = new xAOD::Vertex();
        actualcandidate->makePrivateStore();
        actualcandidate->setVertexType(xAOD::VxType::NotSpecified);
        // to mimic the initialization present in the old
        // EDM constructor

        // TODO: Think about where everything is deleted! Does Trk::MvfFitInfo
        // destructor and do MVFVxTrackAtVertex
        // destructors get called when actualcandidate gets deleted?
        // now add decorations!
        MvfFitInfo(*actualcandidate) =
          new Trk::MvfFitInfo(new xAOD::Vertex(*constraintVertex),
                              new Amg::Vector3D(actualVertex),
                              new Amg::Vector3D(actualVertex));
        isInitialized(*actualcandidate) = false;
        VTAV(*actualcandidate) =
          vector_of_tracks; // TODO: maybe needed before push_back?

        releaseCandidate(oldcandidate);

        for (std::vector<const Trk::ITrackLink*>::const_iterator trkiter =
               trkbegin;
             trkiter != trkend;
             ++trkiter) {

          if (std::fabs(estimateDeltaZ(*(*trkiter)->parameters(),
                                       actualVertex)) < m_TracksMaxZinterval) {
            const double thisTracksSignificance =
              ipSignificance((*trkiter)->parameters(),
                             &actualVertex); // calculate significance
            if (thisTracksSignificance < m_tracksMaxSignificance) {
              // accessing corresponding link to vertices
              Trk::TrackToVtxLink* actuallink = TrackLinkOf[*trkiter];
              std::vector<xAOD::Vertex*>* actualvtxlink =
                actuallink->vertices();
              // adding vertex to candidates of track
              actualvtxlink->push_back(actualcandidate);
              VTAV(*actualcandidate)
                .push_back(
                  new Trk::MVFVxTrackAtVertex((*trkiter)->clone(), actuallink));
            }
          }
        }

        if (VTAV(*actualcandidate).empty()) {
          ATH_MSG_DEBUG("No tracks found near seed, while at least one track "
                        "was expected.");
          delete actualcandidate;
          break;
        }
      } else {
        ATH_MSG_DEBUG("Nearest track to seed is missing.");
        delete actualcandidate;
        break;
      }
    }
    m_MultiVertexFitter->addVtxTofit(actualcandidate);
    ATH_MSG_DEBUG("After fit the current candidate has z: "
                  << actualcandidate->position()[Trk::z]);
    // get link to the tracks (they are now all properly in the
    // std::vector<Trk::VxTrackAtVertex> of the xAOD::Vertex)
    // TODO: maybe I shouldn't be using xAOD::Vertex vector at all for
    // VxTrackAtVertex...
    std::vector<Trk::VxTrackAtVertex*>::iterator trkAtVtxbegin =
      VTAV(*actualcandidate).begin();
    std::vector<Trk::VxTrackAtVertex*>::iterator trkAtVtxend =
      VTAV(*actualcandidate).end();
    // now check that there is at least one track added to the fit
    //(this is not always the case because only tracks above a certain
    //compatibility threshold are considered)
    bool atleastonecompatibletrack = false;
    int numberOfCompatibleTracks = 0;
    for (std::vector<Trk::VxTrackAtVertex*>::iterator trkAtVtxIter =
           trkAtVtxbegin;
         trkAtVtxIter != trkAtVtxend;
         ++trkAtVtxIter) {
      if (((*trkAtVtxIter)->vtxCompatibility() < m_maxVertexChi2 &&
           m_useFastCompatibility) ||
          ((*trkAtVtxIter)->weight() > m_minweight &&
           (*trkAtVtxIter)->trackQuality().chiSquared() < m_maxVertexChi2 &&
           !m_useFastCompatibility)) {
        const Trk::ITrackLink* foundTrack = nullptr;
        for (std::vector<const Trk::ITrackLink*>::const_iterator seedtrkiter =
               seedtrkbegin;
             seedtrkiter != seedtrkend;
             ++seedtrkiter) {
          if ((*seedtrkiter)->parameters() ==
              (*trkAtVtxIter)->trackOrParticleLink()->parameters()) {
            foundTrack = *seedtrkiter;
          }
        }
        if (foundTrack != nullptr) {
          atleastonecompatibletrack = true;
          numberOfCompatibleTracks += 1;
          if (m_addSingleTrackVertices) {
            if (m_useBeamConstraint)
              break;
            if (numberOfCompatibleTracks > 1 && (!m_useBeamConstraint))
              break;
          } else {
            if (numberOfCompatibleTracks > 1)
              break;
          }
        }
      }
    }

    bool newVertexIsFine = false;
    if (m_addSingleTrackVertices) {
      if (m_useBeamConstraint) {
        if (numberOfCompatibleTracks > 0) {
          newVertexIsFine = true;
        }
      } else {
        if (numberOfCompatibleTracks > 1) {
          newVertexIsFine = true;
        }
      }
    } else {
      if (numberOfCompatibleTracks > 1) {
        newVertexIsFine = true;
      }
    }
    // this now should be so powerful to do everything by itself
    // problem now is to delete the really compatible tracks to this fit from
    // the tracks which still remain to be fitted
    if (atleastonecompatibletrack) {
      for (std::vector<Trk::VxTrackAtVertex*>::iterator trkAtVtxIter =
             trkAtVtxbegin;
           trkAtVtxIter != trkAtVtxend;
           ++trkAtVtxIter) {
        // for now using the compatibility at stage before end...
        if (((*trkAtVtxIter)->vtxCompatibility() < m_maxVertexChi2 &&
             m_useFastCompatibility) ||
            ((*trkAtVtxIter)->weight() > m_minweight &&
             (*trkAtVtxIter)->trackQuality().chiSquared() < m_maxVertexChi2 &&
             !m_useFastCompatibility)) {

          std::vector<const Trk::ITrackLink*>::iterator foundTrack = seedtrkend;
          for (std::vector<const Trk::ITrackLink*>::iterator seedtrkiter =
                 seedtrkbegin;
               seedtrkiter != seedtrkend;
               ++seedtrkiter) {
            if ((*seedtrkiter)->parameters() ==
                (*trkAtVtxIter)->trackOrParticleLink()->parameters()) {
              foundTrack = seedtrkiter;
            }
          }
          if (foundTrack != seedtrkend) {
            seedTracks.erase(foundTrack);

            // update end and begin??? should I? yes, he can copy, regenerate,
            // you don't know!
            seedtrkbegin = seedTracks.begin();
            seedtrkend = seedTracks.end();
          }
        }
      }
    } else { // no compatible track found...
      // in this case put out the highest seeding track which didn't give any
      // good result...
      double highestcompatibility = 0;
      Trk::VxTrackAtVertex* trackHighestCompatibility = nullptr;

      for (std::vector<Trk::VxTrackAtVertex*>::iterator trkAtVtxIter =
             trkAtVtxbegin;
           trkAtVtxIter != trkAtVtxend;
           ++trkAtVtxIter) {
        const Trk::ITrackLink* foundTrack = nullptr;
        for (std::vector<const Trk::ITrackLink*>::const_iterator seedtrkiter =
               seedtrkbegin;
             seedtrkiter != seedtrkend;
             ++seedtrkiter) {
          if ((*seedtrkiter)->parameters() ==
              (*trkAtVtxIter)->trackOrParticleLink()->parameters()) {
            foundTrack = *seedtrkiter;
          }
        }
        if (foundTrack != nullptr) {
          double compatibility = (*trkAtVtxIter)->vtxCompatibility();
          if (compatibility > highestcompatibility) {
            highestcompatibility = compatibility;
            trackHighestCompatibility = *trkAtVtxIter;
          }
        }
      }
      if (trackHighestCompatibility != nullptr) {
        std::vector<const Trk::ITrackLink*>::iterator foundTrack = seedtrkend;
        for (std::vector<const Trk::ITrackLink*>::iterator seedtrkiter =
               seedtrkbegin;
             seedtrkiter != seedtrkend;
             ++seedtrkiter) {
          if ((*seedtrkiter)->parameters() ==
              trackHighestCompatibility->trackOrParticleLink()->parameters()) {
            foundTrack = seedtrkiter;
          }
        }
        if (foundTrack != seedtrkend) {
          seedTracks.erase(foundTrack);
          seedtrkbegin = seedTracks.begin();
          seedtrkend = seedTracks.end();
        } else {
          ATH_MSG_FATAL("Cannot find previously determined track ");
          throw;
        }
      } else {
        // alternative method: delete seed track nearest in z to the seed
        double zdistance = 1e8;
        const Trk::ITrackLink* nearestTrack = nullptr;
        for (std::vector<const Trk::ITrackLink*>::const_iterator seedtrkiter =
               seedtrkbegin;
             seedtrkiter != seedtrkend;
             ++seedtrkiter) {
          if (std::fabs((*seedtrkiter)->parameters()->position()[Trk::z] -
                        actualVertex.z()) < zdistance) {
            zdistance =
              std::fabs((*seedtrkiter)->parameters()->position()[Trk::z] -
                        actualVertex.z());
            nearestTrack = *seedtrkiter;
          }
        }
        if (nearestTrack != nullptr) {
          std::vector<const Trk::ITrackLink*>::iterator foundTrackToDelete =
            std::find(seedtrkbegin, seedtrkend, nearestTrack);
          if (foundTrackToDelete != seedtrkend) {
            seedTracks.erase(foundTrackToDelete);
            seedtrkbegin = seedTracks.begin();
            seedtrkend = seedTracks.end();
          } else {
            ATH_MSG_DEBUG("No nearest track found while it was expected.");
            break;
          }
        } else {
          ATH_MSG_DEBUG("No further seeding track was found (3 methods used) "
                        "while it was expected.");
          break;
        }
      }
    }

    ///////////////
    // now break the cycle if you didn't diminish the number of seeds...
    ATH_MSG_DEBUG("Remaining seeds: " << seedTracks.size() << " previous round "
                                      << seedtracknumber);
    bool deleteLastVertex = false;
    if (!newVertexIsFine) {
      deleteLastVertex = true;
      nNoCompatibleTracks++;
      ATH_MSG_DEBUG("Vertex has no compatible tracks");
    } else {
      double contamination = 0.;
      double contaminationNum = 0;
      double contaminationDeNom = 0;
      std::vector<Trk::VxTrackAtVertex*>::iterator TRKtrkbegin(
        VTAV(*actualcandidate).begin());
      std::vector<Trk::VxTrackAtVertex*>::iterator TRKtrkend(
        VTAV(*actualcandidate).end());
      for (std::vector<Trk::VxTrackAtVertex*>::iterator TRKtrkiter =
             TRKtrkbegin;
           TRKtrkiter != TRKtrkend;
           ++TRKtrkiter) {
        double trackWeight = (*TRKtrkiter)->weight();
        contaminationNum += trackWeight * (1. - trackWeight);
        contaminationDeNom += trackWeight * trackWeight;
      }
      if (contaminationDeNom > 0) {
        contamination = contaminationNum / contaminationDeNom;
      }
      if (contamination > m_maximumVertexContamination) {
        deleteLastVertex = true;
        nContamintationCut++;
        ATH_MSG_DEBUG("Vertex failed contamination cut");
      }
      // now try to understand if the vertex was merged with another one...
      const auto& candidatePosition = actualcandidate->position();
      const auto& candidateZPosition = candidatePosition[Trk::z];
      const auto& candidatePositionCovariance =
        actualcandidate->covariancePosition();
      const auto& candidateZPositionCovariance =
        candidatePositionCovariance(Trk::z, Trk::z);
      for (const auto& thisVertex : myxAODVertices) {
        const auto& thisVertexPosition = (thisVertex.second)->position();
        const auto& thisVertexZPosition = thisVertexPosition[Trk::z];
        const auto& thisVertexPositionCovariance =
          thisVertex.second->covariancePosition();
        const auto& thisVertexZPositionCovariance =
          thisVertexPositionCovariance(Trk::z, Trk::z);
        const auto deltaPosition = thisVertexPosition - candidatePosition;
        const auto deltaZPosition = thisVertexZPosition - candidateZPosition;
        const auto sumZCovSq =
          thisVertexZPositionCovariance + candidateZPositionCovariance;

        // in case of no beam spot constraint you should use the full 3d
        // significance on the distance
        double dependence = 0;
        if (!m_do3dSplitting) {
          if (sumZCovSq > 0.) {
            dependence = std::fabs(deltaZPosition) / std::sqrt(sumZCovSq);
          } else {
            dependence = 0.;
          }
        } else {
          Amg::MatrixX sumCovariances =
            thisVertexPositionCovariance + candidatePositionCovariance;
          sumCovariances = sumCovariances.inverse().eval();
          Amg::Vector3D hepVectorPosition;
          hepVectorPosition[0] = deltaPosition.x();
          hepVectorPosition[1] = deltaPosition.y();
          hepVectorPosition[2] = deltaPosition.z();
          dependence = std::sqrt(
            hepVectorPosition.dot(sumCovariances * hepVectorPosition));
        }
        if (dependence < m_cutVertexDependence) {
          deleteLastVertex = true;
          nWithin3sigma++;
          ATH_MSG_DEBUG(
            "Vertex failed significance (cut vertex dependence) test");
          break;
        }
      }
    }
    ////////////
    // Ok all tracks in seed were deleted. You can go ahead and discover further
    // vertices... please clean the track to vertices links before (required by
    // real multivertexfit)
    if (deleteLastVertex) {
      std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkAtVtxBegin =
        VTAV(*actualcandidate).begin();
      std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkAtVtxEnd =
        VTAV(*actualcandidate).end();
      for (std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkIterator =
             MVFtrkAtVtxBegin;
           MVFtrkIterator != MVFtrkAtVtxEnd;
           ++MVFtrkIterator) {
        (static_cast<Trk::MVFVxTrackAtVertex*>(*MVFtrkIterator))
          ->linkToVertices()
          ->vertices()
          ->pop_back();
      }
      seedtracknumber = seedTracks.size();
      m_MultiVertexFitter->addVtxTofit(
        actualcandidate); // TODO: I think this is fine still, but think about
                          // it more
      releaseCandidate(actualcandidate);
    } else {
      seedtracknumber = seedTracks.size();
      myxAODVertices.emplace_back(0, actualcandidate);
    }
  } while (((m_addSingleTrackVertices && !seedTracks.empty()) ||
            ((!m_addSingleTrackVertices) && seedTracks.size() > 1)) &&
           iteration < m_maxIterations);
  if (iteration >= m_maxIterations) {
    ATH_MSG_WARNING("Maximum number of iterations ("
                    << m_maxIterations
                    << ") reached; to reconstruct more vertices, set "
                       "maxIterations to a higher value.");
  }

  ATH_MSG_DEBUG(
    "Vertices deleted for no compatible tracks: " << nNoCompatibleTracks);
  ATH_MSG_DEBUG(
    "Vertices deleted for contamination cut: " << nContamintationCut);
  ATH_MSG_DEBUG(
    "Vertices deleted for proximity to previous: " << nWithin3sigma);

  ATH_MSG_DEBUG("Primary vertex finding complete with "
                << iteration << " iterations and " << myxAODVertices.size()
                << " vertices found.");

  // correction of a bug: you can estimate the probability of being
  // the primary interaction vertex only after the whole multivertexfit
  // is finished!!! (the first is influenced by the fit of the second and so
  // on...)
  std::vector<xAODVertex_pair>::iterator vtxBegin = myxAODVertices.begin();
  std::vector<xAODVertex_pair>::iterator vtxEnd = myxAODVertices.end();
  // To make sure that the right tracks are in the
  // std::vector<Trk::VxTrackAtVertex> of each vertex - up until now, they are
  // kept in the VTAV decoration
  for (std::vector<xAODVertex_pair>::iterator vtxIter = vtxBegin;
       vtxIter != vtxEnd;
       ++vtxIter) {
    xAOD::Vertex* cand = vtxIter->second;
    std::vector<Trk::VxTrackAtVertex>* tracksOfVertex =
      &(cand->vxTrackAtVertex());
    tracksOfVertex->clear();
    std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkBegin =
      VTAV(*cand).begin();
    std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkEnd = VTAV(*cand).end();
    for (std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkIter = MVFtrkBegin;
         MVFtrkIter != MVFtrkEnd;
         ++MVFtrkIter) {
      tracksOfVertex->push_back(**MVFtrkIter);
    }
  }
  // before filling the container, you have to decide what is your most probable
  // signal vertex
  for (std::vector<xAODVertex_pair>::iterator vtxIter = vtxBegin;
       vtxIter != vtxEnd;
       ++vtxIter) {
    (*vtxIter).first = estimateSignalCompatibility((*vtxIter).second);
  }
  std::sort(myxAODVertices.begin(), myxAODVertices.end());
  
  if (myxAODVertices.empty()) {
    ATH_MSG_WARNING(
      "No vertices found: returning a place-holder at the beam spot center.");
    xAOD::Vertex* beamspotCandidate = new xAOD::Vertex;
    beamspotCandidate->makePrivateStore();
    beamspotCandidate->setPosition(beamSpotHandle->beamVtx().position());
    beamspotCandidate->setCovariancePosition(
      beamSpotHandle->beamVtx().covariancePosition());
    beamspotCandidate->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
    // TODO: I don't need to set fitQuality too do I?
    myxAODVertices.emplace_back(0, beamspotCandidate);
  }
  vtxBegin = myxAODVertices.begin();
  vtxEnd = myxAODVertices.end();
  for (std::vector<xAODVertex_pair>::const_iterator vtxIter = vtxBegin;
       vtxIter != vtxEnd;
       ++vtxIter) {
    xAOD::Vertex* cand = vtxIter->second;
    std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkBegin =
      VTAV(*cand).begin();
    std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkEnd = VTAV(*cand).end();
    // TODO: here, I must clean up VTAV decoration separately from vector of
    // VxTrackAtVertex
    for (std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkIter = MVFtrkBegin;
         MVFtrkIter != MVFtrkEnd;
         ++MVFtrkIter) {
      // setting link to TrackToVtxLink to 0 (all TrackToVtxLink will be deleted
      // some lines later)
      (static_cast<Trk::MVFVxTrackAtVertex*>(*MVFtrkIter))
        ->setLinkToVertices(nullptr);
      delete *MVFtrkIter;
      *MVFtrkIter = 0;
    }
    // delete VTAV( *cand );
    delete MvfFitInfo(*cand);
    std::vector<Trk::VxTrackAtVertex>::iterator trkAtVtxBegin =
      cand->vxTrackAtVertex().begin();
    std::vector<Trk::VxTrackAtVertex>::iterator trkAtVtxEnd =
      cand->vxTrackAtVertex().end();
    for (std::vector<Trk::VxTrackAtVertex>::iterator trkAtVtxIter =
           trkAtVtxBegin;
         trkAtVtxIter != trkAtVtxEnd;) {
      // cleaning up incompatible vertices
      if (((*trkAtVtxIter).vtxCompatibility() > m_maxVertexChi2 &&
           m_useFastCompatibility) ||
          (((*trkAtVtxIter).weight() < m_minweight ||
            (*trkAtVtxIter).trackQuality().chiSquared() > m_maxVertexChi2) &&
           !m_useFastCompatibility)) {
        trkAtVtxIter = cand->vxTrackAtVertex().erase(trkAtVtxIter);
        trkAtVtxEnd = cand->vxTrackAtVertex().end();
      } else {
        ++trkAtVtxIter;
      }
    }
    theVertexContainer->push_back(cand);
  }
  // If track links are to xAOD::TrackParticles, set the links directly in
  // xAOD::Vertex with their weights Needed for weight calculator in sorting
  // tool
  xAOD::VertexContainer::iterator vxBegin = theVertexContainer->begin();
  xAOD::VertexContainer::iterator vxEnd = theVertexContainer->end();
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
    for (std::vector<Trk::VxTrackAtVertex>::iterator tracksIter = tracksBegin;
         tracksIter != tracksEnd;
         ++tracksIter) {

      // only set link if track link is to an xAOD::TrackParticle
      Trk::LinkToXAODTrackParticle* linkToXAODTP = nullptr;
      Trk::ITrackLink* tmpLink = (*tracksIter).trackOrParticleLink();
      if (tmpLink->type() == Trk::ITrackLink::ToxAODTrackParticle) {
        linkToXAODTP = static_cast<Trk::LinkToXAODTrackParticle*>(tmpLink);
      }
      if (linkToXAODTP) {
        (*vxIter)->addTrackAtVertex(*linkToXAODTP, (*tracksIter).weight());
      }
    }
  }
  std::vector<Trk::TrackToVtxLink*>::iterator begin = myTrackToVtxLinks.begin();
  std::vector<Trk::TrackToVtxLink*>::iterator end = myTrackToVtxLinks.end();
  // delete all TrackToVtxLink objects
  for (std::vector<Trk::TrackToVtxLink*>::iterator iterator = begin;
       iterator != end;
       ++iterator) {
    delete *iterator;
  }
  //---- add dummy vertex at the end
  //------------------------------------------------------//
  //---- if one or more vertices are already there: let dummy have same position
  //as primary vertex
  if (!theVertexContainer->empty()) {
    xAOD::Vertex* primaryVtx = theVertexContainer->front();
    if (!primaryVtx->vxTrackAtVertex().empty()) {
      primaryVtx->setVertexType(xAOD::VxType::PriVtx);
      xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
      theVertexContainer->push_back(
        dummyxAODVertex); // have to add vertex to container here first so it
                          // can use its aux store
      dummyxAODVertex->setPosition(primaryVtx->position());
      dummyxAODVertex->setCovariancePosition(primaryVtx->covariancePosition());
      dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
      dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
    } else {
      primaryVtx->setVertexType(xAOD::VxType::NoVtx);
    }
  }
  //---- if no vertex is there let dummy be at beam spot
  else if (theVertexContainer->empty()) {
    xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
    theVertexContainer->push_back(
      dummyxAODVertex); // have to add vertex to container here first so it can
                        // use its aux store
    dummyxAODVertex->setPosition(beamSpotHandle->beamVtx().position());
    dummyxAODVertex->setCovariancePosition(
      beamSpotHandle->beamVtx().covariancePosition());
    dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
    dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
  }
  // loop over the pile up to set it as pile up (EXCLUDE first and last vertex:
  // loop from 1 to size-1)
  for (unsigned int i = 0; i < theVertexContainer->size() - 1; i++) {
    if (i > 0) {
      (*theVertexContainer)[i]->setVertexType(xAOD::VxType::PileUp);
    }
  }
  return std::make_pair(theVertexContainer, theVertexAuxContainer);
}

double
InDetAdaptiveMultiPriVxFinderTool::estimateSignalCompatibility(
  xAOD::Vertex* mycand) const
{
  // TODO: put this in a better place
  // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
  // For optimization of access speed
  static const xAOD::Vertex::Decorator<std::vector<Trk::VxTrackAtVertex*>> VTAV(
    "VTAV");

  std::vector<Trk::VxTrackAtVertex*>::iterator begintracks =
    VTAV(*mycand).begin();
  std::vector<Trk::VxTrackAtVertex*>::iterator endtracks = VTAV(*mycand).end();

  if (m_selectiontype == 0) {
    // use just sum over pt squared
    // first get all the variables you need for to discriminate between signal
    // and minimum bias event

    double total_pt_squared = 0;
    int total_num_tracks = 0;

    for (std::vector<Trk::VxTrackAtVertex*>::iterator i = begintracks;
         i != endtracks;
         i++) {

      if (((*i)->vtxCompatibility() < m_finalCutMaxVertexChi2 &&
           m_useFastCompatibility) ||
          ((*i)->weight() > m_minweight &&
           (*i)->trackQuality().chiSquared() < m_finalCutMaxVertexChi2 &&
           !m_useFastCompatibility)) {
        const Trk::TrackParameters* perigee(nullptr);
        if ((*i)->perigeeAtVertex() != nullptr) {
          perigee = (*i)->perigeeAtVertex();
        } else {
          perigee = (*i)->initialPerigee();
        }
        if (perigee == nullptr) {
          ATH_MSG_ERROR(
            "Neutrals are not supported. Skipping track in pT calculation...");
          continue;
        }
        total_pt_squared +=
          std::pow(std::fabs(1. / perigee->parameters()[Trk::qOverP]) *
                     sin(perigee->parameters()[Trk::theta]),
                   2);
        total_num_tracks += 1;
      }
    } // finishing iterating on VxTrackAtVertex associated to **vtxIter
      // xAOD::Vertex

    return total_pt_squared * std::sqrt((double)total_num_tracks);
  }
  if (m_selectiontype == 1) { // use NN
    ATH_MSG_WARNING("Only Selection Type 0 supported for MT");
  }
  return 0;
}

StatusCode
InDetAdaptiveMultiPriVxFinderTool::finalize()
{
  return StatusCode::SUCCESS;
}

void
InDetAdaptiveMultiPriVxFinderTool::printParameterSettings()
{
  ATH_MSG_DEBUG("Adaptive Multi-Vertex Finder: Parameter settings ");
  ATH_MSG_DEBUG(
    "Trackselection cuts handled by the TrackSelectorTool: " << m_trkFilter);
  ATH_MSG_DEBUG("Finder settings: ");
  ATH_MSG_DEBUG("Maximum distance to include track in simultaneous vertex "
                "fits: TracksMaxZinterval "
                << m_TracksMaxZinterval);
  ATH_MSG_DEBUG(
    "Seeding: minimum chi^2 for a track being an outlier:  maxVertexChi2 "
    << m_maxVertexChi2);
  ATH_MSG_DEBUG(
    "Signal identification: final cut on track chi2: finalCutMaxVertexChi2 = "
    << m_finalCutMaxVertexChi2);
  ATH_MSG_DEBUG(
    "Activate complete multi vertex fitting feature: realMultiVertex "
    << m_realMultiVertex);
  ATH_MSG_DEBUG("Merging vertices: upper cut on significance to merge two "
                "vertices: cutVertexDependence = "
                << m_cutVertexDependence);
  ATH_MSG_DEBUG("Maximum number of iterations (and vertices): maxIterations = "
                << m_maxIterations);
  ATH_MSG_DEBUG(
    "Selection type (0 is sqrt(Ntr)*Sum_{tr} pT^2): selectiontype = "
    << m_selectiontype);
  ATH_MSG_DEBUG("Use fast compatibility (if false use refitted chi2 instead of "
                "approximation): useFastCompatibility = "
                << m_useFastCompatibility);
  ATH_MSG_DEBUG("MinWeight (if track weight in the fit is lower, don't perform "
                "the Kalman Update) = "
                << m_minweight);
  ATH_MSG_DEBUG("");
}

void
InDetAdaptiveMultiPriVxFinderTool::SGError(const std::string& errService)
{
  ATH_MSG_FATAL(errService << " not found. Exiting !");
}

double
InDetAdaptiveMultiPriVxFinderTool::estimateDeltaZ(
  const Trk::TrackParameters& myPerigee,
  const Amg::Vector3D& myTransvVertex) const
{
  Amg::Vector3D lp = myTransvVertex;

  Amg::Vector3D expPoint;
  Amg::Vector3D predStatePosition = myPerigee.position();
  expPoint[0] = predStatePosition.x();
  expPoint[1] = predStatePosition.y();
  expPoint[2] = predStatePosition.z();

  // phi_v and functions
  double phi_v = myPerigee.parameters()[Trk::phi];
  double th = myPerigee.parameters()[Trk::theta];
  double tan_th = std::tan(th);
  double sin_phi_v = std::sin(phi_v);
  double cos_phi_v = std::cos(phi_v);
  double q_ov_p = myPerigee.parameters()[Trk::qOverP];

  // momentum
  Amg::Vector3D expMomentum;
  expMomentum[0] = phi_v;
  expMomentum[1] = th;
  expMomentum[2] = q_ov_p;

  double X = expPoint[0] - lp.x();
  double Y = expPoint[1] - lp.y();

  return expPoint[2] - lp.z() - 1. / tan_th * (X * cos_phi_v + Y * sin_phi_v);
}

double
InDetAdaptiveMultiPriVxFinderTool::ipSignificance(
  const Trk::TrackParameters* params,
  const Amg::Vector3D* vertex) const
{
  xAOD::Vertex v;
  v.makePrivateStore();
  v.setPosition(*vertex);
  v.setCovariancePosition(AmgSymMatrix(3)::Zero(3, 3));
  v.setFitQuality(0., 0.);

  double significance = 0.0;
  const Trk::ImpactParametersAndSigma* ipas =
    m_ipEstimator->estimate(params, &v);
  if (ipas != nullptr) {
    if (ipas->sigmad0 > 0 && ipas->sigmaz0 > 0) {
      significance = std::sqrt(std::pow(ipas->IPd0 / ipas->sigmad0, 2) +
                               std::pow(ipas->IPz0 / ipas->sigmaz0, 2));
    }
    delete ipas;
  }
  return significance;
}

void
InDetAdaptiveMultiPriVxFinderTool::releaseCandidate(
  xAOD::Vertex*& candidate) const
{
  if (candidate == nullptr)
    return;

  // decorators
  static const xAOD::Vertex::Decorator<Trk::MvfFitInfo*> MvfFitInfo(
    "MvfFitInfo");
  static const xAOD::Vertex::Decorator<std::vector<Trk::VxTrackAtVertex*>> VTAV(
    "VTAV");

  if (VTAV.isAvailable(*candidate)) {
    for (auto tav : VTAV(*candidate)) {
      if (tav == nullptr)
        continue;
      (static_cast<Trk::MVFVxTrackAtVertex*>(tav))->setLinkToVertices(nullptr);
      delete tav;
      tav = nullptr;
    }
    VTAV(*candidate).clear();
  }

  if (MvfFitInfo.isAvailable(*candidate) && MvfFitInfo(*candidate) != nullptr) {
    delete MvfFitInfo(*candidate);
    MvfFitInfo(*candidate) = nullptr;
  }

  delete candidate;
  candidate = nullptr;
}
} // end namespace InDet
