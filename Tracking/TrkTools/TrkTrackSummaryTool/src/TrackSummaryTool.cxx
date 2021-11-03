/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummaryTool/TrackSummaryTool.h"
//
#include "TrkToolInterfaces/ITRT_ElectronPidTool.h"
//
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
//
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"
//
#include "GaudiKernel/EventContext.h"
#include <algorithm>
#include <cmath>
#include <initializer_list>

namespace {
template<class Indexable, class IntegerArray>
void
setTheseElements(Indexable& container,
                 const IntegerArray& indices,
                 const typename Indexable::value_type& toValue)
{
  for (const auto idx : indices) {
    container[idx] = toValue;
  }
}
//
template<class Indexable>
void
setTheseElements(Indexable& container,
                 const std::initializer_list<size_t>& indices,
                 const typename Indexable::value_type& toValue)
{
  for (const auto idx : indices) {
    container[idx] = toValue;
  }
}
}

Trk::TrackSummaryTool::TrackSummaryTool(const std::string& t,
                                        const std::string& n,
                                        const IInterface* p)
  : base_class(t, n, p)
  , m_detID{}
{
  declareInterface<ITrackSummaryTool>(this);
}

Trk::TrackSummaryTool::~TrackSummaryTool() {}

StatusCode
Trk::TrackSummaryTool::initialize()
{
  ATH_CHECK(detStore()->retrieve(m_detID, "AtlasID"));
  if (m_idTool.empty() && m_muonTool.empty()) {
    ATH_MSG_WARNING("Could get neither InDetHelperTool nor MuonHelperTool.");
  }
  if (not m_idTool.empty()) {
    ATH_CHECK(m_idTool.retrieve());
  }
  if (not m_eProbabilityTool.empty()) {
    ATH_CHECK(m_eProbabilityTool.retrieve());
  }
  if (not m_dedxtool.empty()) {
    ATH_CHECK(m_dedxtool.retrieve());
  }
  if (not m_muonTool.empty()) {
    ATH_CHECK(m_muonTool.retrieve());
  }
  auto bool2Txt = [](const bool flag) -> std::string {
    return flag ? "ON" : "OFF";
  };
  ATH_MSG_INFO("Search for InDet holes using external tool turned "
               << bool2Txt(m_doHolesInDet));
  ATH_MSG_INFO("Search for Muon holes using external tool turned "
               << bool2Txt(m_doHolesMuon));
  return StatusCode::SUCCESS;
}

StatusCode
Trk::TrackSummaryTool::finalize()
{
  return StatusCode::SUCCESS;
}

/*
 * method that creates creates/updates the summary for
 * a non-const track.
 *
 * if a summary is present it uses it as starting point
 * by cloning it and adds additional info.
 *
 * it sets the track summary for the non-const
 * track.
 */
void
Trk::TrackSummaryTool::computeAndReplaceTrackSummary(
  const EventContext& ctx,
  Trk::Track& track,
  const Trk::PRDtoTrackMap* pPrdToTrackMap,
  bool suppress_hole_search) const
{
  track.setTrackSummary(createSummary(ctx,
                                      track,
                                      pPrdToTrackMap,
                                      m_doHolesInDet & !suppress_hole_search,
                                      m_doHolesMuon & !suppress_hole_search));
}
/*
 * method that creates a new summary from const track.
 * It does not modify the const Track
 */
std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::summary(const EventContext& ctx,
                               const Track& track) const
{
  return createSummary(ctx, track, nullptr, m_doHolesInDet, m_doHolesMuon);
}

/*
 * method that creates a new summary from const track.
 * It does not modify the const Track
 */
std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::summary(const EventContext& ctx,
                               const Track& track,
                               const Trk::PRDtoTrackMap* pPrdToTrackMap) const
{
  return createSummary(
    ctx, track, pPrdToTrackMap, m_doHolesInDet, m_doHolesMuon);
}

/*
 * method that creates a new summary from const track.
 * It does not modify the const Track
 * It never does a search for holes
 */
