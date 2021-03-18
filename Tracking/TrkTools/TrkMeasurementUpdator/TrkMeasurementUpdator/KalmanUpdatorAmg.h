/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanUpdatorAmg.h
//   Header file for Kalman Fitter Updator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Andreas.Salzburger@cern.ch, Markus.Elsing@cern.ch, Wolfgang.Liebig@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_KALMANUPDATORAMG_H
#define TRK_KALMANUPDATORAMG_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/ProjectionMatricesSet.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/IUpdator.h"
// Eigen/Amg
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include <string_view>

namespace Trk {

/** @class KalmanUpdatorAmg

    @brief Implementation of Trk::IUpdator based on gain formalism and Eigen
   mathlib.

    Tool providing calculations on track states, i.e. to add or
    remove a measured hit to the state vector.
    Contains the core maths for Kalman filters in ATLAS track reconstruction.
    The track state manipulations in this tool make use of the Kalman
    gain-matrix formalism .

    based on KalmanUpdatorSMatrix from:
    @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
 */
class KalmanUpdatorAmg
  : virtual public IUpdator
  , public AthAlgTool
{

  enum RangeCheckDef
  {
    absoluteCheck = 0,
    differentialCheck = 1
  };

public:
  //!< AlgTool standard constuctor
  KalmanUpdatorAmg(const std::string&, const std::string&, const IInterface*);
  //!<< AlgTool standard destructor
  ~KalmanUpdatorAmg();

  //!< AlgTool initialisation
  virtual StatusCode initialize() override final;
  //!< AlgTool termination
  virtual StatusCode finalize() override final;

  //!< measurement updator for the KalmanFitter getting the meas't coord' from
  //!< Amg::Vector2D (use eg with PRD)
  // fails: @copydoc Trk::IUpdator::addToState(const TrackParameters&, const
  // Amg::Vector2D&, const Amg::MatrixX&)
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  //!< measurement updator for the KalmanFitter getting the coord' from
  // LocalParameters (use for example with MeasurementBase, ROT)
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  //!< measurement updator interface for the KalmanFitter returning the fit
  //!< quality of the state at the same time (Amg::Vector2D-version)
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;
  //!< measurement updator interface for the KalmanFitter returning the fit
  //!< quality of the state at the same time (LocalParameters-version)
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;

  //!< reverse update eg for track property analysis (unbiased residuals)
  //!< getting the measurement coordinates from the Amg::Vector2D class.
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  //!< reverse update eg for track property analysis (unbiased residuals)
  //!< getting the
  // measurement coordinates from the LocalParameters class.
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  //!< reverse update for Kalman filters and other applications using the
  //!< interface with Amg::Vector2D and FitQualityOnSurface.
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;
  //!< reverse update for Kalman filters and other applications using the
  //!< interface with LocalParameters and FitQualityOnSurface.
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;

  /** @brief trajectory state updator which combines two parts of a trajectory
     on a common surface.

      Make sure that the TPs' surfaces are identical and
      that the local hit is not duplicated in both trajectories!
  */
  virtual std::unique_ptr<TrackParameters> combineStates(
    const TrackParameters&,
    const TrackParameters&) const override final;
  /** @brief trajectory state updator which combines two parts of a trajectory
     on a common surface and provides the FitQuality.

      Make sure that the TPs' surfaces are identical and that the local hit is
     not duplicated!*/
  virtual std::unique_ptr<TrackParameters> combineStates(
    const TrackParameters&,
    const TrackParameters&,
    FitQualityOnSurface*&) const override final;

  //!< estimator for FitQuality on Surface from a full track state, that is a
  //!< state which contains the current hit (expressed as Amg::Vector2D).
  virtual const FitQualityOnSurface* fullStateFitQuality(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  //!< estimator for FitQuality on Surface from a full track state, that is a
  //!< state which contains the current hit (expressed as LocalParameters).
  virtual const FitQualityOnSurface* fullStateFitQuality(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  //!< estimator for FitQuality on Surface from a predicted track state, that is
  //!< a state which contains the current hit (expressed as Amg::Vector2D).
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  //!< estimator for FitQuality on Surface from a predicted track state, that is
  //!< a state which contains the current hit (expressed as LocalParameters).
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  //!< estimator for FitQuality on Surface for the situation when a track is
  //!< fitted to the parameters of another trajectory part extrapolated to the
  //!< common surface.
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const TrackParameters&) const override final;
  //!< interface for reference-track KF
  virtual std::pair<AmgVector(5), AmgSymMatrix(5)>* updateParameterDifference(
    const AmgVector(5) &,
    const AmgSymMatrix(5) &,
    const Amg::VectorX&,
    const Amg::MatrixX&,
    const int&,
    Trk::FitQualityOnSurface*&,
    bool) const override final;

  //!< give back how updator is configured for inital covariances
  virtual std::vector<double> initialErrors() const override final;

private:
  //!< common code analysing the measurement's rank and calling the appropriate
  //!< implementation for this rank.
  std::unique_ptr<TrackParameters> prepareFilterStep(const TrackParameters&,
                                                     const LocalParameters&,
                                                     const Amg::MatrixX&,
                                                     const int,
                                                     FitQualityOnSurface*&,
                                                     bool) const;

  //!< estimator for FitQuality on Surface from a full/predicted track state,
  //!< that is a state which contains the current hit (expressed as
  //!< Amg::Vector2D).
  const FitQualityOnSurface* stateFitQuality(const TrackParameters&,
                                             const Amg::Vector2D&,
                                             const Amg::MatrixX&,
                                             int predFull) const;
  //!< estimator for FitQuality on Surface from a full/predicted track state,
  //!< that is a state which contains the current hit (expressed as
  //!< LocalParameters).
  const FitQualityOnSurface* stateFitQuality(const TrackParameters&,
                                             const LocalParameters&,
                                             const Amg::MatrixX&,
                                             int predFull) const;

  //!< common maths calculation code for all addToState and removeFromState
  //!< versions which happen to be called with 1-dim measurements.
  std::unique_ptr<TrackParameters> calculateFilterStep_1D(
    const TrackParameters& TP,
    const AmgSymMatrix(5) & trkCov,
    double measPar,
    double measCov,
    int paramKey,
    int updateDirection,
    FitQualityOnSurface*& fQ,
    bool) const;
  //!< updator maths for pure AMG matrix as needed by reference-track based KF
  std::pair<AmgVector(5), AmgSymMatrix(5)>* calculateFilterStep_1D(
    const AmgVector(5) &,
    const AmgSymMatrix(5) &,
    double measPar,
    double measCov,
    int paramKey,
    FitQualityOnSurface*&,
    bool) const;

  //!< common maths calculation code for all addToState and removeFromState
  //!< versions which happen to be called with (2-4)-dim measurements.
  template<int DIM>
  std::unique_ptr<TrackParameters> calculateFilterStep_T(
    const TrackParameters& TP,
    const AmgSymMatrix(5) & trkCov,
    const AmgVector(DIM) & measPar,
    const AmgSymMatrix(DIM) & measCov,
    int paramKey,
    int updateDirection,
    FitQualityOnSurface*& fQ,
    bool) const;
  //!< updator maths for pure AMG matrix as needed by reference-track based KF,
  //!< 2-4 dim case
  template<int DIM>
  std::pair<AmgVector(5), AmgSymMatrix(5)>* calculateFilterStep_T(
    const AmgVector(5) &,
    const AmgSymMatrix(5) &,
    const AmgVector(DIM) & measPar,
    const AmgSymMatrix(DIM) & measCov,
    int paramKey,
    FitQualityOnSurface*&,
    bool) const;

  //!< common maths calculation code for all addToState and removeFromState
  //!< versions which happen to be called with (2-4)-dim measurements.
  std::unique_ptr<TrackParameters> calculateFilterStep_5D(
    const TrackParameters& TP,
    const AmgSymMatrix(5) & trkCov,
    const AmgVector(5) & measPar,
    const AmgSymMatrix(5) & measCov,
    int updateDirection,
    FitQualityOnSurface*& fQ,
    bool) const;

  //!< updator maths for pure AMG matrix as needed by reference-track based KF,
  //!< 5-dim case
  std::pair<AmgVector(5), AmgSymMatrix(5)>* calculateFilterStep_5D(
    const AmgVector(5) &,
    const AmgSymMatrix(5) &,
    const AmgVector(5) &,
    const AmgSymMatrix(5) &,
    FitQualityOnSurface*&,
    bool) const;

  //!< Avoid multiplications with sparse H matrices by cutting 2D rows&columns
  //!< out of the full cov matrix.
  template<int DIM>
  AmgSymMatrix(DIM) projection_T(const AmgSymMatrix(5) &, const int&) const;

  /** also the chi2 calculation and FitQuality object creation is
      combined in an extra method. It is called by all the XXX-FitQuality()
      methods
      The sign controls the calculation in case a predicted input track state
      (sign=+1) or smoothed/updated input track state (sign=-1).
  */
  //!< make chi2 object for 1D, int is the accessor in the 5-par vector
  FitQualityOnSurface* makeChi2_1D(const AmgVector(5) & trkPar,
                                   const AmgSymMatrix(5) & trkCov,
                                   double measPar,
                                   double measCov,
                                   int accessor,
                                   int updateDirection) const;
  //!< make chi2 object for 2D-4D, int is the paramkey
  template<int DIM>
  FitQualityOnSurface* makeChi2_T(const AmgVector(5) & trkPar,
                                  const AmgSymMatrix(5) & trkCov,
                                  const AmgVector(DIM) &,
                                  const AmgSymMatrix(DIM) &,
                                  int paramKey,
                                  int updateDirection) const;

  //!< Helper method to transform Eigen cov matrix to SMatrix.
  const AmgSymMatrix(5) * getStartCov(const TrackParameters&, const int) const;

  //!< Helper method to convert internal results from SMatrix to Eigen. */
  std::unique_ptr<TrackParameters> convertToClonedTrackPars(
    const TrackParameters&,
    const AmgVector(5) &,
    const AmgSymMatrix(5) &,
    int,
    bool,
    std::string_view) const;

  // === note: any of the following log... method is only called if
  // the msgstream level has been set appropriately.
  //!< internal structuring: debugging output for start of method.
  void logStart(const std::string&, const AmgVector(5) &) const;
  //!< internal structuring: common logfile output of the inputs
  void logInputCov(const AmgSymMatrix(5) &,
                   const Amg::VectorX&,
                   const Amg::MatrixX&) const;
  //!< internal structuring: common logfile output during calculation
  void logGainForm(const Amg::MatrixX&,
                   const Amg::MatrixX&,
                   const Amg::MatrixX&) const;
  //!< internal structuring: common logfile output after calculation
  void logResult(const std::string&,
                 const AmgVector(5) &,
                 const AmgSymMatrix(5) &) const;

  //!< method testing correct use of LocalParameters
  bool consistentParamDimensions(const LocalParameters&, const int&) const;

  //!< Test if angles are inside boundaries.
  /** Absolute phi values should be in [-pi, pi] (how about endpoints?)
      absolute theta values should be in [0, +pi]
      phi differences should also be in [-pi, pi] - else go other way round,
      theta differences should be smaller than pi but can be negative
            => other constraint than absolute theta.
  */
  bool thetaPhiWithinRange_5D(const AmgVector(5) & V,
                              const KalmanUpdatorAmg::RangeCheckDef rcd) const;

  //!< Test if theta angle is inside boundaries. No differential-check option.
  bool thetaWithinRange_5D(const AmgVector(5) & V) const;

  //!< method correcting the calculated angles back to their defined ranges phi
  //!< (-pi, pi) and theta (0, pi).
  /** Only works if the excess is not far from the defined range, as it
      happens e.g. when the update moves across the phi= +/-pi boundary. */
  bool correctThetaPhiRange_5D(AmgVector(5) &,
                               AmgSymMatrix(5) &,
                               const KalmanUpdatorAmg::RangeCheckDef) const;

  std::vector<double> m_cov_stdvec; //!<< job options for initial cov values
  AmgVector(5) m_cov0Vec;           //!<< initial cov values in AmgVector object
  AmgSymMatrix(5) * m_covariance0;  //!<< initial cov values in AmgMatrix
  bool m_useFruehwirth8a; //!<< job options controlling update formula for
                          //!< covariance matrix
  float m_thetaGainDampingValue;

  AmgSymMatrix(5) m_unitMatrix; //!<< avoid mem allocation at every call

  ProjectionMatricesSet m_reMatrices; //!< expansion and reduction matrices set

  static const ParamDefsAccessor s_enumAccessor;
};

template<int DIM>
std::unique_ptr<TrackParameters>
KalmanUpdatorAmg::calculateFilterStep_T(const TrackParameters& TP,
                                        const AmgSymMatrix(5) & trkCov,
                                        const AmgVector(DIM) & measPar,
                                        const AmgSymMatrix(DIM) & measCov,
                                        int paramKey,
                                        int sign,
                                        FitQualityOnSurface*& fQ,
                                        bool createFQoS) const
{

  ATH_MSG_DEBUG("--> entered KalmanUpdatorAmg::calculateFilterStep_T< " << DIM
                                                                        << ">");
  // get the parameter vector
  const AmgVector(5)& trkPar = TP.parameters();
  // reduction matrix H, Kalman gain K, residual r, combined covariance R
  AmgMatrix(DIM, 5) H;
  AmgMatrix(5, DIM) K;
  AmgMatrix(5, 5) M;
  AmgVector(DIM) r;
  AmgSymMatrix(DIM) R;
  // reduction matrix
  H = m_reMatrices.expansionMatrix(paramKey).block<DIM, 5>(0, 0);
  // the projected parameters from the TrackParameters
  AmgVector(DIM) projTrkPar;
  if (paramKey == 3 || paramKey == 7 || paramKey == 15)
    projTrkPar = trkPar.block<DIM, 1>(0, 0);
  else
    projTrkPar = H * trkPar;
  // residual after reduction
  r = measPar - projTrkPar;
  // combined covariance after reduction
  R = (sign * measCov + projection_T<DIM>(trkCov, paramKey)).inverse();
  // Kalman gain matrix
  K = trkCov * H.transpose() * R;
  // screen output
  M = m_unitMatrix - K * H;
  if (msgLvl(MSG::VERBOSE)) {
    logGainForm(r, R, K);
  }
  // --- compute local filtered state
  AmgVector(5) newPar = trkPar + K * r;
  // --- compute filtered covariance matrix
  AmgSymMatrix(5) newCov;
  if (!m_useFruehwirth8a) {
    // compute covariance matrix of local filteres state
    //   C = M * trkCov * M.T() +/- K * covRio * K.T()
    newCov = M * trkCov * M.transpose() + sign * K * measCov * K.transpose();
  } else {
    //   C = (1 - KH) trkCov = trkCov - K*H*trkCov = KtimesH*trkCov
    newCov = M * trkCov;
  }
  if (createFQoS) { // get chi2 = r.T() * R2^-1 * r
    // when adding, the r and R are ready to for calculating the predicted chi2
    // when removing the r and -R are ready for calculating the updated chi2.
    double chiSquared =
      (sign > 0) ? r.transpose() * R * r : r.transpose() * (-R) * r;
    // create the FQSonSurface
    fQ = new FitQualityOnSurface(chiSquared, DIM);
  }
  return convertToClonedTrackPars(TP, newPar, newCov, sign, createFQoS, "2D");
}

template<int DIM>
std::pair<AmgVector(5), AmgSymMatrix(5)>*
KalmanUpdatorAmg::calculateFilterStep_T(const AmgVector(5) & trkPar,
                                        const AmgSymMatrix(5) & trkCov,
                                        const AmgVector(DIM) & measPar,
                                        const AmgSymMatrix(DIM) & measCov,
                                        int paramKey,
                                        FitQualityOnSurface*& fQ,
                                        bool createFQ) const
{
  ATH_MSG_DEBUG("--> entered KalmanUpdatorAmg::calculateFilterStep_T< " << DIM
                                                                        << ">");
  // reduction matrix H, Kalman gain K, residual r, combined covariance R
  AmgMatrix(DIM, 5) H;
  AmgMatrix(5, DIM) K;
  AmgMatrix(5, 5) M;
  AmgVector(DIM) r;
  AmgSymMatrix(DIM) R;
  int sign = 1;
  // reduction matrix
  H = m_reMatrices.expansionMatrix(paramKey).block<DIM, 5>(0, 0);
  // the projected parameters from the TrackParameters
  AmgVector(DIM) projTrkPar;
  if (paramKey == 3 || paramKey == 7 || paramKey == 15)
    projTrkPar = trkPar.block<DIM, 1>(0, 0);
  else
    projTrkPar = H * trkPar;
  // residual after reduction
  r = measPar - projTrkPar;
  // combined covariance after reduction
  R = (sign * measCov + projection_T<DIM>(trkCov, paramKey)).inverse();
  // Kalman gain matrix
  K = trkCov * H.transpose() * R;
  // screen output
  M = m_unitMatrix - K * H;
  if (msgLvl(MSG::VERBOSE)) {
    logGainForm(r, R, K);
  }
  // --- compute local filtered state
  AmgVector(5) newPar = trkPar + K * r;
  // --- compute filtered covariance matrix
  AmgSymMatrix(5) newCov;
  if (!m_useFruehwirth8a) {
    // compute covariance matrix of local filtered state
    //   C = M * trkCov * M.T() +/- K * covRio * K.T()
    newCov = M * trkCov * M.transpose() + sign * K * measCov * K.transpose();
  } else {
    //   C = (1 - KH) trkCov = trkCov - K*H*trkCov = KtimesH*trkCov
    newCov = M * trkCov;
  }
  if (createFQ) { // get chi2 = r.T() * R2^-1 * r
    // when adding, the r and R are ready to for calculating the predicted chi2
    // when removing the r and -R are ready for calculating the updated chi2.
    double chiSquared = r.transpose() * R * r;
    // create the FQSonSurface
    fQ = new FitQualityOnSurface(chiSquared, DIM);
  }
  return new std::pair<AmgVector(5), AmgSymMatrix(5)>(
    std::make_pair(newPar, newCov));
}

template<int DIM>
AmgSymMatrix(DIM) Trk::KalmanUpdatorAmg::projection_T(const AmgSymMatrix(5) & M,
                                                      const int& key) const
{
  if (key == 3 || key == 7 ||
      key == 15) { // shortcuts for the most common use cases
    return M.block<DIM, DIM>(0, 0);
  } else {
    Eigen::Matrix<int, DIM, 1, 0, DIM, 1> iv;
    iv.setZero();
    for (int i = 0, k = 0; i < 5; ++i) {
      if (key & (1 << i))
        iv[k++] = i;
    }
    AmgSymMatrix(DIM) covSubMatrix;
    covSubMatrix.setZero();
    for (int i = 0; i < DIM; ++i) {
      for (int j = 0; j < DIM; ++j) {
        covSubMatrix(i, j) = M(iv(i), iv(j));
      }
    }
    return covSubMatrix;
  }
}

template<int DIM>
Trk::FitQualityOnSurface*
Trk::KalmanUpdatorAmg::makeChi2_T(const AmgVector(5) & trkPar,
                                  const AmgSymMatrix(5) & trkCov,
                                  const AmgVector(DIM) & measPar,
                                  const AmgSymMatrix(DIM) & covPar,
                                  int paramKey,
                                  int sign) const
{ // sign: -1 = updated, +1 = predicted parameters.

  // make reduction matrix H from LocalParameters
  AmgMatrix(DIM, 5) H =
    m_reMatrices.expansionMatrix(paramKey).block<DIM, 5>(0, 0);
  // not get the residual
  ATH_MSG_VERBOSE("-U- makeChi2<"
                  << DIM << ">: H=" << H << " is "
                  << m_reMatrices.expansionMatrix(paramKey).rows() << "x"
                  << m_reMatrices.expansionMatrix(paramKey).cols()
                  << " for paramkey " << paramKey);
  AmgVector(DIM) r = measPar - H * trkPar;
  // get the projected matrix
  AmgSymMatrix(DIM) R = sign * projection_T<DIM>(trkCov, paramKey);
  R += covPar;
  // calcualte the chi2 value
  double chiSquared = 0.0;
  if (R.determinant() == 0.0) {
    ATH_MSG_DEBUG("matrix inversion not possible, set chi2 to zero");
  } else {
    chiSquared = r.transpose() * R.inverse() * r;
  }
  return new FitQualityOnSurface(chiSquared, DIM);
}

inline bool
KalmanUpdatorAmg::thetaPhiWithinRange_5D(
  const AmgVector(5) & V,
  const KalmanUpdatorAmg::RangeCheckDef rcd) const
{
  static const AmgVector(2) thetaMin(0.0, -M_PI);
  return ((fabs(V(Trk::phi)) <= M_PI) &&
          (V(Trk::theta) >= thetaMin((int)rcd)) && (V(Trk::theta) <= M_PI));
}

inline bool
KalmanUpdatorAmg::thetaWithinRange_5D(const AmgVector(5) & V) const
{
  return (V(Trk::theta) >= 0.0 && (V(Trk::theta) <= M_PI));
}

} // end of namespace

#endif // TRK_KALMANUPDATORAMG_H
