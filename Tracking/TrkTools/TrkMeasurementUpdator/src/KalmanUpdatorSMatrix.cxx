/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanUpdatorSMatrix.cxx
//   Source file for class KalmanUpdatorSMatrix
//   uses SMatrix math library internally.
//   The main updating calculations can be found in
//   calculateFilterStep_1D() to _5D().
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
///////////////////////////////////////////////////////////////////

#include "TrkMeasurementUpdator/KalmanUpdatorSMatrix.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"

#include <cstring>

// using namespace ROOT::Math;
// using ROOT::Math::SMatrix;

const Trk::ParamDefsAccessor Trk::KalmanUpdatorSMatrix::s_enumAccessor;

// constructor
Trk::KalmanUpdatorSMatrix::KalmanUpdatorSMatrix(const std::string& t,const std::string& n,const IInterface* p) :
    AthAlgTool (t,n,p),
    m_cov_stdvec{250.,250.,0.25,0.25,0.000001}, // set defaults _before_ reading from job options
    m_thetaGainDampingValue(0.1),
    m_unitMatrix()
{
    // AlgTool stuff
    declareProperty("InitialCovariances",m_cov_stdvec,"default covariance to be used at start of filter");
    declareProperty("FastTrackStateCovCalculation",m_useFruehwirth8a=false,"toggles which formula to use for updated cov");
    declareInterface<IUpdator>( this );
}

// destructor
Trk::KalmanUpdatorSMatrix::~KalmanUpdatorSMatrix()
{}

