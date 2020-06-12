/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KalmanUpdator.cxx
//   Source file for class KalmanUpdator
//   The main updating calculations can be found in calculateFilterStep().
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Markus.Elsing@cern.ch
// Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
///////////////////////////////////////////////////////////////////

#include "TrkMeasurementUpdator/KalmanUpdator.h"

#include "TrkParameters/TrackParameters.h"

#include "EventPrimitives/EventPrimitivesToStringConverter.h"


// constructor
Trk::KalmanUpdator::KalmanUpdator(const std::string& t,const std::string& n,const IInterface* p) :
    AthAlgTool (t,n,p),
    m_cov0{250., 250.,0.25, 0.25, 0.000001}, // set defaults _before_ reading from job options
    m_projectionMatrices(5),
    m_outputlevel(1)
{
    // AlgTool stuff
  declareProperty("InitialCovariances",m_cov0,"default covariance to be used at start of filter");
  declareProperty("FastTrackStateCovCalculation",m_useFruehwirth8a=false,"toggles which formula to use for updated cov");
  declareInterface<IUpdator>( this );
}

// destructor
Trk::KalmanUpdator::~KalmanUpdator()
{}

// initialize
StatusCode Trk::KalmanUpdator::initialize()
{
    // pass individual outputlevel to message stream
  m_outputlevel = msg().level()-MSG::DEBUG;
  if (m_cov0.size() < 5) {
    ATH_MSG_INFO( "Wrong-sized initial covariance given, so set to default: "  );
    m_cov0 = {250.,250,0.25,0.25, 0.000001};
  }
  ATH_MSG_INFO( "Initial covariance: " << m_cov0[0] << ", "
                << m_cov0[1] << ", " << m_cov0[2] << ", "
                << m_cov0[3] << ", " << m_cov0[4] << " (diagonal)"  );
  ATH_MSG_DEBUG( "initialize() successful in " << name()  );
  return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::KalmanUpdator::finalize()
{
    ATH_MSG_DEBUG( "finalize() successful in " << name()  );
    return StatusCode::SUCCESS;
}

// updator #1 for Kalman Fitter - version with Amg::Vector2D (for example for PrepRawData)
Trk::TrackParameters* Trk::KalmanUpdator::addToState (const Trk::TrackParameters& trkPar,
                                                      const Amg::Vector2D&    measmtPos, 
                                                      const Amg::MatrixX&      measmtErr) const {
    if (m_outputlevel <= 0) logStart("addToState(TP,LPOS,ERR)",trkPar);
    FitQualityOnSurface*    fitQoS = nullptr;
    return calculateFilterStep (trkPar, measmtPos, measmtErr,1,fitQoS,false);
}

// updator #2 for Kalman Fitter - version with LocalParameters (for example for RIO_OnTrack)
Trk::TrackParameters* Trk::KalmanUpdator::addToState (const Trk::TrackParameters& trkPar,
                                                            const LocalParameters&  measmtPar,
                                                            const Amg::MatrixX&      measmtErr) const {
    if (m_outputlevel <= 0) logStart("addToState(TP,LPAR,ERR)",trkPar);
    FitQualityOnSurface*    fitQoS = nullptr;
    return calculateFilterStep (trkPar, measmtPar, measmtErr,1,fitQoS, false);
}
// updator #3 for Kalman Fitter - version with Amg::Vector2D (for example for PrepRawData)
Trk::TrackParameters* Trk::KalmanUpdator::addToState (const Trk::TrackParameters& trkPar,
                                                      const Amg::Vector2D&    measmtPos, 
                                                      const Amg::MatrixX&      measmtErr,
                                                      FitQualityOnSurface*&   fitQoS) const {
    if (m_outputlevel <= 0) logStart("addToState(TP,LPOS,ERR,FQ)",trkPar);
    if (fitQoS) {
      ATH_MSG_WARNING( "expect nil FitQuality pointer, refuse operation to avoid mem leak!"  );
      return nullptr;
    } 
      return calculateFilterStep (trkPar, measmtPos, measmtErr, 1, fitQoS, true);
    
}

// updator #4 for Kalman Fitter - version with LocalParameters (for example for RIO_OnTrack)
Trk::TrackParameters* Trk::KalmanUpdator::addToState (const Trk::TrackParameters& trkPar,
                                                      const LocalParameters&  measmtPar,
                                                      const Amg::MatrixX&      measmtErr,
                                                      FitQualityOnSurface*&   fitQoS) const {
    if (m_outputlevel <= 0) logStart("addToState(TP,LPAR,ERR,FQ)",trkPar);
    if (fitQoS) {
      ATH_MSG_WARNING( "expect nil FitQuality pointer, refuse operation to avoid mem leak!"  );
      return nullptr;
    } 
      return calculateFilterStep (trkPar, measmtPar, measmtErr, 1, fitQoS, true);
    
}

// inverse updator #1 for Kalman Fitter - version with Amg::Vector2D (for example for PrepRawData)
Trk::TrackParameters* Trk::KalmanUpdator::removeFromState (const Trk::TrackParameters& trkPar,
                                                           const Amg::Vector2D&    measmtPos, 
                                                           const Amg::MatrixX&      measmtErr) const {
    if (m_outputlevel<=0) logStart("removeFromState(TP,LPOS,ERR)",trkPar);
    FitQualityOnSurface*    fitQoS = nullptr;
    return calculateFilterStep (trkPar, measmtPos, measmtErr,-1,fitQoS, false);
}

// inverse updator #2 for Kalman Fitter - version with LocalParameters (for example for RIO_OnTrack)
Trk::TrackParameters* Trk::KalmanUpdator::removeFromState (const Trk::TrackParameters& trkPar,
                                                           const LocalParameters&  measmtPar,
                                                           const Amg::MatrixX&      measmtErr) const {
    if (m_outputlevel) logStart("removeFromState(TP,LPAR,ERR)",trkPar);
    FitQualityOnSurface*    fitQoS = nullptr;
    return calculateFilterStep (trkPar, measmtPar, measmtErr,-1,fitQoS, false);
}

// inverse updator #3 for Kalman Fitter - version with Amg::Vector2D (for example for PrepRawData)
Trk::TrackParameters* Trk::KalmanUpdator::removeFromState (const Trk::TrackParameters& trkPar,
                                                           const Amg::Vector2D&    measmtPos, 
                                                           const Amg::MatrixX&      measmtErr,
                                                           FitQualityOnSurface*&    fitQoS) const {
    if (m_outputlevel<=0) logStart("removeFromState(TP,LPOS,ERR,FQ)",trkPar);
    if (fitQoS) {
      ATH_MSG_WARNING( "expect nil FitQuality pointer, refuse operation to"
                       << " avoid mem leak!"  );
      return nullptr;
    } 
      return calculateFilterStep (trkPar, measmtPos, measmtErr, -1, fitQoS, true);
    
}

// inverse updator #4 for Kalman Fitter - version with LocalParameters (for example for RIO_OnTrack)
Trk::TrackParameters* Trk::KalmanUpdator::removeFromState (const Trk::TrackParameters& trkPar,
                                                           const LocalParameters&  measmtPar,
                                                           const Amg::MatrixX&      measmtErr,
                                                           FitQualityOnSurface*&    fitQoS) const {
    if (m_outputlevel<=0) logStart("removeFromState(TP,LPAR,ERR,FQ)",trkPar);
    if (fitQoS) {
      ATH_MSG_WARNING( "expect nil FitQuality pointer, refuse operation to"
                       << " avoid mem leak!"  );
        return nullptr;
    } 
        return calculateFilterStep (trkPar, measmtPar, measmtErr, -1, fitQoS, true);
    
}

// state-to-state updator, trajectory combination - version without fitQuality
Trk::TrackParameters* Trk::KalmanUpdator::combineStates (const Trk::TrackParameters& one,
                                                         const Trk::TrackParameters& two) const {
	// remember, either one OR two might have no error, but not both !
  if (!one.covariance() && !two.covariance()) {
    ATH_MSG_WARNING( "both parameters have no errors, invalid "
                     << "use of Updator::combineStates()"  );
    return nullptr;
  }
  // if only one of two has an error, return that one
  if (!one.covariance()) {
    if (m_outputlevel<=0) logResult("combineStates(TP,TP)",two.parameters(),
				    *two.covariance());
    return two.clone();
  }
  if (!two.covariance()) {
    if (m_outputlevel<=0) logResult("combineStates(TP,TP)",one.parameters(),
				    *one.covariance());
    return one.clone();
  }
	
  // ... FIXME - TRT is so difficult, need to check that both parameters are in the same frame
  //			   otherwise go into frame of one !
	
  // ok, normal, let's combine using gain matrix formalism
  const AmgSymMatrix(5)& covTrkOne = (*one.covariance());
  const AmgSymMatrix(5)& covTrkTwo = (*two.covariance());
	
  AmgSymMatrix(5)  sumCov = covTrkOne + covTrkTwo;
  AmgSymMatrix(5)  K      = covTrkOne * sumCov.inverse();
	
  Amg::VectorX         r   = two.parameters() - one.parameters();
  // catch [-pi,pi] phi boundary problems
  if (!diffThetaPhiWithinRange(r)) correctThetaPhiRange(r,sumCov,true);
  Amg::VectorX		  par = one.parameters() + K * r;
  AmgSymMatrix(5)* covPar = new AmgSymMatrix(5)( K * covTrkTwo );
  if ( (!thetaPhiWithinRange(par)) ? !correctThetaPhiRange(par,*covPar,false) : false ) {
    delete covPar;
    ATH_MSG_WARNING( "combineStates(TP,TP): could not combine angular values."  );
    return nullptr;
  }
	
  // return cloned version of Track Parameters (MeasuredPerigee, MeasuredAtA...)
  TrackParameters* comb = one.associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],
                                                                        par[Trk::phi],par[Trk::theta],par[Trk::qOverP],covPar); 
  if (m_outputlevel<=0) logResult("combineStates(TP,TP)", par, *covPar);
  return comb;
}

