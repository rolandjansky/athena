/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkFitterInterfaces_page TrkFitterInterfaces Package

This package contains all interface classes which are used in the TrkFitter
container package.
Concrete implementations can be found in each leaf package.

@author Wolfgang.Liebig <http://consult.cern.ch/xwho/people/485812>

@section TrkFitterInterfaces_TrkFitterInterfacesIntro Introduction

The following interface classes are defined

   - Trk::ITrackFitter               : interface for EDM track fitters
   - Trk::IGlobalTrackFitter         : extension for least-squares fitters which give out a deriv matrix
   - Trk::IDynamicNoiseAdjustor      : interface for material effects noise update to momentum
   - Trk::IForwardKalmanFitter       : interface for the forward filter used by KF and DAF
   - Trk::IKalmanSmoother            : interface for the backward smoother used by KF and DAF
   - Trk::IKalmanOutlierLogic        : interface for outlier logic used by the KF
   - Trk::IMeasurementRecalibrator   : interface for fitters to explicitly change the hit (error) calibration
   - Trk::IKalmanPiecewiseAnnealingFilter : interface for executing local pattern (DAF) on a subset of the trajectory
   - Trk::IMultiTrackFitter           : interface for multi track fitters (fitting several tracks simultaneously)

This package is not built as any library, it is a simple include package.   



*/