// initialize
StatusCode Trk::KalmanUpdatorSMatrix::initialize()
{
    if (m_cov_stdvec.size() < 5) {
      ATH_MSG_INFO( "Wrong-sized initial covariance given, so set to default: ");
      m_cov_stdvec = {250.,250.,0.25, 0.25, 0.000001};
    }
    ATH_MSG_INFO( "Initial covariance: " << m_cov_stdvec[0] << ", "
          << m_cov_stdvec[1] << ", " << m_cov_stdvec[2] << ", "
          << m_cov_stdvec[3] << ", " << m_cov_stdvec[4] << " (diagonal)" );
    m_cov0 = SParVector5(&m_cov_stdvec[0],5);
    if (m_useFruehwirth8a) {
      ATH_MSG_INFO( "Fast computation will be used for track state cov matrices (Fruehwirth-1987 eq. 8a)." );
    } else{
      ATH_MSG_INFO( "Track state cov matrix will be calculated according to Gelb-1975 p305." );
    }

    const SParVector5 IV(1.0, 1.0, 1.0, 1.0, 1.0);
    m_unitMatrix.SetDiagonal(IV);

    ATH_MSG_DEBUG( "initialize() successful in " << name() );
    return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::KalmanUpdatorSMatrix::finalize()
{
    ATH_MSG_DEBUG( "finalize() successful in " << name() );
    return StatusCode::SUCCESS;
}

// updator #1 for Kalman Fitter - version with Amg::Vector2D (for example for PrepRawData)
Trk::TrackParameters* Trk::KalmanUpdatorSMatrix::addToState (const Trk::TrackParameters& inputTrkPar,
                                                             const Amg::Vector2D& measLocPos, 
                                                             const Amg::MatrixX& measLocCov) const {
  if (msgLvl(MSG::VERBOSE)) logStart("addToState(TP,LPOS,ERR)",inputTrkPar);
  FitQualityOnSurface*    fitQoS = nullptr;
  const int updatingSign = 1;

  SCovMatrix5 covTrk;
  if (!getStartCov(covTrk,inputTrkPar,updatingSign)) return nullptr;
  
  
  int nLocCoord = measLocCov.cols();
  if (nLocCoord == 1) {
    return calculateFilterStep_1D (inputTrkPar, // transmit the surface
                                   SParVector5(&inputTrkPar.parameters()[0],5),covTrk,
                                   measLocPos[0],1,measLocCov,
                                   updatingSign,fitQoS,false);
  } if (nLocCoord == 2) {
    return calculateFilterStep_2D (inputTrkPar,// transmit the surface
                                   SParVector5(&inputTrkPar.parameters()[0],5),covTrk,
                                   SParVector2(measLocPos[0],measLocPos[1]),3,
                                   measLocCov,
                                   updatingSign,fitQoS,false);
  } 
    ATH_MSG_WARNING( " number (" << nLocCoord << ") of local coordinates must be 1 or 2, can not update!" ); 
    return nullptr;
  
}

// updator #2 for Kalman Fitter - version with LocalParameters (for example for RIO_OnTrack)
Trk::TrackParameters* Trk::KalmanUpdatorSMatrix::addToState (const Trk::TrackParameters& trkPar,
                                                             const LocalParameters&  measmtPar,
                                                             const Amg::MatrixX& measmtCov) const {
    if (msgLvl(MSG::VERBOSE)) logStart("addToState(TP,LPAR,ERR)",trkPar);
    FitQualityOnSurface*    fitQoS = nullptr;
    return prepareFilterStep (trkPar, measmtPar, measmtCov, 1, fitQoS, false);
}

// updator #3 for Kalman Fitter - version with Amg::Vector2D (for example for PrepRawData)
Trk::TrackParameters* Trk::KalmanUpdatorSMatrix::addToState (const Trk::TrackParameters& inputTP,
                                                             const Amg::Vector2D& measLocPos, 
                                                             const Amg::MatrixX& measLocCov,
                                                             FitQualityOnSurface*& fitQoS) const {
    const int updatingSign = 1;
    if (msgLvl(MSG::VERBOSE)) logStart("addToState(TP,LPOS,ERR,FQ)",inputTP);
    if (fitQoS) {
        ATH_MSG_WARNING( "expect nil FitQuality pointer, refuse operation to avoid mem leak!");
        return nullptr;
    } 

      SCovMatrix5 covTrk;
      if (!getStartCov(covTrk,inputTP,updatingSign)) return nullptr;
            
      int nLocCoord = measLocCov.cols();
      if (nLocCoord == 1) {
        return calculateFilterStep_1D (inputTP,
                                       SParVector5(&inputTP.parameters()[0],5),covTrk,
                                       measLocPos[0],1,measLocCov,
                                       updatingSign,fitQoS,true);
      } if (nLocCoord == 2) {
        return calculateFilterStep_2D (inputTP,
                                       SParVector5(&inputTP.parameters()[0],5),covTrk,
                                       SParVector2(measLocPos[0],measLocPos[1]),3,
                                       measLocCov,
                                       updatingSign,fitQoS,true);
        } 
        ATH_MSG_WARNING( " number (" << nLocCoord << ") of local coordinates must be 1 or 2, can not update!" ); 
        return nullptr;
      
    
}

// updator #4 for Kalman Fitter - version with LocalParameters (for example for RIO_OnTrack)
Trk::TrackParameters* Trk::KalmanUpdatorSMatrix::addToState (const Trk::TrackParameters& trkPar,
                                                             const LocalParameters& measmtPar,
                                                             const Amg::MatrixX& measmtCov,
                                                             FitQualityOnSurface*& fitQoS) const {
    if (msgLvl(MSG::VERBOSE)) logStart("addToState(TP,LPAR,ERR,FQ)",trkPar);
    if (fitQoS) {
        ATH_MSG_WARNING( "expect nil FitQuality pointer, refuse operation to"
              << " avoid mem leak!" );
      return nullptr;
    } 
      return prepareFilterStep (trkPar, measmtPar, measmtCov, 1, fitQoS, true);
    
}

// inverse updator #1 for Kalman Fitter - version with Amg::Vector2D (for example for PrepRawData)
Trk::TrackParameters* Trk::KalmanUpdatorSMatrix::removeFromState (const Trk::TrackParameters& inputTP,
                                                                  const Amg::Vector2D& measLocPos, 
                                                                  const Amg::MatrixX& measLocCov) const {
    if (msgLvl(MSG::VERBOSE)) logStart("removeFromState(TP,LPOS,ERR)",inputTP);
    FitQualityOnSurface*    fitQoS = nullptr;
    const int updatingSign = -1;
    SCovMatrix5 covTrk;
    if (!getStartCov(covTrk,inputTP,updatingSign)) return nullptr;

    int nLocCoord = measLocCov.cols();
    if (nLocCoord == 1) {
      return calculateFilterStep_1D (inputTP,
                                     SParVector5(&inputTP.parameters()[0],5),covTrk,
                                     measLocPos[0],1,measLocCov,
                                     updatingSign,fitQoS,false);
    } if (nLocCoord == 2) {
      return calculateFilterStep_2D (inputTP,
                                     SParVector5(&inputTP.parameters()[0],5),covTrk,
                                     SParVector2(measLocPos[0],measLocPos[1]),3,
                                     measLocCov,
                                     updatingSign,fitQoS,false);
    } 
      ATH_MSG_WARNING( " number (" << nLocCoord << ") of local coordinates "
            << "must be 1 or 2, can not un-update!" ); 
      return nullptr;
    
}

// inverse updator #2 for Kalman Fitter - version with LocalParameters (for example for RIO_OnTrack)
Trk::TrackParameters* Trk::KalmanUpdatorSMatrix::removeFromState (const Trk::TrackParameters& trkPar,
                                                                  const LocalParameters& measmtPar,
                                                                  const Amg::MatrixX& measmtCov) const {
    if (msgLvl(MSG::DEBUG)) logStart("removeFromState(TP,LPAR,ERR)",trkPar);
    FitQualityOnSurface*    fitQoS = nullptr;
    return prepareFilterStep (trkPar, measmtPar, measmtCov,-1,fitQoS, false);
}

// inverse updator #3 for Kalman Fitter - version with Amg::Vector2D (for example for PrepRawData)
Trk::TrackParameters* Trk::KalmanUpdatorSMatrix::removeFromState (const Trk::TrackParameters& inputTP,
                                                                  const Amg::Vector2D& measLocPos, 
                                                                  const Amg::MatrixX& measLocCov,
                                                                  FitQualityOnSurface*& fitQoS) const {
    const int updatingSign = -1;
    if (msgLvl(MSG::VERBOSE)) logStart("removeFromState(TP,LPOS,ERR,FQ)",inputTP);
    if (fitQoS) {
      msg(MSG::WARNING) << "expect nil FitQuality pointer, refuse operation to"
            << " avoid mem leak!" << endmsg;
      return nullptr;
    } 
      SCovMatrix5 covTrk;
      
      if (!getStartCov(covTrk,inputTP,updatingSign)) return nullptr;
      
      int nLocCoord = measLocCov.cols();
      if (nLocCoord == 1) {
        return calculateFilterStep_1D (inputTP,
                                       SParVector5(&inputTP.parameters()[0],5),covTrk,
                                       measLocPos[0],1,measLocCov,
                                       updatingSign,fitQoS,true);
      } if (nLocCoord == 2) {
        return calculateFilterStep_2D (inputTP,
                                       SParVector5(&inputTP.parameters()[0],5),covTrk,
                                       SParVector2(measLocPos[0],measLocPos[1]),3,
                                       measLocCov,
                                       updatingSign,fitQoS,true);
      } 
        ATH_MSG_WARNING( " number (" << nLocCoord << ") of local coordinates"
              << " must be 1 or 2, can not un-update!" );
        return nullptr;
      
    
}

// inverse updator #4 for Kalman Fitter - version with LocalParameters (for example for RIO_OnTrack)
Trk::TrackParameters* Trk::KalmanUpdatorSMatrix::removeFromState (const Trk::TrackParameters& trkPar,
                                                                  const LocalParameters& measmtPar,
                                                                  const Amg::MatrixX& measmtCov,
                                                                  FitQualityOnSurface*& fitQoS) const {
    if (msgLvl(MSG::VERBOSE)) logStart("removeFromState(TP,LPAR,ERR,FQ)",trkPar);
    if (fitQoS) {
        ATH_MSG_WARNING( "expect nil FitQuality pointer, refuse operation to avoid mem leak!" );
        return nullptr;
    } 
        return prepareFilterStep (trkPar, measmtPar, measmtCov, -1, fitQoS, true);
    
}

// state-to-state updator, trajectory combination - version without fitQuality
Trk::TrackParameters* Trk::KalmanUpdatorSMatrix::combineStates (const Trk::TrackParameters& one,
                                                                const Trk::TrackParameters& two) const {
	// try if both Track Parameters are measured ones ?
	const AmgSymMatrix(5)* covOne = one.covariance();
	const AmgSymMatrix(5)* covTwo = two.covariance();

	// remember, either one OR two might have no error, but not both !
	if (!covOne && ! covTwo) {
		ATH_MSG_WARNING( "both parameters have no errors, invalid use of Updator::combineStates()" );
		return nullptr;
	}
	// if only one of two has an error, return that one
	if (!covOne) {
      if (msgLvl(MSG::VERBOSE)) logResult("combineStates(TP,TP)",two.parameters(),*covTwo);
      return two.clone();
    }
	if (!covTwo) {
      if (msgLvl(MSG::VERBOSE)) logResult("combineStates(TP,TP)",one.parameters(),*covOne);
      return one.clone();
    }
	
	// ... FIXME - TRT is so difficult, need to check that both parameters are in the same frame
	//			   otherwise go into frame of one !
	
	// call 5dim updator to combine using gain matrix formalism
    SCovMatrix5 covOneSM;
    for (int i=0; i<5; ++i)
      for (int j=0; j<=i; ++j) {
        covOneSM(j,i) = (*covOne)(j,i);
      }

    FitQualityOnSurface*    fitQoS = nullptr;
    return calculateFilterStep_5D(one,SParVector5(&one.parameters()[0],5),covOneSM,
                                  SParVector5(&two.parameters()[0],5),
                                  *covTwo,
                                  +1,fitQoS,false);
}

// state-to-state updator, trajectory combination - version with fitQuality
Trk::TrackParameters* Trk::KalmanUpdatorSMatrix::combineStates (const Trk::TrackParameters& one,
                                                                const Trk::TrackParameters& two,
                                                                FitQualityOnSurface*& fitQoS) const {
    // try if both Track Parameters are measured ones ?
    const AmgSymMatrix(5)* covOne = one.covariance();
    const AmgSymMatrix(5)* covTwo = two.covariance();

	// remember, either one OR two might have no error, but not both !
	if (!covOne && ! covTwo) {
		ATH_MSG_WARNING( "both parameters have no errors, invalid use of Updator::combineStates()" );
		return nullptr;
	}
	
	if (fitQoS) {
        ATH_MSG_WARNING( "expect nil FitQuality pointer, refuse operation to avoid mem leak!");
        return nullptr;
    }
    
	// if only one of two has an error, return that one
	if (!covOne) {
      if (msgLvl(MSG::VERBOSE)) logResult("combineStates(TP,TP)",two.parameters(),*covTwo);
      return two.clone();
    }
	if (!covTwo) {
      if (msgLvl(MSG::VERBOSE)) logResult("combineStates(TP,TP)",one.parameters(),*covOne);
      return one.clone();
    }
	
    // call 5dim updator to combine using gain matrix formalism
    SCovMatrix5 covOneSM;
    for (int i=0; i<5; ++i)
      for (int j=0; j<=i; ++j) {
        covOneSM(j,i) = (*covOne)(j,i);
    }
      
    return calculateFilterStep_5D(one,SParVector5(&one.parameters()[0],5),covOneSM,
                                  SParVector5(&two.parameters()[0],5),
                                  (*covTwo),
                                  +1,fitQoS,true);
}


// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdatorSMatrix::fullStateFitQuality (const Trk::TrackParameters& trkPar,
                                         const Amg::Vector2D& locPos,
                                         const Amg::MatrixX& covRio) const {
  ATH_MSG_DEBUG( "--> entered KalmanUpdatorSMatrix::fullStateFitQuality(TP,LPOS,ERR)" );
	// try if Track Parameters are measured ones ?
	if (!trkPar.covariance()) {
      ATH_MSG_ERROR( "updated track state has no error matrix" );
      return nullptr;
	}
	// For the LocalPos. version, need to get # meas. coord. from covariance matrix.
	int nLocCoord = covRio.cols();
    if (nLocCoord == 1) {
      return makeChi2_1D(SParVector5(&trkPar.parameters()[0],5),
                         (*trkPar.covariance()),
                         locPos[Trk::locX],covRio(0,0),
                         1,-1); // key=1, signForUpdatedChi2 = -1
    } if (nLocCoord == 2) {
      SCovMatrix2 SmeasCov;
      SmeasCov(0,0) = covRio(0,0);
      SmeasCov(1,0) = covRio(0,1);
      SmeasCov(1,1) = covRio(1,1);
      return makeChi2_2D(SParVector5(&trkPar.parameters()[0],5),
                         (*trkPar.covariance()),
                         SParVector2(locPos[Trk::locX],locPos[Trk::locY]),
                         SmeasCov, 2,-1);
    } 
      ATH_MSG_WARNING( "Error in local position - must be 1D or 2D!" );
      return nullptr;
    
}


// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdatorSMatrix::fullStateFitQuality (const Trk::TrackParameters& trkPar,
                                                const Trk::LocalParameters& parRio,
                                                const Amg::MatrixX& covRio) const {
  ATH_MSG_VERBOSE( "--> entered KalmanUpdatorSMatrix::fullStateFitQuality(TP,LPAR,ERR)" );
	
	// try if Track Parameters are measured ones ?
	if (!trkPar.covariance()) {
		ATH_MSG_ERROR( "updated track state has no error matrix" );
		return nullptr;
	}
    int nLocCoord = parRio.dimension();
    if ( ! consistentParamDimensions(parRio,covRio.cols()) ) return nullptr;
    // local params can NOT be accessed like vector[i], therefore need some acrobatics:
    ROOT::Math::SVector<int,5>  intAccessor;
    for (int i=0,k=0; i<5; ++i) { if (parRio.parameterKey() & (1<<i)) intAccessor(k++)=i; }
    if (nLocCoord == 1) {
      return makeChi2_1D(SParVector5(&trkPar.parameters()[0],5),
                         (*trkPar.covariance()),
                         parRio[s_enumAccessor.pardef[intAccessor(0)]],covRio(0,0),
                         parRio.parameterKey(),-1);
    } if (nLocCoord == 2) {
      SCovMatrix2 SmeasCov;
      for (int i=0, irow=0; i<5; ++i) {
        if (parRio.parameterKey() & (1<<i)) {
          SmeasCov(0,irow) = covRio(0,irow);
          SmeasCov(1,irow) = covRio(1,irow);
          ++irow;
        }
      }
      return makeChi2_2D(SParVector5(&trkPar.parameters()[0],5),
                         (*trkPar.covariance()),
                         SParVector2(parRio[s_enumAccessor.pardef[intAccessor(0)]],
                                     parRio[s_enumAccessor.pardef[intAccessor(1)]]),
                         SmeasCov, parRio.parameterKey(),-1);
    } if (nLocCoord == 5) {
      return makeChi2_5D(SParVector5(&trkPar.parameters()[0],5),
                         (*trkPar.covariance()),
                         SParVector5(parRio[Trk::locX],parRio[Trk::locY],
                                     parRio[Trk::phi0],parRio[Trk::theta],
                                     parRio[Trk::qOverP]),
                         covRio,-1);
    } // stay with Eigen for other cases

      // State to measurement dimensional reduction Matrix ( n x m )
      const Amg::MatrixX& H = parRio.expansionMatrix();
	
      // residuals
      Amg::VectorX r;
      if (parRio.parameterKey()==31) r = (parRio - trkPar.parameters());
      else r = (parRio - H*trkPar.parameters());
      
      // all the rest is outsourced to a common chi2 routine
      return makeChi2Object(r,(*trkPar.covariance()),covRio,H,-1);
    
}

// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdatorSMatrix::predictedStateFitQuality (const Trk::TrackParameters& predPar,
                                                     const Amg::Vector2D& rioLocPos,
                                                     const Amg::MatrixX& covRio) const {
  ATH_MSG_VERBOSE( "--> entered KalmanUpdatorSMatrix::predictedStateFitQuality(TP,LPOS,ERR)" );
	// try if Track Parameters are measured ones ?
	if (!predPar.covariance()) {
#if 0
      if (&predPar == nullptr) 
        ATH_MSG_WARNING( "input state is NULL in predictedStateFitQuality()" );
      else
#endif
        ATH_MSG_WARNING( "input state has no error matrix in predictedStateFitQuality()" );
      return nullptr;
	}
	// For the LocalPos. version, need to get # meas. coord. from covariance matrix.
	int nLocCoord = covRio.cols();
    if (nLocCoord == 1) {
      return makeChi2_1D(SParVector5(&predPar.parameters()[0],5),
                         (*predPar.covariance()),
                         rioLocPos[Trk::locX],covRio(0,0),
                         1,+1); // key=1, signForUpdatedChi2 = +1
    } if (nLocCoord == 2) {
      SCovMatrix2 SmeasCov;
      SmeasCov(0,0) = covRio(0,0);
      SmeasCov(1,0) = covRio(1,0);
      SmeasCov(1,1) = covRio(1,1);
      return makeChi2_2D(SParVector5(&predPar.parameters()[0],5),
                         (*predPar.covariance()),
                         SParVector2(rioLocPos[Trk::locX],rioLocPos[Trk::locY]),
                         SmeasCov, 2,+1);
    } 
      ATH_MSG_WARNING( "Error in local position - must be 1D or 2D!" );
      return nullptr;
    
}

// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdatorSMatrix::predictedStateFitQuality (const Trk::TrackParameters& predPar,
                                                     const Trk::LocalParameters& parRio,
                                                     const Amg::MatrixX& covRio) const {
  ATH_MSG_VERBOSE( "--> entered KalmanUpdatorSMatrix::predictedStateFitQuality(TP,LPAR,ERR)" );

    // try if Track Parameters are measured ones ?
	if (!predPar.covariance()) {
#if 0
      if (&predPar == nullptr) 
        ATH_MSG_WARNING( "input state is NULL in predictedStateFitQuality()" );
      else
#endif
        ATH_MSG_WARNING( "input state has no error matrix in predictedStateFitQuality()" );
      return nullptr;
	}
    int nLocCoord = parRio.dimension();
    if ( ! consistentParamDimensions(parRio,covRio.cols()) ) return nullptr;

    ROOT::Math::SVector<int,5>  intAccessor;
    for (int i=0,k=0; i<5; ++i) { if (parRio.parameterKey() & (1<<i)) intAccessor(k++)=i; }
    if (nLocCoord == 1) {
      return makeChi2_1D(SParVector5(&predPar.parameters()[0],5),
                         (*predPar.covariance()),
                         parRio[s_enumAccessor.pardef[intAccessor(0)]],covRio(0,0),
                         parRio.parameterKey(),+1);
    } if (nLocCoord == 2) {
      SCovMatrix2 SmeasCov;
      for (int i=0, irow=0; i<5; ++i) {
        if (parRio.parameterKey() & (1<<i)) {
          SmeasCov(0,irow) = covRio(0,irow);
          SmeasCov(1,irow) = covRio(1,irow);
          ++irow;
        }
      }
      return makeChi2_2D(SParVector5(&predPar.parameters()[0],5),
                         (*predPar.covariance()),
                         SParVector2(parRio[s_enumAccessor.pardef[intAccessor(0)]],
                                     parRio[s_enumAccessor.pardef[intAccessor(1)]]),
                         SmeasCov, parRio.parameterKey(),+1);
    } if (nLocCoord == 5 ) {
      return makeChi2_5D(SParVector5(&predPar.parameters()[0],5),
                         (*predPar.covariance()),
                         SParVector5(parRio[Trk::locX],parRio[Trk::locY],
                                     parRio[Trk::phi0],parRio[Trk::theta],
                                     parRio[Trk::qOverP]),
                         covRio,+1);
    } // stay with CLHEP for other cases

      // State to measurement dimensional reduction Matrix ( n x m )
      const Amg::MatrixX& H = parRio.expansionMatrix();
      
      // residuals
      Amg::VectorX r;
      if (parRio.parameterKey()==31) r = parRio - predPar.parameters();
      else r = parRio - H * predPar.parameters();
      
      // all the rest is outsourced to a common chi2 routine
      return makeChi2Object(r,(*predPar.covariance()),
                            covRio,H,+1);
    
}

// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdatorSMatrix::predictedStateFitQuality (const Trk::TrackParameters& trkParOne,
                                                     const Trk::TrackParameters& trkParTwo) const {
  ATH_MSG_VERBOSE("--> entered KalmanUpdatorSMatrix::predictedStateFitQuality(TP,TP)");
	// try if both Track Parameters are measured ones ?
  const AmgSymMatrix(5)* covOne = trkParOne.covariance();
	const AmgSymMatrix(5)* covTwo = trkParTwo.covariance();
  // remember, either one OR two might have no error, but not both !
	if (!covOne && ! covTwo) {
		ATH_MSG_WARNING( "both parameters have no errors, invalid use of Updator::fitQuality()" );
		return nullptr;
	}
	// if only one of two has an error, place a message.
	if (!covOne || ! covTwo) {
		ATH_MSG_DEBUG( "One parameter does not have uncertainties, assume initial state and return chi2=0.0");
		return new FitQualityOnSurface(0.f, 5);
	}
    return makeChi2_5D(SParVector5(&trkParOne.parameters()[0],5),
                       *covOne,
                       SParVector5(&trkParTwo.parameters()[0],5),
                       *covTwo, +1);
}

std::vector<double> Trk::KalmanUpdatorSMatrix::initialErrors() const {
  std::vector<double> E(5);
  for (int i=0; i<5; ++i) E[i] = std::sqrt(m_cov0(i));
  return E;
}

// analyse dimension of localParameters to call appropriate fast-access mathematics
Trk::TrackParameters* Trk::KalmanUpdatorSMatrix::prepareFilterStep (const Trk::TrackParameters& inputTrkPar,
                                                                    const Trk::LocalParameters&  parRio,
                                                                    const Amg::MatrixX& covRio,
                                                                    const int sign,
                                                                    Trk::FitQualityOnSurface*& fitQoS,
                                                                    bool createFQoS ) const {

    // try if Track Parameters are measured ones ?
    SCovMatrix5 covTrk;
    if (!getStartCov(covTrk,inputTrkPar,sign)) return nullptr;

    int nLocCoord = covRio.cols();
    if ( ! consistentParamDimensions(parRio,nLocCoord) ) return nullptr;
    if (msgLvl(MSG::VERBOSE)) logInputCov(covTrk,parRio,covRio);

    // local params can NOT be accessed like vector[i], therefore need some acrobatics:
    ROOT::Math::SVector<int,5>  intAccessor;
    for (int i=0,k=0; i<5; ++i) { if (parRio.parameterKey() & (1<<i)) intAccessor(k++)=i; }

    if (nLocCoord==1) return calculateFilterStep_1D (inputTrkPar,SParVector5(&inputTrkPar.parameters()[0],5),covTrk,
                                                     parRio[s_enumAccessor.pardef[intAccessor(0)]],
                                                     parRio.parameterKey(),covRio,
                                                     sign,fitQoS,createFQoS);
    if (nLocCoord==2) return calculateFilterStep_2D (inputTrkPar,SParVector5(&inputTrkPar.parameters()[0],5),covTrk,
                                                     SParVector2(parRio[s_enumAccessor.pardef[intAccessor(0)]],parRio[s_enumAccessor.pardef[intAccessor(1)]]),
                                                     parRio.parameterKey(),covRio,
                                                     sign,fitQoS,createFQoS);

    if (nLocCoord==3) return calculateFilterStep_3D (inputTrkPar,SParVector5(&inputTrkPar.parameters()[0],5),covTrk,
                                                     parRio,covRio,
                                                     sign,fitQoS,createFQoS);
    if (nLocCoord==4) return calculateFilterStep_4D (inputTrkPar,SParVector5(&inputTrkPar.parameters()[0],5),covTrk,
                                                     parRio,covRio,
                                                     sign,fitQoS,createFQoS);
    if (nLocCoord==5) return calculateFilterStep_5D (inputTrkPar,SParVector5(&inputTrkPar.parameters()[0],5),covTrk,
                                                     SParVector5(&(parRio[Trk::loc1]),5),covRio,
                                                     sign,fitQoS,createFQoS);
    return nullptr;
}

