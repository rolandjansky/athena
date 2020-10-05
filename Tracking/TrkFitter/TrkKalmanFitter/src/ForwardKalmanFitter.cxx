/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ForwardKalmanFitter.cxx
//   Source file for class ForwardKalmanFitter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Markus.Elsing@cern.ch
///////////////////////////////////////////////////////////////////

#include "TrkKalmanFitter/ForwardKalmanFitter.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterUtils/FitterStatusCode.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkFitterUtils/FitterStatusCode.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkFitterUtils/DNA_MaterialEffects.h"
#include "TrkFitterUtils/ProtoTrajectoryUtility.h"
#include "TrkFitterUtils/SensorBoundsCheck.h"

// common tracking tools
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkFitterInterfaces/IDynamicNoiseAdjustor.h"
#include "TrkDetDescrInterfaces/IAlignableSurfaceProvider.h"
#include "TrkFitterInterfaces/IMeasurementRecalibrator.h"
#include "TrkFitterInterfaces/IKalmanPiecewiseAnnealingFilter.h"

// temp. fix for perigee not extrapolated to straight line
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/StraightLineSurface.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include <cmath>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Gaudi AlgTool control (constructor, initialise ...)                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
Trk::ForwardKalmanFitter::ForwardKalmanFitter(const std::string& t,const std::string& n,const IInterface* p) :
  AthAlgTool (t,n,p),
  m_extrapolator(nullptr),
  m_extrapolationEngine(""),
  m_useExEngine(false),
  m_updator(nullptr),
  m_ROTcreator(nullptr),
  m_dynamicNoiseAdjustor(nullptr),
  m_alignableSurfaceProvider(nullptr),
  m_recalibrator(nullptr),
  m_internalDAF(nullptr),
  m_idHelper(nullptr),
  m_utility(nullptr),
  m_StateChiSquaredPerNumberDoFPreCut{}
{
  // AlgTool stuff
  declareInterface<IForwardKalmanFitter>( this );
  
  // declare all properties needed to configure fitter, defaults
  declareProperty("StateChi2PerNDFPreCut", m_StateChiSquaredPerNumberDoFPreCut=50.f,
                  "coarse pre-cut on the predicted state's chi2/ndf against outliers destabilising the filter - only in effect if called with runOutlier true");
  // the extrapolation engine
  declareProperty("ExtrapolationEngine",      m_extrapolationEngine);
  declareProperty("UseExtrapolationEngine",   m_useExEngine);
}

// destructor
Trk::ForwardKalmanFitter::~ForwardKalmanFitter(){}

// initialize
StatusCode Trk::ForwardKalmanFitter::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  ATH_CHECK(detStore()->retrieve(m_idHelper, "AtlasID"));
  m_utility = new ProtoTrajectoryUtility(m_idHelper);
  if (m_useExEngine) {
    ATH_CHECK(m_extrapolationEngine.retrieve());
  }
  ATH_MSG_INFO ("stability precut on state Chi2/ndf set to "<< m_StateChiSquaredPerNumberDoFPreCut );
  return sc;
}

