/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanOutlierRecovery_InDet.cxx
//   Source file for class KalmanOutlierRecovery_InDet
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig -at- cern.ch
///////////////////////////////////////////////////////////////////

#include "TrkKalmanFitter/KalmanOutlierRecovery_InDet.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkEventUtils/RoT_Extractor.h"
#include "TrkFitterInterfaces/IMeasurementRecalibrator.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkFitterUtils/ProtoTrajectoryUtility.h"
#include "TrkFitterUtils/SensorBoundsCheck.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include <ext/algorithm>

// to run silicon-specific part
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkSurfaces/TrapezoidBounds.h"

// constructor
Trk::KalmanOutlierRecovery_InDet::KalmanOutlierRecovery_InDet(const std::string& t,const std::string& n,const IInterface* p) :
  AthAlgTool (t,n,p),
  m_Trajectory_Chi2PerNdfCut(17.0),
  m_State_Chi2PerNdfCut(12.5),
  m_Trajectory_Chi2ProbCut{},
  m_extrapolator(nullptr),
  m_updator(nullptr),
  m_recalibrator(nullptr),
  m_utility(nullptr)

{
  // AlgTool stuff
  declareInterface<IKalmanOutlierLogic>( this );
  
  // declare all properties needed to configure fitter, defaults
  declareProperty("TrackChi2PerNDFCut",  m_Trajectory_Chi2PerNdfCut=17.f,
                  "Chi2/ndf cut to reject a fit and start search for outliers");
  declareProperty("StateChi2PerNDFCut",  m_State_Chi2PerNdfCut=12.5f,
                  "chi2/ndf cut on smoothed state to identify and rank outliers");
}

// destructor
Trk::KalmanOutlierRecovery_InDet::~KalmanOutlierRecovery_InDet()
{}

// initialize
StatusCode Trk::KalmanOutlierRecovery_InDet::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  m_utility = new ProtoTrajectoryUtility();

  m_Trajectory_Chi2ProbCut = 1.0-Genfun::CumulativeChiSquare(1)(m_Trajectory_Chi2PerNdfCut);
  ATH_MSG_INFO ("Trajectory quality cut set to chi2/ndf of " << m_Trajectory_Chi2PerNdfCut <<
                " giving chi2-prob " << m_Trajectory_Chi2ProbCut ); // FIXME 
  ATH_MSG_INFO ("outlier state cut set to chi2/ndf of      " << m_State_Chi2PerNdfCut );
  ATH_MSG_INFO ("initialize() successful in " << name() );
  return sc;
}

// finalize
StatusCode Trk::KalmanOutlierRecovery_InDet::finalize()
{
  delete m_utility;
  ATH_MSG_INFO ("finalize() successful in " << name() );
  return StatusCode::SUCCESS;
}