// calculations for Kalman updator and inverse Kalman filter
Trk::TrackParameters*
Trk::KalmanUpdatorSMatrix::calculateFilterStep_1D (const TrackParameters& TP,
                                                   const SParVector5&  trkPar,
                                                   const SCovMatrix5&  trkCov,
                                                   const double& measPar,
                                                   const int& paramKey,
                                                   const Amg::MatrixX& measCov,
                                                   const int sign,
                                                   Trk::FitQualityOnSurface*& fitQoS,
                                                   bool createFQoS ) const {
  // use measuring coordinate (variable "mk") instead of reduction matrix
  int mk=0;
  if (paramKey!=1) for (int i=0; i<5; ++i) if (paramKey & (1<<i)) mk=i;

  double r = measPar - trkPar(mk);
  double R = (sign * measCov(0,0)) + trkCov(mk,mk);
  if (R == 0.0) {
    ATH_MSG_DEBUG( "inversion of the error-on-the-residual failed.");
    return nullptr;
  } R = 1./R;

  // --- compute Kalman gain matrix
  ROOT::Math::SMatrix<double,5,1,ROOT::Math::MatRepStd<double, 5, 1> >
    K = trkCov.Sub<ROOT::Math::SMatrix<double,5,1,ROOT::Math::MatRepStd<double, 5, 1> > >(0,mk)*R;
  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE("-U- residual: r = " << r );
    ATH_MSG_VERBOSE( "-U- inv. sigmaR = " << R);
    ATH_MSG_VERBOSE( "-U- gain mtx     K=("
          <<std::setiosflags(std::ios::fixed | std::ios::showpoint | std::ios::right)
          << std::setw(7) << std::setprecision(4) << K(0,0)<<", " 
          << std::setw(7) << std::setprecision(4) << K(1,0)<<", "
          << std::setw(7) << std::setprecision(4) << K(2,0)<<", "
          << std::setw(7) << std::setprecision(4) << K(3,0)<<", "
          << std::setw(7) << std::setprecision(4) << K(4,0)<<")"
          << std::resetiosflags(std::ios::fixed));
  }

  // --- compute local filtered state, here = TP+K*r = TP + TCov * H.T * R * r
  SParVector5 newPar = trkPar + trkCov.Col(mk) * R * r;

  if (!thetaWithinRange_5D(newPar)) {
    if ( mk!=Trk::theta &&
      (std::abs(R*r)>1.0 || trkCov(Trk::theta,Trk::theta) > 0.1*m_cov0(Trk::theta))) {
      ATH_MSG_DEBUG( "calculateFS_1D(): decided to damp update of theta and re-calculate." );
      SParVector5 dampedCov = trkCov.Col(mk);
      dampedCov(Trk::theta) = dampedCov(Trk::theta)*m_thetaGainDampingValue;
      newPar = trkPar + dampedCov * R * r;
      K(Trk::theta,0) = K(Trk::theta,0)*m_thetaGainDampingValue;
      ATH_MSG_DEBUG( "-U- damped gain  K=("
            <<std::setiosflags(std::ios::fixed | std::ios::showpoint | std::ios::right)
            << std::setw(7) << std::setprecision(4) << K(0,0)<<", " 
            << std::setw(7) << std::setprecision(4) << K(1,0)<<", "
            << std::setw(7) << std::setprecision(4) << K(2,0)<<", "
            << std::setw(7) << std::setprecision(4) << K(3,0)<<", "
            << std::setw(7) << std::setprecision(4) << K(4,0)<<")"
            << std::resetiosflags(std::ios::fixed) );
    } else {
      ATH_MSG_DEBUG( "-U- theta out of range but can not damp this update.");
    }
  }

  // --- compute covariance matrix of local filteres state
  SGenMatrix5 KtimesH ; KtimesH.Place_at(K,0,mk);
  SGenMatrix5 M(m_unitMatrix - KtimesH);
  ATH_MSG_VERBOSE( "-U- matrix M: diag=("
                                 << M(0,0)<<"," << M(1,1)<<","
                                 << M(2,2)<<"," << M(3,3)<<","
                                 << M(4,4)               <<")" );
  SCovMatrix5 newCov;
  if (!m_useFruehwirth8a) {
    // either: use formula C = M * trkCov * M.T() +/- K * covRio * K.T()
    SCovMatrix1 measuredSMatrix1D(measCov(0,0));
    newCov = ROOT::Math::Similarity(M,trkCov)
      + sign*(ROOT::Math::Similarity(K,measuredSMatrix1D));
  } else {
    // or: original Fruehwirth eq. 8a is simpler, expression checked to be symm.
    // C = (1 - K*H) * trkCov
    /*    SGenMatrix5 KtimesH ; KtimesH.Place_at(K,0,mk);
          SCovMatrix5 M;
          ROOT::Math::AssignSym::Evaluate(M, KtimesH * trkCov);
          newCov -= M;*/
    ROOT::Math::AssignSym::Evaluate(newCov, M * trkCov);
  }
  if ( (!thetaPhiWithinRange_5D(newPar, Trk::absoluteCheck)) ?
       !correctThetaPhiRange_5D(newPar, newCov, Trk::absoluteCheck) : false ) {
    ATH_MSG_WARNING("calculateFS_1D(): bad angles in filtered state!");
    return nullptr;
  }


  if (createFQoS) {
    double predictedResidual = (sign<0) ?r:measPar - newPar(mk);
    SCovMatrix5 updatedCov   = (sign<0) ?
      trkCov                             : // when removing, the input are updated par
      newCov                             ; // when adding, chi2 is made from upd. par

    // for both signs (add/remove) the chi2 is now calculated like for updated states
    double chiSquared = measCov(0,0) - updatedCov(mk,mk);
    if (chiSquared == 0.0)
        ATH_MSG_INFO( "division by zero in 1D chi2, set chi2 to 0.0 instead" );
    else {
      // get chi2 = r.T() * R^-1 * r
      chiSquared = predictedResidual*predictedResidual/chiSquared;
      ATH_MSG_VERBOSE( "-U- fitQuality of "<< (sign>0?" added ":"removed")
              <<" state, chi2 :" << chiSquared << " / ndof= 1" );
    }
    fitQoS = new FitQualityOnSurface(chiSquared, 1);
  }
  return convertToClonedTrackPars(TP,newPar,newCov,sign,createFQoS,"1D");
}

