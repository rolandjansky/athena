/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

///////////////////////////////////////////////////////////////////
// InDetExtensionProcessor.cxx
//   Implementation file for class InDetExtensionProcessor
///////////////////////////////////////////////////////////////////
// version 0.0 Markus Elsing
///////////////////////////////////////////////////////////////////

#include "InDetExtensionProcessor.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"
#include <algorithm> //for std::transform
#include <cmath> //for std::abs
#include <functional> //for std::plus<>
#include <memory>



//==================================================================================================
namespace {
  template<typename T, size_t N>
  std::array<T, N>& //overloaded += operator for std::array
  operator += (std::array<T, N>& a1, const std::array<T, N>& a2) {
    std::transform(std::begin(a1), std::end(a1), begin(a2), std::begin(a1), std::plus<int>());
    return a1;
  }
}

//==================================================================================================

// Constructor with parameters:
InDet::InDetExtensionProcessor::InDetExtensionProcessor(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator),
  m_trackName("Tracks"),
  m_cosmics(false),
  m_extensionMapName("TrackExtensionMap"),
  m_newTrackName("ExtendedTrack"),
  m_trackFitter("Trk::KalmanFitter/InDetTrackFitter"),
  m_scoringTool("Trk::TrackScoringTool"),
  m_runOutlier(true),
  m_keepFailedExtensionOnTrack(true),
  m_refitPrds(true),
  m_matEffects(3),
  m_suppressHoleSearch(false),
  m_particleHypothesis(Trk::undefined),
  m_Nevents(0), m_Ninput{0}, m_Nrecognised{0}, m_Nextended{0}, m_Nrejected{0}, m_Nfailed{0},
  m_NrecoveryBremFits{0}, m_NbremFits{0}, m_Nfits{0}, m_NnotExtended{0}, m_NlowScoreBremFits{0}, m_NextendedBrem{0},
  m_etabounds{0.8, 1.6, 2.10} {
  // Get parameter values from jobOptions file
  declareProperty("TrackName", m_trackName, "Name of the input Trackcollection");
  declareProperty("Cosmics", m_cosmics, "switch whether we are running on cosmics");
  declareProperty("ExtensionMap", m_extensionMapName, "Name of the input extension map");
  declareProperty("NewTrackName", m_newTrackName, "Name of the output Trackcollection");
  declareProperty("TrackFitter", m_trackFitter, "Toolhandle for the track fitter");
  declareProperty("ScoringTool", m_scoringTool, "Toolhandle for the track scorer");
  declareProperty("runOutlier", m_runOutlier, "switch whether to run outlier logics or not");
  declareProperty("keepFailedExtension", m_keepFailedExtensionOnTrack,
                  "switch whether to keep failed extension as outlier hits on the new track");
  declareProperty("RefitPrds", m_refitPrds, "switch whether to do the fit with re-calibrated clusters (true) or not");
  declareProperty("matEffects", m_matEffects, "particle hypothesis to assume for material effects in track fit");
  declareProperty("suppressHoleSearch", m_suppressHoleSearch, "suppressing hole search for comparison");
  declareProperty("tryBremFit", m_tryBremFit = false, "brem recover mode");
  declareProperty("caloSeededBrem", m_caloSeededBrem = false, "calo seeded brem recovery");
  declareProperty("pTminBrem", m_pTminBrem = 1000., "min pT for trying a brem recovery");
  declareProperty("etaBounds", m_etabounds, "eta intervals for internal monitoring"); //note: only three values!
}

//==================================================================================================

