/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanUpdatorAmg.cxx
//   Source file for class KalmanUpdatorAmg
//   uses Eigen math library internally.
//   The main updating calculations can be found in
//   calculateFilterStep_1D() to _5D().
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
///////////////////////////////////////////////////////////////////

#include "TrkMeasurementUpdator/KalmanUpdatorAmg.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"

// #include "EventPrimitives/SymmetricMatrixHelpers.h"
// #include <Eigen/LU>
#include <cmath>
#include <cstring>

const Trk::ParamDefsAccessor Trk::KalmanUpdatorAmg::s_enumAccessor;

// constructor
Trk::KalmanUpdatorAmg::KalmanUpdatorAmg(const std::string& t,const std::string& n,const IInterface* p) :
    AthAlgTool (t,n,p),
    m_cov_stdvec(std::vector<double>{250.,250.,0.25,0.25,0.000001}),
    m_covariance0(nullptr),
    m_thetaGainDampingValue(0.1),
    m_unitMatrix(AmgMatrix(5,5)::Identity()),
    m_reMatrices(5)
{
    // AlgTool stuff
    declareProperty("InitialCovariances",           m_cov_stdvec,               "default covariance to be used at start of filter");
    declareProperty("FastTrackStateCovCalculation", m_useFruehwirth8a=false,    "toggles which formula to use for updated cov");
    declareInterface<IUpdator>( this );
}

// destructor
Trk::KalmanUpdatorAmg::~KalmanUpdatorAmg()
{}

