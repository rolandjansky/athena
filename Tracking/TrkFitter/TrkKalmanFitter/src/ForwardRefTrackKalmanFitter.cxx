/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ForwardRefTrackKalmanFitter.cxx
//   Source file for class ForwardRefTrackKalmanFitter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Markus.Elsing@cern.ch
///////////////////////////////////////////////////////////////////

#include <memory>



#include "TrkKalmanFitter/ForwardRefTrackKalmanFitter.h"
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
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkFitterUtils/DNA_MaterialEffects.h"
#include "TrkFitterUtils/ProtoTrajectoryUtility.h"
#include "TrkFitterUtils/ProtoMaterialEffects.h"
#include "TrkFitterUtils/SensorBoundsCheck.h"

// common tracking tools
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkFitterInterfaces/IDynamicNoiseAdjustor.h"
#include "TrkFitterInterfaces/IMeasurementRecalibrator.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Gaudi AlgTool control (constructor, initialise ...)                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
Trk::ForwardRefTrackKalmanFitter::ForwardRefTrackKalmanFitter(const std::string& t,const std::string& n,const IInterface* p) :
  AthAlgTool (t,n,p),
  m_extrapolator(nullptr),
  m_updator(nullptr),
  m_ROTcreator(nullptr),
  m_dynamicNoiseAdjustor(nullptr),
  m_recalibrator(nullptr),
  m_idEnclosingVolume(nullptr),
  m_msCompleteVolume(nullptr),
  m_idHelper(nullptr),
  m_utility(nullptr),
  m_StateChiSquaredPerNumberDoFPreCut{},
  m_particleMasses{}
{
  // AlgTool stuff
  declareInterface<IForwardKalmanFitter>( this );
  
  // declare all properties needed to configure fitter, defaults
  declareProperty("StateChi2PerNDFPreCut", m_StateChiSquaredPerNumberDoFPreCut=50.f,
                  "coarse pre-cut on the predicted state's chi2/ndf against outliers destabilising the filter - only in effect if called with runOutlier true");
}

// destructor
Trk::ForwardRefTrackKalmanFitter::~ForwardRefTrackKalmanFitter(){}

// initialize
StatusCode Trk::ForwardRefTrackKalmanFitter::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  ATH_CHECK (detStore()->retrieve(m_idHelper, "AtlasID"));
  m_utility = new ProtoTrajectoryUtility(m_idHelper);
  ATH_MSG_INFO ("stability precut on state Chi2/ndf set to "<< m_StateChiSquaredPerNumberDoFPreCut );
  ATH_MSG_INFO ("initialize() successful in " << name());
  return sc;
}