// Initialize method:
StatusCode
InDet::InDetExtensionProcessor::initialize() {
  ATH_MSG_DEBUG("InDetExtensionProcessor::initialize()");
  ATH_CHECK(m_trackFitter.retrieve());
  ATH_MSG_DEBUG("Retrieved tool " << m_trackFitter);
  // Configuration of the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_particleHypothesis = particleSwitch.particle[m_matEffects];
  //
  ATH_CHECK(m_trackSummaryTool.retrieve(DisableTool {m_trackSummaryTool.name().empty()}));
  ATH_CHECK(m_scoringTool.retrieve());
  ATH_MSG_DEBUG("Retrieved tool " << m_scoringTool);
  // brem fitting enabled ?
  if (m_tryBremFit) ATH_MSG_DEBUG("Try brem fit and recovery for electron like tracks.");
  m_Ninput.fill(0);
  m_Nrecognised.fill(0);
  m_Nextended.fill(0);
  m_NextendedBrem.fill(0);
  m_Nrejected.fill(0);
  m_Nfailed.fill(0);
  m_NrecoveryBremFits.fill(0);
  m_NbremFits.fill(0);
  m_Nfits.fill(0);
  m_NnotExtended.fill(0);
  m_NlowScoreBremFits.fill(0);
  //
  ATH_CHECK(m_trackName.initialize());
  ATH_CHECK(m_extensionMapName.initialize());
  ATH_CHECK(m_newTrackName.initialize());
  return StatusCode::SUCCESS;
}

//==================================================================================================

// Execute method:
StatusCode
InDet::InDetExtensionProcessor::execute(const EventContext& ctx) const {
  // process the extensions
  SG::ReadHandle<TrackCollection>   tracks(m_trackName, ctx);
  SG::ReadHandle<TrackExtensionMap> trackExtensionMap(m_extensionMapName, ctx);
  SG::WriteHandle<TrackCollection>  extendedTracksResult(m_newTrackName, ctx);
  const auto nTracks {tracks->size()};
  ATH_MSG_DEBUG("Number of Tracks found : " << nTracks);
  // input statistics
  {
    std::lock_guard<std::mutex> lock(m_statMutex);
    ++m_Nevents;
    m_Ninput[InDet::InDetExtensionProcessor::iAll] += nTracks;
  }
  ATH_CHECK(extendedTracksResult.record(std::unique_ptr<TrackCollection>(
    createExtendedTracks(ctx, tracks.cptr(), trackExtensionMap.cptr()))));
  ATH_MSG_DEBUG("Container '" << m_newTrackName.key()
                              << "' recorded in StoreGate, size : "
                              << extendedTracksResult->size());
  if (extendedTracksResult->size() != nTracks) ATH_MSG_WARNING("Lost tracks after extension ??? This is a bug !");
  return StatusCode::SUCCESS;
}

