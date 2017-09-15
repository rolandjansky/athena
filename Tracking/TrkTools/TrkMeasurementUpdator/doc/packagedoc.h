/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkMeasurementUpdator_page The TrkMeasurementUpdator package

@section TrkMeasurementUpdator_introductionTrkMeasurementUpdator Introduction

The TrkMeasurementUpdator package implements the core mathematics
for Kalman filter-based track fitters. It provides the calculations
to add or remove a measurement to/from the state vector of a track.
Inheriting from the Trk::IUpdator abstract interface, the following
operations are implemented:
   - addition of a measurement to a (predicted) track states in local coordinates (i.e. the Kalman filter step)
   - removal of a measurement from a track state (inverse filter)
   - calculation of Chi2/ndf together with the above operation as well as in separate methods
   - operates on Trk::LocalParameters of any dimension (1-5) and definition
   - operates also on Trk::LocalPosition

@section TrkMeasurementUpdator_TrkMeasurementUpdatorOverview Class Overview
  The TrkMeasurementUpdator package contains the following classes:

  - Trk::KalmanUpdator : measurement updator in gain matrix formalism (internally using SMatrix)
  - Trk::KalmanUpdator_CLHEP : measurement updator in gain matrix formalism using CLHEP
  - Trk::KalmanWeightUpdator : measurement updator using weighted means formalism

@author Sebastian Fleischmann <http://consult.cern.ch/xwho/people/630599><br>
        Wolfgang Liebig <http://consult.cern.ch/xwho/people/485812>

@section TrkMeasurementUpdator_TrkMeasurementUpdatorDocReferences References
R. Fruehwirth, <i>Application of Kalman filtering</i>, NIM A262, 446<br>
R. Fruehwirth, et al., <i>Data Analysis Techniques for High-Energy Physics</i>, 2nd ed.

@section TrkMeasurementUpdator_ExtrasTrkMeasurementUpdator Extra Pages

*/