// initialize
StatusCode Trk::KalmanUpdatorAmg::initialize()
{
    // covariance setting
    if (m_cov_stdvec.size() < 5) {
      ATH_MSG_VERBOSE("Wrong-sized initial covariance given, so set to default");
      m_cov_stdvec.clear(); // reset
      m_cov_stdvec = std::vector<double>{250.,250.,0.25,0.25,0.000001};
    }
    m_covariance0 = new AmgSymMatrix(5); m_covariance0->setZero();
    for (size_t it = 0; it < 5; ++it) {
        m_cov0Vec[it]           =  m_cov_stdvec[it];
        (*m_covariance0)(it,it) =  m_cov_stdvec[it];
    }
    if (m_useFruehwirth8a)
      ATH_MSG_INFO("Fast computation will be used for track state cov matrices (Fruehwirth-1987 eq. 8a).");
    else
      ATH_MSG_INFO("Track state cov matrix will be calculated according to Gelb-1975 p305.");

    ATH_MSG_INFO("initialize() successful in " << name());
    
    return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::KalmanUpdatorAmg::finalize()
{
    ATH_MSG_INFO("finalize() successful in " << name());
    return StatusCode::SUCCESS;
}

// updator #1 for Kalman Fitter - version with Amg::Vector2D (PrepRawData method)
Trk::TrackParameters* Trk::KalmanUpdatorAmg::addToState (const Trk::TrackParameters& trkPar,
                                                         const Amg::Vector2D& measLocPos, 
                                                         const Amg::MatrixX& measLocCov) const 
{
  if (msgLvl(MSG::VERBOSE)) {logStart("addToState(TP,LPOS,ERR)",trkPar.parameters());}
  FitQualityOnSurface*    fitQoS = nullptr;
  const int updatingSign = 1;
  
  // get the Start covariance matrix
  const AmgSymMatrix(5)* trkCov = getStartCov(trkPar,updatingSign);
  if (!trkCov) return nullptr;
    
  int nLocCoord = measLocCov.cols();
  if (nLocCoord == 1) {
    return calculateFilterStep_1D (trkPar,*trkCov,
                                   measLocPos[0],measLocCov(0,0),1,
                                   updatingSign,fitQoS,false);
  } if (nLocCoord == 2) {
    return calculateFilterStep_T<2>(trkPar,*trkCov,
                                    measLocPos,measLocCov.block<2,2>(0,0),3,
                                    updatingSign,fitQoS,false);
  } 
  
  ATH_MSG_WARNING(" number (" << nLocCoord << ") of local coordinates must be 1 or 2, can not update!");
  return nullptr;
}

// updator #2 for Kalman Fitter - version with LocalParameters (MeasurementBase method)
Trk::TrackParameters* Trk::KalmanUpdatorAmg::addToState (const Trk::TrackParameters& trkPar,
                                                         const LocalParameters&  measmtPar,
                                                         const Amg::MatrixX& measmtCov) const 
{
    if (msgLvl(MSG::VERBOSE)) {logStart("addToState(TP,LPAR,ERR)",trkPar.parameters());}
    FitQualityOnSurface*    fitQoS = nullptr;
    return prepareFilterStep (trkPar, measmtPar, measmtCov, 1, fitQoS, false);
}

// updator #3 for Kalman Fitter - version with Amg::Vector2D (PrepRawData method)
Trk::TrackParameters* Trk::KalmanUpdatorAmg::addToState (const Trk::TrackParameters& trkPar,
                                                         const Amg::Vector2D& measLocPos, 
                                                         const Amg::MatrixX& measLocCov,
                                                         FitQualityOnSurface*& fitQoS) const
{
    const int updatingSign = 1;
    if (msgLvl(MSG::VERBOSE)) {logStart("addToState(TP,LPOS,ERR,FQ)",trkPar.parameters());}
    if (fitQoS) {
        ATH_MSG_WARNING("expect nil FitQuality pointer, refuse operation to avoid mem leak!");
        return nullptr;
    } 
      // get the Start covariance matrix
      const AmgSymMatrix(5)* trkCov = getStartCov(trkPar,updatingSign);
      if (!trkCov) return nullptr;
      if (msgLvl(MSG::VERBOSE)) {logInputCov(*trkCov,measLocPos,measLocCov);}
      int nLocCoord = measLocCov.cols();
      if (nLocCoord == 1) {
        return calculateFilterStep_1D (trkPar,*trkCov,
                                       measLocPos[0],measLocCov(0,0),1,
                                       updatingSign,fitQoS,true);
      } if (nLocCoord == 2) {
        return calculateFilterStep_T<2>(trkPar,*trkCov,
                                        measLocPos,measLocCov.block<2,2>(0,0),3,
                                        updatingSign,fitQoS,true);
      } 
        ATH_MSG_WARNING(" number (" << nLocCoord << ") of local coordinates must be 1 or 2, can not update!");
        return nullptr;
      
    
}

// updator #4 for Kalman Fitter - version with LocalParameters (MeasurementBase method)
Trk::TrackParameters* Trk::KalmanUpdatorAmg::addToState (const Trk::TrackParameters& trkPar,
                                                         const LocalParameters& measmtPar,
                                                         const Amg::MatrixX& measmtCov,
                                                         FitQualityOnSurface*& fitQoS) const
{
    if (msgLvl(MSG::VERBOSE)) {logStart("addToState(TP,LPAR,ERR,FQ)",trkPar.parameters());}
    if (fitQoS) {
        ATH_MSG_WARNING("expect nil FitQuality pointer, refuse operation to avoid mem leak!");
      return nullptr;
    } 
      return prepareFilterStep (trkPar, measmtPar, measmtCov, 1, fitQoS, true);
    
}

// inverse updator #1 for Kalman Fitter - version with Amg::Vector2D (PrepRawData method)
Trk::TrackParameters* Trk::KalmanUpdatorAmg::removeFromState (const Trk::TrackParameters& trkPar,
                                                              const Amg::Vector2D& measLocPos, 
                                                              const Amg::MatrixX& measLocCov) const {
    if (msgLvl(MSG::VERBOSE)) {logStart("removeFromState(TP,LPOS,ERR)",trkPar.parameters());}
    FitQualityOnSurface*    fitQoS = nullptr;
    const int updatingSign = -1;
    
    // get the Start covariance matrix
    const AmgSymMatrix(5)* trkCov = getStartCov(trkPar,updatingSign);
    if (!trkCov) return nullptr;
      
    if (msgLvl(MSG::VERBOSE)) {logInputCov(*trkCov,measLocPos,measLocCov);}

    int nLocCoord = measLocCov.cols();
    if (nLocCoord == 1) {
      return calculateFilterStep_1D (trkPar,*trkCov,
                                     measLocPos[0],measLocCov(0,0),1,
                                     updatingSign,fitQoS,false);
    } if (nLocCoord == 2) {
      return calculateFilterStep_T<2>(trkPar,*trkCov,
                                     measLocPos,measLocCov.block<2,2>(0,0),3,
                                     updatingSign,fitQoS,false);
    } 
      ATH_MSG_WARNING(" number (" << nLocCoord << ") of local coordinates must be 1 or 2, can not un-update!");
      return nullptr;
    
}

// inverse updator #2 for Kalman Fitter - version with LocalParameters (MeasurementBase method)
Trk::TrackParameters* Trk::KalmanUpdatorAmg::removeFromState (const Trk::TrackParameters& trkPar,
                                                              const LocalParameters& measmtPar,
                                                              const Amg::MatrixX& measmtCov) const {
    if (msgLvl(MSG::DEBUG)) {logStart("removeFromState(TP,LPAR,ERR)",trkPar.parameters());}
    FitQualityOnSurface*    fitQoS = nullptr;
    return prepareFilterStep (trkPar, measmtPar, measmtCov,-1,fitQoS, false);
}

// inverse updator #3 for Kalman Fitter - version with Amg::Vector2D (PrepRawData method)
Trk::TrackParameters* Trk::KalmanUpdatorAmg::removeFromState (const Trk::TrackParameters& trkPar,
                                                              const Amg::Vector2D& measLocPos, 
                                                              const Amg::MatrixX& measLocCov,
                                                              FitQualityOnSurface*& fitQoS) const {
    const int updatingSign = -1;
    if (msgLvl(MSG::VERBOSE)) {logStart("removeFromState(TP,LPOS,ERR,FQ)",trkPar.parameters());}
    if (fitQoS) {
      ATH_MSG_WARNING("expect nil FitQuality pointer, refuse operation to"
            << " avoid mem leak!");
      return nullptr;
    } 

      // get the Start covariance matrix
      const AmgSymMatrix(5)* trkCov = getStartCov(trkPar,updatingSign);
      if (!trkCov) return nullptr;      
      
      if (msgLvl(MSG::VERBOSE)) {logInputCov(*trkCov,measLocPos,measLocCov);}
      
      int nLocCoord = measLocCov.cols();
      if (nLocCoord == 1) {
        return calculateFilterStep_1D (trkPar,*trkCov,
                                       measLocPos[0],measLocCov(0,0),1,
                                       updatingSign,fitQoS,true);
      } if (nLocCoord == 2) {
        return calculateFilterStep_T<2>(trkPar,*trkCov,
                                       measLocPos,measLocCov.block<2,2>(0,0),3,
                                       updatingSign,fitQoS,true);
      } 
        ATH_MSG_WARNING(" number (" << nLocCoord << ") of local coordinates must be 1 or 2, can not un-update!");
        return nullptr;
      
    
}

// inverse updator #4 for Kalman Fitter - version with LocalParameters (MeasurementBase method)
Trk::TrackParameters* Trk::KalmanUpdatorAmg::removeFromState(const Trk::TrackParameters& trkPar,
                                                             const LocalParameters& measmtPar,
                                                             const Amg::MatrixX& measmtCov,
                                                             FitQualityOnSurface*& fitQoS) const 
{
    if (msgLvl(MSG::VERBOSE)) logStart("removeFromState(TP,LPAR,ERR,FQ)",trkPar.parameters());
    if (fitQoS) {
        ATH_MSG_WARNING("expect nil FitQuality pointer, refuse operation to avoid mem leak!");
        return nullptr;
    } 
        return prepareFilterStep (trkPar, measmtPar, measmtCov, -1, fitQoS, true);
    
}

// state-to-state updator, trajectory combination - version without fitQuality
Trk::TrackParameters* Trk::KalmanUpdatorAmg::combineStates (const Trk::TrackParameters& one,
                                                            const Trk::TrackParameters& two) const {
	// try if both Track Parameters are measured ones ?
	const AmgSymMatrix(5)* covOne = one.covariance();
	const AmgSymMatrix(5)* covTwo = two.covariance();

	// remember, either one OR two might have no error, but not both !
	if (!covOne && ! covTwo) {
		ATH_MSG_WARNING("both parameters have no errors, invalid use of Updator::combineStates()");
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
	
    FitQualityOnSurface*    fitQoS = nullptr;
    return calculateFilterStep_5D(one,*covOne,two.parameters(),*covTwo,+1,fitQoS,false);
}

// state-to-state updator, trajectory combination - version with fitQuality
Trk::TrackParameters* Trk::KalmanUpdatorAmg::combineStates (const Trk::TrackParameters& one,
                                                            const Trk::TrackParameters& two,
                                                            FitQualityOnSurface*& fitQoS) const {
    // try if both Track Parameters are measured ones ?
    const AmgSymMatrix(5)* covOne = one.covariance();
    const AmgSymMatrix(5)* covTwo = two.covariance();

	// remember, either one OR two might have no error, but not both !
	if (!covOne && ! covTwo) {
		ATH_MSG_WARNING("both parameters have no errors, invalid use of Updator::combineStates()");
		return nullptr;
	}
	if (fitQoS) {
        ATH_MSG_WARNING("expect nil FitQuality pointer, refuse operation to avoid mem leak!");
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
    return calculateFilterStep_5D(one,(*covOne),two.parameters(),(*covTwo),+1,fitQoS,true);
}

// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdatorAmg::stateFitQuality (const Trk::TrackParameters& trkPar,
                                        const Amg::Vector2D& rioLocPos,
                                        const Amg::MatrixX& rioCov,
                                        int fullPred) const 
{
   
   // try if Track Parameters are measured ones ?
   if (!trkPar.covariance()) {
      ATH_MSG_ERROR("updated track state has no error matrix, return 0.");
      return nullptr;
   }
   // For the LocalPos. version, need to get # meas. coord. from covariance matrix.
   int nLocCoord = rioCov.cols();
    if (nLocCoord == 1) {
      return makeChi2_1D(trkPar.parameters(),(*trkPar.covariance()),
                         rioLocPos[Trk::locX],rioCov(0,0), 1,fullPred); // key=1, signForUpdatedChi2 = -1
   } if (nLocCoord == 2) {
      return makeChi2_T<2>(trkPar.parameters(),(*trkPar.covariance()),
                           rioLocPos,rioCov.block<2,2>(0,0), 3,fullPred);
   } 
      ATH_MSG_WARNING("Error in local position - must be 1D or 2D!");
      return nullptr;
   
}


// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdatorAmg::predictedStateFitQuality (const Trk::TrackParameters& predPar,
                                                 const Amg::Vector2D& rioLocPos,
                                                 const Amg::MatrixX& rioCov) const 
{
   ATH_MSG_VERBOSE("--> entered KalmanUpdatorAmg::predictedStateFitQuality(TP,LPOS,ERR)");
   // ATH_MSG_VERBOSE("LPOS: "<<rioLocPos << " cov:"<<rioCov);
   return stateFitQuality(predPar, rioLocPos,rioCov, -1);
}

// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdatorAmg::fullStateFitQuality (const Trk::TrackParameters& trkPar,
                                            const Amg::Vector2D& rioLocPos,
                                            const Amg::MatrixX& rioCov) const 
{
   ATH_MSG_VERBOSE("--> entered KalmanUpdatorAmg::fullStateFitQuality(TP,LPOS,ERR)");
   return stateFitQuality(trkPar, rioLocPos,rioCov, 1);
}


// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdatorAmg::stateFitQuality (const Trk::TrackParameters& trkPar,
                                        const Trk::LocalParameters& parRio,
                                        const Amg::MatrixX& rioCov,
                                        int fullPred) const 
{
                                                
	// try if Track Parameters are measured ones ?
	if (!trkPar.covariance()) {
        ATH_MSG_ERROR("updated track state has no error matrix, return 0.");
		return nullptr;
	}
    int nLocCoord = parRio.dimension();
    if ( ! consistentParamDimensions(parRio,rioCov.cols()) ) return nullptr;
    if (nLocCoord == 1) {
      return makeChi2_1D(trkPar.parameters(),(*trkPar.covariance()),
                         parRio(0),rioCov(0,0),parRio.parameterKey(),fullPred);
    } 
    if (nLocCoord == 2) {
      return makeChi2_T<2>(trkPar.parameters(),(*trkPar.covariance()),
                           parRio.block<2,1>(0,0),rioCov.block<2,2>(0,0), parRio.parameterKey(),fullPred);
    }
    if (nLocCoord == 3) {
      return makeChi2_T<3>(trkPar.parameters(),(*trkPar.covariance()),
                           parRio.block<3,1>(0,0),rioCov.block<3,3>(0,0), parRio.parameterKey(),fullPred);
    }
    if (nLocCoord == 4) {
      return makeChi2_T<4>(trkPar.parameters(),(*trkPar.covariance()),
                           parRio.block<4,1>(0,0),rioCov.block<4,4>(0,0), parRio.parameterKey(),fullPred);
    }    
    if (nLocCoord == 5) {
      return makeChi2_T<5>(trkPar.parameters(),(*trkPar.covariance()),
                           parRio.block<5,1>(0,0),rioCov.block<5,5>(0,0), parRio.parameterKey(),fullPred);
    }
    ATH_MSG_ERROR("Dimension error in KalmanUpdatorAmg::stateFitQuality(TP,LPAR,ERR).");
    return nullptr;
}




// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdatorAmg::fullStateFitQuality (const Trk::TrackParameters& trkPar,
                                            const Trk::LocalParameters& parRio,
                                            const Amg::MatrixX& rioCov) const 
{
                                                
    ATH_MSG_VERBOSE("--> entered KalmanUpdatorAmg::fullStateFitQuality(TP,LPAR,ERR)");
    return stateFitQuality(trkPar,parRio,rioCov,-1);
}



// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdatorAmg::predictedStateFitQuality (const Trk::TrackParameters& predPar,
                                                 const Trk::LocalParameters& parRio,
                                                 const Amg::MatrixX& rioCov) const
{
    ATH_MSG_VERBOSE("--> entered KalmanUpdatorAmg::predictedStateFitQuality(TP,LPAR,ERR)");
    return stateFitQuality(predPar,parRio,rioCov,+1);
}

// estimator for FitQuality on Surface (allows for setting of LR for straws)
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdatorAmg::predictedStateFitQuality (const Trk::TrackParameters& trkParOne,
                                                 const Trk::TrackParameters& trkParTwo) const 
{
    ATH_MSG_VERBOSE("--> entered KalmanUpdatorAmg::predictedStateFitQuality(TP,TP)");
	// try if both Track Parameters are measured ones ?
    const AmgSymMatrix(5)* covOne = trkParOne.covariance();
	const AmgSymMatrix(5)* covTwo = trkParTwo.covariance();
    // remember, either one OR two might have no error, but not both !
	if (!covOne && ! covTwo) {
		ATH_MSG_WARNING("both parameters have no errors, invalid use of Updator::fitQuality()");
		return nullptr;
	}
	// if only one of two has an error, place a message.
	if (!covOne || !covTwo) {
		ATH_MSG_DEBUG("One parameter does not have uncertainties, assume initial state and return chi2=0.0");
		return new FitQualityOnSurface(0.f, 5);
	}
    return makeChi2_T<5>(trkParOne.parameters(),*covOne,
                         trkParTwo.parameters(),*covTwo, 31, +1);
}

std::vector<double> Trk::KalmanUpdatorAmg::initialErrors() const {
  std::vector<double> E(5);
  for (int i=0; i<5; ++i) E[i] = sqrt(m_cov0Vec(i));
  return E;
}

// analyse dimension of localParameters to call appropriate fast-access mathematics
Trk::TrackParameters* Trk::KalmanUpdatorAmg::prepareFilterStep (const Trk::TrackParameters& trkPar,
                                                                const Trk::LocalParameters& parRio,
                                                                const Amg::MatrixX& rioCov,
                                                                const int sign,
                                                                Trk::FitQualityOnSurface*& fitQoS,
                                                                bool createFQoS ) const {

    // get the Start covariance matrix
    const AmgSymMatrix(5)* trkCov = getStartCov(trkPar,sign);
    if (!trkCov) return nullptr;

    int nLocCoord = rioCov.cols();
    if ( ! consistentParamDimensions(parRio,nLocCoord) ) return nullptr;
    if (msgLvl(MSG::VERBOSE)) logInputCov(*trkCov,parRio,rioCov);

    if (nLocCoord==1) return calculateFilterStep_1D (trkPar,*trkCov,
                                                     parRio(0),rioCov(0,0),
                                                     parRio.parameterKey(),
                                                     sign,fitQoS,createFQoS);
    if (nLocCoord==2) return calculateFilterStep_T<2>(trkPar,*trkCov,
                                                     parRio.block<2,1>(0,0),rioCov.block<2,2>(0,0),
                                                     parRio.parameterKey(),
                                                     sign,fitQoS,createFQoS);
    if (nLocCoord==3) return calculateFilterStep_T<3>(trkPar,*trkCov,
                                                     parRio.block<3,1>(0,0),rioCov.block<3,3>(0,0),
                                                     parRio.parameterKey(),
                                                     sign,fitQoS,createFQoS);
    if (nLocCoord==4) return calculateFilterStep_T<4>(trkPar,*trkCov,
                                                     parRio.block<4,1>(0,0),rioCov.block<4,4>(0,0),
                                                     parRio.parameterKey(),
                                                     sign,fitQoS,createFQoS);
    if (nLocCoord==5) return calculateFilterStep_5D(trkPar,*trkCov,
                                                     parRio.block<5,1>(0,0),rioCov.block<5,5>(0,0),
                                                     sign,fitQoS,createFQoS);
    return nullptr;
}