TrackCollection*
InDet::InDetExtensionProcessor::createExtendedTracks(const EventContext& ctx,
                                                     const TrackCollection* pTracks,
                                                     const TrackExtensionMap* trackExtensionMap) const {
  std::unique_ptr<TrackCollection> newTracks(std::make_unique<TrackCollection>());
  std::array<int, Nregions> Ninput {}, Nrecognised {}, Nextended {}, Nrejected {}, Nfailed {},
                            NrecoveryBremFits {}, NbremFits {}, Nfits {}, NnotExtended {},
                            NlowScoreBremFits {}, NextendedBrem {};
  // loop over tracks
  for (const Trk::Track* thisTrack : *pTracks) {
    // statistics
    incrementRegionCounter(Ninput, thisTrack, false);
    ATH_MSG_DEBUG("evaluate input track : " << thisTrack);
    // try to find this track in extension map
    TrackExtensionMap::const_iterator pThisExtensionPair = trackExtensionMap->find(thisTrack);
    // copy input track if this track is not found in list
    if (pThisExtensionPair == trackExtensionMap->end()) {
      ATH_MSG_DEBUG("track not in extension map, copy original track to output");
      // copy track, set pattern info
      std::unique_ptr<Trk::Track> ntrk(std::make_unique<Trk::Track>(*thisTrack));
      ntrk->info().setPatternRecognitionInfo(Trk::TrackInfo::InDetExtensionProcessor);
      newTracks->push_back(std::move(ntrk));
      // statistics
      incrementRegionCounter(NnotExtended, thisTrack);
    } else {
      ATH_MSG_DEBUG("track found in extension map, processing...");
      // statistics
      incrementRegionCounter(Nrecognised, thisTrack);
      // setup new track
      std::unique_ptr<Trk::Track>  newtrack;
      // keep old track info
      const Trk::TrackInfo oldTrackInfo = thisTrack->info();
      // keep list of PRDs, initially assumed to be same size as pThisExtensionPair->second
      const auto nRIO_OnTrack {pThisExtensionPair->second.size()};
      std::vector<const Trk::PrepRawData*> vecPrd;
      vecPrd.reserve(nRIO_OnTrack);
      // --- refit PRD ?
      if (m_refitPrds) {
        auto getPrepRawDataPtr = [](const Trk::MeasurementBase* m) {
                                   const auto& pRot = dynamic_cast <const Trk::RIO_OnTrack*>(m);
                                   return pRot ? pRot->prepRawData() : nullptr;
                                 };
        const auto& theRIOs_OnTrack {pThisExtensionPair->second};
        //NOTE: Assumes success; I don't remove any nullptrs which result! (could use copy_if)
        std::transform(theRIOs_OnTrack.begin(), theRIOs_OnTrack.end(), std::back_inserter(vecPrd), getPrepRawDataPtr);
        // we have the PRD Vector, we need to refit the track and see if it is better
        ATH_MSG_DEBUG("fit track " << thisTrack << " with PRDs, number : " << vecPrd.size());
        if (!m_cosmics) {
          // normal event processing
          if (m_tryBremFit && thisTrack->info().trackProperties(Trk::TrackInfo::BremFit)) {
            ATH_MSG_DEBUG("brem fit of electron like track");
            //statistics
            incrementRegionCounter(NbremFits, thisTrack);
            // try brem fit of combined track
            newtrack = m_trackFitter->fit(
              ctx, *thisTrack, vecPrd, m_runOutlier, Trk::electron);
          } else {
            ATH_MSG_DEBUG("normal fit track");
            //statistics
            incrementRegionCounter(Nfits, thisTrack);
            // normal fit of combined track
            newtrack = m_trackFitter->fit(ctx,
              *thisTrack, vecPrd, m_runOutlier, m_particleHypothesis);
            if (!newtrack && m_tryBremFit &&
                thisTrack->trackParameters()->front()->pT() > m_pTminBrem &&
                (!m_caloSeededBrem || thisTrack->info().patternRecoInfo(
                                        Trk::TrackInfo::TrackInCaloROI))) {
              ATH_MSG_DEBUG("normal fit track failed, try to recover with brem fit");
              // statistics
              incrementRegionCounter(NrecoveryBremFits, thisTrack);
              // try brem fit of combined track
              newtrack=m_trackFitter->fit(ctx,*thisTrack, vecPrd, m_runOutlier, Trk::electron);
            }
          }
        } else {
          // ===> special case for cosmics
          std::vector<const Trk::PrepRawData*> vecPrdComb;
          // get track parameters
          const Trk::Perigee* siPerigee = thisTrack->perigeeParameters();
          if (!siPerigee) {
            ATH_MSG_DEBUG("no perigee found on track, copy input");
            // statistics
            incrementRegionCounter(Nfailed, thisTrack);
            // copy track, set pattern info
            std::unique_ptr<Trk::Track> ntrk(std::make_unique< Trk::Track>(*thisTrack));
            ntrk->info().setPatternRecognitionInfo(Trk::TrackInfo::InDetExtensionProcessor);
            newTracks->push_back(std::move(ntrk));
          } else {
            // loop over extension RIO-on-tracks and add PRD it into vecPrdComb
            DataVector<const Trk::MeasurementBase>::const_iterator RIOit =
              pThisExtensionPair->first->measurementsOnTrack()->begin();
            for (; RIOit != pThisExtensionPair->first->measurementsOnTrack()->end(); ++RIOit) {
              const Trk::RIO_OnTrack* rot = dynamic_cast <const Trk::RIO_OnTrack*> (*RIOit);
              if (!rot) ATH_MSG_ERROR("cast to ROT failed, should not happen here !");
              else vecPrdComb.push_back(rot->prepRawData());
            }
            // merge lists
            for (unsigned int i = 0; i < vecPrd.size(); i++) {
              double inprod =
                (vecPrd[i]->detectorElement()->surface(vecPrd[i]->identify()).center() - siPerigee->position()).dot(
                  siPerigee->momentum());
              if (inprod < 0) {
                vecPrdComb.insert(vecPrdComb.begin(), vecPrd[i]);
              } else {
                vecPrdComb.push_back(vecPrd[i]);
              }
            }
            //statistics
            incrementRegionCounter(Nfits, thisTrack);
            // normal fit of combined track
            newtrack = m_trackFitter->fit(
              ctx, vecPrdComb, *siPerigee, m_runOutlier, m_particleHypothesis);
          }
        }
      } else {
        const auto& RIOs_OnTrack {pThisExtensionPair->second};
        // we have an extension map with RIO-on-tracks, we need to refit the track and see if it is better
        ATH_MSG_DEBUG("fit track " << thisTrack << " with ROTs, number : " << RIOs_OnTrack.size());
        if (!m_cosmics) {
          if (m_tryBremFit && thisTrack->info().trackProperties(Trk::TrackInfo::BremFit)) {
            ATH_MSG_DEBUG("brem fit of electron like track");
            //statistics
            incrementRegionCounter(NbremFits, thisTrack);
            // try brem fit of combined track
            newtrack=m_trackFitter->fit(ctx,*thisTrack, RIOs_OnTrack, m_runOutlier, Trk::electron);
          } else {
            ATH_MSG_DEBUG("normal fit track");
            //statistics
            incrementRegionCounter(Nfits, thisTrack);
            // fit combined track
            newtrack=m_trackFitter->fit(ctx,*thisTrack, RIOs_OnTrack, m_runOutlier, m_particleHypothesis);
            if (!newtrack && m_tryBremFit &&
                thisTrack->trackParameters()->front()->pT() > m_pTminBrem &&
                (!m_caloSeededBrem || thisTrack->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI))) {
              ATH_MSG_DEBUG("normal fit track failed, try to recover with brem fit");
              // statistics
              incrementRegionCounter(NrecoveryBremFits, thisTrack);
              // try to recover with brem fit
              newtrack=m_trackFitter->fit(ctx,*thisTrack, RIOs_OnTrack, m_runOutlier, Trk::electron);
            }
          }//end-if of normal track fit
        } else {
          // special case for cosmics
          Trk::MeasurementSet rotSet;
          // get track parameters
          const Trk::Perigee* siPerigee = thisTrack->perigeeParameters();
          if (!siPerigee) {
            ATH_MSG_DEBUG("no perigee found on track, copy input");
            // statistics
            incrementRegionCounter(Nfailed, thisTrack);
            // copy track, set pattern info
            std::unique_ptr<Trk::Track>  ntrk(std::make_unique< Trk::Track>(*thisTrack));
            ntrk->info().setPatternRecognitionInfo(Trk::TrackInfo::InDetExtensionProcessor);
            newTracks->push_back(std::move(ntrk));
          } else {
            // extract RIO-on-tracks from extension
            DataVector<const Trk::MeasurementBase>::const_iterator RIOit =
              pThisExtensionPair->first->measurementsOnTrack()->begin();
            for (; RIOit != pThisExtensionPair->first->measurementsOnTrack()->end(); ++RIOit) {
              rotSet.push_back(*RIOit);
            }
            // merge RIO-on-track lists
            for (unsigned int i = 0; i < pThisExtensionPair->second.size(); i++) {
              double inprod = (pThisExtensionPair->second[i]->associatedSurface().center() - siPerigee->position()).dot(
                siPerigee->momentum());
              if (inprod < 0) {
                rotSet.insert(rotSet.begin(), pThisExtensionPair->second[i]);
              } else {
                rotSet.push_back(pThisExtensionPair->second[i]);
              }
            }
            ATH_MSG_DEBUG("normal fit track");
            //statistics
            incrementRegionCounter(Nfits, thisTrack);
            // fit combined track
            newtrack=m_trackFitter->fit(ctx,rotSet, *siPerigee, m_runOutlier, m_particleHypothesis);
          }
        }
      }
      if (!newtrack) {
        ATH_MSG_DEBUG("refit of extended track failed, copy original track to output");
        // statistics
        incrementRegionCounter(Nfailed, thisTrack);
        // push track into output, does copy, needs fixing
        if (m_keepFailedExtensionOnTrack) {
          newTracks->push_back(trackPlusExtension(ctx,thisTrack, pThisExtensionPair->second));
        } else {
          // copy track, set pattern info
          std::unique_ptr<Trk::Track>  ntrk(std::make_unique<Trk::Track>(*thisTrack));
          ntrk->info().setPatternRecognitionInfo(Trk::TrackInfo::InDetExtensionProcessor);
          newTracks->push_back(std::move(ntrk));
        }
      } else {
        if (m_trackSummaryTool.isEnabled()) {
          m_trackSummaryTool->computeAndReplaceTrackSummary(ctx,*newtrack, nullptr, m_suppressHoleSearch);
        }
        // score old and new tool and decide which one to push back
        Trk::TrackScore oldScore = m_scoringTool->score(*thisTrack, m_suppressHoleSearch);
        ATH_MSG_DEBUG("original track has score : " << oldScore);
        Trk::TrackScore newScore = m_scoringTool->score(*newtrack, m_suppressHoleSearch);
        ATH_MSG_DEBUG("new track has score      : " << newScore);
        // do we need to recover with a brem fit
        if (newScore < oldScore && m_tryBremFit &&
            !newtrack->info().trackProperties(Trk::TrackInfo::BremFit) &&
            thisTrack->trackParameters()->front()->pT() > m_pTminBrem &&
            (!m_caloSeededBrem || thisTrack->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI))) {
          ATH_MSG_DEBUG("new track has low score, try to recover track using brem fit");
          // statistics
          incrementRegionCounter(NlowScoreBremFits, thisTrack);
          std::unique_ptr<Trk::Track> newBremTrack;
          // try to recover with brem fit
          if (m_refitPrds) {
            newBremTrack=m_trackFitter->fit(ctx,*thisTrack, vecPrd, m_runOutlier, Trk::electron);
          } else {
            newBremTrack=m_trackFitter->fit(ctx,*thisTrack, pThisExtensionPair->second, m_runOutlier, Trk::electron);
          }
          if (newBremTrack) {
            // score again
            // @TODO should score newBremTrack
            if (m_trackSummaryTool.isEnabled()) {
              m_trackSummaryTool->computeAndReplaceTrackSummary(ctx,*newBremTrack, nullptr, m_suppressHoleSearch);
            }
            newScore = m_scoringTool->score(*newtrack, m_suppressHoleSearch);
            ATH_MSG_DEBUG("recovered new track has score      : " << newScore);
            // copy pointer
            newtrack = std::move(newBremTrack);
          }
        }
        if (newScore >= oldScore) {
          ATH_MSG_DEBUG("take extended track, it's better !");
          // statistics
          incrementRegionCounter(Nextended, newtrack.get());
          if (m_tryBremFit && newtrack->info().trackProperties(Trk::TrackInfo::BremFit)) {
            incrementRegionCounter(NextendedBrem, newtrack.get());
          }
          //storing the precedent info
          newtrack->info().addPatternReco(oldTrackInfo);
          // push track into output
          newtrack->info().setPatternRecognitionInfo(Trk::TrackInfo::InDetExtensionProcessor);
          newTracks->push_back(std::move(newtrack));
        } else {
          ATH_MSG_DEBUG("take original track, new one is worse !");
          // statistics
          incrementRegionCounter(Nrejected, newtrack.get());
          // push track into output, does copy
          std::unique_ptr<Trk::Track> ntrk;
          if (m_keepFailedExtensionOnTrack) {
            ntrk.reset(trackPlusExtension(ctx,thisTrack, pThisExtensionPair->second));
          } else {
            ntrk = std::make_unique<Trk::Track>(*thisTrack);
          }
          ntrk->info().setPatternRecognitionInfo(Trk::TrackInfo::InDetExtensionProcessor);
          newTracks->push_back(std::move(ntrk));
        }
      }
    }
  }

  {
    std::lock_guard<std::mutex> lock(m_statMutex);
    m_Ninput += Ninput;
    m_Nrecognised += Nrecognised;
    m_Nextended += Nextended;
    m_Nrejected += Nrejected;
    m_Nfailed += Nfailed;
    m_NrecoveryBremFits += NrecoveryBremFits;
    m_NbremFits += NbremFits;
    m_Nfits += Nfits;
    m_NnotExtended += NnotExtended;
    m_NlowScoreBremFits += NlowScoreBremFits;
    m_NextendedBrem += NextendedBrem;
  }
  return newTracks.release();
}

