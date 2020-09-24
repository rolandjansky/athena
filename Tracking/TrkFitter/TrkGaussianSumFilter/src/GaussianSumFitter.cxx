/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file GaussianSumFitter.cxx
 * @begin         Monday 7th March 2005
 * @author        Anthony Morley, Tom  Atkinson, Christos Anastopoulos
 * @brief         Implementation code for Gaussian Sum Fitter class
 */

#include "TrkGaussianSumFilter/GaussianSumFitter.h"
#include "TrkEventUtils/MeasurementBaseComparisonFunction.h"
#include "TrkEventUtils/PrepRawDataComparisonFunction.h"
#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"
#include "TrkGaussianSumFilter/MultiComponentStateCombiner.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkCaloCluster_OnTrack/CaloCluster_OnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"

#include <algorithm>
#include <vector>

Trk::GaussianSumFitter::GaussianSumFitter(const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_updator{}
  , m_directionToPerigee(Trk::oppositeMomentum)
  , m_trkParametersComparisonFunction(nullptr)
  , m_inputPreparator(nullptr)
  , m_cutChiSquaredPerNumberDOF(50.)
  , m_overideMaterialEffects(4)
  , m_overideParticleHypothesis(nonInteracting)
  , m_overideMaterialEffectsSwitch(false)
  , m_FitPRD{ 0 }
  , m_FitMeasurementBase{ 0 }
  , m_ForwardFailure{ 0 }
  , m_SmootherFailure{ 0 }
  , m_PerigeeFailure{ 0 }
  , m_fitQualityFailure{ 0 }
  , m_fitSuccess{ 0 }
{
  declareInterface<ITrackFitter>(this);
  declareProperty("SortingReferencePoint", m_sortingReferencePoint);
  declareProperty("StateChi2PerNDOFCut", m_cutChiSquaredPerNumberDOF);
  declareProperty("OverideForwardsMaterialEffects",
                  m_overideMaterialEffectsSwitch);
  declareProperty("MaterialEffectsInForwardFitter", m_overideMaterialEffects);

  // Estrablish reference point as origin
  m_sortingReferencePoint.push_back(0.);
  m_sortingReferencePoint.push_back(0.);
  m_sortingReferencePoint.push_back(0.);
}

StatusCode
Trk::GaussianSumFitter::initialize()
{
  StatusCode sc;
  // Request the GSF extrapolator
  ATH_CHECK(m_extrapolator.retrieve());
  // Request the RIO_OnTrack creator
  // No need to return if RioOnTrack creator tool, only if PrepRawData is used
  if (m_rioOnTrackCreator.retrieve().isFailure()) {
    if (!m_refitOnMeasurementBase) {
      ATH_MSG_FATAL("Attempting to use PrepRawData with no RIO_OnTrack creator "
                    "tool provided... Exiting!");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO(
      "Request to retrieve the RIO_OnTrack Creator"
      << "failed but track is fit at the MeasurementBase level... Continuing!");
  }
  if (m_overideMaterialEffectsSwitch) {
    ATH_MSG_INFO("Material effects in forwards fitter have been overiden by "
                 "jobOptions... New "
                 "Trk::ParticleHypothesis: "
                 << m_overideMaterialEffects);
  }
  if( m_maximumNumberOfComponents > 16){
      ATH_MSG_FATAL("Requested MaximumNumberOfComponents > 16");
      return StatusCode::FAILURE;
  }
  // Initialise the closest track parameters search algorithm
  Amg::Vector3D referencePosition(m_sortingReferencePoint[0],
                                  m_sortingReferencePoint[1],
                                  m_sortingReferencePoint[2]);

  m_trkParametersComparisonFunction =
    std::make_unique<Trk::TrkParametersComparisonFunction>(referencePosition);
  Trk::ParticleSwitcher particleSwitcher;
  m_overideParticleHypothesis =
    particleSwitcher.particle[m_overideMaterialEffects];
  m_inputPreparator = std::make_unique<TrackFitInputPreparator>();

  return StatusCode::SUCCESS;
}

StatusCode
Trk::GaussianSumFitter::finalize()
{
  ATH_MSG_INFO(
    "-----------------------------------------------"
    << '\n'
    << "            Some Brief GSF Statistics          " << '\n'
    << "-----------------------------------------------" << '\n'
    << "Number of Fit PrepRawData Calls:          " << m_FitPRD << '\n'
    << "Number of Fit MeasurementBase Calls:      " << m_FitMeasurementBase
    << '\n'
    << "Number of Forward Fit Failures:           " << m_ForwardFailure << '\n'
    << "Number of Smoother Failures:              " << m_SmootherFailure << '\n'
    << "Number of MakePerigee Failures:           " << m_PerigeeFailure << '\n'
    << "Number of Trks that fail fitquality test: " << m_fitQualityFailure
    << '\n'
    << "Number of successful fits:                " << m_fitSuccess << '\n'
    << '\n'
    << "-----------------------------------------------" << '\n'
    << "Finalisation of " << name() << " was successful");
  return StatusCode::SUCCESS;
}

/*
 * Refitting of a track
 */
std::unique_ptr<Trk::Track>
Trk::GaussianSumFitter::fit(
  const EventContext& ctx,
  const Trk::Track& inputTrack,
  const Trk::RunOutlierRemoval outlierRemoval,
  const Trk::ParticleHypothesis particleHypothesis) const
{

  // Check that the input track has well defined parameters
  if (inputTrack.trackParameters()->empty()) {
    ATH_MSG_FATAL("No estimation of track parameters near origin... Exiting!");
    return nullptr;
  }
  // Check that the input track has associated MeasurementBase objects
  if (inputTrack.trackStateOnSurfaces()->empty()) {
    ATH_MSG_FATAL("Attempting to fit track to empty MeasurementBase "
                  "collection... Exiting!");
    return nullptr;
  }

  // Retrieve the set of track parameters closest to the reference point
  const Trk::TrackParameters* parametersNearestReference =
    *(std::min_element(inputTrack.trackParameters()->begin(),
                       inputTrack.trackParameters()->end(),
                       *m_trkParametersComparisonFunction));

  // If refitting of track is at the MeasurementBase level
  // extract the MeasurementBase from the input track and create a new vector
  if (m_refitOnMeasurementBase) {

    MeasurementSet measurementSet;

    DataVector<const Trk::TrackStateOnSurface>::const_iterator
      trackStateOnSurface = inputTrack.trackStateOnSurfaces()->begin();
    for (; trackStateOnSurface != inputTrack.trackStateOnSurfaces()->end();
         ++trackStateOnSurface) {

      if (!(*trackStateOnSurface)) {
        ATH_MSG_WARNING("This track contains an empty MeasurementBase object "
                        "that won't be included in the fit");
        continue;
      }

      if ((*trackStateOnSurface)->measurementOnTrack()) {
        if ((*trackStateOnSurface)->type(TrackStateOnSurface::Measurement)) {
          measurementSet.push_back(
            (*trackStateOnSurface)->measurementOnTrack());
        } else if (m_reintegrateOutliers &&
                   (*trackStateOnSurface)->type(TrackStateOnSurface::Outlier)) {
          measurementSet.push_back(
            (*trackStateOnSurface)->measurementOnTrack());
        }
      }
    }

    // Apply GSF fit to MeasurementBase objects
    return fit(ctx,
               measurementSet,
               *parametersNearestReference,
               outlierRemoval,
               particleHypothesis);
  }

  // If refitting of the track is at the PrepRawData level then extract the
  // PrepRawData objects from the input track

  PrepRawDataSet prepRawDataSet;

  DataVector<const Trk::MeasurementBase>::const_iterator measurementOnTrack =
    inputTrack.measurementsOnTrack()->begin();

  for (; measurementOnTrack != inputTrack.measurementsOnTrack()->end();
       ++measurementOnTrack) {

    if (!(*measurementOnTrack)) {
      ATH_MSG_DEBUG("This track contains an empty MeasurementBase object... "
                    "Ignoring object");
      continue;
    }

    // Dynamic cast to a RIO_OnTrack object
    const Trk::RIO_OnTrack* rioOnTrack = nullptr;
    if ((*measurementOnTrack)->type(Trk::MeasurementBaseType::RIO_OnTrack)) {
      rioOnTrack = static_cast<const Trk::RIO_OnTrack*>(*measurementOnTrack);
    }

    if (!rioOnTrack) {
      ATH_MSG_DEBUG(
        "Measurement could not be cast as a RIO_OnTrack object... continuing");
      continue;
    }

    const PrepRawData* prepRawData = rioOnTrack->prepRawData();

    if (!prepRawData) {
      ATH_MSG_DEBUG(
        "Defined RIO_OnTrack object has no associated PrepRawData object..."
        << "this object will be ignored in fit");
      continue;
    }

    prepRawDataSet.push_back(prepRawData);
  }

  // Apply GSF fit to PrepRawData objects
  return fit(ctx,
             prepRawDataSet,
             *parametersNearestReference,
             outlierRemoval,
             particleHypothesis);
}

/*
   Fitting of a set of PrepRawData objects
*/

std::unique_ptr<Trk::Track>
Trk::GaussianSumFitter::fit(
  const EventContext& ctx,
  const Trk::PrepRawDataSet& prepRawDataSet,
  const Trk::TrackParameters& estimatedParametersNearOrigin,
  const Trk::RunOutlierRemoval outlierRemoval,
  const Trk::ParticleHypothesis particleHypothesis) const
{
  ++m_FitPRD;

  if (outlierRemoval) {
    ATH_MSG_DEBUG(
      "Outlier removal not yet implemented for the Gaussian Sum Filter");
  }

  // Protect against empty PrepRawDataSet object
  if (prepRawDataSet.empty()) {
    ATH_MSG_FATAL("PrepRawData set for fit is empty... Exiting!");
    return nullptr;
  }

  Trk::PrepRawDataSet sortedPrepRawDataSet = PrepRawDataSet(prepRawDataSet);

  if (m_doHitSorting) {
    Trk::PrepRawDataComparisonFunction prdComparisonFunction =
      Trk::PrepRawDataComparisonFunction(
        estimatedParametersNearOrigin.position(),
        estimatedParametersNearOrigin.momentum());

    std::sort(sortedPrepRawDataSet.begin(),
              sortedPrepRawDataSet.end(),
              prdComparisonFunction);
  }
  // Create Extrapolator cache that holds material effects cache;
  Trk::IMultiStateExtrapolator::Cache extrapolatorCache;

  // Perform GSF forwards fit
  std::unique_ptr<ForwardTrajectory> forwardTrajectory =
    fitPRD(ctx,
           extrapolatorCache,
           sortedPrepRawDataSet,
           estimatedParametersNearOrigin,
           particleHypothesis);

  if (!forwardTrajectory || forwardTrajectory->empty()) {
    ATH_MSG_DEBUG("Forward GSF fit failed... Exiting!");
    ++m_ForwardFailure;
    return nullptr;
  }

  // Perform GSF smoother operation
  std::unique_ptr<SmoothedTrajectory> smoothedTrajectory =
    fit(ctx, extrapolatorCache, *forwardTrajectory, particleHypothesis);

  // Protect against failed smoother fit
  if (!smoothedTrajectory) {
    ATH_MSG_DEBUG("Smoother GSF fit failed... Exiting!");
    ++m_SmootherFailure;
    return nullptr;
  }

  // Outlier m_logic and track finalisation
  std::unique_ptr<FitQuality> fitQuality = buildFitQuality(*smoothedTrajectory);
  if (!fitQuality) {
    ATH_MSG_DEBUG("Chi squared could not be calculated... Bailing");
    ++m_fitQualityFailure;
    return nullptr;
  }

  if (m_makePerigee) {
    const Trk::MultiComponentStateOnSurface* perigeeMultiStateOnSurface =
      makePerigee(
        ctx, extrapolatorCache, *smoothedTrajectory, particleHypothesis);
    ATH_MSG_DEBUG(
      "perigeeMultiStateOnSurface  :" << perigeeMultiStateOnSurface);
    if (perigeeMultiStateOnSurface) {
      smoothedTrajectory->push_back(perigeeMultiStateOnSurface);
    } else {
      ATH_MSG_DEBUG("Perigee asked to be created but failed.....Exiting");
      ++m_PerigeeFailure;
      return nullptr;
    }
  }

  // Reverse the order of the TSOS's to make be order flow from inside to out
  std::reverse(smoothedTrajectory->begin(), smoothedTrajectory->end());

  // Store only TSOS in tracks instead of MCSOS
  if (!m_StoreMCSOS) {
    auto slimmedSmoothedTrajectory =
      std::make_unique<Trk::SmoothedTrajectory>();
    for (const Trk::TrackStateOnSurface* tsos : *smoothedTrajectory) {
      slimmedSmoothedTrajectory->push_back(new Trk::TrackStateOnSurface(*tsos));
    }
    // Create new track
    Trk::TrackInfo info(Trk::TrackInfo::GaussianSumFilter, particleHypothesis);
    info.setTrackProperties(TrackInfo::BremFit);
    info.setTrackProperties(TrackInfo::BremFitSuccessful);
    ++m_fitSuccess;
    return std::make_unique<Track>(
      info, slimmedSmoothedTrajectory.release(), fitQuality.release());
  }

  // Create new track
  Trk::TrackInfo info(Trk::TrackInfo::GaussianSumFilter, particleHypothesis);
  info.setTrackProperties(TrackInfo::BremFit);
  info.setTrackProperties(TrackInfo::BremFitSuccessful);
  ++m_fitSuccess;
  return std::make_unique<Track>(
    info, smoothedTrajectory.release(), fitQuality.release());
}

/*
 * Fitting of a set of MeasurementBase objects
 */

std::unique_ptr<Trk::Track>
Trk::GaussianSumFitter::fit(
  const EventContext& ctx,
  const Trk::MeasurementSet& measurementSet,
  const Trk::TrackParameters& estimatedParametersNearOrigin,
  const Trk::RunOutlierRemoval outlierRemoval,
  const Trk::ParticleHypothesis particleHypothesis) const
{

  if (outlierRemoval) {
    ATH_MSG_DEBUG(
      "Outlier removal not yet implemented for the Gaussian Sum Filter");
  }

  ++m_FitMeasurementBase;
  // Protect against empty PrepRawDataSet object
  if (measurementSet.empty()) {
    ATH_MSG_FATAL("MeasurementSet for fit is empty... Exiting!");
    return nullptr;
  }

  // Find the CCOT if it exsists
  const Trk::CaloCluster_OnTrack* ccot(nullptr);
  Trk::MeasurementSet cleanedMeasurementSet;

  MeasurementSet::const_iterator itSet = measurementSet.begin();
  MeasurementSet::const_iterator itSetEnd = measurementSet.end();
  for (; itSet != itSetEnd; ++itSet) {
    if (!(*itSet)) {
      ATH_MSG_WARNING("There is an empty MeasurementBase object in the track! "
                      "Skip this object..");
    } else {
      ccot = nullptr;
      if ((*itSet)->type(Trk::MeasurementBaseType::CaloCluster_OnTrack)) {
        ccot = static_cast<const Trk::CaloCluster_OnTrack*>(*itSet);
      }
      if (!ccot) {
        cleanedMeasurementSet.push_back(*itSet);
      } else {
        ATH_MSG_DEBUG(
          "The  MeasurementBase object is a Trk::CaloCluster_OnTrack");
      }
    }
  }

  // A const stl container cannot be sorted. This will re-cast it so that it
  // can.
  Trk::MeasurementSet sortedMeasurementSet =
    MeasurementSet(cleanedMeasurementSet);

  if (m_doHitSorting) {

    Trk::MeasurementBaseComparisonFunction measurementBaseComparisonFunction(
      estimatedParametersNearOrigin.position(),
      estimatedParametersNearOrigin.momentum());
    sort(sortedMeasurementSet.begin(),
         sortedMeasurementSet.end(),
         measurementBaseComparisonFunction);
  }

  // Create Extrapolator cache that holds material effects cache;
  Trk::IMultiStateExtrapolator::Cache extrapolatorCache;

  // Perform GSF forwards fit - new memory allocated in forwards fitter
  std::unique_ptr<ForwardTrajectory> forwardTrajectory =
    fitMeasurements(ctx,
                    extrapolatorCache,
                    sortedMeasurementSet,
                    estimatedParametersNearOrigin,
                    particleHypothesis);

  if (!forwardTrajectory) {
    ATH_MSG_DEBUG("Forward GSF fit failed... Exiting!");
    ++m_ForwardFailure;
    return nullptr;
  }

  if (forwardTrajectory->empty()) {
    ATH_MSG_DEBUG("No states in forward trajectory... Exiting!");
    ++m_ForwardFailure;
    return nullptr;
  }

  // Perform GSF smoother operation

  std::unique_ptr<SmoothedTrajectory> smoothedTrajectory =
    fit(ctx, extrapolatorCache, *forwardTrajectory, particleHypothesis, ccot);

  // Protect against failed smoother fit
  if (!smoothedTrajectory) {
    ATH_MSG_DEBUG("Smoother GSF fit failed... Exiting!");
    ++m_SmootherFailure;
    return nullptr;
  }

  // Outlier m_logic and track finalisation
  std::unique_ptr<FitQuality> fitQuality = buildFitQuality(*smoothedTrajectory);

  if (!fitQuality) {
    ATH_MSG_DEBUG("Chi squared could not be calculated... Bailing");
    ++m_fitQualityFailure;
    return nullptr;
  }

  if (m_makePerigee) {
    const Trk::MultiComponentStateOnSurface* perigeeMultiStateOnSurface =
      makePerigee(
        ctx, extrapolatorCache, *smoothedTrajectory, particleHypothesis);
    ATH_MSG_DEBUG(
      "perigeeMultiStateOnSurface  :" << perigeeMultiStateOnSurface);

    if (perigeeMultiStateOnSurface) {
      smoothedTrajectory->push_back(perigeeMultiStateOnSurface);
    } else {
      ATH_MSG_DEBUG("Perigee asked to be created but failed.....Exiting");
      ++m_PerigeeFailure;
      return nullptr;
    }
  }

  // Reverse the order of the TSOS's to make be order flow from inside to out
  std::reverse(smoothedTrajectory->begin(), smoothedTrajectory->end());

  // Store only TSOS in tracks instead of MCSOS
  if (!m_StoreMCSOS) {
    auto slimmedSmoothedTrajectory =
      std::make_unique<Trk::SmoothedTrajectory>();
    for (const Trk::TrackStateOnSurface* tsos : *smoothedTrajectory) {
      slimmedSmoothedTrajectory->push_back(new Trk::TrackStateOnSurface(*tsos));
    }
    // Create new track
    Trk::TrackInfo info(Trk::TrackInfo::GaussianSumFilter, particleHypothesis);
    info.setTrackProperties(TrackInfo::BremFit);
    info.setTrackProperties(TrackInfo::BremFitSuccessful);
    ++m_fitSuccess;
    return std::make_unique<Track>(
      info, slimmedSmoothedTrajectory.release(), fitQuality.release());
  }

  // Create new track
  Trk::TrackInfo info(Trk::TrackInfo::GaussianSumFilter, particleHypothesis);
  info.setTrackProperties(TrackInfo::BremFit);
  info.setTrackProperties(TrackInfo::BremFitSuccessful);
  ++m_fitSuccess;
  return std::make_unique<Track>(
    info, smoothedTrajectory.release(), fitQuality.release());
}

std::unique_ptr<Trk::Track>
Trk::GaussianSumFitter::fit(const EventContext& ctx,
                            const Track& intrk,
                            const PrepRawDataSet& addPrdColl,
                            const RunOutlierRemoval runOutlier,
                            const ParticleHypothesis matEffects) const
{

  // protection, if empty PrepRawDataSet
  if (addPrdColl.empty()) {
    ATH_MSG_WARNING(
      "client tries to add an empty PrepRawDataSet to the track fit.");
    return fit(ctx, intrk, runOutlier, matEffects);
  }

  /*  determine the Track Parameter which is the start of the trajectory,
      i.e. closest to the reference point */
  ATH_MSG_DEBUG("get track parameters near origin "
                << (m_doHitSorting ? "via STL sort" : "from 1st state"));

  const TrackParameters* estimatedStartParameters =
    m_doHitSorting ? *(std::min_element(intrk.trackParameters()->begin(),
                                        intrk.trackParameters()->end(),
                                        *m_trkParametersComparisonFunction))
                   : *intrk.trackParameters()->begin();

  // use external preparator class to prepare PRD set for fitter interface

  Amg::Vector3D referencePosition(m_sortingReferencePoint[0],
                                  m_sortingReferencePoint[1],
                                  m_sortingReferencePoint[2]);

  TrackFitInputPreparator* inputPreparator =
    new TrackFitInputPreparator(referencePosition);

  PrepRawDataSet orderedPRDColl = inputPreparator->stripPrepRawData(
    intrk, addPrdColl, m_doHitSorting, true /* do not lose outliers! */);

  delete inputPreparator;

  return fit(
    ctx, orderedPRDColl, *estimatedStartParameters, runOutlier, matEffects);
}

std::unique_ptr<Trk::Track>
Trk::GaussianSumFitter::fit(const EventContext& ctx,
                            const Track& inputTrack,
                            const MeasurementSet& measurementSet,
                            const RunOutlierRemoval runOutlier,
                            const ParticleHypothesis matEffects) const
{

  // protection, if empty MeasurementSet
  if (measurementSet.empty()) {
    ATH_MSG_WARNING(
      "Client tries to add an empty MeasurementSet to the track fit.");
    return fit(ctx, inputTrack, runOutlier, matEffects);
  }

  // Check that the input track has well defined parameters
  if (inputTrack.trackParameters()->empty()) {
    ATH_MSG_FATAL("No estimation of track parameters near origin... Exiting!");
    return nullptr;
  }

  // Check that the input track has associated MeasurementBase objects
  if (inputTrack.trackStateOnSurfaces()->empty()) {
    ATH_MSG_FATAL("Attempting to fit track to empty MeasurementBase "
                  "collection... Exiting!");
    return nullptr;
  }

  // Retrieve the set of track parameters closest to the reference point
  const Trk::TrackParameters* parametersNearestReference =
    *(std::min_element(inputTrack.trackParameters()->begin(),
                       inputTrack.trackParameters()->end(),
                       *m_trkParametersComparisonFunction));

  MeasurementSet combinedMS = m_inputPreparator->stripMeasurements(
    inputTrack, measurementSet, true, false);

  // Apply GSF fit to MeasurementBase objects
  return fit(
    ctx, combinedMS, *parametersNearestReference, runOutlier, matEffects);
}

std::unique_ptr<Trk::Track>
Trk::GaussianSumFitter::fit(const EventContext& ctx,
                            const Track& intrk1,
                            const Track& intrk2,
                            const RunOutlierRemoval runOutlier,
                            const ParticleHypothesis matEffects) const
{
  // Not a great implementation but simple...  Just add the hits on track
  // protection against not having measurements on the input tracks
  if (!intrk1.trackStateOnSurfaces() || !intrk2.trackStateOnSurfaces() ||
      intrk1.trackStateOnSurfaces()->size() < 2) {
    ATH_MSG_WARNING("called to refit empty track or track with too little "
                    "information, reject fit");
    return nullptr;
  }

  if (!intrk1.trackParameters() || intrk1.trackParameters()->empty()) {
    ATH_MSG_WARNING("input #1 fails to provide track parameters for "
                    "seeding the GXF, reject fit");
    return nullptr;
  }

  const TrackParameters* minPar = *intrk1.trackParameters()->begin();
  DataVector<const TrackStateOnSurface>::const_iterator itStates =
    intrk1.trackStateOnSurfaces()->begin();
  DataVector<const TrackStateOnSurface>::const_iterator endState =
    intrk1.trackStateOnSurfaces()->end();
  DataVector<const TrackStateOnSurface>::const_iterator itStates2 =
    intrk2.trackStateOnSurfaces()->begin();
  DataVector<const TrackStateOnSurface>::const_iterator endState2 =
    intrk2.trackStateOnSurfaces()->end();

  Trk::MeasurementSet ms;

  for (; itStates != endState; ++itStates) {
    if (!((*itStates)->type(Trk::TrackStateOnSurface::Measurement) ||
          (*itStates)->type(Trk::TrackStateOnSurface::Outlier))) {
      continue;
    }

    if ((*itStates)->measurementOnTrack()->type(
          Trk::MeasurementBaseType::PseudoMeasurementOnTrack)) {
      continue;
    }

    ms.push_back((*itStates)->measurementOnTrack());
  }

  for (; itStates2 != endState2; ++itStates2) {

    if (!((*itStates2)->type(Trk::TrackStateOnSurface::Measurement) ||
          (*itStates2)->type(Trk::TrackStateOnSurface::Outlier))) {
      continue;
    }

    if ((*itStates2)
          ->measurementOnTrack()
          ->type(Trk::MeasurementBaseType::PseudoMeasurementOnTrack)) {
      continue;
    }

    ms.push_back((*itStates2)->measurementOnTrack());
  }

  return fit(ctx, ms, *minPar, runOutlier, matEffects);
}

const Trk::MultiComponentStateOnSurface*
Trk::GaussianSumFitter::makePerigee(
  const EventContext& ctx,
  Trk::IMultiStateExtrapolator::Cache& extrapolatorCache,
  const Trk::SmoothedTrajectory& smoothedTrajectory,
  const Trk::ParticleHypothesis particleHypothesis) const
{

  // Propagate track to perigee
  const Trk::PerigeeSurface perigeeSurface;
  const Trk::TrackStateOnSurface* stateOnSurfaceNearestOrigin =
    smoothedTrajectory.back();
  const Trk::MultiComponentStateOnSurface*
    multiComponentStateOnSurfaceNearestOrigin = nullptr;

  if (stateOnSurfaceNearestOrigin->variety() ==
      Trk::TrackStateOnSurface::MultiComponent) {

    multiComponentStateOnSurfaceNearestOrigin =
      static_cast<const Trk::MultiComponentStateOnSurface*>(
        stateOnSurfaceNearestOrigin);
  }

  const Trk::MultiComponentState* multiComponentState = nullptr;
  if (!multiComponentStateOnSurfaceNearestOrigin) {
    // we need to make a dummy multicomponent surface
    Trk::ComponentParameters dummyComponent(
      stateOnSurfaceNearestOrigin->trackParameters()->clone(), 1.);
    auto tmp_multiComponentState = std::make_unique<Trk::MultiComponentState>();
    tmp_multiComponentState->push_back(std::move(dummyComponent));
    multiComponentState = tmp_multiComponentState.release();
  } else {
    multiComponentState =
      multiComponentStateOnSurfaceNearestOrigin->components();
  }
  // Extrapolate to perigee, taking material effects considerations into account
  Trk::MultiComponentState stateExtrapolatedToPerigee =
    m_extrapolator->extrapolate(ctx,
                                extrapolatorCache,
                                *multiComponentState,
                                perigeeSurface,
                                m_directionToPerigee,
                                false,
                                particleHypothesis);

  if (stateExtrapolatedToPerigee.empty()) {
    ATH_MSG_DEBUG("Track could not be extrapolated to perigee... returning 0");
    return nullptr;
  }

  // Clean-up & pointer reset
  if (!multiComponentStateOnSurfaceNearestOrigin) {
    delete multiComponentState;
  }
  multiComponentState = nullptr;

  // Calculate the mode of the q/p distribution
  double modeQoverP = 0;

  // Determine the combined state as well to be passed to the
  // MultiComponentStateOnSurface object
  std::unique_ptr<Trk::TrackParameters> combinedPerigee =
    MultiComponentStateCombiner::combine(stateExtrapolatedToPerigee, true);

  // Perigee is given as an additional MultiComponentStateOnSurface
  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>
    pattern(0);
  pattern.set(Trk::TrackStateOnSurface::Perigee);

  if (std::abs(combinedPerigee->parameters()[Trk::qOverP]) > 1e8) {
    // Protection against 0-momentum track .. this check should NEVER be needed.
    // actual cutoff is 0.01eV track
    ATH_MSG_ERROR(
      "makePerigee() about to return with 0 momentum!! Returning null instead");
    return nullptr;
  }

  const Trk::MultiComponentStateOnSurface* perigeeMultiStateOnSurface =
    new MultiComponentStateOnSurface(
      nullptr,
      combinedPerigee.release(),
      MultiComponentStateHelpers::clone(stateExtrapolatedToPerigee).release(),
      nullptr,
      nullptr,
      pattern,
      modeQoverP);
  ATH_MSG_DEBUG("makePerigee() returning sucessfully!");
  return perigeeMultiStateOnSurface;
}

std::unique_ptr<Trk::FitQuality>
Trk::GaussianSumFitter::buildFitQuality(
  const Trk::SmoothedTrajectory& smoothedTrajectory) const
{

  double chiSquared = 0.;
  int numberDoF = -5;
  // Loop over all TrackStateOnSurface objects in trajectory
  SmoothedTrajectory::const_iterator stateOnSurface =
    smoothedTrajectory.begin();
  for (; stateOnSurface != smoothedTrajectory.end(); ++stateOnSurface) {
    if (!(*stateOnSurface)->type(TrackStateOnSurface::Measurement)) {
      continue;
    }
    if ((*stateOnSurface)->fitQualityOnSurface() == nullptr) {
      continue;
    }
    chiSquared += (*stateOnSurface)->fitQualityOnSurface()->chiSquared();
    numberDoF += (*stateOnSurface)->fitQualityOnSurface()->numberDoF();
  }

  if (std::isnan(chiSquared) || chiSquared <= 0.) {
    return nullptr;
  }

  return std::make_unique<FitQuality>(chiSquared, numberDoF);
}

/*
 * Forwards fit on a set of PrepRawData
 */
std::unique_ptr<Trk::ForwardTrajectory>
Trk::GaussianSumFitter::fitPRD(
  const EventContext& ctx,
  Trk::IMultiStateExtrapolator::Cache& extrapolatorCache,
  const Trk::PrepRawDataSet& inputPrepRawDataSet,
  const Trk::TrackParameters& estimatedTrackParametersNearOrigin,
  const Trk::ParticleHypothesis particleHypothesis) const
{

  // Configure for forwards filtering material effects overide
  Trk::ParticleHypothesis configuredParticleHypothesis;

  if (m_overideMaterialEffectsSwitch) {
    configuredParticleHypothesis = m_overideParticleHypothesis;
  } else {
    configuredParticleHypothesis = particleHypothesis;
  }

  // Extract PrepRawDataSet into new local object and check that the PrepRawData
  // is associated with a detector element
  Trk::PrepRawDataSet prepRawDataSet;
  Trk::PrepRawDataSet::const_iterator prepRawData = inputPrepRawDataSet.begin();

  for (; prepRawData != inputPrepRawDataSet.end(); ++prepRawData) {

    if (!(*prepRawData)->detectorElement()) {
      ATH_MSG_WARNING("PrepRawData has no Element link... disregard it");
    } else {
      prepRawDataSet.push_back(*prepRawData);
    }
  }

  // Create new trajectory
  auto forwardTrajectory = std::make_unique<Trk::ForwardTrajectory>();

  // Prepare the multi-component state. For starting guess this has single
  // component, weight 1
  const AmgVector(5)& par = estimatedTrackParametersNearOrigin.parameters();

  Trk::ComponentParameters componentParametersNearOrigin(
    estimatedTrackParametersNearOrigin.associatedSurface()
      .createTrackParameters(par[Trk::loc1],
                             par[Trk::loc2],
                             par[Trk::phi],
                             par[Trk::theta],
                             par[Trk::qOverP],
                             nullptr /*no errors*/),
    1.);

  Trk::MultiComponentState multiComponentStateNearOrigin{};
  multiComponentStateNearOrigin.push_back(
    std::move(componentParametersNearOrigin));

  // Loop over all PrepRawData measurements
  prepRawData = prepRawDataSet.begin();

  for (; prepRawData != prepRawDataSet.end(); ++prepRawData) {

    // Every valid step the ForwardTrajectory object passed to the
    // stepForwardFit method is updated
    bool stepIsValid = stepForwardFit(
      ctx,
      extrapolatorCache,
      forwardTrajectory.get(),
      *prepRawData,
      nullptr,
      (*prepRawData)->detectorElement()->surface((*prepRawData)->identify()),
      multiComponentStateNearOrigin,
      configuredParticleHypothesis);

    if (!stepIsValid) {
      ATH_MSG_DEBUG("Fitter step is not valid... Exiting!");
      return nullptr;
    }
  }
  return forwardTrajectory;
}

/*
 * Forwards fit on a set of Measurements
 */
std::unique_ptr<Trk::ForwardTrajectory>
Trk::GaussianSumFitter::fitMeasurements(
  const EventContext& ctx,
  Trk::IMultiStateExtrapolator::Cache& extrapolatorCache,
  const Trk::MeasurementSet& inputMeasurementSet,
  const Trk::TrackParameters& estimatedTrackParametersNearOrigin,
  const Trk::ParticleHypothesis particleHypothesis) const
{

  if (!m_extrapolator) {
    ATH_MSG_ERROR("The extrapolator is not configured... Exiting!");
    return nullptr;
  }

  if (inputMeasurementSet.empty()) {
    ATH_MSG_ERROR("Input MeasurementSet is empty... Exiting!");
    return nullptr;
  }

  // Configure for forwards filtering material effects overide
  Trk::ParticleHypothesis configuredParticleHypothesis;

  if (m_overideMaterialEffectsSwitch) {
    configuredParticleHypothesis = m_overideParticleHypothesis;
  } else {
    configuredParticleHypothesis = particleHypothesis;
  }

  // This memory should be freed by the fitter / smoother master method
  auto forwardTrajectory = std::make_unique<Trk::ForwardTrajectory>();

  // Prepare the multi-component state. For starting guess this has single
  // component, weight 1
  const AmgVector(5)& par = estimatedTrackParametersNearOrigin.parameters();

  AmgSymMatrix(5)* covariance = nullptr;

  Trk::ComponentParameters componentParametersNearOrigin(
    estimatedTrackParametersNearOrigin.associatedSurface()
      .createTrackParameters(par[Trk::loc1],
                             par[Trk::loc2],
                             par[Trk::phi],
                             par[Trk::theta],
                             par[Trk::qOverP],
                             covariance /*no errors*/),
    1.);

  Trk::MultiComponentState multiComponentStateNearOrigin{};
  multiComponentStateNearOrigin.push_back(
    std::move(componentParametersNearOrigin));

  // Loop over all MeasurementBase objects in set
  Trk::MeasurementSet::const_iterator measurement = inputMeasurementSet.begin();

  for (; measurement != inputMeasurementSet.end(); ++measurement) {

    bool stepIsValid = stepForwardFit(ctx,
                                      extrapolatorCache,
                                      forwardTrajectory.get(),
                                      nullptr,
                                      *measurement,
                                      (*measurement)->associatedSurface(),
                                      multiComponentStateNearOrigin,
                                      configuredParticleHypothesis);

    if (!stepIsValid) {
      return nullptr;
    }
  }
  return forwardTrajectory;
}

/*
 *   StepForwardFit() private method
 */
bool
Trk::GaussianSumFitter::stepForwardFit(
  const EventContext& ctx,
  Trk::IMultiStateExtrapolator::Cache& extrapolatorCache,
  ForwardTrajectory* forwardTrajectory,
  const Trk::PrepRawData* originalPrepRawData,
  const Trk::MeasurementBase* originalMeasurement,
  const Trk::Surface& surface,
  Trk::MultiComponentState& updatedState,
  const Trk::ParticleHypothesis particleHypothesis) const
{
  // Protect against undefined Measurement or PrepRawData
  if (!originalPrepRawData && !originalMeasurement) {
    ATH_MSG_WARNING(
      "No measurement information passed to StepForwardFit... Exiting!");
    return false;
  }

  // Protect against ForwardTrajectory not defined
  if (!forwardTrajectory) {
    ATH_MSG_WARNING("ForwardTrajectory object is not defined... Exiting!");
    return false;
  }

  // Extrapolate multi-component state to the next measurement surface
  Trk::MultiComponentState extrapolatedState =
    m_extrapolator->extrapolate(ctx,
                                extrapolatorCache,
                                updatedState,
                                surface,
                                Trk::alongMomentum,
                                false,
                                particleHypothesis);
  if (extrapolatedState.empty()) {
    ATH_MSG_DEBUG("Extrapolation failed... returning false");
    return false;
  } else {
    ATH_MSG_DEBUG(
      "Extrapolation worked... state size: " << extrapolatedState.size());
  }
  // =======================
  // Measurement Preparation
  // =======================
  std::unique_ptr<Trk::TrackParameters> combinedState = nullptr;
  std::unique_ptr<const Trk::MeasurementBase> measurement = nullptr;
  if (originalMeasurement) {
    // Clone original MeasurementBase object (refit with no new calibration)
    measurement.reset(originalMeasurement->clone());
  } else {
    combinedState = MultiComponentStateCombiner::combine(extrapolatedState);
    if (!combinedState) {
      ATH_MSG_WARNING("State combination failed... exiting");
      return false;
    }
    // Create a new MeasurementBase object from PrepRawData using new
    // calibration
    measurement.reset(
      m_rioOnTrackCreator->correct(*originalPrepRawData, *combinedState));
    combinedState.reset();
  }

  // Perform measurement update
  if (!measurement) {
    ATH_MSG_WARNING("Cannot use MeasurementBase for measurement update, it is "
                    "not defined... Exiting!");
    return false;
  }

  auto fitQuality = std::make_unique<Trk::FitQualityOnSurface>();
  updatedState = m_updator.update(
    std::move(*(MultiComponentStateHelpers::clone(extrapolatedState))),
    *measurement,
    *fitQuality);
  if (updatedState.empty()) {
    ATH_MSG_DEBUG("Measurement update of the state failed... Exiting!");
    return false;
  }

  // Reject hits with excessive Chi2
  if (fitQuality->chiSquared() >
      m_cutChiSquaredPerNumberDOF * fitQuality->numberDoF()) {
    ATH_MSG_DEBUG("Update with new measurement caused track to fail Chi "
                  "Squared test, removing the object");
    fitQuality = std::make_unique<FitQuality>(1, 1);
    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type(0);
    type.set(TrackStateOnSurface::Outlier);
    const Trk::MultiComponentStateOnSurface* multiComponentStateOnSurface =
      new MultiComponentStateOnSurface(
        measurement.release(),
        MultiComponentStateHelpers::clone(extrapolatedState).release(),
        fitQuality.release(),
        nullptr,
        type);

    forwardTrajectory->push_back(multiComponentStateOnSurface);
    // Clean up objects associated with removed measurement
    updatedState = std::move(extrapolatedState);
  } else {
    const Trk::MultiComponentStateOnSurface* multiComponentStateOnSurface =
      new MultiComponentStateOnSurface(
        measurement.release(),
        MultiComponentStateHelpers::clone(extrapolatedState).release(),
        fitQuality.release());
    forwardTrajectory->push_back(multiComponentStateOnSurface);
  }
  return true;
}

std::unique_ptr<Trk::SmoothedTrajectory>
Trk::GaussianSumFitter::fit(
  const EventContext& ctx,
  Trk::IMultiStateExtrapolator::Cache& extrapolatorCache,
  const ForwardTrajectory& forwardTrajectory,
  const ParticleHypothesis particleHypothesis,
  const Trk::CaloCluster_OnTrack* ccot) const
{
  // Check that the forward trajectory is filled
  if (forwardTrajectory.empty()) {
    ATH_MSG_ERROR(
      "Attempting to smooth an empty forward trajectory... Exiting!");
    return nullptr;
  }
  /* Instantiate the returned trajectoy. This is a vector of TrackStateOnSurface
     object In the GSF Smoother these TrackStateOnSurface objects are the base
     class for the MultiComponentStateOnSurface. This memory should be freed by
     the fitter / smoother master method */

  /*
   * In general for things we want to control their lifetime locally
   * we use unique_ptr
   *
   * For things where we get or pass ownership to a multistate and then continue
   * to use them we opt for const ptr* for now.
   */
  auto smoothedTrajectory = std::make_unique<Trk::SmoothedTrajectory>();
  auto smootherPredictionMultiState =
    std::make_unique<Trk::MultiComponentState>();
  /*
   * Get the initial smoother prediction. It is the last prediction in the
   * forwards trajectory
   */
  /* these can be  owned by the input so const ptr*/
  const Trk::TrackStateOnSurface* smootherPredictionStateOnSurface =
    forwardTrajectory.back();
  /*
   * We can have single or Multi components here, so we choose what to clone
   */
  const Trk::MultiComponentStateOnSurface*
    smootherPredictionMultiStateOnSurface = nullptr;

  // Check if we already have a MultiComponent state
  if (smootherPredictionStateOnSurface->variety() ==
      Trk::TrackStateOnSurface::MultiComponent) {
    smootherPredictionMultiStateOnSurface =
      static_cast<const Trk::MultiComponentStateOnSurface*>(
        smootherPredictionStateOnSurface);
  }

  if (!smootherPredictionMultiStateOnSurface) {
    auto dummyMultiState = std::make_unique<Trk::MultiComponentState>();
    Trk::ComponentParameters dummyParams(
      smootherPredictionStateOnSurface->trackParameters()->clone(), 1.0);
    dummyMultiState->push_back(std::move(dummyParams));
    smootherPredictionMultiState = std::move(dummyMultiState);
  } else {
    smootherPredictionMultiState = MultiComponentStateHelpers::clone(
      *(smootherPredictionMultiStateOnSurface->components()));
  }

  /*
   * Perform the measurement update
   * The first smoother prediction state is equivalent to the last forwards
   * prediction updated with the last measurement Retrieve the first
   * MeasurementBase object for the smoother
   */
  Trk::FitQualityOnSurface fitQuality;
  /*
   * The following 2 pointers if not null will be owned by the
   * MultiComponentStateOnSurface* updatedStateOnSurface
   * at the end of the code and we need to re-use them.
   * So const ptr
   */
  const Trk::MeasurementBase* firstSmootherMeasurementOnTrack =
    smootherPredictionStateOnSurface->measurementOnTrack()->clone();
  if (!firstSmootherMeasurementOnTrack) {
    ATH_MSG_WARNING(
      "Initial state on surface in smoother does not have an associated "
      "MeasurementBase object... returning 0");
    return nullptr;
  }
  Trk::MultiComponentState firstSmoothedState =
    m_updator.update(std::move(*smootherPredictionMultiState),
                     *firstSmootherMeasurementOnTrack,
                     fitQuality);

  if (firstSmoothedState.empty()) {
    ATH_MSG_DEBUG("First GSF smoothing update failed... Exiting!");
    delete firstSmootherMeasurementOnTrack;
    return nullptr;
  }

  std::unique_ptr<Trk::TrackParameters> combinedFirstSmoothedState =
    MultiComponentStateCombiner::combine(firstSmoothedState, true);

  /*Create updatedStateOnSurface which owned be the tragectory*/
  const Trk::MultiComponentStateOnSurface* updatedStateOnSurface =
    new MultiComponentStateOnSurface(
      firstSmootherMeasurementOnTrack,
      combinedFirstSmoothedState.release(),
      MultiComponentStateHelpers::clone(firstSmoothedState).release(),
      (std::make_unique<Trk::FitQualityOnSurface>(fitQuality)).release());
  smoothedTrajectory->push_back(updatedStateOnSurface);

  /*
   * Reset pointers
   * first the const ptr
   */
  smootherPredictionStateOnSurface = nullptr;
  updatedStateOnSurface = nullptr;
  smootherPredictionMultiStateOnSurface = nullptr;
  /*
   * Then the unique ptr
   */
  smootherPredictionMultiState.reset();

  if (!MultiComponentStateHelpers::isMeasured(firstSmoothedState)) {
    ATH_MSG_WARNING(
      "Updated state is not measured. Rejecting smoothed state... returning 0");
    return nullptr;
  }
  /*
   * Generate prediction by scaling the covariance of all components in the
   * initial state
   *
   * Generate a large prediction for extrapolation. This way there is no
   * dependance on error of prediction NB local Y and theta are not blown out
   * too much to help in the TRT
   */
  std::unique_ptr<Trk::MultiComponentState> smoothedStateWithScaledError =
    MultiComponentStateHelpers::cloneWithScaledError(
      firstSmoothedState, 15., 5., 15., 5., 15.);

  // Perform a measurement update on this new state
  Trk::MultiComponentState updatedState = m_updator.update(
    std::move(*smoothedStateWithScaledError), *firstSmootherMeasurementOnTrack);

  if (updatedState.empty()) {
    ATH_MSG_WARNING(
      "Smoother prediction could not be determined... returning 0");
    return nullptr;
  }

  // Clear rioOnTrack pointer
  firstSmootherMeasurementOnTrack = nullptr;

  // Loop over all remaining TrackStateOnSurface objects in the forward
  // trajectory
  Trk::ForwardTrajectory::const_reverse_iterator trackStateOnSurface =
    forwardTrajectory.rbegin() + 1;
  Trk::ForwardTrajectory::const_reverse_iterator lasttrackStateOnSurface =
    forwardTrajectory.rend() - 1;
  // TSOS that the cluster measuremenet will added on after .. can’t be teh last
  // as the impact parameters will be compromised
  Trk::ForwardTrajectory::const_reverse_iterator secondLastTrackStateOnSurface =
    forwardTrajectory.rend() - 2;

  for (; trackStateOnSurface != forwardTrajectory.rend();
       ++trackStateOnSurface) {
    // Retrieve the MeasurementBase object from the TrackStateOnSurface object
    const Trk::MeasurementBase* measurement_in =
      (*trackStateOnSurface)->measurementOnTrack();
    if (!measurement_in) {
      ATH_MSG_WARNING("MeasurementBase object could not be extracted from a "
                      "measurement... continuing");
      continue;
    }
    // Clone the MeasurementBase object
    std::unique_ptr<const Trk::MeasurementBase> measurement =
      std::unique_ptr<const Trk::MeasurementBase>(measurement_in->clone());
    /* Extrapolate the current multi-component state to the
     next measurement surface. For the smoother the direction of propagation
     is opposite to the direction of momentum */

    Trk::MultiComponentState extrapolatedState =
      m_extrapolator->extrapolate(ctx,
                                  extrapolatorCache,
                                  updatedState,
                                  measurement->associatedSurface(),
                                  Trk::oppositeMomentum,
                                  false,
                                  particleHypothesis);

    if (extrapolatedState.empty()) {
      ATH_MSG_DEBUG(
        "Extrapolation to measurement surface failed... rejecting track!");
      return nullptr;
    }

    // Original measurement was flagged as  an outlier
    if (!(*trackStateOnSurface)->type(TrackStateOnSurface::Measurement)) {
      updatedState = std::move(extrapolatedState);

      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type(
        0);
      type.set(TrackStateOnSurface::Outlier);

      Trk::MultiComponentStateOnSurface* updatedStateOnSurface =
        new Trk::MultiComponentStateOnSurface(
          measurement.release(),
          MultiComponentStateHelpers::clone(updatedState).release(),
          new FitQuality(1, 1),
          nullptr,
          type);
      smoothedTrajectory->push_back(updatedStateOnSurface);
      continue;
    }

    // Update newly extrapolated state with MeasurementBase measurement
    updatedState =
      m_updator.update(std::move(extrapolatedState), *measurement, fitQuality);
    if (updatedState.empty()) {
      ATH_MSG_WARNING(
        "Could not update the multi-component state... rejecting track!");
      return nullptr;
    }

    /* =============================================================
       Optional combine smoother state with fitter state
       ============================================================= */

    if (m_combineWithFitter) {

      const Trk::MultiComponentState* forwardsMultiState = nullptr;
      std::unique_ptr<Trk::MultiComponentState> forwardsMultiStateOwn;

      const Trk::MultiComponentStateOnSurface* forwardsMultiStateOnSurface =
        nullptr;
      // Check if we already have a MultiComponent state on surface
      if ((*trackStateOnSurface)->variety() ==
          Trk::TrackStateOnSurface::MultiComponent) {
        forwardsMultiStateOnSurface =
          static_cast<const Trk::MultiComponentStateOnSurface*>(
            *trackStateOnSurface);
      }

      if (!forwardsMultiStateOnSurface) {
        // Create new multiComponentState from single state
        Trk::ComponentParameters componentParameters(
          (*trackStateOnSurface)->trackParameters()->clone(), 1.);
        forwardsMultiStateOwn = std::make_unique<Trk::MultiComponentState>();
        forwardsMultiStateOwn->push_back(std::move(componentParameters));
        forwardsMultiState = forwardsMultiStateOwn.get();
      } else {
        forwardsMultiState = forwardsMultiStateOnSurface->components();
      }
      Trk::MultiComponentState combinedState2 =
        combine(*forwardsMultiState, updatedState);
      if (combinedState2.empty()) {
        ATH_MSG_WARNING("Could not combine state from forward fit with "
                        "smoother state... rejecting track!");
        return nullptr;
      }
      auto combinedFitQuality = std::make_unique<Trk::FitQualityOnSurface>(
        m_updator.fitQuality(combinedState2, *measurement));
      // In the case of combination with forwards state - push back the combined
      // state
      const Trk::MultiComponentStateOnSurface* combinedStateOnSurface =
        new MultiComponentStateOnSurface(
          measurement.release(),
          MultiComponentStateHelpers::clone(combinedState2).release(),
          combinedFitQuality.release());
      smoothedTrajectory->push_back(combinedStateOnSurface);
    } else {
      // If combination with forwards state is not done - push back updated
      // state
      const Trk::MultiComponentStateOnSurface* updatedStateOnSurface = nullptr;

      if (trackStateOnSurface == lasttrackStateOnSurface) {
        std::unique_ptr<Trk::TrackParameters> combinedLastState =
          MultiComponentStateCombiner::combine(updatedState, true);

        if (combinedLastState) {
          updatedStateOnSurface = new Trk::MultiComponentStateOnSurface(
            measurement.release(),
            combinedLastState.release(),
            MultiComponentStateHelpers::clone(updatedState).release(),
            (std::make_unique<Trk::FitQualityOnSurface>(fitQuality)).release());
        } else {
          updatedStateOnSurface = new Trk::MultiComponentStateOnSurface(
            measurement.release(),
            MultiComponentStateHelpers::clone(updatedState).release(),
            (std::make_unique<Trk::FitQualityOnSurface>(fitQuality)).release());
        }
      } else {
        updatedStateOnSurface = new Trk::MultiComponentStateOnSurface(
          measurement.release(),
          MultiComponentStateHelpers::clone(updatedState).release(),
          (std::make_unique<Trk::FitQualityOnSurface>(fitQuality)).release());
      }

      smoothedTrajectory->push_back(updatedStateOnSurface);

      /* =============================================================
         Add measurement from calo if  it is present
         ============================================================= */

      if (ccot && trackStateOnSurface == secondLastTrackStateOnSurface) {

        Trk::MultiComponentState ccotState =
          addCCOT(ctx, updatedStateOnSurface, ccot, *smoothedTrajectory);
        if (!ccotState.empty()) {
          updatedState = std::move(ccotState);
        }
      }
    }
  } // End for loop over all components
  return smoothedTrajectory;
}

Trk::MultiComponentState
Trk::GaussianSumFitter::combine(
  const Trk::MultiComponentState& forwardsMultiState,
  const Trk::MultiComponentState& smootherMultiState) const
{

  std::unique_ptr<Trk::MultiComponentState> combinedMultiState =
    std::make_unique<MultiComponentState>();
  /* ================================================
     Loop over all components in forwards multi-state
     ================================================ */

  for (auto& forwardsComponent : forwardsMultiState) {
    // Need to check that all components have associated weight matricies
    const AmgSymMatrix(5)* forwardMeasuredCov =
      forwardsComponent.first->covariance();

    /* ====================================================
       Loop over all components in the smoother multi-state
       ==================================================== */

    for (auto& smootherComponent : smootherMultiState) {
      // Need to check that all components have associated weight matricies
      const AmgSymMatrix(5)* smootherMeasuredCov =
        smootherComponent.first->covariance();
      if (!smootherMeasuredCov && !forwardMeasuredCov) {
        ATH_MSG_WARNING("Cannot combine two components both without associated "
                        "errors... returning 0");
        return {};
      }

      if (!forwardMeasuredCov) {
        Trk::ComponentParameters smootherComponentOnly(
          smootherComponent.first->clone(), smootherComponent.second);
        combinedMultiState->push_back(std::move(smootherComponentOnly));
        continue;
      }

      if (!smootherMeasuredCov) {
        Trk::ComponentParameters forwardComponentOnly(
          forwardsComponent.first->clone(), forwardsComponent.second);
        combinedMultiState->push_back(std::move(forwardComponentOnly));
        continue;
      }

      const AmgSymMatrix(5) summedCovariance =
        *forwardMeasuredCov + *smootherMeasuredCov;
      const AmgSymMatrix(5) K =
        *forwardMeasuredCov * summedCovariance.inverse();
      const Amg::VectorX newParameters =
        forwardsComponent.first->parameters() +
        K * (smootherComponent.first->parameters() -
             forwardsComponent.first->parameters());
      const Amg::VectorX parametersDiff =
        forwardsComponent.first->parameters() -
        smootherComponent.first->parameters();

      AmgSymMatrix(5)* covarianceOfNewParameters =
        new AmgSymMatrix(5)(K * *smootherMeasuredCov);

      Trk::TrackParameters* combinedTrackParameters =
        (forwardsComponent.first)
          ->associatedSurface()
          .createTrackParameters(newParameters[Trk::loc1],
                                 newParameters[Trk::loc2],
                                 newParameters[Trk::phi],
                                 newParameters[Trk::theta],
                                 newParameters[Trk::qOverP],
                                 covarianceOfNewParameters);
      // Covariance matrix object now owned by TrackParameters object. Reset
      // pointer to prevent delete
      covarianceOfNewParameters = nullptr;
      const AmgSymMatrix(5) invertedSummedCovariance =
        summedCovariance.inverse();
      // Determine the scaling factor for the new weighting. Determined from the
      // PDF of the many-dimensional gaussian
      double exponent =
        parametersDiff.transpose() * invertedSummedCovariance * parametersDiff;
      double weightScalingFactor = exp(-0.5 * exponent);
      double combinedWeight = smootherComponent.second *
                              forwardsComponent.second * weightScalingFactor;
      Trk::ComponentParameters combinedComponent(combinedTrackParameters,
                                                 combinedWeight);
      combinedMultiState->push_back(std::move(combinedComponent));
    }
  }

  // Component reduction on the combined state
  Trk::MultiComponentState mergedState =
    QuickCloseComponentsMultiStateMerger::merge(std::move(*combinedMultiState),
                                                m_maximumNumberOfComponents);

  // Before return the weights of the states need to be renormalised to one.
  MultiComponentStateHelpers::renormaliseState(mergedState);

  return mergedState;
}

Trk::MultiComponentState
Trk::GaussianSumFitter::addCCOT(
  const EventContext& ctx,
  const Trk::TrackStateOnSurface* currentState,
  const Trk::CaloCluster_OnTrack* ccot,
  Trk::SmoothedTrajectory& smoothedTrajectory) const
{

  const Trk::MultiComponentStateOnSurface* currentMultiStateOS = nullptr;

  // Check if we already have a MultiComponent state
  if (currentState->variety() == Trk::TrackStateOnSurface::MultiComponent) {
    currentMultiStateOS =
      static_cast<const Trk::MultiComponentStateOnSurface*>(currentState);
  }

  if (!currentMultiStateOS || !ccot) {
    return {};
  }

  const Trk::MultiComponentState* currentMultiComponentState =
    currentMultiStateOS->components();
  const Trk::MeasurementBase* measurement = currentState->measurementOnTrack();
  const Trk::Surface* currentSurface(nullptr);

  if (measurement) {
    currentSurface = &(measurement->associatedSurface());
  }
  Trk::MultiComponentState extrapolatedState{};
  // Extrapolate to the Calo
  if (currentSurface) {
    extrapolatedState =
      m_extrapolator->extrapolateDirectly(ctx,
                                          *currentMultiComponentState,
                                          ccot->associatedSurface(),
                                          Trk::alongMomentum,
                                          false,
                                          Trk::nonInteracting);
  }

  // Extrapolation Failed continue
  if (extrapolatedState.empty()) {
    return {};
  }
  // Update newly extrapolated state with MeasurementBase measurement
  Trk::FitQualityOnSurface fitQuality;
  Trk::MultiComponentState updatedState =
    m_updator.update(std::move(extrapolatedState), *ccot, fitQuality);

  if (updatedState.empty()) {
    return {};
  }

  // Build TSOS with CCOT at the surface of the surface of calo
  Trk::MultiComponentStateOnSurface* updatedMCSOS =
    new Trk::MultiComponentStateOnSurface(
      ccot->clone(),
      MultiComponentStateHelpers::clone(updatedState).release(),
      (std::make_unique<Trk::FitQualityOnSurface>(fitQuality)).release());

  // Extrapolate back to the surface nearest the origin
  extrapolatedState = m_extrapolator->extrapolateDirectly(ctx,
                                                          updatedState,
                                                          *currentSurface,
                                                          Trk::oppositeMomentum,
                                                          false,
                                                          Trk::nonInteracting);

  if (extrapolatedState.empty()) {
    delete updatedMCSOS;
    return {};
  }

  // Now build a dummy measurement ....  we dont want to a double count the
  // measurement but we need to extrapolate back to origin to allow for the
  // perigee parameters to be estimated Note this only important if the track is
  // refit otherwise it has no influence.
  AmgSymMatrix(5) covMatrix;
  covMatrix.setZero();
  covMatrix(0, 0) = 1e6;

  Trk::DefinedParameter locX(0, Trk::locX);
  std::vector<Trk::DefinedParameter> defPar;
  defPar.push_back(locX);
  Trk::LocalParameters locpars(defPar);

  Trk::PseudoMeasurementOnTrack* pseudoMeasurement =
    new PseudoMeasurementOnTrack(locpars, covMatrix, *currentSurface);

  //  Combine the state using and find the mode of the distribution
  std::unique_ptr<Trk::TrackParameters> combinedState =
    MultiComponentStateCombiner::combine(extrapolatedState, true);
  auto combinedFitQuality = std::make_unique<Trk::FitQualityOnSurface>(
    m_updator.fitQuality(extrapolatedState, *ccot));

  // Build a TSOS using the dummy measurement and combined state
  Trk::MultiComponentStateOnSurface* finalSOS =
    new Trk::MultiComponentStateOnSurface(
      pseudoMeasurement,
      combinedState.release(),
      MultiComponentStateHelpers::clone(extrapolatedState).release(),
      combinedFitQuality.release());
  smoothedTrajectory.push_back(updatedMCSOS);
  smoothedTrajectory.push_back(finalSOS);

  return extrapolatedState;
}