// updator interface for reference-track KF on pure AMG matrices
std::pair<AmgVector(5), AmgSymMatrix(5)>* Trk::KalmanUpdatorAmg::updateParameterDifference(
    const AmgVector(5)&  trkParV, const AmgSymMatrix(5)& trkCov,
    const Amg::VectorX&  parRio, const Amg::MatrixX& covRio, const int& paramKey,
    Trk::FitQualityOnSurface*& fitQoS, bool createFQoS ) const {

    if (msgLvl(MSG::VERBOSE)) logStart("updateParDiff(TP5,Cov5,LparX,CovX)",trkParV);
    int nLocCoord = covRio.cols();
    // not the right interface // if ( ! consistentParamDimensions(parRio,nLocCoord) ) return 0;
    if (msgLvl(MSG::VERBOSE)) logInputCov(trkCov,parRio,covRio);

    if (nLocCoord==1) return calculateFilterStep_1D (trkParV,trkCov,
                                                     parRio(0),covRio(0,0),
                                                     paramKey,
                                                     fitQoS,createFQoS);
    if (nLocCoord==2) return calculateFilterStep_T<2>(trkParV,trkCov,
                                                      parRio.block<2,1>(0,0),covRio.block<2,2>(0,0),
                                                      paramKey,
                                                      fitQoS,createFQoS);
    if (nLocCoord==3) return calculateFilterStep_T<3>(trkParV,trkCov,
                                                      parRio.block<3,1>(0,0),covRio.block<3,3>(0,0),
                                                      paramKey,
                                                      fitQoS,createFQoS);
    if (nLocCoord==4) return calculateFilterStep_T<4>(trkParV,trkCov,
                                                      parRio.block<4,1>(0,0),covRio.block<4,4>(0,0),
                                                      paramKey,
                                                      fitQoS,createFQoS);
    if (nLocCoord==5) return calculateFilterStep_5D(trkParV,trkCov,
                                                    parRio.block<5,1>(0,0),covRio.block<5,5>(0,0),
                                                    fitQoS,createFQoS);
    return nullptr;

}



// calculations for Kalman updator and inverse Kalman filter
Trk::TrackParameters*
Trk::KalmanUpdatorAmg::calculateFilterStep_1D (const TrackParameters& TP, const AmgSymMatrix(5)&  trkCov,
                                               double measPar, double measCov, int paramKey,
                                               int sign,
                                               Trk::FitQualityOnSurface*& fitQoS,
                                               bool createFQoS ) const 
{
  
  // use measuring coordinate (variable "mk") instead of reduction matrix
  ATH_MSG_DEBUG("--> entered KalmanUpdatorAmg::calculateFilterStep_1D ");
  int mk=0;
  if (paramKey!=1) for (int i=0; i<5; ++i) if (paramKey & (1<<i)) { mk=i; break; }
  // get the parameters from the 
  const AmgVector(5)&  trkPar = TP.parameters();
  // use measuring coordinate (variable "mk") instead of reduction matrix
  double r = measPar - trkPar(mk);
  double R = (sign * measCov) + trkCov(mk,mk);
  if (R == 0.0) {
    ATH_MSG_DEBUG("inversion of the error-on-the-residual failed.");
    return nullptr;
  } R = 1./R;
  // --- compute Kalman gain matrix
  AmgMatrix(5,1) K = trkCov.block<5,1>(0,mk) * R;
  ATH_MSG_VERBOSE ("-U- residual            = " << r << '\n'<< "-U- inv. sigmaR         = " <<
                   R << '\n'<< "-U- gain mtx K          = " << Amg::toString(K));

  // --- compute local filtered state, here = TP+K*r = TP + TCov * H.T * R * r
  AmgVector(5) newPar = trkPar + trkCov.col(mk) * R * r;

 if (!thetaWithinRange_5D(newPar)) {
   if ( mk!=Trk::theta && (fabs(R*r)>1.0 || trkCov(Trk::theta,Trk::theta) > 0.1*m_cov0Vec(Trk::theta))) {
     ATH_MSG_DEBUG("calculateFS_1D(): decided to damp update of theta and re-calculate.");
     AmgVector(5) dampedCov = trkCov.col(mk);
     dampedCov(Trk::theta) = dampedCov(Trk::theta)*m_thetaGainDampingValue;
     newPar = trkPar + dampedCov * R * r;
     K(Trk::theta,0) = K(Trk::theta,0)*m_thetaGainDampingValue;
     ATH_MSG_DEBUG("-U- damped gain mtx K       = " << Amg::toString(K));
   } else 
     ATH_MSG_DEBUG("-U- theta out of range but can not damp this update.");
 }
  // --- compute covariance matrix of local filteres state
  AmgMatrix(5,5) KtimesH; KtimesH.setZero(); KtimesH.col(mk) = K;
  AmgMatrix(5,5) M = m_unitMatrix - KtimesH;
  ATH_MSG_DEBUG( "-U- matrix M diagonal     = " << M(0,0)<<"," << M(1,1)<<","
                  << M(2,2)<<"," << M(3,3)<<","<< M(4,4));
  AmgSymMatrix(5) newCov; newCov.setZero();
  if (!m_useFruehwirth8a) {
    // either: use formula C = M * trkCov * M.T() +/- K * rioCov * K.T()
    AmgSymMatrix(1) measuredSMatrix1D;
    measuredSMatrix1D(0,0) = measCov;
    newCov = M*trkCov*M.transpose() + sign*K*measCov*K.transpose();
  } else {
    // or: original Fruehwirth eq. 8a is simpler, expression checked to be symm.
    // C = (1 - K*H) * trkCov
    newCov = M*trkCov;
  }
  if ( (!thetaPhiWithinRange_5D(newPar, Trk::KalmanUpdatorAmg::absoluteCheck)) ?
        !correctThetaPhiRange_5D(newPar, newCov, Trk::KalmanUpdatorAmg::absoluteCheck) : false ) {
    ATH_MSG_WARNING("calculateFS_1D(): bad angles in filtered state!");
    return nullptr;
  }

  if (createFQoS) {
    double predictedResidual = (sign<0) ?  r : (measPar - newPar(mk)) ;
    const AmgSymMatrix(5)& updatedCov   = (sign<0) ? 
      trkCov : // when removing, the input are updated par
      newCov ; // when adding, chi2 is made from upd. par

    // for both signs (add/remove) the chi2 is now calculated like for updated states
    double chiSquared = measCov - updatedCov(mk,mk);
    if (chiSquared == 0.0)
        ATH_MSG_INFO("division by zero in 1D chi2, set chi2 to 0.0 instead");
    else {
      // get chi2 = r.T() * R^-1 * r
      chiSquared = predictedResidual*predictedResidual/chiSquared;
      ATH_MSG_VERBOSE("-U- fitQuality of "<< (sign>0?" added ":"removed")
                    <<" state, chi2 :" << chiSquared << " / ndof= 1");
    }
    fitQoS = new FitQualityOnSurface(chiSquared, 1);
  }
  return convertToClonedTrackPars(TP,newPar,newCov,sign,createFQoS,"1D");
}

    //!< updator maths for pure AMG matrix as needed by reference-track based KF
