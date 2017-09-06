/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigSteering_page TrigSteering Package


New classes:
@author Till.Eifert@cern.ch
@author Nicolas.Berger@cern.ch
@author Tomasz.Bold@cern.ch

@section TrigSteering_TrigSteeringIntro Introduction

This package is used for the High Level Trigger (HLT), both
in the Level 2 (L2) and Event Filter (EF).
It provides one Gaudi TopAlgorithm (StepController) that runs all other
sub algorithms.
This package provides: tools to convert the results from previous trigger
levels; the actual controlling unit, which is responsible for
managing (scheduling) the execution of all HLT PESA
algorithms.



@section TrigSteering_TrigSteeringOverview Class Overview
  The TrigSteering package contains of following classes:

  - HLT::TrigSteer : Gaudi TopAlgorithm, controlling all other sub algorithms
  - HLT::Chain : representing one chain of signatures
  - HLT::Signature : representing one signature
  - HLT::Sequence : one sequence = PESA algorithm(s), input type(s), output type
  - HLT::LvlXConverter : various tools to do the trigger level conversion
  - Scaler : various classes which provide scaler engines for both: pass-throughs and prescales



*/