StatusCode Trk::KalmanOutlierRecovery_InDet::configureWithTools(Trk::IExtrapolator* extrap,
                                                                const Trk::IUpdator* updator,
                                                                const Trk::IMeasurementRecalibrator* recal)
{
  m_extrapolator = extrap;
  m_updator      = updator;
  m_recalibrator = recal;
  
  // protection, if not confiured
  if (!m_updator) {
    ATH_MSG_ERROR ("Updator missing, need to configure it !" );
    return StatusCode::FAILURE;
  }
  if (!m_extrapolator) {
    ATH_MSG_ERROR ("Extrapolator missing, need to configure it !");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// helper operator for STL min_element
struct minChi2State {
	bool operator() (Trk::Trajectory::iterator one, Trk::Trajectory::iterator two) const {
      // once more, correct ndof for rotated 1D measurements (SCT endcap, TGC strip)
      int trueNumberDoF1 = one->fitQuality()->numberDoF();
      if (trueNumberDoF1 == 2 && dynamic_cast<const Trk::TrapezoidBounds*>
          (&one->measurement()->associatedSurface().bounds())) trueNumberDoF1 = 1;
      int trueNumberDoF2 = two->fitQuality()->numberDoF();
      if (trueNumberDoF2 == 2 && dynamic_cast<const Trk::TrapezoidBounds*>
          (&two->measurement()->associatedSurface().bounds())) trueNumberDoF2 = 1;

      return (one->fitQuality()->chiSquared() * trueNumberDoF2
              >
              two->fitQuality()->chiSquared() * trueNumberDoF1 );
	};
};

// BKS-cov at 1st meas: 0.000378  0.00539  1.28e-05  1.37e-05  9.00e-10
// 200*cov            : 0.0750    1.00000  2.56e-03  2.74e-03  1.80e-08

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// track cleaning: flag outliers or recalibrate measurements                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
bool Trk::KalmanOutlierRecovery_InDet::flagNewOutliers(Trk::Trajectory& T,
                                                       const Trk::FitQuality& /* FQ*/,
                                                       int& firstNew,
                                                       const int fitIteration) const
{
  ATH_MSG_VERBOSE ("-O- entered KalmanOutlierRecovery_InDet::flagNewOutliers()");

  // count number of hits and outliers
  int numberOfRejections = 0;
  bool failureRecoveryNeedsRefit = false;
  int c_allPix=0,c_allSct=0,c_allTrt=0; int c_outPix=0,c_outSct=0,c_outTrt=0;
  for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++) {
    using namespace Trk::TrackState;
    if (it->measurementType()==Pixel) {if (it->isNewOutlier()) ++c_outPix; ++c_allPix;}
    if (it->measurementType()==SCT)   {if (it->isNewOutlier()) ++c_outSct; ++c_allSct;}
    if (it->measurementType()==TRT)   {if (it->isNewOutlier()) ++c_outTrt; ++c_allTrt;}
    if ( (it->measurementType()==Pixel || it->measurementType()==SCT 
          || it->measurementType()==TRT) && it->isNewOutlier() 
         && it->iterationShowingThisOutlier() == fitIteration) ++numberOfRejections;
  }

  // recovery of bad cases in the Silicon
  if (numberOfRejections > 0 || 
      (m_utility->numberOfNewOutliers(T) > std::min(.4*T.size(),7.) )) {
    using namespace Trk::TrackState;
    Trk::Trajectory::iterator state1 = T.begin();
    while (!state1->forwardTrackParameters() && state1!=T.end()) ++state1;
    if (state1 == T.end()) { 
      ATH_MSG_WARNING ("outlier recovery internally inconsistent.");
      return false;
    }
    const Trk::TrackParameters* filterInputPar = state1->forwardTrackParameters();

    ATH_MSG_DEBUG ("-O- outliers in " << fitIteration <<"-th round:  " << c_outPix<<"("<<c_allPix<<
                   ") -- " << c_outSct<<"("<< c_allSct<<") -- " << c_outTrt<<"("<<c_allTrt<<")");

    // === part 1: pixel === recover occas. pixel outliers as broad clusters if SCT is reliable
    if (fitIteration <= 2 && m_recalibrator!=nullptr 
        && c_allPix > 0 && 2*c_outPix <= c_allPix
        && 2*c_outSct < c_allSct  ) {

      for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++) {
        if (it->isNewOutlier() && it->measurementType()==Pixel
            && it->trackStateType() != PredictedOutlier ) {

          Trk::Trajectory::iterator restartState = m_utility->nextFittableState(T,it);
          const Trk::TrackParameters* smoothedParAtOutlier =
	    (restartState != T.end() ?
	     m_extrapolator->extrapolate(*restartState->smoothedTrackParameters(),
					 it->measurement()->associatedSurface(), Trk::oppositeMomentum,
					 false, Trk::nonInteracting) :
	     nullptr );
          if (!smoothedParAtOutlier) continue;

          if ( (it->measurement()->localCovariance()(Trk::locX,Trk::locX) < 0.02*0.02)
               || (it->measurement()->localCovariance()(Trk::locY,Trk::locY) < 0.2*0.2) ) {
            const Trk::MeasurementBase* broadPixel = 
              m_recalibrator->makeBroadMeasurement(*it->measurement(),
                                                   *smoothedParAtOutlier, Pixel);
            if (broadPixel!=nullptr) {
              if (( broadPixel->localCovariance()(Trk::locX,Trk::locX)
                    > 1.05*1.05* it->measurement()->localCovariance()(Trk::locX,Trk::locX)) &&
                  ( broadPixel->localCovariance()(Trk::locY,Trk::locY)
                    > 1.05*1.05*it->measurement()->localCovariance()(Trk::locY,Trk::locY)) ) {
                ATH_MSG_DEBUG ("Pixel dLocX="<<sqrt(it->measurement()->localCovariance()(Trk::locX,Trk::locX)) <<
                               ", dLocY="<<sqrt(it->measurement()->localCovariance()(Trk::locY,Trk::locY)) <<
                               " replaced by hit with dLocX=" <<
                               sqrt(broadPixel->localCovariance()(Trk::locX,Trk::locX))<<", dLocY="<<
                               sqrt(broadPixel->localCovariance()(Trk::locY,Trk::locY)) );
                it->replaceMeasurement(broadPixel,Trk::TrackState::BroadCluster);
                it->isOutlier(false);
                if (it->positionOnTrajectory() < firstNew) firstNew = it->positionOnTrajectory(); 
                failureRecoveryNeedsRefit=true;
              } else {
                delete broadPixel;
                ATH_MSG_DEBUG ("could not get a 'broader' cluster, flag outlier.");
              }
            }
          }
          delete smoothedParAtOutlier;
        }
      }
    }

    // === part 2: SCT === recover lost SCT filtering (again using broad pixels clusters)
    //    int nSCT = m_utility->numberOfSpecificStates(T,SCT,AnyState);
    if (m_recalibrator!=nullptr
        && (fabs(filterInputPar->parameters()[Trk::qOverP])<0.00125) // dont try on lowPT
        && (c_allPix - c_outPix > 1 )
        && (c_allSct + c_allPix > 7 )
        && (fitIteration < 2 || m_utility->numberOfOutliers(T,7)>1)
        && c_allSct > 3
        && 2*c_outSct >= c_allSct ) {
      ATH_MSG_DEBUG ("-O- Triggered SCT filter-loss recovery ");
      if (fitIteration >= 2) {
        // further ideas: check of predictive vs. pattern momentum at states 2-3-4
        ATH_MSG_DEBUG ("-O- Giving up... hope for predictive seed recovery ");
        for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++)
          it->isOutlier(GeneralOutlier,fitIteration);
        firstNew = 1;
        return true;
      } 

      const int badRankedNumberOfMeas = m_utility->rankedNumberOfMeasurements(T);
      Trk::Trajectory::iterator lastSctState = T.end();
      for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++)
        if (it->measurementType() == SCT) lastSctState = it;

      const Trk::TrackParameters* sctExitTemp =
        m_extrapolator->extrapolate(*filterInputPar,
                                    lastSctState->measurement()->associatedSurface(),
                                    Trk::alongMomentum,
                                    false, Trk::nonInteracting);
      if (sctExitTemp == nullptr) {
        ATH_MSG_DEBUG ("-O- can not reproduce trajectory in SCT recovery!");
        for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++)
          it->isOutlier(GeneralOutlier,fitIteration);
        firstNew = 1;
        return true;
      }
      Amg::VectorX par(5);
      par[Trk::locX]=lastSctState->measurement()->localParameters()[Trk::locX];
      par[Trk::locY]=sctExitTemp->parameters()[Trk::locY];
      par[Trk::phi]=sctExitTemp->parameters()[Trk::phi];
      par[Trk::theta]=sctExitTemp->parameters()[Trk::theta];
      par[Trk::qOverP]=sctExitTemp->parameters()[Trk::qOverP];
      AmgSymMatrix(5)* cov = new AmgSymMatrix(5);
      cov->setZero();
      (*cov)(0,0) = 25.0;
      (*cov)(1,1) = 400.0;
      (*cov)(2,2) = 0.04;
      (*cov)(3,3) = 0.16;
      (*cov)(4,4) = 0.04*sctExitTemp->parameters()[Trk::qOverP]
        *sctExitTemp->parameters()[Trk::qOverP];

      const Trk::TrackParameters* sctExit = sctExitTemp->associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],
													 par[Trk::phi],par[Trk::theta],par[Trk::qOverP],cov); 
      delete sctExitTemp;
      ATH_MSG_DEBUG ("-O- At iteration " << fitIteration << " SCT exit: meas't (" <<
                     lastSctState->measurement()->localParameters()[Trk::locX] << ",  " <<
                     lastSctState->measurement()->localParameters()[Trk::locY] << ") vs. prediction ("<<
                     sctExit->parameters()[Trk::locX]<< ", "<<sctExit->parameters()[Trk::locY]<< ") ");
      
      // switch to reverse gear and run a small filter through the SCT
      Trk::Trajectory::reverse_iterator lastSctState_r
        = Trk::Trajectory::reverse_iterator(++lastSctState);
      const Trk::TrackParameters* sctFitResult =
        m_updator->addToState(*sctExit, lastSctState_r->measurement()->localParameters(),
                              lastSctState_r->measurement()->localCovariance());
      delete sctExit; 
      Trk::Trajectory::reverse_iterator rit = lastSctState_r + 1;
      Trk::Trajectory::reverse_iterator firstSctState_r = T.rend();
      for( ; rit!=T.rend(); ++rit) {
        if ( rit->measurementType() == SCT) {
          const Trk::TrackParameters* predPar =
	    (sctFitResult!=nullptr ?
	     m_extrapolator->extrapolate(*sctFitResult,
					 rit->measurement()->associatedSurface(),
					 Trk::oppositeMomentum,
					 false, Trk::muon) :
	     nullptr);
          delete sctFitResult;
          if (!predPar) {
	    ATH_MSG_DEBUG ("-O- internal mini-filter failed in SCT recovery!");
	    // for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++)
	    //  it->isOutlier(GeneralOutlier,fitIteration);
	    //	    firstNew = 1;
	    // return true;
	    return failureRecoveryNeedsRefit;
          }
          sctFitResult = m_updator->addToState(*predPar,
                                               rit->measurement()->localParameters(),
                                               rit->measurement()->localCovariance());
          ATH_MSG_VERBOSE ("At "<<rit->positionOnTrajectory() << " errs in locX: "<<
                           rit->measurement()->localCovariance()(Trk::locX) );
          firstSctState_r = rit;
          delete predPar;
        }
      }

      int numberOfChangedStates=0;
      const Trk::TrackParameters* updatedPar = sctFitResult;
      sctFitResult=nullptr;
      rit = firstSctState_r + 1; // now on last Pixel state!
      for( ; rit!=T.rend(); ++rit) {
        const Trk::TrackParameters* filteredPar =
	  ( updatedPar != nullptr ?
	    m_extrapolator->extrapolate(*updatedPar,
					rit->measurement()->associatedSurface(),
					Trk::oppositeMomentum,
					false, Trk::muon) :
	    nullptr );
        delete updatedPar;
        const Trk::FitQualityOnSurface* testQuality = (filteredPar==nullptr)? nullptr :
          m_updator->predictedStateFitQuality(*filteredPar,
                                              rit->measurement()->localParameters(),
                                              rit->measurement()->localCovariance());
        if (testQuality == nullptr) {
          ATH_MSG_DEBUG ("-O- can not reproduce trajectory at the end of SCT recovery!");
          for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++)
            it->isOutlier(GeneralOutlier,fitIteration);
          firstNew = 1;
          return true;
        }

        // got Pixel analysis from SCT point of view, try first: broad cluster error
        if ( ( ( testQuality->chiSquared() > 1.5*testQuality->numberDoF()
                 || fitIteration == 2 )
               || (rit->isOutlier() && rit->trackStateType() != PredictedOutlier) )
             && (rit->measurement()->localCovariance()(Trk::locX) < 0.02)
             && (rit->measurement()->localCovariance()(Trk::locY) < 0.2) ) {
          const Trk::MeasurementBase* broadPixel = 
            m_recalibrator->makeBroadMeasurement(*rit->measurement(), *filteredPar, Pixel);
          if (broadPixel!=nullptr) {
            if (( broadPixel->localCovariance()(Trk::locX)
                  > rit->measurement()->localCovariance()(Trk::locX)) &&
                ( broadPixel->localCovariance()(Trk::locY)
                  > rit->measurement()->localCovariance()(Trk::locY)) ) {
              ATH_MSG_DEBUG ("-O- SCT-filter replaced Pixel dLocX=" <<
                             rit->measurement()->localCovariance()(Trk::locX)<<", dLocY=" <<
                             rit->measurement()->localCovariance()(Trk::locY) <<
                             " by hit with dLocX=" <<
                             broadPixel->localCovariance()(Trk::locX)<<", dLocY=" <<
                             broadPixel->localCovariance()(Trk::locX) );
              rit->replaceMeasurement(broadPixel, Trk::TrackState::BroadCluster);
              rit->isOutlier(false);
              ++numberOfChangedStates;
              delete testQuality;
              testQuality =  m_updator->predictedStateFitQuality
                (*filteredPar, rit->measurement()->localParameters(),
                 rit->measurement()->localCovariance());
            } else delete broadPixel;
          } else ATH_MSG_WARNING ("could not re-do pixel ROT creation!");
        }

        ATH_MSG_DEBUG ("At " << rit->positionOnTrajectory() << " errs in locX: " <<
                       rit->measurement()->localCovariance()(Trk::locX)   <<
                       " errs in locY: " << rit->measurement()->localCovariance()(Trk::locY));
        ATH_MSG_DEBUG ("At " << rit->positionOnTrajectory() << " fit quality: " <<
                       (testQuality? testQuality->chiSquared() : -1.0) );

        // next step: decide if outlier is too big for the current mini-filter
        if (!testQuality || testQuality->chiSquared() > 
		    4*m_State_Chi2PerNdfCut * testQuality->numberDoF() ) {
          updatedPar = filteredPar;
          filteredPar=nullptr;
          ATH_MSG_DEBUG ("At "<<rit->positionOnTrajectory() <<": decided not to use this state, chi2="<<
                         (testQuality ? testQuality->chiSquared() : -1.0) );
        } else {
          updatedPar = m_updator->addToState(*filteredPar,
                                             rit->measurement()->localParameters(),
                                             rit->measurement()->localCovariance());
          if (updatedPar==nullptr) updatedPar=filteredPar; else delete filteredPar;
          ATH_MSG_DEBUG ("At "<<rit->positionOnTrajectory()<<": using this state to proceed.");
        }
        if (!testQuality ||
	    (testQuality->chiSquared() > 
		    0.5*m_State_Chi2PerNdfCut * testQuality->numberDoF() ) ) {
          rit->isOutlier(Trk::TrackState::TrackOutlier,fitIteration);
          ++numberOfChangedStates;
          ATH_MSG_DEBUG ("At "<<rit->positionOnTrajectory() <<": remove state with tightened "<<
                         "outlier cut, chi2="<<testQuality->chiSquared());
        } else if (!rit->isOutlier() && 
                   rit->measurement()->localCovariance()(Trk::locX) > 0.1) {
          rit->isOutlier(Trk::TrackState::TrackOutlier,fitIteration);
          ++numberOfChangedStates;
          ATH_MSG_DEBUG ("At "<<rit->positionOnTrajectory()<<": remove state as suspicious cluster.");
        } else if (rit->isOutlier() && rit->trackStateType() != PredictedOutlier) {
          rit->isOutlier(false);
          ++numberOfChangedStates;
          ATH_MSG_DEBUG ("At "<<rit->positionOnTrajectory() <<": reintegrate state!");
        }
        delete testQuality;
      }
      delete updatedPar;
      if (numberOfChangedStates>0) {
        failureRecoveryNeedsRefit=true;
        firstNew = 1;

        int improvedRank = m_utility->rankedNumberOfMeasurements(T);
        improvedRank += m_utility->numberOfSpecificStates(T,SCT,GeneralOutlier);
        if (improvedRank <= badRankedNumberOfMeas) { // no good in removing lots of pixel!
          ATH_MSG_DEBUG ("SCT recovery has produced bogus solution!");
          for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++)
            if ( (it->measurementType()==Pixel) && (it->trackStateType() != PredictedOutlier) )
              it->isOutlier(false);
        } else {
          ATH_MSG_DEBUG ("SCT recovery achieved through cleaning pixel part");
          for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++)
            if ( (it->measurementType() == SCT) && (it->isNewOutlier()) )
              it->isOutlier(false);
        }
      }
    
    }      

  } // stop recovery for bad cases

  // === part 3: TRT ===
  if (c_allTrt-c_outTrt > 0 && m_recalibrator!=nullptr ) {

    if (numberOfRejections > 0 ) {
      for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++) {

        if (msgLvl(MSG::VERBOSE)) {
          const Trk::RIO_OnTrack* rot=nullptr;
          Trk::RoT_Extractor::extract(rot,it->measurement());
          if (it->isDriftCircle() && rot!=nullptr && rot->prepRawData()!=nullptr)
            msg(MSG::VERBOSE) << "Testing L/R of " << (it->isOutlier() ? "Outl." : "Hit  " )
                              << it->positionOnTrajectory() << " : Par = "
                              << (it->smoothedTrackParameters() ? 
                                  it->smoothedTrackParameters()->parameters()[Trk::locR] : -999.0)
                              <<  " Mbs = " 
                              << it->measurement()->localParameters()[Trk::locR] << ", Prd = "
                              <<(rot->prepRawData() ? rot->prepRawData()->localPosition()[Trk::locR] : -999.)
                              << endmsg;
        }
        
        const Trk::TrackParameters* refTrkPar = it->smoothedTrackParameters();
        // step 1 - try re-integrating drift radius outliers as tube hit inlayers
        if (it->measurementType() == Trk::TrackState::TRT && refTrkPar!=nullptr
            && (it->trackStateType() == Trk::TrackState::StateOutlier ||
                it->trackStateType() == Trk::TrackState::TrackOutlier)
            && it->iterationShowingThisOutlier() == fitIteration 
            && (Trk::TrackState::TubeHit != 
                m_recalibrator->calibrationStatus(*it->measurement(),Trk::TrackState::TRT))
            ) {
          const Trk::RIO_OnTrack* recalibratedROT = 
            m_recalibrator->makeBroadMeasurement(*it->measurement(),*refTrkPar,
                                                 Trk::TrackState::TRT);
          if (recalibratedROT!=nullptr) {
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "-O- re-integrating drift circle outlier dR="
                                       << it->measurement()->localCovariance()(Trk::locR);
            it->replaceMeasurement(recalibratedROT,Trk::TrackState::TubeHit);
            it->isOutlier(false);
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " as tube hitdR="  <<
              it->measurement()->localCovariance()(Trk::locR)<<endmsg;
          }
        }
	else
        // step 2 - try re-integrating tube hits as drift radius hits
        if (it->measurementType() == Trk::TrackState::TRT && refTrkPar!=nullptr
            && !it->isOutlier()
            && (Trk::TrackState::TubeHit == 
                m_recalibrator->calibrationStatus(*it->measurement(),Trk::TrackState::TRT))
            ) {
	  const Trk::RIO_OnTrack* trt=dynamic_cast<const Trk::RIO_OnTrack*>(it->measurement());
	  const TrackParameters* mTP = it->smoothedTrackParameters();
	  if (trt && trt->prepRawData() && mTP->covariance()
	      && ( fabs(trt->prepRawData()->localPosition()[Trk::locR]
                        - fabs(it->smoothedTrackParameters()->parameters()[Trk::locR])) 
		   < 2.5*sqrt(trt->prepRawData()->localCovariance()(Trk::locR,Trk::locR)
			      + (*mTP->covariance())(Trk::locR,Trk::locR)))
	      ) {
	    const Trk::RIO_OnTrack* circlehit = m_recalibrator->makePreciseMeasurement
	      (*it->measurement(), *refTrkPar, Trk::TrackState::TRT);
	    if (circlehit && fabs(circlehit->localParameters()[Trk::locR])>0.001) {
	      it->replaceMeasurement(circlehit,Trk::TrackState::Nominal);
	      failureRecoveryNeedsRefit = true;
	      ATH_MSG_VERBOSE ("Outlier control: upgraded tube hit to drift circle." );
	    } else delete circlehit;
          }
	}
      }
    }

    /* logic to prevent cases where a Si+TRT track loses over a Si-only track because the TRT comes
       with a large chi2 penalty.
       sees only 7 more tracks extended at cost of assignment efficiency
    */
    if (c_allPix+c_allSct>4) {
      Trk::FitQuality trtFQ = m_utility->forwardFilterQuality(T,Trk::TrackState::TRT);
      double trtProb = 1.0;
      if (trtFQ.numberDoF() >= 0)
	trtProb = 1.0 - Genfun::CumulativeChiSquare(trtFQ.numberDoF()+2)(trtFQ.chiSquared());

      if (trtProb < 10.0e-3 && (2*c_outTrt < c_allTrt) ) {
	ATH_MSG_VERBOSE ("risk of losing TRT extension, reduce TRT's chi2 contrib.");
	for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++) {
	  const Trk::RIO_OnTrack* trt=dynamic_cast<const Trk::RIO_OnTrack*>(it->measurement());
	  if (it->measurementType() == Trk::TrackState::TRT
	      && !it->isOutlier() && it->smoothedTrackParameters()!=nullptr
	      && trt && trt->prepRawData() ) {
	    bool tubeHitMade=false;
	    if ( (m_recalibrator->calibrationStatus(*it->measurement(),Trk::TrackState::TRT)
		  != Trk::TrackState::TubeHit)
		 && (it->measurement()->localParameters()[Trk::locR]
		     > fabs(it->smoothedTrackParameters()->parameters()[Trk::locR]))
		 && it->fitQuality()->chiSquared() > 6.0
		 ) {
	      const Trk::RIO_OnTrack* tubehit = m_recalibrator->makeBroadMeasurement
		(*it->measurement(), *it->smoothedTrackParameters(), Trk::TrackState::TRT);
	      if (tubehit) {
		it->replaceMeasurement(tubehit,Trk::TrackState::TubeHit);
		tubeHitMade = true;
		failureRecoveryNeedsRefit = true;
		ATH_MSG_VERBOSE ("Chi2 control: downgraded drift circle to tube hit." );
	      }
	    }
	    const TrackParameters* mTP = it->smoothedTrackParameters();
	    if (!tubeHitMade && mTP
		&& (m_recalibrator->calibrationStatus(*it->measurement(),Trk::TrackState::TRT)
		    == Trk::TrackState::TubeHit)
		&& ( fabs(trt->prepRawData()->localPosition()[Trk::locR] 
                        - fabs(it->smoothedTrackParameters()->parameters()[Trk::locR])) 
		     < 2.0*sqrt(trt->prepRawData()->localCovariance()(Trk::locR,Trk::locR)
				+ (*mTP->covariance())(Trk::locR,Trk::locR)))
		) {
	      const Trk::RIO_OnTrack* circlehit = m_recalibrator->makePreciseMeasurement
		(*it->measurement(), *it->smoothedTrackParameters(), Trk::TrackState::TRT);
	      if (circlehit && fabs(circlehit->localParameters()[Trk::locR])>0.001) {
		it->replaceMeasurement(circlehit,Trk::TrackState::Nominal);
		failureRecoveryNeedsRefit = true;
		ATH_MSG_VERBOSE ("Chi2 control: upgraded tube hit to drift circle." );
	      } else delete circlehit;
	    }
	  }
	}
      }
      }
  }
  return  failureRecoveryNeedsRefit;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// track judgement: validate fit quality and trajectory, does not change it   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
