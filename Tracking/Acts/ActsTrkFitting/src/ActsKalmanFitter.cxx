// /*
//   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
// */

#include "ActsKalmanFitter.h"

// ATHENA
#include "GaudiKernel/ListItem.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrackSummary/TrackSummary.h"

// ACTS
#include "Acts/Definitions/TrackParametrization.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/TrackFitting/KalmanFitter.hpp"
#include "Acts/TrackFitting/GainMatrixSmoother.hpp"
#include "Acts/TrackFitting/GainMatrixUpdater.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/CalibrationContext.hpp"


// PACKAGE
#include "ActsGeometry/ActsATLASConverterTool.h"
#include "ActsGeometry/ActsDetectorElement.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsInterop/Logger.h"

// STL
#include <memory>
#include <vector>
#include <fstream>
#include <string>

namespace {

template <typename track_fitter_t>
struct TrackFitterFunctionImpl {
  track_fitter_t trackFitter;

  TrackFitterFunctionImpl(track_fitter_t&& f) : trackFitter(std::move(f)) {}

  ActsKalmanFitter::TrackFitterResult operator()(
      const std::vector<ATLASSourceLink>& sourceLinks,
      const Acts::BoundTrackParameters& initialParameters,
      const ActsKalmanFitter::TrackFitterOptions& options)
      const {
    return trackFitter.fit(sourceLinks, initialParameters, options);
  };
};

}  // namespace


ActsKalmanFitter::ActsKalmanFitter(const std::string& t,const std::string& n,
                                const IInterface* p) :
  AthAlgTool (t,n,p),
  m_trkSummaryTool("")
{
  declareInterface<ITrackFitter>( this );
  // Get parameter values from jobOptions file
  declareProperty("SummaryTool" , m_trkSummaryTool, "ToolHandle for track summary tool");  

  // -- job options - 
  declareProperty("OutlierChi2Cut",m_option_outlierChi2Cut=12.5,
		              "Chi2 cut used by the outlier finder");
  declareProperty("ReverseFilteringPt",m_option_ReverseFilteringPt=1.0,
		              "Pt cut used for the ReverseFiltering logic");
  declareProperty("MaxPropagationStep",m_option_maxPropagationStep=5000,
                  "Maximum number of steps for one propagate call");
  declareProperty("SeedCovarianceScale",m_option_seedCovarianceScale=100.,
                  "Scale factor for the input seed covariance when doing refitting");

}

// destructor
ActsKalmanFitter::~ActsKalmanFitter()
{}

StatusCode ActsKalmanFitter::initialize() {

  ATH_MSG_DEBUG(name() << "::" << __FUNCTION__);
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  ATH_CHECK(m_extrapolationTool.retrieve());
  ATH_CHECK(m_ATLASConverterTool.retrieve());
  ATH_CHECK(m_trkSummaryTool.retrieve());

  m_fit = makeTrackFitterFunction(m_trackingGeometryTool->trackingGeometry());
  m_logger = makeActsAthenaLogger(this, "Acts Kalman Refit");
  return StatusCode::SUCCESS;
}

// finalize
StatusCode ActsKalmanFitter::finalize()
{
  ATH_MSG_INFO ("finalize() successful in " << name());
  return StatusCode::SUCCESS;
}

