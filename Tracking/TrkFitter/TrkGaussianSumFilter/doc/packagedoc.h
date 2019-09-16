/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS
collaboration */

/**

@page TrkGaussianSumFilter_page TrkGaussianSumFilter Package

Fitting of tracks using the Gaussian Sum Filter Algorithm

@author Anthony.Morley@cern.ch, Tom.Atkinson@cern.ch

@section TrkGaussianSumFilter_TrkGsfIntro Introduction

Package allows for fitting using the Gaussian Sum Filter Algorithm. This algorithm finds particular 
application in the case where material effects or measurement errors are non-gaussian. 
The current implementation is limited to modelling the energy loss of relativistic electrons through 
the Bethe-Heitler distribution. Non-gaussian measurement errors are not yet included but will be in the future.

The Gaussian Sum Filter can be thought of as a number of Kalman Filters running in parallel. As such it uses 
a number of classes developed for the Kalman Filter. The main extension is to allow a track state to be described 
by a gaussian sum of track parameters objects, rather than the usual single component. 
The class TrkMultiComponentStateOnSurface handles this at present.

@section TrkGaussianSumFilter_TrkGsfOverview Class Overview

   There Gaussian sum filter makes use of the following existing interfaces
   - Trk::ITrackFitter
   - Trk::IUpdator (Trk::KalmanUpdator)
   - Trk::IPropagator - linear propagation

   The following classes are added. Most are simply to allow extrapolation / update of a Trk::MultiComponentState object.
   - Trk::IMultiStateExtrapolator - for extrapolation of Trk::MultiComponentState objects
   - Trk::IMultiStateMeasurementUpdator - for update of a Trk::MultiComponentState object with a measurement
   - Trk::IMultiStateMaterialEffectsUpdator - for convolution of Trk::MultiStateMaterialEffects with an existing state
   - Trk::IMultiStateMaterialEffects - for determining the material effects based on a Trk::MultiComponentState object and material information
   - Trk::IMultiStateComponentMerger - for performing component reduction
   - Trk::IGSFMaterialEffects - linear material effects
   All of these classes are pure virtual (with the exception of Trk::IMultiStateMaterialEffects - which provides some 
   implementation common to all material effects packages). From these are dervied the concrete classes:

   - Trk::GsfExtrapolator (Inherits from Trk::IMultiStateExtrapolator) for the Gaussian Sum Filter extrapolation proceedure
   - Trk::GsfMeasurementUpdator (Inherits from Trk::IMultiStateMeasurementUpdator)
   - Trk::GsfMaterialEffectsUpdator (Inherits from Trk::IMultiStateMaterialEffectsUpdator)
   - Trk::GsfCombinedMaterialEffects (Inherits from Trk::IMultiStateMaterialEffects)
   - Trk::GsfBetheHeitlerEffects (Inherits from Trk::IMultiStateMaterialEffects) - for the inclusion of Bethe-Heitler material effects
   - Trk::MultiStateMaterialEffectsAdapter (Inherits from Trk::IMultiStateMaterialEffects) - for converting normal material effects to Trk::IMultiStateMaterialEffects
   - Trk::QuickCloseComponentsMultiStateMerger (Inherits from Trk::IMultiStateComponentMerger) - merges like components (preserves 1st and 2nd moment of Bethe-Heitler distribution)
   like components (preserves 1st and 2nd moment of Bethe-Heitler distribution)
   - Trk::LargestWeightsComponentMerger (Inherits from Trk::IMultiStateComponentMerger) - discards small weighted components (does NOT preserve 1st and 2nd moment of Bethe-Heitler distribution)
   
   The following classes are AlgTools without IAlgTool interface definition yet
   - Trk::GaussianSumFitter - Performs the fitting, smoothing and fitQuality calculations associated with the GSF
   - Trk::ForwardGsfFitter - Performs the filter steps of the GSF
   - Trk::GsfSmoother -  Performs the smoother steps of the GSF
   - Trk::GsfOutlierLogic - Outlier logic for the GSF (does not do much other than calculate chi2 of track as per the Trk::TrkKalmanFilter implementation
   - Trk::MultiComponentStateCombiner - combines the components of a gaussian sum reducing it to a single gaussian distribution with mean and variance
   - Trk::MultiComponentStateAssembler - Assembles components into a state, performs 'house-keeping' operations such as removing small-weighted states
   - Trk::PosteriorWeightsCalculator - For performing the weights adjustment of the Gaussian Sum during the measurement update
  */  