// calculations for Kalman updator and inverse Kalman filter
Trk::TrackParameters*
Trk::KalmanUpdatorSMatrix::calculateFilterStep_2D (const TrackParameters& TP,
                                                   const SParVector5&  trkPar,
                                                   const SCovMatrix5&  trkCov,
                                                   const SParVector2&  SmeasPar,
                                                   const int& paramKey,
                                                   const Amg::MatrixX& measCov,
                                                   const int sign,
                                                   Trk::FitQualityOnSurface*& fitQoS,
                                                   bool createFQoS ) const {
  // make reduction matrix H from LocalParameters
  ROOT::Math::SMatrix<double,2,5,ROOT::Math::MatRepStd<double, 2, 5> > H;
  // and transform EDM to new math lib
  SCovMatrix2 SmeasCov;
  for (int i=0, irow=0; i<5; ++i) {
    if (paramKey & (1<<i)) {
      SParVector5 v; v(i)=1.0;
      H.Place_in_row(v, irow, 0);
      SmeasCov(0,irow) = measCov(0,irow);
      SmeasCov(1,irow) = measCov(1,irow);
      ++irow;
    }
  }
  SParVector2 r = SmeasPar - H * trkPar;
  //  FIXME catchPiPi;
  SCovMatrix2 R = sign * SmeasCov + projection_2D(trkCov,paramKey);
  if (!R.Invert()) {
    ATH_MSG_DEBUG( "inversion of residual error matrix (2D) failed.");
    return nullptr;
  }

  // --- compute Kalman gain matrix
  ROOT::Math::SMatrix<double,5,2,ROOT::Math::MatRepStd<double, 5, 2> > 
    K = trkCov * ROOT::Math::Transpose(H) * R;
  if (msgLvl(MSG::VERBOSE)) {
    SParVector5 trans_r; SCovMatrix5 trans_R; SGenMatrix5 trans_K; 
    logGainForm (2,  trans_r.Place_at(r,0),
                 trans_R.Place_at(R,0,0), 
                 trans_K.Place_at(K,0,0));
  }

  // --- compute local filtered state
  SParVector5 newPar = trkPar + K * r;
  if (!thetaWithinRange_5D(newPar)) {
    if (H(0,Trk::theta) != 1.0 && H(1,Trk::theta) != 1.0 &&
        ( std::abs(R(0,0)*r(0))>1.0 || std::abs(R(1,1)*r(1))>1.0 ||
          trkCov(Trk::theta,Trk::theta) > 0.1*m_cov0(Trk::theta))) {
      ATH_MSG_DEBUG( "calculateFS_2D(): decided to damp update of theta and re-calculate.");
      K(Trk::theta,0) = K(Trk::theta,0)*m_thetaGainDampingValue;
      K(Trk::theta,1) = K(Trk::theta,1)*m_thetaGainDampingValue;
      newPar = trkPar + K * r;
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "-U- damped gain K0=("
              <<std::setiosflags(std::ios::fixed | std::ios::showpoint | std::ios::right)
              << std::setw(7) << std::setprecision(4) << K(0,0)<<", " 
              << std::setw(7) << std::setprecision(4) << K(1,0)<<", "
              << std::setw(7) << std::setprecision(4) << K(2,0)<<", "
              << std::setw(7) << std::setprecision(4) << K(3,0)<<", "
              << std::setw(7) << std::setprecision(4) << K(4,0)<<")"
              << std::resetiosflags(std::ios::fixed) << endmsg;
        msg(MSG::DEBUG) << "-U-             K1=("
              <<std::setiosflags(std::ios::fixed | std::ios::showpoint | std::ios::right)
              << std::setw(7) << std::setprecision(4) << K(0,1)<<", " 
              << std::setw(7) << std::setprecision(4) << K(1,1)<<", "
              << std::setw(7) << std::setprecision(4) << K(2,1)<<", "
              << std::setw(7) << std::setprecision(4) << K(3,1)<<", "
              << std::setw(7) << std::setprecision(4) << K(4,1)<<")"
              << std::resetiosflags(std::ios::fixed) << endmsg;
      }
    } else {
      ATH_MSG_DEBUG( "-U- theta out of range but can not damp this update.");
    }
  }

  // --- compute filtered covariance matrix
  SGenMatrix5 M = m_unitMatrix - K * H;
  SCovMatrix5 newCov;
  if (!m_useFruehwirth8a) {
    // compute covariance matrix of local filteres state
    //   C = M * covTrk * M.T() +/- K * covRio * K.T()
    newCov = ROOT::Math::Similarity(M,trkCov)
      + sign*(ROOT::Math::Similarity(K,SmeasCov));
  } else {
    //   C = (1 - KH) trkCov = trkCov - K*H*trkCov
    /*    SGenMatrix5 KtimesH = K*H;
    SCovMatrix5 M;
    ROOT::Math::AssignSym::Evaluate(M, KtimesH * trkCov);
    newCov -= M; */
    ROOT::Math::AssignSym::Evaluate(newCov, M * trkCov);
  }
  if ( (!thetaPhiWithinRange_5D(newPar, Trk::absoluteCheck)) ?
       !correctThetaPhiRange_5D(newPar, newCov, Trk::absoluteCheck) : false ) {
    ATH_MSG_WARNING( "calculateFS_2D(): bad angles in filtered state!" );
    return nullptr;
  }

  if (createFQoS) {      // get chi2 = r.T() * R2^-1 * r
    double  chiSquared = (sign>0) ?
    // when adding, the r and R are ready to for calculating the predicted chi2
    ROOT::Math::Similarity(r,R) :
    // when removing the r and -R are ready for calculating the updated chi2.
    ROOT::Math::Similarity(r,-R);
    ATH_MSG_DEBUG( "-U- fitQuality of "<< (sign>0?" added ":"removed")
            <<" state, chi2 :" << chiSquared << " / ndof= 2" );
    fitQoS = new FitQualityOnSurface(chiSquared, 2);
  }
  return convertToClonedTrackPars(TP,newPar,newCov,sign,createFQoS,"2D");
}

// calculations for Kalman updator and inverse Kalman filter
Trk::TrackParameters*
Trk::KalmanUpdatorSMatrix::calculateFilterStep_3D (const TrackParameters& TP,
                                                   const SParVector5&  trkPar,
                                                   const SCovMatrix5&  trkCov,
                                                   const Trk::LocalParameters&  measPar,
                                                   const Amg::MatrixX& measCov,
                                                   const int sign,
                                                   Trk::FitQualityOnSurface*& fitQoS,
                                                   bool createFQoS ) const {
  // transform EDM to new math lib
  SParVector3 SmeasPar(measPar[Trk::locX],measPar[Trk::locY],measPar[Trk::phi]);
  SCovMatrix3 SmeasCov;
  // make reduction matrix H from LocalParameters
  ROOT::Math::SMatrix<double,3,5,ROOT::Math::MatRepStd<double, 3, 5> > H;
  for (int i=0, irow=0; i<5; ++i) {
    if (measPar.parameterKey() & (1<<i)) {
      SParVector5 v; v(i)=1.0;
      H.Place_in_row(v, irow, 0);
      SmeasCov(0,irow) = measCov(0,irow);
      SmeasCov(1,irow) = measCov(1,irow);
      SmeasCov(2,irow) = measCov(2,irow);
      ++irow;
    }
  }
  SParVector3 r = SmeasPar - H * trkPar;
  //  FIXME catchPiPi;
  SCovMatrix3 R = sign * SmeasCov + projection_3D(trkCov,measPar.parameterKey());
  if (!R.Invert()) {
    ATH_MSG_DEBUG( "inversion of residual error matrix (3D) failed.");
    return nullptr;
  }

  // compute Kalman gain matrix
  ROOT::Math::SMatrix<double,5,3,ROOT::Math::MatRepStd<double, 5, 3> > 
    K = trkCov * ROOT::Math::Transpose(H) * R;
  SGenMatrix5 M = m_unitMatrix - K * H;
  if (msgLvl(MSG::VERBOSE)) {
    SParVector5 trans_r; SCovMatrix5 trans_R; SGenMatrix5 trans_K; 
    logGainForm (3,  trans_r.Place_at(r,0),
                 trans_R.Place_at(R,0,0), trans_K.Place_at(K,0,0));
  }

  // compute local filtered state
  SParVector5 newPar = trkPar + K * r;

  SCovMatrix5 newCov;
  if (!m_useFruehwirth8a) {
    // compute covariance matrix of local filteres state
    //   C = M * covTrk * M.T() +/- K * covRio * K.T()
    newCov = ROOT::Math::Similarity(M,trkCov)
      + sign*(ROOT::Math::Similarity(K,SmeasCov));
  } else {
    //   C = (1 - KH) trkCov = trkCov - K*H*trkCov
    /*    SGenMatrix5 KtimesH = K*H;
    SCovMatrix5 M;
    ROOT::Math::AssignSym::Evaluate(M, KtimesH * trkCov);
    newCov -= M; */
    ROOT::Math::AssignSym::Evaluate(newCov, M * trkCov);
  }

  if ( (!thetaPhiWithinRange_5D(newPar,Trk::absoluteCheck)) ?
       !correctThetaPhiRange_5D(newPar,newCov,Trk::absoluteCheck) : false ) {
    ATH_MSG_WARNING( "calculateFS_3D(): bad angles in filtered state!" );
    return nullptr;
  }

  if (createFQoS) {
    SParVector3 predictedResidual = (sign<0) ?
      r                                      :
      SmeasPar - H * newPar                  ;
    SCovMatrix5 updatedCov   = (sign<0) ?
      trkCov                            : // when removing, the input are updated par
      newCov                            ; // when adding, chi2 is made from upd. par

    // for both signs (add/remove) the chi2 is now calculated like for updated states
    SCovMatrix3 R2 = SmeasCov - projection_3D(updatedCov,measPar.parameterKey());
    double  chiSquared;	
    if ( !R2.Invert() ) {
      ATH_MSG_DEBUG( "matrix (3D) inversion not possible, set chi2 to zero");
      chiSquared = 0.f;
    } else {
      // get chi2 = r.T() * R2^-1 * r
      chiSquared = ROOT::Math::Similarity(predictedResidual,R2);
      ATH_MSG_VERBOSE( "-U- fitQuality of "<< (sign>0?" added ":"removed")
              <<" state, chi2 :" << chiSquared << " / ndof= 3" );
    }
    fitQoS = new FitQualityOnSurface(chiSquared, 3);
  }
  return convertToClonedTrackPars(TP,newPar,newCov,sign,createFQoS,"3D");
}