// state-to-state updator, trajectory combination - version with fitQuality
Trk::TrackParameters* Trk::KalmanUpdator::combineStates (const Trk::TrackParameters& one,
                                                         const Trk::TrackParameters& two,
                                                         FitQualityOnSurface*& fitQoS) const {
    // try if both Track Parameters are measured ones ?
  // remember, either one OR two might have no error, but not both !
  if (!one.covariance() && !two.covariance()) {
    ATH_MSG_WARNING( "both parameters have no errors, invalid use of Updator::combineStates()"  );
    return nullptr;
  }
  if (fitQoS) {
    ATH_MSG_WARNING( "expect nil FitQuality pointer, refuse operation to avoid mem leak!"  );
    return nullptr;
  } 
    // if only one of two has an error, return that one
    if (!one.covariance()) {
      fitQoS =  new FitQualityOnSurface(0.f, 5);
      if (m_outputlevel<=0) logResult("combineStates(TP,TP,FQ)", one.parameters(),
				      (*two.covariance()));
      return two.clone();
    }
    if (!two.covariance()) {
      fitQoS =  new FitQualityOnSurface(0.f, 5);
      if (m_outputlevel<=0) logResult("combineStates(TP,TP,FQ)", one.parameters(),
				      (*one.covariance()));
      return one.clone();
    }

    // covariance matrix for prediction and the state to be added
    const AmgSymMatrix(5)& covTrkOne = (*one.covariance());
    const AmgSymMatrix(5)& covTrkTwo = (*two.covariance());

    // chi2 calculation and Kalman Gain preparation
    Amg::VectorX    r = two.parameters() - one.parameters();
    AmgSymMatrix(5) R = covTrkOne + covTrkTwo;
    // catch [-pi,pi] phi boundary problems to keep chi2 under control
    if (!diffThetaPhiWithinRange(r)) correctThetaPhiRange(r,R,true);
    AmgSymMatrix(5) R_inv = R.inverse();
    AmgSymMatrix(5) K   = covTrkOne * R_inv;
    Amg::VectorX	  par = one.parameters() + K * r;
    AmgSymMatrix(5)* covPar = new AmgSymMatrix(5)(K * covTrkTwo);
    if ( (!thetaPhiWithinRange(par)) ? !correctThetaPhiRange(par,*covPar) : false ) {
      ATH_MSG_WARNING( "combineStates(TP,TP,FQ): could not combine angular values."  );
      delete covPar; return nullptr;
    }

    // compute fit quality
    double  chiSquared = r.transpose()*R_inv*r;
    fitQoS =  new FitQualityOnSurface(chiSquared, 5);
	
    // return cloned version of Track Parameters (MeasuredPerigee, MeasuredAtA...)
    TrackParameters* comb = one.associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],
                                                                          par[Trk::phi],par[Trk::theta],par[Trk::qOverP],covPar); 
    if (m_outputlevel<=0) logResult("combineStates(TP,TP,FQ)", par, *covPar);
    return comb;
  
}


// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdator::fullStateFitQuality (const Trk::TrackParameters& trkPar,
                                         const Amg::Vector2D& locPos,
                                         const Amg::MatrixX& rioErr) const {
    ATH_MSG_DEBUG( "--> entered KalmanUpdator::fullStateFitQuality(TP,LPOS,ERR)"  );
	
    // try if Track Parameters are measured ones ?
    if (!trkPar.covariance()) {
      ATH_MSG_ERROR( "updated smoother/trajectory has no error matrix"  );
      return nullptr;
    }
    // covariance matrix for prediction
    const AmgSymMatrix(5)& covTrk = (*trkPar.covariance());

    // For the Amg::Vector2D version, need to know # meas. coord. from covariance matrix.
    int nLocCoord = rioErr.cols();
    Amg::VectorX rioPar(nLocCoord);
    for (int iLocCoord=0; iLocCoord < nLocCoord; iLocCoord++) {
      rioPar[iLocCoord] = locPos[iLocCoord];
    }
    
    // measurement Matrix ( n x m )
    Amg::MatrixX H(rioErr.cols(),covTrk.cols());
    H.setZero();
    for (int i=0; i < nLocCoord; i++) H(i,i)=1.f;
    
    // residuals
    Amg::VectorX    r = rioPar - H * trkPar.parameters();
    
    // all the rest is outsourced to a common chi2 routine
    return makeChi2Object(r,covTrk,rioErr,-1,(nLocCoord==1?1:3));
}


// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdator::fullStateFitQuality (const Trk::TrackParameters& trkPar,
                                         const Trk::LocalParameters& rioPar,
                                         const Amg::MatrixX&     rioErr) const {
  ATH_MSG_VERBOSE( "--> entered KalmanUpdator::fullStateFitQuality(TP,LPAR,ERR)"  );
	
  // try if Track Parameters are measured ones ?
  if (!trkPar.covariance()) {
    ATH_MSG_ERROR( "updated smoother/trajectory has no error matrix"  );
    return nullptr;
  }
  if ( !consistentParamDimensions(rioPar,rioErr.cols()) ) return nullptr;

  // covariance matrix for prediction
  const AmgSymMatrix(5)& covTrk = (*trkPar.covariance());

  // State to measurement dimensional reduction Matrix ( n x m )
  const Amg::MatrixX& H(rioPar.expansionMatrix());
	
  // residuals
  Amg::VectorX    r = rioPar;
  if( rioPar.parameterKey()==31 ) r -= trkPar.parameters();
  else                            r -= H*trkPar.parameters();

  // all the rest is outsourced to a common chi2 routine
  return makeChi2Object(r,covTrk,rioErr,-1,rioPar.parameterKey());
}

// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdator::predictedStateFitQuality (const Trk::TrackParameters& predPar,
                                              const Amg::Vector2D& rioLocPos,
                                              const Amg::MatrixX& rioLocErr) const {
  ATH_MSG_VERBOSE( "--> entered KalmanUpdator::predictedStateFitQuality(TP,LPOS,ERR)"  );
  // try if Track Parameters are measured ones ?
  if (predPar.covariance() == nullptr) {
#if 0
    if (&predPar == nullptr) 
      ATH_MSG_WARNING( "input state is NULL in predictedStateFitQuality()"  );
    else
#endif
      ATH_MSG_WARNING( "input state has no error matrix in predictedStateFitQuality()"  );
    return nullptr;
  }
  // covariance matrix for prediction
  const AmgSymMatrix(5)& covPred = (*predPar.covariance());

  // For the Amg::Vector2D version, need to know # meas. coord. from covariance matrix.
  int nLocCoord = rioLocErr.cols();
  Amg::VectorX rioPar(nLocCoord);
  for (int iLocCoord=0; iLocCoord < nLocCoord; iLocCoord++) {
    rioPar[iLocCoord] = rioLocPos[iLocCoord];
  }

  // measurement Matrix ( n x m )
  Amg::MatrixX H(rioLocErr.cols(),covPred.cols());
  H.setZero();
  for (int i=0; i < nLocCoord; i++) H(i,i)=1.f;
	
  // residuals
  Amg::VectorX    r = rioPar - H * predPar.parameters();
		
  // all the rest is outsourced to a common chi2 routine
  return makeChi2Object(r,covPred,rioLocErr,+1,(nLocCoord==1?1:3));
}

// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdator::predictedStateFitQuality (const Trk::TrackParameters& predPar,
                                              const Trk::LocalParameters& rioPar,
                                              const Amg::MatrixX&     rioErr) const {
  ATH_MSG_VERBOSE( "--> entered KalmanUpdator::predictedStateFitQuality(TP,LPAR,ERR)"  );

  // try if Track Parameters are measured ones ?
  if (predPar.covariance() == nullptr) {
#if 0
    if (&predPar == nullptr) 
      ATH_MSG_WARNING( "input state is NULL in predictedStateFitQuality()"  );
    else
#endif
      ATH_MSG_WARNING( "input state has no error matrix in predictedStateFitQuality()"  );
    return nullptr;
  }

  if ( ! consistentParamDimensions(rioPar,rioErr.cols()) ) return nullptr;

  // covariance matrix for prediction
  const AmgSymMatrix(5)& covPred = (*predPar.covariance());

  // State to measurement dimensional reduction Matrix ( n x m )
  const Amg::MatrixX& H(rioPar.expansionMatrix());
	
  // residuals
  Amg::VectorX    r = rioPar;
  if(rioPar.parameterKey()==31) r -= predPar.parameters();
  else                          r -= H * predPar.parameters();
  
  // all the rest is outsourced to a common chi2 routine
  return makeChi2Object(r,covPred,rioErr,+1,rioPar.parameterKey());
}

// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdator::predictedStateFitQuality (const Trk::TrackParameters& one,
                                              const Trk::TrackParameters& two) const {
  ATH_MSG_VERBOSE( "--> entered KalmanUpdator::predictedStateFitQuality(TP,TP)"  );
  // remember, either one OR two might have no error, but not both !
  if (!one.covariance() && !two.covariance()) {
    ATH_MSG_WARNING( "both parameters have no errors, invalid "
                     << "use of Updator::fitQuality()"  );
    return nullptr;
  }
  // if only one of two has an error, place a message.
  if (!one.covariance() || !two.covariance()) {
    ATH_MSG_DEBUG( "One parameter does not have uncertainties, "
                   << "assume initial state and return chi2=0.0"  );
    return new FitQualityOnSurface(0.f, 5);
  }
    
  // covariance matrix for prediction and the state to be added
  const AmgSymMatrix(5)& covTrkOne = (*one.covariance());
  const AmgSymMatrix(5)& covTrkTwo = (*two.covariance());
  // residuals
  Amg::VectorX r = two.parameters() - one.parameters();
  AmgSymMatrix(5) R = (covTrkOne + covTrkTwo).inverse();
  // chi2 calculation
  double  chiSquared = r.transpose()*R*r;
  return new FitQualityOnSurface(chiSquared, 5);
}

std::vector<double> Trk::KalmanUpdator::initialErrors() const {
  std::vector<double> E(5);
  for (int i=0; i<5; ++i) E[i] = std::sqrt(m_cov0[i]);
  return E;
}

// mathematics for Kalman updator and inverse Kalman filter
Trk::TrackParameters* Trk::KalmanUpdator::calculateFilterStep (const Trk::TrackParameters&  trkPar,
                                                               const Amg::Vector2D&    locPos,
                                                               const Amg::MatrixX& covRio,
                                                               const int sign,
                                                               Trk::FitQualityOnSurface*& fitQoS,
                                                               bool createFQoS) const {

    // try if Track Parameters are measured ones ?
  AmgSymMatrix(5) covTrk;
  if (!trkPar.covariance()) {
    if (sign<0) {
      ATH_MSG_WARNING( "MeasuredTrackParameters == Null, can not calculate updated parameter state"  );
      return nullptr;
    } 
      // no error given - use a huge error matrix for the time
      // covTrk = Amg::MatrixX(5, 1) * 1000.f;
      ATH_MSG_VERBOSE( "-U- no covTrk at input -  assign large error matrix for the time being."  );
      covTrk(0,0) = m_cov0[0];
      covTrk(1,1) = m_cov0[1];
      covTrk(2,2) = m_cov0[2];
      covTrk(3,3) = m_cov0[3];
      covTrk(4,4) = m_cov0[4];
    
  }else {
    covTrk = (*trkPar.covariance());
  }


  // covariance matrix and parameters of track
  Amg::VectorX parTrk = trkPar.parameters();
  if (!thetaPhiWithinRange(parTrk)) {
    ATH_MSG_WARNING( (sign>0?"addToState(TP,LPOS,ERR..)":"removeFromState(TP,LPOS,ERR..)")
                     << ": undefined phi,theta range in input parameters."  );
    return nullptr;
  }
  
  // measurement vector of RIO_OnTrack - needs more care for # local par?
  int nLocCoord = covRio.cols();
  if ( (nLocCoord < 1) || (nLocCoord > 2 ) ) {
    ATH_MSG_WARNING( " number of local coordinates must be 1 or 2, but it is "
                     << nLocCoord  );
  }
  Amg::VectorX rioPar(nLocCoord);
  for (int iLocCoord=0; iLocCoord < nLocCoord; iLocCoord++) {
    rioPar[iLocCoord] = locPos[iLocCoord];
  }
  if (m_outputlevel<0) logInputCov(covTrk,rioPar,covRio);

  // measurement Matrix ( n x m )
  Amg::MatrixX H(covRio.cols(),covTrk.cols());
  H.setZero();
  for (int i=0; i < nLocCoord; i++) H(i,i)=1.f;
    
  // residual from reconstructed hit wrt. predicted state
  Amg::VectorX r = rioPar - H * parTrk;
    
  // compute covariance on of residual R = +/- covRIO + H * covTrk * H.T()
  Amg::MatrixX R = (sign * covRio) + projection(covTrk,(nLocCoord==1 ? 1 : 3) ); // .similarity(H);
  // compute Kalman gain matrix
  Amg::MatrixX K   = covTrk * H.transpose() * R.inverse();
  AmgSymMatrix(5) I;  // 5x5 unit matrix
  I.setIdentity();
  AmgSymMatrix(5) M = I - K * H;
  if (m_outputlevel<0) {logGainForm (nLocCoord,r,R.inverse(),K,M);}

  // compute local filtered state
  Amg::VectorX par = parTrk + K * r;

  // compute covariance matrix of local filteres state:
  AmgSymMatrix(5)* covPar;
  if (m_useFruehwirth8a) {
    // one can use as well: covPar = M * covTrk; see A.Gelb why.
    covPar = new AmgSymMatrix(5)(M*covTrk);
  } else {
    //   C = M*covTrk*M.T() +/- K*covRio*K.T(), supposedly more robust Gelb form.
    // bad_alloc:  covPar = new AmgSymMatrix(5)(covTrk.similarity(M) + (sign * covRio.similarity(K)));
    covPar = new AmgSymMatrix(5)(M*covTrk*M.transpose() + sign*K*covRio*K.transpose());
  }
  if ( (!thetaPhiWithinRange(par)) ? !correctThetaPhiRange(par,*covPar) : false ) {
    ATH_MSG_WARNING( "calculateFS(TP,LPOS,ERR): bad angles in filtered state!"  );
    delete covPar;
    return nullptr;
  }
  if (m_outputlevel<=0) logResult(createFQoS?
				  (sign>0?"addToState(TP,LPOS,ERR,FQ)":"removeFromState(TP,LPOS,ERR,FQ)"):
				  (sign>0?"addToState(TP,LPOS,ERR)":"removeFromState(TP,LPOS,ERR)"),
				  par,*covPar);
  
  // if a pointer is given, compute chi2
  if (createFQoS) {
    if (sign<0) {
      // when removing, the input are updated par
      fitQoS = makeChi2Object(r,covTrk,covRio,-1,(nLocCoord==1?1:3));
    } else {
      // when adding chi2 is made from the updated par
      const Amg::VectorX r_upd = rioPar - H * par;
      fitQoS = makeChi2Object(r_upd,*covPar,covRio,-1,(nLocCoord==1?1:3));
    }
  }
  // return cloned version of Track Parameters (MeasuredPerigee, MeasuredAtA...)
  TrackParameters* updated = trkPar.associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],
											  par[Trk::phi],par[Trk::theta],par[Trk::qOverP],covPar); 
  return updated;
}