std::pair<AmgVector(5), AmgSymMatrix(5)>*
Trk::KalmanUpdatorAmg::calculateFilterStep_1D(const AmgVector(5)& trkPar, const AmgSymMatrix(5)& trkCov,
                                              double measPar, double measCov, int paramKey,
                                              FitQualityOnSurface*& fQ, bool createFQoS) const
{
  ATH_MSG_DEBUG("--> entered KalmanUpdatorAmg::calculateFilterStep_1D ");
  int mk=0;
  int sign=1;
  if (paramKey!=1) for (int i=0; i<5; ++i) if (paramKey & (1<<i)) { mk=i; break; }
  double r = measPar - trkPar(mk);
  double R = (sign * measCov) + trkCov(mk,mk);
  if (R == 0.0) {
    ATH_MSG_DEBUG("inversion of the error-on-the-residual failed.");
    return nullptr;
  } R = 1./R;
  // --- compute Kalman gain matrix
  AmgMatrix(5,1) K = trkCov.block<5,1>(0,mk) * R;
  ATH_MSG_VERBOSE ("-U- residual            = " << r << '\n'<< "-U- inv. sigmaR         = " <<
                   R << '\n'<< "-U- gain mtx K          = " << Amg::toString(K));
  // --- compute local filtered state, here = TP+K*r = TP + TCov * H.T * R * r
  AmgVector(5) newPar = trkPar + trkCov.col(mk) * R * r;

  if ( (newPar(Trk::theta)<-0.5*M_PI) || (newPar(Trk::theta) >0.5* M_PI)) {
    if ( mk!=Trk::theta && (fabs(R*r)>1.0 || trkCov(Trk::theta,Trk::theta) > 0.1*m_cov0Vec(Trk::theta))) {
      ATH_MSG_DEBUG("calculateFS_1D(): decided to damp update of theta and re-calculate.");
      AmgVector(5) dampedCov = trkCov.col(mk);
      dampedCov(Trk::theta) = dampedCov(Trk::theta)*m_thetaGainDampingValue;
      newPar = trkPar + dampedCov * R * r;
      K(Trk::theta,0) = K(Trk::theta,0)*m_thetaGainDampingValue;
      ATH_MSG_DEBUG("-U- damped gain mtx K       = " << Amg::toString(K));
    } else
      ATH_MSG_DEBUG("-U- theta out of range but can not damp this update.");
  }
  // --- compute covariance matrix of local filtered state
  AmgMatrix(5,5) KtimesH; KtimesH.setZero(); KtimesH.col(mk) = K;
  AmgMatrix(5,5) M = m_unitMatrix - KtimesH;
  ATH_MSG_DEBUG( "-U- matrix M diagonal     = " << M(0,0)<<"," << M(1,1)<<","
                  << M(2,2)<<"," << M(3,3)<<","<< M(4,4));
  AmgSymMatrix(5) newCov; newCov.setZero();
  if (!m_useFruehwirth8a) {
    // either: use formula C = M * trkCov * M.T() +/- K * rioCov * K.T()
    AmgSymMatrix(1) measuredSMatrix1D(measCov);
    newCov = M*trkCov*M.transpose() + sign*K*measCov*K.transpose();
  } else {
    // or: original Fruehwirth eq. 8a is simpler, expression checked to be symm.
    // C = (1 - K*H) * trkCov
   newCov = M*trkCov;
  }
  if ( (!thetaPhiWithinRange_5D(newPar, Trk::KalmanUpdatorAmg::differentialCheck)) ?
        !correctThetaPhiRange_5D(newPar, newCov, Trk::KalmanUpdatorAmg::differentialCheck) : false ) {
    ATH_MSG_WARNING("calculateFS_1D(): bad angles in filtered state!");
    return nullptr;
  }

  if (createFQoS) {
    double predictedResidual = (measPar - newPar(mk)) ;
    const AmgSymMatrix(5)& updatedCov   = newCov ; // when adding, chi2 is made from upd. par

    // for both signs (add/remove) the chi2 is now calculated like for updated states
    double chiSquared = measCov - updatedCov(mk,mk);
    if (chiSquared == 0.0)
        ATH_MSG_INFO("division by zero in 1D chi2, set chi2 to 0.0 instead");
    else {
      // get chi2 = r.T() * R^-1 * r
      chiSquared = predictedResidual*predictedResidual/chiSquared;
      ATH_MSG_VERBOSE("-U- fitQuality of "<< (" added ")
                    <<" state, chi2 :" << chiSquared << " / ndof= 1");
    }
    fQ = new FitQualityOnSurface(chiSquared, 1);
  }
  return new std::pair<AmgVector(5), AmgSymMatrix(5)>(std::make_pair(newPar,newCov));
}

