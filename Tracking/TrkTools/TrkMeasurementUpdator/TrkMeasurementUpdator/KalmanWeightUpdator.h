/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanWeightUpdator.h
//   Header file for Kalman Fitter Updator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_KALMANWEIGHTUPDATOR_H
#define TRK_KALMANWEIGHTUPDATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/MsgStream.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkToolInterfaces/IUpdator.h"
#include <cmath>

namespace Trk {

/** @class KalmanWeightUpdator
 *  @brief Implementation of Trk::IUpdator using the weight formalism.
 *
 *  Tool providing the calculations to add or remove a measured
 *  hit to the state vector. Contains the core maths of KalmanFilters.
 *  This version of the tool implements the weight formalism
 *  @author Sebastian.Fleischmann@cern.ch
 */
class KalmanWeightUpdator
  : virtual public IUpdator
  , public AthAlgTool
{
public:
  //! standard AlgTool constructor
  KalmanWeightUpdator(const std::string&,
                      const std::string&,
                      const IInterface*);
  ~KalmanWeightUpdator();

  //! standard Athena initialisation
  virtual StatusCode initialize() override final;
  //! standard Athena termination
  virtual StatusCode finalize() override final;

  /** @brief measurement updator for the KalmanFitter getting the measurement
      coordinates from Amg::Vector2D (use for example with PrepRawData) */
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  /** @brief measurement updator for the KalmanFitter getting the coordinates
      from LocalParameters (use for example with MeasurementBase, ROT) */
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  /** @brief measurement updator interface for the KalmanFitter returning the
      fit quality of the state at the same time (Amg::Vector2D-version) */
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;
  /** @brief measurement updator interface for the KalmanFitter returning the
      fit quality of the state at the same time (LocalParameters-version) */
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;

  /** @brief reverse updator for the KalmanFitter and other fitters, getting
      the measurement coordinates from the Amg::Vector2D class.

      It can be used to get unbiased TrackParameters for track residual studies.
  */
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  /** @brief reverse updator for the KalmanFitter and other fitters, getting
      the measurement coordinates from the LocalParameters class.

      It can be used to get unbiased TrackParameters for track residual studies.
  */
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  /** @brief reverse updator for the KalmanFitter and other fitters using
      the interface with Amg::Vector2D and FitQualityOnSurface.
  */
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;
  /** @brief reverse updator for the KalmanFitter and other fitters using
      the interface with LocalParameters and FitQualityOnSurface.
  */
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const override final;

  //! trajectory state updator which combines two parts of a trajectory.
  /** make sure that the TPs' surfaces are identical and
      that the local hit is not duplicated in both trajectories.
  */
  virtual std::unique_ptr<TrackParameters> combineStates(
    const TrackParameters&,
    const TrackParameters&) const override final;
  //! combines two parts of a trajectory on a common surface and provides the
  //! FitQuality
  /** make sure that the TPs' surfaces are identical and that the local hit is
   * not duplicated.
   */
  virtual std::unique_ptr<TrackParameters> combineStates(
    const TrackParameters&,
    const TrackParameters&,
    FitQualityOnSurface*&) const override final;

  /** @brief estimator for FitQualityOnSurface from a full track state, i.e.
      a state which contains the current hit (expressed as Amg::Vector2D).
  */
  virtual const FitQualityOnSurface* fullStateFitQuality(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  /** @brief estimator for FitQualityOnSurface from a full track state, i.e.
      a state which contains the current hit (expressed as LocalParameters).
  */
  virtual const FitQualityOnSurface* fullStateFitQuality(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  /** @brief estimator for FitQualityOnSurface from a predicted track state,
    i.e. a state which contains the current hit (expressed as Amg::Vector2D).
  */
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const override final;
  /** @brief estimator for FitQualityOnSurface from a predicted track state,
  i.e. a state which contains the current hit (expressed as LocalParameters).
  */
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const override final;
  /** @brief estimator for FitQuality on Surface for the situation when a
      track is fitted to the parameters of another trajectory part
      extrapolated to the common surface.
  */
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const TrackParameters&) const override final;
  //! interface for update for reference-track KF, not implemented.
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

  //! give back how updator is configured for inital weights
  virtual std::vector<double> initialErrors() const override final;

private:
  //! common maths calculation code for addToState and removeFromState
  std::unique_ptr<TrackParameters> calculateFilterStep(const TrackParameters&,
                                                       const Amg::Vector2D&,
                                                       const Amg::MatrixX&,
                                                       const int,
                                                       FitQualityOnSurface*&,
                                                       bool) const;
  /** @brief common maths calculation code for addToState and removeFromState.

  Used formulas:

  (R. Fruehwirth, et al., Data Analysis Techniques for High-Energy Physics, 2nd
  ed.)<br> \f$ p_{k|k} = G_{k|k}^{-1} ( G_{k|k-1} p_{k|k-1} H^T_k W_k m_k )\f$
  \f$ G_{k|k} = G_{k|k-1} + H^T_k W_k H_k \f$<br>
  (R. Fruehwirth, Application of Kalman filtering, NIM A262, 446)<br>
  \f$ r_{k|k} = m_k - H_k p_{k|k} \f$
  \f$ \chi_{k|F}^2 = r_{k|k}^T W_k r_{k|k} + (p_{k|k} - p_{k|k-1})^T G_{k|k-1}
  (p_{k|k} - p_{k|k-1}) \f$
   */
  std::unique_ptr<TrackParameters> calculateFilterStep(const TrackParameters&,
                                                       const LocalParameters&,
                                                       const Amg::MatrixX&,
                                                       const int,
                                                       FitQualityOnSurface*&,
                                                       bool) const;

  /** @brief also the chi2 calculation and FitQuality object creation using
      gain matrix formalism is combined in an extra method.

      It is called by ___FitQuality().
      The sign controls the calculation in case a predicted input track state
     (sign=+1) or smoothed/updated input track state (sign=-1).<br> Used
     formula: (R. Fruehwirth, et al., Data Analysis Techniques for High-Energy
     Physics, 2nd ed.) \f$ r = m - H p \f$ \f$ \chi^2 = r^T (V - H C H^T)^{-1} r
     \f$
      */
  FitQualityOnSurface* makeChi2Object(Amg::VectorX&,
                                      const Amg::MatrixX&,
                                      const Amg::MatrixX&,
                                      const Amg::MatrixX&,
                                      const int) const;
  /** @brief The chi2 calculation and FitQuality object creation using
      weighted means formalism is combined in an extra method.

      It is called by calculateFilterStep() and combineStates().
      Used formula:
      (Fruehwirth, Application of Kalman filtering, NIM A262, 446)
      \f$ r_1 = m - H p_{new} \f$
      \f$ r_2 = p_{new} - p_{old} \f$
      \f$ \chi^2 = r_1^T V^{-1}.r_1 + r_2^T C_{old}^{-1}.r_2 \f$
      */
  FitQualityOnSurface* makeChi2Object(Amg::VectorX& residual1,
                                      const Amg::MatrixX& weight1,
                                      const int key1,
                                      Amg::VectorX& residual2,
                                      const Amg::MatrixX& weight2,
                                      const int key2) const;

  //! debugging output at input; only called if log.level() is low. */
  void logInputCov(const Amg::MatrixX&,
                   const Amg::VectorX&,
                   const Amg::MatrixX&) const;
  //! debugging output of result; only called if log.level() is low. */
  void logResult(const std::string&, const TrackParameters&) const;
  //! debugging output of cov result; only called if log.level() is low. */
  void logOutputCov(const Amg::VectorX&, const Amg::MatrixX&) const;
  //! debugging output of weight mat; only called if log.level() is low. */
  void logOutputWeightMat(Amg::MatrixX& covPar) const;

  //! job options: initial weight matrix for input pars without error matrix
  std::vector<double> m_weight;

  //! cached log.level()
  int m_outputlevel;

  //! check correct ranges of absolute phi (-pi, pi) and theta (0, pi) values
  bool thetaPhiWithinRange(const Amg::VectorX&, const int key = 31) const;
  //! check correct ranges of phi (-pi, pi) and theta (0, pi) differences
  bool diffThetaPhiWithinRange(const Amg::VectorX&, const int key = 31) const;
  //! bring phi or theta back into allowed range using periodicity
  bool correctThetaPhiRange(Amg::VectorX&,
                            const bool isDifference = false,
                            const int key = 31) const;
};

inline bool
KalmanWeightUpdator::thetaPhiWithinRange(const Amg::VectorX& V,
                                         const int key) const
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
KalmanWeightUpdator::diffThetaPhiWithinRange(const Amg::VectorX& V,
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

} // end of namespace

#endif // TRK_KALMANWEIGHTUPDATOR_H