// mathematics for Kalman updator and inverse Kalman filter
Trk::TrackParameters* Trk::KalmanUpdator::calculateFilterStep (const Trk::TrackParameters&  trkPar,
                                                               const Trk::LocalParameters&  rioPar,
                                                               const Amg::MatrixX& covRio,
                                                               const int sign,
                                                               Trk::FitQualityOnSurface*& fitQoS,
                                                               bool createFQoS ) const {

  // try if Track Parameters are measured ones ?
  AmgSymMatrix(5) covTrk;
  if (!trkPar.covariance()) {
    if (sign<0) {
      ATH_MSG_WARNING( "MeasuredTrackParameters == Null, can not calculate "
                       << "updated parameter state."  );
      return nullptr;
    } 
      // no error given - use a huge error matrix for the time
      // covTrk = Amg::MatrixX(5, 1) * 1000.f;
      ATH_MSG_VERBOSE( "-U- no covTrk at input - "
                       << "assign large error matrix for the time being."  );
      covTrk(0,0) = m_cov0[0];
      covTrk(1,1) = m_cov0[1];
      covTrk(2,2) = m_cov0[2];
      covTrk(3,3) = m_cov0[3];
      covTrk(4,4) = m_cov0[4];
    
  } else {
    covTrk = (*trkPar.covariance());
  }

  //    LocalParameters  parTrk = trkPar.parameters();
  Amg::VectorX parTrk = trkPar.parameters();
  if (!thetaPhiWithinRange(parTrk)) {
    ATH_MSG_WARNING( (sign>0?"addToState(TP,LPAR,ERR..)":"removeFromState(TP,LPAR,ERR..)")
                     << ": undefined phi,theta range in input parameters."  );
    return nullptr;
  }
  if (!thetaPhiWithinRange(rioPar,rioPar.parameterKey())) {
    ATH_MSG_WARNING( (sign>0?"addToState(TP,LPAR,ERR..)":"removeFromState(TP,LPAR,ERR..)")
                     << ": undefined phi,theta range in input measurement !!"  );
    return nullptr;
  }

  // measurement vector of RIO_OnTrack - needs more care for # local par?
  int nLocCoord = covRio.cols();
  if ( ! consistentParamDimensions(rioPar,covRio.cols()) ) return nullptr;
  if (m_outputlevel<0) logInputCov(covTrk,rioPar,covRio);

  // State to measurement dimensional reduction Matrix ( n x m )
  Amg::MatrixX H(rioPar.expansionMatrix());

  // residual from reconstructed hit wrt. predicted state
  Amg::VectorX r = rioPar - H * parTrk;
  // compute covariance of residual R = +/- covRIO + H * covTrk * H.T()
  Amg::MatrixX R = (sign * covRio) + projection(covTrk,rioPar.parameterKey()); // .similarity(H);
  // catch [-pi,pi] phi boundary problems to keep chi2 under control
  if (!diffThetaPhiWithinRange(r,rioPar.parameterKey()) )
    correctThetaPhiRange(r,R,true,rioPar.parameterKey());
  
  // compute Kalman gain matrix
  Amg::MatrixX K   = covTrk * H.transpose() * R.inverse();
  AmgSymMatrix(5) I;  // 5x5 unit matrix
  I.setIdentity();
  AmgSymMatrix(5) M  = I - K * H;
  if (m_outputlevel<0) {logGainForm (nLocCoord,r,R.inverse(),K,M);}
  // compute local filtered state
  Amg::VectorX par = parTrk + K * r;
        
  // compute covariance matrix of local filteres state
  AmgSymMatrix(5)* covPar;
  if (m_useFruehwirth8a) {
    // one can use as well: covPar = M * covTrk; see A.Gelb why.
    covPar = new AmgSymMatrix(5)(M*covTrk);
  } else {
    covPar = new AmgSymMatrix(5)(M*covTrk*M.transpose() + sign*K*covRio*K.transpose());
  }
  if ( (!thetaPhiWithinRange(par)) ? !correctThetaPhiRange(par,*covPar) : false ) {
    ATH_MSG_WARNING( "calculateFS(TP,LPAR,ERR): bad angles in filtered state!"  );
    delete covPar;
    return nullptr;
  }
  if (m_outputlevel<=0) logResult(createFQoS?
				  (sign>0?"addToState(TP,LPAR,ERR,FQ)":"removeFromState(TP,LPAR,ERR,FQ)"):
				  (sign>0?"addToState(TP,LPAR,ERR)":"removeFromState(TP,LPAR,ERR)"),
				  par,*covPar);

  // if a pointer is given, compute chi2
  if (createFQoS) {
    if (sign<0) {
      // when removing, the input are updated par
      fitQoS = makeChi2Object(r,covTrk,covRio,-1,rioPar.parameterKey());
    } else {
      // when adding chi2 is made from the updated par
      Amg::VectorX r_upd = rioPar - H * par;
      fitQoS = makeChi2Object(r_upd,*covPar,covRio,-1,rioPar.parameterKey());
    }
  }
  // return cloned version of Track Parameters (MeasuredPerigee, MeasuredAtA...)
  TrackParameters* updated = trkPar.associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],
                                                                              par[Trk::phi],par[Trk::theta],par[Trk::qOverP],covPar); 
  return updated;
}