Trk::TrackParameters* 
Trk::KalmanUpdatorAmg::calculateFilterStep_5D(const TrackParameters& TP, const AmgSymMatrix(5)& trkCov,
                                             const AmgVector(5)& measPar, const AmgSymMatrix(5)& measCov,
                                             int sign,
                                             FitQualityOnSurface*& fQ,
                                             bool createFQoS) const
  {
    ATH_MSG_DEBUG("--> entered KalmanUpdatorAmg::calculateFilterStep_5D ");
    // get the parameter vector
    const AmgVector(5)&  trkPar = TP.parameters();
    // Kalman gain K, residual r, combined covariance R
    AmgMatrix(5,5) K; AmgMatrix(5,5) M; 
    AmgVector(5) r; AmgSymMatrix(5) R; 
    
    // same stuff without reduction    
    r = measPar - trkPar;

    // for full safety in Eigen see http://eigen.tuxfamily.org/dox/classEigen_1_1FullPivLU.html
    R = (sign * measCov + trkCov).inverse();
    // if (!(sign * measCov + trkCov).inverseSym5(R)) return 0;
    K = trkCov * R;
    // screen output
    if (msgLvl(MSG::VERBOSE)) logGainForm (r, R, K);
    M = m_unitMatrix - K;
    // --- compute local filtered state
    AmgVector(5) newPar = trkPar + K * r;
    // --- compute filtered covariance matrix
    AmgSymMatrix(5) newCov;
    if (!m_useFruehwirth8a) {
      // compute covariance matrix of local filteres state
      //   C = M * trkCov * M.T() +/- K * covRio * K.T()
      newCov = trkCov.similarity(M) + sign*measCov.similarity(K);
    } else {
      //   C = (1 - KH) trkCov = trkCov - K*H*trkCov = KtimesH*trkCov
      newCov = M * trkCov;
    }
    if (createFQoS) {      // get chi2 = r.T() * R2^-1 * r
      // when adding, the r and R are ready to for calculating the predicted chi2
      // when removing the r and -R are ready for calculating the updated chi2.
      double  chiSquared = (sign>0) ?  r.transpose()*R*r : r.transpose()*(-R)*r;
      // create the FQSonSurface
      fQ = new FitQualityOnSurface(chiSquared, 5);
    }
    return convertToClonedTrackPars(TP,newPar,newCov,sign,createFQoS,"2D");
    
 }

std::pair<AmgVector(5), AmgSymMatrix(5)>*
Trk::KalmanUpdatorAmg::calculateFilterStep_5D(const AmgVector(5)& trkPar, const AmgSymMatrix(5)& trkCov,
                                             const AmgVector(5)& measPar, const AmgSymMatrix(5)& measCov,
                                             FitQualityOnSurface*& fQ, bool createFQoS) const
{
    int sign=1;
    ATH_MSG_DEBUG("--> entered KalmanUpdatorAmg::calculateFilterStep_5D ");
    // Kalman gain K, residual r, combined covariance R
    AmgMatrix(5,5) K; AmgMatrix(5,5) M;
    AmgVector(5) r; AmgSymMatrix(5) R;
    // same stuff without reduction    
    r = measPar - trkPar;
    R = (sign * measCov + trkCov).inverse();
    K = trkCov * R;
    // screen output
    if (msgLvl(MSG::VERBOSE)) logGainForm (r, R, K);
    M = m_unitMatrix - K;
    // --- compute local filtered state
    AmgVector(5) newPar = trkPar + K * r;
    // --- compute filtered covariance matrix
    AmgSymMatrix(5) newCov;
    if (!m_useFruehwirth8a) {
      // compute covariance matrix of local filteres state
      // C = M * trkCov * M.T() +/- K * covRio * K.T()
      newCov = trkCov.similarity(M) + sign*measCov.similarity(K);
    } else {
      //   C = (1 - KH) trkCov = trkCov - K*H*trkCov = KtimesH*trkCov
      newCov = M * trkCov;
    }
    if (createFQoS) {      // get chi2 = r.T() * R2^-1 * r
      // when adding, the r and R are ready to for calculating the predicted chi2
      // when removing the r and -R are ready for calculating the updated chi2.
      double  chiSquared = r.transpose()*R*r;
      // create the FQSonSurface
      fQ = new FitQualityOnSurface(chiSquared, 5);
    }
    return new std::pair<AmgVector(5),AmgSymMatrix(5)>(std::make_pair(newPar,newCov));
}


Trk::FitQualityOnSurface* Trk::KalmanUpdatorAmg::makeChi2_1D(const AmgVector(5)& trkPar,
                                                             const AmgSymMatrix(5)& trkCov,
                                                             double valRio, double rioCov, int paramKey,
                                                             int sign) const
{ 
  int mk=0;
  if (paramKey!=1) for (int i=0; i<5; ++i) if (paramKey & (1<<i)) { mk=i; break; }  
  // sign: -1 = updated, +1 = predicted parameters.
  double r = valRio - trkPar(mk);
  //  if (mk==3) catchPiPi;
  double chiSquared = rioCov + sign * trkCov(mk,mk);
  if (chiSquared == 0.0) {
    ATH_MSG_DEBUG( "inversion of the error-on-the-residual failed for 1D measurement, set chi2 to zero.");
    return nullptr;
  } 
    chiSquared = r*r/chiSquared;
  
  return new FitQualityOnSurface(chiSquared, 1);
}

