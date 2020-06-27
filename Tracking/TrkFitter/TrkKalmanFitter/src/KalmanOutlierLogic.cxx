/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanOutlierLogic.cxx
//   Source file for class KalmanOutlierLogic
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Markus.Elsing -at-cern.ch, Wolfgang.Liebig -at- cern.ch
///////////////////////////////////////////////////////////////////

#include "TrkKalmanFitter/KalmanOutlierLogic.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkEventUtils/RoT_Extractor.h"
#include "TrkFitterInterfaces/IMeasurementRecalibrator.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkFitterUtils/ProtoTrajectoryUtility.h"
#include "TrkFitterUtils/SensorBoundsCheck.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "TrkSurfaces/TrapezoidBounds.h"
#include <algorithm>

namespace{ //anonymous namespace for comparison helper
  bool 
  minChi2State(const Trk::Trajectory::iterator & pFirst, Trk::Trajectory::iterator & pSecond){
    auto dof = [](const Trk::Trajectory::iterator & pTrajectory)->int{
      return pTrajectory->fitQuality()->numberDoF();
    };
    auto itsTrapezoid = [](const Trk::Trajectory::iterator & pTrajectory)->bool{
      return dynamic_cast<const Trk::TrapezoidBounds*>(&pTrajectory->measurement()->associatedSurface().bounds()) != nullptr;
    };
    int dof1{dof(pFirst)};
    int dof2{dof(pSecond)};
    if (dof1 == 2 and itsTrapezoid(pFirst)) dof1 = 1;
    if (dof2 == 2 and itsTrapezoid(pSecond)) dof2 = 1;
    auto chiSq =[](const Trk::Trajectory::iterator & pTrajectory, const int trueDof){
      return pTrajectory->fitQuality()->chiSquared() * trueDof;
    };
    // X1^2/DoF1 > X2^2/DoF2, but do multiplication to be faster
    return (chiSq(pFirst, dof2) > chiSq(pSecond, dof1));
  }

}

// constructor
Trk::KalmanOutlierLogic::KalmanOutlierLogic(const std::string& t,const std::string& n,const IInterface* p) :
  AthAlgTool (t,n,p),
  m_Trajectory_Chi2PerNdfCut(17.0),
  m_State_Chi2PerNdfCut(12.5),
  m_surfaceProximityCut(3.0),
  m_Trajectory_Chi2ProbCut{},
  m_extrapolator(nullptr),
  m_updator(nullptr),
  m_recalibrator(nullptr),
  m_utility(nullptr)

{
  // AlgTool stuff
  declareInterface<IKalmanOutlierLogic>( this );
  
  // declare all properties needed to configure fitter, defaults
  declareProperty("SurfaceProximityCut", m_surfaceProximityCut=3.f,
                  "tolerance limit in sigmas beyond which a prediction is out of active-sensor bounds");
  declareProperty("TrackChi2PerNDFCut",  m_Trajectory_Chi2PerNdfCut=17.f,
                  "Chi2/ndf cut to reject a fit and start search for outliers");
  declareProperty("StateChi2PerNDFCut",  m_State_Chi2PerNdfCut=12.5f,
                  "chi2/ndf cut on smoothed state to identify and rank outliers");
}

// destructor
Trk::KalmanOutlierLogic::~KalmanOutlierLogic()
{}

// initialize
StatusCode Trk::KalmanOutlierLogic::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  m_utility = new ProtoTrajectoryUtility();

  m_Trajectory_Chi2ProbCut = 1.0-Genfun::CumulativeChiSquare(1)(m_Trajectory_Chi2PerNdfCut);
  ATH_MSG_INFO ("Trajectory quality cut set to chi2/ndf of " 
        << m_Trajectory_Chi2PerNdfCut << " giving chi2-prob "
        << m_Trajectory_Chi2ProbCut ); // FIXME
  ATH_MSG_INFO ("outlier state cut set to chi2/ndf of      "<<m_State_Chi2PerNdfCut);
  ATH_MSG_INFO ("tolerance for surface proximity check is  "
        << m_surfaceProximityCut << " sigma(localTP)");
  ATH_MSG_INFO ("initialize() successful in " << name());
  return sc;
}

