/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
  */

/**

@page TrkGaussianSumFilter_page TrkGaussianSumFilter Package

Fitting of tracks using the Gaussian Sum Filter Algorithm

@author Anthony Morley, Tom Atkinson, Christos Anastopoulos

@section TrkGaussianSumFilter_TrkGsfIntro Introduction

Package allows for fitting using the Gaussian Sum Filter Algorithm. This
algorithm finds particular application in the case where material effects or
measurement errors are non-gaussian. The current implementation is limited to
modelling the energy loss of relativistic electrons through the Bethe-Heitler
distribution.

The Gaussian Sum Filter can be thought of as a number of Kalman Filters running
in parallel. As such it uses a number of classes developed for the Kalman
Filter. The main extension is to allow a track state to be described by a
gaussian sum of track parameters objects, rather than the usual single
component. The class TrkMultiComponentStateOnSurface handles this at present.

@section TrkGaussianSumFilter_TrkGsfOverview Class Overview


   The following  interfaces are declared
   - IForwardGsfFitter - Forward GSF filter
   - IGsfSmoother - GSF smoother
   - Trk::IMultiStateExtrapolator - for extrapolation of
Trk::MultiComponentState objects
   - Trk::IMultiStateMaterialEffectsUpdator - for convolution of
MultiStateMaterialEffects with an existing state
   - Trk::IMultiStateMaterialEffects - for determining the material effects
based on a Multi Component State object and material information
Trk::IMultiStateMaterialEffects - which provides some implementation common to
all material effects packages).


   The following implementation of the above Interfaces are provided
   - Trk::GaussianSumFitter (implements Trk::ITrackFitter)
   - Trk::ForwardGsfFitter (implements IForwardGsfFitter) Performs the filter
steps of the GSF
   - Trk::GsfSmoother (implements IGsfSmoother)  Performs the smoother steps of
the GSF
   - Trk::GsfExtrapolator (implements Trk::IMultiStateExtrapolator) for the
Gaussian Sum Filter extrapolation proceedure
   - Trk::GsfMaterialEffectsUpdator (implements
Trk::IMultiStateMaterialEffectsUpdator)
   - Trk::GsfCombinedMaterialEffects (implements
Trk::IMultiStateMaterialEffects)
   - Trk::GsfBetheHeitlerEffects (implements Trk::IMultiStateMaterialEffects)
for the inclusion of Bethe-Heitler material effects
  */