// calculations for Kalman updator and inverse Kalman filter
Trk::TrackParameters*
Trk::KalmanUpdatorSMatrix::calculateFilterStep_4D (const TrackParameters& TP,
                                            const SParVector5&  trkPar,
                                            const SCovMatrix5&  trkCov,
                                            const Trk::LocalParameters&  measPar,
                                            const Amg::MatrixX& measCov,
                                            const int sign,
                                            Trk::FitQualityOnSurface*& fitQoS,
                                            bool createFQoS ) const {
  // transform EDM to new math lib
  SParVector4 SmeasPar(measPar[Trk::d0],measPar[Trk::z0],
                       measPar[Trk::phi0],measPar[Trk::theta]);
  SCovMatrix4 SmeasCov;

  // make reduction matrix H from LocalParameters
  ROOT::Math::SMatrix<double,4,5,ROOT::Math::MatRepStd<double, 4, 5> > H;
  for (int i=0, irow=0; i<5; ++i) {
    if (measPar.parameterKey() & (1<<i)) {
      SParVector5 v; v(i)=1.0;
      H.Place_in_row(v, irow, 0);
      SmeasCov(0,irow) = measCov(0,irow);
      SmeasCov(1,irow) = measCov(1,irow);
      SmeasCov(2,irow) = measCov(2,irow);
      SmeasCov(3,irow) = measCov(3,irow);
      ++irow;
    }
  }
  SParVector4 r = SmeasPar - H * trkPar;
  //  FIXME catchPiPi;
  SCovMatrix4 R = sign * SmeasCov + projection_4D(trkCov,measPar.parameterKey());
  if (!R.Invert()) {
    ATH_MSG_DEBUG( "inversion of residual error matrix (4D) failed." );
    return nullptr;
  }

  // compute Kalman gain matrix
  ROOT::Math::SMatrix<double,5,4,ROOT::Math::MatRepStd<double, 5, 4> > 
    K = trkCov * ROOT::Math::Transpose(H) * R;
  SGenMatrix5 M = m_unitMatrix - K * H;
  if (msgLvl(MSG::VERBOSE)) {
    SParVector5 trans_r; SCovMatrix5 trans_R; SGenMatrix5 trans_K; 
    logGainForm (4,  trans_r.Place_at(r,0),
                 trans_R.Place_at(R,0,0), trans_K.Place_at(K,0,0));
  }

  // compute local filtered state
  SParVector5 newPar = trkPar + K * r;

  SCovMatrix5 newCov;
  if (!m_useFruehwirth8a) {
    // compute covariance matrix of local filteres state
    //   C = M * covTrk * M.T() +/- K * covRio * K.T()
    newCov = ROOT::Math::Similarity(M,trkCov)
      + sign*(ROOT::Math::Similarity(K,SmeasCov));
  } else {
    //   C = (1 - KH) trkCov = trkCov - K*H*trkCov
    /*    SGenMatrix5 KtimesH = K*H;
    SCovMatrix5 M;
    ROOT::Math::AssignSym::Evaluate(M, KtimesH * trkCov);
    newCov -= M; */
    ROOT::Math::AssignSym::Evaluate(newCov, M * trkCov);
  }

  if ( (!thetaPhiWithinRange_5D(newPar,Trk::absoluteCheck)) ?
       !correctThetaPhiRange_5D(newPar,newCov,Trk::absoluteCheck) : false ) {
    ATH_MSG_WARNING( "calculateFS_4D(): bad angles in filtered state!" );
    return nullptr;
  }

  if (createFQoS) {
    SParVector4 predictedResidual = (sign<0) ?r:SmeasPar - H * newPar;
    SCovMatrix5 updatedCov   = (sign<0) ?
      trkCov                            : // when removing, the input are updated par
      newCov                            ; // when adding, chi2 is made from upd. par

    // for both signs (add/remove) the chi2 is now calculated like for updated states
    SCovMatrix4 R2 = SmeasCov - projection_4D(updatedCov,measPar.parameterKey());
    double  chiSquared;	
    if ( !R2.Invert() ) {
      ATH_MSG_DEBUG( "matrix (4D) inversion not possible, set chi2 to zero");
      chiSquared = 0.f;
    } else {
      // get chi2 = r.T() * R2^-1 * r
      chiSquared = ROOT::Math::Similarity(predictedResidual,R2);
      ATH_MSG_VERBOSE( "-U- fitQuality of "<< (sign>0?" added ":"removed")
              <<" state, chi2 :" << chiSquared << " / ndof= 4" );
    }
    fitQoS = new FitQualityOnSurface(chiSquared, 4);
  }
  return convertToClonedTrackPars(TP,newPar,newCov,sign,createFQoS,"4D");
}

// calculations for Kalman updator and inverse Kalman filter
Trk::TrackParameters*
Trk::KalmanUpdatorSMatrix::calculateFilterStep_5D (const TrackParameters& TP,
                                            const SParVector5&  trkParOne,
                                            const SCovMatrix5&  trkCovOne,
                                            const SParVector5&  trkParTwo,
                                            const Amg::MatrixX& measCov,
                                            const int sign,
                                            Trk::FitQualityOnSurface*& fitQoS,
                                            bool createFQoS ) const {
  SCovMatrix5 trkCovTwo;   // transform EDM to new math lib
  for (int i=0; i<5; ++i)
    for (int j=0; j<=i; ++j) {
      trkCovTwo(j,i) = measCov(j,i);
    }

  // this update is symmetric in state and measurement parameters, H == 1.
  SCovMatrix5 R = sign * trkCovTwo + trkCovOne;
  if (!R.Invert()) {
    ATH_MSG_DEBUG( "inversion of residual error matrix (5D) failed." );
    return nullptr;
  }

  SParVector5 r = trkParTwo - trkParOne;
  if ( (!thetaPhiWithinRange_5D(r,Trk::differentialCheck)) ?
       !correctThetaPhiRange_5D(r,R,Trk::differentialCheck) : false ) {
    ATH_MSG_WARNING( "calculateFS_5D(): bad angles in intermediate residual!" );
    return nullptr;
  }
  SGenMatrix5 K = trkCovOne * R;
  if (msgLvl(MSG::VERBOSE)) logGainForm (5,  r, R, K);

  // compute local filtered state
  SParVector5 newPar = trkParOne + K * r;
  SCovMatrix5 newCov;
  /*  if (!m_useFruehwirth8a) {
    SGenMatrix5 M = m_unitMatrix - K;
    // compute covariance matrix of local filtered state
    //   C = M * covTrk * M.T() +/- K * covRio * K.T()
    newCov = ROOT::Math::Similarity(M,trkCovOne)
      + sign*(ROOT::Math::Similarity(K,trkCovTwo));
      } else { */
    // original Fruehwirth eq. 8a is simpler, expression checked to be symm.
    ROOT::Math::AssignSym::Evaluate(newCov, K * trkCovTwo);
    //  }

  if ( (!thetaPhiWithinRange_5D(newPar,Trk::absoluteCheck)) ?
       !correctThetaPhiRange_5D(newPar,newCov,Trk::absoluteCheck) : false ) {
    ATH_MSG_WARNING( "calculateFS_5D(): bad angles in filtered state!" );
    return nullptr;
  }

  bool goodMatrix(true);
  for (int i=0; i<5; ++i) {
    if (newCov(i,i) < 0.0 && goodMatrix ) goodMatrix=false;
  }
  if (!goodMatrix) {
    SGenMatrix5 M = m_unitMatrix - K;
    // compute covariance matrix of local filtered state
    //   C = M * covTrk * M.T() +/- K * covRio * K.T()
    newCov = ROOT::Math::Similarity(M,trkCovOne)
      + sign*(ROOT::Math::Similarity(K,trkCovTwo));
    goodMatrix = true;
    for (int i=0; i<5; ++i) {
      if (newCov(i,i) < 0.0 && goodMatrix ) goodMatrix=false;
      if (!goodMatrix) {
        ATH_MSG_DEBUG("calculateFS_5D(): unphysical cov!");
        return nullptr;
      }
    }
  }

  if (createFQoS) {      // get chi2 = r.T() * R2^-1 * r
    double  chiSquared = (sign>0) ?
      // when adding, the r and R are ready to for calculating the predicted chi2
      ROOT::Math::Similarity(r,R) :
      // when removing the r and -R are ready for calculating the updated chi2.
      ROOT::Math::Similarity(r,-R);
    ATH_MSG_VERBOSE( "-U- fitQuality of "<< (sign>0?" added ":"removed")
            <<" state, chi2 :" << chiSquared << " / ndof= 5" );
    fitQoS = new FitQualityOnSurface(chiSquared, 5);
  }
  return convertToClonedTrackPars(TP,newPar,newCov,sign,createFQoS,"5D");
}


Trk::FitQualityOnSurface* Trk::KalmanUpdatorSMatrix::makeChi2_1D(const SParVector5& parTrk,
                                                          const Amg::MatrixX& covTrk,
                                                          const double& valRio,
                                                          const double& covRio,
                                                          const int& key,
                                                          const int& sign) const
{   // sign: -1 = updated, +1 = predicted parameters.
  int mk=0;
  if (key!=1) for (int i=0; i<5; ++i) if (key & (1<<i)) mk=i;
  double r = valRio - parTrk(mk);
  //  if (mk==3) catchPiPi;
  double chiSquared = covRio + sign * covTrk(mk,mk);
  if (chiSquared == 0.0) {
    ATH_MSG_DEBUG( "inversion of the error-on-the-residual failed." );
    return nullptr;
  } 
    chiSquared = r*r/chiSquared;
  
  return new FitQualityOnSurface(chiSquared, 1);
}