Amg::MatrixX Trk::KalmanUpdator::projection(const Amg::MatrixX& M, const int key) const
{
  //  ATH_MSG_DEBUG( "projection("<<M[0][0]<<", "<<key<<")" );
  if (key == 31) return M;
  // reduction matrix
  const Amg::MatrixX& redMatrix = m_projectionMatrices.reductionMatrix(key);
  Amg::MatrixX R = redMatrix.transpose()*M*redMatrix;
  return R;
  
}



bool Trk::KalmanUpdator::consistentParamDimensions(const Trk::LocalParameters& P,
                                                         const int& dimCov) const {
  if (P.dimension() != dimCov ) {
    ATH_MSG_WARNING( "Inconsistency in dimension of local coord - problem with LocalParameters object?"  );
    ATH_MSG_WARNING( "dim of local parameters: "<< P.dimension()<< " vs. dim of error matrix: "<<dimCov  );
    ATH_MSG_INFO( "==> refuse update or chi2 calculation"  );
    return false;
  }
  if ( (dimCov < 1) || (dimCov > 5 ) ) {
    ATH_MSG_WARNING( "invalid dimension for local coordinates: " << dimCov  );
    return false;
  }
  return true;
}


bool Trk::KalmanUpdator::correctThetaPhiRange(Amg::VectorX& V, AmgSymMatrix(5)& C,
					      const bool isDifference, const int key) const
{
  // get phi and theta coordinate
  int jphi = -1;
  int jtheta = -1;
  double thetaMin = (isDifference ? -M_PI : 0);
  if (key == 31) jphi = Trk::phi;
  else if (key & 4) { // phi is within localParameter and a measured coordinate
    for (int itag = 0, ipos=1 ; itag<Trk::phi; ++itag, ipos*=2) if (key & ipos) ++jphi;
  }
  if (key == 31) jtheta = Trk::theta;
  else if (key & 8) { // theta is within localParameter and a measured coordinate
    for (int itag = 0, ipos=1 ; itag<Trk::theta; ++itag, ipos*=2) if (key & ipos) ++jtheta;
  }
  
  // correct theta and phi coordinate
  if ((jtheta>=0) && (V[jtheta]<thetaMin || V[jtheta]> M_PI) ) {
    if (m_outputlevel <= 0 ) {
      if (key !=31) ATH_MSG_WARNING( "-U- key: "<<key << " jphi: "<<jphi << " jtheta: "<<jtheta  );
      msg() << MSG::WARNING << "-U- " << (isDifference?"diff. ":" ") << "angles out of range,   phi = ";
      if (jphi>=0) msg() << V[jphi]; else msg() <<"free";
      msg() << " theta =  " << V[jtheta] <<endmsg;
    }
    // absolute theta: repair if between -pi and +2pi.
    // differential theta: repair if between -pi and +pi
    if ( ( V(jtheta) < -M_PI ) ||
         ( V(jtheta) > (isDifference? M_PI : 2*M_PI) )
         ) {
      ATH_MSG_WARNING( "-U- track theta too far from defined range, stop update."  );
      msg() << MSG::WARNING << "-U- " << (isDifference?"diff. ":" ") << "angles out of range,   phi = ";
      if (jphi>=0) msg() << V[jphi]; else msg() <<"free";
      msg() << " theta =  " << V[jtheta] <<endmsg;
      return false;
    }
    if (V[jtheta] > M_PI) {
      V[jtheta] = 2*M_PI - V[jtheta];
      if (jphi>=0) V[jphi]   += (V[jphi]>0.0) ? -M_PI : M_PI;
    }
    if (V[jtheta] < 0.0) {
      V[jtheta] = -V[jtheta];
      if (jphi>=0) V[jphi]   += (V[jphi]>0.0) ? -M_PI : M_PI;
    }
    if (jtheta==0 && key==24) C(jtheta,1) = -C(jtheta,1);
    if (jtheta==1)  {
      C(0,jtheta) = -C(0,jtheta);
      if (key>15) C(2,1) = -C(2,1);
    }
    if (jtheta==2)  {
      C(0,jtheta) = -C(0,jtheta);
      C(1,jtheta) = -C(1,jtheta);
      if (key>15) C(3,jtheta) = -C(3,jtheta);
    }
    if (jtheta==3)  {
      C(0,jtheta) = -C(0,jtheta);
      C(1,jtheta) = -C(1,jtheta);
      C(2,jtheta) = -C(2,jtheta);
      if (key>15) C(4,jtheta) = -C(4,jtheta);
    }
    
    if (m_outputlevel <=0) {
      msg() << MSG::DEBUG << "-U- now use corrected " << (isDifference?"diff. ":" ") << "value phi= ";
      if (jphi>=0) msg() << V[jphi]; else msg() <<"free";
      msg() << " theta =  " << V[jtheta] <<endmsg;
    }
  }

  // correct phi coordinate if necessary
  if ((jphi>=0) && (V[jphi] > M_PI) ) {
    ATH_MSG_DEBUG("-U- phi= " << V[jphi]);
    V[jphi] = std::fmod(V[jphi]+M_PI,2*M_PI)-M_PI;
    ATH_MSG_DEBUG( " out of range, now corrected to " << V[jphi]);
  } else if ((jphi>=0) && (V[jphi] < -M_PI) ) {
    ATH_MSG_DEBUG( "-U- phi= " << V[jphi]);
    V[jphi] = std::fmod(V[jphi]-M_PI,2*M_PI)+M_PI;
    ATH_MSG_DEBUG( " out of range, now corrected to " << V[jphi] );
  }
  return true;
}


