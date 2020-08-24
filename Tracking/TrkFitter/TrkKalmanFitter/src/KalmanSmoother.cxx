/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanSmoother.cxx
//   Source file for class KalmanSmoother
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Markus.Elsing@cern.ch
///////////////////////////////////////////////////////////////////

#include "TrkKalmanFitter/KalmanSmoother.h"

#include "CLHEP/Matrix/Matrix.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkFitterUtils/DNA_MaterialEffects.h"
#include "TrkFitterUtils/ProtoTrajectoryUtility.h"
#include "TrkFitterUtils/ProtoMaterialEffects.h"

// common tracking tools
#include "TrkDetDescrInterfaces/IAlignableSurfaceProvider.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkFitterInterfaces/IDynamicNoiseAdjustor.h"
#include "TrkToolInterfaces/IUpdator.h"
#include <memory>

#include <sstream>
#include <iomanip>

// InterfaceID
// const InterfaceID& Trk::KalmanSmoother::interfaceID() { 
//	return InterfaceID_KalmanSmoother; 
//}

// constructor
Trk::KalmanSmoother::KalmanSmoother(const std::string& t,const std::string& n,const IInterface* p) :
  AthAlgTool (t,n,p),
  m_extrapolator(nullptr),
  m_extrapolationEngine(""),
  m_useExEngine(false),
  m_updator(nullptr),
  m_dynamicNoiseAdjustor(nullptr),
  m_alignableSurfaceProvider(nullptr),
  m_utility(nullptr),
  m_doSmoothing(true),
  m_calculateFitQuality{},
  m_initialErrors{},
  m_initialCovSeedFactor{},
  m_option_relErrorLimit{},
  m_fitStatistics{},
  m_particleMasses{}
{
  declareProperty("InitialCovarianceSeedFactor",m_initialCovSeedFactor=1000.,
                  "factor by which to initialise backward filter");
  declareProperty("RelativeInitialErrorLimit",m_option_relErrorLimit=10.0,
                  "limit by which a coordinate is deemed unconstraint and its cov entry reset instead of scaled");
  // the extrapolation engine
  declareProperty("ExtrapolationEngine",      m_extrapolationEngine);
  declareProperty("UseExtrapolationEngine",   m_useExEngine);
  
  declareInterface<IKalmanSmoother>( this );
}

// destructor
Trk::KalmanSmoother::~KalmanSmoother()
{}