// refit a track
// -------------------------------------------------------
std::unique_ptr<Trk::Track>
ActsKalmanFitter::fit(const EventContext& ctx,
                       const Trk::Track& inputTrack,
                       const Trk::RunOutlierRemoval /*runOutlier*/,
                       const Trk::ParticleHypothesis /*prtHypothesis*/) const
{
  std::unique_ptr<Trk::Track> track = nullptr;
  ATH_MSG_VERBOSE ("--> enter KalmanFitter::fit(Track,,)    with Track from author = "
		   << inputTrack.info().dumpInfo());

  // protection against not having measurements on the input track
  if (!inputTrack.measurementsOnTrack() || inputTrack.measurementsOnTrack()->size() < 2) {
    ATH_MSG_INFO ("called to refit empty track or track with too little information, reject fit");
    return nullptr;
  }

  // protection against not having track parameters on the input track
  if (!inputTrack.trackParameters() || inputTrack.trackParameters()->empty()) {
    ATH_MSG_INFO ("input fails to provide track parameters for seeding the KF, reject fit");
    return nullptr;
  }

  // Construct a perigee surface as the target surface
  auto pSurface = Acts::Surface::makeShared<Acts::PerigeeSurface>(
      Acts::Vector3{0., 0., 0.});
  
  Acts::GeometryContext tgContext = m_trackingGeometryTool->getGeometryContext(ctx).context();
  Acts::MagneticFieldContext mfContext = m_extrapolationTool->getMagneticFieldContext(ctx);
  // CalibrationContext converter not implemented yet.
  Acts::CalibrationContext calContext = Acts::CalibrationContext();

  Acts::PropagatorPlainOptions propagationOption;
  propagationOption.maxSteps = m_option_maxPropagationStep;
  // Set the KalmanFitter options
  Acts::KalmanFitterOptions<ATLASSourceLinkCalibrator, ATLASOutlierFinder, ReverseFilteringLogic>
      kfOptions(tgContext, mfContext, calContext,
                ATLASSourceLinkCalibrator(), ATLASOutlierFinder{m_option_outlierChi2Cut}, ReverseFilteringLogic{m_option_ReverseFilteringPt},
                Acts::LoggerWrapper{logger()}, propagationOption,
                &(*pSurface));
  std::vector<ATLASSourceLink> trackSourceLinks = m_ATLASConverterTool->ATLASTrackToSourceLink(inputTrack);
  // protection against error in the conversion from Atlas masurement to Acts source link
  if (trackSourceLinks.empty()) {
    ATH_MSG_INFO("input contain measurement but no source link created, probable issue with the converter, reject fit ");
    return track;
  }

  const auto& initialParams = m_ATLASConverterTool->ATLASTrackParameterToActs(inputTrack.perigeeParameters());

  // The covariance from already fitted track are too small and would result an incorect smoothing.
  // We scale up the input covaraiance to avoid this.
  Acts::BoundSymMatrix scaledCov = Acts::BoundSymMatrix::Identity();
  for (int i=0; i<6; ++i) {
    double scale = m_option_seedCovarianceScale;
    (scaledCov)(i,i) = scale * initialParams.covariance().value()(i,i);
  }

  const Acts::BoundTrackParameters scaledInitialParams(initialParams.referenceSurface().getSharedPtr(),
                                                       initialParams.parameters(),
                                                       scaledCov);

  // Perform the fit
  auto result = m_fit(trackSourceLinks, scaledInitialParams, kfOptions);
  if (result.ok()) {
    track = makeTrack(ctx, tgContext, result);
  }
  return track;
}

// fit a set of MeasurementBase objects
// --------------------------------
std::unique_ptr<Trk::Track>
ActsKalmanFitter::fit(const EventContext& ctx,
                       const Trk::MeasurementSet& inputMeasSet,
                       const Trk::TrackParameters& estimatedStartParameters,
                       const Trk::RunOutlierRemoval /*runOutlier*/,
                       const Trk::ParticleHypothesis /*matEffects*/) const
{
  std::unique_ptr<Trk::Track> track = nullptr;

  // protection against not having measurements on the input track
  if (inputMeasSet.size() < 2) {
    ATH_MSG_INFO ("called to refit empty measurement set or a measurement set with too little information, reject fit");
    return nullptr;
  }

  // Construct a perigee surface as the target surface
  auto pSurface = Acts::Surface::makeShared<Acts::PerigeeSurface>(
      Acts::Vector3{0., 0., 0.});
  
  Acts::GeometryContext tgContext = m_trackingGeometryTool->getGeometryContext(ctx).context();
  Acts::MagneticFieldContext mfContext = m_extrapolationTool->getMagneticFieldContext(ctx);
  // CalibrationContext converter not implemented yet.
  Acts::CalibrationContext calContext = Acts::CalibrationContext();

  Acts::PropagatorPlainOptions propagationOption;
  propagationOption.maxSteps = m_option_maxPropagationStep;
  // Set the KalmanFitter options
  Acts::KalmanFitterOptions<ATLASSourceLinkCalibrator, ATLASOutlierFinder, ReverseFilteringLogic>
      kfOptions(tgContext, mfContext, calContext,
                ATLASSourceLinkCalibrator(), ATLASOutlierFinder{m_option_outlierChi2Cut}, ReverseFilteringLogic{m_option_ReverseFilteringPt},
                Acts::LoggerWrapper{logger()}, propagationOption,
                &(*pSurface));

  std::vector<ATLASSourceLink> trackSourceLinks;
  trackSourceLinks.reserve(inputMeasSet.size());
  for (auto it = inputMeasSet.begin(); it != inputMeasSet.end(); ++it){
    trackSourceLinks.push_back(m_ATLASConverterTool->ATLASMeasurementToSourceLink(*it));
  }
  // protection against error in the conversion from Atlas masurement to Acts source link
  if (trackSourceLinks.empty()) {
    ATH_MSG_INFO("input contain measurement but no source link created, probable issue with the converter, reject fit ");
    return track;
  }

  const auto& initialParams = m_ATLASConverterTool->ATLASTrackParameterToActs(&estimatedStartParameters); 

  // Perform the fit
  auto result = m_fit(trackSourceLinks, initialParams, kfOptions);
  if (result.ok()) {
    track = makeTrack(ctx, tgContext, result);
  }
  return track;
}