// finalize
StatusCode Trk::ForwardRefTrackKalmanFitter::finalize()
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
StatusCode Trk::ForwardRefTrackKalmanFitter::configureWithTools(const Trk::IExtrapolator* extrap,
                                                                const Trk::IUpdator* updator,
                                                                const Trk::IRIO_OnTrackCreator* rot_crea,
                                                                const Trk::IDynamicNoiseAdjustor* dna,
                                                                const Trk::IAlignableSurfaceProvider*,
                                                                const Trk::IMeasurementRecalibrator* mr,
                                                                const Trk::IKalmanPiecewiseAnnealingFilter*)
{
  m_extrapolator = extrap;
  m_updator      = updator;
  m_ROTcreator   = rot_crea;
  m_dynamicNoiseAdjustor = dna;
  m_recalibrator = mr;
  
  // protection, if not confiured
  if (!m_updator) {
    ATH_MSG_ERROR ("Updator missing, need to configure with it !");
    return StatusCode::FAILURE;
  }
  if (!m_extrapolator) {
    ATH_MSG_ERROR ("Extrapolator missing, need to configure with it !");
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << "Configuring " << name() << " with tools from KalmanFitter:" << endmsg;
  msg(MSG::INFO) << "Updator and Extrapolator    - present" << endmsg;
  msg(MSG::INFO) << "Dyn.NoiseAdjustment Pix/SCT - " 
        << (m_dynamicNoiseAdjustor? "present" : "none") << endmsg;
  msg(MSG::INFO) << "General RIO_OnTrackCreator  - " 
        << (m_ROTcreator? "present" : "none") << endmsg;
  msg(MSG::INFO) << "RIO_OnTrack Recalibrator    - " << (m_recalibrator?"present":"none")<<endmsg;
  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// forward fit on set of PrepRawData                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

Trk::FitterStatusCode 
Trk::ForwardRefTrackKalmanFitter::fit(Trk::Trajectory&,
                                      const Trk::PrepRawDataSet& ,
                                      const Trk::TrackParameters& ,
                                      const Trk::RunOutlierRemoval ,
                                      const Trk::KalmanMatEffectsController& ) const
{
  ATH_MSG_WARNING ("Not yet implemented.");
  return FitterStatusCode::BadInput;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// forward fit on a set of MeasurementBase                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
Trk::FitterStatusCode
Trk::ForwardRefTrackKalmanFitter::fit(Trk::Trajectory& trajectory,
                              const Trk::TrackParameters& ,
                              const Trk::RunOutlierRemoval  runOutlier,
                              const Trk::KalmanMatEffectsController&  controlledMatEffects,
                              const bool allowRecalibrate,
                              const int  filterStartState) const
{
  if (trajectory.empty()) {
    ATH_MSG_WARNING ("internal error: empty trajectory!");
    return FitterStatusCode::BadInput;
  }
  if (allowRecalibrate) m_utility->identifyMeasurements(trajectory);
  Trk::Trajectory::iterator it = m_utility->firstFittableState(trajectory);
  std::unique_ptr< std::pair<AmgVector(5),AmgSymMatrix(5)> >updatedDifference;        // delete & remake during filter
  ATH_MSG_DEBUG ("-F- entering FwFilter with matEff="<<controlledMatEffects.particleType()<<
                 ", "<<(allowRecalibrate?"recalibrate:yes":"recalibrate:no")<<
                 " and start state "<<filterStartState);

  if ((filterStartState>2) && (it->positionOnTrajectory() < filterStartState)) {
    // set iterator to *before* new outlier, because we didn't keep the update from there.
    it = trajectory.begin();
    it += (filterStartState-2);
//     Trk::Trajectory::iterator testIt = m_utility->firstFittableState(trajectory);
//     while (testIt != trajectory.end() &&
//            testIt->positionOnTrajectory()<filterStartState) {
//       it = testIt;
//       testIt = m_utility->nextFittableState(trajectory,it);
//     }
  }
  ATH_MSG_VERBOSE ("-F- start FwFilter with parameters from "<< 
                   (filterStartState>2 ? "before this new Outlier state":"the seed") 
                   << " at pos. " << it->positionOnTrajectory() << " filter start state " << filterStartState);

  if (!it->parametersDifference() || !it->referenceParameters()) {
    ATH_MSG_WARNING ("-F- convention not followed: expect ref & pardifference"<<
                     " at first state in trajectory.");
    return FitterStatusCode::BadInput;
  }

  const TransportJacobian* jacobian = nullptr;

  //////////////////////////////////////////////////////////////////////////////////////////
  // the regular filter loop after finding the correct initialisation
  for( ; it!=trajectory.end(); it++) {

    const AmgVector(5)* predDiffPar = nullptr;
    const AmgSymMatrix(5)* predDiffCov = nullptr;
    if (jacobian==nullptr) {
      ATH_MSG_DEBUG ("-F- beginning of trajectory, have already predPar");
      predDiffPar = it->parametersDifference();
      predDiffCov = it->parametersCovariance();
    } else {
      // extrapolate from previous state
      const TransportJacobian& jac = *jacobian;
      predDiffPar = new AmgVector(5)(jac*(updatedDifference->first));
      AmgSymMatrix(5)* C = new AmgSymMatrix(5) (jac*(updatedDifference->second)*jac.transpose());
      // add uncertainties from material effects:
      if (it->materialEffects()) {
        (*C)(Trk::phi,Trk::phi)      += std::pow(it->materialEffects()->sigmaDeltaPhi(),2);
        (*C)(Trk::theta,Trk::theta)  += std::pow(it->materialEffects()->sigmaDeltaTheta(),2);
        // TODO: check: neglect mass here?
        const double& mass = m_particleMasses.mass[controlledMatEffects.particleType()];
        const double currQOverPsquared = std::pow((*predDiffPar)(Trk::qOverP) + it->referenceParameters()->parameters()[Trk::qOverP], 2);
        // sigmaDeltaQoverP^2 = sigmaDeltaE^2 * E^2 / p^6 = sigmaDeltaE^2 * (m^2 + p^2) / p^6
        const double sigmaDeltaQoverPsquared = (currQOverPsquared > 0.) ? (std::pow(it->materialEffects()->sigmaDeltaE(), 2) * (mass*mass + 1./currQOverPsquared) * (currQOverPsquared*currQOverPsquared*currQOverPsquared)) : 0.;
        (*C)(Trk::qOverP,Trk::qOverP)+= sigmaDeltaQoverPsquared;
        ATH_MSG_VERBOSE ("mass=" << mass << ", qOverP_ref="<< std::scientific << it->referenceParameters()->parameters()[Trk::qOverP]
                << ", qOverP_diff=" << (*predDiffPar)(Trk::qOverP)
                << ", sigmaDeltaE=" << it->materialEffects()->sigmaDeltaE()
                << ", sigmaDeltaQoverP=" << sqrt(sigmaDeltaQoverPsquared) << std::fixed);//std::defaultfloat);
        ATH_MSG_VERBOSE ("Added material effects.");
      }
      predDiffCov = C; // to solve constness problem
      // back into trajectory
      it->checkinParametersDifference(predDiffPar);
      it->checkinParametersCovariance(predDiffCov); 
      updatedDifference.reset();
      // delete updatedDiffCov; updatedDiffCov=0;
    }
    if (msgLvl(MSG::DEBUG)) 
      printGlobalParams( it->positionOnTrajectory(), " extrp",
                         *it->referenceParameters(),*predDiffPar );

    const MeasurementBase* fittableMeasurement = it->measurement();
    if ((!fittableMeasurement) || it->isOutlier()) { // pure material state
      updatedDifference = std::make_unique<std::pair<AmgVector(5),AmgSymMatrix(5)>>( std::make_pair(*predDiffPar,*predDiffCov) );
    } else {

      // Posibly re-calibrate. TRT extension finder leaves many TRT hits as wire-hits w/o LR solution
      if (allowRecalibrate && m_recalibrator
          && it->measurementType() == TrackState::TRT)  {
        const AmgVector(5) x = it->referenceParameters()->parameters()+(*predDiffPar);
        const Trk::TrackParameters* param = CREATE_PARAMETERS(*it->referenceParameters(),x,nullptr);
        Trk::TrackState::CalibrationType oldType = it->calibrationType();

        // the replaceMeas.-method does ownership right and detects if the
        // recalibration produces the same measurement again
        it->replaceMeasurement
          ( m_recalibrator->makePreciseMeasurement(*it->measurement(),*param,
                                                   TrackState::TRT),
            m_recalibrator->calibrationStatus(*it->measurement(),TrackState::TRT) );
        fittableMeasurement = it->measurement();
	ATH_MSG_VERBOSE ("TRT ROT calibration changed, from "<<oldType<<" to "<<it->calibrationType());
        delete param;
      }

      Trk::FitQualityOnSurface* fitQS=nullptr;
      updatedDifference.reset(
        m_updator->updateParameterDifference(*predDiffPar, *predDiffCov,
                                               *(it->measurementDifference()),
                                               fittableMeasurement->localCovariance(),
                                               fittableMeasurement->localParameters().parameterKey(),
                                             fitQS, /*doFQ=*/true ) );
      if (!updatedDifference || fitQS == nullptr) {
        delete fitQS;
        ATH_MSG_DEBUG("update in reference-parameter formalism failed.");
        return FitterStatusCode::UpdateFailure;
      }
      if (msgLvl(MSG::DEBUG)) printGlobalParams (it->positionOnTrajectory()," updat",
                                                 *it->referenceParameters(), updatedDifference->first );
      const float updatedQoverP = (it->referenceParameters()->parameters()[Trk::qOverP]) + (updatedDifference->first[Trk::qOverP]);
      if (fabs(updatedQoverP) > 0.1) {
            delete fitQS;
            ATH_MSG_DEBUG("update in reference-parameter formalism failed, momentum too low: qoverp=" << updatedQoverP);
            return FitterStatusCode::UpdateFailure;
      }

      ////////////////////////////////////////////////////////////////////
      // analyse the fit quality, possibly flag as outlier
      if ( runOutlier && fitQS->chiSquared() >  m_StateChiSquaredPerNumberDoFPreCut
           * fitQS->numberDoF() ) {
          
        // allow making of wire-hits for TRT fits with L/R-solving on the fly
        if ( m_recalibrator && it->measurementType() == TrackState::TRT) {
          const AmgVector(5) x = it->referenceParameters()->parameters()+(*predDiffPar);
          const Trk::TrackParameters* param = CREATE_PARAMETERS(*it->referenceParameters(),x,nullptr);
          if ( Trk::SensorBoundsCheck::areParamsInside
                (*fittableMeasurement, param->parameters(), *predDiffCov, 1.0, -1.0)) {
            Trk::TrackState::CalibrationType oldType = it->calibrationType();
            it->replaceMeasurement
              ( m_recalibrator->makeBroadMeasurement(*it->measurement(),*param,
                                                     TrackState::TRT),
                m_recalibrator->calibrationStatus(*it->measurement(),TrackState::TRT));
            fittableMeasurement = it->measurement();
	    ATH_MSG_VERBOSE ("TRT ROT calibration changed, from "<<oldType<<" to broad hit "<<
                           it->calibrationType()<< " instead of outlier");
            delete fitQS; fitQS=nullptr;
            updatedDifference.reset( 
              m_updator->updateParameterDifference(*predDiffPar, *predDiffCov,
                                                   *(it->measurementDifference()),
                                                   fittableMeasurement->localCovariance(),
                                                   fittableMeasurement->localParameters().parameterKey(),
                                                   fitQS, /*doFQ=*/true ) );
            if ( (updatedDifference || !fitQS ||
                 (runOutlier && fitQS->chiSquared() > m_StateChiSquaredPerNumberDoFPreCut
                  * fitQS->numberDoF())) ) {
              ATH_MSG_DEBUG ("Chi2 ("<<fitQS->chiSquared()<<"/"<<fitQS->numberDoF()<<
                             ") too large, remove measurement from track fit.");
              it->isOutlier(Trk::TrackState::StateOutlier);
            }
          }
          delete param;
        } else {     
          ATH_MSG_DEBUG ( "failed Chi2 test, remove measurement from track fit." );
          it->isOutlier ( Trk::TrackState::StateOutlier );
        }
      } else if ( runOutlier
                 && (*predDiffCov)(Trk::phi,Trk::phi)   < 0.01*0.01
                 && (*predDiffCov)(Trk::theta,Trk::theta) < 0.01*0.01
	         && ! ( Trk::SensorBoundsCheck::areParamsInside
                          ( *fittableMeasurement, 
                            (it->referenceParameters()->parameters()+updatedDifference->first),
                            updatedDifference->second, 5.0, 4.0 ) ) ) {
        ATH_MSG_DEBUG ( "failed track-within-sensor test, remove measurement from fit." );
        it->isOutlier ( Trk::TrackState::SensorOutlier );
      }

      // reset current state in case it is now defined outlier
      if ( !it->isOutlier() ) {
        ATH_MSG_VERBOSE ( "update and coarse Chi2 ("<<fitQS->chiSquared() <<"/"<<
                          fitQS->numberDoF() <<") okay, add to trajectory" );
      } else {
        if ( it->positionOnTrajectory()==1 ) {
          // TO-DO investigate this! (TRT-only with PseudoMeast removed, and thus refPars not matching)
          ATH_MSG_WARNING ( "Failed on first filter step, this indicates bad inputs or job options!" );
          delete fitQS; //other objects managed by trajectory
          return FitterStatusCode::BadInput;
        }
        // copy over prePar to updatedPar because we ignore this update
        updatedDifference = std::make_unique<std::pair<AmgVector(5),AmgSymMatrix(5)>>( std::make_pair(*predDiffPar,*predDiffCov) );
      }
      // @TODO fitQS maybe NULL in case it points to an outlier.
      //    Shall this part of the code be reached for outliers ?
      if (fitQS) {
        it->setForwardStateFitQuality ( *fitQS );
        delete fitQS;
      }
    }
    // bremStateIfBremFound = NULL; 

    // extract jacobian which is the one to transport from this to next state
    jacobian = it->jacobian();
  }

  int testNumber = m_utility->rankedNumberOfMeasurements(trajectory);
  if (testNumber < 5) {
    ATH_MSG_DEBUG ("Filtered trajectory has only " << testNumber
                   << " and thus too few fittable measurements!");
    return FitterStatusCode::FewFittableMeasurements; // caller (KalmanFitter) will clean up...
  }

  if (msgLvl(MSG::DEBUG)) m_utility->dumpTrajectory(trajectory,"end of FKF");
  return FitterStatusCode::Success;
}



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// forward filter preparation: get filter onto 1st state and assign big error //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
Trk::FitterStatusCode Trk::ForwardRefTrackKalmanFitter::enterSeedIntoTrajectory
(Trajectory&            trajectory,
 const Trk::TrackParameters& inputPar,
 const std::vector<double>& cov0,
 const Trk::KalmanMatEffectsController& controlledMatEffects,
 bool  createReferenceTrajectory) const {


  Trk::Trajectory::iterator ffs = m_utility->firstFittableState(trajectory);
  
  if (createReferenceTrajectory && ffs->jacobian()) {
    ATH_MSG_WARNING ("internal mistake: reference parameters should be recreated only on"<<
                     " EMPTY trajectory.");
    return FitterStatusCode::BadInput;
  }
  if (!createReferenceTrajectory && !ffs->jacobian() && !ffs->referenceParameters()) {
    ATH_MSG_WARNING ("internal mistake: re-making of reference parameters OFF but "<<
                     "trajectory is EMPTY.\n This can not work!");
    return FitterStatusCode::BadInput;
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
    inputParAtStartSurface = m_extrapolator->extrapolateDirectly(inputPar,
                                                                 startSurface,
                                                                 Trk::anyDirection,
                                                                 false, Trk::nonInteracting);
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

  // make first seed for filtering
  AmgVector(5)* diffPar = nullptr;
  if (ffs->referenceParameters())
    diffPar = new AmgVector(5)(inputParAtStartSurface->parameters()
                               -ffs->referenceParameters()->parameters());
  else {
    diffPar = new AmgVector(5)(); diffPar->setZero();
  }
  ffs->checkinParametersDifference(diffPar);
  AmgSymMatrix(5)* cov = new AmgSymMatrix(5)(); cov->setZero();
  for (int i=0; i<5; ++i) (*cov)(i,i) = cov0[i];
  ffs->checkinParametersCovariance(const_cast<const AmgSymMatrix(5)*>(cov));

  // const Trk::TrackParameters* seedPar = CREATE_PARAMETERS((*inputParAtStartSurface),par, cov);
  // ffs->checkinForwardPar(seedPar);
  ATH_MSG_VERBOSE ("-Fe prepared trajectory with seed parameters on state "<<ffs->positionOnTrajectory());

  if (createReferenceTrajectory) {
    ATH_MSG_DEBUG("start to create or recreate the reference trajectory+Jacobians");

    // lazy initialization because the tracking geometry is built after conditions are there
    Trk::MagneticFieldProperties useFullField(Trk::FullField);
    if (m_idEnclosingVolume == nullptr ) m_idEnclosingVolume = 
      m_extrapolator->trackingGeometry()->trackingVolume("InDet::Containers::InnerDetector");
    if (m_msCompleteVolume == nullptr )  m_msCompleteVolume = 
      m_extrapolator->trackingGeometry()->trackingVolume("All::Container::CompleteDetector");
    if (m_idEnclosingVolume==nullptr && m_msCompleteVolume == nullptr) {
      ATH_MSG_WARNING ("Could not get any detector enclosing volume from tracking geometry.");
      return FitterStatusCode::BadInput;
    }
    const IPropagator* propagator = nullptr;
    bool  isInsideID = true;
    if (m_idEnclosingVolume && m_idEnclosingVolume->inside(inputParAtStartSurface->position())) {
      ATH_MSG_VERBOSE ("subPropagator detection: found id volume of signature ID " << 
      (unsigned int)m_idEnclosingVolume->geometrySignature());
      propagator = m_extrapolator->subPropagator(*m_idEnclosingVolume);
    } else if (m_msCompleteVolume) {
      propagator = m_extrapolator->subPropagator(*m_msCompleteVolume);
      isInsideID = false;
      ATH_MSG_VERBOSE ("subPropagator detector: found MS enclosing volume with signature ID " << 
                       (unsigned int)m_msCompleteVolume->geometrySignature());
    } else {
      ATH_MSG_WARNING ("No way to get the correct propagator!");
      return FitterStatusCode::BadInput;
    }

    const Trk::TrackParameters* lastPropagatedPar = 
      CREATE_PARAMETERS((*inputParAtStartSurface),
                        inputParAtStartSurface->parameters(), nullptr); // remove covariance
    for (auto it=m_utility->firstFittableState ( trajectory ); it!=trajectory.end(); ++it) {
      if (!it->referenceParameters()) {
        it->checkinReferenceParameters (lastPropagatedPar);
      } else {
        delete lastPropagatedPar; lastPropagatedPar=nullptr;
        // FIXME study this further, whether cov really not needed and how close in param
        // space the old and new references are.
        ATH_MSG_VERBOSE("At state T"<<it->positionOnTrajectory()<<" have already reference.");
        if (it->referenceParameters()->covariance()) 
          ATH_MSG_DEBUG("Warning - reference parameters have a covariance, which is unnecessary.");
      }
      if ((it+1) == trajectory.end()) break; // no jacobian beyond last trajectory state
      const Trk::Surface& nextSurface = *((it+1)->surface());
      if (isInsideID && !m_idEnclosingVolume->inside(nextSurface.center())
          && m_msCompleteVolume) {
	isInsideID = false;
        propagator = m_extrapolator->subPropagator(*m_msCompleteVolume);
        ATH_MSG_DEBUG ("changed from ID to MS propagator.");
      } else if (!isInsideID && m_idEnclosingVolume 
                 && m_idEnclosingVolume->inside(nextSurface.center())) {
        isInsideID = true;
        propagator = m_extrapolator->subPropagator(*m_idEnclosingVolume);
        ATH_MSG_DEBUG ("changed from MS to ID propagator.");
      }
      TransportJacobian* jac = nullptr;
      double pathLimit = -1.;
      if (!propagator) {
        ATH_MSG_WARNING("NO propagator!!");
        return FitterStatusCode::BadInput;
      }
      lastPropagatedPar = propagator->propagate(*it->referenceParameters(),
                                                nextSurface,
                                                Trk::anyDirection, 
                                                /*bcheck=*/ false,
                                                useFullField,jac, pathLimit,
                                                controlledMatEffects.particleType() );
      if (!jac || !lastPropagatedPar) {
        ATH_MSG_WARNING ("lost reference track while propagating.");
        delete jac;
        delete lastPropagatedPar;
        return FitterStatusCode::BadInput;
      }
      it->checkinTransportJacobian(jac,true);
      ATH_MSG_VERBOSE ("Done with state T"<<it->positionOnTrajectory());
    }
  } else ATH_MSG_DEBUG("trajectory prepared without touching reference trajectory");

  if (inputParAtStartSurface != &inputPar) delete inputParAtStartSurface;

  return FitterStatusCode::Success;
}


// private -- helper to make pretty debug output
void Trk::ForwardRefTrackKalmanFitter::printGlobalParams(int istate, const std::string& ptype,
                                                         const Trk::TrackParameters& ref,
                                                         const AmgVector(5)& diff) const
{
  const AmgVector(5) x = ref.parameters()+diff;
  const Trk::TrackParameters* param = CREATE_PARAMETERS(ref,x,nullptr);
  char tt[80]; snprintf(tt,79,"T%.2d",istate);
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
  delete param;
}
