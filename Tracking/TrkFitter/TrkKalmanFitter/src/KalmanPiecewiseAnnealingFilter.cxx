/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KalmanPiecewiseAnnealingFilter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkKalmanFitter/KalmanPiecewiseAnnealingFilter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkToolInterfaces/ICompetingRIOsOnTrackTool.h"
#include "TrkFitterInterfaces/IMeasurementRecalibrator.h"
#include "TrkFitterInterfaces/IDynamicNoiseAdjustor.h"
#include "TrkFitterUtils/ProtoTrajectoryUtility.h"
#include "TrkFitterUtils/ProtoMaterialEffects.h"
#include "TrkEventUtils/RoT_Extractor.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

//================ Constructor =================================================

Trk::KalmanPiecewiseAnnealingFilter::KalmanPiecewiseAnnealingFilter(const std::string& t,
								    const std::string& n,
								    const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_forwardFitter("Trk::ForwardKalmanFitter/InDetAnnealFKF"),
  m_smoother     ("Trk::KalmanSmoother/InDetAnnealBKS"),
  m_compRotTool  ("Trk::CompetingRIOsOnTrackTool/KalmanCompROT_Tool"),
  m_option_outlierCut(0.20),
  m_extrapolator(nullptr),
  m_updator(nullptr),
  m_recalibrator(nullptr),
  m_dynamicNoiseAdjustor(nullptr),
  m_trajPiece{},
  m_utility(nullptr),
  m_chi2DuringAnnealing{},
  m_particleMasses{}
{
  declareInterface<IKalmanPiecewiseAnnealingFilter>(this);

  m_option_annealingScheme.clear();
  m_option_annealingScheme.push_back(2048);
  m_option_annealingScheme.push_back(256.);
  //  m_option_annealingScheme.push_back(144);//(81.);
  m_option_annealingScheme.push_back(16.);
  m_option_annealingScheme.push_back(4.);
  m_option_annealingScheme.push_back(1.);
  m_chi2DuringAnnealing.resize(m_option_annealingScheme.size(), 0.0);

  declareProperty("ForwardFitter",          m_forwardFitter);
  declareProperty("BackwardSmoother",       m_smoother);
  declareProperty("CompetingRIOsOnTrackCreator", m_compRotTool);
  declareProperty("AnnealingScheme",        m_option_annealingScheme);
  declareProperty("OutlierAssgnProbCut",    m_option_outlierCut);
}

//================ Destructor =================================================

Trk::KalmanPiecewiseAnnealingFilter::~KalmanPiecewiseAnnealingFilter()
{}


//================ Initialisation =================================================

StatusCode Trk::KalmanPiecewiseAnnealingFilter::initialize()
{
  
  StatusCode sc = AlgTool::initialize();

  if (sc.isFailure()) return sc;
  m_trajPiece.reserve(100);

  // get Forward Filter and Backward Smoother as private tools
  if (m_forwardFitter.retrieve().isFailure()) {
    ATH_MSG_ERROR ("Could not retrieve "<< m_forwardFitter <<" (forward filter) ");
    return sc;
  }
  if (m_smoother.retrieve().isFailure()) {
    ATH_MSG_ERROR ("Could not retrieve "<< m_smoother <<" (Kalman Smoother) ");
    return sc;
  }
  if (m_compRotTool.retrieve().isFailure()) {
    ATH_MSG_ERROR ("can not retrieve " <<
                   m_compRotTool.typeAndName() << " (Cluster-type CompRot-Maker)" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO ("annealing scheme for " << name() << " : " << m_option_annealingScheme);

  ATH_MSG_INFO ("initialize() successful in " << name() );
  return StatusCode::SUCCESS;
}

//================ Custom-Initialisation ========================================
StatusCode Trk::KalmanPiecewiseAnnealingFilter::configureWithTools
(const Trk::IExtrapolator*       extrap,
 const Trk::IUpdator*            updator,
 const Trk::IMeasurementRecalibrator* recalibrator,
 const Trk::ProtoTrajectoryUtility* utility,
 const Trk::IDynamicNoiseAdjustor* dna)
{
  if (extrap==NULL || updator == NULL || recalibrator == NULL || utility==NULL)
    return StatusCode::FAILURE;
  m_extrapolator = extrap;
  m_updator      = updator;
  m_recalibrator = recalibrator;
  m_utility      = utility;
  m_dynamicNoiseAdjustor = dna;

  // configure piecewise ForwardKalmanFitter
  StatusCode sc;
  sc = m_forwardFitter->configureWithTools(const_cast<Trk::IExtrapolator*>(m_extrapolator), m_updator,
					   0 /* no rot-creator needed */, m_dynamicNoiseAdjustor);
  if(sc.isFailure()) return sc;

  sc = m_smoother->configureWithTools(const_cast<Trk::IExtrapolator*>(m_extrapolator), m_updator,
				      m_dynamicNoiseAdjustor,
				      0,       // no alignable Surface Provider
				      true,    // always do smoothing
				      false);  // no creation of FitQualityOnSurface objects (DAF-like)
  if(sc.isFailure()) return sc;

  return StatusCode::SUCCESS;
}


//================ Finalising =================================================

StatusCode Trk::KalmanPiecewiseAnnealingFilter::finalize()
{
  return StatusCode::SUCCESS;
}


bool Trk::KalmanPiecewiseAnnealingFilter::annealingProblem_driftCircles
(const FitQuality& oldFitQ,
 const FitQuality& newFitQ,
 FitterStatusCode&  fwFilterResult,
 const int&  annealer,
 const bool& alreadyTried) const
{
  /* strategy to improve the piecewise annealing by claiming that the first
     measurement is 'bad' and should be removed as an outlier.
     Several (bad) conditions have to be fulfilled before this is tried.
     Note: to correct annealing it writes directly to the m_trajPiece private member!  */

  // stop if strategy won't work - like when it was already tried.
  Trk::Trajectory::const_iterator it = m_trajPiece.begin();
  if (it->isOutlier() || alreadyTried 
      || m_option_annealingScheme.at(annealer) <= 2.0 ) return false;

  // strategy only shown to improve on drift-circle initiating fits
  bool isDriftCircle
    = it->isDriftCircle() || (++it)->isDriftCircle() || (++it)->isDriftCircle();
  if (!isDriftCircle || oldFitQ.numberDoF() <= 0) return false;

  bool badAnnealing = (newFitQ.numberDoF() <= 0 && oldFitQ.numberDoF() > 0)
    || fwFilterResult.isFailure() ;

  // Flag cases of [1] increasing *and* [2] large chi2/ndf as sign of misguided annealing
  if (!badAnnealing)
    badAnnealing = (newFitQ.chiSquared()*oldFitQ.numberDoF()
                    > (oldFitQ.chiSquared()*newFitQ.numberDoF() + 10.0) )
      && (newFitQ.chiSquared()/newFitQ.numberDoF())>10.0;
  if (!badAnnealing) return false;

  ATH_MSG_DEBUG ("annealing in TRT deemed to be failing!   FKF-FQ : " << oldFitQ );

  const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>
    (m_trajPiece.front().measurement());
  const Trk::TrackParameters* tp = (m_trajPiece.front().smoothedTrackParameters() ?
                                    m_trajPiece.front().smoothedTrackParameters() :
                                    m_trajPiece.front().forwardTrackParameters());

  if (!crot || !tp || !m_trajPiece.front().isDriftCircle()
      || (m_trajPiece.front().measurement() &&
          fabs(m_trajPiece.front().measurement()->localParameters()[Trk::locR]) <= 0.01)) {
    m_trajPiece.front().isOutlier(Trk::TrackState::FilterOutlier, 1);
  } else {
    const Trk::RIO_OnTrack* recalibratedROT = 
      m_recalibrator->makeBroadMeasurement((crot->rioOnTrack(crot->indexOfMaxAssignProb())),*tp);
    if (recalibratedROT) 
      m_trajPiece.front().replaceMeasurement(recalibratedROT, Trk::TrackState::TubeHit);
    else
      m_trajPiece.front().isOutlier(Trk::TrackState::FilterOutlier, 1);
  }
  return true;
}

bool Trk::KalmanPiecewiseAnnealingFilter::annealingProblem_all
(const FitQuality& newFitQ,
 const int&  annealer,
 const bool& alreadyTried) const
{
  /* strategy to improve the piecewise annealing by preventing high-precision
     clusters to lose assignment probability.

     Note: to correct annealing it writes directly to the m_trajPiece private member! */

  // stop if strategy won't work - like when it was already tried.
  Trk::Trajectory::iterator it = m_trajPiece.begin();
  if (alreadyTried || annealer != (int)m_option_annealingScheme.size()-2 ) return false;

  // strategy only shown to improve on drift-circle initiating fits
  bool isDriftCircle
    = it->isDriftCircle() || (++it)->isDriftCircle() || (++it)->isDriftCircle();
  if (isDriftCircle || newFitQ.chiSquared() <= 0.0 || newFitQ.numberDoF() <= 0) return false;

  double ffProb = 1.0-Genfun::CumulativeChiSquare(newFitQ.numberDoF())(newFitQ.chiSquared());
  if (ffProb > 0.001) return false;

  ATH_MSG_DEBUG ("deemed worth trying broadcluster improvement ");
  bool foundImprovement = false;

  for(Trajectory::iterator it = m_trajPiece.begin(); it != m_trajPiece.end(); ++it) {
    const Trk::CompetingRIOsOnTrack* compROT = 
      dynamic_cast<const Trk::CompetingRIOsOnTrack*>(it->measurement());
    if (compROT == NULL || it->isOutlier()) continue;

    double assPrb = compROT->assignmentProbability(compROT->indexOfMaxAssignProb());
    double msErrX = sqrt(compROT->rioOnTrack(compROT->indexOfMaxAssignProb()).localCovariance()(Trk::locX,Trk::locX));
    double msErrY = (compROT->localParameters().contains(Trk::locY) ?
                     sqrt(compROT->rioOnTrack(compROT->indexOfMaxAssignProb()).localCovariance()(Trk::locY,Trk::locY)) :
		     999.0);
    bool   highPrecisionPixel = (it->measurementType() == Trk::TrackState::Pixel)
      && (msErrX < 0.010 || msErrY < 0.080);
    bool   highPrecisionSCT = (it->measurementType() == Trk::TrackState::SCT)
      && (msErrX < 0.018);
    double chi2ts = (it->fitQuality()? it->fitQuality()->chiSquared() : -999.0);
    const Trk::TrackParameters* refPars = it->smoothedTrackParameters() ? 
      it->smoothedTrackParameters() : it->forwardTrackParameters();

    if ( (assPrb < 0.90 || chi2ts > 1.0)
         && (highPrecisionPixel || highPrecisionSCT)
         && refPars ) {
      const Trk::RIO_OnTrack* broadCluster = 
        m_recalibrator->makeBroadMeasurement(*compROT, *refPars,
                                             it->measurementType());
      if (broadCluster!=NULL) {
        ATH_MSG_DEBUG ("annealing in silicon deemed to be failing, replace CompROT-cluster dLocX="<<
                       sqrt(compROT->rioOnTrack(compROT->indexOfMaxAssignProb()).localCovariance()(Trk::locX,Trk::locX))
		       <<" by hit with dLocX="<<sqrt(broadCluster->localCovariance()(Trk::locX,Trk::locX)));
	Trk::CompetingRIOsOnTrack* modifiableCompROT
	  = const_cast<Trk::CompetingRIOsOnTrack*>(compROT);
	m_compRotTool->reequipCompetingROT(modifiableCompROT,broadCluster,
					   *refPars, m_option_annealingScheme.at(annealer),
					   it->measurementType());
	it->isOutlier(false);
	foundImprovement = true;
      }
    }
  }
  return foundImprovement;

}

bool Trk::KalmanPiecewiseAnnealingFilter::annealingProblem_all
(const FitQuality& oldFitQ,
 const FitQuality& newFitQ,
 const int&  annealer,
 const bool& alreadyTried) const
{
  /* strategy to improve the piecewise annealing by preventing high-precision
     clusters to lose assignment probability.

     Note: to correct annealing it writes directly to the m_trajPiece private member! */

  // stop if strategy won't work - like when it was already tried.
  if (alreadyTried ) return false;

  // strategy only shown to improve on drift-circle initiating fits
  Trk::Trajectory::iterator it = m_trajPiece.begin();
  bool isDriftCircle
    = it->isDriftCircle() || (++it)->isDriftCircle() || (++it)->isDriftCircle();
  if (isDriftCircle || newFitQ.chiSquared() <= 0.0 || newFitQ.numberDoF() <= 0) return false;

  double ffProb = 1.0-Genfun::CumulativeChiSquare(newFitQ.numberDoF())(newFitQ.chiSquared());

  bool badAnnealing = (newFitQ.chiSquared()*oldFitQ.numberDoF()
		       > (oldFitQ.chiSquared()*newFitQ.numberDoF() + 5.0) )
    && ffProb < 0.001;
  if (!badAnnealing) return false;

  ATH_MSG_DEBUG ("deemed worth trying broadcluster improvement.");
  bool foundImprovement = false;

  for(Trajectory::iterator it = m_trajPiece.begin(); it != m_trajPiece.end(); ++it) {
    const Trk::CompetingRIOsOnTrack* compROT = 
      dynamic_cast<const Trk::CompetingRIOsOnTrack*>(it->measurement());
    if (compROT == NULL || it->isOutlier()) continue;

    double msErrX = sqrt(compROT->rioOnTrack(compROT->indexOfMaxAssignProb()).localCovariance()(Trk::locX,Trk::locX));
    double msErrY = (compROT->localParameters().contains(Trk::locY) ?
		     sqrt(compROT->rioOnTrack(compROT->indexOfMaxAssignProb()).localCovariance()(Trk::locY,Trk::locY))     :
		     999.0);
    bool   highPrecisionPixel = (it->measurementType() == Trk::TrackState::Pixel)
      && (msErrX < 0.010 || msErrY < 0.080);
    bool   highPrecisionSCT = (it->measurementType() == Trk::TrackState::SCT)
      && (msErrX < 0.018);
    const Trk::TrackParameters* refPars = it->smoothedTrackParameters() ? 
      it->smoothedTrackParameters() : it->forwardTrackParameters();

    if ( (highPrecisionPixel || highPrecisionSCT)
	 && refPars ) {
      const Trk::RIO_OnTrack* broadCluster = 
	m_recalibrator->makeBroadMeasurement(*compROT, *refPars,
					     it->measurementType());
      if (broadCluster!=NULL) {

	ATH_MSG_DEBUG ("annealing in silicon deemed to be failing, replace CompROT-cluster dLocX="<<
		       sqrt(compROT->rioOnTrack(compROT->indexOfMaxAssignProb()).localCovariance()(Trk::locX,Trk::locX))
		       <<" by hit with dLocX="<<(broadCluster->localCovariance()(Trk::locX,Trk::locX)));
	double beta      = ( annealer+1 >= (int)m_option_annealingScheme.size() ?
			     1.0                                         :
			     m_option_annealingScheme.at(annealer+1)     );
	Trk::CompetingRIOsOnTrack* modifiableCompROT
	  = const_cast<Trk::CompetingRIOsOnTrack*>(compROT);
	m_compRotTool->reequipCompetingROT(modifiableCompROT,broadCluster,
					   *refPars, beta, it->measurementType());
	it->isOutlier(false);
	foundImprovement = true;
      }
    }
  }

  return foundImprovement;

}

const Trk::FitterStatusCode
Trk::KalmanPiecewiseAnnealingFilter::filterTrajectory
(Trajectory& trajectory, const ParticleHypothesis&   particleType) const
{
  const Trk::TrackParameters* predPar = 0;
  const Trk::TrackParameters* updPar  = 0;
  Trk::Trajectory::iterator  start = m_utility->firstFittableState(trajectory);
  return this->filterTrajectoryPiece(trajectory, start, updPar, predPar,
                                     trajectory.size(), particleType);
}

//================ main method: the piece-wise filter ==========================

const Trk::FitterStatusCode
Trk::KalmanPiecewiseAnnealingFilter::filterTrajectoryPiece 
(Trajectory& trajectory,
 Trajectory::iterator& start,
 const TrackParameters*&     start_updatedPar,
 const TrackParameters*&     start_predPar,
 int                         pieceSize,
 const ParticleHypothesis&   particleType) const
{

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "entering filterTrajectoryPiece() with annealing scheme";
    for (unsigned int annealer=0; annealer < m_option_annealingScheme.size(); ++annealer)
      msg(MSG::DEBUG) <<" " << m_option_annealingScheme.at(annealer);
    msg(MSG::DEBUG) << endmsg;
  }
  if (pieceSize<3) pieceSize=8; // some reasonable size for annealing
  Trk::KalmanMatEffectsController dafMec(particleType, m_dynamicNoiseAdjustor!=NULL);

  //////////////////////////////////////////////////////////////////////////////
  // step 1: make first loop while turning ROT/PRDs into competing ones
  //////////////////////////////////////////////////////////////////////////////
  const TrackParameters* predPar     = 0;
  std::unique_ptr<const AmgVector(5)> predDiffPar;
  const TrackParameters* updatedPar  = 0;
  bool end_reached = false;
  bool hadAnnealingProblemDC = false;
  bool hadAnnealingProblem   = false;
  int iDafSteps    = 1;
  double beta      = m_option_annealingScheme.at(0);
  Trajectory::iterator   input_it = start;
  for( ; !end_reached; ++input_it, ++iDafSteps) {

    ATH_MSG_VERBOSE ("Filling Annealing trajectory, state " << iDafSteps);
    if (iDafSteps == 1) {
      if (input_it->referenceParameters()) {
        if (start_predPar) {
          predPar = start_predPar->clone();
          predDiffPar.reset(  new AmgVector(5)(start_predPar->parameters()
                                               - start->referenceParameters()->parameters() ) );
        } else {
          predDiffPar.reset(  start->checkoutParametersDifference() );
          predPar = CREATE_PARAMETERS(*start->referenceParameters(),
                                      (start->referenceParameters()->parameters() + (*predDiffPar)),
                                      new AmgSymMatrix(5)(*start->parametersCovariance()));
          delete start->checkoutParametersCovariance();
        }
      } else {
        if (start_predPar) predPar     = start_predPar->clone();
        else predPar = start->checkoutForwardPar();
      }
    } else {
      if (!input_it->referenceParameters()) {
        // filter using extrapolator
        predPar = m_extrapolator->extrapolate(*updatedPar,
					      *input_it->surface(),
					      Trk::alongMomentum,
					      false, particleType);
      } else {
        // filter using differences
        const TransportJacobian& jac = *m_trajPiece.back().jacobian();
        AmgVector(5) updDiffPar = updatedPar->parameters() 
                                  - m_trajPiece.back().referenceParameters()->parameters();
        predDiffPar.reset(  new AmgVector(5)(jac*updDiffPar) );
        AmgSymMatrix(5)* C = new AmgSymMatrix(5) (jac*(*updatedPar->covariance())*jac.transpose());
        // add uncertainties from material effects:
        if (input_it->materialEffects()) {
          (*C)(Trk::phi,Trk::phi)     += std::pow(input_it->materialEffects()->sigmaDeltaPhi(),2);
          (*C)(Trk::theta,Trk::theta) += std::pow(input_it->materialEffects()->sigmaDeltaTheta(),2);
          const double& mass = m_particleMasses.mass[particleType];
          const double currQOverPsquared = std::pow((*predDiffPar)(Trk::qOverP) + input_it->referenceParameters()->parameters()[Trk::qOverP], 2);
          const double sigmaDeltaQoverPsquared = (currQOverPsquared > 0.) ? (std::pow(input_it->materialEffects()->sigmaDeltaE(), 2) * (mass*mass + 1./currQOverPsquared) * (currQOverPsquared*currQOverPsquared*currQOverPsquared)) : 0.;
          (*C)(Trk::qOverP,Trk::qOverP)+= sigmaDeltaQoverPsquared;
          ATH_MSG_VERBOSE ("mass=" << mass << ", qOverP_ref="<< std::scientific << input_it->referenceParameters()->parameters()[Trk::qOverP]
                  << ", qOverP_diff=" << (*predDiffPar)(Trk::qOverP)
                  << ", sigmaDeltaE=" << input_it->materialEffects()->sigmaDeltaE()
                  << ", sigmaDeltaQoverP=" << sqrt(sigmaDeltaQoverPsquared) << std::fixed);//std::defaultfloat);
          ATH_MSG_VERBOSE ("Added material effects.");
        }
        const AmgVector(5) x = input_it->referenceParameters()->parameters()+(*predDiffPar);
        predPar = CREATE_PARAMETERS(*input_it->referenceParameters(),x,C);
        ATH_MSG_DEBUG("used difference to make predpar = " << *predPar);
      }
    }

    if (!predPar) {
      ATH_MSG_INFO ("lost track piece in filterTrajPiece() annealing "<<beta <<
		    " and node " << iDafSteps );
      delete updatedPar;
      m_trajPiece.clear();
      return Trk::FitterStatusCode::ForwardFilterFailure;
    }

    const Trk::RIO_OnTrack* testROT 
      = dynamic_cast<const Trk::RIO_OnTrack*>(input_it->measurement());

    if (input_it->isOutlier() || testROT==NULL || input_it->measurement()==NULL) {
      // outlier, material and unambiguous hits: *COPY* into piecewise trajectory.

      if (input_it->measurement())
        m_trajPiece.push_back(ProtoTrackStateOnSurface(input_it->measurement(),
                                                       input_it->isOutlier(),false,iDafSteps));
      else {
        m_trajPiece.push_back(ProtoTrackStateOnSurface());
        m_trajPiece.back().checkinMaterialEffects
              (new Trk::ProtoMaterialEffects(*input_it->materialEffects()));
        m_trajPiece.back().isOutlier(TrackState::Scatterer);
        m_trajPiece.back().positionOnTrajectory(iDafSteps);
      }
      m_trajPiece.back().setMeasurementType(input_it->measurementType(),
					    input_it->calibrationType());
    } else {

      // --- make a simple competing ROT: Left/right ambiguity for drift circles and
      //     competition against outlier hypothesis for (unambiguous) clusters

      const Trk::CompetingRIOsOnTrack* highBetaCluster = 
	m_compRotTool->createSimpleCompetingROT(*testROT->prepRawData(),*predPar,beta);
      if (!highBetaCluster) {
	ATH_MSG_DEBUG ("CompROT creation failed in filterTrajPiece() annealing " <<beta);
	delete updatedPar;
	m_trajPiece.clear();
	return Trk::FitterStatusCode::ForwardFilterFailure; // not really... don't have other.
      }
      if (input_it->isCluster() && msgLvl(MSG::VERBOSE))
	std::cout << "DAF's CompCluster: " << *highBetaCluster << std::endl;
      Trk::ProtoTrackStateOnSurface newState = 
	Trk::ProtoTrackStateOnSurface(highBetaCluster,false,true,iDafSteps);
      m_trajPiece.push_back(newState);
      m_trajPiece.back().setMeasurementType(input_it->measurementType(),
					    Trk::TrackState::HighPrecision);
    } 

    if (m_forwardFitter->needsReferenceTrajectory()) {
      if (!input_it->referenceParameters()) {
        ATH_MSG_WARNING ("No reference trajectory in original data, can not create internal DAF");
        m_trajPiece.clear();
        return Trk::FitterStatusCode::UpdateFailure;
      }
      // if (input_it->jacobian()) m_trajPiece.back().checkinTransportJacobian(new Trk::TransportJacobian(*input_it->jacobian()));
      if (input_it->jacobian()) m_trajPiece.back().checkinTransportJacobian(input_it->jacobian(),false);
      m_trajPiece.back().checkinReferenceParameters(input_it->referenceParameters(),false);
      m_trajPiece.back().checkinParametersDifference(predDiffPar.release() );
      m_trajPiece.back().checkinParametersCovariance(new AmgSymMatrix(5)(*predPar->covariance()));
    }
 
    if (input_it->isOutlier() || !m_trajPiece.back().measurement() ) {
      delete updatedPar;
      updatedPar = predPar;
      predPar    = 0;
    } else {
      m_trajPiece.back().checkinForwardPar(predPar);
      delete updatedPar;
      FitQualityOnSurface* fitQuality=0;
      updatedPar = m_updator->addToState(*predPar,
					 m_trajPiece.back().measurement()->localParameters(),
					 m_trajPiece.back().measurement()->localCovariance(),
					 fitQuality);
      if (!updatedPar) {
	ATH_MSG_DEBUG ("addToState failed in filterTrajPiece(), annealing "<<beta);
	m_trajPiece.clear();
	delete fitQuality;
	return Trk::FitterStatusCode::UpdateFailure;
      }
      if (fitQuality != NULL) {
	m_trajPiece.back().setForwardStateFitQuality(*fitQuality);
	delete fitQuality;
      }
    }
    end_reached = (iDafSteps >= pieceSize)
      || (input_it+1 == trajectory.end());
  }
  delete updatedPar;

  FitQuality  currForwardFQ = m_utility->forwardFilterQuality(m_trajPiece);
  m_chi2DuringAnnealing[0] = currForwardFQ.numberDoF()>0 ? 
    currForwardFQ.chiSquared()/currForwardFQ.numberDoF() : 0.0;

  //////////////////////////////////////////////////////////////////////////////
  // === step 2: enter annealing loop acting on pairs of forward-backward filters
  //////////////////////////////////////////////////////////////////////////////
  FitQuality* newFitQuality  = 0;
  FitQuality  savedForwardFQ = m_utility->forwardFilterQuality(m_trajPiece);
  for (unsigned int annealer=0; annealer < m_option_annealingScheme.size(); ++annealer) {

    // skip 1st annealing iteration, since that is done 'manually' in step 1 to get CompROTs made
    if (annealer>0) {
      // re-set trajectory and possibly advance seed pars from main trajectory
      Trk::Trajectory::iterator ffs = m_utility->firstFittableState(m_trajPiece);
      m_utility->clearFitResultsAfterOutlier(m_trajPiece,newFitQuality,ffs->positionOnTrajectory()+1);
      if (!ffs->forwardTrackParameters() && !ffs->parametersDifference()) {
        ATH_MSG_WARNING ("Programming error - lost seed");
        m_utility->dumpTrajectory(m_trajPiece, name());
      }
	/* old code for lost seed: 
          const TrackParameters* newPredPar = 
	  m_extrapolator->extrapolate(*start_predPar,
				      ffs->measurement()->associatedSurface(),
				      Trk::alongMomentum, false, particleType); */
      ATH_MSG_DEBUG ("entering FKF at annealing factor " <<beta);
      FitterStatusCode fitStatus = m_forwardFitter->fit(m_trajPiece,
                                                        *start->forwardTrackParameters(), // no check needed: void in refKF
							false /*no runOutlier*/, dafMec,
							false, 1);
      FitQuality  currForwardFQ = m_utility->forwardFilterQuality(m_trajPiece);
      m_chi2DuringAnnealing[annealer] = currForwardFQ.numberDoF()>0 ? 
	currForwardFQ.chiSquared()/currForwardFQ.numberDoF() : 0.0;
      
      if ( this->annealingProblem_driftCircles(savedForwardFQ,currForwardFQ,
					       fitStatus,annealer,hadAnnealingProblemDC) ) {
	ATH_MSG_DEBUG ("AnnMonitor-DC sends piecewise DAF back to start!");
	hadAnnealingProblemDC = true;
	--annealer; // go back one step!
	continue;
      }
      if ( this->annealingProblem_all(savedForwardFQ,currForwardFQ,
				      annealer,hadAnnealingProblem) ) {
	ATH_MSG_DEBUG ("AnnMonitor-all sends piecewise DAF back to start! ");
	hadAnnealingProblem = true;
	--annealer; // go back one step!
	continue;
      }
      savedForwardFQ = currForwardFQ;

      if (fitStatus.isFailure()) {
	ATH_MSG_DEBUG ("internal fw.-filter failed at annealing factor "<<beta);
	m_trajPiece.clear();
	return fitStatus;
      }
    }

    double assgnProbSum = 0.0;
    for(Trajectory::iterator it = m_trajPiece.begin(); it != m_trajPiece.end(); ++it) {
      const Trk::CompetingRIOsOnTrack* compROT = 
	dynamic_cast<const Trk::CompetingRIOsOnTrack*>(it->measurement());
      if (compROT != NULL && !it->isOutlier()) assgnProbSum += 
						 compROT->assignmentProbability(compROT->indexOfMaxAssignProb());
    }
    if (assgnProbSum < 0.5) {
      if (msgLvl(MSG::DEBUG)) {
	msg(MSG::DEBUG) << "not enough assignment probability left in entire trajectory "
			<< "piece! Stopping." << endmsg;
	m_utility->dumpTrajectory(m_trajPiece, name());
      }
      m_trajPiece.clear();
      return Trk::FitterStatusCode::ForwardFilterFailure;
    }

    // --- call backward filter
    ATH_MSG_DEBUG ("entering smoother at annealing factor " <<beta);
    FitterStatusCode fitStatus = Trk::FitterStatusCode::ForwardFilterFailure;
    if (m_forwardFitter->needsReferenceTrajectory())
      fitStatus = m_smoother->fitWithReference(m_trajPiece, newFitQuality, dafMec);
    else
      fitStatus = m_smoother->fit(m_trajPiece, newFitQuality, dafMec);

    if (fitStatus.isFailure()) {
      ATH_MSG_DEBUG ("internal backward smoother failed at annealing factor "<<beta);
      m_trajPiece.clear();
      return fitStatus;
    }
    if (m_trajPiece.begin()->forwardTrackParameters() ) 
      ATH_MSG_DEBUG ("check traj consistency " << *m_trajPiece.begin()->forwardTrackParameters() );

    beta      = ( annealer+1 >= m_option_annealingScheme.size() ?
		  1.0                                         :
		  m_option_annealingScheme.at(annealer+1)     );
    ATH_MSG_DEBUG ("updating CompROT assignment probs with annealing factor " <<beta);

    // --loop and lower the annealing temperature
    for(Trajectory::iterator it = m_trajPiece.begin(); it != m_trajPiece.end(); ++it) {
      const Trk::CompetingRIOsOnTrack* compROT = 
	dynamic_cast<const Trk::CompetingRIOsOnTrack*>(it->measurement());
      if (compROT && it->smoothedTrackParameters() && !it->isOutlier()) {
        const Trk::TrackParameters* annealingPars = it->smoothedTrackParameters();
        Trk::CompetingRIOsOnTrack* newCompROT = compROT->clone();
        m_compRotTool->updateCompetingROT(*newCompROT, *annealingPars, beta);
        it->replaceMeasurement(newCompROT);
        compROT = newCompROT;
      }
    }

    if (msgLvl(MSG::VERBOSE)) {
      for(Trajectory::iterator it = m_trajPiece.begin(); it != m_trajPiece.end(); ++it) {
        const Trk::CompetingRIOsOnTrack* compROT = 
	  dynamic_cast<const Trk::CompetingRIOsOnTrack*>(it->measurement());
        const Trk::FitQuality* testFQ = 0;
        if (it->smoothedTrackParameters() && it->measurement()) {
	  testFQ = m_updator->fullStateFitQuality(*(it->smoothedTrackParameters()),
						  it->measurement()->localParameters(),
						  it->measurement()->localCovariance());
	  std::cout << std::setiosflags(std::ios::fixed) << "results: " << it->positionOnTrajectory()
                    << " smooPar " << it->smoothedTrackParameters()->parameters()[Trk::locR]
                    << " meas="    << it->measurement()->localParameters()[Trk::locX]
                    << " chi2=" << (it->fitQuality()? it->fitQuality()->chiSquared() : -999.0)
                    << " test=" << (testFQ ? testFQ->chiSquared() : -999.0)
                    << " max=" << (compROT? compROT->rioOnTrack(compROT->indexOfMaxAssignProb()).localParameters()[Trk::locX] : -999.0)
                    <<" prob="
                    << (compROT? compROT->assignmentProbability(compROT->indexOfMaxAssignProb()) : -999.0)
                    << std::endl;
          delete testFQ;
        }
      }
    }

  }

  //////////////////////////////////////////////////////////////////////////////
  // step 3: copy data back to main trajectory, condensing CompROT to standard ROT
  //////////////////////////////////////////////////////////////////////////////
  ATH_MSG_DEBUG ("Chi2 development: " << m_chi2DuringAnnealing );

  Trajectory::iterator it = m_trajPiece.begin();
  Trajectory::iterator fittableState = start, itForKF = start;
  for(; it != m_trajPiece.end(); ++it, ++itForKF) {
    if (itForKF>=trajectory.end()) {
      ATH_MSG_WARNING ("wrong counting in traj_piece -> main_traj copy!" );
      m_trajPiece.clear(); delete newFitQuality;
      return Trk::FitterStatusCode::BadInput;
    }
    if (!it->isOutlier()) fittableState = itForKF;
    const Trk::CompetingRIOsOnTrack* compROT = 
      dynamic_cast<const Trk::CompetingRIOsOnTrack*>(it->measurement());

    if (msgLvl(MSG::VERBOSE) && it->measurement() && itForKF->measurement()) 
      std::cout << std::setiosflags(std::ios::fixed)
		<< "results: " << itForKF->positionOnTrajectory()
		<< " predict " << (it->smoothedTrackParameters() ?
				   it->smoothedTrackParameters()->parameters()[Trk::locR] : -999.0)
		<< " old=" << itForKF->measurement()->localParameters()[Trk::locX]
		<< " vs eff=" << it->measurement()->localParameters()[Trk::locX] << " max="
		<< (compROT? compROT->rioOnTrack(compROT->indexOfMaxAssignProb()).localParameters()[Trk::locX] : -999.0)
		<<" prob="
		<< (compROT? compROT->assignmentProbability(compROT->indexOfMaxAssignProb()) : -999.0);
    
    std::string decision = "ex. outlier";
    if (it->isOutlier() && itForKF->trackStateType() != Trk::TrackState::ExternalOutlier)
      itForKF->isOutlier(Trk::TrackState::FilterOutlier);

    if ( !it->isOutlier() && !compROT 
	 && (it->measurement() != itForKF->measurement()) ) {
      itForKF->replaceMeasurement(it->checkoutMeasurement());
      itForKF->isOutlier(false);
      decision = "new measurement";
    }
    if (!it->isOutlier() && compROT) {
      double predictedLocalR = it->smoothedTrackParameters()->parameters()[Trk::locR];
      double assProb = compROT->assignmentProbability(compROT->indexOfMaxAssignProb());
      const PrepRawData* prd = compROT->rioOnTrack(compROT->indexOfMaxAssignProb()).prepRawData(); 
      if (it->isDriftCircle()) {
	if (assProb > 1.5*m_option_outlierCut) {
	  if (fabs(prd->localPosition()[Trk::locR])<0.30) {
	    const Trk::RIO_OnTrack* recalibratedROT = 
	      m_recalibrator->makePreciseMeasurement(*compROT,*it->smoothedTrackParameters());
	    itForKF->replaceMeasurement(recalibratedROT,Trk::TrackState::Nominal);
	    itForKF->isOutlier(false);
	    decision = "drift circle (should be wire hit)";
	  } else {
	    itForKF->replaceMeasurement(compROT->rioOnTrack(compROT->indexOfMaxAssignProb()).clone());
	    decision = "drift circle (max prob)";
	  }
	} else if (fabs(predictedLocalR) < 2.10) {
	  const Trk::RIO_OnTrack* recalibratedROT = 
	    m_recalibrator->makeBroadMeasurement(*compROT,*it->smoothedTrackParameters());
	  itForKF->replaceMeasurement(recalibratedROT); itForKF->isOutlier(false);
	  decision = "tube hit";
	} else {
	  itForKF->isOutlier(Trk::TrackState::AssProbOutlier, 1);
	  decision = "new outlier";
	}
      }
      if (it->isCluster()) {
	if (assProb < m_option_outlierCut) {
	  itForKF->isOutlier(Trk::TrackState::AssProbOutlier, 1);
	  decision = "new outlier";
	} else decision = "cluster hit";
        
	// TO-DO start doing broad-cluster association here.
      }
    }
    if (msgLvl(MSG::VERBOSE)) std::cout << " ==> " << decision << std::endl;

    if (compROT) itForKF->identifier
		   (compROT->rioOnTrack(compROT->indexOfMaxAssignProb()).identify());
    // if (it->trackStateType() != Trk::TrackState::ExternalOutlier) {
      if (it->forwardTrackParameters()) itForKF->checkinForwardPar (it->checkoutForwardPar());
      if (it->parametersDifference())
        itForKF->checkinParametersDifference(it->checkoutParametersDifference());
      if (it->parametersCovariance())
        itForKF->checkinParametersCovariance(it->checkoutParametersCovariance());
      itForKF->setForwardStateFitQuality(it->forwardStateChiSquared(),it->forwardStateNumberDoF());
      if (it->dnaMaterialEffects()!=NULL)      
	itForKF->checkinDNA_MaterialEffects(it->checkoutDNA_MaterialEffects());
    // }
  }
  if (m_utility->numberOfNewOutliers(m_trajPiece) > 0.7*m_trajPiece.size()) {
    ATH_MSG_INFO ("Too many outliers in piecewise annealing!" );
    m_trajPiece.clear(); return Trk::FitterStatusCode::OutlierLogicFailure;
  }


  //////////////////////////////////////////////////////////////////////////////
  // step 4: boost filter to first state after annealed trajectory piece.
  //////////////////////////////////////////////////////////////////////////////
  ATH_MSG_DEBUG ("now stepping the main trajectory iterators ahead." );
  Trajectory::iterator lastStateOnPiece= m_utility->lastFittableState(m_trajPiece);
  Trajectory::iterator resumeKfState   = m_utility->nextFittableState(trajectory,fittableState);
  
  if (resumeKfState == trajectory.end()) { // ## case 1: DAF on full trajectory
   /* resumeKfState = fittableState;
    lastStateOnPiece = m_utility->previousFittableState(m_trajPiece,lastStateOnPiece);
    if (m_forwardFitter->needsReferenceTrajectory() &&
        start->referenceParameters() && start->parametersDifference()) {
      candidate_predPar = CREATE_PARAMETERS(*start->referenceParameters(),
                                            (start->referenceParameters()->parameters()
                                               + (*start->parametersDifference())),
                                            new AmgSymMatrix(5)(*start->parametersCovariance()));
    } else candidate_predPar    = resumeKfState->checkoutForwardPar(); */

    ATH_MSG_VERBOSE ("End of main trajectory, therefore setting filter onto last state ("<<
		     fittableState->positionOnTrajectory()<<") of trajectory.");

  } else { // ## case 2: DAF on a subset of trajectory

    if (m_forwardFitter->needsReferenceTrajectory()) ATH_MSG_ERROR("Code missing!");
    if (start_predPar) delete start_predPar;
    start_predPar = 
      m_extrapolator->extrapolate(*lastStateOnPiece->smoothedTrackParameters(),
				  resumeKfState->measurement()->associatedSurface(),
				  Trk::alongMomentum, false, particleType);
    if (!start_predPar) {
      ATH_MSG_INFO ("final extrapolation to finish off piecewise filter failed!" <<
		    " input or internal sorting problem?" );
      m_trajPiece.clear(); return Trk::FitterStatusCode::BadInput;
    }
    if (start_updatedPar) { // only for piecewise-modus
      delete start_updatedPar;
      start_updatedPar = lastStateOnPiece->checkoutSmoothedPar();
    }
    const Trk::RIO_OnTrack* rot;
    Trk::RoT_Extractor::extract(rot,resumeKfState->measurement());
    if (rot) resumeKfState->identifier(rot->identify());
    start            = resumeKfState;
  }

  m_trajPiece.clear(); delete newFitQuality;
  ATH_MSG_DEBUG ("terminating filterTrajPiece() with >Success<." );
  if (msgLvl(MSG::VERBOSE)) m_utility->dumpTrajectory(trajectory, name());

  return Trk::FitterStatusCode::Success;
}
//================================================================================