// fit a set of PrepRawData objects
// --------------------------------
std::unique_ptr<Trk::Track>
ActsKalmanFitter::fit(const EventContext& /*ctx*/,
                       const Trk::PrepRawDataSet& /*inputPRDColl*/,
                       const Trk::TrackParameters& /*estimatedStartParameters*/,
                       const Trk::RunOutlierRemoval /*runOutlier*/,
                       const Trk::ParticleHypothesis /*prtHypothesis*/) const
{
  ATH_MSG_INFO ("Fit of PrepRawDataSet not yet implemented");
  return nullptr;
}

// extend a track fit to include an additional set of MeasurementBase objects
// re-implements the TrkFitterUtils/TrackFitter.cxx general code in a more
// mem efficient and stable way
// --------------------------------
std::unique_ptr<Trk::Track>
ActsKalmanFitter::fit(const EventContext& ctx,
                       const Trk::Track& inputTrack,
                       const Trk::MeasurementSet& addMeasColl,
                       const Trk::RunOutlierRemoval /*runOutlier*/,
                       const Trk::ParticleHypothesis /*matEffects*/) const
{
  ATH_MSG_VERBOSE ("--> enter KalmanFitter::fit(Track,Meas'BaseSet,,)");
  ATH_MSG_VERBOSE ("    with Track from author = " << inputTrack.info().dumpInfo());

  // protection, if empty MeasurementSet
  if (addMeasColl.empty()) {
    ATH_MSG_WARNING( "client tries to add an empty MeasurementSet to the track fit." );
    return fit(ctx,inputTrack);
  }

  // protection against not having measurements on the input track
  if (!inputTrack.measurementsOnTrack() || (inputTrack.measurementsOnTrack()->size() < 2 && addMeasColl.empty())) {
    ATH_MSG_INFO ("called to refit empty track or track with too little information, reject fit");
    return nullptr;
  }

  // protection against not having track parameters on the input track
  if (!inputTrack.trackParameters() || inputTrack.trackParameters()->empty()) {
    ATH_MSG_INFO ("input fails to provide track parameters for seeding the KF, reject fit");
    return nullptr;
  }

   std::unique_ptr<Trk::Track> track = nullptr;

  // Construct a perigee surface as the target surface
  auto pSurface = Acts::Surface::makeShared<Acts::PerigeeSurface>(
      Acts::Vector3{0., 0., 0.});
  
  Acts::GeometryContext tgContext = m_trackingGeometryTool->getGeometryContext(ctx).context();
  Acts::MagneticFieldContext mfContext = m_extrapolationTool->getMagneticFieldContext(ctx);
  // CalibrationContext converter not implemented yet.
  Acts::CalibrationContext calContext = Acts::CalibrationContext();

  Acts::PropagatorPlainOptions propagationOption;
  propagationOption.maxSteps = m_option_maxPropagationStep;
  // Set the KalmanFitter options
  Acts::KalmanFitterOptions<ATLASSourceLinkCalibrator, ATLASOutlierFinder, ReverseFilteringLogic>
      kfOptions(tgContext, mfContext, calContext,
                ATLASSourceLinkCalibrator(), ATLASOutlierFinder{m_option_outlierChi2Cut}, ReverseFilteringLogic{m_option_ReverseFilteringPt},
                Acts::LoggerWrapper{logger()}, propagationOption,
                &(*pSurface));

  std::vector<ATLASSourceLink> trackSourceLinks = m_ATLASConverterTool->ATLASTrackToSourceLink(inputTrack);
  const auto& initialParams = m_ATLASConverterTool->ATLASTrackParameterToActs(inputTrack.perigeeParameters());
  trackSourceLinks.resize(trackSourceLinks.size() + addMeasColl.size());
  for (auto it = addMeasColl.begin(); it != addMeasColl.end(); ++it)
  {
    trackSourceLinks.push_back(m_ATLASConverterTool->ATLASMeasurementToSourceLink(*it));
  }
  // protection against error in the conversion from Atlas masurement to Acts source link
  if (trackSourceLinks.empty()) {
    ATH_MSG_INFO("input contain measurement but no source link created, probable issue with the converter, reject fit ");
    return track;
  }

  // Perform the fit
  auto result = m_fit(trackSourceLinks, initialParams, kfOptions);
  if (result.ok()) {
    track = makeTrack(ctx, tgContext, result);
  }
  return track;
}

