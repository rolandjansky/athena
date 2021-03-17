/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 least squared fit to track hit data => PerigeeParameters with covariance
 and fit quality.
 ------------------------------------------------------------------------
 ***************************************************************************/


#include "iPatTrackFitter/TrackFitter.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/MsgStream.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkVolumes/Volume.h"
#include "TrkiPatFitterUtils/ExtrapolationType.h"
#include "TrkiPatFitterUtils/FitMeasurement.h"
#include "TrkiPatFitterUtils/FitParameters.h"
#include "TrkiPatFitterUtils/FitProcedure.h"
#include "TrkiPatFitterUtils/FitProcedureQuality.h"
#include "TrkiPatFitterUtils/IMaterialAllocator.h"
#include "VxVertex/RecVertex.h"
#include <cmath>
#include <iomanip>


TrackFitter::TrackFitter(const std::string& type,
                         const std::string& name,
                         const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_materialAllocator("Trk::MaterialAllocator/MaterialAllocator")
  , m_rungeKuttaIntersector("Trk::RungeKuttaIntersector/RungeKuttaIntersector")
  , m_scatteringAngle(13.6)
  , // Coulomb scattering constant
  m_solenoidalIntersector("Trk::SolenoidalIntersector/SolenoidalIntersector")
  , m_straightLineIntersector(
      "Trk::StraightLineIntersector/StraightLineIntersector")
  , m_stepPropagator("Trk::STEP_Propagator/AtlasSTEP_Propagator")
  , m_trackingVolumesSvc("Trk::TrackingVolumesSvc/Trk::TrackingVolumesSvc",
                         name)
  , m_vertexMeasurement(0)
  , m_indetVolume(nullptr)
{
  declareInterface<IiPatFitter>(this);
  declareProperty("MaterialAllocator", m_materialAllocator);
  declareProperty("RungeKuttaIntersector", m_rungeKuttaIntersector);
  declareProperty("SolenoidalIntersector", m_solenoidalIntersector);
  declareProperty("StraightLineIntersector", m_straightLineIntersector);
}

TrackFitter::~TrackFitter(void) {}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

TrackFitter::State::~State()
{
  clearMeasurements();
  delete m_parameters;
}

void
TrackFitter::State::clearMeasurements()
{
  for (auto m : m_measurements) {
    delete m;
  }
  m_measurements.clear();
}

void
TrackFitter::createMeasurements(State& state,
                                hit_const_iterator begin,
                                hit_const_iterator end) const
{
  state.m_driftHits = 0;
  state.m_planarHits = 0;

  // copy hits into fitMeasurement vector
  int hit = 0;
  if (state.m_vertexMeasurement)
    state.m_measurements.push_back(state.m_vertexMeasurement);
  for (hit_const_iterator h = begin; h != end; ++h) {
    // fit up to brem hit
    if ((**h).status() == brem)
      break;
    switch ((**h).status()) {
      case pending:
        break;
      case hole:
      case rejected:
      case dead:
      case boundary:
        ++hit;
        break;
      case broad_pixel:
      case pixel_cluster:
      case broad_strip:
      case strip_cluster:
        ++state.m_planarHits;
        if ((**h).rot()) {
          state.m_measurements.push_back(
            new Trk::FitMeasurement(hit, *h, (**h).rot()));

        } else if ((**h).isPixel()) {
          state.m_measurements.push_back(
            new Trk::FitMeasurement(hit,
                                    *h,
                                    Amg::Vector3D((**h).position()),
                                    (**h).sigma(),
                                    (**h).secondSigma(),
                                    (**h).sinStereo(),
                                    static_cast<int>((**h).status()),
                                    (**h).surface(),
                                    Trk::pixelCluster));
        } else {
          state.m_measurements.push_back(
            new Trk::FitMeasurement(hit,
                                    *h,
                                    Amg::Vector3D((**h).position()),
                                    (**h).sigma(),
                                    0.,
                                    (**h).sinStereo(),
                                    static_cast<int>((**h).status()),
                                    (**h).surface(),
                                    Trk::stripCluster));
        }
        ++hit;
        break;
      case pixel_reject:
      case strip_reject:
        if ((**h).rot()) {
          state.m_measurements.push_back(
            new Trk::FitMeasurement(hit, *h, (**h).rot()));

        } else if ((**h).isPixel()) {
          state.m_measurements.push_back(
            new Trk::FitMeasurement(hit,
                                    *h,
                                    Amg::Vector3D((**h).position()),
                                    (**h).sigma(),
                                    (**h).secondSigma(),
                                    (**h).sinStereo(),
                                    static_cast<int>((**h).status()),
                                    (**h).surface(),
                                    Trk::pixelCluster));
        } else {
          state.m_measurements.push_back(
            new Trk::FitMeasurement(hit,
                                    *h,
                                    Amg::Vector3D((**h).position()),
                                    (**h).sigma(),
                                    0.,
                                    (**h).sinStereo(),
                                    static_cast<int>((**h).status()),
                                    (**h).surface(),
                                    Trk::stripCluster));
        }
        (**state.m_measurements.rbegin()).setOutlier();
        ++hit;
        break;
      case drift_time:
      case drift_wire:
        if (!(**h).rot()) {
          ATH_MSG_WARNING(" rot should be provided for drift hit");
          break;
        }

        ++state.m_driftHits;
        state.m_measurements.push_back(
          new Trk::FitMeasurement(hit, *h, (**h).rot()));
        ++hit;
        break;
      case drift_reject:
        if (!(**h).rot()) {
          ATH_MSG_WARNING(" rot should be provided for drift hit");
          break;
        }
        state.m_measurements.push_back(
          new Trk::FitMeasurement(hit, *h, (**h).rot()));
        (**state.m_measurements.rbegin()).setOutlier();
        ++hit;
        break;
      case inactive:
      case inert:
      case scatterer:
        state.m_measurements.push_back(
          new Trk::FitMeasurement((**h).materialEffects(),
                                  Trk::ParticleMasses().mass[Trk::pion],
                                  Amg::Vector3D((**h).position())));

        ++hit;
        break;
      default:
        break;
    };
  }

  // ignore material at the end of the track
  Trk::FitMeasurement* meas = state.m_measurements.back();
  while (!meas->isPositionMeasurement()) {
    delete meas;
    state.m_measurements.pop_back();
    meas = state.m_measurements.back();
  }
}

void
TrackFitter::fitProcedure(State& state,
                          TrackStatus status,
                          const PerigeeParameters& perigee,
                          hit_const_iterator begin,
                          hit_const_iterator end) const
{
  // set parameters to starting values
  delete state.m_parameters;
  state.m_parameters = new Trk::FitParameters(perigee.transverseImpact(),
                                              perigee.z(),
                                              perigee.cosPhi(),
                                              perigee.sinPhi(),
                                              perigee.cotTheta(),
                                              perigee.qOverPt(),
                                              Amg::Vector3D(perigee.vertex()));

  // clean up previous fit
  state.m_fitProcedureQuality = nullptr;
  state.clearMeasurements();
  // create measurement vector, note special treatment for three_point tracks
  if (status == three_point) {
    threePoint(state, perigee, begin, end);
  } else {
    createMeasurements(state, begin, end);
  }

  // initialize the scattering centres
  m_materialAllocator->initializeScattering(state.m_measurements);

  // fail fit if there's a missing surface
  state.m_fitCode = 0;
  for (std::vector<Trk::FitMeasurement*>::const_iterator m =
         state.m_measurements.begin();
       m != state.m_measurements.end();
       ++m) {
    if ((**m).surface() || (**m).isVertex() || (**m).isPerigee())
      continue;
    ATH_MSG_WARNING(" fit fails as Trk::Surface is missing");
    state.m_fitCode = 1;
    state.m_fitProcedureQuality =
      std::make_unique<Trk::FitProcedureQuality>(state.m_fitCode, -1);
  }

  // main fit procedure
  if (!state.m_fitCode) {
    MsgStream log(msgSvc(), name());
    Trk::FitProcedure::Cache cache(
      m_fitProcedure->constrainedAlignmentEffects());
    state.m_fitProcedureQuality =
      std::make_unique<Trk::FitProcedureQuality>(m_fitProcedure->execute(
        cache, false, log, state.m_measurements, state.m_parameters, 0, true));
    state.m_fitCode = state.m_fitProcedureQuality->fitCode();

    // include leading material
    if (status != three_point && !state.m_fitCode) {
      Trk::IMaterialAllocator::Garbage_t garbage;
      m_materialAllocator->addLeadingMaterial(
        state.m_measurements, Trk::pion, *state.m_parameters, garbage);
    }
  }
}

void
TrackFitter::createPerigee(const State& state,
                           PerigeeParameters& perigeeParameters) const
{
  // return when fit fails
  if (state.m_fitCode != 0)
    return;

  // fill the perigee parameters and their covariance matrix
  perigeeParameters.fillPerigee(state.m_parameters->cosPhi(),
                                state.m_parameters->cotTheta(),
                                state.m_parameters->ptInv0(),
                                state.m_parameters->sinPhi(),
                                state.m_parameters->d0(),
                                state.m_parameters->z0());
  if (!perigeeParameters.hasCovariance()) {
    perigeeParameters.covariance(new AmgSymMatrix(5));
  }

  AmgSymMatrix(5)& covariance = *perigeeParameters.getCovariance();
  const AmgSymMatrix(5)& fitCovariance = *state.m_parameters->finalCovariance();
  for (int i = 0; i != 5; ++i) {
    for (int j = 0; j != 5; ++j) {
      covariance(i, j) = fitCovariance(i, j);
    }
  }

  // from theta to cotTheta
  double d3 =
    -1. / (state.m_parameters->sinTheta() * state.m_parameters->sinTheta());
  covariance(3, 3) *= d3;
  for (int j = 0; j != 5; ++j)
    covariance(j, 3) *= d3;

  // from qOverP to qOverPt
  double d4 = 1. / state.m_parameters->sinTheta();
  covariance(4, 4) *= d4;
  for (int j = 0; j != 5; ++j)
    covariance(j, 4) *= d4;
}

void
TrackFitter::print(const State& state,
                   TrackStatus status,
                   const PerigeeParameters& perigeeParameters,
                   const FitQuality& fitQuality) const
{
  msg(MSG::DEBUG) << "      " << status;
  perigeeParameters.print();
  if (fitQuality.fit_code()) {
    msg() << std::setiosflags(std::ios::fixed) << std::setw(3) << "   fitCode "
          << fitQuality.fit_code() << "   chi2(degf)" << std::setw(8)
          << std::setprecision(0) << fitQuality.chi_squared() << "("
          << fitQuality.degrees_of_freedom() << ")";
  } else {
    msg() << std::setiosflags(std::ios::fixed) << std::setw(7)
          << std::setprecision(4) << "   fitProb "
          << fitQuality.fit_probability();
  }
  msg() << std::setiosflags(std::ios::fixed) << std::setw(6)
        << state.m_planarHits << "(" << std::setw(2) << state.m_driftHits
        << ") planar(drift) hits " << std::setw(5)
        << state.m_parameters->numberScatterers() << " scattering parameters "
        << endmsg;
}

void
TrackFitter::createQuality(const State& state, FitQuality*& fitQuality) const
{
  if (fitQuality == 0)
    fitQuality = new FitQuality;
  fitQuality->fill(state.m_fitProcedureQuality->chiSq(),
                   state.m_fitProcedureQuality->numberDoF(),
                   state.m_fitProcedureQuality->fitCode(),
                   state.m_fitProcedureQuality->fitProbability(),
                   state.m_fitProcedureQuality->chiSqWorst());
}

parameter_vector*
TrackFitter::createScatterers(const State& state) const
{
  // return empty vector for fit without scattering
  parameter_vector* scattererParameters = new parameter_vector;
  scattererParameters->reserve(20);
  if (!state.m_parameters->numberScatterers())
    return scattererParameters;

  int param = 5;
  double qOverP = 0.;
  int scatter = 0;
  for (std::vector<Trk::FitMeasurement*>::const_iterator m =
         state.m_measurements.begin();
       m != state.m_measurements.end();
       ++m) {
    if (!(**m).isScatterer())
      continue;
    const Trk::TrackSurfaceIntersection& intersection =
      (**m).intersection(Trk::FittedTrajectory);
    Amg::Vector3D direction = intersection.direction();
    double sinTheta = direction.perp();
    qOverP = (**m).qOverP();
    double inversePt = qOverP / sinTheta;
    direction /= sinTheta;
    if (!(**m).numberDoF()) {
      scattererParameters->push_back(
        new ScattererParameters(intersection.position().x(),
                                intersection.position().y(),
                                intersection.position().z(),
                                direction.x(),
                                direction.y(),
                                direction.z(),
                                inversePt,
                                0.,
                                0.,
                                0.,
                                0.));
      continue;
    }

    scattererParameters->push_back(
      new ScattererParameters(intersection.position().x(),
                              intersection.position().y(),
                              intersection.position().z(),
                              direction.x(),
                              direction.y(),
                              direction.z(),
                              inversePt,
                              state.m_parameters->scattererTheta(scatter),
                              state.m_parameters->scattererPhi(scatter),
                              0.,
                              0.));

    // sqrt(fitCovariance[param+1][param+1]),
    // sqrt(fitCovariance[param][param])));
    param += 2;
    ++scatter;
  }

  // add parameters at last point (null scatterer)
  const Trk::TrackSurfaceIntersection& intersection =
    (**(state.m_measurements.rbegin())).intersection(Trk::FittedTrajectory);
  Amg::Vector3D direction = intersection.direction();
  double sinTheta = direction.perp();
  double inversePt = qOverP / sinTheta;
  direction /= sinTheta;
  scattererParameters->push_back(
    new ScattererParameters(intersection.position().x(),
                            intersection.position().y(),
                            intersection.position().z(),
                            direction.x(),
                            direction.y(),
                            direction.z(),
                            inversePt,
                            0.,
                            0.,
                            0.,
                            0.));
  return scattererParameters;
}

void
TrackFitter::setResiduals(State& state) const
{
  // set residual from FitMeasurement into corresponding HitOnTrack
  for (std::vector<Trk::FitMeasurement*>::iterator m =
         state.m_measurements.begin();
       m != state.m_measurements.end();
       ++m) {
    HitOnTrack* hit = (**m).hitOnTrack();
    if (!hit)
      continue;
    if ((**m).hasIntersection(Trk::FittedTrajectory)) {
      hit->intersection(new Trk::TrackSurfaceIntersection(
        (**m).intersection(Trk::FittedTrajectory)));
    }
    if (!(**m).numberDoF())
      continue;

    hit->residual((**m).residual());
    if ((**m).is2Dimensional())
      hit->secondResidual((**m).residual2());
  }
}

void
TrackFitter::threePoint(State& state,
                        const PerigeeParameters& perigee,
                        hit_const_iterator begin,
                        hit_const_iterator end) const
{
  // TEMPORARY for 3-point tracks:
  // when cluster.sigma() working and checked this should move to
  // CandidateBuilder for 3-point tracks: compute scattering error to add to
  // first and last layers from separation wrt 2nd layer - assuming 1% radiation
  // length
  double scatteringAngle =
    m_scatteringAngle * perigee.sinTheta() / perigee.abs_pt();
  hit_const_iterator h2 = begin;
  ++h2;
  hit_const_iterator h3 = end;
  --h3;
  if ((**h2).isPixel()) {
    state.m_hit_pt3 = 2;
  } else {
    state.m_hit_pt3 = 3;
  }

  double inv_sinTheta_square = 1. + perigee.cotTheta() * perigee.cotTheta();
  state.m_sigma_pt1 =
    scatteringAngle * ((**h2).position().perp() - (**begin).position().perp());
  state.m_sigma_pt1 *= 0.01 * state.m_sigma_pt1 * inv_sinTheta_square;
  // TODO - check barrel / endcap (dr or dz/cotTheta)
  // 0.5 as 2 meas in SCT
  state.m_sigma_pt3 =
    scatteringAngle * ((**h3).position().perp() - (**h2).position().perp());
  state.m_sigma_pt3 *= 0.5 * 0.01 * state.m_sigma_pt3 * inv_sinTheta_square;

  state.m_second_sigma_pt1 = state.m_sigma_pt1 * inv_sinTheta_square;
  if ((**begin).isBarrel()) {
    // 	    double delta_r = ((**h2).position().perp() -
    // (**begin).position().perp()); 	    double sigma_scat1 =
    // sqrt(state.m_sigma_pt1); 	    double sigma_scat3 = sqrt(state.m_sigma_pt3);
    // 	    cout << " 3point scat (barrel) " << 10000.*sigma_scat1
    // 		 << "  " << 10000.*sigma_scat3 << "  delta_r12 " << delta_r;
  } else {
    double inv_cosTheta_square =
      1. + 1. / perigee.cotTheta() * perigee.cotTheta();
    state.m_second_sigma_pt1 = state.m_sigma_pt1 * inv_cosTheta_square;

    // 	    double delta_r = ((**h2).position().z() -
    // (**begin).position().z())/perigee.cotTheta(); 	    double sigma_scat1 =
    // sqrt(state.m_sigma_pt1); 	    double sigma_scat3 = sqrt(state.m_sigma_pt3);
    // 	    cout << " 3point scat (endcap) " << 10000.*sigma_scat1
    // 		 << "  " << 10000.*sigma_scat3 << "  delta_r12 " << delta_r;
  }
  // 	cout << endl;

  state.m_driftHits = 0;
  state.m_planarHits = 0;

  // copy hits into fitMeasurement vector
  int hit = 0;
  for (hit_const_iterator h = begin; h != end; ++h) {
    double sigmaWidth = (**h).sigma();
    double sigmaLength = (**h).secondSigma();
    switch ((**h).status()) {
      case pixel_cluster:
      case broad_pixel:
        ++state.m_planarHits;
        if (hit == 0) {
          sigmaWidth = sqrt(state.m_sigma_pt1 + sigmaWidth * sigmaWidth);
          sigmaLength =
            sqrt(state.m_second_sigma_pt1 + sigmaLength * sigmaLength);
        }
        state.m_measurements.push_back(
          new Trk::FitMeasurement(hit,
                                  *h,
                                  Amg::Vector3D((**h).position()),
                                  sigmaWidth,
                                  sigmaLength,
                                  (**h).sinStereo(),
                                  static_cast<int>((**h).status()),
                                  (**h).surface(),
                                  Trk::pixelCluster));
        ++hit;
        break;
      case strip_cluster:
      case broad_strip:
        ++state.m_planarHits;
        if (hit >= state.m_hit_pt3) {
          sigmaWidth = sqrt(state.m_sigma_pt3 + sigmaWidth * sigmaWidth);
        }
        state.m_measurements.push_back(
          new Trk::FitMeasurement(hit,
                                  *h,
                                  Amg::Vector3D((**h).position()),
                                  sigmaWidth,
                                  0.,
                                  (**h).sinStereo(),
                                  static_cast<int>((**h).status()),
                                  (**h).surface(),
                                  Trk::stripCluster));
        ++hit;
        break;
      default:
        break;
    };
  }
}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
TrackFitter::initialize()
{
  // print name and package version
  ATH_MSG_INFO("TrackFitter::initialize() - package version "
               << PACKAGE_VERSION);

  // retrieve the MaterialProvider and various Intersectors
  if (m_materialAllocator.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_materialAllocator);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_materialAllocator);
  }
  if (m_rungeKuttaIntersector.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_rungeKuttaIntersector);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_rungeKuttaIntersector);
  }
  if (m_solenoidalIntersector.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_solenoidalIntersector);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_solenoidalIntersector);
  }
  if (m_straightLineIntersector.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_straightLineIntersector);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_straightLineIntersector);
  }

  if (m_stepPropagator.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_stepPropagator);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_stepPropagator);
  }

  if (m_trackingVolumesSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve Svc " << m_trackingVolumesSvc);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved Svc " << m_trackingVolumesSvc);
    m_indetVolume = new Trk::Volume(m_trackingVolumesSvc->volume(
      Trk::ITrackingVolumesSvc::CalorimeterEntryLayer));
  }

  // can now create FitProcedure class
  m_fitProcedure =
    std::make_unique<Trk::FitProcedure>(false,
                                        false,
                                        false,
                                        m_rungeKuttaIntersector,
                                        m_solenoidalIntersector,
                                        m_straightLineIntersector,
                                        m_stepPropagator,
                                        m_indetVolume,
                                        1);

  return StatusCode::SUCCESS;
}

