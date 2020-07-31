/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/***************************************************************************
   least squared fit to track hit data => PerigeeParameters with covariance
   and fit quality.
***************************************************************************/

#include <cmath>
#include <iomanip>
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/AlignmentEffectsOnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkiPatFitterUtils/ExtrapolationType.h"
#include "TrkiPatFitterUtils/FitProcedureQuality.h"
#include "TrkiPatFitter/iPatFitter.h"

namespace Trk
{
  iPatFitter::iPatFitter (const std::string& type,
                          const std::string& name,
                          const IInterface* parent,
                          bool globalFit)
    :   AthAlgTool(type, name, parent),
    m_globalFit(globalFit),
    m_materialAllocator("Trk::MaterialAllocator/MaterialAllocator", this),
    m_rungeKuttaIntersector("Trk::RungeKuttaIntersector/RungeKuttaIntersector", this),
    m_solenoidalIntersector("Trk::SolenoidalIntersector/SolenoidalIntersector", this),
    m_stepPropagator("Trk::STEP_Propagator/AtlasSTEP_Propagator", this),
    m_straightLineIntersector("Trk::StraightLineIntersector/StraightLineIntersector", this),
    m_trackingVolumesSvc("TrackingVolumesSvc/TrackingVolumesSvc", name),
    m_trackSummaryTool("MuonTrackSummaryTool"),
    m_stepField(Trk::MagneticFieldProperties(Trk::FullField)) {
    m_messageHelper = std::make_unique<MessageHelper>(*this);
    declareInterface<ITrackFitter>(this);
    declareProperty("MaterialAllocator", m_materialAllocator);
    declareProperty("RungeKuttaIntersector", m_rungeKuttaIntersector);
    declareProperty("SolenoidalIntersector", m_solenoidalIntersector);
    declareProperty("StraightLineIntersector", m_straightLineIntersector);
    declareProperty("TrackingVolumesSvc", m_trackingVolumesSvc);
    declareProperty("TrackSummaryTool", m_trackSummaryTool );
  }