//==================================================================================================

void InDet::InDetExtensionProcessor::incrementRegionCounter(std::array<int, 4>& Ntracks, const Trk::Track* track,
                                                            bool updateAll) const {
  if (updateAll) Ntracks[InDet::InDetExtensionProcessor::iAll] += 1;
  // test
  if (!track) {
    ATH_MSG_ERROR("track pointer zero, should not happen!");
    return;
  }
  const auto *const pTrackParameters {track->trackParameters()};
  // use first parameter
  if (not pTrackParameters) {
    ATH_MSG_WARNING("No track parameters, needed for statistics code in Trk::SimpleAmbiguityProcessorTool!");
  } else {
    const double absEta = std::abs(pTrackParameters->front()->eta());
    if (absEta < m_etabounds[0]) ++Ntracks[InDet::InDetExtensionProcessor::iBarrel];
    else if (absEta < m_etabounds[1]) ++Ntracks[InDet::InDetExtensionProcessor::iTransi];
    else if (absEta < m_etabounds[2]) ++Ntracks[InDet::InDetExtensionProcessor::iEndcap];
  }
}

//==================================================================================================

Trk::Track*
InDet::InDetExtensionProcessor::trackPlusExtension(
  const EventContext& ctx,
  const Trk::Track* siTrack,
  const std::vector<const Trk::MeasurementBase*>& extension) const
{
  const auto& trackStatesOnSurfaces{ *(siTrack->trackStateOnSurfaces()) };
  auto pExtendedTrajectory = DataVector<const Trk::TrackStateOnSurface>();

  pExtendedTrajectory.reserve(trackStatesOnSurfaces.size() + extension.size());
  int nSiStates = 0, nExtStates = 0;
  // copy existing si track as first part to new track - including all track pars since fit does not change
  auto cloneTSOS = [](const Trk::TrackStateOnSurface* pTSOS) {
                     return new Trk::TrackStateOnSurface(*pTSOS);
                   };
  std::transform(trackStatesOnSurfaces.begin(), trackStatesOnSurfaces.end(), std::back_inserter(
                   pExtendedTrajectory), cloneTSOS);
  nSiStates += trackStatesOnSurfaces.size();
  // copy proposed (but failed) extension as second part onto new track - all hits flagged as outliers.
  constexpr auto outlierDigit {1 << Trk::TrackStateOnSurface::Outlier};
  constexpr std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> outlierPattern(outlierDigit);
  //create new track state on surface
  auto createNewTSOS = [outlierPattern](const Trk::MeasurementBase* pm) -> const Trk::TrackStateOnSurface*{
                         return new Trk::TrackStateOnSurface(pm->uniqueClone(), nullptr, nullptr, nullptr, outlierPattern);
                       };
  //Adding to cosmic tracks beginning or end depending on the direction of track
  auto addNewTSOS_ForCosmics = [&pExtendedTrajectory, siTrack, createNewTSOS](const Trk::MeasurementBase* pm) {
                                 const auto& perigee {siTrack->perigeeParameters()};
                                 //the sign of this gives the direction
                                 const double inprod = (pm->associatedSurface().center() - perigee->position()).dot(
                                   perigee->momentum());
                                 if (inprod < 0) {
                                   pExtendedTrajectory.insert(pExtendedTrajectory.begin(), createNewTSOS(pm));
                                 } else {
                                   pExtendedTrajectory.push_back(createNewTSOS(pm));
                                 }
                               };
  //actual copying done here, using preceding lambda functions
  if (not m_cosmics) {
    std::transform(extension.begin(), extension.end(), std::back_inserter(pExtendedTrajectory), createNewTSOS);
  } else {
    //difficult to use std::transform here, since don't know whether back or front are added to
    for (const auto *const pMeasurementBase: extension) {
      addNewTSOS_ForCosmics(pMeasurementBase);
    }
  }
  nExtStates += pExtendedTrajectory.size();
  const auto& pFitQuality {siTrack->fitQuality()};
  Trk::Track* extTrack =
    new Trk::Track(siTrack->info(), std::move(pExtendedTrajectory), (pFitQuality ? pFitQuality->clone() : nullptr));
  if (m_trackSummaryTool.isEnabled()) {
    m_trackSummaryTool->computeAndReplaceTrackSummary(ctx,*extTrack, nullptr, m_suppressHoleSearch);
  }
  Trk::TrackScore extScore = m_scoringTool->score(*extTrack, m_suppressHoleSearch);
  ATH_MSG_DEBUG("rejected extension saved as Trk::Track with " << nSiStates <<
                " fitted hits and " << nExtStates << " additional Outliers, score :" << extScore);
  return extTrack;
}