std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::summaryNoHoleSearch(const EventContext& ctx,
                                           const Track& track) const
{
  return createSummary(ctx, track, nullptr, false, false);
}

/*
 * method that creates a new summary from const track.
 * It does not modify the const Track
 * It never does a search for holes
 */
std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::summaryNoHoleSearch(
  const EventContext& ctx,
  const Track& track,
  const Trk::PRDtoTrackMap* pPrdToTrackMap) const
{
  return createSummary(ctx, track, pPrdToTrackMap, false, false);
}

/*
 * Method that creates a new summary from const track.
 * It does not modify the const Track
 */
std::unique_ptr<Trk::TrackSummary>
Trk::TrackSummaryTool::createSummary(const EventContext& ctx,
                                     const Track& track,
                                     const Trk::PRDtoTrackMap* pPrdToTrackMap,
                                     bool doHolesInDet,
                                     bool doHolesMuon) const
{
  std::unique_ptr<Trk::TrackSummary> ts;
  // first check if track has summary already and then clone it.
  if (track.trackSummary() != nullptr) {
    ts = std::make_unique<Trk::TrackSummary>(*(track.trackSummary()));
  } else {
    ts = std::make_unique<Trk::TrackSummary>();
  }
  // fill the summary
  fillSummary(ctx, *ts, track, pPrdToTrackMap, doHolesInDet, doHolesMuon);
  return ts;
}

/*
 * Method that actually fills the summary
 * Work is delegated mainly the tools.
 * Here we set the summary info to initial values
 * and call other methods/tools to fill it
 */
