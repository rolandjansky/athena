/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************************************
            MultiComponentStateOnSurface.h  -  description
            -----------------------------------------------
begin                : Monday 20th December 2004
author               : atkinson, amorley,anastopoulos (adapted from Moyse)
description          : This class is a multi component adaptation of the
                      class TrackStateOnSurface.
                      In that class the track state was represented
                      by a single 5 component track paramter
                      vector (a0, z0, phi0, theta0, q/p) and the associated
                      covariance matrix.
                      In its multi-component form the track
                      state on surface is represented by many track parameters
                      each with a covariance matrix and additionally a
                      weighting is attached to each component which reflects
                      the importance of that particular component in the
                      overall mixture of components which is used to describe
                      the track state at that surface.
                      Instances  of this
                      class are EDM objects. So objects passed to this class
                      as inputs via the ctors are owned by this class instances.
*******************************************************************************/

#ifndef TrkMultiComponentStateOnSurface_H
#define TrkMultiComponentStateOnSurface_H

#include "TrkEventPrimitives/FitQualityOnSurface.h" //typedef
#include "TrkGaussianSumFilterUtils/MultiComponentState.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include <iostream>

class MsgStream;
namespace Trk {
class MaterialEffectsBase;
class MeasurementBase;

class MultiComponentStateOnSurface final : public TrackStateOnSurface
{
public:
  /** Default constructor for POOL. This should not be used! */
  MultiComponentStateOnSurface();

  /** Create a MultiComponentStateOnSurface Object. This has the same form as
     the singular version (Trk::TrackStateOnSurface) with the exception that the
     pointer to
      a single track paramters vector is now repleaced with a pointer to a
     multi-component state */
  MultiComponentStateOnSurface(
    std::unique_ptr<const MeasurementBase>,
    MultiComponentState&&,
    std::unique_ptr<const FitQualityOnSurface>,
    std::unique_ptr<const MaterialEffectsBase> materialEffectsOnTrack = nullptr,
    double modeQoverP = 0.);

  /** Create a MultiComponentStateOnSurface Object with an explicit declaration
   * of the track parameters to be passed to the Trk::TrackStateOnSurface base
   * class */
  MultiComponentStateOnSurface(
    std::unique_ptr<const MeasurementBase>,
    std::unique_ptr<const TrackParameters>,
    MultiComponentState&&,
    std::unique_ptr<const FitQualityOnSurface>,
    std::unique_ptr<const MaterialEffectsBase> materialEffectsOnTrack = nullptr,
    double modeQoverP = 0.);

  /** Create TrackStateOnSurface with TrackStateOnSurfaceType. */
  MultiComponentStateOnSurface(
    std::unique_ptr<const MeasurementBase>,
    MultiComponentState&&,
    std::unique_ptr<const FitQualityOnSurface>,
    std::unique_ptr<const MaterialEffectsBase>,
    const std ::bitset<NumberOfTrackStateOnSurfaceTypes>&,
    double modeQoverP = 0.);

  /** Create a MultiComponentStateOnSurface Object with an explicit declaration
   * o the track parameters to be passed to the base and also a
   * TrackStateOnSurfaceType */
  MultiComponentStateOnSurface(
    std::unique_ptr<const MeasurementBase>,
    std::unique_ptr<const TrackParameters>,
    MultiComponentState&&,
    std::unique_ptr<const FitQualityOnSurface>,
    std::unique_ptr<const MaterialEffectsBase>,
    const std ::bitset<NumberOfTrackStateOnSurfaceTypes>& types,
    double modeQoverP = 0.);

  /** Constructor without a FitQualityOnSurface. */
  MultiComponentStateOnSurface(std::unique_ptr<const MeasurementBase>,
                               MultiComponentState);

  /** Copy constructor and assignment*/
  MultiComponentStateOnSurface(const MultiComponentStateOnSurface& other);
  MultiComponentStateOnSurface& operator=(
    const MultiComponentStateOnSurface& other);

  /** Move constructor and assignment*/
  MultiComponentStateOnSurface(MultiComponentStateOnSurface&& other) noexcept =
    default;
  MultiComponentStateOnSurface& operator=(
    MultiComponentStateOnSurface&& other) noexcept = default;

  /** Virtual destructor */
  virtual ~MultiComponentStateOnSurface() = default;

  /** Clone method for deep copy of MultiComponentStateOnSurface - overidden
   * from base class */
  virtual TrackStateOnSurface* clone() const override final;

  /** This is Multi, since we MultiComponent */
  virtual TrackStateOnSurface::Variety variety() const override final;

  /** Method to return a pointer to the multi-component state  const overload*/
  const MultiComponentState& components() const;

  /** Method to return a pointer to the multi-component state non const
   * overload*/
  MultiComponentState& components();

  /** Method to return the mode of the multi-component state */
  double mixtureModeQoverP() const;

private:
  MultiComponentState m_multiComponentState{};
  double m_mixtureModeQoverP{};
};

/** Overload of << operator for MsgStream for debug output */
MsgStream&
operator<<(MsgStream&, const MultiComponentStateOnSurface&);

/** Overload of << operator for std::ostream for debug output */
std::ostream&
operator<<(std::ostream&, const MultiComponentStateOnSurface&);

} // end of Trk namespace

/// Trk::Track is constucted from  DataVector<const Trk::TrackStateOnSurface>.
/// Let the type system know this class inherits so we can have
/// DataVector<const Trk::MultiComponentStateOnSurface>
#include "AthContainers/DataVector.h"
DATAVECTOR_BASE(const Trk::MultiComponentStateOnSurface,
                const Trk::TrackStateOnSurface);
typedef DataVector<const Trk::MultiComponentStateOnSurface>
  TrkMultiComponentStateOnSurfaceDV;

#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.icc"
#endif