//==================================================================================================

// Finalize method:
StatusCode InDet::InDetExtensionProcessor::finalize() {
  if (msgLvl(MSG::INFO)) {
     MsgStream &out = msg(MSG::INFO);
     out << "::finalize() -- statistics:" << std::endl;
     dumpStat(out);
     out << endmsg;
  }
  return StatusCode::SUCCESS;
}

MsgStream &InDet::InDetExtensionProcessor::dumpStat(MsgStream &out) const
{
    int iw = 9;
    out.width(9);
    out << "-------------------------------------------------------------------------------" << std::endl;
    out << "  Number of events processed      :   " << m_Nevents << std::endl;
    out << "  statistics by eta range          ------All---Barrel---Trans.-- Endcap-- " << std::endl;
    out << "-------------------------------------------------------------------------------" << std::endl;
    out << "  Number of tracks at input       :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ninput[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ninput[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ninput[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ninput[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    out << "  Number of not extended tracks   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NnotExtended[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) <<
    std::setw(iw)
              << m_NnotExtended[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) <<
    std::setw(iw)
              << m_NnotExtended[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) <<
    std::setw(iw)
              << m_NnotExtended[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    out << "  Number of proposed ext. roads   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nrecognised[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nrecognised[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(
      iw)
              << m_Nrecognised[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(
      iw)
              << m_Nrecognised[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    out << "-------------------------------------------------------------------------------" << std::endl;
    out << "  Number of track fits            :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    if (m_tryBremFit) {
      out << "  + brem fits for electron tracks :" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NbremFits[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NbremFits[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(
        iw)
                << m_NbremFits[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(
        iw)
                << m_NbremFits[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
      out << "  + brem fit to recover failed fit:" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NrecoveryBremFits[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) <<
      std::setw(iw)
                << m_NrecoveryBremFits[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) <<
      std::setw(iw)
                << m_NrecoveryBremFits[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) <<
      std::setw(iw)
                << m_NrecoveryBremFits[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
      out << "  + brem fit to recover low score :" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NlowScoreBremFits[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) <<
      std::setw(iw)
                << m_NlowScoreBremFits[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) <<
      std::setw(iw)
                << m_NlowScoreBremFits[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) <<
      std::setw(iw)
                << m_NlowScoreBremFits[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    }
    out << "-------------------------------------------------------------------------------" << std::endl;
    out << "  Number of track fit failing     :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfailed[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfailed[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfailed[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfailed[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    out << "  Number of rejected extensions   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nrejected[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nrejected[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) <<
    std::setw(iw)
              << m_Nrejected[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) <<
    std::setw(iw)
              << m_Nrejected[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    out << "  Number of successful extensions :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nextended[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nextended[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) <<
    std::setw(iw)
              << m_Nextended[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) <<
    std::setw(iw)
              << m_Nextended[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    if (m_tryBremFit) {
      out << "  including brem fitted tracks    :" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NextendedBrem[InDet::InDetExtensionProcessor::iAll] << std::setiosflags(std::ios::dec) <<
      std::setw(iw)
                << m_NextendedBrem[InDet::InDetExtensionProcessor::iBarrel] << std::setiosflags(std::ios::dec) <<
      std::setw(iw)
                << m_NextendedBrem[InDet::InDetExtensionProcessor::iTransi] << std::setiosflags(std::ios::dec) <<
      std::setw(iw)
                << m_NextendedBrem[InDet::InDetExtensionProcessor::iEndcap] << std::endl;
    }
    out << "-------------------------------------------------------------------------------" << std::endl;
    out << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2)
              << "    definition: ( 0.0 < Barrel < " << m_etabounds[0] << " < Transition < " << m_etabounds[1]
              << " < Endcap < " << m_etabounds[2] << " )" << std::setprecision(-1) << std::endl;
    out << "-------------------------------------------------------------------------------" << std::endl;
    return out;
}