void
Trk::TrackSummaryTool::fillSummary(const EventContext& ctx,
                                   Trk::TrackSummary& ts,
                                   const Trk::Track& track,
                                   const Trk::PRDtoTrackMap* prdToTrackMap,
                                   bool doHolesInDet,
                                   bool doHolesMuon) const
{
  // Resize the vector where we will keep the information needed for the summary
  std::vector<int>& information = ts.m_information;
  information.resize(std::min(information.size(),
                              static_cast<size_t>(numberOfTrackSummaryTypes)));
  std::vector<float> eProbability = Trk::eProbabilityDefault;
  float dedx = -1;
  int nHitsUsed_dEdx = -1;
  int nOverflowHits_dEdx = -1;

  constexpr int toZero{ 0 };
  if (!m_idTool.empty()) {
    if (m_pixelExists) {
      // Pixel counters set to 0
      constexpr size_t numberOfPixelCounters{ 14 };
      constexpr std::array<size_t, numberOfPixelCounters> atPixelIndices{
        numberOfContribPixelLayers,
        numberOfInnermostPixelLayerHits,
        numberOfInnermostPixelLayerOutliers,
        numberOfNextToInnermostPixelLayerHits,
        numberOfNextToInnermostPixelLayerOutliers,
        numberOfPixelHits,
        numberOfPixelOutliers,
        numberOfGangedPixels,
        numberOfGangedFlaggedFakes,
        numberOfPixelSpoiltHits,
        numberOfGangedFlaggedFakes,
        numberOfPixelSplitHits,
        numberOfInnermostLayerSplitHits,
        numberOfNextToInnermostLayerSplitHits
      };
      setTheseElements(information, atPixelIndices, toZero);
      information[Trk::numberOfDBMHits] = 0;

      // calculate  dedx
      if (track.info().trackFitter() != TrackInfo::Unknown &&
          !m_dedxtool.empty()) {
        dedx = m_dedxtool->dEdx(ctx, track, nHitsUsed_dEdx, nOverflowHits_dEdx);
      }
    }
    // SCT and TRT counters set to 0
    constexpr size_t numberOfSctOrTrtCounters{ 11 };
    constexpr std::array<size_t, numberOfSctOrTrtCounters> atSctOrTrtIndices{
      numberOfSCTHits,
      numberOfSCTSpoiltHits,
      numberOfSCTOutliers,
      numberOfTRTHits,
      numberOfTRTXenonHits,
      numberOfTRTHighThresholdHits,
      numberOfTRTHighThresholdHitsTotal,
      numberOfTRTOutliers,
      numberOfTRTHighThresholdOutliers,
      numberOfTRTTubeHits,
      numberOfTRTSharedHits
    };
    setTheseElements(information, atSctOrTrtIndices, toZero);

    // Calculate the  electron probability
    if (!m_eProbabilityTool.empty()) {
      eProbability = m_eProbabilityTool->electronProbability(ctx, track);
      information[Trk::numberOfTRTHitsUsedFordEdx] = static_cast<int>(
        eProbability[Trk::eProbabilityNumberOfTRTHitsUsedFordEdx]);
    }
  }
  // set electron probability and dedx
  ts.m_eProbability = eProbability;
  ts.m_dedx = dedx;
  ts.m_nhitsdedx = nHitsUsed_dEdx;
  ts.m_nhitsoverflowdedx = nOverflowHits_dEdx;

  if (m_doSharedHits) {
    // Shared hits counters set to 0
    information[numberOfSCTSharedHits] = 0;
    if (m_pixelExists) {
      setTheseElements(information,
                       { numberOfInnermostPixelLayerSharedHits,
                         numberOfNextToInnermostPixelLayerSharedHits,
                         numberOfPixelSharedHits },
                       toZero);
    }
  }
  if (!m_muonTool.empty()) {
    // Muon counters set to 0
    constexpr size_t numberOfMuonRelatedCounters{ 15 };
    constexpr std::array<size_t, numberOfMuonRelatedCounters> atMuonIndices{
      numberOfMdtHits,          numberOfTgcPhiHits,
      numberOfTgcEtaHits,       numberOfCscPhiHits,
      numberOfCscEtaHits,       numberOfCscUnspoiltEtaHits,
      numberOfRpcPhiHits,       numberOfRpcEtaHits,
      Trk::numberOfMdtHoles,    Trk::numberOfTgcEtaHoles,
      Trk::numberOfTgcPhiHoles, Trk::numberOfCscEtaHoles,
      Trk::numberOfCscPhiHoles, Trk::numberOfRpcEtaHoles,
      Trk::numberOfRpcPhiHoles,
    };
    // New Small Wheel counters set to 0
    constexpr size_t numberOfNswRelatedCounters{ 6 };
    constexpr std::array<size_t, numberOfNswRelatedCounters> atNswIndices{
      Trk::numberOfStgcEtaHits,  Trk::numberOfStgcPhiHits,  Trk::numberOfMmHits,
      Trk::numberOfStgcEtaHoles, Trk::numberOfStgcPhiHoles, Trk::numberOfMmHoles
    };
    setTheseElements(information, atMuonIndices, toZero);
    setTheseElements(information, atNswIndices, toZero);
  }

  // update the summart counters processing the track states
  std::bitset<numberOfDetectorTypes> hitPattern;
  if (track.trackStateOnSurfaces()) {
    information[Trk::numberOfOutliersOnTrack] = 0;
    processTrackStates(ctx,
                       track,
                       prdToTrackMap,
                       track.trackStateOnSurfaces(),
                       information,
                       hitPattern,
                       doHolesInDet,
                       doHolesMuon);
  } else {
    ATH_MSG_WARNING("Null pointer to TSoS found on Track (author = "
                    << track.info().dumpInfo()
                    << "). This should never happen! ");
  }
  ts.m_idHitPattern = hitPattern.to_ulong();

  //Now the possible additional summary steps
  // Is the hole search done , could be there in the input already
  bool holeSearchDone = (information[Trk::numberOfPixelHoles] != -1 &&
                         information[Trk::numberOfSCTHoles] != -1 &&
                         information[Trk::numberOfSCTDoubleHoles] != -1 &&
                         information[Trk::numberOfPixelDeadSensors] != -1 &&
                         information[Trk::numberOfSCTDeadSensors] != -1);

  // Do the holes search if we have to or insist to
  if ((doHolesInDet || doHolesMuon) &&
      (!holeSearchDone || m_alwaysRecomputeHoles.value())) {
    if (m_pixelExists) {
      information[numberOfPixelHoles] = 0;
    }
    information[numberOfSCTHoles] = 0;
    information[numberOfSCTDoubleHoles] = 0;
    searchHolesStepWise(track, information, doHolesInDet, doHolesMuon);
  }

  // add detailed summary for indet
  if (m_addInDetDetailedSummary && !m_idTool.empty()) {
    m_idTool->addDetailedTrackSummary(ctx, track, ts);
  }
  // add detailed summary for muons
  if (m_addMuonDetailedSummary && !m_muonTool.empty()) {
    m_muonTool->addDetailedTrackSummary(ctx, track, ts);
  }
}