// initialize
StatusCode Trk::KalmanSmoother::initialize()
{
  delete m_utility;
  m_utility = new ProtoTrajectoryUtility();
  ATH_MSG_INFO ("initial covariance inflated by factor     "<< m_initialCovSeedFactor);
  ATH_MSG_INFO ("forward-TP is weakly constraint if err is "<< m_option_relErrorLimit <<
                " x larger than start value");
  ATH_MSG_INFO ("initialize() successful in " << name());

  if (m_useExEngine) {
    ATH_CHECK(m_extrapolationEngine.retrieve());
  }
  std::vector<int> statVec(nStatIndex, 0);
  m_fitStatistics.resize(nFitStatusCodes, statVec);

  return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::KalmanSmoother::finalize()
{
    delete m_utility;
    if (msgLvl(MSG::INFO)) {
        
        int iw=9;
        std::stringstream ss;
        ss << "-------------------------------------------------------------------------------" << std::endl;
        ss << "  track fits by eta range          ------All---Barrel---Trans.-- Endcap-- " << std::endl;
        std::vector<std::string> statusNames{
         "  Number of smoother iterations   :",
         "  Number of successful iterations :",
         "  fits using straight track model :",
         "  Number of calls with bad input  :",
         "  Number of update failures       :",
         "  Number of fail. getting fit qual:",
         "  Number of missing covariances   :",
         "  Number of extrapolation failures:",
         "  extrapol fail with low momentum :",
         "  failed state combinations       :",
         "  fits w/ weakly constrained forw.:",
         "  forw pars outside surface bounds:",
        };
        size_t idx{0};
        for (const auto & thisName: statusNames) {
            ss << thisName << std::setiosflags(std::ios::dec) << std::setw(iw)
                    << (m_fitStatistics[idx])[iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                    << (m_fitStatistics[idx])[iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
                    << (m_fitStatistics[idx])[iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                    << (m_fitStatistics[idx])[iEndcap] << "\n";
            idx++;
        }
        ss << "-------------------------------------------------------------------------------\n";
        ATH_MSG_INFO (" Fitter statistics for " << name() <<"\n"<<ss.str());
    }
    ATH_MSG_INFO ("finalize() successful in " << name());
    return StatusCode::SUCCESS;
}
		
// configure the Kalman Smoother
// needs:   Propagator				- define which track model to be used for extrapolating tracks
//			Updator				    - defines the statistics for updating the estimator
StatusCode Trk::KalmanSmoother::configureWithTools(IExtrapolator* extrap,
                                                   const Trk::IUpdator* updator,
                                                   const Trk::IDynamicNoiseAdjustor* dna_tool,
                                                   const Trk::IAlignableSurfaceProvider* asp_tool,
                                                   const bool  doSmoothing,
                                                   const bool  calculateFitQuality)
{
  m_extrapolator         = extrap;
  m_updator              = updator;
  m_dynamicNoiseAdjustor = dna_tool;
  m_alignableSurfaceProvider = asp_tool;
  m_doSmoothing          = doSmoothing;
  m_calculateFitQuality  = calculateFitQuality;
  if (!m_updator) {
    ATH_MSG_ERROR ("configuration incomplete, no updator given.");
    return StatusCode::FAILURE;
  }
  if (!m_extrapolator) {
    ATH_MSG_ERROR ("configuration incomplete, no extrapolator given.");
    return StatusCode::FAILURE;
  }

  if (m_dynamicNoiseAdjustor) {
    ATH_MSG_INFO ("tool for DNA present, so dyn. noise adjustment inside Si detectors will be active!");
  }  
  
  m_initialErrors = m_updator->initialErrors();
  ATH_MSG_DEBUG ("queried current Updator for fitter initialisation. Result:");
  ATH_MSG_DEBUG ( m_initialErrors[0] << ", " << m_initialErrors[1]
        << ", " << m_initialErrors[2] << ", " << m_initialErrors[3] << ", "
        << m_initialErrors[4] );

  ATH_MSG_INFO ("configure() with extrapolator and updator");
  return StatusCode::SUCCESS;
}

// Kalman Smoother fit
// note regarding memory clean-up:
// a return StatusCode "failure" will (and must) trigger the calling fitter
// to clean up the trajectory vector.
Trk::FitterStatusCode Trk::KalmanSmoother::fit(Trk::Trajectory&              trajectory,
                                          Trk::FitQuality*&             trackFitQuality,
                                          const Trk::KalmanMatEffectsController& kalMec) const
{
  ATH_MSG_VERBOSE ("--> enter KalmanSmoother::fit");
  
  // protection against being unconfigured
  if (!m_updator) {
    ATH_MSG_ERROR ("need to first configure with updator");
    return FitterStatusCode::BadInput;
  }
  // protection against empty Trajectory
  if (trajectory.empty()) {
    ATH_MSG_INFO ("attempt to smooth empty forward trajectory");
    if (msgLvl(MSG::INFO)) monitorTrackFits( Call, 1000. );
    if (msgLvl(MSG::INFO)) monitorTrackFits( BadInput, 1000. );
    return FitterStatusCode::BadInput;
  }

  // take care of different fit Quality objects in place during smoothing:
  //
  Trk::FitQualityOnSurface* fitQual = nullptr;               // FQ of a given smoothed state
  Trk::FitQualityOnSurface* trackQualityIncrement = nullptr; // increment to build total fitQ
  if (trackFitQuality) {                               // object to hold total fit quality
    ATH_MSG_WARNING ("fitQuality pointer is not NULL, cancel Kalman fit to prevent mem leak!");
    if (msgLvl(MSG::INFO)) monitorTrackFits( Call, 1000. );
    if (msgLvl(MSG::INFO)) monitorTrackFits( BadInput, 1000. );
    return FitterStatusCode::BadInput;
  }
  double chi2Increment = 0.f;
  int    ndofIncrement = -5;  // five free parameters
 
  ////////////////////////////////////////////////////////////////////////////////////
  // get last MeasurementBase and perform the missing update 
  
  /*-- careful with STL reverse iterator: the underlying normal iterator points to
    a different element which is off by one, otherwise couldn't access rbegin() etc.
    Transformation (rit constructor or rit's base() method) needs offset by +/- 1.
  */
  Trk::Trajectory::reverse_iterator lastPredictedState
    = Trk::Trajectory::reverse_iterator(++(m_utility->lastFittableState(trajectory)));
  const MeasurementBase*	   fittableMeasurement = lastPredictedState->measurement();
  const TrackParameters*     forwardTPar  = lastPredictedState->forwardTrackParameters();
  int    previousStatePosOnTraj = lastPredictedState->positionOnTrajectory();
  ATH_MSG_VERBOSE ("create smoothed state at end of track by adding the last meas't");
  std::unique_ptr<const TrackParameters> smooPar;
  double smooPar_eta_for_monitoring=1000.;
  if (!fittableMeasurement || !forwardTPar) 
    m_utility->dumpTrajectory(trajectory, "DAF-inconsistency");
  // first smoothed TrkParameter is last forward prediction updated with last MBase
  else smooPar.reset(  m_updator->addToState(*forwardTPar,
					     fittableMeasurement->localParameters(),
					     fittableMeasurement->localCovariance(),
                                             fitQual) );
  if (msgLvl(MSG::INFO)) monitorTrackFits( Call, ( forwardTPar ? forwardTPar->eta() : 1000. ) );
  if(!smooPar) {
    ATH_MSG_WARNING ("first smoother update failed, reject track");
    if (msgLvl(MSG::INFO)) monitorTrackFits( UpdateFailure, ( forwardTPar ? forwardTPar->eta() : 1000. ) );
    return FitterStatusCode::UpdateFailure;
  }
  if(!fitQual) {
    ATH_MSG_WARNING ("no fit quality in spite of successfull update.");
    if (msgLvl(MSG::INFO)) monitorTrackFits( FitQualityFailure, ( forwardTPar ? forwardTPar->eta() : 1000. ) );
    return FitterStatusCode::FitQualityFailure;
  }
  lastPredictedState->checkinSmoothedPar(smooPar.release());
  lastPredictedState->checkinFitQuality(fitQual);
  if (!lastPredictedState->smoothedTrackParameters()->covariance()) {
    ATH_MSG_INFO ("forward kalman filter has no error - eject track");
    if (msgLvl(MSG::INFO)) monitorTrackFits( MissingCovariance, ( forwardTPar ? forwardTPar->eta() : 1000. ) );
    return FitterStatusCode::MissingCovariance; // delete smooPar via trajectory-clearing
  }
  if (msgLvl(MSG::DEBUG)) printGlobalParams(previousStatePosOnTraj, "+ init",
                                            lastPredictedState->forwardTrackParameters() );
  bool foundStraightTrack = this->straightTrackModel(*(lastPredictedState->smoothedTrackParameters()));
  if (foundStraightTrack) ++ndofIncrement;

  ////////////////////////////////////////////////////////////////////////////////////
  // start backward filtering
  
  // use result of forward kalman filter as initial prediction, but scale error matrix.

  AmgSymMatrix(5)* firstErrMtx = initialiseSmoother(*(lastPredictedState->smoothedTrackParameters())->covariance());
  const AmgVector(5)& par = lastPredictedState->smoothedTrackParameters()->parameters();
  std::unique_ptr<const TrackParameters> predPar( CREATE_PARAMETERS(*(lastPredictedState->smoothedTrackParameters()),par,firstErrMtx) ); 
  // The first step of backward-filtering is done before any loop because of the
  // specially formed prediction (from the last forward parameters).
  std::unique_ptr<const TrackParameters> updatedPar( m_updator->addToState(*predPar,
									   fittableMeasurement->localParameters(),
									   fittableMeasurement->localCovariance(),
                                                                           trackQualityIncrement) );
  if(!updatedPar || !trackQualityIncrement) {
    if (msgLvl(MSG::INFO)) monitorTrackFits( UpdateFailure, predPar->eta() );
    ATH_MSG_INFO (" update using initial measurement failed, reject track");
    return FitterStatusCode::UpdateFailure;
  }
  lastPredictedState->backwardStateChiSquared(trackQualityIncrement->chiSquared());
  addChi2IncrementAndDelete(trackQualityIncrement,chi2Increment,ndofIncrement);
  //    Trk::Surface& testSf = fittableMeasurement->associatedSurface();
  
  
  ////////////////////////////////////////////////////////////////////////////////////
  // now do the rest of the forward trajectory by means of a reverse iterated loop
  
  Trk::Trajectory::reverse_iterator rit = lastPredictedState + 1;
  Trk::Trajectory::reverse_iterator lastSmoothableState
    = Trk::Trajectory::reverse_iterator(m_utility->firstFittableState(trajectory)) - 1; // this takes outliers into account  
  for( ; rit!=trajectory.rend(); rit++) {
    if (!rit->isOutlier()) {
      smooPar_eta_for_monitoring = 1000.;

      fittableMeasurement = rit->measurement();
      
      if (msgLvl(MSG::DEBUG)) {
        printGlobalParams(previousStatePosOnTraj, " start", updatedPar.get() );
        // ATH_MSG_VERBOSE << "    Now trying to hit surface " << fittableMeasurement->associatedSurface() << endmsg;
        BoundaryCheck trackWithinSurface = true;
        //        if ( ! testSf.isOnSurface( updatedPar->position(), trackWithinSurface) ) 
        //  ATH_MSG_VERBOSE << "    previous updated parameters are outside surface bounds!" << endmsg;
        if ( ! fittableMeasurement->associatedSurface().isOnSurface( rit->forwardTrackParameters()->position(),
                                                                     trackWithinSurface) ) {
          ATH_MSG_VERBOSE ("    for information: forward-filtered pars are outside surface bounds!");
          if (msgLvl(MSG::INFO)) monitorTrackFits( ForwParOutsideSurfaceBounds, ( updatedPar ? updatedPar->eta() : 1000. ) );
          // ATH_MSG_VERBOSE (fittableMeasurement->associatedSurface());
        }
    }
    previousStatePosOnTraj = rit->positionOnTrajectory();
      const Trk::Surface& sf = m_alignableSurfaceProvider    ?
        m_alignableSurfaceProvider->retrieveAlignableSurface
        (fittableMeasurement->associatedSurface())           :
        fittableMeasurement->associatedSurface()             ;

      // now propagate updated TrkParameters to surface of ROT
      if (!m_useExEngine)
	predPar.reset(  m_extrapolator->extrapolate(*updatedPar, sf,
                                                    Trk::oppositeMomentum, // reverse filtering
                                                    false,                 // no boundary check
                                                    kalMec.particleType()) );
      else {
	ATH_MSG_DEBUG ("Smoother Kalman Fitter --> starting extrapolation engine");
	Trk::ExtrapolationCell <Trk::TrackParameters> ecc(*updatedPar, Trk::oppositeMomentum);
	ecc.setParticleHypothesis(kalMec.particleType());
	Trk::ExtrapolationCode eCode =  m_extrapolationEngine->extrapolate(ecc, &sf, false);
        
	if (eCode.isSuccess() && ecc.endParameters) {
	  ATH_MSG_DEBUG ("Smoother Kalman Fitter --> extrapolation engine success");
	  predPar.reset(ecc.endParameters);
	} else {
	  ATH_MSG_WARNING ("Smoother Kalman Fitter --> extrapolation engine did not succeed");
	}    
      }

      if(!predPar) {
        FitterStatusCode fsc = FitterStatusCode::ExtrapolationFailure;
        if (!foundStraightTrack && updatedPar->momentum().perp() < 150.) {
          ATH_MSG_DEBUG ("extrapolation during backward filtering failed "
                << "due to small momentum - reject track" );
          if (msgLvl(MSG::INFO)) monitorTrackFits( ExtrapolationFailureDueToSmallMomentum, ( updatedPar ? updatedPar->eta() : 1000. ) );
          fsc = FitterStatusCode::ExtrapolationFailureDueToSmallMomentum;
        } else {
          ATH_MSG_DEBUG ("extrapolation during backward filtering "
                << "failed, reject track (activate DEBUG for more info)" );
          if (msgLvl(MSG::INFO)) monitorTrackFits( ExtrapolationFailure, ( updatedPar ? updatedPar->eta() : 1000. ) );
          ATH_MSG_DEBUG ("the trajectory is supposed to be 'good' after the forward filter,");
          ATH_MSG_DEBUG ("so if this happens during backwards smoothing, the cause could be for example");
          ATH_MSG_DEBUG ("* bad backwards navigation - check tracking geometry code & setup!");
          ATH_MSG_DEBUG ("* bad input track - check if there are no duplicated hits or x-mirrored straws!");
          ATH_MSG_DEBUG ("* bad material densities or too low momentum estimate.");
        }
        return fsc;
      }

      ////////////////////////////////////////////////////////////////////
      // adjust the momentum + error according to target measurement (brem fit)
      const Trk::DNA_MaterialEffects* detectedMomentumNoise = nullptr;
      Trk::Trajectory::reverse_iterator stateWithNoise 
        = m_utility->previousFittableState(trajectory, rit);
      if (kalMec.doDNA() && stateWithNoise!=trajectory.rend()) {

        const TrackParameters *predPar_temp=predPar.release();
        const TrackParameters *updatedPar_temp=updatedPar.release();

        detectedMomentumNoise = 
          m_dynamicNoiseAdjustor->DNA_Adjust(predPar_temp, // change according to where meas is
                                             updatedPar_temp, // previous state's pars (start)
                                             fittableMeasurement, // the meas't
                                             kalMec,
                                             Trk::oppositeMomentum,
                                             stateWithNoise->dnaMaterialEffects());
        predPar.reset( predPar_temp);
        updatedPar.reset( updatedPar_temp);
      }
      if (msgLvl(MSG::DEBUG))
        printGlobalParams(rit->positionOnTrajectory(), "  pred", predPar.get(),
                          detectedMomentumNoise );
      // update track parameters (allows for preset LR solution for straws)
      updatedPar.reset(  m_updator->addToState(*predPar, fittableMeasurement->localParameters(),
                                               fittableMeasurement->localCovariance(),
                                               trackQualityIncrement) );
      
      if (!updatedPar || !trackQualityIncrement) {
        if (msgLvl(MSG::INFO)) monitorTrackFits( UpdateFailure, predPar->eta() );
        ATH_MSG_INFO ("could not update Track Parameters, reject track");
        delete trackQualityIncrement;
        return FitterStatusCode::UpdateFailure;
      }

      /* for smoothed trajectories the total chi2 can not be calculated from the
	 FitQualityOnSurface information: this is mathematically not correct since
	 the chi2 at the surfaces are correlated. Use fitQuality increment known
	 inside the smoother interface instead. See Fruehwirth [1987]. */
      rit->backwardStateChiSquared(trackQualityIncrement->chiSquared());
      addChi2IncrementAndDelete(trackQualityIncrement,chi2Increment,ndofIncrement);

      // smoothed parameters = combination of predicted (!) forward and updated (!) backward state
      // remember: first TrkParameter on ForwardTrajectory has no error matrix !!!
      
      if (rit == lastSmoothableState) { // at the last don't do state combination.
        ATH_MSG_VERBOSE ("Identified state" << (rit->positionOnTrajectory()>9? " " : " 0")<<
                         rit->positionOnTrajectory() << " as last fittable state.");
        smooPar = std::move( updatedPar );
      } else {
        if (m_doSmoothing) {
          forwardTPar = rit->forwardTrackParameters();
          smooPar.reset(  m_updator->combineStates( *forwardTPar, *updatedPar) );
        } else {
          smooPar.reset( updatedPar->clone() );
        }
      }
      if (!smooPar) {
        ATH_MSG_INFO ("could not combine Track Parameters, reject track");
        if (msgLvl(MSG::INFO)) monitorTrackFits( CombineStatesFailure, ( updatedPar ? updatedPar->eta() : 1000. ) );
        return FitterStatusCode::CombineStatesFailure;
      }
      // get FitQualityOnSurface
      fitQual    = const_cast<Trk::FitQualityOnSurface*>(m_updator->fullStateFitQuality
							 ( *smooPar, fittableMeasurement->localParameters(),
							   fittableMeasurement->localCovariance() ));
      if (!fitQual) {
        // can't allow NULL pointing fitquality with a measurement state,
        // since downstream logics will use fitqualities it all over the place.
        ATH_MSG_WARNING ("NULL FitQuality*, should not happen during smoothing.");
        if (msgLvl(MSG::INFO)) monitorTrackFits( FitQualityFailure, ( smooPar ? smooPar->eta() : 1000. ) );
        return FitterStatusCode::FitQualityFailure;
      }
      // write the smoothed state onto the internal Trajectory data format
      // remember smooPar eta for monitoring
      smooPar_eta_for_monitoring = smooPar->eta();
      rit->checkinSmoothedPar(smooPar.release() );
      rit->checkinFitQuality(fitQual);
      if (msgLvl(MSG::DEBUG)) printGlobalParams(previousStatePosOnTraj, "  smoo", rit->smoothedTrackParameters() );
      // write also possible material effects onto Trajectory
      if (detectedMomentumNoise) {
        if (stateWithNoise->dnaMaterialEffects())
          delete stateWithNoise->checkoutDNA_MaterialEffects();
        stateWithNoise->checkinDNA_MaterialEffects(detectedMomentumNoise);
      }
    } // end if not an outlier
    
  } // end loop over trajectory states
  
  ATH_MSG_VERBOSE ("-S- smoothed trajectory created ");
  
  
  // we made it
  trackFitQuality = new Trk::FitQuality(chi2Increment,ndofIncrement);
  if (msgLvl(MSG::INFO)) monitorTrackFits( Success, smooPar_eta_for_monitoring) ;
  return FitterStatusCode::Success;
}

Trk::FitterStatusCode Trk::KalmanSmoother::fitWithReference(Trk::Trajectory&              trajectory,
                                                            Trk::FitQuality*&             trackFitQuality,
                                                            const Trk::KalmanMatEffectsController& controlledMatEffects) const
{
  ATH_MSG_VERBOSE ("--> enter KalmanSmoother::fitWithRef");
  if (msgLvl(MSG::INFO)) monitorTrackFits( Call, 1000. );
  // protection against empty Trajectory
  if (trajectory.empty()) {
    ATH_MSG_INFO ("attempt to smooth empty forward trajectory");
    if (msgLvl(MSG::INFO)) monitorTrackFits( BadInput, 1000. );
    return FitterStatusCode::BadInput;
  }

  // take care of different fit Quality objects in place during smoothing:
  //
  Trk::FitQualityOnSurface* fitQual = nullptr;               // FQ of a given smoothed state
  Trk::FitQualityOnSurface* trackQualityIncrement = nullptr; // increment to build total fitQ
  if (trackFitQuality) {                               // object to hold total fit quality
    ATH_MSG_WARNING ("fitQuality pointer is not NULL, cancel Kalman fit to prevent mem leak!");
    if (msgLvl(MSG::INFO)) monitorTrackFits( BadInput, 1000. );
    return FitterStatusCode::BadInput;
  }
  double chi2Increment = 0.0;
  int    ndofIncrement = -5;  // five free parameters
 
  ////////////////////////////////////////////////////////////////////////////////////
  // get last MeasurementBase and perform the missing update 
  
  /*-- careful with STL reverse iterator: the underlying normal iterator points to
    a different element which is off by one, otherwise couldn't access rbegin() etc.
    Transformation (rit constructor or rit's base() method) needs offset by +/- 1.
  */
  Trk::Trajectory::reverse_iterator lastPredictedState
    = Trk::Trajectory::reverse_iterator(++(m_utility->lastFittableState(trajectory)));
  if (!lastPredictedState->measurement() || !lastPredictedState->parametersDifference() ||
      !lastPredictedState->referenceParameters()) {
    m_utility->dumpTrajectory(trajectory, "code inconsistency");
    return FitterStatusCode::BadInput;
  }

  const AmgVector(5)& forwardDiffPar = *lastPredictedState->parametersDifference();
  const AmgSymMatrix(5)& forwardCov  = *lastPredictedState->parametersCovariance();
  const Trk::MeasurementBase* lastMeasurement = lastPredictedState->measurement();
  int    previousStatePosOnTraj = lastPredictedState->positionOnTrajectory();
  ATH_MSG_VERBOSE ("create smoothed state at end of track by adding the last meas't");
  std::unique_ptr<const TrackParameters>  smooPar;
  // first smoothed TrkParameter is last forward prediction updated with last MBase
  std::unique_ptr<std::pair<AmgVector(5),AmgSymMatrix(5)> > updatedDifference (
    m_updator->updateParameterDifference(forwardDiffPar, forwardCov,
                                         *(lastPredictedState->measurementDifference()),
                                         lastMeasurement->localCovariance(),
                                         lastMeasurement->localParameters().parameterKey(),
                                         fitQual, /*doFQ=*/true ) );
  const AmgVector(5) x = lastPredictedState->referenceParameters()->parameters()
                       + updatedDifference->first;
  smooPar.reset(  updatedDifference? CREATE_PARAMETERS(*lastPredictedState->referenceParameters(),
                                                       x,new AmgSymMatrix(5)(updatedDifference->second)) : nullptr );
  if (msgLvl(MSG::INFO)) monitorTrackFits( Call, ( smooPar ? smooPar->eta() : 1000. ) );
  if (!smooPar || !fitQual) {
    ATH_MSG_WARNING ("first smoother update failed, reject track");
    if (msgLvl(MSG::INFO)) monitorTrackFits( UpdateFailure, 
                                             lastPredictedState->referenceParameters()->eta() );
    delete fitQual;
    return FitterStatusCode::UpdateFailure;
  }
  lastPredictedState->checkinSmoothedPar(smooPar.release() );
  lastPredictedState->checkinFitQuality(fitQual);
  if (msgLvl(MSG::DEBUG)) printGlobalParams(previousStatePosOnTraj, "+ init",lastPredictedState->smoothedTrackParameters());
  if (this->straightTrackModel(*(lastPredictedState->smoothedTrackParameters()))) ++ndofIncrement;

  ////////////////////////////////////////////////////////////////////////////////////
  // start backward filtering

// updatedDifference->first  const AmgVector(5)& par = lastPredictedState->smoothedTrackParameters()->parameters();
  // The first step of backward-filtering is done before any loop because of the
  // specially formed prediction: result of FKF with scaled error matrix
  AmgSymMatrix(5)* firstErrMtx = initialiseSmoother(*(lastPredictedState->smoothedTrackParameters()->covariance()));
  AmgVector(5)     firstDiff   = updatedDifference->first; // make copy and delete
  updatedDifference.reset( 
    m_updator->updateParameterDifference(firstDiff, *firstErrMtx,
					 *(lastPredictedState->measurementDifference()),
					 lastMeasurement->localCovariance(),
					 lastMeasurement->localParameters().parameterKey(),
                                         trackQualityIncrement, /*doFQ=*/true ) );
  if(!updatedDifference || !trackQualityIncrement) {
    if (msgLvl(MSG::INFO)) monitorTrackFits( UpdateFailure, lastPredictedState->referenceParameters()->eta() );
    ATH_MSG_INFO (" update using initial measurement failed, reject track");
    delete trackQualityIncrement;
    return FitterStatusCode::UpdateFailure;
  }
  lastPredictedState->backwardStateChiSquared(trackQualityIncrement->chiSquared());
  addChi2IncrementAndDelete(trackQualityIncrement,chi2Increment,ndofIncrement);
  
  double smooPar_eta_for_monitoring=1000.;
  ////////////////////////////////////////////////////////////////////////////////////
  // now do the rest of the forward trajectory by means of a reverse iterated loop
  Trk::Trajectory::reverse_iterator rit = lastPredictedState + 1; // go to one-but-last state
  Trk::Trajectory::reverse_iterator lastSmoothableState
    = Trk::Trajectory::reverse_iterator(m_utility->firstFittableState(trajectory)) - 1; // this takes outliers into account
  for( ; rit!=trajectory.rend(); rit++) {
      
    smooPar_eta_for_monitoring=1000.;

    ATH_MSG_VERBOSE ("Now inverting Jacobian... (pointer is "<<(rit->jacobian()?"OK":"NULL")<<")");
    AmgMatrix(5,5) invJac = rit->jacobian()->inverse(); // we go BACK
    // FIXME do we need inverse().transpose() ?
    previousStatePosOnTraj = rit->positionOnTrajectory();

    AmgVector(5) predDiffPar(invJac*(updatedDifference->first));
    AmgSymMatrix(5) predCov(invJac*(updatedDifference->second)*invJac.transpose());
    // add uncertainties from material effects:
    if (rit->materialEffects()) {
      predCov(Trk::phi,Trk::phi)      += std::pow(rit->materialEffects()->sigmaDeltaPhi(),2);
      predCov(Trk::theta,Trk::theta)  += std::pow(rit->materialEffects()->sigmaDeltaTheta(),2);
      // TODO: check: neglect mass here?
      const double& mass = m_particleMasses.mass[controlledMatEffects.particleType()];
      const double currQOverPsquared = std::pow((predDiffPar)(Trk::qOverP) + rit->referenceParameters()->parameters()[Trk::qOverP], 2);
      // sigmaDeltaQoverP^2 = sigmaDeltaE^2 * E^2 / p^6 = sigmaDeltaE^2 * (m^2 + p^2) / p^6
      const double sigmaDeltaQoverPsquared = (currQOverPsquared > 0.) ? (std::pow(rit->materialEffects()->sigmaDeltaE(), 2) * (mass*mass + 1./currQOverPsquared) * (currQOverPsquared*currQOverPsquared*currQOverPsquared)) : 0.;
      predCov(Trk::qOverP,Trk::qOverP)+= sigmaDeltaQoverPsquared;
      ATH_MSG_VERBOSE ("mass=" << mass << ", qOverP_ref="<< std::scientific << rit->referenceParameters()->parameters()[Trk::qOverP]
                << ", qOverP_diff=" << (predDiffPar)(Trk::qOverP)
                << ", sigmaDeltaE=" << rit->materialEffects()->sigmaDeltaE()
                << ", sigmaDeltaQoverP=" << sqrt(sigmaDeltaQoverPsquared) << std::fixed);//std::defaultfloat);
      ATH_MSG_VERBOSE ("Added material effects.");      
    }
    /* possibly check quality of backward "extrapolation", if not good return a
       monitorTrackFits( FitterStatusCode::ExtrapolationFailure, eta) or 
       monitorTrackFits( ExtrapolationFailureDueToSmallMomentum, TP->eta())
     */
    updatedDifference.reset();
    if (msgLvl(MSG::DEBUG)) {
      const AmgVector(5) x = rit->referenceParameters()->parameters()+predDiffPar;
      const Trk::TrackParameters* param = CREATE_PARAMETERS(*rit->referenceParameters(),x,nullptr);
      printGlobalParams( rit->positionOnTrajectory(), " extrp", param );
      delete param;
    }

    const MeasurementBase* fittableMeasurement = rit->measurement();
    if (!fittableMeasurement || rit->isOutlier() ) { // pure material state or outlier
      updatedDifference = std::make_unique<std::pair<AmgVector(5),AmgSymMatrix(5)>>(  std::make_pair(predDiffPar,predCov) );
    } else {
      updatedDifference.reset( 
        m_updator->updateParameterDifference(predDiffPar, predCov,
                                               *(rit->measurementDifference()),
                                               fittableMeasurement->localCovariance(),
                                               fittableMeasurement->localParameters().parameterKey(),
                                             trackQualityIncrement, /*doFQ=*/true ) );
      if (!updatedDifference || trackQualityIncrement == nullptr) {
        if (msgLvl(MSG::INFO)) monitorTrackFits( UpdateFailure, rit->referenceParameters()->eta() );
        ATH_MSG_INFO ("could not update Track Parameters, reject track");
        delete trackQualityIncrement;
        return FitterStatusCode::UpdateFailure;
      }
      const float updatedQoverP = (rit->referenceParameters()->parameters()[Trk::qOverP]) + (updatedDifference->first[Trk::qOverP]);
      if (fabs(updatedQoverP) > 0.1) {
            delete trackQualityIncrement;
            if (msgLvl(MSG::INFO)) monitorTrackFits( UpdateFailure, rit->referenceParameters()->eta() );
            ATH_MSG_INFO("could not update Track Parameters, momentum too low: qoverp=" << updatedQoverP);
            return FitterStatusCode::UpdateFailure;
      }

      /* for smoothed trajectories the total chi2 can not be calculated from the
         FitQualityOnSurface information: this is mathematically not correct since
         the chi2 at the surfaces are correlated. Use fitQuality increment known
         inside the smoother interface instead. See Fruehwirth [1987]. */
      rit->backwardStateChiSquared(trackQualityIncrement->chiSquared());
      addChi2IncrementAndDelete(trackQualityIncrement,chi2Increment,ndofIncrement);

      if (msgLvl(MSG::DEBUG)) {
        const AmgVector(5) x = rit->referenceParameters()->parameters()
                             + updatedDifference->first;
        const Trk::TrackParameters* param = CREATE_PARAMETERS(*rit->referenceParameters(),x,nullptr);
        printGlobalParams (rit->positionOnTrajectory()," updat", param );
        delete param;
      }
    } // end if there is a measurement
    // FIXME check carefully whether current logic is correct to stop measurement-specific
    // case here and whether to produce smoothed states for meas AND material states.

    // smoothed parameters = combination of predicted (!) forward and updated (!) backward state
    // remember: first TrkParameter on ForwardTrajectory has no error matrix !!!
      
    if (rit == lastSmoothableState) { // at the last don't do state combination.
      ATH_MSG_VERBOSE ("Identified state" << (rit->positionOnTrajectory()>9? " " : " 0")<<
                       rit->positionOnTrajectory() << " as last fittable state.");
      smooPar.reset( CREATE_PARAMETERS(*rit->referenceParameters(),
                                     (rit->referenceParameters()->parameters()+updatedDifference->first),
                                       new AmgSymMatrix(5)(updatedDifference->second)) );
      rit->checkinSmoothedPar(smooPar.release() );
    } else if (m_doSmoothing) {
      std::unique_ptr< std::pair<AmgVector(5),AmgSymMatrix(5)> > smoothedDifference(
        m_updator->updateParameterDifference(*(rit->parametersDifference()),
                                             *(rit->parametersCovariance()),
                                             updatedDifference->first,updatedDifference->second,31,
                                             fitQual, /*doFQ=*/false ) );
      // alternative would be to construct full pars, then call m_updator->combineStates
      if (!smoothedDifference) {
        ATH_MSG_INFO ("could not combine Track Parameters, reject track");
        if (msgLvl(MSG::INFO)) monitorTrackFits(CombineStatesFailure, rit->referenceParameters()->eta());
        return FitterStatusCode::CombineStatesFailure;
      }
      const AmgVector(5) x = rit->referenceParameters()->parameters()
                           + smoothedDifference->first;
      smooPar.reset( CREATE_PARAMETERS(*rit->referenceParameters(),x,
                                       new AmgSymMatrix(5)(smoothedDifference->second)) );
      rit->checkinSmoothedPar(smooPar.release() );
    } else {
      smooPar.reset();
      ATH_MSG_VERBOSE ("No fitted track parameters made at this state because doSmoothing is OFF");
    }
    // get FitQualityOnSurface
    if (rit->smoothedTrackParameters() && fittableMeasurement) {
      fitQual    = const_cast<Trk::FitQualityOnSurface*>
        ( m_updator->fullStateFitQuality( *(rit->smoothedTrackParameters()), fittableMeasurement->localParameters(),
                                          fittableMeasurement->localCovariance() ));
      if (!fitQual) {
        // can't allow NULL pointing fitquality with a measurement state,
        // since downstream logics will use fitqualities it all over the place.
        ATH_MSG_INFO ("NULL FitQuality*, should not happen during smoothing.");
        if (msgLvl(MSG::INFO)) monitorTrackFits( FitQualityFailure, rit->smoothedTrackParameters()->eta());
        return FitterStatusCode::FitQualityFailure;
      }
      rit->checkinFitQuality(fitQual);fitQual=nullptr;
    }
    if (rit->smoothedTrackParameters()) {
      smooPar_eta_for_monitoring = rit->smoothedTrackParameters()->eta();
    }
    if (rit->smoothedTrackParameters() && msgLvl(MSG::DEBUG)) printGlobalParams(previousStatePosOnTraj, "  smoo", rit->smoothedTrackParameters() );
    if (rit== lastSmoothableState) break; // if first state is outlier, loop will malfunction

  } // end loop over trajectory states
  
  ATH_MSG_VERBOSE ("-S- smoothed trajectory created ");
  
  // cleanup
  
  // we made it
  trackFitQuality = new Trk::FitQuality(chi2Increment,ndofIncrement);
  if (msgLvl(MSG::INFO)) monitorTrackFits( Success, smooPar_eta_for_monitoring );
  return FitterStatusCode::Success;
}

// Seed the backward filter by taking the original cov matrix and
// - strip away the off-diagonal elements
// - inflate the errors by a factor, which is tuned to the specific detector.
AmgSymMatrix(5)* Trk::KalmanSmoother::initialiseSmoother(const AmgSymMatrix(5)& err) const {
  std::vector<bool> isConstraint(5);
  AmgSymMatrix(5)* cov = new AmgSymMatrix(5)(); // a 5x5 0-matrix
  cov->setZero();
  for (int i=0; i<5; ++i) {
    isConstraint[i] = (m_option_relErrorLimit* sqrt(err(i,i)) < m_initialErrors[i]);
    (*cov)(i,i) = isConstraint[i] ? err(i,i)*m_initialCovSeedFactor : m_initialErrors[i]*m_initialErrors[i];
  }
  if (msgLvl(MSG::DEBUG)) {
    bool weakParIsDetected=false;
    for (int i=0; i<5; ++i) {
      if (!isConstraint[i] && weakParIsDetected) msg(MSG::VERBOSE) << i << " ";
      if (!isConstraint[i] && !weakParIsDetected) {
        msg(MSG::VERBOSE) << "Some parameters are only weakly constraint "
              << "after fwd.Filter: " << i << " ";
        weakParIsDetected = true;
        if (msgLvl(MSG::INFO)) monitorTrackFits( WeaklyConstraintForwardPar, 1000. );
      }
    }
    if (weakParIsDetected) msg(MSG::VERBOSE) << endmsg;
  }
  return cov;
}


AmgSymMatrix(5)* Trk::KalmanSmoother::initialiseSmootherFixed(const AmgSymMatrix(5)&) const {
  AmgSymMatrix(5)* cov = new AmgSymMatrix(5)();
  cov->setZero();
  for (int i=0; i<5; ++i)
    (*cov)(i,i) = m_initialErrors[i]*m_initialErrors[i];
  return cov;
}

// helper function to find out if the track model is straight line or curved
bool Trk::KalmanSmoother::straightTrackModel(const Trk::TrackParameters& TP) const {
  double momentumCorrelnTest = (*TP.covariance())(4,0) // cov=NULL case already rejected
    + (*TP.covariance())(4,1)
    + (*TP.covariance())(4,2)
    + (*TP.covariance())(4,3);
  bool straightTrackModel = (fabs(momentumCorrelnTest) < 1.0E-100);
  if (straightTrackModel) {
    ATH_MSG_DEBUG ("Straight track model identified => reduce track's ndof to 4");
    if (msgLvl(MSG::INFO)) monitorTrackFits( StraightTrackModelUsed, TP.eta() );
  }
  return straightTrackModel;
}


// private -- helper to add an update-increment to the total chi2
void Trk::KalmanSmoother::addChi2IncrementAndDelete(Trk::FitQualityOnSurface*& fitQualIncrement,
                                                    double& totalChi2, int& totalNdof) const
{
  if (fitQualIncrement) {
    totalChi2 += fitQualIncrement->chiSquared();
    totalNdof += fitQualIncrement->numberDoF();
    delete fitQualIncrement;
    fitQualIncrement = nullptr;
  } else if (msgLvl(MSG::INFO)) {
    ATH_MSG_DEBUG ("incremental fitQuality object does not exist, assuming zero increment.");
  }
}

// private -- helper to make pretty debug output
void Trk::KalmanSmoother::printGlobalParams(int istate, const std::string& ptype,
                                            const Trk::TrackParameters* param,
                                            const Trk::DNA_MaterialEffects* mefot) const
{
  char tt[80]; sprintf(tt,"T%.2u",istate);
  if (not (msgLvl(MSG::VERBOSE))) return;
  msg(MSG::VERBOSE) << tt << ptype << " GP:" 
        << std::setiosflags(std::ios::fixed | std::ios::showpoint | std::ios::right )
        << std::setw(9) << std::setprecision(2) << param->position()[0]
        << std::setw(9) << std::setprecision(2) << param->position()[1]
        << std::setw(9) << std::setprecision(2) << param->position()[2]
        << " -- GM:" <<std::resetiosflags(std::ios::showpoint) << std::setiosflags( std::ios::right )
        << std::setw(8) << std::setprecision(0) << param->momentum()[0]
        << std::setw(8) << std::setprecision(0) << param->momentum()[1]
        << std::setw(8) << std::setprecision(0) << param->momentum()[2]
        << std::setprecision(6) << endmsg;
  if (mefot)
    //    ATH_MSG_VERBOSE << "Mefot found with " << *mefot << endmsg;
    msg(MSG::VERBOSE) << "-S- DNA kicked in at t/X0 of " <<mefot->thicknessInX0()
          << " with noise "  << mefot->addSigmaQoverP() << endmsg;
}

void Trk::KalmanSmoother::monitorTrackFits(FitStatusCodes code, const double& eta) const {
    ((m_fitStatistics[code])[iAll])++;
    if (fabs(eta) < 0.80 ) ((m_fitStatistics[code])[iBarrel])++;
    else if (fabs(eta) < 1.60) ((m_fitStatistics[code])[iTransi])++;
    else if (fabs(eta) < 2.50) ((m_fitStatistics[code])[iEndcap])++;
}
