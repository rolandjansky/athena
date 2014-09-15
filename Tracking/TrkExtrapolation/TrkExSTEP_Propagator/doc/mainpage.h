/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage TrkExSTEP_Propagator package

@section TrkExSTEPIntroduction Introduction

The STEP_Propagator implements the IPropagator interface with a Runge-Kutta-Nystrom integration method to be used in an inhomogenous magnetic field with material effects included. The covariance matrix is also tranported.

Common transformations with the RungeKuttaPropagator can be found in the TrkExUtils package.

The STEP_Propagator has ten properties that can be set in the jobOptions. Among these are the accuracy of the propagation and
a switch for excluding material effects.

Details are found in:
Parameter propagation: http://cdsweb.cern.ch/record/1113528
Error propagation:     http://cdsweb.cern.ch/record/1114177
Material interactions: http://cdsweb.cern.ch/record/1114577

Units are mm, MeV and kiloGauss.

@section TrkExSTEPComments Comments
Please let me know of any errors, or if anything is unclear.

@author esben.lund@fys.uio.no

@include requirements
*/