StatusCode
TrackFitter::finalize()
{
  return StatusCode::SUCCESS;
}

void
TrackFitter::fit(TrackStatus status,
                 FitQuality*& fitQuality,
                 PerigeeParameters& perigeeParameters,
                 hit_const_iterator begin,
                 hit_const_iterator end) const
{
  ATH_MSG_VERBOSE(" fit " << status);

  // ESD independent fit procedure
  State state;
  fitProcedure(state, status, perigeeParameters, begin, end);

  // store results back into input classes
  createQuality(state, fitQuality);
  createPerigee(state, perigeeParameters);
}

void
TrackFitter::fitWithRejection(TrackStatus status,
                              FitQuality*& fitQuality,
                              int maxReject,
                              PerigeeParameters& perigeeParameters,
                              parameter_vector*& scattererParameters,
                              hit_iterator begin,
                              hit_iterator end,
                              hit_const_iterator original_begin,
                              hit_const_iterator original_end) const
{
  ATH_MSG_VERBOSE(" fitWithRejection " << status);
  double chi_squared_cut_off = 6.25; // -> flat trt prob
  int number_rejected = 0;
  int iter = 0;
  bool fit = true;

  State state;
  while (fit) {
    // ESD independent fit procedure
    fitProcedure(state, status, perigeeParameters, begin, end);

    // for a good fit: store results back into input classes
    createQuality(state, fitQuality);
    createPerigee(state, perigeeParameters);

    // iterate after a good fit with a large hit residual
    if (state.m_fitCode ||
        fitQuality->worst_hit_chi_squared() < chi_squared_cut_off)
      break;

    // don't touch any of the original hits from the candidate track
    // space-points demote drift hits to wire hits as first attempt (assume
    // occupancy effect) demote clusters to broad on assumption they are shared
    // or deltas when above chi_squared_cut_off
    hit_iterator h = begin;
    hit_const_iterator o = original_begin;
    int i;
    for (i = 1; i < state.m_fitProcedureQuality->worstMeasurement(); ++i)
      ++h;
    bool original = false;
    while (i-- > 0 && o != original_end) {
      if ((*o)->identifier() == (**h).identifier()) {
        original = true;
        break;
      }
      ++o;
    }
    if (original){
      break;
    }

    // as a first attempt:
    // demote drift hits to wire hits - assume due to occupancy effect
    // demote clusters to broad on assumption they are shared or deltas
    // reject the hit if this fails
    HitStatus hitStatus = pending;
    bool reject = true;
    switch ((**h).status()) {
      case pixel_cluster:
        if ((**h).broadSigma() * (**h).broadSigma() * chi_squared_cut_off >
            (**h).sigma() * (**h).sigma() *
              fitQuality->worst_hit_chi_squared()) {
          hitStatus = broad_pixel;
          reject = false;
        } else {
          hitStatus = pixel_reject;
        }
        break;
      case broad_pixel:
        hitStatus = pixel_reject;
        break;
      case strip_cluster:
        if ((**h).broadSigma() * (**h).broadSigma() * chi_squared_cut_off >
            (**h).sigma() * (**h).sigma() *
              fitQuality->worst_hit_chi_squared()) {
          hitStatus = broad_strip;
          reject = false;
        } else {
          hitStatus = strip_reject;
        }
        break;
      case broad_strip:
        hitStatus = strip_reject;
        break;
      case drift_time:
        hitStatus = drift_wire;
        break;
      case drift_wire:
        hitStatus = drift_reject;
        break;
      default:
        ATH_MSG_DEBUG(" reject " << (**h).status());
        break;
    };

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << iter << " reject " << hitStatus << "  "
                        << state.m_fitProcedureQuality->worstMeasurement()
                        << "  " << fitQuality->worst_hit_chi_squared();
      if (hitStatus == broad_strip)
        msg() << "  ratio " << ((**h).broadSigma() / (**h).sigma());
      if (reject && number_rejected == maxReject)
        msg() << "  number_rejected " << number_rejected;
      msg() << endmsg;
    }

    if (reject && ++number_rejected > maxReject) {
      fit = false;
    } else {
      (**h).status(hitStatus);
      ++iter;
    }
  }

  // set the remaining track requirements
  if (!state.m_fitCode) {
    scattererParameters = createScatterers(state);
    setResiduals(state);
  }
}