/*
 * Method to update the shared hits count in the summary
 */
void
Trk::TrackSummaryTool::updateSharedHitCount(
  const Track& track,
  const Trk::PRDtoTrackMap* prdToTrackMap,
  TrackSummary& summary) const
{
  if (m_idTool) {
    m_idTool->updateSharedHitCount(track, prdToTrackMap, summary);
  }
}

/*
 * Method to update additional info in the summary
 */
void
Trk::TrackSummaryTool::updateAdditionalInfo(const Track& track,
                                            TrackSummary& summary,
                                            bool initialiseToZero) const
{
  std::vector<float> eProbability = Trk::eProbabilityDefault;
  if (!m_eProbabilityTool.empty()) {
    eProbability = m_eProbabilityTool->electronProbability(track);
    int nHits = eProbability[Trk::eProbabilityNumberOfTRTHitsUsedFordEdx];
    if (!summary.update(Trk::numberOfTRTHitsUsedFordEdx,
                        static_cast<uint8_t>(std::max(nHits, 0)))) {
      ATH_MSG_WARNING("Attempt to update numberOfTRTHitsUsedFordEdx but this "
                      "summary information is "
                      "already set. numberOfTRTHitsUsedFordEdx is:"
                      << summary.get(numberOfTRTHitsUsedFordEdx)
                      << " =?= should:" << nHits);
    }
  }
  const int initialValue(initialiseToZero ? 0 : -1);
  float dedx = initialValue;
  int nHitsUsed_dEdx = initialValue;
  int nOverflowHits_dEdx = initialValue;
  if (track.info().trackFitter() != TrackInfo::Unknown && !m_dedxtool.empty()) {
    dedx = m_dedxtool->dEdx(track, nHitsUsed_dEdx, nOverflowHits_dEdx);
  }
  if (m_idTool) {
    m_idTool->updateAdditionalInfo(
      summary, eProbability, dedx, nHitsUsed_dEdx, nOverflowHits_dEdx);
    m_idTool->updateExpectedHitInfo(track, summary);
    if (m_addInDetDetailedSummary)
      m_idTool->addDetailedTrackSummary(track, summary);
  } else {
    ATH_MSG_INFO(
      "No updates attempted, as the SummaryHelperTool is not defined.");
  }
}

/*
 * Internal helper processing the Track State on Surfaces
 */