//// hack code duplication
bool Trk::KalmanUpdator::correctThetaPhiRange(Amg::VectorX& V, Amg::MatrixX& C,
					      const bool isDifference, const int key) const
{
  // get phi and theta coordinate
  int jphi = -1;
  int jtheta = -1;
  double thetaMin = (isDifference ? -M_PI : 0);
  if (key == 31) jphi = Trk::phi;
  else if (key & 4) { // phi is within localParameter and a measured coordinate
    for (int itag = 0, ipos=1 ; itag<Trk::phi; ++itag, ipos*=2) if (key & ipos) ++jphi;
  }
  if (key == 31) jtheta = Trk::theta;
  else if (key & 8) { // theta is within localParameter and a measured coordinate
    for (int itag = 0, ipos=1 ; itag<Trk::theta; ++itag, ipos*=2) if (key & ipos) ++jtheta;
  }
  
  // correct theta and phi coordinate
  if ((jtheta>=0) && (V[jtheta]<thetaMin || V[jtheta]> M_PI) ) {
    if (m_outputlevel <= 0 ) {
      if (key !=31) ATH_MSG_WARNING( "-U- key: "<<key << " jphi: "<<jphi << " jtheta: "<<jtheta  );
      msg() << MSG::WARNING << "-U- " << (isDifference?"diff. ":" ") << "angles out of range,   phi = ";
      if (jphi>=0) msg() << V[jphi]; else msg() <<"free";
      msg() << " theta =  " << V[jtheta] <<endmsg;
    }
    // absolute theta: repair if between -pi and +2pi.
    // differential theta: repair if between -pi and +pi
    if ( ( V(jtheta) < -M_PI ) ||
         ( V(jtheta) > (isDifference? M_PI : 2*M_PI) )
         ) {
      ATH_MSG_WARNING( "-U- track theta too far from defined range, stop update."  );
      msg() << MSG::WARNING << "-U- " << (isDifference?"diff. ":" ") << "angles out of range,   phi = ";
      if (jphi>=0) msg() << V[jphi]; else msg() <<"free";
      msg() << " theta =  " << V[jtheta] <<endmsg;
      return false;
    }
    if (V[jtheta] > M_PI) {
      V[jtheta] = 2*M_PI - V[jtheta];
      if (jphi>=0) V[jphi]   += (V[jphi]>0.0) ? -M_PI : M_PI;
    }
    if (V[jtheta] < 0.0) {
      V[jtheta] = -V[jtheta];
      if (jphi>=0) V[jphi]   += (V[jphi]>0.0) ? -M_PI : M_PI;
    }
    if (jtheta==0 && key==24) C(jtheta,1) = -C(jtheta,1);
    if (jtheta==1)  {
      C(0,jtheta) = -C(0,jtheta);
      if (key>15) C(2,1) = -C(2,1);
    }
    if (jtheta==2)  {
      C(0,jtheta) = -C(0,jtheta);
      C(1,jtheta) = -C(1,jtheta);
      if (key>15) C(3,jtheta) = -C(3,jtheta);
    }
    if (jtheta==3)  {
      C(0,jtheta) = -C(0,jtheta);
      C(1,jtheta) = -C(1,jtheta);
      C(2,jtheta) = -C(2,jtheta);
      if (key>15) C(4,jtheta) = -C(4,jtheta);
    }
    
    if (m_outputlevel <=0) {
      msg() << MSG::DEBUG << "-U- now use corrected " << (isDifference?"diff. ":" ") << "value phi= ";
      if (jphi>=0) msg() << V[jphi]; else msg() <<"free";
      msg() << " theta =  " << V[jtheta] <<endmsg;
    }
  }

  // correct phi coordinate if necessary
  if ((jphi>=0) && (V[jphi] > M_PI) ) {
    if (m_outputlevel <=0) msg() << MSG::DEBUG << "-U- phi= " << V[jphi];
    V[jphi] = std::fmod(V[jphi]+M_PI,2*M_PI)-M_PI;
    if (m_outputlevel <=0) ATH_MSG_DEBUG( " out of range, now "
                                          << "corrected to " << V[jphi]  );
  } else if ((jphi>=0) && (V[jphi] < -M_PI) ) {
    if (m_outputlevel <=0) msg() << MSG::DEBUG << "-U- phi= " << V[jphi];
    V[jphi] = std::fmod(V[jphi]-M_PI,2*M_PI)+M_PI;
    if (m_outputlevel <=0) ATH_MSG_DEBUG( " out of range, now "
                                          << "corrected to " << V[jphi]  );
  }
  return true;
}

void Trk::KalmanUpdator::logStart(const std::string& IDstring,
                                  const Trk::TrackParameters& tp) const
{
  ATH_MSG_DEBUG( "--> entered KalmanUpdator::" << IDstring  );
  ATH_MSG_VERBOSE( "-U- TrkPar:" << std::setiosflags(std::ios::right)<<std::setprecision(4)
                   << std::setw( 9)<<tp.parameters()[0]<< std::setw(10)<<tp.parameters()[1]<<std::setprecision(5)
                   << std::setw(10)<<tp.parameters()[2]<< std::setw(10)<<tp.parameters()[3]<<std::setprecision(4)
                   << std::setw(10)<<tp.parameters()[4] );
}