  StatusCode
  iPatFitter::initialize() {
    // print name and package version
    if (!msgLvl(MSG::DEBUG)) { m_extendedDebug = false; }
    ATH_MSG_INFO("iPatFitter::initialize() - package version " << PACKAGE_VERSION);
    msg(MSG::INFO) << " with options: ";
    if (m_aggregateMaterial) { msg() << " AggregateMaterial"; }
    if (m_asymmetricCaloEnergy) { msg() << " AsymmetricCaloEnergy"; }
    if (m_fullCombinedFit) { msg() << " FullCombinedFit"; }
    if (m_globalFit) { msg() << " GlobalFitter";
    if (m_lineFit) { msg() << " LineFit with p = "
                           << m_lineMomentum / Gaudi::Units::GeV << " GeV"; }
    if (m_constrainedAlignmentEffects) msg() << " ConstrainedAlignmentEffects"; }
    if (m_extendedDebug) { msg() << " ExtendedDebug"; }
    if (m_forcedRefitsForValidation) { msg() << " ForcedRefitsForValidation = "
                                             << m_forcedRefitsForValidation; }
    msg() << endmsg;

    // fill WARNING messages
    m_messageHelper->setMaxNumberOfMessagesPrinted(m_maxWarnings);
    m_messageHelper->setNumberOfMessages(26);
    m_messageHelper->setMessage(0, "fit (Track): outlier removal not implemented");
    m_messageHelper->setMessage(1, "fit (Track): track without perigee");
    m_messageHelper->setMessage(2, "fit (Track): track without trackStateOnSurfaces");
    m_messageHelper->setMessage(3, "fit (Track + PrepRawDataSet): interface not implemented");
    m_messageHelper->setMessage(4, "fit (PrepRawDataSet): interface not implemented");
    m_messageHelper->setMessage(5, "fit (Track + MeasurementSet): outlier removal not implemented");
    m_messageHelper->setMessage(6, "fit (Track + MeasurementSet): track without trackStateOnSurfaces");
    m_messageHelper->setMessage(7, "fit (Track + MeasurementSet): track without measuredPerigee");
    m_messageHelper->setMessage(8, "fit (Track + MeasurementSet): FIX material may get double counted");
    m_messageHelper->setMessage(9, "fit (Perigee + MeasurementSet): outlier removal not implemented");
    m_messageHelper->setMessage(10, "fit (Perigee + MeasurementSet): null perigee");
    m_messageHelper->setMessage(11, "fit (combined muon): outlier removal not implemented");
    m_messageHelper->setMessage(12, "fit (combined muon): no perigee start value");
    m_messageHelper->setMessage(13, "fit (combined muon): indet track without trackStateOnSurfaces");
    m_messageHelper->setMessage(14, "fit (combined muon): indet track without measuredPerigee");
    m_messageHelper->setMessage(15, "addMeasurements: no intersection to MeasurementSet");
    m_messageHelper->setMessage(16, "addMeasurements: skip TSOS as not understood. Type: ");
    m_messageHelper->setMessage(17, "addMeasurements: skip TSOS with missing trackParameters. Type: ");
    m_messageHelper->setMessage(18,
                                "addMeasurements: skip measurement as fail to intersect associated surface from given starting parameters");
    m_messageHelper->setMessage(19, "addMeasurements: TSOS skipped. Type: ");
    m_messageHelper->setMessage(20, "fail fit as CaloDeposit outside calo volume");
    m_messageHelper->setMessage(21, "conflicting energy deposit sign for inDet material");
    m_messageHelper->setMessage(22, "conflicting energy deposit sign for spectrometer material");
    m_messageHelper->setMessage(23, "excessive calorimeter energy loss : ");
    m_messageHelper->setMessage(24, "excessive spectrometer energy loss : ");
    m_messageHelper->setMessage(25, "flipped track measurement order");

    // retrieve the MaterialProvider and various Intersectors
    auto retrieveTool = [&](auto handle) {
      if (handle.retrieve().isFailure()) {
        ATH_MSG_FATAL("Failed to retrieve tool " << handle);
        return false;
      } 
        ATH_MSG_INFO("Retrieved tool " << handle);
        return true;
      
    };
    if (!retrieveTool(m_materialAllocator)) { return StatusCode::FAILURE; }        
    if (!retrieveTool(m_rungeKuttaIntersector)) { return StatusCode::FAILURE; }
    if (!retrieveTool(m_solenoidalIntersector)) { return StatusCode::FAILURE; }
    if (!retrieveTool(m_stepPropagator)) { return StatusCode::FAILURE; };
    if (!retrieveTool(m_straightLineIntersector)) { return StatusCode::FAILURE; }

    // need to create the IndetExit and MuonEntrance TrackingVolumes
    if (m_trackingVolumesSvc.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve Svc " << m_trackingVolumesSvc);
      return StatusCode::FAILURE;
    } 
      ATH_MSG_INFO("Retrieved Svc " << m_trackingVolumesSvc);
      m_calorimeterVolume = std::make_unique<Trk::Volume>(
        m_trackingVolumesSvc->volume(ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
      m_indetVolume = std::make_unique<Volume>(
        m_trackingVolumesSvc->volume(ITrackingVolumesSvc::CalorimeterEntryLayer));
    

    ATH_CHECK(m_trackSummaryTool.retrieve());

    // can now create FitProcedure class
    m_fitProcedure = std::make_unique<FitProcedure>(
      m_constrainedAlignmentEffects,
      m_extendedDebug,
      m_lineFit,
      m_rungeKuttaIntersector,
      m_solenoidalIntersector,
      m_straightLineIntersector,
      m_stepPropagator,
      m_indetVolume.get(),
      m_maxIterations,
      1); // useStepPropagator
    // useStepPropagator 0 means not used (so Intersector used)
    // 1 Intersector not used and StepPropagator used with FullField
    // 2 StepPropagator with FastField propagation
    // 99 debug mode where both are ran with FullField
    return StatusCode::SUCCESS;
  }

  StatusCode
  iPatFitter::finalize() {
    // print summary statistics
    double fits = static_cast<double>(m_countFitAttempts);
    double goodFit = 0.;
    double iterations = 0.;

    if (m_countFitAttempts) { goodFit = static_cast<double>(100 * m_countGoodFits) / fits; }
    if (m_countGoodFits) { iterations = static_cast<double>(m_countIterations) /
                                        static_cast<double>(m_countGoodFits); }
    ATH_MSG_INFO(std::setiosflags(std::ios::fixed)
                 << "finalized after " << m_countFitAttempts
                 << " track-fits attempted, out of which "
                 << std::setw(5) << std::setprecision(1) << goodFit
                 << "% converged, taking an average "
                 << std::setw(5) << std::setprecision(2) << iterations
                 << " iterations");
    if (m_forcedRefitsForValidation) {
      double refits = static_cast<double>(m_countRefitAttempts);
      double goodRefit = 0.;
      double refitIterations = 0.;
      if (m_countRefitAttempts) { goodRefit = static_cast<double>(100 * m_countGoodRefits) / refits; }
      if (m_countGoodRefits) { refitIterations = static_cast<double>(m_countRefitIterations) /
                                                 static_cast<double>(m_countGoodRefits); }
      ATH_MSG_INFO(std::setiosflags(std::ios::fixed)
                   << "finalized after " << m_countRefitAttempts
                   << "     refits attempted, out of which "
                   << std::setw(5) << std::setprecision(1) << goodRefit
                   << "% converged, taking an average "
                   << std::setw(5) << std::setprecision(2) << refitIterations
                   << " iterations");
    }

    m_messageHelper->printSummary();

    return StatusCode::SUCCESS;
  }

  auto
  iPatFitter::fitWithState(const EventContext& ctx,
                           const Track& track,
                           const RunOutlierRemoval runOutlier,
                           const ParticleHypothesis particleHypothesis) const
    -> std::pair<std::unique_ptr<Track>, std::unique_ptr<FitState>>
  {
    ATH_MSG_VERBOSE(" track fit ");
    auto fitState = std::make_unique<FitState>();

    m_countFitAttempts++;
    // outlier removal not implemented
    if (runOutlier) { m_messageHelper->printWarning(0); } // TODO Make thread-safe

    // create Perigee if starting parameters are for a different surface type
    const Perigee* perigeeParameters = track.perigeeParameters(); 
    // Note: we don't own the Perigee from perigeeParameters(), but if it returns nullptr, we have to make our own, 
    // and we need to delete it, so it's put in this unique_ptr
    std::unique_ptr<Perigee> newPerigee;
    std::unique_ptr<PerigeeSurface> perigeeSurface;

    if (!perigeeParameters) {
      auto i = track.trackStateOnSurfaces()->begin();
      while (i != track.trackStateOnSurfaces()->end() && !(**i).trackParameters()) { i++; }
      const TrackStateOnSurface& s = (**i);
      if (!s.trackParameters()) {
        // input track without parameters
        m_messageHelper->printWarning(1);
        return {nullptr, std::move(fitState)};
      }

      Amg::Vector3D origin(s.trackParameters()->position());
      perigeeSurface = std::make_unique<PerigeeSurface>(origin);
      newPerigee = std::make_unique<Perigee>(
        s.trackParameters()->position(),
        s.trackParameters()->momentum(),
        s.trackParameters()->charge(),
        *perigeeSurface);
    }

    const Perigee& perigee = newPerigee ? *newPerigee : *perigeeParameters; // Use the valid Perigee

    fitState->parameters = std::make_unique<FitParameters>(perigee);

    // set up the measurements
    if (!track.trackStateOnSurfaces()) {
      // input track without trackStateOnSurfaces
      m_messageHelper->printWarning(2); // TODO Make thread-safe
      return {nullptr, std::move(fitState)};
    }

    fitState->newMeasurements();

    bool haveMaterial = addMeasurements(ctx,
                                        fitState->getMeasurements(),
                                        *fitState->parameters,
                                        particleHypothesis,
                                        *track.trackStateOnSurfaces());

    // allocate material
    Garbage_t garbage;
    if (!haveMaterial && particleHypothesis != Trk::nonInteracting) {
      m_materialAllocator->allocateMaterial(fitState->getMeasurements(),
                                            particleHypothesis,
                                            *fitState->parameters,
                                            perigee,
                                            garbage);
    }

    // perform fit and return fitted track
    TrackInfo trackInfo(TrackInfo::iPatTrackFitter, particleHypothesis);
    trackInfo.addPatternReco(track.info());
    std::unique_ptr<Trk::Track> fittedTrack { performFit(*fitState,
                                                         particleHypothesis,
                                                         trackInfo,
                                                         nullptr,
                                                         nullptr,
                                                         garbage) };

    // validation
    for (int i = 0; i < m_forcedRefitsForValidation; ++i) {
      if (fittedTrack) { refit(ctx,*fitState, *fittedTrack, runOutlier, particleHypothesis); }
    }

    return {std::move(fittedTrack), std::move(fitState)};
  }

  std::unique_ptr<Track> 
  iPatFitter::fit(const EventContext& ctx, 
                  const Track& track,
                  const RunOutlierRemoval runOutlier,
                  const ParticleHypothesis particleHypothesis) const {
    auto [fittedTrack, fitState] = fitWithState(ctx,track, runOutlier, particleHypothesis);
    return std::move(fittedTrack);
  }

  std::unique_ptr<Track>
  iPatFitter::fit(const EventContext&,
                  const Track& /*track*/,
                  const PrepRawDataSet& /*prepRawDataSet*/,
                  const RunOutlierRemoval /*trackrunOutlier*/,
                  const ParticleHypothesis /*trackparticleHypothesis*/) const
  {
    m_countFitAttempts++;
    // track + PrepRawDataSet interface not implemented
    m_messageHelper->printWarning(3);
    return nullptr;
  }

  std::unique_ptr<Track>
  iPatFitter::fit(const EventContext&,
                  const PrepRawDataSet& /*prepRawDataSet*/,
                  const TrackParameters& /*estimatedParametersNearOrigin*/,
                  const RunOutlierRemoval /*trackrunOutlier*/,
                  const ParticleHypothesis /*trackparticleHypothesis*/) const
  {
    m_countFitAttempts++;
    // PrepRawDataSet interface not implemented
    m_messageHelper->printWarning(4);
    return nullptr;
  }

  std::unique_ptr<Track>
  iPatFitter::fit(const EventContext& ctx,
                  const Track& track,
                  const MeasurementSet& measurementSet,
                  const RunOutlierRemoval runOutlier,
                  const ParticleHypothesis particleHypothesis) const
  {
    ATH_MSG_VERBOSE(" track + measurementSet fit ");
    m_countFitAttempts++;
    // outlier removal not implemented
    if (runOutlier) { m_messageHelper->printWarning(5); }

    // create starting parameters
    if (!track.trackStateOnSurfaces()) {
      // track without trackStateOnSurfaces
      m_messageHelper->printWarning(6);
      return nullptr;
    }
    const Perigee* perigee = dynamic_cast<const Perigee*>
                             ((**track.trackStateOnSurfaces()->begin()).trackParameters());
    if (!perigee) {
      // track without measuredPerigee
      m_messageHelper->printWarning(7);
      return nullptr;
    }

    FitState fitState;
    fitState.parameters = std::make_unique<FitParameters>(*perigee);

    // set up the measurements (and material)
    fitState.newMeasurements();
    if (addMeasurements(ctx,
                        fitState.getMeasurements(),
                        *fitState.parameters,
                        particleHypothesis,
                        *track.trackStateOnSurfaces()))
      m_messageHelper->printWarning(8); // FIX needed: material may
                                        // get double counted
    addMeasurements(
      ctx, fitState.getMeasurements(), measurementSet, *fitState.parameters);
    Garbage_t garbage;
    if (particleHypothesis != Trk::nonInteracting) {
      const TrackParameters& endParams = *(track.trackStateOnSurfaces()->back()->trackParameters());
      m_materialAllocator->allocateMaterial(fitState.getMeasurements(),
                                            particleHypothesis,
                                            *fitState.parameters,
                                            endParams,
                                            garbage);
    }

    // perform fit and return fitted track
    TrackInfo trackInfo(TrackInfo::iPatTrackFitter, particleHypothesis);
    trackInfo.addPatternReco(track.info());
    return performFit(fitState,
                      particleHypothesis,
                      trackInfo,
                      track.trackStateOnSurfaces(),
                      track.fitQuality(),
                      garbage);
  }

  std::unique_ptr<Trk::Track>
  iPatFitter::fit(const EventContext& ctx,
                  const MeasurementSet& measurementSet,
                  const TrackParameters& perigeeStartValue,
                  const RunOutlierRemoval runOutlier,
                  const ParticleHypothesis particleHypothesis) const
  {
    ATH_MSG_VERBOSE(" fit from measurement set + perigeeStartValue ");
    m_countFitAttempts++;
    // outlier removal not implemented
    if (runOutlier) { m_messageHelper->printWarning(9); }

    const Perigee* perigee = dynamic_cast<const Perigee*>(&perigeeStartValue);
    if (!perigee) {
      // track without Perigee start value
      m_messageHelper->printWarning(10);
      return nullptr;
    }

    FitState fitState;
    fitState.parameters = std::make_unique<FitParameters>(*perigee);

    // set up the measurements (and material)
    fitState.newMeasurements();
    addMeasurements(ctx,fitState.getMeasurements(), measurementSet, *fitState.parameters);
    Garbage_t garbage;
    if (particleHypothesis != Trk::nonInteracting) {
      m_materialAllocator->allocateMaterial(fitState.getMeasurements(),
                                            particleHypothesis,
                                            *fitState.parameters,
                                            perigeeStartValue,
                                            garbage);
    }

    // perform fit and return fitted track
    TrackInfo trackInfo(TrackInfo::iPatTrackFitter, particleHypothesis);
    return performFit(fitState, particleHypothesis, trackInfo, nullptr, nullptr, garbage);
  }

  std::unique_ptr<Track>
  iPatFitter::fit(const EventContext& ctx,
                  const Track& indetTrack,
                  const Track& spectrometerTrack,
                  const RunOutlierRemoval runOutlier,
                  const ParticleHypothesis particleHypothesis) const
  {
    ATH_MSG_VERBOSE(" combined muon fit ");
    m_countFitAttempts++;
    // outlier removal not implemented
    if (runOutlier) { m_messageHelper->printWarning(11); }

    // indet (full refit to measurements or use measured perigee)
    bool haveMaterial = true;
    FitState fitState;
    fitState.newMeasurements();
    if (indetTrack.perigeeParameters()) {
      fitState.parameters = std::make_unique<FitParameters>(*indetTrack.perigeeParameters());
    } else if (spectrometerTrack.perigeeParameters()
               && m_indetVolume->inside(spectrometerTrack.perigeeParameters()->associatedSurface().center())) {
      fitState.parameters = std::make_unique<FitParameters>(*spectrometerTrack.perigeeParameters());
    } else {
      // combined muon fit without Perigee start value"
      m_messageHelper->printWarning(12);
      return nullptr;
    }

    // get both perigee parameters, use most precise for momentum starting value
    const Perigee* indetPerigee = dynamic_cast<const Perigee*>(indetTrack.perigeeParameters());
    const Perigee* spectrometerPerigee = dynamic_cast<const Perigee*>(spectrometerTrack.perigeeParameters());
    if (spectrometerPerigee
        && !m_indetVolume->inside(spectrometerPerigee->associatedSurface().center())) { spectrometerPerigee = nullptr; }
    if (!spectrometerTrack.info().trackProperties(Trk::TrackInfo::StraightTrack)
        && spectrometerPerigee) {
      if (indetTrack.info().trackProperties(Trk::TrackInfo::StraightTrack)
          || !indetPerigee
          || !indetPerigee->covariance()) {
        fitState.parameters->qOverP(spectrometerPerigee->parameters()[Trk::qOverP]);
        ATH_MSG_VERBOSE(" set starting momentum from spectrometer "
                        << 1. / (fitState.parameters->qOverP() * Gaudi::Units::GeV) << " GeV");
      } else if (indetPerigee) {
        if (spectrometerPerigee->covariance()
            && (*spectrometerPerigee->covariance())(Trk::qOverP, Trk::qOverP) <
            (*indetPerigee->covariance())(Trk::qOverP, Trk::qOverP)) {
          fitState.parameters->qOverP(spectrometerPerigee->parameters()[Trk::qOverP]);
          ATH_MSG_VERBOSE(" set starting momentum from spectrometer "
                          << 1. / (fitState.parameters->qOverP() * Gaudi::Units::GeV) << " GeV");
        }
      }
    }

    if (m_fullCombinedFit) {
      // set up the measurements
      if (!indetTrack.trackStateOnSurfaces()) {
        // fail as indet track without trackStateOnSurfaces
        m_messageHelper->printWarning(13);
        return nullptr;
      }
      if (!addMeasurements(ctx,
                           fitState.getMeasurements(),
                           *fitState.parameters,
                           particleHypothesis,
                           *indetTrack.trackStateOnSurfaces())) {
        haveMaterial = false;
      }
    }

    // add the spectrometer measurements
    if (!addMeasurements(ctx,
                         fitState.getMeasurements(),
                         *fitState.parameters,
                         particleHypothesis,
                         *spectrometerTrack.trackStateOnSurfaces())) {
      haveMaterial = false;
    }
    Garbage_t garbage;
    if (!haveMaterial && particleHypothesis != Trk::nonInteracting) {
      Perigee* startingPerigee = fitState.parameters->startingPerigee();
      if (startingPerigee) {
        m_materialAllocator->allocateMaterial(fitState.getMeasurements(),
                                              particleHypothesis,
                                              *fitState.parameters,
                                              *startingPerigee,
                                              garbage);
        delete startingPerigee;
      }
    }

    // create starting parameters, perform fit and return fitted track
    TrackInfo trackInfo(TrackInfo::iPatTrackFitter, particleHypothesis);
    trackInfo.addPatternReco(indetTrack.info());
    trackInfo.addPatternReco(spectrometerTrack.info());
    if (m_fullCombinedFit) {
      std::unique_ptr<Trk::Track> fittedTrack = performFit(
        fitState, particleHypothesis, trackInfo, nullptr, nullptr, garbage);

      // validation
      for (int i = 0; i < m_forcedRefitsForValidation; ++i) {
        if (fittedTrack) {
          refit(ctx, fitState, *fittedTrack, runOutlier, particleHypothesis);
        }
      }

      return fittedTrack;
    } // hybrid fit
      if (!indetPerigee) {
        // fail combined muon fit as indet track without measuredPerigee
        m_messageHelper->printWarning(14);
        return nullptr;
      }
      fitState.getMeasurements().insert(fitState.getMeasurements().begin(), new FitMeasurement(*indetPerigee));
      FitParameters measuredParameters(*indetPerigee);
      std::unique_ptr<Trk::Track> fittedTrack =
        performFit(fitState,
                   particleHypothesis,
                   trackInfo,
                   indetTrack.trackStateOnSurfaces(),
                   indetTrack.fitQuality(),
                   garbage);

      // validation
      for (int i = 0; i < m_forcedRefitsForValidation; ++i) {
        if (fittedTrack) {
          refit(ctx, fitState, *fittedTrack, runOutlier, particleHypothesis);
        }
      }

      return fittedTrack;
    
  }

  void
  iPatFitter::addMeasurements(const EventContext& ctx,
                              std::vector<FitMeasurement*>& measurements,
                              const MeasurementSet& measurementSet,
                              const FitParameters& parameters) const
  {
    // extrapolation to set FittedTrajectory
    double qOverP = parameters.qOverP();
    double previousDistance = -m_orderingTolerance;
    double previousDistanceR = -m_orderingTolerance;
    double previousDistanceZ = -m_orderingTolerance;
    bool reorder = false;

    Amg::Vector3D startDirection = parameters.direction();
    Amg::Vector3D startPosition = parameters.position();
    ExtrapolationType type = FittedTrajectory;
    std::unique_ptr<const TrackSurfaceIntersection> intersection {parameters.intersection()};

    const TrackSurfaceIntersection* startIntersection = intersection.get();
    int hit = measurements.size();
    for (MeasurementSet::const_iterator m = measurementSet.begin();
         m != measurementSet.end();
         m++, hit++) {
      std::unique_ptr<const TrackSurfaceIntersection> newIntersection{
        m_stepPropagator->intersectSurface(ctx,
                                           (**m).associatedSurface(),
                                           startIntersection,
                                           qOverP,
                                           m_stepField,
                                           Trk::muon)
      };
      if (newIntersection) {
        intersection = std::move(newIntersection);

        // check if ordering OK
        if (!reorder) {
          double distance = startDirection.dot(
            intersection->position() - startPosition);
          Amg::Vector3D positionMst = (**m).globalPosition();
          double distanceR = sqrt(
            (positionMst.x() - startPosition.x()) * (positionMst.x() - startPosition.x()) +
            (positionMst.y() - startPosition.y()) * (positionMst.y() - startPosition.y()));
          double distanceZ = (positionMst.z() - startPosition.z());
          if (startDirection.z() < 0) { distanceZ = -distanceZ; }
          if (distance < previousDistance
              && distanceR < previousDistanceR
              && distanceZ < previousDistanceZ) {
            reorder = true;
            ATH_MSG_DEBUG(" reorder 3D distance " << distance - previousDistance
                                                  << " R distance " << distanceR - previousDistanceR
                                                  << " Z distance " << distanceZ - previousDistanceZ);
          }
          previousDistance = distance - m_orderingTolerance;
          previousDistanceR = distanceR - m_orderingTolerance;
          previousDistanceZ = distanceZ - m_orderingTolerance;
        }
      } else {
        // FIXME
        // no intersection to MeasurementSet
        m_messageHelper->printWarning(15);
        intersection = std::make_unique<TrackSurfaceIntersection>(*intersection);
      }
      auto measurement = std::make_unique<FitMeasurement>(hit, nullptr, *m);
      measurement->intersection(type, intersection.release());
      measurement->qOverP(qOverP);
      measurements.push_back(measurement.release());
      //remember the last intersection for the next loop iteration
      startIntersection=&(measurements.back()->intersection(type));
    }

    // reorder if necessary
    if (reorder) { m_materialAllocator->orderMeasurements(measurements, startDirection, startPosition); }
  }

  bool
  iPatFitter::addMeasurements(
    const EventContext& ctx,
    std::vector<FitMeasurement*>& measurements,
    const FitParameters& parameters,
    ParticleHypothesis particleHypothesis,
    const DataVector<const TrackStateOnSurface>& trackStateOnSurfaces) const
  {
    // create vector of any TSOS'es which require fitted alignment corrections
    std::vector<Identifier> misAlignedTSOS;
    std::vector<int> misAlignmentNumbers;
    int misAlignmentNumber = 0;
    int tsos = 0;
    //  BUG that shifts ...   misAlignmentNumbers.push_back(misAlignmentNumber);
    for (DataVector<const TrackStateOnSurface>::const_iterator
         i = trackStateOnSurfaces.begin();
         i != trackStateOnSurfaces.end();
         ++i, ++tsos) {
      const TrackStateOnSurface& r = (**i);
      if (!r.alignmentEffectsOnTrack() || !r.trackParameters()) { continue; }
      const AlignmentEffectsOnTrack& AEOT = *r.alignmentEffectsOnTrack();
      ++misAlignmentNumber;
      for (const Identifier& a : AEOT.vectorOfAffectedTSOS()) {
        misAlignedTSOS.push_back(a);
        misAlignmentNumbers.push_back(misAlignmentNumber);
      }
      if (m_extendedDebug) { ATH_MSG_DEBUG(" tsos " << tsos
                                                    << " misAlignedTSOS.size() " << misAlignedTSOS.size()
                                                    << "        misAlignmentNumber " << misAlignmentNumber
                                                    << "  offset " << AEOT.deltaTranslation()
                                                    << " +- " << AEOT.sigmaDeltaTranslation()
                                                    << "  rotation " << AEOT.deltaAngle()
                                                    << " +- " << AEOT.sigmaDeltaAngle()); }
    }

    // create ordered list of FitMeasurements
    bool haveMaterial = false;
    bool haveMeasurement = false;
    int hit = measurements.size();
    double previousDistance = -m_orderingTolerance;
    double previousDistanceR = -m_orderingTolerance;
    double previousDistanceZ = -m_orderingTolerance;
    bool reorder = false;
    const bool skipVertexMeasurement = !measurements.empty();
    const Amg::Vector3D startDirection = parameters.direction();
    const Amg::Vector3D& startPosition = parameters.position();
    const TrackSurfaceIntersection* vertex = parameters.intersection();
    const TrackSurfaceIntersection* intersection = vertex;
    bool measurementsFlipped = false;
    double qOverP = parameters.qOverP();
    const ExtrapolationType type = FittedTrajectory;
    tsos = 0;
    for (DataVector<const TrackStateOnSurface>::const_iterator
         i = trackStateOnSurfaces.begin();
         i != trackStateOnSurfaces.end();
         ++i, ++hit, ++tsos) {
      const TrackStateOnSurface& s = (**i);
      std::unique_ptr<FitMeasurement> measurement1;
      std::unique_ptr<FitMeasurement> measurement2;
      const Surface* surface = nullptr;
      if (s.materialEffectsOnTrack() && s.trackParameters()) {
        Amg::Vector3D position = s.trackParameters()->position();
        bool calo = (!m_indetVolume->inside(position)
                     && m_calorimeterVolume->inside(position));
        qOverP = s.trackParameters()->parameters()[Trk::qOverP];
        surface = &s.trackParameters()->associatedSurface();

        // skip negligibly thin scatterers (exception for CaloEnergy)
        bool keepScatterer = true;
        if (s.materialEffectsOnTrack()->thicknessInX0() < 0.0001) {
          keepScatterer = false;
          if (calo) {
            const MaterialEffectsOnTrack* meot =
              dynamic_cast<const MaterialEffectsOnTrack*>(s.materialEffectsOnTrack());
            if (meot) {
              const EnergyLoss* energyLoss = meot->energyLoss();
              if (energyLoss && std::abs(energyLoss->deltaE()) > 0.1 * Gaudi::Units::MeV) { keepScatterer = true; }
            }
          }
        }
        if (keepScatterer) {
          measurement1 = std::make_unique<FitMeasurement>(s.materialEffectsOnTrack(), 
            ParticleMasses().mass[particleHypothesis], position, qOverP, calo);
          if (!calo
              && !haveMaterial
              && (haveMeasurement || s.measurementOnTrack())) { haveMaterial = true; }
        }
      } else if (s.alignmentEffectsOnTrack() && s.trackParameters()) {
        Amg::Vector3D direction = s.trackParameters()->momentum().unit();
        Amg::Vector3D position = s.trackParameters()->position();
        measurement1 = std::make_unique<FitMeasurement>(s.alignmentEffectsOnTrack(), direction, position);
      }
      if (s.measurementOnTrack()) {
        // option to skip vertex measurement (i.e. when not at front of list)
        if (skipVertexMeasurement && dynamic_cast<const PerigeeSurface*>(&s.surface())) {
          measurement1.reset();
          continue;
        }
        haveMeasurement = true;
        surface = &s.measurementOnTrack()->associatedSurface();
        measurement2 = std::make_unique<FitMeasurement>(hit, nullptr, s.measurementOnTrack());
        if (s.type(TrackStateOnSurface::Outlier)) { measurement2->setOutlier(); }
        // redundant surely??
        // if (measurement2->isCluster() || measurement2->isDrift()) haveMeasurement = true;
//	    if (misAlignmentNumber && misAlignedTSOS.back() == *s)
//	    {
//		measurement2->alignmentParameter(misAlignmentNumber);
//		misAlignedTSOS.pop_back();
//		misAlignmentNumbers.pop_back();
//		misAlignmentNumber	= misAlignmentNumbers.back();
//	    }
//
//	    Peter
//	    measurement2->alignmentParameter(0);
        if (misAlignmentNumber) {
          const Trk::MeasurementBase* meas = s.measurementOnTrack();
          Identifier id = Identifier();
          if (meas) {
            const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(meas);
            if (rot) {
              id = rot->identify();
            } else {
              const Muon::CompetingMuonClustersOnTrack* crot =
                dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(meas);
              if (crot && !crot->containedROTs().empty() && crot->containedROTs().front()) {
                id = crot->containedROTs().front()->identify();
              }
            }
          }
          for (unsigned int im = 0; im < misAlignedTSOS.size(); ++im) {
            if (misAlignedTSOS[im] != id) { continue; }
            measurement2->alignmentParameter(misAlignmentNumbers[im]);
          }
          if (m_extendedDebug) {
            for (unsigned int im = 0; im < misAlignedTSOS.size(); ++im) {
              if (misAlignedTSOS[im] != id) { continue; }
              if (measurement2->isDrift()) {
                ATH_MSG_DEBUG(" tsos " << tsos
                                       << "   Drift Measurement im " << im
                                       << "  with misAlignmentNumber " << misAlignmentNumbers[im]);
              } else {
                ATH_MSG_DEBUG(" tsos " << tsos
                                       << " Cluster Measurement im " << im
                                       << "  with misAlignmentNumber " << misAlignmentNumbers[im]);
              }
            }
          }
        }
      } else if (!measurement1 && s.trackParameters()) {
        if (s.type(TrackStateOnSurface::Hole)) {
          measurement2 = std::make_unique<FitMeasurement>(s);
        } else if (s.type(TrackStateOnSurface::Perigee)) {
          if (i == trackStateOnSurfaces.begin()) { continue; }
          const Perigee* perigee = dynamic_cast<const Perigee*>(s.trackParameters());
          if (!perigee) { continue; }
          measurement2 = std::make_unique<FitMeasurement>(*perigee);
        } else if (s.type(TrackStateOnSurface::Parameter)) {
          continue;
        } else {
          // TSOS type not understood.
          m_messageHelper->printWarning(16, s.dumpType());
          continue;
        }
      } else if (s.materialEffectsOnTrack()) {
        surface = &s.materialEffectsOnTrack()->associatedSurface();
      } else if (s.alignmentEffectsOnTrack()) {
        surface = &s.alignmentEffectsOnTrack()->associatedSurface();
      } else {
        // skip TSOS with missing trackParameters
        // this should never happen (i.e. where's the surface?)
        m_messageHelper->printWarning(17, s.dumpType());
        continue;
      }

      // current intersection
      if (s.trackParameters() && (measurement1 || measurement2)) {
        Amg::Vector3D direction = s.trackParameters()->momentum().unit();
        if (startDirection.dot(direction) < 0.) {
          measurementsFlipped = true;
          direction = -direction;
          if (measurement1) { measurement1->flipDriftDirection(); }
          if (measurement2) { measurement2->flipDriftDirection(); }
        }
        qOverP = s.trackParameters()->parameters()[Trk::qOverP];
        intersection = new TrackSurfaceIntersection(s.trackParameters()->position(),
                                                    direction,
                                                    0.);
      } else if (surface) {
        const TrackSurfaceIntersection* newIntersection =
          m_stepPropagator->intersectSurface(
            ctx, *surface, intersection, qOverP, m_stepField, Trk::muon);

        if (!newIntersection) {
          m_messageHelper->printWarning(18);
          measurement2.reset();
          continue;
        }

        if (newIntersection == intersection) {
          intersection = new TrackSurfaceIntersection(*intersection);
        } else {
          intersection = newIntersection;
        }
        if (s.materialEffectsOnTrack()) {
          const Amg::Vector3D& position = intersection->position();
          bool calo = (!m_indetVolume->inside(position)
                       && m_calorimeterVolume->inside(position));
          measurement1 = std::make_unique<FitMeasurement>(s.materialEffectsOnTrack(), 
            ParticleMasses().mass[particleHypothesis], intersection->position(), qOverP, calo);
          if (!calo
              && !haveMaterial
              && haveMeasurement) { haveMaterial = true; }
        } else if (!measurement2) {
          // TSOS skipped
          m_messageHelper->printWarning(19, s.dumpType());
          // delete intersection;
          continue;
        }
      }

      // check if ordering OK
      if (!reorder) {
        double distance = startDirection.dot(intersection->position() - startPosition);
        Amg::Vector3D positionMst = startPosition;
        if (s.measurementOnTrack()) { positionMst = s.measurementOnTrack()->globalPosition(); }
        if (s.materialEffectsOnTrack()) { positionMst = s.materialEffectsOnTrack()->associatedSurface().center(); }
        double distanceR = sqrt(
          (positionMst.x() - startPosition.x()) * (positionMst.x() - startPosition.x()) +
          (positionMst.y() - startPosition.y()) * (positionMst.y() - startPosition.y()));
        double distanceZ = (positionMst.z() - startPosition.z());
        if (startDirection.z() < 0) { distanceZ = -distanceZ; }
        if (distance < previousDistance
            && distanceR < previousDistanceR
            && distanceZ < previousDistanceZ) {
          reorder = true;
          ATH_MSG_DEBUG(" reorder 3D distance " << distance - previousDistance
                                                << " R distance " << distanceR - previousDistanceR
                                                << " Z distance " << distanceZ - previousDistanceZ);
        }
        previousDistance = distance - m_orderingTolerance;
        previousDistanceR = distanceR - m_orderingTolerance;
        previousDistanceZ = distanceZ - m_orderingTolerance;
      }

      // insert measurement(s) in list
      if (measurement1) {
        if (measurement2) {
          const TrackSurfaceIntersection* intersectionCopy =
            new TrackSurfaceIntersection(*intersection);

          measurement1->intersection(type, intersectionCopy);
          measurements.push_back(measurement1.release());
        } else {
          measurement1->intersection(type, intersection);
          measurement1->qOverP(qOverP);
          measurements.push_back(measurement1.release());
        }
      }
      if (measurement2) {
        measurement2->intersection(type, intersection);
        measurement2->qOverP(qOverP);
        measurements.push_back(measurement2.release());
      }
    }

    // reorder if necessary
    if (reorder) { m_materialAllocator->orderMeasurements(measurements, startDirection, startPosition); }
    if (measurementsFlipped) {
      m_messageHelper->printWarning(25);
    }

    // flag whether material has already been allocated
    delete vertex;
    return haveMaterial;
  }

  std::unique_ptr<Trk::Track>
  iPatFitter::performFit(
    FitState& fitState,
    const ParticleHypothesis particleHypothesis,
    const TrackInfo& trackInfo,
    const DataVector<const TrackStateOnSurface>* leadingTSOS,
    const FitQuality* perigeeQuality,
    Garbage_t& garbage) const
  {
    std::vector<FitMeasurement*>& measurements = fitState.getMeasurements();
    FitParameters* parameters = fitState.parameters.get();
    // initialize the scattering centres
    m_materialAllocator->initializeScattering(measurements);

    // set fixed momentum in lineFit case according to configuration
    if (m_lineFit) {
      parameters->fitMomentum(false);
      if (m_lineMomentum > 0.) { parameters->qOverP(1. / m_lineMomentum); }
    }

    // perform fit
    MsgStream log(msgSvc(), name());
    std::scoped_lock lock(m_fitProcedureMutex);
    const FitProcedureQuality& quality = m_fitProcedure->execute(m_asymmetricCaloEnergy,
                                                                 log,
                                                                 measurements,
                                                                 parameters,
                                                                 perigeeQuality);
    std::unique_ptr<Track> fittedTrack;
    if (!quality.fitCode()) {
//  // iterate if material reallocation needed
//  if (m_materialAllocator->reallocateMaterial(*measurements,parameters))
//  {
//      // do something
//      quality	= m_fitProcedure->execute(log,
//                        measurements,
//                        parameters,
//                        perigeeQuality);
//  }

      // include leading material
      m_materialAllocator->addLeadingMaterial(measurements, particleHypothesis, *parameters, garbage);


      // construct the fitted track
      fittedTrack.reset(m_fitProcedure->constructTrack(measurements,
                                                   *parameters,
                                                   trackInfo,
                                                   leadingTSOS));
      if (fittedTrack) {
        // set StraightLine when momentum unfitted
        if (!parameters->fitMomentum()) { fittedTrack->info().setTrackProperties(TrackInfo::StraightTrack); }

        // special check for CaloDeposit - parameters must be inside calorimeter
        for (DataVector<const TrackStateOnSurface>::const_iterator
             s = fittedTrack->trackStateOnSurfaces()->begin();
             s != fittedTrack->trackStateOnSurfaces()->end();
             ++s) {
          if (!(**s).type(TrackStateOnSurface::CaloDeposit)) { 
            continue; 
          }
          if ((**s).trackParameters()) {
            Amg::Vector3D position = (**s).trackParameters()->position();
            if (!m_indetVolume->inside(position) && m_calorimeterVolume->inside(position)) { 
              break; 
            }
          }

          // something badly wrong: WARN and kill track
          // fail fit as CaloDeposit outside calo volume
          m_messageHelper->printWarning(20);
          fittedTrack.reset();
          break;
        }
      }

      // statistics for successful fit
      if (fittedTrack) {
        ++m_countGoodFits;
        m_countIterations += quality.iterations();
        fitState.iterations = quality.iterations();

        // report material
        if (msgLvl(MSG::DEBUG)) {
          int calo = 0;
          double caloX0 = 0.;
          double caloEloss = 0.;
          int indet = 0;
          double indetX0 = 0.;
          double indetEloss = 0.;
          int spect = 0;
          double spectX0 = 0.;
          double spectEloss = 0.;
          for (FitMeasurement* m : measurements) {
            if (m->isEnergyDeposit()) {
              calo++;
              caloEloss += m->energyLoss();
            } else if (!m->isScatterer()) {
              continue;
            }

            if (m_indetVolume->inside(m->position())) {
              indet++;
              indetX0 += m->materialEffects()->thicknessInX0();
              indetEloss += m->energyLoss();
              // conflicting energy deposit sign for inDet material
              if (m->energyLoss() * indetEloss < 0.) { m_messageHelper->printWarning(21); }
              continue;
            } if (m_calorimeterVolume->inside(
                         m->intersection(FittedTrajectory).position())) {
              calo++;
              caloX0 += m->materialEffects()->thicknessInX0();
              continue;
            }

            spect++;
            spectX0 += m->materialEffects()->thicknessInX0();
            spectEloss += m->energyLoss();
            // conflicting energy deposit sign for spectrometer material
            if (m->energyLoss() * spectEloss < 0.) { m_messageHelper->printWarning(22); }
          }

          // WARN in case of bug #56297
          if (calo) {
            // excessive calorimeter energy loss
            if (std::abs(caloEloss) > 200. * Gaudi::Units::GeV
                && m_messageHelper->wouldPrintWarning(23)) {
              std::stringstream ss;
              ss << caloEloss / Gaudi::Units::GeV << " for track with P "
                 << fittedTrack->perigeeParameters()->momentum().mag() / Gaudi::Units::GeV
                 << " GeV";
              m_messageHelper->printWarning(23, ss.str());
            }

            // excessive spectrometer energy loss
            if ((std::abs(spectEloss) > (0.5 * Gaudi::Units::GeV))
                && (std::abs(spectEloss * caloX0) > std::abs(4. * caloEloss * spectX0))
                && m_messageHelper->wouldPrintWarning(24)) {
              std::stringstream ss;
              ss << spectEloss / Gaudi::Units::GeV
                 << " ( " << caloEloss / Gaudi::Units::GeV
                 << " calorimeter energy loss). Track has P "
                 << fittedTrack->perigeeParameters()->momentum().mag() / Gaudi::Units::GeV
                 << " GeV";
              m_messageHelper->printWarning(24, ss.str());
            }
          }

          msg(MSG::VERBOSE) << std::setiosflags(std::ios::fixed);
          if (indet) {
            msg() << std::setw(5) << indet << " indet scatterers with"
                  << " material: X0" << std::setw(6) << std::setprecision(3) << indetX0;
            if (calo && caloEloss < 0.) {
              msg() << "  energyGain";
            } else {
              msg() << "  energyLoss";
            }
            msg() << std::setw(6) << std::setprecision(3) << indetEloss / Gaudi::Units::GeV << ",";
          }

          if (spect) {
            msg() << std::setw(5) << spect << " spectrometer scatterers with"
                  << " material: X0" << std::setw(8) << std::setprecision(3) << spectX0;
            if (calo && caloEloss < 0.) {
              msg() << "  energyGain";
            } else {
              msg() << "  energyLoss";
            }
            msg() << std::setw(7) << std::setprecision(3) << spectEloss / Gaudi::Units::GeV;
          }

          if (!indet && !spect) { msg() << "    0 scatterers - no tracking material"; }
          msg() << endmsg;
        }
      }
    }

   // generate a track summary for this candidate
   if ( (m_trackSummaryTool.isEnabled()) && (fittedTrack!=nullptr) ) {
     m_trackSummaryTool->computeAndReplaceTrackSummary(*fittedTrack, nullptr, false);
   }

    return fittedTrack;
  }

  void
  iPatFitter::printTSOS(const Track& track) const {
    // debugging aid
    MsgStream log(msgSvc(), name());

    msg(MSG::INFO) << " track with " << track.trackStateOnSurfaces()->size() << " TSOS " << endmsg;
    int tsos = 0;
    for (DataVector<const TrackStateOnSurface>::const_iterator t = track.trackStateOnSurfaces()->begin();
         t != track.trackStateOnSurfaces()->end();
         ++t, ++tsos) {
      msg() << std::setiosflags(std::ios::fixed | std::ios::right)
            << " TSOS# " << std::setw(3) << tsos
            << "   parameters:   "
            << std::setw(7) << std::setprecision(1)
            << (**t).trackParameters()->position().perp()
            << std::setw(8) << std::setprecision(4)
            << (**t).trackParameters()->position().phi()
            << std::setw(9) << std::setprecision(1)
            << (**t).trackParameters()->position().z()
            << " position  "
            << std::setw(8) << std::setprecision(4)
            << (**t).trackParameters()->momentum().phi()
            << " phi  "
            << std::setw(7) << std::setprecision(4)
            << (**t).trackParameters()->momentum().theta()
            << " theta  "
            << std::setw(9) << std::setprecision(4)
            << (**t).trackParameters()->momentum().mag() / Gaudi::Units::GeV << " GeV";

      if ((**t).measurementOnTrack()) {
        msg() << "  meas ";
      } else {
        msg() << "       ";
      }
      if ((**t).materialEffectsOnTrack()) {
        msg() << "  scat ";
      } else {
        msg() << "       ";
      }
      msg() << (**t).dumpType() << endmsg;
    }
  }

  void
  iPatFitter::refit(
    const EventContext& ctx,
    FitState& fitState,
    const Track& track,
    const RunOutlierRemoval runOutlier,
    const ParticleHypothesis particleHypothesis) const
  {
    ATH_MSG_VERBOSE(" refit ");
    unsigned countGoodFits = m_countGoodFits;
    unsigned countIterations = m_countIterations;
    m_countRefitAttempts++;
    // outlier removal not implemented
    if (runOutlier) { m_messageHelper->printWarning(0); }

    // create Perigee if starting parameters are for a different surface type
    const Perigee* perigeeParameters = track.perigeeParameters(); 
    // Note: we don't own the Perigee from perigeeParameters(), but if it returns nullptr, we have to make our own, 
    // and we need to delete it, so it's put in this unique_ptr
    std::unique_ptr<Perigee> newPerigee;
    std::unique_ptr<PerigeeSurface> perigeeSurface;

    if (!perigeeParameters) {
      auto i = track.trackStateOnSurfaces()->begin();
      while (i != track.trackStateOnSurfaces()->end() && !(**i).trackParameters()) { i++; }
      const TrackStateOnSurface& s = (**i);
      if (!s.trackParameters()) {
        // input track without parameters
        m_messageHelper->printWarning(1);
        m_countGoodRefits += m_countGoodFits - countGoodFits;
        m_countGoodFits = countGoodFits;
        m_countRefitIterations += m_countIterations - countIterations;
        m_countIterations = countIterations;
        return;
      }

      Amg::Vector3D origin(s.trackParameters()->position());
      perigeeSurface = std::make_unique<PerigeeSurface>(origin);
      newPerigee = std::make_unique<Perigee>(
        s.trackParameters()->position(),
        s.trackParameters()->momentum(),
        s.trackParameters()->charge(),
        *perigeeSurface);
    }

    const Perigee& perigee = newPerigee ? *newPerigee : *perigeeParameters; // Use the valid Perigee

    fitState.parameters = std::make_unique<FitParameters>(perigee);

    // set up the measurements
    if (!track.trackStateOnSurfaces()) {
      // input track without trackStateOnSurfaces
      m_messageHelper->printWarning(2);
      m_countGoodRefits += m_countGoodFits - countGoodFits;
      m_countGoodFits = countGoodFits;
      m_countRefitIterations += m_countIterations - countIterations;
      m_countIterations = countIterations;
      return;
    }
    
    fitState.newMeasurements();

    bool haveMaterial = addMeasurements(ctx,
                                        fitState.getMeasurements(),
                                        *fitState.parameters,
                                        particleHypothesis,
                                        *track.trackStateOnSurfaces());

    // allocate material
    Garbage_t garbage;
    if (!haveMaterial && particleHypothesis != Trk::nonInteracting) {
      m_materialAllocator->allocateMaterial(fitState.getMeasurements(),
                                            particleHypothesis,
                                            *fitState.parameters,
                                            perigee,
                                            garbage);
    }

    // perform fit and return fitted track
    TrackInfo trackInfo(TrackInfo::iPatTrackFitter, particleHypothesis);
    trackInfo.addPatternReco(track.info());
    std::unique_ptr<Trk::Track> fittedTrack { performFit(fitState, particleHypothesis, trackInfo, nullptr, nullptr, garbage) };

    m_countGoodRefits += m_countGoodFits - countGoodFits;
    m_countGoodFits = countGoodFits;
    m_countRefitIterations += m_countIterations - countIterations;
    m_countIterations = countIterations;
 }
} // end of namespace
