/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanWeightUpdator.cxx
//   Source file for class KalmanWeightUpdator
//   The main updating calculations can be found in calculateFilterStep().
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "TrkMeasurementUpdator/KalmanWeightUpdator.h"

#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkParameters/TrackParameters.h"


// constructor
Trk::KalmanWeightUpdator::KalmanWeightUpdator(const std::string& t,const std::string& n,const IInterface* p) :
  AthAlgTool (t,n,p),
  m_weight(std::vector<double>(0)),
  m_log(msgSvc(), n),
  m_outputlevel(1) {
    // AlgTool stuff
    declareProperty("InitialWeight", m_weight);
    m_weight.push_back(1./250.); // set defaults _before_ reading from job options
    m_weight.push_back(1./250.);
    m_weight.push_back(1./0.25);
    m_weight.push_back(1./0.25);
    m_weight.push_back(1./0.000001);
    declareInterface<IUpdator>( this );
}

// destructor
Trk::KalmanWeightUpdator::~KalmanWeightUpdator() {}

// initialize
StatusCode Trk::KalmanWeightUpdator::initialize() {
    // pass individual outputlevel to message stream
    m_log.setLevel(msgLevel());
    m_outputlevel = m_log.level()-MSG::DEBUG;

    if (m_weight.size() < 5) {
        m_log << MSG::INFO << "Wrong-sized initial weight given, so set to default: \n";
        m_weight.clear(); // reset
        m_weight.push_back(1./250.);
        m_weight.push_back(1./250.);
        m_weight.push_back(1./0.25);
        m_weight.push_back(1./0.25);
        m_weight.push_back(1./0.000001);
    }
    m_log   << MSG::INFO << "Initial weight matrix: " << m_weight[0] << ", "
            << m_weight[1] << ", " << m_weight[2] << ", "
            << m_weight[3] << ", " << m_weight[4] << " (diagonal)" << endreq;
    m_log << MSG::INFO << "initialize() successful in " << name() << endreq;
    return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::KalmanWeightUpdator::finalize() {
    m_log << MSG::INFO << "finalize() successful in " << name() << endreq;
    return StatusCode::SUCCESS;
}

// updator #1 for Kalman Fitter - version with Amg::Vector2D (for example for PrepRawData)
const Trk::TrackParameters* Trk::KalmanWeightUpdator::addToState (  const Trk::TrackParameters& trkPar,
                                                                    const Amg::Vector2D&    measmtPos,
                                                                    const Amg::MatrixX&      measmtErr) const {

    FitQualityOnSurface*        fitQoS = 0;
    const Trk::TrackParameters* outPar = calculateFilterStep (trkPar, measmtPos, measmtErr,1,fitQoS, false);
    if (m_outputlevel <= 0 && outPar)
        logResult("addToState(TP,LPOS,ERR)",*outPar);
    return outPar;
}

// updator #2 for Kalman Fitter - version with LocalParameters (for example for RIO_OnTrack)
const Trk::TrackParameters* Trk::KalmanWeightUpdator::addToState (  const Trk::TrackParameters& trkPar,
                                                                    const LocalParameters&  measmtPar,
                                                                    const Amg::MatrixX&      measmtErr) const {

    FitQualityOnSurface*        fitQoS = 0;
    const Trk::TrackParameters* outPar = calculateFilterStep (trkPar, measmtPar, measmtErr,1,fitQoS, false);
    if (m_outputlevel <= 0 && outPar)
        logResult("addToState(TP,LPAR,ERR)",*outPar);
    return outPar;
}

// updator #3 for Kalman Fitter - version with Amg::Vector2D (for example for PrepRawData)
const Trk::TrackParameters* Trk::KalmanWeightUpdator::addToState (  const Trk::TrackParameters& trkPar,
                                                                    const Amg::Vector2D&    measmtPos,
                                                                    const Amg::MatrixX&      measmtErr,
                                                                    FitQualityOnSurface*&   fitQoS) const {

    if (fitQoS) {
        m_log << MSG::WARNING << "expect nil FitQuality pointer, refuse operation to"
        << " avoid mem leak!" << endreq;
        return 0;
    } else {
        const Trk::TrackParameters* outPar = calculateFilterStep (trkPar, measmtPos, measmtErr, 1, fitQoS, true);
        if (!outPar)
            fitQoS = 0;
        if (m_outputlevel <= 0 && outPar)
            logResult("addToState(TP,LPOS,ERR,FQ)",*outPar);
        return outPar;
    }
}

// updator #4 for Kalman Fitter - version with LocalParameters (for example for RIO_OnTrack)
const Trk::TrackParameters* Trk::KalmanWeightUpdator::addToState (  const Trk::TrackParameters& trkPar,
                                                                    const LocalParameters&  measmtPar,
                                                                    const Amg::MatrixX&      measmtErr,
                                                                    FitQualityOnSurface*&   fitQoS) const {
    if (fitQoS) {
        m_log << MSG::WARNING << "expect nil FitQuality pointer, refuse operation to"
        << " avoid mem leak!" << endreq;
        return 0;
    } else {
        const Trk::TrackParameters* outPar = calculateFilterStep (trkPar, measmtPar, measmtErr, 1, fitQoS, true);
        if (!outPar)
            fitQoS = 0;
        if (m_outputlevel <= 0 && outPar)
            logResult("addToState(TP,LPAR,ERR,FQ)",*outPar);
        return outPar;
    }
}

// inverse updator #1 for Kalman Fitter - version with Amg::Vector2D (for example for PrepRawData)
const Trk::TrackParameters* Trk::KalmanWeightUpdator::removeFromState ( const Trk::TrackParameters& trkPar,
                                                                        const Amg::Vector2D&    measmtPos,
                                                                        const Amg::MatrixX&      measmtErr) const {
    FitQualityOnSurface*        fitQoS = 0;
    const Trk::TrackParameters* outPar = calculateFilterStep (trkPar, measmtPos, measmtErr,-1,fitQoS, false);
    if (m_outputlevel<=0 && outPar)
        logResult("removeFromState(TP,LPOS,ERR)",*outPar);
    return outPar;
}

// inverse updator #2 for Kalman Fitter - version with LocalParameters (for example for RIO_OnTrack)
const Trk::TrackParameters* Trk::KalmanWeightUpdator::removeFromState ( const Trk::TrackParameters& trkPar,
                                                                        const LocalParameters&   measmtPar,
                                                                        const Amg::MatrixX&      measmtErr) const {

    FitQualityOnSurface*        fitQoS = 0;
    const Trk::TrackParameters* outPar = calculateFilterStep (trkPar, measmtPar, measmtErr,-1,fitQoS,false);
    if (m_outputlevel && outPar)
        logResult("removeFromState(TP,LPAR,ERR)",*outPar);
    return outPar;
}

// inverse updator #3 for Kalman Fitter - version with Amg::Vector2D (for example for PrepRawData)
const Trk::TrackParameters* Trk::KalmanWeightUpdator::removeFromState ( const Trk::TrackParameters& trkPar,
                                                                        const Amg::Vector2D&    measmtPos,
                                                                        const Amg::MatrixX&      measmtErr,
                                                                        FitQualityOnSurface*&    fitQoS) const {
    if (fitQoS) {
        m_log << MSG::WARNING << "expect nil FitQuality pointer, refuse operation to"
        << " avoid mem leak!" << endreq;
        return 0;
    } else {
        const Trk::TrackParameters* outPar = calculateFilterStep (trkPar, measmtPos, measmtErr, -1, fitQoS, true);
        if (!outPar)
            fitQoS = 0;
        if (m_outputlevel<=0 && outPar)
            logResult("removeFromState(TP,LPOS,ERR,FQ)",*outPar);
        return outPar;
    }
}

// inverse updator #4 for Kalman Fitter - version with LocalParameters (for example for RIO_OnTrack)
const Trk::TrackParameters* Trk::KalmanWeightUpdator::removeFromState ( const Trk::TrackParameters& trkPar,
                                                                        const LocalParameters&      measmtPar,
                                                                        const Amg::MatrixX&         measmtErr,
                                                                        FitQualityOnSurface*&       fitQoS) const {
    if (fitQoS) {
        m_log << MSG::WARNING << "expect nil FitQuality pointer, refuse operation to"
        << " avoid mem leak!" << endreq;
        return 0;
    } else {
        const Trk::TrackParameters* outPar = calculateFilterStep (trkPar, measmtPar, measmtErr, -1, fitQoS, true);
        if (!outPar)
            fitQoS = 0;
        if (m_outputlevel<=0 && outPar)
            logResult("removeFrommState(TP,LPAR,ERR,FQ)",*outPar);
        return outPar;
    }
}

// state-to-state updator, trajectory combination - version without fitQuality
const Trk::TrackParameters* Trk::KalmanWeightUpdator::combineStates (   const Trk::TrackParameters& one,
                                                                        const Trk::TrackParameters& two) const {
    // remember, either one OR two might have no error, but not both !
    if (!one.covariance() && !two.covariance()) {
        m_log << MSG::WARNING << "both parameters have no errors, invalid "
        << "use of Updator::combineStates(TP,TP)" << endreq;
        return 0;
    }
    // if only one of two has an error, return that one
    if (!one.covariance()) {
        if (m_outputlevel<=0)
            logResult("combineStates(TP,TP)",two);
        return two.clone();
    }
    if (!two.covariance()) {
        if (m_outputlevel<=0)
            logResult("combineStates(TP,TP)",one);
        return one.clone();
    }

    // m_log << MSG::INFO << one << two << endreq;

    // ... FIXME - TRT is so difficult, need to check that both parameters are in the same frame
    //             otherwise go into frame of one !

    // ok, normal, let's combine using weighted means formalism:
    //  G = G_1 + G_2
    //  p = G^-1.( G_1.p_1 + G_2.p_2 )
    AmgSymMatrix(5) G1 = one.covariance()->inverse();
    AmgSymMatrix(5) G2 = two.covariance()->inverse();
    AmgSymMatrix(5) G = G1 + G2;

    // avoid making weighted sum at opposite side of detector. 
    // We know that one&two's phis are in range, so lets put them back out of range.
    Amg::VectorX correctedTwo = two.parameters();
    if (fabs(two.parameters()[Trk::phi] - one.parameters()[Trk::phi])> M_PI) {
      correctedTwo[Trk::phi] += (correctedTwo[Trk::phi]>0.0) ? -2*M_PI : 2*M_PI;
    }
    Amg::VectorX weightedSum = G1 * one.parameters() + G2 * correctedTwo;
    AmgSymMatrix(5)* covNew = new AmgSymMatrix(5)(G.inverse());
    Amg::VectorX p = (*covNew) * weightedSum;
    if (m_outputlevel<=0) m_log << MSG::VERBOSE << "CS: p: (" << p[0] << "," << p[1] << "," << p[2] << "," << p[3] << "," << p[4] << ")" << endreq;
    // check if combined parameter has correct angular range and correct them otherwise
    if ( (!thetaPhiWithinRange(p)) ? !correctThetaPhiRange(p) : false ) {
        m_log << MSG::WARNING << "combineStates(TP,TP): could not combine angular values." << endreq;
	delete covNew;
        return 0;
    }
    // return cloned version of Track Parameters (MeasuredPerigee, MeasuredAtA...)
    const TrackParameters* comb = one.associatedSurface().createTrackParameters(p[Trk::loc1],p[Trk::loc2],
										      p[Trk::phi],p[Trk::theta],p[Trk::qOverP],covNew); 
    if (m_outputlevel<=0 && comb)
        logResult("combineStates(TP,TP)",*comb);
    return comb;
}

// state-to-state updator, trajectory combination - version with fitQuality
const Trk::TrackParameters* Trk::KalmanWeightUpdator::combineStates (   const Trk::TrackParameters& one,
                                                                        const Trk::TrackParameters& two,
                                                                        FitQualityOnSurface*& fitQoS) const {
    // try if both Track Parameters are measured ones ?
    // remember, either one OR two might have no error, but not both !
    if (!one.covariance() && !two.covariance()) {
        m_log << MSG::WARNING << "both parameters have no errors, invalid "
        << "use of Updator::combineStates(TP,TP,FQ)" << endreq;
        return 0;
    }
    if (fitQoS) {
        m_log << MSG::WARNING << "expect nil FitQuality pointer, refuse operation to"
        << " avoid mem leak!" << endreq;
        return 0;
    }
    // if only one of two has an error, return that one
    if (!one.covariance()) {
        fitQoS =  new FitQualityOnSurface(0.f, 5);
        if (m_outputlevel<=0)
            logResult("combineStates(TP,TP,FQ)",two);
        return two.clone();
    }
    if (!two.covariance()) {
        fitQoS =  new FitQualityOnSurface(0.f, 5);
        if (m_outputlevel<=0)
            logResult("combineStates(TP,TP,FQ)",one);
        return one.clone();
    }

    // ... FIXME - TRT is so difficult, need to check that both parameters are in the same frame
    //             otherwise go into frame of one !

    // ok, normal, let's combine using weighted means formalism:
    //  G = G_1 + G_2
    //  p = G^-1.( G_1.p_1 + G_2.p_2 )
    AmgSymMatrix(5) G1 = one.covariance()->inverse();
    AmgSymMatrix(5) G2 = two.covariance()->inverse();
    AmgSymMatrix(5) G = G1 + G2;

    // avoid making weighted sum at opposite side of detector. 
    // We know that one&two's phis are in range, so lets put them back out of range.
    Amg::VectorX correctedTwo = two.parameters();
    if (fabs(two.parameters()[Trk::phi] - one.parameters()[Trk::phi])> M_PI) {
      correctedTwo[Trk::phi] += (correctedTwo[Trk::phi]>0.0) ? -2*M_PI : 2*M_PI;
    }
    Amg::VectorX weightedSum = G1 * one.parameters() + G2 * correctedTwo;
    AmgSymMatrix(5)* covNew = new AmgSymMatrix(5)(G.inverse());
    Amg::VectorX p = (*covNew) * weightedSum;
    if (m_outputlevel<=0) m_log << MSG::VERBOSE << "CS: p: (" << p[0] << "," << p[1] << "," << p[2] << "," << p[3] << "," << p[4] << ")" << endreq;
    // check if combined parameter has correct angular range and correct them otherwise
    if ( (!thetaPhiWithinRange(p)) ? !correctThetaPhiRange(p) : false ) {
        m_log << MSG::WARNING << "combineStates(TP,TP): could not combine angular values." << endreq;
	delete covNew;
        return 0;
    }
    // return cloned version of Track Parameters (MeasuredPerigee, MeasuredAtA...)
    const TrackParameters* comb = one.associatedSurface().createTrackParameters(p[Trk::loc1],p[Trk::loc2],
										      p[Trk::phi],p[Trk::theta],p[Trk::qOverP],covNew); 
    // compute fit quality:
    // chi^2 = (p_2 - p)^T G_2 (p_2 - p) + (p - p_1)^T G_1 (p - p_1)
    Amg::VectorX r2 = two.parameters() - p;
    Amg::VectorX r1 = p - one.parameters();
    fitQoS = makeChi2Object(r2, G2, 31, r1, G1, 31);

    if (m_outputlevel<=0 && comb)
        logResult("combineStates(TP,TP,FQ)",*comb);
    return comb;
}


// estimator for FitQuality on Surface
const Trk::FitQualityOnSurface*
Trk::KalmanWeightUpdator::fullStateFitQuality ( const Trk::TrackParameters& trkPar,
                                                const Amg::Vector2D& locPos,
                                                const Amg::MatrixX& rioErr) const {
    if (m_outputlevel<=0) {
        m_log << MSG::DEBUG << "--> entered KalmanWeightUpdator::fullStateFitQuality()" << endreq;
    }

    // try if Track Parameters are measured ones ?
    if (!trkPar.covariance()) {
        m_log << MSG::ERROR << "updated smoother/trajectory has no error matrix" << endreq;
        return 0;
    }
    // covariance matrix for prediction
    const AmgSymMatrix(5)& covTrk = *trkPar.covariance();

    // For the Amg::Vector2D version, need to know # meas. coord. from covariance matrix.
    int nLocCoord = rioErr.cols();
    Amg::VectorX rioPar(nLocCoord);
    for (int iLocCoord=0; iLocCoord < nLocCoord; iLocCoord++) {
        rioPar[iLocCoord] = locPos[iLocCoord];
    }

    // measurement Matrix ( n x m )
    Amg::MatrixX H(rioErr.cols(),covTrk.cols());
    H.setZero();
    for (int i=0; i < nLocCoord; i++)
      H(i,i)=1.f;

    // residuals
    Amg::VectorX    r = rioPar - H * trkPar.parameters();

    // all the rest is outsourced to a common chi2 routine
    return makeChi2Object(r,covTrk,rioErr,H,-1);
}


// estimator for FitQuality on Surface
const Trk::FitQualityOnSurface*
Trk::KalmanWeightUpdator::fullStateFitQuality ( const Trk::TrackParameters& trkPar,
                                                const Trk::LocalParameters& rioPar,
                                                const Amg::MatrixX&     rioErr) const {
    if (m_outputlevel<=0) {
        m_log << MSG::VERBOSE
        << "--> entered KalmanWeightUpdator::fullStateFitQuality()" << endreq;
    }

    // try if Track Parameters are measured ones ?
    if (!trkPar.covariance()) {
      m_log << MSG::ERROR << "updated smoother/trajectory has no error matrix" << endreq;
      return 0;
    }
    // covariance matrix for prediction
    const AmgSymMatrix(5)& covTrk = *trkPar.covariance();

    // State to measurement dimensional reduction Matrix ( n x m )
    Amg::MatrixX H(rioPar.expansionMatrix());

    // residuals
    Amg::VectorX r = rioPar - H * trkPar.parameters();
    if (!diffThetaPhiWithinRange(r), rioPar.parameterKey()) correctThetaPhiRange(r,true,rioPar.parameterKey());

    // all the rest is outsourced to a common chi2 routine
    return makeChi2Object(r,covTrk,rioErr,H,-1);
}

// estimator for FitQuality on Surface
const Trk::FitQualityOnSurface*
Trk::KalmanWeightUpdator::predictedStateFitQuality (    const Trk::TrackParameters& predPar,
                                                        const Amg::Vector2D&   rioLocPos,
                                                        const Amg::MatrixX&     rioLocErr) const {
    if (m_outputlevel<=0) {
        m_log << MSG::VERBOSE
        << "--> entered KalmanWeightUpdator::predictedStateFitQuality()" << endreq;
    }
    // try if Track Parameters are measured ones ?
    if (!predPar.covariance()) {
      m_log << MSG::ERROR << "input trajectory state has no error matrix" << endreq;
      return 0;
    }
    // covariance matrix for prediction
    const AmgSymMatrix(5)& covPred = *predPar.covariance();

    // For the Amg::Vector2D version, need to know # meas. coord. from covariance matrix.
    int nLocCoord = rioLocErr.cols();
    Amg::VectorX rioPar(nLocCoord);
    for (int iLocCoord=0; iLocCoord < nLocCoord; iLocCoord++) {
        rioPar[iLocCoord] = rioLocPos[iLocCoord];
    }

    // measurement Matrix ( n x m )
    Amg::MatrixX H(rioLocErr.cols(),covPred.cols());
    H.setZero();
    for (int i=0; i < nLocCoord; i++)
      H(i,i)=1.f;

    // residuals
    Amg::VectorX    r = rioPar - H * predPar.parameters();

    // all the rest is outsourced to a common chi2 routine
    return makeChi2Object(r,covPred,rioLocErr,H,+1);
}

// estimator for FitQuality on Surface
const Trk::FitQualityOnSurface*
Trk::KalmanWeightUpdator::predictedStateFitQuality (    const Trk::TrackParameters& predPar,
                                                        const Trk::LocalParameters& rioPar,
                                                        const Amg::MatrixX&     rioErr) const {
    if (m_outputlevel<=0) {
        m_log << MSG::VERBOSE
        << "--> entered KalmanWeightUpdator::predictedStateFitQuality()" << endreq;
    }

    // try if Track Parameters are measured ones ?
    if (!predPar.covariance()) {
      m_log << MSG::ERROR << "input trajectory state has no error matrix" << endreq;
      return 0;
    }
    // covariance matrix for prediction
    const AmgSymMatrix(5)& covPred = *predPar.covariance();

    // State to measurement dimensional reduction Matrix ( n x m )
    Amg::MatrixX H(rioPar.expansionMatrix());

    // residuals
    Amg::VectorX    r = rioPar - H * predPar.parameters();

    // all the rest is outsourced to a common chi2 routine
    return makeChi2Object(r,covPred,rioErr,H,+1);
}

// estimator for FitQuality on Surface
const Trk::FitQualityOnSurface*
Trk::KalmanWeightUpdator::predictedStateFitQuality (    const Trk::TrackParameters& one,
                                                        const Trk::TrackParameters& two ) const {
    if (m_outputlevel<=0) {
        m_log << MSG::VERBOSE << "--> entered KalmanWeightUpdator::predictedStateFitQuality(TP,TP)" << endreq;
    }
    // try if both Track Parameters are measured ones ?
    // remember, either one OR two might have no error, but not both !
    if (!one.covariance() && !two.covariance()) {
        m_log << MSG::WARNING << "both parameters have no errors, invalid "
        << "use of Updator::fitQuality()" << endreq;
        return 0;
    }
    // if only one of two has an error, place a message.
    if (!one.covariance() || !two.covariance()) {
        m_log << MSG::DEBUG << "One parameter does not have uncertainties, "
        << "assume initial state and return chi2=0.0" << endreq;
        return new FitQualityOnSurface(0.f, 5);
    }

    // covariance matrix for prediction and the state to be added
    const AmgSymMatrix(5)& covTrkOne = *one.covariance();
    const AmgSymMatrix(5)& covTrkTwo = *two.covariance();
    // residuals
    Amg::VectorX    r = two.parameters() - one.parameters();
    // catch [-pi,pi] phi boundary problems to keep chi2 under control
    if (!diffThetaPhiWithinRange(r)) correctThetaPhiRange(r,true);

    AmgSymMatrix(5) R = (covTrkOne + covTrkTwo).inverse();

    double  chiSquared = r.transpose()*R*r;
    return new FitQualityOnSurface(chiSquared, 5);
}

const std::vector<double> Trk::KalmanWeightUpdator::initialErrors() const {
  std::vector<double> E(5);
  for (int i=0; i<5; ++i) E[i] = (m_weight[i]!=0.) ? 
      sqrt(1./m_weight[i]) : 1E5 ;
  return E;
}

// mathematics for Kalman updator and inverse Kalman filter
const Trk::TrackParameters* Trk::KalmanWeightUpdator::calculateFilterStep ( const Trk::TrackParameters& p,
                                                                            const Amg::Vector2D&  mpos,
                                                                            const Amg::MatrixX& cov,
                                                                            const int sign,
                                                                            Trk::FitQualityOnSurface*& fitQoS,
                                                                            bool createFQoS ) const {
    // TODO: use commom math function for localPosition and localParameters

    // Naming conventions of Fruehwirth, Data Analysis Techniques for High-Energy Physics, 2nd ed.:
    // m = m_k (measurement in layer k)
    // W = W_k ( = V_k^{-1} ) (weight matrix of m)
    // pOld = p_{ k|k-1 } for Forward Filter step (addToState)     (old track state)
    //      = p_{ k|n }   for inverse Kalman filter (removeFromState)
    // pnew = p_{ k|k }   for Forward Filter step (addToState)     (new track state)
    //      = p*_{ k|n }  for inverse Kalman filter (removeFromState)
    // GOld = G_{ k|k-1 } for Forward Filter step (addToState)     (weight matrix of old track state)
    //      = G_{ k|n }   for inverse Kalman filter (removeFromState)
    // GNew = G_{ k|k }   for Forward Filter step (addToState)     (weight matrix of new track state)
    //      = G*_{ k|n }  for inverse Kalman filter (removeFromState)

  Amg::MatrixX W = cov.inverse();

  // try if Track Parameters are measured ones ?

  AmgSymMatrix(5) GOld;
  if (!p.covariance()) {
    if (sign<0) {
      m_log << MSG::ERROR << "MeasuredTrackParameters == Null, can not calculate "
            << "updated track state" << endreq;
      return 0;
    } else {
      // no error given - use zero weight for the time
      GOld.setZero();
      if (m_outputlevel <= 0)
	m_log << MSG::VERBOSE << "-U- no covTrk at input - "
	      << "assign zero weight matrix for the time being." << endreq;
      (GOld)(0,0) = m_weight[0];
      (GOld)(1,1) = m_weight[1];
      (GOld)(2,2) = m_weight[2];
      (GOld)(3,3) = m_weight[3];
      (GOld)(4,4) = m_weight[4];
    }
  } else {
    GOld = p.covariance()->inverse();
  }

  //    LocalParameters  parTrk = trkPar.parameters();
  Amg::VectorX pOld = p.parameters();
  if (!thetaPhiWithinRange(pOld)) {
    m_log << MSG::WARNING << (sign>0?"addToState(TP,LPOS,ERR..)":"removeFromState(TP,LPOS,ERR..)")
	  << ": undefined phi,theta range in input parameters." << endreq;
    return 0;
  }

  // measurement vector of RIO_OnTrack - needs more care for # local par?
  int nLocCoord = W.cols();
  if ( (nLocCoord < 1) || (nLocCoord > 5 ) ) {
    m_log << MSG::WARNING << " strange number of local coordinates: "
	  << nLocCoord << endreq;
  }
  Amg::VectorX m(nLocCoord);
  for (int iLocCoord=0; iLocCoord < nLocCoord; iLocCoord++) {
    m[iLocCoord] = mpos[iLocCoord];
  }
  if (m_outputlevel<0)
    logInputCov(GOld, m, W);

  // measurement Matrix ( n x m )
  Amg::MatrixX H(W.cols(), GOld.cols());
  H.setZero();
  for (int i=0; i < nLocCoord; i++)
    H(i,i)=1.f;

  // calc updated weight matrix: GNew = GOld +/- H.T * W * H
  // NB: use of W.similarityT(H) is very inefficient. Copying of entries will be much faster...
  AmgSymMatrix(5) Gnew = GOld + sign * H.transpose()*W*H;
  AmgSymMatrix(5)* covNew = new AmgSymMatrix(5)(Gnew.inverse());
  // calc new track state: pNew = Gnew^{-1} * ( GOld * pOld +/- H.T * W * m)
  Amg::VectorX weightedSum = GOld * pOld + sign * H.transpose() * W * m;
  
  Amg::VectorX pNew = (*covNew) * weightedSum;
  if ( (!thetaPhiWithinRange(pNew)) ? !correctThetaPhiRange(pNew) : false ) {
    m_log << MSG::WARNING << "calculateFS(TP,LPAR,ERR): bad angles in filtered state!" << endreq;
    return 0;
  }
  
  // compute chi2 if needed
  if (createFQoS) {
      // calc Chi2
      // ref. Fruehwirth, Application of Kalman filtering, NIM A262, 446:
      // Chi^2 = r^T W.r + (pNew - pOld)^T GOld.(pNew - pOld)
    Amg::VectorX r = m - H * pNew;
    Amg::VectorX deltaP = pNew - pOld;
    //double chiSquared = W.similarity(r) + GOld.similarity(deltaP);
    fitQoS = makeChi2Object(r, W, 0, deltaP, GOld, 31);
  }
    
  const TrackParameters* updated = p.associatedSurface().createTrackParameters(pNew[Trk::loc1],pNew[Trk::loc2],
										     pNew[Trk::phi],pNew[Trk::theta],
										     pNew[Trk::qOverP],covNew); 
  return updated;
}


// mathematics for Kalman updator and inverse Kalman filter
const Trk::TrackParameters* Trk::KalmanWeightUpdator::calculateFilterStep ( const Trk::TrackParameters& p,
                                                                            const Trk::LocalParameters&  m,
                                                                            const Amg::MatrixX& W,
                                                                            const int sign,
                                                                            Trk::FitQualityOnSurface*& fitQoS,
                                                                            bool createFQoS ) const {
  // in FQoS-version set pointer to 1, then use it as bool: if FQoS then new FQoS().

  // Naming conventions of Fruehwirth, Data Analysis Techniques for High-Energy Physics, 2nd ed.:
  // m = m_k (measurement in layer k)
  // W = W_k ( = V_k^{-1} ) (weight matrix of m)
  // pOld = p_{ k|k-1 } for Forward Filter step (addToState)     (old track state)
  //      = p_{ k|n }   for inverse Kalman filter (removeFromState)
  // pnew = p_{ k|k }   for Forward Filter step (addToState)     (new track state)
  //      = p*_{ k|n }  for inverse Kalman filter (removeFromState)
  // GOld = G_{ k|k-1 } for Forward Filter step (addToState)     (weight matrix of old track state)
  //      = G_{ k|n }   for inverse Kalman filter (removeFromState)
  // GNew = G_{ k|k }   for Forward Filter step (addToState)     (weight matrix of new track state)
  //      = G*_{ k|n }  for inverse Kalman filter (removeFromState)


  AmgSymMatrix(5) GOld;
  if (!p.covariance()) {
    if (sign<0) {
      m_log << MSG::ERROR << "MeasuredTrackParameters == Null, can not calculate "
            << "updated track state" << endreq;
      return 0;
    } else {
      // no error given - use zero weight for the time
      GOld.setZero();
      if (m_outputlevel <= 0)
	m_log << MSG::VERBOSE << "-U- no covTrk at input - "
	      << "assign zero weight matrix for the time being." << endreq;
      (GOld)(0,0) = m_weight[0];
      (GOld)(1,1) = m_weight[1];
      (GOld)(2,2) = m_weight[2];
      (GOld)(3,3) = m_weight[3];
      (GOld)(4,4) = m_weight[4];
    }
  } else {
    GOld = p.covariance()->inverse();
  }
  //    LocalParameters  parTrk = trkPar.parameters();
  Amg::VectorX pOld = p.parameters();
  if (!thetaPhiWithinRange(pOld)) {
    m_log << MSG::WARNING << (sign>0?"addToState(TP,LPAR,ERR..)":"removeFromState(TP,LPAR,ERR..)")
	  << ": undefined phi,theta range in input parameters." << endreq;
    return 0;
  }
  if (!thetaPhiWithinRange(m, m.parameterKey())) {
    m_log << MSG::WARNING << (sign>0?"addToState(TP,LPAR,ERR..)":"removeFromState(TP,LPAR,ERR..)")
	  << ": undefined phi,theta range in input measurement !!" << endreq;
    return 0;
  }

  // measurement vector of RIO_OnTrack - needs more care for # local par?
  int nLocCoord = W.cols();
  if (m.dimension() != nLocCoord )
    m_log << MSG::ERROR << "Inconsistency in dimension of local coord - "
	  << "problem with new LocalParameters objects?" << endreq;
  if ( (nLocCoord < 1) || (nLocCoord > 5 ) ) {
    m_log << MSG::WARNING << " strange number of local coordinates: "
	  << nLocCoord << endreq;
  }

  // State to measurement dimensional reduction Matrix (wrong name in Amg::MatrixX)
  Amg::MatrixX H(m.expansionMatrix());

  // calc updated weight matrix: GNew = GOld +/- H.T * W * H
  // NB: use of W.similarityT(H) is very inefficient. Copying of entries will be much faster...
  AmgSymMatrix(5) Gnew = GOld + sign * H.transpose()*W*H;
  AmgSymMatrix(5)* Cnew = new AmgSymMatrix(5)(Gnew.inverse());

  // calc new track state: pNew = Gnew^{-1} * ( GOld * pOld +/- H.T * W * m)
  Amg::VectorX weightedSum = GOld * pOld + sign * H.transpose() * W * m;
  Amg::VectorX pNew = (*Cnew) * weightedSum;
  if (m_outputlevel<=0) m_log << MSG::VERBOSE << "FS: pNew: (" << pNew[0] << "," << pNew[1] << "," << pNew[2] << "," << pNew[3] << "," << pNew[4] << ")" << endreq;
  if ( (!thetaPhiWithinRange(pNew)) ? !correctThetaPhiRange(pNew) : false ) {
    m_log << MSG::WARNING << "calculateFS(TP,LPAR,ERR): bad angles in filtered state!" << endreq;
    return 0;
  }
  if (m_outputlevel <= 0) {
    logOutputCov(pNew, *Cnew);
  }

  // compute chi2 if needed
  if (createFQoS) {
    // calc Chi2
    // ref. Fruehwirth, Application of Kalman filtering, NIM A262, 446:
    // Chi^2 = r^T W.r + (pNew - pOld)^T GOld.(pNew - pOld)
    Amg::VectorX r = m - H * pNew;
    Amg::VectorX deltaP = pNew - pOld;
    //double chiSquared = W.similarity(r) + GOld.similarity(deltaP);
    fitQoS = makeChi2Object(r, W, m.parameterKey(), deltaP, GOld, 31);
  }

  const TrackParameters* updated = p.associatedSurface().createTrackParameters(pNew[Trk::loc1],pNew[Trk::loc2],
										     pNew[Trk::phi],pNew[Trk::theta],
										     pNew[Trk::qOverP],Cnew); 
  return updated;
}


// chi2 calculation with weighted means formalism
Trk::FitQualityOnSurface* Trk::KalmanWeightUpdator::makeChi2Object( Amg::VectorX& residual1,
                                                                    const Amg::MatrixX& weight1,
                                                                    const int key1,
                                                                    Amg::VectorX& residual2,
                                                                    const Amg::MatrixX& weight2,
                                                                    const int key2 ) const {

    // compute fit quality:
    // chi^2 = (m - H.p_new)^T W (m - H.p_new) + (p_new - p_old)^T G_old (p_new - p_old)
    //       = r_1^T G_1 r_1 + r_2^T G_2 r_2

    if (m_outputlevel<=0) {
        m_log << MSG::VERBOSE << "residual1: " << residual1[0] << endreq;
        m_log << MSG::VERBOSE << "residual2: " << residual2[0] << endreq;
    }
    // catch [-pi,pi] phi boundary problems to keep chi2 under control
    if (key1) {
      if (!diffThetaPhiWithinRange(residual1, key1)) correctThetaPhiRange(residual1,true, key1);
    }
    if (key2) {
      if (!diffThetaPhiWithinRange(residual2, key2)) correctThetaPhiRange(residual2,true, key2);
    }

    // attention: similarity(vector) and similarity(matrix) are defined differently in CLHEP:
    // similarity(const Amg::VectorX &v): Returns v.T()*s*v
    // similarity(const HepSymMatrix &m1): Returns m1*s*m1.T()
    double  chiSquared = residual1.transpose()*weight1*residual1;
    chiSquared += residual2.transpose()*weight2*residual2;
    // number of degree of freedom added
    int numberDoF  = weight1.cols();

    if (m_outputlevel<0) m_log << MSG::VERBOSE << "-U- fitQuality on surface, chi2 :" << chiSquared << " / ndof= " << numberDoF << endreq;

    return new FitQualityOnSurface(chiSquared, numberDoF);
}

// chi2 calculation with gain matrix formalism and predicted states, resp.
Trk::FitQualityOnSurface* Trk::KalmanWeightUpdator::makeChi2Object( Amg::VectorX& residual,
                                                                    const Amg::MatrixX& covTrk,
                                                                    const Amg::MatrixX& covRio,
                                                                    const Amg::MatrixX& H,
                                                                    const int sign) const
{   // sign: -1 = updated, +1 = predicted parameters.
    Amg::MatrixX R = covRio + sign* covTrk.similarity(H);
    R = R.inverse();
    // get chi2 = r.T() * R^-1 * r
    double  chiSquared = residual.transpose()*R*residual;
    if (m_outputlevel<0) {
      m_log << MSG::VERBOSE << "-U- fitQuality of "<< (sign>0?"predicted":"updated")
	    <<" state, chi2 :" << chiSquared << " / ndof= " << covRio.cols() << endreq;
    }

    // number of degree of freedom added
    int     numberDoF  = covRio.cols();
    return new FitQualityOnSurface(chiSquared, numberDoF);
}


void Trk::KalmanWeightUpdator::logInputCov(const Amg::MatrixX& covTrk,
        const Amg::VectorX& rioPar, const Amg::MatrixX& covRio) const {
    m_log << MSG::VERBOSE << "-U- cov"<<std::setiosflags(std::ios::right)<<std::setprecision(3)
    << std::setw(9)<<covTrk(0,0)<<" "<< std::setw(9)<<covTrk(0,1)<<" "
    << std::setw(9)<<covTrk(0,2)<<" "<< std::setw(9)<<covTrk(0,3)<<" "
    << std::setw(9)<<covTrk(0,4)<<"\n";
    m_log << "                                      " << "      " << "           "
    << std::setw(9)<<covTrk(1,1)<<" "<< std::setw(9)<<covTrk(1,2)<<" "
    << std::setw(9)<<covTrk(1,3)<<" "<< std::setw(9)<<covTrk(1,4)<<"\n";
    m_log << "  covariance matrix                   " << "      " << "                     "
    << std::setw(9)<<covTrk(2,2)<<" "<< std::setw(9)<<covTrk(2,3)<<" "
    << std::setw(9)<<covTrk(2,4)<< "\n"  ;
    m_log << "  of the PREDICTED track pars         " << "      " << "                               "
    << std::setw(9)<<covTrk(3,3)<<" "<< std::setw(9)<<covTrk(3,4)<<"\n"  ;
    m_log << "                                      " << "      " << "                                         "
    << std::setw(9)<<covTrk(4,4)<<std::setprecision(6)<< endreq;

    int nLocCoord = covRio.cols();
    m_log << MSG::VERBOSE << "-U- measurement locPos: ";
    for (int i=0; i<nLocCoord; i++)
        m_log << rioPar[i] << " ";
    m_log << endreq;
    m_log << MSG::VERBOSE << "-U- measurement (err)^2: " <<std::setprecision(4)<<covRio(0,0);
    for (int i=1; i<nLocCoord; i++)
      m_log << ", "<<covRio(i,i);
    m_log << std::setprecision(6)<<endreq;
}

void Trk::KalmanWeightUpdator::logOutputCov(const Amg::VectorX& par,
        const Amg::MatrixX& covPar) const {
    // again some verbose debug output

    m_log << MSG::VERBOSE << "-U- par'"<<std::setiosflags(std::ios::right)<<std::setprecision(4)
    << std::setw( 9)<<par[0]<< std::setw(10)<<par[1]
    << std::setw(10)<<par[2]<< std::setw(10)<<par[3]
    << std::setw(10)<<par[4]                <<endreq;
    m_log << MSG::VERBOSE << "-U- cov'" <<std::setiosflags(std::ios::right)<<std::setprecision(3)
    << std::setw(9)<<covPar(0,0)<<" "<< std::setw(9)<<covPar(0,1)<<" "
	  << std::setw(9)<<covPar(0,2)<<" "<< std::setw(9)<<covPar(0,3)<< " "
	  << std::setw(9)<<covPar(0,4)<< "\n";
    m_log << "                                      " << "        " << "          "
    << std::setw(9)<<covPar(1,1)<<" "<< std::setw(9)<<covPar(1,2)<<" "
    << std::setw(9)<<covPar(1,3)<<" "<< std::setw(9)<<covPar(1,4)<< "\n";
    m_log << "  covariance matrix                   " << "        " << "                    "
    << std::setw(9)<<covPar(2,2)<<" "<< std::setw(9)<<covPar(2,3)<<" "
    << std::setw(9)<<covPar(2,4)<< "\n";
    m_log << "  of the UPDATED   track pars         " << "        "
    << "                              " <<std::setw(9)<<covPar(3,3)<< " "
    << std::setw(9)<<covPar(3,4)<< "\n";
    m_log << "                                      " << "        "
    << "                                        "
    << std::setw(9)<<covPar(4,4)<<std::setprecision(6)<< endreq;
}

void Trk::KalmanWeightUpdator::logResult(const std::string& IDstring,
        const Trk::TrackParameters& tp) const {
    m_log << MSG::DEBUG   << "--> Result for KalmanWeightUpdator::" << IDstring << endreq;
    m_log << MSG::VERBOSE << "    Trkpar:" << std::setiosflags(std::ios::right)<<std::setprecision(4)
    << std::setw( 9)<<tp.parameters()[0]<< std::setw(10)<<tp.parameters()[1]
    << std::setw(10)<<tp.parameters()[2]<< std::setw(10)<<tp.parameters()[3]
    << std::setw(10)<<tp.parameters()[4]<< endreq;
}


void Trk::KalmanWeightUpdator::logOutputWeightMat(Amg::MatrixX& covPar) const {
    m_log << MSG::VERBOSE << "        " <<std::setiosflags(std::ios::right)<<std::setprecision(3)
	  << std::setw(9)<<covPar(0,0)<<" "<< std::setw(9)<<covPar(0,1)<<" "
	  << std::setw(9)<<covPar(0,2)<<" "<< std::setw(9)<<covPar(0,3)<< " "
	  << std::setw(9)<<covPar(0,4)<< "\n";
    m_log << "                                      " << "        " << "          "
    << std::setw(9)<<covPar(1,1)<<" "<< std::setw(9)<<covPar(1,2)<<" "
    << std::setw(9)<<covPar(1,3)<<" "<< std::setw(9)<<covPar(1,4)<< "\n";
    m_log << "   weight    matrix                   " << "        " << "                    "
    << std::setw(9)<<covPar(2,2)<<" "<< std::setw(9)<<covPar(2,3)<<" "
    << std::setw(9)<<covPar(2,4)<< "\n";
    m_log << "                                      " << "        "
    << "                              " <<std::setw(9)<<covPar(3,3)<< " "
    << std::setw(9)<<covPar(3,4)<< "\n";
    m_log << "                                      " << "        "
    << "                                        "
    << std::setw(9)<<covPar(4,4)<<std::setprecision(6)<< endreq;
}

bool Trk::KalmanWeightUpdator::correctThetaPhiRange(Amg::VectorX& V, const bool isDifference, const int key) const {
    // get phi and theta coordinate.
    // The case of no angles among meas'coordinates should have been blocked
    // by inline function thetaPhiWithinRange() - but is caught also here ("-1")
    int jphi = -1, jtheta = -1;
    double thetaMin = (isDifference ? -M_PI : 0);
    if (key == 31) jphi = Trk::phi;
    else if (key & 4) // phi is within localParameter and a measured coordinate
        for (int itag = 0, ipos=1 ; itag<Trk::phi; ++itag, ipos*=2) if (key & ipos) ++jphi;
    if (key == 31) jtheta = Trk::theta;
    else if (key & 8) // theta is within localParameter and a measured coordinate
        for (int itag = 0, ipos=1 ; itag<Trk::theta; ++itag, ipos*=2) if (key & ipos) ++jtheta;

    // correct phi coordinate if necessary
    if ((jphi>=0) && (fabs(V[jphi]) > M_PI ) ) {
        if (m_outputlevel <=0) 
          m_log << MSG::DEBUG << "-U- phi value out of range, phi = "<<V[jphi]
                << "  (param key = "<<key<<")"<<endreq;
        if (fabs(V[jphi]) > 10*M_PI) { // protect while loop
            m_log << MSG::WARNING << "-U- track direction angles have numerical problems, stop update." << endreq;
            m_log << MSG::WARNING << "-U- phi value: "<<V[jphi]<<endreq;
            return false;
        }
        while (fabs(V[jphi]) > M_PI ) V[jphi] += (V[jphi]>0) ? -2*M_PI : 2*M_PI;
        if (m_outputlevel <=0) m_log << MSG::DEBUG << "-U- now use corrected phi value = "<<V[jphi]<<endreq;
    }

    // correct theta and phi coordinate
    if ((jtheta>=0) && (V[jtheta]<thetaMin || V[jtheta]> M_PI) ) {
        if (m_outputlevel <= (isDifference?0:0) ) {
            if (key !=31) m_log << MSG::WARNING << "-U- key: "<<key << " jphi: "<<jphi << " jtheta: "<<jtheta << endreq;
            m_log << MSG::WARNING << "-U- " << (isDifference?"diff. ":" ") << "angles out of range,   phi = ";
            if (jphi>=0) m_log << V[jphi]; else m_log <<"free";
            m_log << " theta =  " << V[jtheta] <<endreq;
        }
        if ( (fabs(V[jtheta]) > 5*M_PI) ||              // protect while loop
            (isDifference && fabs(V[jtheta])>M_PI) ) {  // can't repair over-limit theta differences
            m_log << MSG::WARNING << "-U- track direction angles have numerical problems, stop update." << endreq;
            m_log << MSG::WARNING << "-U- " << (isDifference?"diff. ":" ") << "angles out of range,   phi = ";
            if (jphi>=0) m_log << V[jphi]; else m_log <<"free";
            m_log << " theta =  " << V[jtheta] <<endreq;
            return false;
        }
        while (V[jtheta] > 2*M_PI) V[jtheta] -= 2*M_PI;
        while (V[jtheta] < -M_PI)   V[jtheta] += M_PI;
        if (V[jtheta] > M_PI) {
            V[jtheta] = 2*M_PI - V[jtheta];
            if (jphi>=0) V[jphi]   += (V[jphi]>0) ? -M_PI : M_PI;
        }
        if (V[jtheta] < 0.0) {
            V[jtheta] = -V[jtheta];
            if (jphi>=0) V[jphi]   += (V[jphi]>0) ? -M_PI : M_PI;
        }
        if (m_outputlevel <=0) {
            m_log << MSG::DEBUG << "-U- now use corrected " << (isDifference?"diff. ":" ") << "value phi= ";
            if (jphi>=0) m_log << V[jphi]; else m_log <<"free";
            m_log << " theta =  " << V[jtheta] <<endreq;
        }
    }
    return true;
}