void
Trk::TrackSummaryTool::processTrackStates(
  const EventContext& ctx,
  const Track& track,
  const Trk::PRDtoTrackMap* prdToTrackMap,
  const DataVector<const TrackStateOnSurface>* tsos,
  std::vector<int>& information,
  std::bitset<numberOfDetectorTypes>& hitPattern,
  bool doHolesInDet,
  bool doHolesMuon) const
{
  int measCounter = 0;
  int cntAddChi2 = 0;
  float chi2Sum = 0;
  float chi2Sum2 = 0;
  DataVector<const TrackStateOnSurface>::const_iterator it = tsos->begin();
  DataVector<const TrackStateOnSurface>::const_iterator itEnd = tsos->end();
  for (; it != itEnd; ++it) {
    const auto& trackState = **it;
    const auto isMeasurement =
      trackState.type(Trk::TrackStateOnSurface::Measurement);
    const auto isOutlier = trackState.type(Trk::TrackStateOnSurface::Outlier);
    if (isMeasurement or isOutlier) {
      ++measCounter;
      const Trk::MeasurementBase* measurement = trackState.measurementOnTrack();
      if (!measurement) {
        ATH_MSG_WARNING("measurementOnTrack == null for a TrackStateOnSurface "
                        "of type Measurement or Outlier");
      } else {
        if (isOutlier)
          ++information[Trk::numberOfOutliersOnTrack]; // increment outlier
                                                       // counter
        processMeasurement(
          ctx, track, prdToTrackMap, measurement, *it, information, hitPattern);
      } // if have measurement pointer
    }   // if type measurement, scatterer or outlier

    if (isMeasurement) {
      if (const auto *const pFitQuality{ trackState.fitQualityOnSurface() };
          pFitQuality and pFitQuality->numberDoF() > 0) {
        ++cntAddChi2;
        if (const auto& chiSq{ pFitQuality->chiSquared() };
            chiSq > 1.e5) { // limit unphysical values and protect against FPE
          chi2Sum += 1.e5;
          chi2Sum2 += 1.e10;
        } else {
          const float chi2add = chiSq / pFitQuality->numberDoF();
          chi2Sum += chi2add;
          chi2Sum2 += chi2add * chi2add;
        }
      }
    }

    if (trackState.type(Trk::TrackStateOnSurface::Hole) &&
        trackState.trackParameters()) {
      if (!doHolesInDet ||
          !doHolesMuon) { // no dedicated hole search via extrapolation, but
                          // take what might be on the track already.
        if (trackState.trackParameters()
              ->associatedSurface()
              .associatedDetectorElement() != nullptr) {
          const Identifier& id = trackState.trackParameters()
                                   ->associatedSurface()
                                   .associatedDetectorElementIdentifier();
          if (!doHolesInDet && m_detID->is_pixel(id))
            ++information[Trk::numberOfPixelHoles];
          if (!doHolesInDet && m_detID->is_sct(id))
            ++information[Trk::numberOfSCTHoles];
          if (!doHolesMuon && m_detID->is_mdt(id))
            ++information[Trk::numberOfMdtHoles];
        }
      }
    }
  }

  float varChi2 = 0;
  if (cntAddChi2 > 0) {
    const auto inverseCount{ 1. / cntAddChi2 };
    varChi2 = (chi2Sum2 - (chi2Sum * chi2Sum * inverseCount)) * inverseCount;
  }
  if (varChi2 > 0 && varChi2 < 1.e13)
    information[Trk::standardDeviationOfChi2OS] = int(std::sqrt(varChi2) * 100);
}

/*
 * Internal helper going through the measurements
 * and delegating filling the summary to the
 * relevant Helper tool
 */
void
Trk::TrackSummaryTool::processMeasurement(
  const EventContext& ctx,
  const Track& track,
  const Trk::PRDtoTrackMap* prdToTrackMap,
  const Trk::MeasurementBase* meas,
  const Trk::TrackStateOnSurface* tsos,
  std::vector<int>& information,
  std::bitset<numberOfDetectorTypes>& hitPattern) const
{
  // Check if the measurement type is RIO on Track (ROT)
  const RIO_OnTrack* rot = nullptr;
  if (meas->type(Trk::MeasurementBaseType::RIO_OnTrack)) {
    rot = static_cast<const RIO_OnTrack*>(meas);
  }
  if (rot) {
    // have RIO_OnTrack
    const Trk::IExtendedTrackSummaryHelperTool* tool = getTool(rot->identify());
    if (tool == nullptr) {
      ATH_MSG_WARNING("Cannot find tool to match ROT. Skipping.");
    } else {
      tool->analyse(
        ctx, track, prdToTrackMap, rot, tsos, information, hitPattern);
    }
  } else {
    // check if the measurement type is CompetingRIOsOnTrack
    const Trk::CompetingRIOsOnTrack* compROT = nullptr;
    if (meas->type(Trk::MeasurementBaseType::CompetingRIOsOnTrack)) {
      compROT = static_cast<const CompetingRIOsOnTrack*>(meas);
    }
    if (compROT) {
      // if this works we have a CompetingRIOsOnTrack.
      rot = &compROT->rioOnTrack(0); // get 1st rot
      const Trk::IExtendedTrackSummaryHelperTool* tool =
        getTool(rot->identify()); // Use 'main' ROT to get detector type
      if (tool == nullptr) {
        ATH_MSG_WARNING("Cannot find tool to match cROT. Skipping.");
      } else {
        tool->analyse(
          ctx, track, prdToTrackMap, compROT, tsos, information, hitPattern);
      }
    }
  }
}