// finalize
StatusCode Trk::ForwardKalmanFitter::finalize()
{
  delete m_utility;
  ATH_MSG_INFO ("finalize() successful in " << name());
  return StatusCode::SUCCESS;
}
		
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// configure the Forward Filter
// needs: Extrapolator       - configurable tool to wrap parameter propagation
//        Updator            - statistics for updating the estimator
//        RIO_OnTrackCreator - correct Clusters and DriftCircles using track predictions
//        DynamicNoiseAdjustor - adapts the momentum if strong brem in Silicon
//
////////////////////////////////////////////////////////////////////////////////
StatusCode Trk::ForwardKalmanFitter::configureWithTools(const Trk::IExtrapolator* extrap,
                                                        const Trk::IUpdator* updator,
                                                        const Trk::IRIO_OnTrackCreator* rot_crea,
                                                        const Trk::IDynamicNoiseAdjustor* dna_tool,
                                                        const Trk::IAlignableSurfaceProvider* asp_tool,
                                                        const Trk::IMeasurementRecalibrator* mr,
                                                        const Trk::IKalmanPiecewiseAnnealingFilter* kpaf)
{
  m_extrapolator = extrap;
  m_updator      = updator;
  m_ROTcreator   = rot_crea;
  m_dynamicNoiseAdjustor = dna_tool;
  m_alignableSurfaceProvider = asp_tool;
  m_recalibrator = mr;
  m_internalDAF  = kpaf;
  
  // protection, if not confiured
  if (!m_updator) {
    ATH_MSG_ERROR ("Updator missing, need to configure with it !");
    return StatusCode::FAILURE;
  }
  if (!m_extrapolator) {
    ATH_MSG_ERROR ("Extrapolator missing, need to configure with it !");
    return StatusCode::FAILURE;
  }
  auto toolPresent = [] (const bool yn){
    return std::string((yn ? "present\n" : "none\n"));
  };
  std::string message="Configuring " + name() + " with tools from KalmanFitter\n";
  message += "Updator and Extrapolator    - present\n";
  message += "Dyn.NoiseAdjustment Pix/SCT - " + toolPresent(m_dynamicNoiseAdjustor);
  message += "General RIO_OnTrackCreator  - " + toolPresent(m_ROTcreator);
  message += "RIO_OnTrack Recalibrator    - " + toolPresent(m_recalibrator);
  message += "Internal Piecewise DAF      - " + toolPresent(m_internalDAF);
  if (m_alignableSurfaceProvider)
    message += "Ext. for alignable Surfaces - present\n";
  ATH_MSG_INFO(message);
  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// forward fit on set of PrepRawData                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

Trk::FitterStatusCode 
Trk::ForwardKalmanFitter::fit(Trk::Trajectory& trajectory,
                              const Trk::PrepRawDataSet& inputPRDColl,
                              const Trk::TrackParameters& estParamNear0,
                              const Trk::RunOutlierRemoval  runOutlier,
                              const Trk::KalmanMatEffectsController& controlledMatEffects) const
{
  if (!trajectory.empty()) {
    ATH_MSG_WARNING  ("internal error: for the PRD-based interface the Trajectory needs to be empty!");
    return FitterStatusCode::BadInput;
  }
  if (!m_ROTcreator) { // for other interfaces it is OK to work without ROT creator
    ATH_MSG_WARNING  ("internal error: KF can not fit on PRD without a RIO_OnTrackCreator tool!");
    return FitterStatusCode::BadInput;
  }
  
  ATH_MSG_VERBOSE ("Prepare the old start parameters...");
  // copy input TrkParameter into prediction without Errors
  const TrackParameters* updatedPar = estParamNear0.clone();
  Trk::ProtoTrackStateOnSurface* bremStateIfBremFound = nullptr;
  
  // loop over all PreRawData objects in Set
  int itcounter=1;
  PrepRawDataSet::const_iterator it    = inputPRDColl.begin();
  for(  ; it!=inputPRDColl.end(); it++) {
    const Trk::TrackParameters* predPar =
      this->predict( updatedPar, 
                     (*it)->detectorElement()->surface( (*it)->identify() ),
                     controlledMatEffects, itcounter, itcounter);

    if(!predPar) {
      // with no TP calculated, the fitter can't call the ROT creator -> kill this state!
      ATH_MSG_DEBUG ( (itcounter<10?"T0":"T") << " --- missed surface with "
                      << "extrapolation, remove PrepRawData object from input --- ");
      continue;
    }

    ATH_MSG_VERBOSE ("create RIO_OnTrack using prediction parameters " << *predPar);
    const Trk::RIO_OnTrack* fittableMeasurement = m_ROTcreator->correct(**it,*predPar);
    if (!fittableMeasurement){
      ATH_MSG_WARNING ("The RIO_OnTrack creation has not succeeded, this PrepRawData"
                       << "will be lost in the track fit." );
    }

    Trk::ProtoTrackStateOnSurface predictedState = 
      Trk::ProtoTrackStateOnSurface(fittableMeasurement,false,true,itcounter);
    trajectory.push_back(predictedState);
    if (runOutlier) m_utility->identifyMeasurement(predictedState);
    Trk::Trajectory::iterator newestState = trajectory.end()-1;

    FitterStatusCode stepFwF =
      this->buildAndAnalyseTrajectory(trajectory,newestState,updatedPar,predPar,
                                      controlledMatEffects,itcounter,
                                      bremStateIfBremFound,false /*no recalibrate*/);
    if (stepFwF.isFailure()) { 
      delete updatedPar; delete predPar; return stepFwF;
    }
    stepFwF = this->updateOrSkip(newestState,updatedPar,predPar,itcounter,
                                 runOutlier,bremStateIfBremFound);
    bremStateIfBremFound = nullptr; 
    if (stepFwF.isFailure()) { delete updatedPar; return stepFwF; }
    ++itcounter;

  }
  delete updatedPar; updatedPar = nullptr;     // clean up
  int testNumber = m_utility->rankedNumberOfMeasurements(trajectory);
  if (testNumber < 5) {
    ATH_MSG_DEBUG ("Filtered trajectory has only "<<testNumber<<" and thus too few fittable meas'ts!");
    return FitterStatusCode::FewFittableMeasurements; // master tool (KalmanFitter) will clean up the trajectory...
  }
  return FitterStatusCode::Success;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// forward fit on a set of MeasurementBase                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
Trk::FitterStatusCode
Trk::ForwardKalmanFitter::fit(Trk::Trajectory& trajectory,
                              const Trk::TrackParameters& referenceParameters,
                              const Trk::RunOutlierRemoval  runOutlier,
                              const Trk::KalmanMatEffectsController& controlledMatEffects,
                              const bool allowRecalibrate,
                              const int  filterStartState) const
{
  int itcounter=0;
  if (trajectory.empty()) {
    ATH_MSG_WARNING ("internal error: empty trajectory!");
    return FitterStatusCode::BadInput;
  }
  if (allowRecalibrate) m_utility->identifyMeasurements(trajectory);
  Trk::Trajectory::iterator it = m_utility->firstFittableState(trajectory);
  const TrackParameters* updatedPar = nullptr;        // delete & remake during filter
  Trk::ProtoTrackStateOnSurface* bremStateIfBremFound = nullptr;
  ATH_MSG_DEBUG ("-F- entering FwFilter with matEff="<<controlledMatEffects.particleType()<<
                 ", "<<(allowRecalibrate?"recalibrate:yes":"recalibrate:no")<<
                 " and start state "<<filterStartState);

  //////////////////////////////////////////////////////////////////////////////////////////
  /* the next lines bring the filter in position, espec. when resuming during iterations:

     - start 0 "initialise"  -> use par values (extrap to 1st meas) and huge cov from updator
     - start 1 "continue"    -> use fully measured params from first state on trajectory
                                (meant for seed-iterations and piecewise filtering)
     - start 2 "resume-at-1" -> re-filter after outlier, but need to get back to 1 anyway.
     - start i>2 "resume"    -> re-filter after outlier at i, prepare skipping i.

     cases 1 and higher do the first node in the if-case below, not in the for loop.
  */

  if ( filterStartState<0 ) {
    updatedPar = referenceParameters.clone();
    ATH_MSG_VERBOSE ("-F- start filtering with input reference parameters (no cov matrix)...");

  } else {
    if (filterStartState>2) {
      // set iterator to *before* new outlier, because we didn't keep the update from there.
      Trk::Trajectory::iterator testIt = m_utility->firstFittableState(trajectory);
      while (testIt != trajectory.end() &&
             testIt->positionOnTrajectory()<filterStartState) {
        it = testIt;
        testIt = m_utility->nextFittableState(trajectory,it);
      }
    }
    ATH_MSG_VERBOSE ("-F- start filtering with parameters from "<< 
                     (filterStartState>2 ? "before this new Outlier state":"the seed") 
                     << " at pos. " << it->positionOnTrajectory());

    if (!it->forwardTrackParameters()) {
      ATH_MSG_WARNING ("-F- violates convention: start fwd filter with parameters at the first state.");
      return FitterStatusCode::BadInput;
    }

    const Trk::TrackParameters* predPar = it->checkoutForwardPar(); // get ownership right for main loop.
    if (it->dnaMaterialEffects()) delete it->checkoutDNA_MaterialEffects();

    FitterStatusCode stepFwF =
      this->buildAndAnalyseTrajectory(trajectory,it,updatedPar /*=nullptr pointer*/,
                                      predPar, controlledMatEffects,itcounter,
                                      bremStateIfBremFound,allowRecalibrate);
    if (stepFwF.isFailure()) {
      ATH_MSG_INFO ("-F- first buildAndAnalyseTrajectory() failed.");
      it->checkinForwardPar(predPar);
      return stepFwF;
    }
    if (filterStartState == 0 ) 
      stepFwF = this->updateOrSkip(it,updatedPar,predPar /* check in again to trajectory*/,
                                   1,runOutlier,bremStateIfBremFound);
    else
      stepFwF = this->updateOrSkip(it,updatedPar,predPar /* check in again to trajectory*/,
                                   filterStartState,runOutlier,bremStateIfBremFound);
    if (stepFwF.isFailure()) {
      ATH_MSG_DEBUG ("-F- first updateOrSkip() failed.");
      it->checkinForwardPar(predPar); 
      delete updatedPar; return stepFwF; 
    }
    itcounter = it->positionOnTrajectory()+1;
    ++it;
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  // the regular filter loop after finding the correct initialisation
  for( ; it!=trajectory.end(); it++) if (!it->isOutlier()) {

    const Trk::TrackParameters* predPar =
      this->predict( updatedPar, it->measurement()->associatedSurface(),
                     controlledMatEffects, ++itcounter, it->positionOnTrajectory());

    FitterStatusCode stepFwF =
      this->buildAndAnalyseTrajectory(trajectory,it,updatedPar,predPar,
                                      controlledMatEffects,itcounter,
                                      bremStateIfBremFound,allowRecalibrate);
    if (stepFwF.isFailure()) { 
      delete updatedPar; delete predPar; return stepFwF;
    }
    stepFwF = this->updateOrSkip(it,updatedPar,predPar,itcounter,
                                 runOutlier,bremStateIfBremFound);
    bremStateIfBremFound = nullptr; 

    if (stepFwF.isFailure()) { delete updatedPar; return stepFwF; }
  }

  delete updatedPar;
  int testNumber = m_utility->rankedNumberOfMeasurements(trajectory);
  if (testNumber < 5) {
    ATH_MSG_DEBUG ("Filtered trajectory has only " << testNumber
                   << " and thus too few fittable measurements!");
    return FitterStatusCode::FewFittableMeasurements; // caller (KalmanFitter) will clean up...
  }

  return FitterStatusCode::Success;
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// dynamic-trajectory forward filter step part 1: prediction to next surface  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* Trk::ForwardKalmanFitter::predict
(const Trk::TrackParameters*    updatedPar, 
 const Trk::Surface&            nominalDestSurface,
 const Trk::KalmanMatEffectsController& controlledMatEffects,
 const int                      filterCounter,
 const int                      tjPositionCounter) const
{
  if (msgLvl(MSG::DEBUG) && filterCounter == 1)
    printGlobalParams( (tjPositionCounter-1), "  init", updatedPar );
  //  m_log << MSG::DEBUG << "From " << *updatedPar << " to " << destinationSurface << endmsg;
  
  const Trk::Surface& destinationSurface = m_alignableSurfaceProvider              ?
    m_alignableSurfaceProvider->retrieveAlignableSurface(nominalDestSurface):
    nominalDestSurface                                                      ;
  
  const Trk::TrackParameters* predPar = nullptr;
  if (filterCounter == 1) {

    ////////////////////////////////////////////////////////////////////////////
    // --- cover two cases in this method: this one initialises the filter
    if ((updatedPar->associatedSurface()) == (destinationSurface)) {
      predPar = updatedPar->clone();
      ATH_MSG_VERBOSE ("-Fp start params already expressed at 1st"
                       << " meas't - no extrapolation needed.");
    } else {
      ATH_MSG_VERBOSE ("-Fp get filter onto 1st surface by direct extrapolation.");
      if (!m_useExEngine) 
	predPar = m_extrapolator->extrapolateDirectly(*updatedPar, destinationSurface,
						      Trk::anyDirection,
						      false, Trk::nonInteracting);

      else {
        ATH_MSG_INFO ("Forward Kalman Fitter --> starting extrapolation engine");
        Trk::ExtrapolationCell <Trk::TrackParameters> ecc(*updatedPar, Trk::anyDirection);
        ecc.setParticleHypothesis(Trk::nonInteracting);
        ecc.addConfigurationMode(Trk::ExtrapolationMode::Direct);
        Trk::ExtrapolationCode eCode =  m_extrapolationEngine->extrapolate(ecc, &destinationSurface, false);
        
	if (eCode.isSuccess() && ecc.endParameters) {
	  ATH_MSG_INFO ("Forward Kalman Fitter --> extrapolation engine success");
	  predPar = ecc.endParameters;
	} else {
          ATH_MSG_WARNING ("Forward Kalman Fitter --> extrapolation engine did not succeed");
          predPar = nullptr;
        }
      }      

    }

    /* possible difficulty here
       when the starting track parameters are already at the 1st surface.
       Solutions involve to go backward by a bit (if a certain type of propagator
       does not check if the surfaces overlap, but are different (Perigee & Line 
       defined at the same point). This has to be recognised, and the TP cloned */
    if (!predPar && destinationSurface.isOnSurface(updatedPar->position(),true,5.0)) {
      const Trk::Perigee* testPer = dynamic_cast<const Trk::Perigee*>(updatedPar);
      const Trk::StraightLineSurface* wireSurface = 
        dynamic_cast<const Trk::StraightLineSurface*>(&destinationSurface);
      if (testPer and wireSurface) {
        predPar = new Trk::AtaStraightLine(updatedPar->parameters()[Trk::loc1],
					   updatedPar->parameters()[Trk::loc2],
                                           updatedPar->parameters()[Trk::phi],
                                           updatedPar->parameters()[Trk::theta],
                                           updatedPar->parameters()[Trk::qOverP],
                                           *wireSurface);
        ATH_MSG_INFO ("-Fp initial Perigee transformed to TrkPars *manually* - this may need a fix!");
      }
    }
    
  } else {

    ////////////////////////////////////////////////////////////////////////////
    // --- 2nd case covers filter loop: extrapolate to next surface with full matEffects
    if (!m_useExEngine) 
      predPar = m_extrapolator->extrapolate(*updatedPar,destinationSurface,
					    Trk::alongMomentum,false,
					    controlledMatEffects.particleType());
    else {
      ATH_MSG_DEBUG ("Forward Kalman Fitter --> starting extrapolation engine");
      Trk::ExtrapolationCell <Trk::TrackParameters> ecc(*updatedPar, Trk::alongMomentum);
      ecc.setParticleHypothesis(controlledMatEffects.particleType());
      Trk::ExtrapolationCode eCode =  m_extrapolationEngine->extrapolate(ecc, &destinationSurface, false);
      
      if (eCode.isSuccess() && ecc.endParameters) {
	ATH_MSG_DEBUG ("Forward Kalman Fitter --> extrapolation engine success");
	predPar = ecc.endParameters;
      } else
        ATH_MSG_WARNING ("Forward Kalman Fitter --> extrapolation engine does not succeed");
    }
  }
  return predPar;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// forward filter step part 2: validation, calibration, trajectory analysis   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
Trk::FitterStatusCode Trk::ForwardKalmanFitter::buildAndAnalyseTrajectory
(Trk::Trajectory&                T,
 Trajectory::iterator&           predictedState,
 const Trk::TrackParameters*&    updatedPar,
 const Trk::TrackParameters*&    predPar,
 const Trk::KalmanMatEffectsController&  controlledMatEffects,
 const int&                      filterCounter,
 Trk::ProtoTrackStateOnSurface*& bremEffectsState,
 const bool                      allowRecalibrate) const
{
  bremEffectsState = nullptr; // means no brem
  const MeasurementBase* fittableMeasurement = predictedState->measurement();
  int I = predictedState->positionOnTrajectory();

  ////////////////////////////////////////////////////////////////////////////
  // --- a couple of sanity checks and debug output first
  if (!fittableMeasurement){
    ATH_MSG_WARNING ("The Meas'Base extraction failed, software problem!");
    return FitterStatusCode::BadInput;
  }
  if ( filterCounter>1 && updatedPar!=nullptr 
       && ( (updatedPar->associatedSurface()) == 
            (fittableMeasurement->associatedSurface()) )) {
    ATH_MSG_WARNING ("Measurements " << (I<11?"T0":"T") << I-1 <<
                   (I<10?" and T0":" and T") <<I<<" are at the same surface ==> reject track.");
    ATH_MSG_WARNING ("This may be a duplication of input measurements "
                     << " or a wrong use of the TrackStateOnSurface EDM.");
    ATH_MSG_DEBUG ("updatedPar" << *updatedPar << " at surface ptr " << &updatedPar->associatedSurface()
                   << "\n" << updatedPar->associatedSurface());
    ATH_MSG_DEBUG ("fittableMs" << *fittableMeasurement << " at surface ptr "
                   << &(fittableMeasurement->associatedSurface()) << "\n" 
                   << fittableMeasurement->associatedSurface());
     m_utility->dumpTrajectory(T,"TSoS consistency problem");
    return FitterStatusCode::BadInput;
  }
  
  if(!predPar) {
    ATH_MSG_DEBUG ((I<10?"T0":"T") << I << " --- missed surface with "
                   << "extrapolation, flag state as outlier --- " );
    if (nullptr != dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(fittableMeasurement) ) {
      ATH_MSG_INFO ("lost PseudoMeasurement during fwd extrapolation - "
                    << "PM-surface or sorting problem.");
      if (msgLvl(MSG::DEBUG)) {
        m_utility->identifyMeasurements(T);    
        m_utility->dumpTrajectory(T,"PseudoMeasurementProblem");
        if (updatedPar) ATH_MSG_DEBUG ("start surface (updatedPar) " 
                                      << updatedPar->associatedSurface());
        ATH_MSG_DEBUG ("PM surface " << *fittableMeasurement);
      }
    }
    predictedState->isOutlier(Trk::TrackState::MissedOutlier,1);
    return Trk::FitterStatusCode::Success;
  }
  if (msgLvl(MSG::DEBUG)) printGlobalParams( predictedState->positionOnTrajectory(), " extrp", predPar );
  
  //// 2a ////////////////////////////////////////////////////////////////
  // Posibly re-calibrate
    
  if (allowRecalibrate && m_recalibrator
      && predictedState->measurementType() == TrackState::TRT)  {
    // the replaceMeas.-method does ownership right and detects if the
    // recalibration produces the same measurement again

    predictedState->replaceMeasurement
      ( m_recalibrator->makePreciseMeasurement(*predictedState->measurement(),*predPar,
                                               TrackState::TRT),
        m_recalibrator->calibrationStatus(*predictedState->measurement(),TrackState::TRT)
        ); 
    fittableMeasurement = predictedState->measurement();
  }

  if (!updatedPar) return Trk::FitterStatusCode::Success;

  ////////////////////////////////////////////////////////////////////
  // search for brem and adjust the error according to target measurement (brem fit)
  if (filterCounter>2) {
    const Trk::DNA_MaterialEffects* updMomNoise = 
      !controlledMatEffects.doDNA() ? nullptr :
      m_dynamicNoiseAdjustor->DNA_Adjust(predPar, // change according to where meas is
                                         updatedPar, // re-start from old pars
                                         fittableMeasurement, // the meas't
                                         controlledMatEffects,
                                         Trk::alongMomentum);
    if (updMomNoise) {
      Trk::Trajectory::iterator b = m_utility->previousFittableState(T, predictedState);
      if (b!=T.end()) {
        b->checkinDNA_MaterialEffects(updMomNoise);
        bremEffectsState = &(*b);
        ATH_MSG_VERBOSE ("-F- DNA kicked in at t/X0 of " << updMomNoise->thicknessInX0() <<
                         " with noise " << updMomNoise->addSigmaQoverP());
      }
      else delete updMomNoise;
    }
  }

  ////////////////////////////////////////////////////////////////////
  // if it is configured, try mini-DAF in first iteration - sw. OFF in KF.cxx
  if ( m_internalDAF != nullptr && allowRecalibrate ) {
    const Trk::StraightLineSurface* testNextSurfaceIfWire = 
      dynamic_cast<const Trk::StraightLineSurface*>(&predPar->associatedSurface());
    const Trk::StraightLineSurface* testOldSurfaceIfWire = 
      dynamic_cast<const Trk::StraightLineSurface*>(&updatedPar->associatedSurface());
    const Trk::RIO_OnTrack* testROT 
      = dynamic_cast<const Trk::RIO_OnTrack*>(fittableMeasurement);
    double testPredErr  = (predPar->covariance()==nullptr ? 0.0 : std::sqrt( (*predPar->covariance())(Trk::locX,Trk::locX)));
    double testDistance = (predPar->position() - updatedPar->position()).mag();

    if ( msgLvl(MSG::DEBUG) &&
         (testOldSurfaceIfWire == nullptr || testDistance > 1000.0*CLHEP::mm || filterCounter<=2) &&
         (testNextSurfaceIfWire != nullptr) ){
      ATH_MSG_DEBUG ("precise prediction? " << predPar->parameters()[Trk::locR]
                     << " +/- " << testPredErr
                     << " measurement " << fittableMeasurement->localParameters()[Trk::locR]
                     << " +/- " << std::sqrt(fittableMeasurement->localCovariance()(Trk::locR,Trk::locR)));
    }
    double testRadius = fittableMeasurement->localParameters()[Trk::locR];
    double testPred   = predPar->parameters()[Trk::locR];

    if ( (testOldSurfaceIfWire == nullptr || testDistance > 1000.0*CLHEP::mm || filterCounter<=2)
         && (testNextSurfaceIfWire != nullptr)
         && testROT != nullptr // FIXME will be blocked by PseudoMeasts like this
         // FIXME also look at anticipated fraction of tube hits
         && (testPredErr > std::min(2.0*std::sqrt(fittableMeasurement->localCovariance()(Trk::locX,Trk::locX)),0.4)
             || (std::fabs(testRadius-testPred)>0.5))) {
      ATH_MSG_DEBUG ("Starting driftcircle L/R solving, observed at state " 
                     << predictedState->positionOnTrajectory() << " err="<<testPredErr);

      m_internalDAF->filterTrajectoryPiece(T, predictedState, updatedPar, predPar, 12, 
      					   controlledMatEffects.particleType());
    }

  }

  return Trk::FitterStatusCode::Success;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// dynamic-trajectory forward filter step part 3: update and chi2 cut         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
Trk::FitterStatusCode Trk::ForwardKalmanFitter::updateOrSkip
(const Trajectory::iterator&     predictedState,
 const Trk::TrackParameters*&    updatedPar,
 const Trk::TrackParameters*&    predPar,
 const int&                      filterCounter,
 const Trk::RunOutlierRemoval&   runOutlier,
 Trk::ProtoTrackStateOnSurface*  bremEffectsState) const
{
  // if extrapolation had failed, skip this method altogether.
  if (predPar==nullptr)   return FitterStatusCode::Success;
  const MeasurementBase* fittableMeasurement = predictedState->measurement();
  FitQualityOnSurface* fitQuality=nullptr;
  delete updatedPar;

  ////////////////////////////////////////////////////////////////////
  // special fits with PseudoMeasurement (eg TRT only): PM replaces initial cov.
  // FIXME needs further study if pseudomeasurements in TRT are actually OK
  if (filterCounter == 1 &&
      // don't check on MeasuredPredPar nullptr, else fails on iteration
      // dynamic_cast<const Trk::MeasuredTrackParameters*>(predPar) == nullptr &&
      dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(fittableMeasurement) !=nullptr &&
      predPar != nullptr ) { // FIXME needs check that surfaces are identical?
    ATH_MSG_DEBUG ("Detected external stabilisation from PseudoMeast at start");
    AmgVector(5) par;
    par.setZero();
    const std::vector<double> cov0{250., 250., 0.25, 0.25, 0.000001};
    AmgSymMatrix(5)* cov = new AmgSymMatrix(5);
    cov->setZero();
    for (int i=0, j=0; i<5; ++i) {
      if (fittableMeasurement->localParameters().contains(((Trk::ParamDefs)i))) {
        par[((Trk::ParamDefs)i)] = fittableMeasurement->localParameters()[((Trk::ParamDefs)i)];
        (*cov)(i,i) = fittableMeasurement->localCovariance()(j,j);
        ATH_MSG_VERBOSE ("Stabilise " << i << " with "
			 << fittableMeasurement->localParameters()[((Trk::ParamDefs)i)] << " and +/- "
                         << fittableMeasurement->localCovariance()(j,j));
        ++j;
      } else {
        par[((Trk::ParamDefs)i)] = predPar->parameters()[((Trk::ParamDefs)i)];
        (*cov)(i,i) = cov0[i];
      }
    }
    updatedPar = CREATE_PARAMETERS(*predPar,par,cov); 
    fitQuality = new Trk::FitQuality(0.0, fittableMeasurement->localParameters().dimension());
  } else {


    ////////////////////////////////////////////////////////////////////
    // make the update
    updatedPar = m_updator->addToState(*predPar, fittableMeasurement->localParameters(),
                                       fittableMeasurement->localCovariance(),
                                       fitQuality);
    ////////////////////////////////////////////////////////////////////
    // check that updated parameters are ok and write back to trajectory
    if (!updatedPar) {
      ATH_MSG_INFO (" --- update of track params failed, stop filter");
      delete predPar;
      if (fitQuality) delete fitQuality;
      return FitterStatusCode::UpdateFailure;
    }
  }
  if (msgLvl(MSG::DEBUG)) printGlobalParams( predictedState->positionOnTrajectory(),
                                             " updat", updatedPar );
  
  ////////////////////////////////////////////////////////////////////
  // analyse the fit quality, possibly flag as outlier
  if (fitQuality == nullptr) {
    ATH_MSG_INFO ( "Updator failed to create any FitQuality object at all!"
                   << endmsg << "==> OK to flag as outlier?");
    predictedState->isOutlier(Trk::TrackState::FilterOutlier);
  }
  else if (runOutlier && fitQuality->chiSquared() >  m_StateChiSquaredPerNumberDoFPreCut
           * fitQuality->numberDoF()) {

    if ( m_recalibrator &&
         predictedState->measurementType() == TrackState::TRT &&
         ( Trk::SensorBoundsCheck::areParamsInside
           (*fittableMeasurement, predPar->parameters(), *predPar->covariance(), 1.0, -1.0)))  {
      predictedState->replaceMeasurement
        ( m_recalibrator->makeBroadMeasurement(*predictedState->measurement(),*predPar,
                                               TrackState::TRT),
          m_recalibrator->calibrationStatus(*predictedState->measurement(),TrackState::TRT)
          ); 
      fittableMeasurement = predictedState->measurement();
      ATH_MSG_DEBUG ("Broadened TRT hit instead of outlier");
      delete updatedPar; delete fitQuality; fitQuality=nullptr;
      ////////////////////////////////////////////////////////////////////
      // make the update
      updatedPar = m_updator->addToState(*predPar, fittableMeasurement->localParameters(),
                                         fittableMeasurement->localCovariance(),
                                         fitQuality);
      if ( (!updatedPar || !fitQuality ||
           (runOutlier && fitQuality->chiSquared() > m_StateChiSquaredPerNumberDoFPreCut
            * fitQuality->numberDoF())) ) {
        if (fitQuality) ATH_MSG_DEBUG ("Chi2 ("<<fitQuality->chiSquared()<<"/"<<fitQuality->numberDoF()<<
				       ") too large, remove measurement from track fit.");
        predictedState->isOutlier(Trk::TrackState::StateOutlier);
      }
    } else {
      ATH_MSG_DEBUG ("failed Chi2 test, remove measurement from track fit.");
      predictedState->isOutlier(Trk::TrackState::StateOutlier);
    }
  }
  else if (predPar->covariance()!=nullptr && runOutlier
           && (*predPar->covariance())(Trk::phi,Trk::phi)   < 0.01*0.01
           && (*predPar->covariance())(Trk::theta,Trk::theta) < 0.01*0.01
           && ! (Trk::SensorBoundsCheck::areParamsInside
                 (*fittableMeasurement, updatedPar->parameters(), *updatedPar->covariance(), 5.0, 4.0)) ) {
    ATH_MSG_DEBUG ("failed track-within-sensor test, remove measurement from fit.");
    ATH_MSG_VERBOSE (predPar->associatedSurface());
    predictedState->isOutlier(Trk::TrackState::SensorOutlier);
  }
  if ( !predictedState->isOutlier() ) {
    if (fitQuality) ATH_MSG_VERBOSE ("update and coarse Chi2 ("<<fitQuality->chiSquared()<<"/"<<
				     fitQuality->numberDoF()<<") okay, add to trajectory");
  } else {
    if (filterCounter==1) {
      // TO-DO investigate this! (TRT only fits with PseudoMeast removed, and thus refPars not matching)
      ATH_MSG_DEBUG ("Failed on first filter step, this indicates bad inputs or job options!");
      if (fitQuality) delete fitQuality; //other objects managed by trajectory
      return FitterStatusCode::BadInput;
    }
    // copy over prePar to updatedPar because we ignore this update
    if (bremEffectsState!=nullptr) delete bremEffectsState->checkoutDNA_MaterialEffects();
    delete updatedPar;
    updatedPar = predPar->clone();
    // delete predPar; done by ProtoTrackStateOnSurface
  } // end if fitQuality OK
  if (fitQuality) predictedState->setForwardStateFitQuality(*fitQuality);
  predictedState->checkinForwardPar(predPar);
  delete fitQuality;
  return FitterStatusCode::Success;
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// forward filter preparation: get filter onto 1st state and assign big error //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
Trk::FitterStatusCode Trk::ForwardKalmanFitter::enterSeedIntoTrajectory
(Trajectory&            trajectory,
 const Trk::TrackParameters& inputPar,
 const std::vector<double>& cov0,
 const Trk::KalmanMatEffectsController& ,
 bool) const {

  // FIXME check that trajectory is empty
  Trk::Trajectory::iterator ffs = m_utility->firstFittableState(trajectory);
  if (ffs->forwardTrackParameters() || ffs->referenceParameters()) {
    ATH_MSG_WARNING (" wrong input? Dont call enterSeedIntoTrajectory on a full trajectory");
    if (ffs->forwardTrackParameters()) delete ffs->checkoutForwardPar();
    if (ffs->referenceParameters()) delete ffs->checkoutReferenceParameters();
  }
  if (ffs->measurement() == nullptr) {
    ATH_MSG_WARNING ("-Fe mess-up in enterSeedIntoTraj: can not find a first measurement!");
    trajectory.clear();
    return FitterStatusCode::BadInput;
  }
  const Trk::Surface& startSurface = ffs->measurement()->associatedSurface();
  const Trk::TrackParameters* inputParAtStartSurface = nullptr;
  
  // chk if TPar are already in correct local frame: first pointer check (quick) then geometric
  if ( &startSurface ==  &inputPar.associatedSurface() ||
       startSurface == inputPar.associatedSurface() ) {

    inputParAtStartSurface = &inputPar;
    ATH_MSG_VERBOSE ("-Fe start params already expressed at 1st meas't - do no extrapolate.");
  } else {
    ATH_MSG_VERBOSE ("-Fe get filter onto 1st surface by direct extrapolation.");
    if (!m_useExEngine)
      inputParAtStartSurface = m_extrapolator->extrapolateDirectly(inputPar,
								   startSurface,
								   Trk::anyDirection,
								   false, Trk::nonInteracting);
    else {
      ATH_MSG_DEBUG ("Forward Kalman Fitter --> starting extrapolation engine");
      Trk::ExtrapolationCell <Trk::TrackParameters> ecc(inputPar, Trk::anyDirection);
      ecc.setParticleHypothesis(Trk::nonInteracting);
      ecc.addConfigurationMode(Trk::ExtrapolationMode::Direct);
      Trk::ExtrapolationCode eCode =  m_extrapolationEngine->extrapolate(ecc, &startSurface, false);
      
      if (eCode.isSuccess() && ecc.endParameters) {
	ATH_MSG_DEBUG ("Forward Kalman Fitter --> extrapolation engine success");
	inputParAtStartSurface = ecc.endParameters;
      } else ATH_MSG_WARNING ("Forward Kalman Fitter --> extrapolation engine did not succeed");
    }      

    if (inputParAtStartSurface == nullptr) {
      ATH_MSG_WARNING ("-Fe can not transport input param to first measurement => extrap problem or bad input");
      ATH_MSG_INFO ("-Fe parameters R="<< inputPar.position().perp() << ", z="<<
                    inputPar.position().z()<<" q/p="<<inputPar.parameters()[Trk::qOverP]<<
                    " to Sf at "<<startSurface.center());
      if (msgLvl(MSG::DEBUG)) m_utility->dumpTrajectory(trajectory,"seed initialisation method");
      ATH_MSG_DEBUG ("EDM pars are "<<inputPar<<"\n and first meas. Surface"<<startSurface);
      trajectory.clear();
      return FitterStatusCode::BadInput;
    }
  }

  AmgSymMatrix(5)* cov = new AmgSymMatrix(5)(); cov->setZero();
  for (int i=0; i<5; ++i) (*cov)(i,i) = cov0[i];
  const AmgVector(5)& par = inputParAtStartSurface->parameters();
  // TODO: check does one need covariance here?
  const Trk::TrackParameters* seedPar = CREATE_PARAMETERS((*inputParAtStartSurface),par, cov);
  if (inputParAtStartSurface != &inputPar) delete inputParAtStartSurface;
  ffs->checkinForwardPar(seedPar);
  ATH_MSG_VERBOSE ("-Fe prepared trajectory with seed parameters on state "<<ffs->positionOnTrajectory());

  return FitterStatusCode::Success;
}


// private -- helper to make pretty debug output
void Trk::ForwardKalmanFitter::printGlobalParams(int istate, const std::string& ptype,
                                                 const Trk::TrackParameters* param,
                                                 const Trk::DNA_MaterialEffects* mefot)
  const
{
  char tt[80]; snprintf(tt,79,"T%.2u",istate);
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
    msg(MSG::VERBOSE) << (istate>9?" T":" T0") << istate << ", Mefot found with " 
          << *mefot << endmsg;
}