Trk::FitQualityOnSurface* Trk::KalmanUpdatorSMatrix::makeChi2_2D(const SParVector5& parTrk,
                                                          const Amg::MatrixX& covTrk,
                                                          const SParVector2& parRio,
                                                          const SCovMatrix2& covRio,
                                                          const int& key,
                                                          const int& sign) const
{   // sign: -1 = updated, +1 = predicted parameters.

  ROOT::Math::SVector<int,2> index(0,1); // locX and Y if key==3
  if (key != 3) { // other localPar expansion vector if not key==3.
    for (int i=0, irow=0; i<5; ++i)
      if (key & (1<<i)) index(irow++) = i;
  }
  SParVector2 r(-parTrk(index(0)),-parTrk(index(1))); r += parRio;
  SCovMatrix2 R = sign*projection_2D(covTrk,key);     R += covRio;
  double chiSquared = 0.0;
  if (!R.Invert()) {
    ATH_MSG_DEBUG( "matrix inversion not possible, set chi2 to zero" );
  } else {
    chiSquared = ROOT::Math::Similarity(r,R);
    ATH_MSG_VERBOSE( "-U- fitQuality of "<< (sign>0?"predicted":"updated")
            <<" state, chi2 :" << chiSquared << " / ndof= 2" );
  }
  return new FitQualityOnSurface(chiSquared, 2);
}

Trk::FitQualityOnSurface* Trk::KalmanUpdatorSMatrix::makeChi2_5D(const SParVector5& parOne,
                                                          const Amg::MatrixX& covOne,
                                                          const SParVector5& parTwo,
                                                          const Amg::MatrixX& covTwo,
                                                          const int& sign) const
{   // sign: -1 = updated, +1 = predicted parameters.

  SCovMatrix5 ScovOne;
  SCovMatrix5 ScovTwo; // trafo EDM to new math lib
  for (int i=0; i<5; ++i) 
    for (int j=0; j<=i; ++j) {
      ScovOne(i,j) = covOne(i,j);
      ScovTwo(i,j) = covTwo(i,j);
    }
  SParVector5 r = parTwo - parOne;
  SCovMatrix5 R = sign*ScovOne + ScovTwo;
  double chiSquared = 0.0;
  if (!R.Invert()) {
    ATH_MSG_DEBUG( "matrix inversion not possible, set chi2 to zero" );
  } else {
    chiSquared = ROOT::Math::Similarity(r,R);
    ATH_MSG_VERBOSE( "-U- fitQuality of "<< (sign>0?"predicted":"updated")
            <<" state, chi2 :" << chiSquared << " / ndof= 2" );
  }
  return new FitQualityOnSurface(chiSquared, 5);
}

Trk::TrackParameters* 
Trk::KalmanUpdatorSMatrix::convertToClonedTrackPars(const Trk::TrackParameters& TP,
                                                    const SParVector5& par,
                                                    const SCovMatrix5& covpar,
                                                    const int& sign,
                                                    const bool& createFQoS,
                                                    std::string_view ndtext) const {
  AmgSymMatrix(5)* C = new AmgSymMatrix(5);
  C->setZero();
  for (int i=0; i<5; ++i) {
    for (int j=0; j<=i; ++j) {
      C->fillSymmetric(i,j, covpar(i,j)); 
    }
  }
  
  Trk::TrackParameters* resultPar = 
    TP.associatedSurface().createTrackParameters(par[0],par[1],par[2],par[3],par[4],C);
  
  if (msgLvl(MSG::VERBOSE) && resultPar) {
    char reportCalledInterface[80];
    char ndtext2[5];
    memset(ndtext2, '\0', 5 ); ndtext.copy(ndtext2,2); // convert char to string
    if (sign>0) 
      sprintf(reportCalledInterface,"%s-%s,%s)",
              (ndtext=="5D"?"combineStates(TP,TP":"addToState(TP,Meas"),
              ndtext2,(createFQoS?"Err,FQ":"Err"));
    else 
      sprintf(reportCalledInterface,"%s,Meas-%s,%s)","removeFromState(TP,",
              ndtext2,(createFQoS?"Err,FQ":"Err"));
    logResult((std::string)reportCalledInterface, resultPar->parameters(),*C);
  }
  
  return resultPar;
}

SCovMatrix2 Trk::KalmanUpdatorSMatrix::projection_2D(const SCovMatrix5& M, const int& key) const
{
  if (key == 3) { // shortcut the most-used case
    SCovMatrix2 S = M.Sub<SCovMatrix2> (0,0);
    return S;
  } 
    ROOT::Math::SVector<int,2>  iv;
    for (int i=0,k=0; i<5; ++i) { if (key & (1<<i)) iv(k++)=i; }
    SCovMatrix2 covSubMatrix;
    for (int i=0; i<2; ++i) {
      for (int j=0; j<2; ++j) {
        covSubMatrix(i,j) = M(iv(i),iv(j));
      }
    }
    return covSubMatrix;
  
}

SCovMatrix2 Trk::KalmanUpdatorSMatrix::projection_2D(const Amg::MatrixX& M,
                                              const int& key) const
{
    ROOT::Math::SVector<int,2>  iv;
    for (int i=0,k=0; i<5; ++i) { if (key & (1<<i)) iv(k++)=i; }
    SCovMatrix2 covSubMatrix;
    for (int i=0; i<2; ++i) {
      for (int j=0; j<2; ++j) {
        covSubMatrix(i,j) = M(iv(i),iv(j));
      }
    }
    return covSubMatrix;
}

SCovMatrix3 Trk::KalmanUpdatorSMatrix::projection_3D(const SCovMatrix5& M, const int& key) const
{
  if (key == 7) { // shortcut the most-used case
    return M.Sub<SCovMatrix3> (0,0);
  } 
    ROOT::Math::SVector<int,3>  iv;
    for (int i=0,k=0; i<5; ++i) { if (key & (1<<i)) iv(k++)=i; }
    SCovMatrix3 covSubMatrix;
    for (int i=0; i<3; ++i) {
      for (int j=0; j<3; ++j) {
        covSubMatrix(i,j) = M(iv(i),iv(j));
      }
    }
    return covSubMatrix;
  
}

SCovMatrix4 Trk::KalmanUpdatorSMatrix::projection_4D(const SCovMatrix5& M, const int& key) const
{
  if (key == 15) { // shortcut the most-used case
    return M.Sub<SCovMatrix4> (0,0);
  } 
    ROOT::Math::SVector<int,4> iv;
    for (int i=0,k=0; i<5; ++i) { if (key & (1<<i)) iv(k++)=i; }
    SCovMatrix4 covSubMatrix;
    for (int i=0; i<4; ++i) {
      for (int j=0; j<4; ++j) {
        covSubMatrix(i,j) = M(iv(i),iv(j));
      }
    }
    return covSubMatrix;
  
}

bool Trk::KalmanUpdatorSMatrix::getStartCov(SCovMatrix5& M,
                                            const Trk::TrackParameters& inputParameters,
                                            const int isign) const
{
  
  const AmgSymMatrix(5)* covariance = inputParameters.covariance();
  if (!covariance) {
    if (isign<0) {
      ATH_MSG_WARNING ("MeasuredTrackParameters == Null, can not calculate updated parameter state.");
      return false;
    } 
      // no error given - use a huge error matrix for the time
      ATH_MSG_VERBOSE ("-U- no covTrk at input - assign large error matrix for the time being.");
      M.SetDiagonal(m_cov0);
    
  } else {
    //    int k = measTrkPar->localAmg::MatrixX().covariance().cols(); is always 5
    for (int i=0; i<5; ++i) {
      for (int j=0; j<=i; ++j) {
        M(i,j) = (*covariance)(i,j); // can improve speed by apply() access
      }
    }
  }
  return true;
}


Trk::FitQualityOnSurface* Trk::KalmanUpdatorSMatrix::makeChi2Object(const Amg::VectorX& residual,
                                                                    const Amg::MatrixX& covTrk,
                                                                    const Amg::MatrixX& covRio,
                                                                    const Amg::MatrixX& H,
                                                                    const int& sign) const
{   // sign: -1 = updated, +1 = predicted parameters.
    Amg::MatrixX R = covRio + sign * covTrk.similarity(H);
    double chiSquared = 0.;
    if (false) { //!< TODO check on inverse failure ?
        ATH_MSG_DEBUG( "matrix inversion not possible, set chi2 to zero" );
		chiSquared = 0.f;
    } else {
      // get chi2 = r.T() * R^-1 * r
      chiSquared = residual.transpose() * R.inverse() * residual;
      ATH_MSG_VERBOSE( "-U- fitQuality of "<< (sign>0?"predicted":"updated")
              <<" state, chi2 :" << chiSquared << " / ndof= " << covRio.cols() );
    }

    // number of degree of freedom added
    int		numberDoF  = covRio.cols();
	
   return new FitQualityOnSurface(chiSquared, numberDoF);
}

bool Trk::KalmanUpdatorSMatrix::consistentParamDimensions(const Trk::LocalParameters& P,
                                                   const int& dimCov) const {
  if (P.dimension() != dimCov ) {
    ATH_MSG_WARNING ("Inconsistency in dimension of local coord - problem with LocalParameters object?");
    ATH_MSG_WARNING ("dim of local parameters: "<<P.dimension()<<" vs. dim of error matrix: "<<dimCov);
    ATH_MSG_INFO    ("==> refuse update or chi2 calculation");
    return false;
  } return true;
}