bool Trk::KalmanOutlierRecovery_InDet::reject(const Trk::FitQuality& fitQuality) const
{
  // use chi2 value
  //  if ( fitQuality.chiSquared() > m_Trajectory_Chi2PerNdfCut * fabs(fitQuality.numberDoF()) ) {
  //  ATH_MSG_DEBUG ( << "-O- trajectory with total chi2/ndf="
  //				  << fitQuality.chiSquared()/fabs(fitQuality.numberDoF())
  //				  << " fails quality cut" << endmsg;

  // use probablity
  if (fitQuality.numberDoF() > 0 && fitQuality.chiSquared() > 0) {
    double prob = 1.0-Genfun::CumulativeChiSquare(fitQuality.numberDoF())(fitQuality.chiSquared());
    if ( prob < m_Trajectory_Chi2ProbCut) {
      ATH_MSG_DEBUG ("-O- trajectory with total chi2/ndf="
                     << fitQuality.chiSquared()/std::abs(fitQuality.numberDoF())
                     << ", prob= " << prob << " fails quality cut" );
      return true;
    } ATH_MSG_VERBOSE ( "-O- trajectory passes quality cut, prob= " << prob );
  } else {
    if ( !(fitQuality.numberDoF() > 0))
      ATH_MSG_DEBUG ("-O- number d.o.f not positive - reject trajectory.");
    if ( !(fitQuality.chiSquared() > 0.0))
      ATH_MSG_DEBUG ("-O- chi2 is not positive - numerical problems?" );
    return true;	    
  }
  // ok - it's accepted
  return false;
}