const Trk::IExtendedTrackSummaryHelperTool*
Trk::TrackSummaryTool::getTool(const Identifier& id) const
{
  if (m_detID->is_indet(id)) {
    if (!m_idTool.empty()) {
      return &*m_idTool;
    }
    ATH_MSG_WARNING("getTool: Identifier is from ID but have no ID tool");
  } else if (m_detID->is_muon(id)) {
    if (!m_muonTool.empty()) {
      return &*m_muonTool;
    }
    ATH_MSG_WARNING("getTool: Identifier is from Muon but have no Muon tool");
  } else {
    ATH_MSG_WARNING(
      "getTool: Identifier is of unknown type! id: " << id.getString());
  }
  return nullptr;
}

void
Trk::TrackSummaryTool::searchHolesStepWise(const Trk::Track& track,
                                           std::vector<int>& information,
                                           bool doHolesInDet,
                                           bool doHolesMuon) const
{
  ATH_MSG_VERBOSE("Entering Trk::TrackSummaryTool::searchHolesStepWise");
  // -------- obtain hits in Pixel and SCT only
  if (track.trackStateOnSurfaces() == nullptr) {
    constexpr int toMinusOne{ -1 };
    constexpr std::array<size_t, 16> atIndices{ numberOfPixelHoles,
                                                numberOfPixelDeadSensors,
                                                numberOfSCTHoles,
                                                numberOfSCTDoubleHoles,
                                                numberOfSCTDeadSensors,
                                                numberOfTRTHoles,
                                                numberOfTRTDeadStraws,
                                                numberOfCscEtaHoles,
                                                numberOfCscPhiHoles,
                                                numberOfRpcEtaHoles,
                                                numberOfRpcPhiHoles,
                                                numberOfTgcEtaHoles,
                                                numberOfTgcPhiHoles,
                                                // new small wheels
                                                numberOfStgcEtaHoles,
                                                numberOfStgcPhiHoles,
                                                numberOfMmHoles };
    setTheseElements(information, atIndices, toMinusOne);
    return;
  }

  constexpr int toZero{ 0 };
  if (doHolesInDet) {
    // -------- perform the InDet hole search
    if (m_pixelExists) {
      setTheseElements(
        information, { numberOfPixelHoles, numberOfPixelDeadSensors }, toZero);
    }
    setTheseElements(
      information,
      { numberOfSCTHoles, numberOfSCTDoubleHoles, numberOfSCTDeadSensors },
      toZero);
    m_idTool->searchForHoles(track, information, Trk::pion);
  }
  if (!m_muonTool.empty() && doHolesMuon) {
    // now do Muon hole search. It works completely differently to the above,
    // so we need to make this all a bit more general
    // and probably more efficient. But this hopefully works for now! EJWM
    constexpr size_t numberOfRelatedMuonCounters{ 10 };
    constexpr std::array<size_t, numberOfRelatedMuonCounters> atMuonIndices{
      numberOfMdtHoles,
      numberOfCscEtaHoles,
      numberOfCscPhiHoles,
      numberOfRpcEtaHoles,
      numberOfRpcPhiHoles,
      numberOfTgcEtaHoles,
      numberOfTgcPhiHoles,
      // new small wheels
      numberOfStgcEtaHoles,
      numberOfStgcPhiHoles,
      numberOfMmHoles
    };
    setTheseElements(information, atMuonIndices, toZero);
    m_muonTool->searchForHoles(track, information, Trk::muon);
  }
}

