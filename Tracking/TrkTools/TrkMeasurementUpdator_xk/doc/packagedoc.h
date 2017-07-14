/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

i/**
@page TrkMeasurementUpdator_xk_page The TrkMeasurementUpdator_xk package

@section TrkMeasurementUpdator_xk_introTrkMeasurementUpdatorXK Overview

The TrkMeasurementUpdator_xk package implements the core mathematics
for Kalman filter-based track fitters. It provides the calculations
to add or remove a measurement to/from the state vector of a track
using fast mathematic routines and a simple internal data model.
It inherits from two abstract interfaces, Trk::IUpdator and Trk::IPatternParametersUpdator,
and implements the following operations:
   - addition of a measurement to a (predicted) track states in local coordinates (i.e. the Kalman filter step)
   - removal of a measurement from a track state (inverse filter)
   - calculation of Chi2/ndf together with the above operation as well as in separate methods
   - operates on Trk::LocalParameters of any dimension (1-5) and definition
   - operates also on Trk::LocalPosition

If retrieved under the Trk::IUpdator interface, the Trk::KalmanUpdator_xk provides a set of
updating-tools for EDM Trk::TrackParameters.<br>
If retrieved under the Trk::IPatternParametersUpdator interface, the 
Trk::KalmanUpdator_xk provides a set of methods for manipulating
Trk::PatternTrackParameters.<br>
The code has been migrated from the xKalman package to this module.

@author Igor Gavrilenko

@section TrkMeasurementUpdator_xk_RefsTrkMeasurementUpdatorXK References
R. Fruehwirth, <i>Application of Kalman filtering</i>, NIM A262, 446<br>
R. Fruehwirth, et al., <i>Data Analysis Techniques for High-Energy Physics</i>, 2nd ed.

@section TrkMeasurementUpdator_xk_ExtrasTrkMeasurementUpdatorXK Extra Pages

*/
