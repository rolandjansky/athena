/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanUpdator.h
//   Header file for Kalman Fitter Updator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Markus.Elsing@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_KALMANUPDATOR_H
#define TRK_KALMANUPDATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/MsgStream.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/ProjectionMatricesSet.h"
#include "TrkToolInterfaces/IUpdator.h"
#include <cmath>

namespace Trk {

/** @class KalmanUpdator

    @brief Implementation of Trk::IUpdator based on gain formalism and Eigen.

    Tool to provide calculations for
    Kalman filtering, i.e. to add or remove a measured hit to the
    state vector. Implemented fully in Eigen using the EDM natively.
    As a drawback due to Eigen's internal implementation of matrix
    operations, this tool is generally slower than its alternatives
    which are written based on array or SMatrix of fixed size Eigen
    internal representations of EDM objects.

    @author M. Elsing, W. Liebig <http://consult.cern.ch/xwho>
 */
class KalmanUpdator
  : virtual public IUpdator
  , public AthAlgTool
{
public:
  //! AlgTool standard constuctor
  KalmanUpdator(const std::string&, const std::string&, const IInterface*);
  ~KalmanUpdator();

  //! AlgTool initialisation
  virtual StatusCode initialize() override final;
  //! AlgTool termination
  virtual StatusCode finalize() override final;

  //! measurement updator for the KalmanFitter getting the meas't coord'
  // from Amg::Vector2D (use eg with PRD)
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  //! measurement updator for the KalmanFitter getting the coordinates
  // from LocalParameters (use eg with MeasurementBase, ROT)
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  //! measurement updator interface for the KalmanFitter returning the fit
  //! quality
  // of the state at the same time (Amg::Vector2D-version)
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;
  //! measurement updator interface for the KalmanFitter returning the fit
  //! quality
  // of the state at the same time (LocalParameters-version)
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;

  //! reverse update eg for track property analysis (unbiased residuals)
  // getting the measurement coordinates from the Amg::Vector2D class.
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  //! reverse update eg for track property analysis (unbiased residuals)
  // getting the measurement coordinates from the LocalParameters class.
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  //! reverse updator for the KalmanFitter and other fitters using the
  // interface with Amg::Vector2D and FitQualityOnSurface.
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;
  //! reverse updator for the KalmanFitter and other fitters using
  // the interface with LocalParameters and FitQualityOnSurface.
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
   * on a common surface and provides the FitQuality.

      Make sure that the TPs' surfaces are identical and that the local hit is
   not duplicated!
  */
  virtual std::unique_ptr<TrackParameters> combineStates(
    const TrackParameters&,
    const TrackParameters&,
    FitQualityOnSurface*&) const override final;

  //! estimator for FitQuality on Surface from a full track state, that is a
  //! state
  // which contains the current hit (expressed as Amg::Vector2D).
  virtual const FitQualityOnSurface* fullStateFitQuality(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;

  //! estimator for FitQuality on Surface from a full track state, that is
  // a state which contains the current hit (expressed as LocalParameters).
  virtual const FitQualityOnSurface* fullStateFitQuality(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  //! estimator for FitQuality on Surface from a predicted track state, that is
  //! a state
  // which contains the current hit (expressed as Amg::Vector2D).
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  //! estimator for FitQuality on Surface from a predicted track state, that is
  //! a state
  // which contains the current hit (expressed as LocalParameters).
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  //! estimator for FitQuality on Surface for the situation when a track is
  //! fitted to
  // the parameters of another trajectory part extrapolated to the common
  // surface.
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const TrackParameters&) const override final;
  //! interface for reference-track KF, not implemented.
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

  //! gives back how updator is configured for inital covariances
  virtual std::vector<double> initialErrors() const override final;

private:
  //! Common maths calculation code for addToState and removeFromState -
  //! Amg::Vector2D interface.
  std::unique_ptr<TrackParameters> calculateFilterStep(const TrackParameters&,
                                                       const Amg::Vector2D&,
                                                       const Amg::MatrixX&,
                                                       const int,
                                                       FitQualityOnSurface*&,
                                                       bool) const;
  //! Common maths calculation code for addToState and removeFromState -
  //! LocalParameters interface.
  std::unique_ptr<TrackParameters> calculateFilterStep(const TrackParameters&,
                                                       const LocalParameters&,
                                                       const Amg::MatrixX&,
                                                       const int,
                                                       FitQualityOnSurface*&,
                                                       bool) const;
  /** also the chi2 calculation and FitQuality object creation is
      combined in an extra method. It is called by ___FitQuality() and
     calculateFilterStep() The sign controls the calculation in case a predicted
     input track state (sign=+1) or smoothed/updated input track state
     (sign=-1).
  */
  FitQualityOnSurface* makeChi2Object(const Amg::VectorX&,
                                      const Amg::MatrixX&,
                                      const Amg::MatrixX&,
                                      const int,
                                      const int) const;

  //! avoid CLHEP's empty math operations (H-matrix) by copying members out
  Amg::MatrixX projection(const Amg::MatrixX&, const int) const;

  //! internal structuring: debugging output for start of method.
  void logStart(const std::string&, const TrackParameters&) const;
  //! internal structuring: common logfile output of the inputs
  void logInputCov(const Amg::MatrixX&,
                   const Amg::VectorX&,
                   const Amg::MatrixX&) const;
  //! internal structuring: common logfile output during calculation
  void logGainForm(int,
                   const Amg::VectorX&,
                   const Amg::MatrixX&,
                   const Amg::MatrixX&,
                   const Amg::MatrixX&) const;
  //! internal structuring: common logfile output after calculation
  void logResult(const std::string&,
                 const Amg::VectorX&,
                 const Amg::MatrixX&) const;

  //! method testing correct use of LocalParameters */
  bool consistentParamDimensions(const LocalParameters&, const int&) const;
  //! tests if ranges of abolute phi (-pi, pi) and theta (0, pi) are correct */
  bool thetaPhiWithinRange(const Amg::VectorX&, const int key = 31) const;
  //! tests if ranges of phi (-pi, pi) and theta (0, pi) residuals are correct
  //! */
  bool diffThetaPhiWithinRange(const Amg::VectorX&, const int key = 31) const;
  //! brings phi/theta back into valid range using 2pi periodicity
  bool correctThetaPhiRange(Amg::VectorX&,
                            AmgSymMatrix(5) &,
                            const bool isDifference = false,
                            const int key = 31) const;
  bool correctThetaPhiRange(Amg::VectorX&,
                            Amg::MatrixX&,
                            const bool isDifference = false,
                            const int key = 31) const;

  std::vector<double> m_cov0; //!< job option: initial covariance matrix
  ProjectionMatricesSet
    m_projectionMatrices; //!< get the correct projection matrix
  bool m_useFruehwirth8a; //!< job option: formula for cov update
  int m_outputlevel;      //!< MsgStream output level cached
};

inline bool
KalmanUpdator::thetaPhiWithinRange(const Amg::VectorX& V, const int key) const
{
  if (!(key & 4 || key & 8))
    return true; // in case no angles measured.
  if (key == 31)
    return ((std::abs(V[Trk::phi]) <= M_PI) && (V[Trk::theta] >= 0.0) &&
            (V[Trk::theta] <= M_PI));
  else { // if vector is compressed (i.e. localParameters) need to extract
         // phi,theta first.
    bool okay = true;
    if (key & 4) { // phi is being measured
      int jphi = 0;
      for (int itag = 0, ipos = 1; itag < Trk::phi; ++itag, ipos *= 2)
        if (key & ipos)
          ++jphi;
      okay = okay && (std::abs(V[jphi]) <= M_PI);
    }
    if (key & 8) { // theta is being measured
      int jtheta = 0;
      for (int itag = 0, ipos = 1; itag <= Trk::theta; ++itag, ipos *= 2)
        if (key & ipos)
          ++jtheta;
      okay = okay && (std::abs(V[jtheta] - M_PI_2) <= M_PI_2);
    }
    return okay;
  }
}

// for speed reasons make two separate inline functions
// phi   differences should be smaller than pi (else go other way round) => same
// as absolute phi value. theta differences should be smaller than pi but can be
// negative => other constraint than absolute theta.
inline bool
KalmanUpdator::diffThetaPhiWithinRange(const Amg::VectorX& V,
                                       const int key) const
{
  if (!(key & 4 || key & 8))
    return true; // in case no angles measured.
  if (key == 31)
    return ((std::abs(V[Trk::phi]) <= M_PI) && (V[Trk::theta] >= -M_PI) &&
            (V[Trk::theta] <= M_PI));

  else { // if vector is compressed (i.e. localParameters) need to extract
         // phi,theta first.
    bool okay = true;
    if (key & 4) { // phi is being measured
      int jphi = 0;
      for (int itag = 0, ipos = 1; itag < Trk::phi; ++itag, ipos *= 2)
        if (key & ipos)
          ++jphi;
      okay = okay && (std::abs(V[jphi]) <= M_PI);
    }
    if (key & 8) { // theta is being measured
      int jtheta = 0;
      for (int itag = 0, ipos = 1; itag <= Trk::theta; ++itag, ipos *= 2)
        if (key & ipos)
          ++jtheta;
      okay = okay && (std::abs(V[jtheta]) <= M_PI);
    }
    return okay;
  }
}

inline FitQualityOnSurface*
KalmanUpdator::makeChi2Object(const Amg::VectorX& residual,
                              const Amg::MatrixX& covTrk,
                              const Amg::MatrixX& covRio,
                              const int sign,
                              const int key) const
{ // sign: -1 = updated, +1 = predicted parameters.
  Amg::MatrixX R = covRio + sign * projection(covTrk, key); // .similarity(H);
  if (R.determinant() == 0) {
    ATH_MSG_DEBUG("matrix inversion failed");
    return new FitQualityOnSurface(0.0, (int)covRio.cols());
  }
  // get chi2 = r.T() * R^-1 * r
  double chiSquared = residual.transpose() * R.inverse() * residual;
  ATH_MSG_VERBOSE("-U- fitQuality of " << (sign > 0 ? "predicted" : "updated")
                                       << " state, chi2 :" << chiSquared
                                       << " / ndof= " << covRio.cols());
  // return the FitQualityOnSurface object
  return new FitQualityOnSurface(chiSquared, int(covRio.cols()));
}

} // end of namespace

#endif // TRK_KALMANUPDATOR_CLHEP_H