// extend a track fit to include an additional set of PrepRawData objects
// --------------------------------
std::unique_ptr<Trk::Track>
ActsKalmanFitter::fit(const EventContext& /*ctx*/,
                       const Trk::Track& /*inputTrack*/,
                       const Trk::PrepRawDataSet& /*addPrdColl*/,
                       const Trk::RunOutlierRemoval /*runOutlier*/,
                       const Trk::ParticleHypothesis /*matEffects*/) const
{

  ATH_MSG_INFO ("Fit of Track with additional PrepRawDataSet not yet implemented");
  return nullptr;
}

// combined fit of two tracks
// --------------------------------
std::unique_ptr<Trk::Track>
ActsKalmanFitter::fit(const EventContext& ctx,
                       const Trk::Track& intrk1,
                       const Trk::Track& intrk2,
                       const Trk::RunOutlierRemoval /*runOutlier*/,
                       const Trk::ParticleHypothesis /*matEffects*/) const
{
  ATH_MSG_VERBOSE ("--> enter KalmanFitter::fit(Track,Track,)");
  ATH_MSG_VERBOSE ("    with Tracks from #1 = " << intrk1.info().dumpInfo()
                   << " and #2 = " << intrk2.info().dumpInfo());

  // protection, if empty track2
  if (!intrk2.measurementsOnTrack()) {
    ATH_MSG_WARNING( "input #2 is empty try to fit track 1 alone" );
    return fit(ctx,intrk1);
  }

  // protection, if empty track1
  if (!intrk1.measurementsOnTrack()) {
    ATH_MSG_WARNING( "input #1 is empty try to fit track 2 alone" );
    return fit(ctx,intrk2);
  }

  // protection against not having track parameters on the input track
  if (!intrk1.trackParameters() || intrk1.trackParameters()->empty()) {
    ATH_MSG_INFO ("input #1 fails to provide track parameters for seeding the KF, reject fit");
    return nullptr;
  }

   std::unique_ptr<Trk::Track> track = nullptr;

  // Construct a perigee surface as the target surface
  auto pSurface = Acts::Surface::makeShared<Acts::PerigeeSurface>(
      Acts::Vector3{0., 0., 0.});
  
  Acts::GeometryContext tgContext = m_trackingGeometryTool->getGeometryContext(ctx).context();
  Acts::MagneticFieldContext mfContext = m_extrapolationTool->getMagneticFieldContext(ctx);
  // CalibrationContext converter not implemented yet.
  Acts::CalibrationContext calContext = Acts::CalibrationContext();

  Acts::PropagatorPlainOptions propagationOption;
  propagationOption.maxSteps = m_option_maxPropagationStep;
  // Set the KalmanFitter options
  Acts::KalmanFitterOptions<ATLASSourceLinkCalibrator, ATLASOutlierFinder, ReverseFilteringLogic>
      kfOptions(tgContext, mfContext, calContext,
                ATLASSourceLinkCalibrator(), ATLASOutlierFinder{m_option_outlierChi2Cut}, ReverseFilteringLogic{m_option_ReverseFilteringPt},
                Acts::LoggerWrapper{logger()}, propagationOption,
                &(*pSurface));

  std::vector<ATLASSourceLink> trackSourceLinks = m_ATLASConverterTool->ATLASTrackToSourceLink(intrk1);
  std::vector<ATLASSourceLink> trackSourceLinks2 = m_ATLASConverterTool->ATLASTrackToSourceLink(intrk2);
  trackSourceLinks.insert(trackSourceLinks.end(), trackSourceLinks2.begin(), trackSourceLinks2.end());
  // protection against error in the conversion from Atlas masurement to Acts source link
  if (trackSourceLinks.empty()) {
    ATH_MSG_INFO("input contain measurement but no source link created, probable issue with the converter, reject fit ");
    return track;
  }

  const auto &initialParams = m_ATLASConverterTool->ATLASTrackParameterToActs(intrk1.perigeeParameters());

  // The covariance from already fitted track are too small and would result an incorect smoothing.
  // We scale up the input covaraiance to avoid this.
  Acts::BoundSymMatrix scaledCov = Acts::BoundSymMatrix::Identity();
  for (int i=0; i<6; ++i) {
    double scale = m_option_seedCovarianceScale;
    (scaledCov)(i,i) = scale * initialParams.covariance().value()(i,i);
  }

  const Acts::BoundTrackParameters scaledInitialParams(initialParams.referenceSurface().getSharedPtr(),
                                                       initialParams.parameters(),
                                                       scaledCov);

  // Perform the fit
  auto result = m_fit(trackSourceLinks, scaledInitialParams, kfOptions);
  if (result.ok()) {
    track = makeTrack(ctx, tgContext, result);
  }
  return track;
}