// finalize
StatusCode Trk::KalmanOutlierLogic::finalize()
{
  delete m_utility;
  ATH_MSG_INFO ("finalize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Trk::KalmanOutlierLogic::configureWithTools(Trk::IExtrapolator* extrap,
                                                       const Trk::IUpdator* updator,
                                                       const Trk::IMeasurementRecalibrator* recal)
{
  m_extrapolator = extrap;
  m_updator      = updator;
  m_recalibrator = recal;
  
  // protection, if not confiured
  if (!m_updator) {
    ATH_MSG_ERROR ("Updator missing, need to configure it !");
    return StatusCode::FAILURE;
  }
  if (!m_extrapolator) {
    ATH_MSG_ERROR ("Extrapolator missing, need to configure it !");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// BKS-cov at 1st meas: 0.000378  0.00539  1.28e-05  1.37e-05  9.00e-10
// 200*cov            : 0.0750    1.00000  2.56e-03  2.74e-03  1.80e-08

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// track cleaning: flag outliers or recalibrte measurements                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
bool Trk::KalmanOutlierLogic::flagNewOutliers(Trk::Trajectory& T,
                                                    const Trk::FitQuality& FQ,
                                                    int& firstNew,
                                                    const int fitIteration) const
{
  // count number of outliers
  int  numberOfRejections = 0;
  int  numberOfSensorOutliers = 0;
  double chi2GainFromSensorOutliers = -1.0;
  int  firstStateWithOutlier = 1000;
  Trk::FitQuality ffFQ = m_utility->forwardFilterQuality(T);
  double ffProb = 0.0;
  if (ffFQ.numberDoF()>0 && ffFQ.chiSquared() > 0.0)
    ffProb = 1.0-Genfun::CumulativeChiSquare(ffFQ.numberDoF())(ffFQ.chiSquared());
  bool isBadForward = 10.*ffProb < m_Trajectory_Chi2ProbCut; // or just reject(ffFQ);
  bool isBadBackward = reject(FQ);
  bool isBadTrack    = isBadForward || isBadBackward;
  ATH_MSG_VERBOSE ("-O- entered KalmanOutlierLogic::flagNewOutliers()");

  /* - try out: if AFB not near 0 monitor (1) qop(F)-qop(B) / sigma(qop)smoothed
       find out if F or B has the instability, then clear out at one of 2 ends. */

  
  double chi2_AFB = (ffFQ.chiSquared() - FQ.chiSquared()) / (ffFQ.chiSquared() + FQ.chiSquared());
  if ( (isBadForward || isBadBackward) )
    ATH_MSG_DEBUG ("-O- bad fit, study chi2_AFB = "<< chi2_AFB << " from F:"<<
                   ffFQ.chiSquared()<<"/"<<ffFQ.numberDoF() << " vs B:"<<
                   FQ.chiSquared() <<"/"<<FQ.numberDoF());

  // loop over all meas'ts and do outlier cuts
  for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++) {
    if (it->measurement() && !it->isOutlier()) {

      // first check: track should be inside active sensor boundaries + tolerance
      if ( ! Trk::SensorBoundsCheck::areParamsInside
           (*it->measurement(), it->smoothedTrackParameters()->parameters(),
            *it->smoothedTrackParameters()->covariance(), m_surfaceProximityCut) ){

        ATH_MSG_VERBOSE ("-O- state #" << it->positionOnTrajectory() << " at r=" <<
                         it->smoothedTrackParameters()->position().perp() << " z=" <<
                         it->smoothedTrackParameters()->position().z()<< " fails boundary check");
        it->isOutlier(Trk::TrackState::SensorOutlier, fitIteration);
        numberOfRejections++; numberOfSensorOutliers++;
        chi2GainFromSensorOutliers += it->fitQuality()->chiSquared();
        firstStateWithOutlier = std::min(it->positionOnTrajectory(),firstStateWithOutlier);
      }
      // second cut: smoothed chi2, equivalent to hit pull.
      else if ( isBadTrack && ( it->fitQuality()->chiSquared() >
		    m_State_Chi2PerNdfCut * it->fitQuality()->numberDoF() ) ) {
        it->isOutlier(Trk::TrackState::StateOutlier, fitIteration);
        numberOfRejections++;
        firstStateWithOutlier = std::min(it->positionOnTrajectory(),firstStateWithOutlier);
        ATH_MSG_VERBOSE ("-O- state #" << it->positionOnTrajectory() << " at r="<<
                         it->smoothedTrackParameters()->position().perp()<< " z=" <<
                         it->smoothedTrackParameters()->position().z() <<" chi2/ndf=" <<
                         it->fitQuality()->chiSquared()/it->fitQuality()->numberDoF()<<" fails chi2 cut");
      }
    }              
  }
  
  double prob = 1.0-Genfun::CumulativeChiSquare(FQ.numberDoF())(FQ.chiSquared());
  if (isBadTrack &&    // Grrr! We have to bite hard on this track.
      ( numberOfRejections==0   ||                       // no candidate yet
        ( numberOfRejections         <= 3 &&             // or: no *significant* candidate yet.
          numberOfRejections         == numberOfSensorOutliers &&
          10.0*prob                   < m_Trajectory_Chi2ProbCut &&
          chi2GainFromSensorOutliers  < 0.2 * FQ.chiSquared()) ) ) {
    
    // look for really bad forward fits - may hamper things like TRT extension, combined reco.
    if (10.0*ffProb < m_Trajectory_Chi2ProbCut &&
        (m_utility->numberOfOutliers(T)<4) && fitIteration==1) {
      if (!isBadBackward) {
          ATH_MSG_DEBUG ("-O- No outlier found yet, but detected problematic forward " <<
                         "fit with AFB "<<chi2_AFB<< " trying if this comes from chi2 on last hits");
        int nLastCandidates = 0;
        Trk::Trajectory::reverse_iterator outlierCandidate
          = Trk::Trajectory::reverse_iterator(++(m_utility->lastFittableState(T)));
        for( ; outlierCandidate!=T.rend(); ++outlierCandidate) {
          if (!outlierCandidate->measurement()) continue;
          ++ nLastCandidates;
          ATH_MSG_VERBOSE ("testing candidates, iterate as " << nLastCandidates );
          if ( (nLastCandidates < 4) && !outlierCandidate->isOutlier() &&
               (outlierCandidate->forwardStateChiSquared() > 0.5 * ffFQ.chiSquared()) ) {
            outlierCandidate->isOutlier(Trk::TrackState::StateOutlier, fitIteration);
            numberOfRejections++;
            firstStateWithOutlier = std::min(outlierCandidate->positionOnTrajectory(),firstStateWithOutlier);
            ATH_MSG_DEBUG ("removed outlier " << outlierCandidate->positionOnTrajectory()
                  << " with chi2 " << outlierCandidate->forwardStateChiSquared());
          }
          if (nLastCandidates >= 4) break;
        }
        
      } else {
        ATH_MSG_VERBOSE ("-O- No outlier found yet, but detected problematic forward fit "<<
                         "with AFB "<<chi2_AFB<< " trying if this comes from beginning of fit");
        int nFirstCandidates = 0;
        Trk::Trajectory::iterator outlierCandidate = m_utility->firstFittableState(T);
        for( ; outlierCandidate!=T.end(); ++outlierCandidate) {
          if (!outlierCandidate->measurement()) continue;
          ++ nFirstCandidates;
          ATH_MSG_VERBOSE ("testing candidates, iterate as "<<nFirstCandidates);
          if ( !outlierCandidate->isOutlier() &&
               (outlierCandidate->backwardStateChiSquared() > 0.3 * FQ.chiSquared()) ) {
            outlierCandidate->isOutlier(Trk::TrackState::StateOutlier, fitIteration);
            numberOfRejections++;
            firstStateWithOutlier = std::min(outlierCandidate->positionOnTrajectory(),firstStateWithOutlier);
            ATH_MSG_DEBUG ("beginning-of-track outlier " << outlierCandidate->positionOnTrajectory() <<
                           " with chi2 " << outlierCandidate->backwardStateChiSquared() );
          }
          if (nFirstCandidates >= 4) break;
        }

        if (numberOfRejections>0) {
          firstNew = firstStateWithOutlier; return true;
        }
      }
    }

    if ( !isBadForward && isBadBackward
         && (chi2_AFB < -0.5)
         && fitIteration == 1 && (m_utility->numberOfOutliers(T)<4) ) {
      ATH_MSG_DEBUG ("-O- No outlier found yet, but detected problematic backward fit "<<
                     "with AFB "<<chi2_AFB<<" trying if this comes from problem in last hits");
      int nLastCandidates = 0;
      double candidateChi2     = 0.0;
      Trk::ProtoTrackStateOnSurface* suspiciousState = nullptr;
      Trk::Trajectory::reverse_iterator rit
        = Trk::Trajectory::reverse_iterator(++(m_utility->lastFittableState(T)));
      for( ; rit!=T.rend(); ++rit) {
        if (!rit->measurement()) continue;
        ++ nLastCandidates;
        if ( !rit->isOutlier() &&
             (rit->forwardStateChiSquared() > candidateChi2) ) {
          suspiciousState = &(*rit);
          candidateChi2 = rit->forwardStateChiSquared();
        }
        if (nLastCandidates >= 4) break;
      }
      if (candidateChi2 > 4.0 && /*code-checker-report*/ suspiciousState) {
        suspiciousState->isOutlier(Trk::TrackState::StateOutlier, fitIteration);
        numberOfRejections++;
        firstStateWithOutlier = std::min(suspiciousState->positionOnTrajectory(),
                                         firstStateWithOutlier);
        ATH_MSG_DEBUG ("end-of-track outlier " << suspiciousState->positionOnTrajectory() <<
                       " with chi2 " << suspiciousState->forwardStateChiSquared() );
      }
    }

    // if no obvious outlier found so far, need to search: worst chi2 contributor(s).
    if ( numberOfRejections == 0 || numberOfRejections == numberOfSensorOutliers) {

      // search for worst states - do this via an STL sort so to be able to do more checks
      std::vector<Trk::Trajectory::iterator> bestStates(0);
      for(auto it = T.begin(); it!=T.end(); ++it)
        if ( it->measurement() && !it->isOutlier() ) bestStates.push_back(it);
      std::sort( bestStates.begin(), bestStates.end(), minChi2State ); //can use partial sort?

      if (numberOfRejections == 0 ) {
        ATH_MSG_VERBOSE ("-O- No outlier found so far, taking state with highest chi2/NDF instead.");

        Trk::Trajectory::iterator worstState       = *(bestStates.begin());
        Trk::Trajectory::iterator secondWorstState = *(bestStates.begin()+1);
        ATH_MSG_VERBOSE ("-O- found hit #" << worstState->positionOnTrajectory() << " at rT="<<
                         worstState->measurement()->globalPosition().perp() << " z=" <<
                         worstState->measurement()->globalPosition().z() <<
                         " as hit with worst chi2/ndf = " << worstState->fitQuality()->chiSquared()
                             /worstState->fitQuality()->numberDoF() );
        worstState->isOutlier(Trk::TrackState::TrackOutlier,fitIteration);
        numberOfRejections++;
        firstStateWithOutlier = std::min( worstState->positionOnTrajectory(),
                                          firstStateWithOutlier);

        // remove two outliers at once if the track probability is really, really small
        // or if a very simply-projected chi2 improvement is in reach 

        double projectedProb = 1.0;
        if ( FQ.numberDoF() > (worstState->fitQuality()->numberDoF()+2) &&
             FQ.chiSquared()>worstState->fitQuality()->chiSquared())
          projectedProb -= Genfun::CumulativeChiSquare(FQ.numberDoF()-worstState->fitQuality()->numberDoF())(FQ.chiSquared()-worstState->fitQuality()->chiSquared());

        if ( ( 100.0*prob < m_Trajectory_Chi2ProbCut ||
               10000.0*ffProb < m_Trajectory_Chi2ProbCut ||
               projectedProb < m_Trajectory_Chi2ProbCut ) &&
             (m_utility->numberOfMeasurements(T)>10 || fitIteration > 1)   ) {
          ATH_MSG_VERBOSE ("-O- chosen also #" << secondWorstState->positionOnTrajectory() <<
                           " at rT=" << secondWorstState->measurement()->globalPosition().perp()<<
                           " z=" << secondWorstState->measurement()->globalPosition().z()<<
                           " as second-worst hit, chi2/ndf= " << secondWorstState->fitQuality()->chiSquared()
                              /secondWorstState->fitQuality()->numberDoF() );
          secondWorstState->isOutlier(Trk::TrackState::TrackOutlier,fitIteration);
          numberOfRejections++;
          firstStateWithOutlier = std::min(secondWorstState->positionOnTrajectory(),
                                           firstStateWithOutlier);
        }
      } else {
        if (m_utility->numberOfSpecificStates(T,Trk::TrackState::TRT,Trk::TrackState::Fittable)>4) {
          auto itsTRT = [](const Trk::Trajectory::iterator & pTrajectory){
            return (pTrajectory->measurementType()==Trk::TrackState::TRT);
          };
          auto isAnOutlier = [](const Trk::Trajectory::iterator & pTrajectory){
            return pTrajectory->isOutlier();
          };
          auto absR_CutPasses = [](const Trk::Trajectory::iterator & pTrajectory){
            return (std::fabs(pTrajectory->measurement()->localParameters()[Trk::locR]) > 0.01);
          };
          
          for (const auto & state: bestStates){
            if (itsTRT(state) and not isAnOutlier(state) and absR_CutPasses(state)){
              const int pos = state->positionOnTrajectory();
              ATH_MSG_DEBUG ("Outliers found as sensor outliers but they " <<
                             "did not contribute to the chi2. Now remove " << pos <<
                             " with chi2 contrib " << *(state->fitQuality()) );
              firstStateWithOutlier = std::min(pos,firstStateWithOutlier);
              state->isOutlier(Trk::TrackState::TrackOutlier,fitIteration);
              break;
            }
          }
        }
      }
    }
  }


  if (numberOfRejections == 0) ATH_MSG_VERBOSE ("-O- no new outliers detected." );
  // FIXME remove  if (firstStateWithOutlier == 2) firstStateWithOutlier=0; // state #1=OK same as new filter
  firstNew = (numberOfRejections == 0 ? 0 : firstStateWithOutlier);
  return (numberOfRejections > 0);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// track judgement: validate fit quality and trajectory, does not change it   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
bool Trk::KalmanOutlierLogic::reject(const Trk::FitQuality& fitQuality) const
{
  // use probablity
  if (fitQuality.numberDoF() > 0 && fitQuality.chiSquared() > 0) {
    double prob = 1.0-Genfun::CumulativeChiSquare(fitQuality.numberDoF())(fitQuality.chiSquared());
    if ( prob < m_Trajectory_Chi2ProbCut) {
      ATH_MSG_DEBUG ("-O- trajectory with total chi2/ndf=" <<
                     fitQuality.chiSquared()/std::abs(fitQuality.numberDoF()) <<
                     ", prob= " << prob << " fails quality cut" );
      return true;
    } ATH_MSG_VERBOSE ("-O- trajectory passes quality cut, prob= " << prob);
  } else {
    if ( !(fitQuality.numberDoF() > 0))
      ATH_MSG_DEBUG ("-O- number d.o.f not positive - reject trajectory.");
    if ( !(fitQuality.chiSquared() > 0.0))
      ATH_MSG_DEBUG ("-O- chi2 is not positive - numerical problems?");
    return true;	    
  }
  // ok - it's accepted
  return false;
}
