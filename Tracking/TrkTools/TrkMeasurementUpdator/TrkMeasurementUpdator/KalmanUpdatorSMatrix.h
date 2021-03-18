/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanUpdatorSMatrix.h
//   Header file for Kalman Fitter Updator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Markus.Elsing@cern.ch, Wolfgang.Liebig@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_KALMANUPDATORSMATRIX_H
#define TRK_KALMANUPDATORSMATRIX_H

#if __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-overflow"
#if !defined(__clang__) && __GNUC__ >= 6
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif
#endif

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/IUpdator.h"
// Eigen/Amg
#include "EventPrimitives/EventPrimitives.h"
// ROOT SMatrix lin alg: for internal calculations
#include "Math/SMatrix.h"
#include "Math/SVector.h"
#include <cmath>
#include <string_view>

typedef ROOT::Math::SMatrix<double, 1, 1, ROOT::Math::MatRepSym<double, 1>>
  SCovMatrix1;
typedef ROOT::Math::SMatrix<double, 2, 2, ROOT::Math::MatRepSym<double, 2>>
  SCovMatrix2;
typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3>>
  SCovMatrix3;
typedef ROOT::Math::SMatrix<double, 4, 4, ROOT::Math::MatRepSym<double, 4>>
  SCovMatrix4;
typedef ROOT::Math::SMatrix<double, 5, 5, ROOT::Math::MatRepSym<double, 5>>
  SCovMatrix5;
typedef ROOT::Math::SMatrix<double, 2, 2, ROOT::Math::MatRepStd<double, 2, 2>>
  SGenMatrix2;
typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepStd<double, 3, 3>>
  SGenMatrix3;
typedef ROOT::Math::SMatrix<double, 4, 4, ROOT::Math::MatRepStd<double, 4, 4>>
  SGenMatrix4;
typedef ROOT::Math::SMatrix<double, 5, 5, ROOT::Math::MatRepStd<double, 5, 5>>
  SGenMatrix5;
typedef ROOT::Math::SVector<double, 2> SParVector2;
typedef ROOT::Math::SVector<double, 3> SParVector3;
typedef ROOT::Math::SVector<double, 4> SParVector4;
typedef ROOT::Math::SVector<double, 5> SParVector5;