bool Trk::KalmanUpdatorSMatrix::correctThetaPhiRange_5D(SParVector5& V,SCovMatrix5& M,
                                                        const RangeCheckDef rcd) const
{
  static const SParVector2 thetaMin(0.0, -M_PI);
  
  // correct theta coordinate
  if ( V(Trk::theta)<thetaMin((int)rcd) || V(Trk::theta)> M_PI ) {
    ATH_MSG_DEBUG ("-U- theta angle out of range: theta= "<<V(Trk::theta)<<", phi= "<<V(Trk::phi));
    // absolute theta: repair if between -pi and +2pi.
    // differential theta: repair if between -pi and +pi
    if ( ( V(Trk::theta) < -M_PI ) ||
         ( V(Trk::theta) > (rcd==Trk::differentialCheck ? M_PI : 2*M_PI) )
         ) {
      ATH_MSG_WARNING ("-U- track theta = "<<V(Trk::theta)<<" -> this is too far from defined range, stop update.");
      return false;
    }
    if (V(Trk::theta) > M_PI) {
      V(Trk::theta) = 2*M_PI - V(Trk::theta);
      V(Trk::phi)  += (V(Trk::phi)>0.0) ? -M_PI : M_PI;
    } else if (V(Trk::theta) < 0.0) {
      V(Trk::theta) = -V(Trk::theta);
      V(Trk::phi)  += (V(Trk::phi)>0.0) ? -M_PI : M_PI;
    }
    // correct also cov matrix:
    M(0,3) = -M(0,3); // cov(polar,locX)
    M(1,3) = -M(1,3); // cov(polar,locY)
    M(2,3) = -M(2,3); // cov(polar,azimuthal)
    M(3,4) = -M(3,4); // cov(polar,qOverP)
    ATH_MSG_DEBUG ("-U- now use corrected value phi= "<<V(Trk::phi)<<" theta = "<<V(Trk::theta));
  }


  // correct phi coordinate if necessary
  if ( (V(Trk::phi) > M_PI) ) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "-U- phi = " << V(Trk::phi);
    V(Trk::phi) = std::fmod(V(Trk::phi)+M_PI,2*M_PI)-M_PI;
    ATH_MSG_DEBUG( " out of range, now corrected to " << V(Trk::phi) );
  } else if( (V(Trk::phi)<-M_PI) ) {
    ATH_MSG_DEBUG( "-U- phi = " << V(Trk::phi));
    V(Trk::phi) = std::fmod(V(Trk::phi)-M_PI,2*M_PI)+M_PI;
    ATH_MSG_DEBUG( " out of range, now corrected to " << V(Trk::phi) );
  }
  
  return true;
}

void Trk::KalmanUpdatorSMatrix::logStart(const std::string& IDstring,
                                  const Trk::TrackParameters& tp) const
{
  ATH_MSG_DEBUG( "--> entered KalmanUpdatorSMatrix::" << IDstring );
  ATH_MSG_VERBOSE( "-U- TrkPar:" << std::setiosflags(std::ios::right)<<std::setprecision(4)
        << std::setw( 9)<<tp.parameters()[0]<< std::setw(10)<<tp.parameters()[1]<<std::setprecision(5)
        << std::setw(10)<<tp.parameters()[2]<< std::setw(10)<<tp.parameters()[3]<<std::setprecision(4)
        << std::setw(10)<<tp.parameters()[4]);
}

void Trk::KalmanUpdatorSMatrix::logInputCov(const SCovMatrix5& covTrk,
                                            const Amg::VectorX& parRio, const Amg::MatrixX& covRio) const
{
  ATH_MSG_VERBOSE( "-U- cov    "<<std::setiosflags(std::ios::right)<<std::setprecision(3)
        << std::setw(9)<<covTrk(0,0)<<" "<< std::setw(9)<<covTrk(0,1)<<" "
        << std::setw(9)<<covTrk(0,2)<<" "<< std::setw(9)<<covTrk(0,3)<<" "
        << std::setw(9)<<covTrk(0,4));
  ATH_MSG_VERBOSE( "                                      " << "          " << "           "
        << std::setw(9)<<covTrk(1,1)<<" "<< std::setw(9)<<covTrk(1,2)<<" "
        << std::setw(9)<<covTrk(1,3)<<" "<< std::setw(9)<<covTrk(1,4));
  ATH_MSG_VERBOSE( "  covariance matrix                   " << "          " << "                     "
        << std::setw(9)<<covTrk(2,2)<<" "<< std::setw(9)<<covTrk(2,3)<<" "
        << std::setw(9)<<covTrk(2,4));
  ATH_MSG_VERBOSE( "  of the PREDICTED track pars         " << "          " << "                               " 
        << std::setw(9)<<covTrk(3,3)<<" "<< std::setw(9)<<covTrk(3,4));
  ATH_MSG_VERBOSE("                                      " << "          " << "                                         " 
        << std::setw(9)<<covTrk(4,4)<<std::setprecision(6));

  int nLocCoord = covRio.cols();
  ATH_MSG_VERBOSE( "-U- measurement locPar: ");
  for (int i=0; i<nLocCoord; i++) ATH_MSG_VERBOSE(parRio[i]);
  msg(MSG::VERBOSE) << "-U- measurement (err)^2: " <<std::setprecision(4)<<covRio(0,0);
  for (int i=1; i<nLocCoord; i++) msg(MSG::VERBOSE) << ", "<<covRio(i,i);
  msg(MSG::VERBOSE) << std::setprecision(6)<<"\n";
}

void Trk::KalmanUpdatorSMatrix::logGainForm(int nc, const SParVector5& r,
                                     const SCovMatrix5& R,
                                     const SGenMatrix5& K) const
{
  // again some verbose debug output showing internals of updating
  msg(MSG::VERBOSE) << "-U- residual: r=("<<r(0);
  for (int i=1; i<nc; i++) msg(MSG::VERBOSE) <<","<<r(i);
  msg(MSG::VERBOSE) << ")\n";
  msg(MSG::VERBOSE) << "-U- inv. sigmaR=("<< R(0,0);
  for (int i=1; i<nc; i++) msg(MSG::VERBOSE) << "," << R(i,i);
  msg(MSG::VERBOSE) << ")\n";
  for (int i=0; i<nc; i++)
    msg(MSG::VERBOSE) // K is a row x col = 5 x nc matrix.
          << ( i==0 ? "-U- gain mtx  K=(" : "                (" )
          << std::setiosflags(std::ios::fixed | std::ios::showpoint | std::ios::right )
          << std::setw(7) << std::setprecision(4) << K(0,i)<<", " 
          << std::setw(7) << std::setprecision(4) << K(1,i)<<", "
          << std::setw(7) << std::setprecision(4) << K(2,i)<<", "
          << std::setw(7) << std::setprecision(4) << K(3,i)<<", "
          << std::setw(7) << std::setprecision(4) << K(4,i)<<")"
          << std::resetiosflags(std::ios::fixed) << "\n";
}

void Trk::KalmanUpdatorSMatrix::logResult(const std::string& methodName, 
                                   const AmgVector(5)& par, const AmgSymMatrix(5)& covPar) const
{
    // again some verbose debug output
    msg(MSG::VERBOSE) << "-U- ==> result for KalmanUpdatorSMatrix::"<<methodName<<endmsg;
    msg(MSG::VERBOSE) << "-U- new par"<<std::setiosflags(std::ios::right)<<std::setprecision(4)
          << std::setw( 9)<<par[0]<< std::setw(10)<<par[1]<<std::setprecision(5)
          << std::setw(10)<<par[2]<< std::setw(10)<<par[3]<<std::setprecision(4)
          << std::setw(10)<<par[4]                <<"\n";
    msg(MSG::VERBOSE) << "-U- new cov" <<std::setiosflags(std::ios::right)<<std::setprecision(3)
          << std::setw(9)<<(covPar)(0,0)<<" "<< std::setw(9)<<(covPar)(0,1)<<" " 
          << std::setw(9)<<(covPar)(0,2)<<" "<< std::setw(9)<<(covPar)(0,3)
          << " " << std::setw(9)<<(covPar)(0,4)<< "\n";
    msg(MSG::VERBOSE) << "                                      " << "           " << "          "
          << std::setw(9)<<(covPar)(1,1)<<" "<< std::setw(9)<<(covPar)(1,2)<<" "
          << std::setw(9)<<(covPar)(1,3)<<" "<< std::setw(9)<<(covPar)(1,4)<< "\n";
    msg(MSG::VERBOSE) << "  covariance matrix                   " << "           " << "                    "
          << std::setw(9)<<(covPar)(2,2) <<" "<< std::setw(9)<<(covPar)(2,3) <<" "
          << std::setw(9)<<(covPar)(2,4) << "\n";
    msg(MSG::VERBOSE) << "  of the UPDATED   track pars         " << "           " 
          << "                              " <<std::setw(9)<<(covPar)(3,3) << " "
          << std::setw(9)<<(covPar)(3,4) << "\n";
    msg(MSG::VERBOSE) << "                                      " << "           " 
          << "                                        " 
          << std::setw(9)<<(covPar)(4,4) <<std::setprecision(6)<< "\n";
}