void Trk::KalmanUpdator::logInputCov(const Amg::MatrixX& covTrk,
                                     const Amg::VectorX& rioPar, const Amg::MatrixX& covRio) const
{
  msg() << MSG::VERBOSE << "-U- cov    "<<std::setiosflags(std::ios::right)<<std::setprecision(3)
        << std::setw(9)<<covTrk(0,0)<<" "<< std::setw(9)<<covTrk(0,1)<<" "
        << std::setw(9)<<covTrk(0,2)<<" "<< std::setw(9)<<covTrk(0,3)<<" "
        << std::setw(9)<<covTrk(0,4)<<"\n";
  msg() << "                                      " << "          " << "           "
        << std::setw(9)<<covTrk(1,1)<<" "<< std::setw(9)<<covTrk(1,2)<<" "
        << std::setw(9)<<covTrk(1,3)<<" "<< std::setw(9)<<covTrk(1,4)<<"\n";
  msg() << "  covariance matrix                   " << "          " << "                     "
        << std::setw(9)<<covTrk(2,2)<<" "<< std::setw(9)<<covTrk(2,3)<<" "
        << std::setw(9)<<covTrk(2,4)<< "\n"  ;
  msg() << "  of the PREDICTED track pars         " << "          " << "                               " 
        << std::setw(9)<<covTrk(3,3)<<" "<< std::setw(9)<<covTrk(3,4)<<"\n"  ;
  msg() << "                                      " << "          " << "                                         " 
        << std::setw(9)<<covTrk(4,4)<<std::setprecision(6)<< endmsg;

  int nLocCoord = covRio.cols();
  msg() << MSG::VERBOSE << "-U- measurement locPos: ";
  for (int i=0; i<nLocCoord; i++) msg() << rioPar[i] << " ";
  msg() << endmsg;
  msg() << MSG::VERBOSE << "-U- measurement (err)^2: " <<std::setprecision(4)<<covRio(0,0);
  for (int i=1; i<nLocCoord; i++) msg() << ", "<<covRio(i,i);
  msg() << std::setprecision(6)<<endmsg;
}

void Trk::KalmanUpdator::logGainForm(int nc, const Amg::VectorX& r, const Amg::MatrixX& R,
                                     const Amg::MatrixX& K, const Amg::MatrixX& M) const
{
  // again some verbose debug output showing internals of updating
  msg() << MSG::VERBOSE << "-U- residual: r=("<<r[0];
  for (int i=1; i<nc; i++) msg() <<","<<r[i];
  msg() << ")" << endmsg;
  msg() << MSG::VERBOSE << "-U- inv. sigmaR=("<< R(0,0);
  for (int i=1; i<nc; i++) msg() << "," << R(i,i);
  msg() << ")" << endmsg;
  for (int i=0; i<nc; i++)  msg() << MSG::VERBOSE // K is a row x col = 5 x nc matrix.
	<< ( i==0 ? "-U- gain mtx  K=(" : "                (" )
        << std::setiosflags(std::ios::fixed | std::ios::showpoint | std::ios::right )
        << std::setw(7) << std::setprecision(4) << K(0,i)<<", " 
        << std::setw(7) << std::setprecision(4) << K(1,i)<<", "
        << std::setw(7) << std::setprecision(4) << K(2,i)<<", "
        << std::setw(7) << std::setprecision(4) << K(3,i)<<", "
        << std::setw(7) << std::setprecision(4) << K(4,i)<<")"
        << std::resetiosflags(std::ios::fixed) << endmsg;
  msg() << MSG::VERBOSE << "-U- matrix M: diag=("
        << M(0,0)<<"," << M(1,1)<<","
        << M(2,2)<<"," << M(3,3)<<","
        << M(4,4)                <<")" << endmsg;
}

void Trk::KalmanUpdator::logResult(const std::string& methodName, 
                                   const Amg::VectorX& par, const Amg::MatrixX& covPar) const
{
    // again some verbose debug output
    msg() << MSG::VERBOSE << "-U- ==> result for KalmanUpdator::"<<methodName<<endmsg;
    msg() << MSG::VERBOSE << "-U- new par"<<std::setiosflags(std::ios::right)<<std::setprecision(4)
          << std::setw( 9)<<par[0]<< std::setw(10)<<par[1]<<std::setprecision(5)
          << std::setw(10)<<par[2]<< std::setw(10)<<par[3]<<std::setprecision(4)
          << std::setw(10)<<par[4]                <<endmsg;
    msg() << MSG::VERBOSE << "-U- new cov" <<std::setiosflags(std::ios::right)<<std::setprecision(3)
          << std::setw(9)<<(covPar)(0,0)<<" "<< std::setw(9)<<(covPar)(0,1)<<" " 
          << std::setw(9)<<(covPar)(0,2)<<" "<< std::setw(9)<<(covPar)(0,3)
          << " " << std::setw(9)<<(covPar)(0,4)<< "\n";
    msg() << "                                      " << "           " << "          "
          << std::setw(9)<<(covPar)(1,1)<<" "<< std::setw(9)<<(covPar)(1,2)<<" "
          << std::setw(9)<<(covPar)(1,3)<<" "<< std::setw(9)<<(covPar)(1,4)<< "\n";
    msg() << "  covariance matrix                   " << "           " << "                    "
          << std::setw(9)<<(covPar)(2,2)<<" "<< std::setw(9)<<(covPar)(2,3)<<" "
          << std::setw(9)<<(covPar)(2,4)<< "\n";
    msg() << "  of the UPDATED   track pars         " << "           " 
          << "                              " <<std::setw(9)<<(covPar)(3,3)<< " "
          << std::setw(9)<<(covPar)(3,4)<< "\n";
    msg() << "                                      " << "           " 
          << "                                        " 
          << std::setw(9)<<(covPar)(4,4)<<std::setprecision(6)<< endmsg;
}