namespace Trk {

enum RangeCheckDef
{
  absoluteCheck = 0,
  differentialCheck = 1
};

/** @class KalmanUpdatorSMatrix

    @brief Implementation of Trk::IUpdator based on gain formalism and SMatrix
   mathlib.

    Tool providing calculations on track states, i.e. to add or
    remove a measured hit to the state vector.
    Contains the core maths for Kalman filters in ATLAS track reconstruction.
    The track state manipulations in this tool make use of the Kalman
    gain-matrix formalism .

    @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
 */
class KalmanUpdatorSMatrix
  : virtual public IUpdator
  , public AthAlgTool
{
public:
  //! AlgTool standard constuctor
  KalmanUpdatorSMatrix(const std::string&,
                       const std::string&,
                       const IInterface*);
  virtual ~KalmanUpdatorSMatrix();

  //! AlgTool initialisation
  virtual StatusCode initialize() override;
  //! AlgTool termination
  virtual StatusCode finalize() override;

  //! measurement updator for the KalmanFitter getting the meas't coord' from
  //! Amg::Vector2D (use eg with PRD)
  // fails: @copydoc Trk::IUpdator::addToState(const TrackParameters&, const
  // Amg::Vector2D&, const Amg::MatrixX&)
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  //! measurement updator for the KalmanFitter getting the coord' from
  //! LocalParameters (use for example with MeasurementBase, ROT)
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  //! measurement updator interface for the KalmanFitter returning the fit
  //! quality of the state at the same time (Amg::Vector2D-version)
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;
  //! measurement updator interface for the KalmanFitter returning the fit
  //! quality of the state at the same time (LocalParameters-version)
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;

  //! reverse update eg for track property analysis (unbiased residuals) getting
  //! the measurement coordinates from the Amg::Vector2D class.
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  //! reverse update eg for track property analysis (unbiased residuals) getting
  //! the measurement coordinates from the LocalParameters class.
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  //! reverse update for Kalman filters and other applications using the
  //! interface with Amg::Vector2D and FitQualityOnSurface.
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;
  //! reverse update for Kalman filters and other applications using the
  //! interface with LocalParameters and FitQualityOnSurface.
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
     not duplicated!
  */
  virtual std::unique_ptr<TrackParameters> combineStates(
    const TrackParameters&,
    const TrackParameters&,
    FitQualityOnSurface*&) const override final;

  //! estimator for FitQuality on Surface from a full track state, that is a
  //! state which contains the current hit (expressed as Amg::Vector2D).
  virtual const FitQualityOnSurface* fullStateFitQuality(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  //! estimator for FitQuality on Surface from a full track state, that is a
  //! state which contains the current hit (expressed as LocalParameters).
  virtual const FitQualityOnSurface* fullStateFitQuality(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  //! estimator for FitQuality on Surface from a predicted track state, that is
  //! a state which contains the current hit (expressed as Amg::Vector2D).
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  //! estimator for FitQuality on Surface from a predicted track state, that is
  //! a state which contains the current hit (expressed as LocalParameters).
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  //! estimator for FitQuality on Surface for the situation when a track is
  //! fitted to the parameters of another trajectory part extrapolated to the
  //! common surface.
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const TrackParameters&) const override final;
  //! interface for reference-track KF, not implemented
  virtual std::pair<AmgVector(5), AmgSymMatrix(5)>* updateParameterDifference(
    const AmgVector(5) &,
    const AmgSymMatrix(5) &,
    const Amg::VectorX&,
    const Amg::MatrixX&,
    const int&,
    Trk::FitQualityOnSurface*&,
    bool) const override final
  {
    return nullptr;
  }

  //! give back how updator is configured for inital covariances
  virtual std::vector<double> initialErrors() const override final;

private:
  //! common code analysing the measurement's rank and calling the appropriate
  //! implementation for this rank.
  std::unique_ptr<TrackParameters> prepareFilterStep(const TrackParameters&,
                                                     const LocalParameters&,
                                                     const Amg::MatrixX&,
                                                     const int,
                                                     FitQualityOnSurface*&,
                                                     bool) const;
  //! common maths calculation code for all addToState and removeFromState
  //! versions which happen to be called with 1-dim measurements.
  std::unique_ptr<TrackParameters> calculateFilterStep_1D(
    const TrackParameters&,
    const SParVector5&,
    const SCovMatrix5&,
    const double&,
    const int&,
    const Amg::MatrixX&,
    const int,
    FitQualityOnSurface*&,
    bool) const;
  //! common maths calculation code for all addToState and removeFromState
  //! versions which happen to be called with 2-dim measurements.
  std::unique_ptr<TrackParameters> calculateFilterStep_2D(
    const TrackParameters&,
    const SParVector5&,
    const SCovMatrix5&,
    const SParVector2&,
    const int&,
    const Amg::MatrixX&,
    const int,
    FitQualityOnSurface*&,
    bool) const;
  //! common maths calculation code for all addToState and removeFromState
  //! versions which happen to be called with 3-dim measurements.
  std::unique_ptr<TrackParameters> calculateFilterStep_3D(
    const TrackParameters&,
    const SParVector5&,
    const SCovMatrix5&,
    const LocalParameters&,
    const Amg::MatrixX&,
    const int,
    FitQualityOnSurface*&,
    bool) const;
  //! common maths calculation code for all addToState and removeFromState
  //! versions which happen to be called with 4-dim measurements.
  std::unique_ptr<TrackParameters> calculateFilterStep_4D(
    const TrackParameters&,
    const SParVector5&,
    const SCovMatrix5&,
    const LocalParameters&,
    const Amg::MatrixX&,
    const int,
    FitQualityOnSurface*&,
    bool) const;
  //! common maths calculation code for all addToState and removeFromState
  //! versions which happen to be called with 5-dim measurements or two track
  //! states.
  /** For 5-dim track states the ParameterKey is known to be 31 and
      does not need to be passed through the interface. */
  std::unique_ptr<TrackParameters> calculateFilterStep_5D(
    const TrackParameters&,
    const SParVector5&,
    const SCovMatrix5&,
    const SParVector5&,
    const Amg::MatrixX&,
    const int,
    FitQualityOnSurface*&,
    bool) const;

  //! Helper method to transform Eigen cov matrix to SMatrix.
  bool getStartCov(SCovMatrix5&, const TrackParameters&, const int) const;
  //! Helper method to convert internal results from SMatrix to Eigen. */
  std::unique_ptr<TrackParameters> convertToClonedTrackPars(
    const TrackParameters&,
    const SParVector5&,
    const SCovMatrix5&,
    const int&,
    const bool&,
    std::string_view) const;
  /** also the chi2 calculation and FitQuality object creation is
      combined in an extra method. It is called by all the XXX-FitQuality()
      methods - SMatrix version for 1D, 2D, 5D and Eigen for 3D, 4D.
      The calculateFilterStep() have the code duplicated to avoid
      re-computing the residual-error matrix.
      The sign controls the calculation in case a predicted input track state
      (sign=+1) or smoothed/updated input track state (sign=-1).
  */
  FitQualityOnSurface* makeChi2_1D(const SParVector5&,
                                   const Amg::MatrixX&,
                                   const double&,
                                   const double&,
                                   const int&,
                                   const int&) const;
  FitQualityOnSurface* makeChi2_2D(const SParVector5&,
                                   const Amg::MatrixX&,
                                   const SParVector2&,
                                   const SCovMatrix2&,
                                   const int&,
                                   const int&) const;
  FitQualityOnSurface* makeChi2_5D(const SParVector5&,
                                   const Amg::MatrixX&,
                                   const SParVector5&,
                                   const Amg::MatrixX&,
                                   const int&) const;
  FitQualityOnSurface* makeChi2Object(const Amg::VectorX&,
                                      const Amg::MatrixX&,
                                      const Amg::MatrixX&,
                                      const Amg::MatrixX&,
                                      const int&) const;

  //! Avoid multiplications with sparse H matrices by cutting 2D rows&columns
  //! out of the full cov matrix.
  SCovMatrix2 projection_2D(const SCovMatrix5&, const int&) const;
  //! Avoid multiplications with sparse H matrices by cutting 2D rows&columns
  //! out of the full cov matrix.
  SCovMatrix2 projection_2D(const Amg::MatrixX&, const int&) const;
  //! Avoid multiplications with sparse H matrices by cutting 3D rows&columns
  //! out of the full cov matrix.
  SCovMatrix3 projection_3D(const SCovMatrix5&, const int&) const;
  //! Avoid multiplications with sparse H matrices by cutting 4D rows&columns
  //! out of the full cov matrix.
  SCovMatrix4 projection_4D(const SCovMatrix5&, const int&) const;

  // === note: any of the following log... method is only called if
  // the msgstream level has been set appropriately.
  //! internal structuring: debugging output for start of method.
  void logStart(const std::string&, const TrackParameters&) const;
  //! internal structuring: common logfile output of the inputs
  void logInputCov(const SCovMatrix5&,
                   const Amg::VectorX&,
                   const Amg::MatrixX&) const;
  //! internal structuring: common logfile output during calculation
  void logGainForm(int,
                   const SParVector5&,
                   const SCovMatrix5&,
                   const SGenMatrix5&) const;
  //! internal structuring: common logfile output after calculation
  void logResult(const std::string&,
                 const AmgVector(5) &,
                 const AmgSymMatrix(5) &) const;

  //! method testing correct use of LocalParameters
  bool consistentParamDimensions(const LocalParameters&, const int&) const;

  //! Test if angles are inside boundaries.
  /** Absolute phi values should be in [-pi, pi] (how about endpoints?)
      absolute theta values should be in [0, +pi]
      phi differences should also be in [-pi, pi] - else go other way round,
      theta differences should be smaller than pi but can be negative
            => other constraint than absolute theta.
  */
  bool thetaPhiWithinRange_5D(const SParVector5& V,
                              const RangeCheckDef rcd) const;

  //! Test if theta angle is inside boundaries. No differential-check option.
  bool thetaWithinRange_5D(const SParVector5& V) const;

  //! method correcting the calculated angles back to their defined ranges phi
  //! (-pi, pi) and theta (0, pi).
  /** Only works if the excess is not far from the defined range, as it
      happens e.g. when the update moves across the phi= +/-pi boundary. */
  bool correctThetaPhiRange_5D(SParVector5&,
                               SCovMatrix5&,
                               const RangeCheckDef) const;

  std::vector<double> m_cov_stdvec; //!< job options for initial cov values
  SParVector5 m_cov0;               //!< initial cov values in SMatrix object
  bool m_useFruehwirth8a; //!< job options controlling update formula for
                          //!< covariance matrix
  float m_thetaGainDampingValue;

  SCovMatrix5 m_unitMatrix; //!< avoid mem allocation at every call
  static const ParamDefsAccessor s_enumAccessor;
};
inline bool
KalmanUpdatorSMatrix::thetaPhiWithinRange_5D(const SParVector5& V,
                                             const RangeCheckDef rcd) const
{
  static const SParVector2 thetaMin(0.0, -M_PI);
  return ((std::abs(V(Trk::phi)) <= M_PI) &&
          (V(Trk::theta) >= thetaMin((int)rcd)) && (V(Trk::theta) <= M_PI));
}

inline bool
KalmanUpdatorSMatrix::thetaWithinRange_5D(const SParVector5& V) const
{
  return (V(Trk::theta) >= 0.0 && (V(Trk::theta) <= M_PI));
}

} // end of namespace

#if __GNUC__
#pragma GCC diagnostic pop
#endif

#endif // TRK_KALMANUPDATOR_H
