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
measurement errors are non-gaussian.
We currently implement and use it for the modelling
of the energy loss of relativistic electrons through the Bethe-Heitler
distribution.

The Gaussian Sum Filter can be thought of as a number of Kalman Filters running
in parallel. The main extension is to allow a track state to be described by a
gaussian sum of track parameters objects, rather than the usual single
component.

@section TrkGaussianSumFilter_TrkGsfOverview Class Overview

   The following addtional interfaces are declared
   - Trk::IMultiStateExtrapolator - for extrapolation of
     Trk::MultiComponentState objects
   - IMaterialMixtureConvolution - for convolution of
     MultiStateMaterialEffects with an existing state


   The following implementations  are provided
   - Trk::GaussianSumFitter (implements Trk::ITrackFitter)
   - Trk::GsfExtrapolator (implements Trk::IMultiStateExtrapolator) for the
                           Gaussian Sum Filter extrapolation proceedure
   - Trk::ElectronMaterialMixtureConvolution.h (implements
     Trk::IMultiStateMaterialEffects) for electrons material effects
     (Bethe-Heitler)
  */