Trk::TrackParameters* 
Trk::KalmanUpdatorAmg::convertToClonedTrackPars(const Trk::TrackParameters& TP,
                                               const AmgVector(5)& par,
                                               const AmgSymMatrix(5)& covPar,
                                               int sign,
                                               bool createFQoS,
                                               std::string_view ndtext) const 
{
  
  Trk::TrackParameters* resultPar = 
      TP.associatedSurface().createTrackParameters(par[0],par[1],par[2],par[3],par[4],new AmgSymMatrix(5)(covPar));
  // screen output
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
    logResult((std::string)reportCalledInterface, resultPar->parameters(),covPar);
  }
  
  return resultPar;
}

const AmgSymMatrix(5)* Trk::KalmanUpdatorAmg::getStartCov(const Trk::TrackParameters& inputParameters,
                                                          const int isign) const
{
  
  const AmgSymMatrix(5)* covariance = inputParameters.covariance();
  if (!covariance) {
    if (isign<0) {
      ATH_MSG_WARNING ("-U- no trkCov in fit iteration, can not calculate updated parameter state.");
      return nullptr;
    } 
      // no error given - use a huge error matrix for the time
      ATH_MSG_VERBOSE ("-U- no trkCov at input - assign large error matrix for the time being.");
      return m_covariance0;
    
  }
  return covariance;
}

bool Trk::KalmanUpdatorAmg::consistentParamDimensions(const Trk::LocalParameters& P,
                                                      const int& dimCov) const {
  if (P.dimension() != dimCov ) {
    ATH_MSG_WARNING ("Inconsistency in dimension of local coord - problem with LocalParameters object?");
    ATH_MSG_WARNING ("dim of local parameters: "<<P.dimension()<<" vs. dim of error matrix: "<<dimCov);
    ATH_MSG_INFO    ("==> refuse update or chi2 calculation");
    return false;
  } return true;
}

bool Trk::KalmanUpdatorAmg::correctThetaPhiRange_5D(AmgVector(5)& V,AmgSymMatrix(5)& M,
                                                        const KalmanUpdatorAmg::RangeCheckDef rcd) const
{
  static const AmgVector(2) thetaMin(0.0, -M_PI);
  
  // correct theta coordinate
  if ( V(Trk::theta)<thetaMin((int)rcd) || V(Trk::theta)> M_PI ) {
    ATH_MSG_DEBUG ("-U- theta angle out of range: theta= "<<V(Trk::theta)<<", phi= "<<V(Trk::phi));
    // absolute theta: repair if between -pi and +2pi.
    // differential theta: repair if between -pi and +pi
    if ( ( V(Trk::theta) < -M_PI ) ||
         ( V(Trk::theta) > (rcd==Trk::KalmanUpdatorAmg::differentialCheck ? M_PI : 2*M_PI) )
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
    M.fillSymmetric(0,3,-M(0,3)); // cov(polar,locX)
    M.fillSymmetric(1,3,-M(1,3)); // cov(polar,locY)
    M.fillSymmetric(2,3,-M(2,3)); // cov(polar,azimuthal)
    M.fillSymmetric(3,4,-M(3,4)); // cov(polar,qOverP)
    ATH_MSG_DEBUG ("-U- now use corrected value phi= "<<V(Trk::phi)<<" theta = "<<V(Trk::theta));
  }

  // correct phi coordinate if necessary
  if ( (V(Trk::phi) > M_PI) ) {
    ATH_MSG_DEBUG("-U- phi = " << V(Trk::phi));
    V(Trk::phi) = fmod(V(Trk::phi)+M_PI,2*M_PI)-M_PI;
    ATH_MSG_DEBUG(" out of range, now " << "corrected to " << V(Trk::phi));
  } else if( (V(Trk::phi)<-M_PI) ) {
    ATH_MSG_DEBUG("-U- phi = " << V(Trk::phi));
    V(Trk::phi) = fmod(V(Trk::phi)-M_PI,2*M_PI)+M_PI;
    ATH_MSG_DEBUG(" out of range, now "<< "corrected to " << V(Trk::phi));
  }
  
  return true;
}

void Trk::KalmanUpdatorAmg::logStart(const std::string& IDstring, const AmgVector(5)& tp) const
{
  ATH_MSG_DEBUG("--> entered KalmanUpdatorAmg::" << IDstring);
  ATH_MSG_VERBOSE("-U- TrkPar              = " << '\n' << Amg::toString(tp));
}


void Trk::KalmanUpdatorAmg::logInputCov(const AmgSymMatrix(5)& trkCov, const Amg::VectorX& rioPar, const Amg::MatrixX& rioCov) const
{
  ATH_MSG_DEBUG("-U- cov of trkPar       = " << '\n' << Amg::toString(trkCov));
  ATH_MSG_VERBOSE( "-U- measurement locPar  = " << '\n' << Amg::toString(rioPar) << '\n'
                   <<"-U- measurement (err)^2 = " << '\n' << Amg::toString(rioCov));
}


void Trk::KalmanUpdatorAmg::logGainForm(const Amg::MatrixX& r, const Amg::MatrixX& R, const Amg::MatrixX& K) const
{
  // again some verbose debug output showing internals of updating
  ATH_MSG_VERBOSE("-U- residual            = " << '\n' << Amg::toString(r)  << '\n'
                  << "-U- inv. sigmaR         = " << '\n' << Amg::toString(R) << '\n'
                  << "-U- gain mtx K          = " << '\n' << Amg::toString(K));
}

void Trk::KalmanUpdatorAmg::logResult(const std::string& methodName, const AmgVector(5)& par, const AmgSymMatrix(5)& covPar) const
{
    // again some verbose debug output
    ATH_MSG_VERBOSE("-U- ==> result for KalmanUpdatorAmg::"<< methodName <<'\n'
                    << "-U- new par           = " << '\n' << Amg::toString(par)    << '\n'
                    << "-U- new cov           = " << '\n' << Amg::toString(covPar));
}