void
TrackFitter::fitWithResiduals(TrackStatus status,
                              FitQuality*& fitQuality,
                              PerigeeParameters& perigeeParameters,
                              parameter_vector*& scattererParameters,
                              hit_const_iterator begin,
                              hit_const_iterator end) const
{
  ATH_MSG_VERBOSE(" fitWithResiduals " << status);

  // ESD independent fit procedure
  State state;
  fitProcedure(state, status, perigeeParameters, begin, end);

  // for a good fit: store results back into input classes
  // otherwise quit in case of fit failure
  createQuality(state, fitQuality);
  if (!state.m_fitCode) {
    createPerigee(state, perigeeParameters);
    scattererParameters = createScatterers(state);
    setResiduals(state);
  }
}

void
TrackFitter::fitWithVertex(TrackStatus status,
                           FitQuality*& fitQuality,
                           PerigeeParameters& perigeeParameters,
                           const Trk::RecVertex& vertex,
                           hit_const_iterator begin,
                           hit_const_iterator end) const
{
  ATH_MSG_VERBOSE(" fitWithVertex " << status);
  State state;

  // include transverse vertex
  Amg::Vector3D delta = vertex.position() - perigeeParameters.vertex();
  double cosPhi = perigeeParameters.cosPhi();
  double sinPhi = perigeeParameters.sinPhi();
  double d0 = delta.y() * cosPhi - delta.x() * sinPhi;
  const AmgSymMatrix(3)& covariance = vertex.covariancePosition();
  double sigma = sqrt(sinPhi * sinPhi * covariance(0, 0) +
                      cosPhi * cosPhi * covariance(1, 1) -
                      2. * cosPhi * sinPhi * covariance(1, 0));
  state.m_vertexMeasurement =
    new Trk::FitMeasurement(d0, vertex.position(), sigma);

  // EDM independent fit procedure
  fitProcedure(state, status, perigeeParameters, begin, end);

  // store results back into input classes
  state.m_vertexMeasurement = 0;
  createQuality(state, fitQuality);
  createPerigee(state, perigeeParameters);
}
