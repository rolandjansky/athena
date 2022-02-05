/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticleCnvAlg.cxx 298303 2013-12-05 08:41:30Z emoyse $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):

#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthKey.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

// Local include(s):
#include "TrackParticleCnvAlg.h"
#include "xAODTrackingCnv/IRecTrackParticleContainerCnvTool.h"
#include "xAODTrackingCnv/ITrackCollectionCnvTool.h"


namespace xAODMaker {
TrackParticleCnvAlg::TrackParticleCnvAlg(const std::string& name,
                                         ISvcLocator* svcLoc)
  : AthReentrantAlgorithm(name, svcLoc)
  , m_particleCreator("Trk::TrackParticleCreatorTool/TrackParticleCreatorTool")
  , m_TrackCollectionCnvTool(
      "xAODMaker::TrackCollectionCnvTool/TrackCollectionCnvTool",
      this)
  , m_RecTrackParticleContainerCnvTool(
      "xAODMaker::RecTrackParticleContainerCnvTool/"
      "RecTrackParticleContainerCnvTool",
      this)
  , m_aod("TrackParticleCandidate")
  , m_tracks("Tracks")
  , m_xaodout("InDetTrackParticles")
  , m_xaodTrackParticlesout("ConvertedTrackParticleCandidate")
  , m_truthParticleLinkVec("xAODTruthLinks")
  , m_aodTruth("")
  , m_trackTruth("")
{
  declareProperty("AODContainerName", m_aod);
  declareProperty("xAODContainerName", m_xaodTrackParticlesout);
  declareProperty("TrackParticleCreator", m_particleCreator);
  declareProperty("AddTruthLink", m_addTruthLink = false);
  declareProperty("AODTruthContainerName", m_aodTruth);
  declareProperty("TrackTruthContainerName", m_trackTruth);
  declareProperty("xAODTruthLinkVector", m_truthParticleLinkVec);
  declareProperty("TrackContainerName", m_tracks);
  declareProperty("xAODTrackParticlesFromTracksContainerName", m_xaodout);
  declareProperty("ConvertTrackParticles", m_convertAODTrackParticles = true);
  declareProperty("ConvertTracks", m_convertTracks = false);
  declareProperty("TrackCollectionCnvTool", m_TrackCollectionCnvTool);
  declareProperty("RecTrackParticleContainerCnvTool",
                  m_RecTrackParticleContainerCnvTool);
  declareProperty("DoMonitoring", m_doMonitoring = false);
  declareProperty("AugmentObservedTracks", m_augmentObservedTracks = false, "augment observed tracks");
  declareProperty("TracksMapName", m_tracksMap, "name of observed tracks map saved in store");
}

StatusCode
TrackParticleCnvAlg::initialize()
{

  ATH_MSG_DEBUG("Initializing TrackParticleCnvAlg");
  ATH_MSG_DEBUG("AODContainerName  = " << m_aod.key());
  ATH_MSG_DEBUG("xAODContainerName = " << m_xaodTrackParticlesout.key());
  ATH_CHECK(m_particleCreator.retrieve());
  if (m_addTruthLink)
    ATH_CHECK(m_truthClassifier.retrieve());
  else
    m_truthClassifier.disable();
  ATH_CHECK(m_TrackCollectionCnvTool.retrieve(DisableTool{ !m_convertTracks }));
  ATH_CHECK(m_RecTrackParticleContainerCnvTool.retrieve(
    DisableTool{ !m_convertAODTrackParticles }));
  // to preserve the inisialised parameters of the ParticleCreatorTool:
  ATH_MSG_DEBUG("Overriding particle creator tool settings.");
  if (m_convertTracks)
    ATH_CHECK(
      m_TrackCollectionCnvTool->setParticleCreatorTool(&m_particleCreator));
  if (m_convertAODTrackParticles)
    ATH_CHECK(m_RecTrackParticleContainerCnvTool->setParticleCreatorTool(
      &m_particleCreator));

  ATH_CHECK(m_xaodout.initialize(m_convertTracks));
  ATH_CHECK(m_xaodTrackParticlesout.initialize(m_convertAODTrackParticles));
  ATH_CHECK(m_aod.initialize(m_convertAODTrackParticles));
  ATH_CHECK(m_tracks.initialize(m_convertTracks));
  ATH_CHECK(m_truthParticleLinkVec.initialize(m_addTruthLink));
  ATH_CHECK(
    m_aodTruth.initialize(m_addTruthLink && m_convertAODTrackParticles));
  ATH_CHECK(m_trackTruth.initialize(m_addTruthLink && m_convertTracks));

  // Retrieve monitoring tools if provided
  ATH_CHECK(m_trackMonitoringTool.retrieve(DisableTool{ !m_doMonitoring }));
  ATH_CHECK(m_monTool.retrieve(DisableTool{ !m_doMonitoring }));

  ATH_CHECK(m_tracksMap.initialize(m_augmentObservedTracks));

  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode
TrackParticleCnvAlg::execute(const EventContext& ctx) const
{

  const Rec::TrackParticleContainer* aod = nullptr;
  const TrackCollection* tracks = nullptr;
  const xAODTruthParticleLinkVector* truthLinks = nullptr;
  const TrackParticleTruthCollection* aodTruth = nullptr;
  const TrackTruthCollection* trackTruth = nullptr;
  const ObservedTrackMap* tracksMap = nullptr;

  //timer object for total execution time
  auto mnt_timer_Total  = Monitored::Timer<std::chrono::milliseconds>("TIME_Total");
  
  // Retrieve the AOD particles:
  if (m_convertAODTrackParticles) {
    SG::ReadHandle<Rec::TrackParticleContainer> rh_aod(m_aod, ctx);
    if (!rh_aod.isValid()) {
      ATH_MSG_ERROR(m_aod.key() << " not found");
      return StatusCode::FAILURE;
    } else {
      aod = rh_aod.cptr();
      ATH_MSG_VERBOSE("Got TrackParticleContainer with key " << m_aod.key()
                                                             << " found.");
    }
  }
  // Retrieve the Tracks:
  if (m_convertTracks) {
    SG::ReadHandle<TrackCollection> rh_tracks(m_tracks, ctx);
    if (!rh_tracks.isValid()) {
      ATH_MSG_ERROR(m_tracks.key() << " not found");
      return StatusCode::SUCCESS;
    } else {
      tracks = rh_tracks.cptr();
      ATH_MSG_VERBOSE("Got TrackCollection with key " << m_tracks.key()
                                                      << " found.");
    }
  }
  if (m_addTruthLink) {
    if (m_convertAODTrackParticles) {
      SG::ReadHandle<TrackParticleTruthCollection> rh_aodTruth(m_aodTruth, ctx);
      if (!rh_aodTruth.isValid()) {
        ATH_MSG_WARNING("No TrackParticleTruthCollection with key "
                        << m_aodTruth.key() << " found. Do nothing.");
        return StatusCode::SUCCESS;
      } else
        aodTruth = rh_aodTruth.cptr();
    }
    if (m_convertTracks) {
      SG::ReadHandle<TrackTruthCollection> rh_trackTruth(m_trackTruth, ctx);
      if (!rh_trackTruth.isValid()) {
        ATH_MSG_WARNING("No DetailedTrackTruthCollection with key "
                        << m_trackTruth.key() << " found. Do nothing.");
        return StatusCode::SUCCESS;
      } else
        trackTruth = rh_trackTruth.cptr();
    }

    SG::ReadHandle<xAODTruthParticleLinkVector> rh_truthParticleLinkVec(
      m_truthParticleLinkVec, ctx);
    if (!rh_truthParticleLinkVec.isValid()) {
      ATH_MSG_WARNING("No xAODTruthParticleLinkVector with key "
                      << m_truthParticleLinkVec.key() << " found. Do nothing.");
      return StatusCode::SUCCESS;
    } else
      truthLinks = rh_truthParticleLinkVec.cptr();
  }
  if (m_convertTracks) {
    SG::WriteHandle<xAOD::TrackParticleContainer> wh_xaodout(m_xaodout, ctx);
    ATH_CHECK(
      wh_xaodout.record(std::make_unique<xAOD::TrackParticleContainer>(),
                        std::make_unique<xAOD::TrackParticleAuxContainer>()));

    // Augment track particles with information from observer tool
    if (m_augmentObservedTracks){
      SG::ReadHandle<ObservedTrackMap> rh_tracksMap(m_tracksMap, ctx);
      if (!rh_tracksMap.isValid()) {
        ATH_MSG_ERROR(m_tracksMap.key() << " not found");
        return StatusCode::FAILURE;
      }
      else {
        tracksMap = rh_tracksMap.cptr();
        ATH_MSG_VERBOSE("Got ObservedTrackMap with key " << m_tracksMap.key()
                                                        << " found.");
      }

      convert(
        (*tracks), trackTruth, m_TrackCollectionCnvTool, wh_xaodout, truthLinks, tracksMap);
    }
    else{
      convert(
        (*tracks), trackTruth, m_TrackCollectionCnvTool, wh_xaodout, truthLinks);
    }
    // Monitor track parameters
    if (m_doMonitoring)
      m_trackMonitoringTool->monitor_tracks("Track", "Pass", *wh_xaodout);
  }
  if (m_convertAODTrackParticles) {
    SG::WriteHandle<xAOD::TrackParticleContainer> wh_xaodTrackParticlesout(
      m_xaodTrackParticlesout, ctx);
    ATH_CHECK(wh_xaodTrackParticlesout.record(
      std::make_unique<xAOD::TrackParticleContainer>(),
      std::make_unique<xAOD::TrackParticleAuxContainer>()));
    convert((*aod),
            aodTruth,
            m_RecTrackParticleContainerCnvTool,
            wh_xaodTrackParticlesout,
            truthLinks);
  }

  //extra scope needed to trigger the monitoring
  {auto monTime = Monitored::Group(m_monTool, mnt_timer_Total);}
  
  return StatusCode::SUCCESS;
}

template<typename CONT>
class AssociationHelper;

template<>
class AssociationHelper<TrackCollection>
{
public:
  AssociationHelper(const TrackCollection&, xAOD::TrackParticleContainer*) {}
  const Trk::Track* operator()(xAOD::TrackParticle* track_particle,
                               unsigned int) const
  {
    return track_particle->track();
  }
};

template<>
class AssociationHelper<Rec::TrackParticleContainer>
{
public:
  AssociationHelper(const Rec::TrackParticleContainer& cont_src,
                    xAOD::TrackParticleContainer* cont_dest)
    : m_contSrc(&cont_src)
  {
    if (cont_src.size() != cont_dest->size()) {
      std::stringstream message;
      message << __FILE__ << ":" << __LINE__
              << " Expected one-to-one conversion from AOD to xAOD "
                 "TrackParticles but sizes differ: "
              << cont_src.size() << " != " << cont_dest->size();
      throw std::runtime_error(message.str());
    }
  }

  const Rec::TrackParticle* operator()(xAOD::TrackParticle*,
                                       unsigned int idx) const
  {
    return m_contSrc->at(idx);
  }

private:
  const Rec::TrackParticleContainer* m_contSrc;
};

template<typename CONT, typename TRUTHCONT, typename CONVTOOL>
int
TrackParticleCnvAlg::convert(
  const CONT& container,
  const TRUTHCONT& truth,
  CONVTOOL& conv_tool,
  SG::WriteHandle<xAOD::TrackParticleContainer>& xaod,
  const xAODTruthParticleLinkVector* truthLinkVec,
  const ObservedTrackMap* obs_track_map /*=0*/) const
{
  // Create the xAOD container and its auxiliary store:

  // convert the track containers separately with the converting tools that are
  // also used by TrigHLTtoxAODTool
  ATH_MSG_DEBUG("calling the converting tool for " << xaod.name());
  // Augment track particles using track map if available
  if (obs_track_map){
    if (conv_tool->convertAndAugment(&container, xaod.ptr(), obs_track_map).isFailure()) {
      ATH_MSG_ERROR("Couldn't convert and augment aod to xaod ("
                    << xaod.name() << ") with the converting tool");
      return -1;
    }
  }
  else{
    if (conv_tool->convert(&container, xaod.ptr()).isFailure()) {
      ATH_MSG_ERROR("Couldn't convert aod to xaod ("
                    << xaod.name() << ") with the converting tool");
      return -1;
    }
  }
  // Create the xAOD objects:
  xAOD::TrackParticleContainer::iterator itr_xaod = xaod->begin();
  xAOD::TrackParticleContainer::iterator end_xaod = xaod->end();

  AssociationHelper<CONT> association_to_src(container, xaod.ptr());
  unsigned int trackCounter(0);
  // loop over AOD and converted xAOD for summary info and truth links
  for (; itr_xaod != end_xaod; ++itr_xaod) {
    // protect if something went wrong and there is no converted xaod equivalent
    
    if (!(*itr_xaod)) {
      ATH_MSG_WARNING("WTaF? Empty element in xAOD container!");
      continue;
    }

    xAOD::TrackParticle* particle = *itr_xaod;

    if (!particle) {
      ATH_MSG_WARNING("Failed to get an xAOD::TrackParticle");
      continue;
    }

    trackCounter++;
    if(msgLvl(MSG::DEBUG)){
      int npix, nsct, ntrt, npixh, nscth, npixshim, npixsplit;
      npix = nsct = ntrt = npixh = nscth = npixshim = npixsplit = -1;
      const Trk::Track *tr = particle->track();
      if (tr){
	const Trk::TrackSummary *ts = tr->trackSummary();
	if (ts){
	  npix = ts->get(Trk::numberOfPixelHits);
	  nsct = ts->get(Trk::numberOfSCTHits);
	  ntrt = ts->get(Trk::numberOfTRTHits);
	  nscth= ts->get(Trk::numberOfSCTHoles);
	  npixh= ts->get(Trk::numberOfPixelHoles);
	  npixshim = ts->get(Trk::numberOfInnermostPixelLayerSharedHits);
	  npixsplit= ts->get(Trk::numberOfPixelSplitHits);
	  
	}
      }
      msg() << MSG::DEBUG << "REGTEST: " << std::setw(5) << trackCounter
	    << "  pT:  " << std::setw(10) << particle->pt()
	    << "  eta: " << particle->eta()
	    << "  phi: " << particle->phi()
	    << "  d0:  " << particle->d0()
	    << "  z0:  " << particle->z0()
	    << "\t" << npix << "/" << nsct << "/" << ntrt << "/holes/" << npixh << "/" << nscth
	    << "/sharedIM/" << npixshim << "/pixsplit/" << npixsplit 
	    << endmsg;

    }
 

    //
    // --------- statistics
    //

    if (m_addTruthLink) {
      MCTruthPartClassifier::ParticleType type = MCTruthPartClassifier::Unknown;
      MCTruthPartClassifier::ParticleOrigin origin =
        MCTruthPartClassifier::NonDefined;
      float probability = -1.0;
      ElementLink<xAOD::TruthParticleContainer> link;

      ElementLink<CONT> tpLink(
        association_to_src(*itr_xaod, itr_xaod - xaod->begin()), container);
      if (!tpLink.isValid()) {
        ATH_MSG_WARNING("Failed to create ElementLink to Track/TrackParticle");
      } else {
        auto result = truth->find(tpLink);
        if (result == truth->end()) {
          ATH_MSG_WARNING(
            "Failed find truth associated with Track/TrackParticle");
        } else {
          // setTruthLink(link,result->second, type, origin);
          ATH_MSG_VERBOSE("Found track Truth: barcode  "
                          << result->second.particleLink().barcode() << " evt "
                          << result->second.particleLink().eventIndex());
          probability = result->second.probability();
          link = truthLinkVec->find(result->second.particleLink());
          if (link.isValid()) {
            ATH_MSG_DEBUG("Found matching xAOD Truth: barcode "
                          << (*link)->barcode() << " pt " << (*link)->pt()
                          << " eta " << (*link)->eta() << " phi "
                          << (*link)->phi());
            // if configured also get truth classification
            if (result->second.particleLink().cptr() &&
                !m_truthClassifier.empty()) {
#ifdef HEPMC3
              auto truthClass = m_truthClassifier->particleTruthClassifier(
                result->second.particleLink().scptr());
#else
              auto truthClass = m_truthClassifier->particleTruthClassifier(
                result->second.particleLink().cptr());
#endif
              type = truthClass.first;
              origin = truthClass.second;
              ATH_MSG_VERBOSE("Got truth type  " << static_cast<int>(type)
                                                 << "  origin "
                                                 << static_cast<int>(origin));
            }
          } else {
            if (result->second.particleLink().barcode() > 0) {
              ATH_MSG_WARNING("No associated xAOD truth for valid truth link "
                              << result->second.particleLink());
            }
          }
        }
      }
      static const SG::AuxElement::Accessor<
        ElementLink<xAOD::TruthParticleContainer>>
        theLink("truthParticleLink");
      static const SG::AuxElement::Accessor<float> theProbability(
        "truthMatchProbability");
      theLink(*particle) = link;
      theProbability(*particle) = probability;
      if (!m_truthClassifier.empty()) {
        static const SG::AuxElement::Accessor<int> theType("truthType");
        static const SG::AuxElement::Accessor<int> theOrigin("truthOrigin");
        theType(*particle) = static_cast<int>(type);
        theOrigin(*particle) = static_cast<int>(origin);
      }
    }
  } // loop over aod tracks

  ATH_MSG_DEBUG("Converted [" << container.size() << " -> " << xaod->size()
                              << "] TrackParticles and stored in "
                              << xaod.name());
  if (container.size() != xaod->size()) {
    ATH_MSG_WARNING("number of items in the AOD container: "
                    << container.size()
                    << " is not equal to the number of items in its converted "
                       "xAOD equivalent: "
                    << xaod->size());
  }

  return 1;
}

xAOD::TrackParticle*
TrackParticleCnvAlg::createParticle(
  xAOD::TrackParticleContainer& xaod,
  const Rec::TrackParticleContainer& /**container*/,
  const Rec::TrackParticle& tp)
{
  // create the xAOD::TrackParticle, the pointer is added to the container in
  // the function
  xAOD::TrackParticle* xp = m_particleCreator->createParticle(tp, &xaod);
  return xp;
} // createParticleAndTruth

xAOD::TrackParticle*
TrackParticleCnvAlg::createParticle(xAOD::TrackParticleContainer& xaod,
                                    const TrackCollection& container,
                                    const Trk::Track& tp)
{
  // create the xAOD::TrackParticle, the pointer is added to the container in
  // the function
  ElementLink<TrackCollection> trackLink(&tp, container);
  return m_particleCreator->createParticle(trackLink, &xaod);
}

StatusCode
TrackParticleCnvAlg::finalize()
{
  return StatusCode::SUCCESS;
}

} // namespace xAODMaker