std::unique_ptr<Trk::Track>
ActsKalmanFitter::makeTrack(const EventContext& ctx, Acts::GeometryContext& tgContext, TrackFitterResult& fitResult) const {
  std::unique_ptr<Trk::Track> newtrack = nullptr;
  // Get the fit output object
  const auto& fitOutput = fitResult.value();
  if (fitOutput.fittedParameters) {
    auto finalTrajectory = DataVector<const Trk::TrackStateOnSurface>();
    // initialise the number of dead Pixel and Acts strip
    int numberOfDeadPixel = 0;
    int numberOfDeadSCT = 0;

    std::vector<std::unique_ptr<const Acts::BoundTrackParameters>> actsSmoothedParam;
    // Loop over all the output state to create track state
    fitOutput.fittedStates.visitBackwards(fitOutput.lastMeasurementIndex, [&](const auto &state) {
      // First only concider state with an associated detector element not in the TRT
      auto flag = state.typeFlags();
      if (state.referenceSurface().associatedDetectorElement() != nullptr) {
        const auto* actsElement = dynamic_cast<const ActsDetectorElement*>(state.referenceSurface().associatedDetectorElement());
        if (actsElement != nullptr 
            && dynamic_cast<const InDetDD::TRT_BaseElement*>(actsElement->upstreamDetectorElement()) == nullptr) {
          const auto* detElem = dynamic_cast<const InDetDD::SiDetectorElement*>(actsElement->upstreamDetectorElement());
          // We need to determine the type of state 
          std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
          const Trk::TrackParameters *parm;

          // State is a hole (no associated measurement), use predicted parameters      
          if (flag[Acts::TrackStateFlag::HoleFlag] == true){         
            const Acts::BoundTrackParameters actsParam(state.referenceSurface().getSharedPtr(),
                                                       state.predicted(),
                                                       state.predictedCovariance());
            parm = m_ATLASConverterTool->ActsTrackParameterToATLAS(actsParam, tgContext);
            auto boundaryCheck = m_boundaryCheckTool->boundaryCheck(*parm);
            
            // Check if this is a hole, a dead sensors or a state outside the sensor boundary
            if(boundaryCheck == Trk::BoundaryCheckResult::DeadElement){
              if (detElem->isPixel()) {
                ++numberOfDeadPixel;
              }
              else if (detElem->isSCT()) {
                ++numberOfDeadSCT;
              }
              // Dead sensors states are not stored              
              return;
            } else if (boundaryCheck != Trk::BoundaryCheckResult::Candidate){
                // States outside the sensor boundary are ignored
                return;
              }
            typePattern.set(Trk::TrackStateOnSurface::Hole);
          }
          // The state was tagged as an outlier, use filtered parameters
          else if (flag[Acts::TrackStateFlag::OutlierFlag] == true){
            const Acts::BoundTrackParameters actsParam(state.referenceSurface().getSharedPtr(),
                                                       state.filtered(),
                                                       state.filteredCovariance());
            parm = m_ATLASConverterTool->ActsTrackParameterToATLAS(actsParam, tgContext);
            typePattern.set(Trk::TrackStateOnSurface::Outlier);
          }
          // The state is a measurement state, use smoothed parameters 
          else{
            const Acts::BoundTrackParameters actsParam(state.referenceSurface().getSharedPtr(),
                                                       state.smoothed(),
                                                       state.smoothedCovariance());

            actsSmoothedParam.push_back(std::make_unique<const Acts::BoundTrackParameters>(Acts::BoundTrackParameters(actsParam)));
            parm = m_ATLASConverterTool->ActsTrackParameterToATLAS(actsParam, tgContext);
            typePattern.set(Trk::TrackStateOnSurface::Measurement);                                           
          }
          const Trk::MeasurementBase *measState = nullptr;
          if (state.hasUncalibrated()){
            measState = state.uncalibrated().atlasHit().clone();
          }
          double nDoF = state.calibratedSize();
          const Trk::FitQualityOnSurface *quality = new Trk::FitQualityOnSurface(state.chi2(), nDoF);
          const Trk::TrackStateOnSurface *perState = new Trk::TrackStateOnSurface(measState, parm, quality, nullptr, typePattern);
          // If a state was succesfully created add it to the trajectory 
          if (perState) {
            finalTrajectory.insert(finalTrajectory.begin(), perState);
          }
        }
      }
    return;
    });

    // Convert the perigee state and add it to the trajectory
    const Acts::BoundTrackParameters actsPer = fitOutput.fittedParameters.value();
    const Trk::TrackParameters *per = m_ATLASConverterTool->ActsTrackParameterToATLAS(actsPer, tgContext);
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Perigee);
    const Trk::TrackStateOnSurface *perState = new Trk::TrackStateOnSurface(nullptr, per, nullptr, nullptr, typePattern);
    if (perState) finalTrajectory.insert(finalTrajectory.begin(), perState);

    // Create the track using the states
    Trk::TrackInfo newInfo(Trk::TrackInfo::TrackFitter::KalmanFitter, Trk::noHypothesis);
    newInfo.setTrackFitter(Trk::TrackInfo::TrackFitter::KalmanFitter); //Mark the fitter as KalmanFitter
    newtrack = std::make_unique<Trk::Track>(newInfo, std::move(finalTrajectory), nullptr);
    if (newtrack) {
      // Create the track summary and update the holes information
      auto holeSurfaces = fitOutput.missedActiveSurfaces;
      if (!newtrack->trackSummary()) {
        newtrack->setTrackSummary(std::make_unique<Trk::TrackSummary>());
        newtrack->trackSummary()->update(Trk::numberOfPixelHoles, 0);
        newtrack->trackSummary()->update(Trk::numberOfSCTHoles, 0);
        newtrack->trackSummary()->update(Trk::numberOfTRTHoles, 0);
        newtrack->trackSummary()->update(Trk::numberOfPixelDeadSensors, numberOfDeadPixel);
        newtrack->trackSummary()->update(Trk::numberOfSCTDeadSensors, numberOfDeadSCT);
      }
      m_trkSummaryTool->updateTrackSummary(ctx, *newtrack, nullptr, true);
    }
  }
  return newtrack;
}

ActsKalmanFitter::TrackFitterFunction
ActsKalmanFitter::makeTrackFitterFunction(
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry) {
  using Updater = Acts::GainMatrixUpdater;
  using Smoother = Acts::GainMatrixSmoother;

  // unpack the magnetic field variant and instantiate the corresponding fitter.
  return std::visit(
      [trackingGeometry]() -> TrackFitterFunction {
        // each entry in the variant is already a shared_ptr
        // need ::element_type to get the real magnetic field type
        using Stepper = Acts::EigenStepper<>;
        using Navigator = Acts::Navigator;
        using Propagator = Acts::Propagator<Stepper, Navigator>;
        using Fitter = Acts::KalmanFitter<Propagator, Updater, Smoother>;

        // construct all components for the fitter
        auto field = std::make_shared<ATLASMagneticFieldWrapper>();;
        Stepper stepper(field);
        Acts::Navigator navigator( Acts::Navigator::Config{ trackingGeometry } );     
        Propagator propagator(std::move(stepper), std::move(navigator));
        Fitter trackFitter(std::move(propagator));

        // build the fitter functions. owns the fitter object.
        return TrackFitterFunctionImpl<Fitter>(std::move(trackFitter));
      });
}