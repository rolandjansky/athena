/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IUpdator.h
//   Header file for interface of Updator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Markus.Elsing@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_IUPDATOR_H
#define TRK_IUPDATOR_H

#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h" // typedef
#include "TrkParameters/TrackParameters.h"          // typedef
#include <memory>
#include <vector>
static const InterfaceID IID_IUpdator("Trk::IUpdator", 1, 0);

namespace Trk {

#ifndef CreateParDef
#define CreateParDef
#define CREATE_PARAMETERS(ref, x, cov)                                         \
  (ref).associatedSurface().createUniqueTrackParameters(x[Trk::loc1],          \
                                                        x[Trk::loc2],          \
                                                        x[Trk::phi],           \
                                                        x[Trk::theta],         \
                                                        x[Trk::qOverP],        \
                                                        cov)
#define CLONEWITHOUTCOV(ref)                                                   \
  ref.associatedSurface().createUniqueTrackParameters(                         \
    ref.parameters[Trk::loc1],                                                 \
    ref.parameters[Trk::loc2],                                                 \
    ref.parameters[Trk::phi],                                                  \
    ref.parameters[Trk::theta],                                                \
    ref.parameters[Trk::qOverP],                                               \
    nullptr)
#endif

class LocalParameters;

/** @class Trk::IUpdator

    @brief Set of interfaces for methods operating on track states, mainly for
   Kalman filtering.

    Provides the interface to encapsulate the measurement updating
    calculations needed in progressive fitters such as the
    TrkKalmanFitter.
    The idea is that Updators take care internally of all
    calculations needed to update or remove a hit from a state,
    and to estimate the chi2 of a state, while the calling
    fitter takes care of the iteration strategy itself.

    @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
  */

class IUpdator : virtual public IAlgTool
{
public:
  /** Algtool infrastructure */
  static const InterfaceID& interfaceID();
  /** updator for Kalman-Filter based algorithms getting the measurement
      coordinates from Amg::Vector2D (use for example with PrepRawData) */
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const = 0;
  /** updator for Kalman-Filter based algorithms getting the measurement
      coordinates from LocalParameters (use for example with
      MeasurementBase or RIO_OnTrack) */
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const = 0;
  /** the updator interface with FitQualityOnSurface allows to save the chi2 in
      one step with the updating (the chi2 is automatically known during
      the updating maths). Version using Amg::Vector2D. */
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const = 0;
  /** the updator interface with FitQualityOnSurface allows to save the chi2 in
      one step with the updating (the chi2 is automatically known during
      the updating maths). Version using LocalParameters. */
  virtual std::unique_ptr<TrackParameters> addToState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const = 0;

  /** the reverse updating or inverse KalmanFilter removes a measurement
      from the track state, giving a predicted or unbiased state, here
      working with Amg::Vector2D from (for example) PrepRawData objects.
    */
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const = 0;
  /** the reverse updating or inverse KalmanFilter removes a measurement
      from the track state, giving a predicted or unbiased state, here
      working with LocalParameters from (for example) MeasurementBase
      or RIO_OnTrack objects.
    */
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const = 0;
  /** the reverse updating or inverse KalmanFilter removes a measurement from
      the track state, giving a predicted or unbiased state, here working with
      with Amg::Vector2D and in addition giving back the fit quality
      of the given state.
    */
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const = 0;
  /** the reverse updating or inverse KalmanFilter removes a measurement
      from the track state, giving a predicted or unbiased state, here
      working with LocalParameters and in addition giving back the
      fit quality of the given state.
    */
  virtual std::unique_ptr<TrackParameters> removeFromState(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&) const = 0;

  /** adds to a track state the parameters from another state using a
      statistical combination - use with care! In particular it is the
      caller's responsiblility that both states are expressed on the _same_
      surface and that the measurement on this surface is contained in not
      more than one of the two states. Method to be used e.g. for Kalman
     Smoothing or InDet - Muons track combination.
  */
  virtual std::unique_ptr<TrackParameters> combineStates(
    const TrackParameters&,
    const TrackParameters&) const = 0;
  /** adds to a track state the parameters from another state using a
      statistical combination and determines fit quality - use with care!
      In particular it is the caller's responsiblility that both states
      are expressed on the _same_ surface and that the measurement on
      this surface is contained in not more than one of the two states.
      Method to be used e.g. for Kalman Smoothing or InDet - Muons track
      combination.
  */
  virtual std::unique_ptr<TrackParameters> combineStates(
    const TrackParameters&,
    const TrackParameters&,
    FitQualityOnSurface*&) const = 0;

  /** pure AMG interface for reference-track KF, allowing update of parameter
   * differences
   */
  virtual std::pair<AmgVector(5), AmgSymMatrix(5)>* updateParameterDifference(
    const AmgVector(5) &,
    const AmgSymMatrix(5) &,
    const Amg::VectorX&,
    const Amg::MatrixX&,
    const int&,
    Trk::FitQualityOnSurface*&,
    bool) const = 0;

  /** estimator for FitQuality on Surface from a full track state,
      that is a state which contains the current hit (expressed as
      Amg::Vector2D). Keep in mind that this job can be done inside
      addToState if you have kept the original prediction, thus
      saving CPU time.
  */
  virtual const FitQualityOnSurface* fullStateFitQuality(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const = 0;
  /** estimator for FitQuality on Surface from a full track state,
      that is a state which contains the current hit (expressed as
      LocalParameters). Keep in mind that this job can be done inside
      addToState if you have kept the original prediction, thus
      saving CPU time.
  */
  virtual const FitQualityOnSurface* fullStateFitQuality(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const = 0;
  /** estimator for FitQuality on Surface from a predicted track state,
      that is a state which does not contain the current hit (expressed as
      Amg::Vector2D). Keep in mind that this job is already done inside
      addToState if you use the addToState(TP,LP,Err,FQoS) interface,
      thus saving CPU time.
  */
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const Amg::Vector2D&,
    const Amg::MatrixX&) const = 0;
  /** estimator for FitQuality on Surface from a predicted track state,
      that is a state which does not contain the current hit (expressed as
      LocalParameters). Keep in mind that this job is already done inside
      addToState if you use the addToState(TP,LP,Err,FQoS) interface,
      thus saving CPU time.
  */
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&) const = 0;
  /** estimator for FitQuality on Surface for the situation when a track
      is fitted to the parameters of another trajectory part extrapolated
      to the common surface.
  */
  virtual const FitQualityOnSurface* predictedStateFitQuality(
    const TrackParameters&,
    const TrackParameters&) const = 0;

  /** let the client tools know how the assumptions on the initial
      precision for non-measured track parameters are configured */
  virtual std::vector<double> initialErrors() const = 0;
};

} // end of namespace

inline const InterfaceID&
Trk::IUpdator::interfaceID()
{
  return IID_IUpdator;
}

#endif // TRK_IUPDATOR_H
