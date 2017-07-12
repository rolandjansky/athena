/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkExRungeKuttaPropagator_page The TrkExRungeKutta package

Propagation of TrackParameters and their associated covariance matrices in a Runge-Kutta integrated method

@section TrkExRungeKuttaPropagator_TrkExRkIntroduction Introduction

The RungeKuttaPropagator implements the IPropagator interface with a Runge-Kutta 
integrated method to be used in a non-homogenous magnetic field.
In case of absence of a magnetic field the RungeKuttaPropagator would propagate the track-parameters
according to a straight line, in case of an homogenous magnetic field, a helical propagation is performed.

Common transformations with the STEP_Propagator can be found in the TrkExUtils package.

The tool implements two abstract interfaces:
- under Trk::IPropagator it provides propagation of EDM Trk::TrackParameters
- under Trk::IPatternParametersPropagator it provides propagation of parameters for internal use, the Trk::PatternTrackParameters

@section TrkExRungeKuttaPropagator_TrkExRkComments Comments

Please let me know of any errors, or if anything is unclear.
@author Igor.Gavrilenko@cern.ch

*/
