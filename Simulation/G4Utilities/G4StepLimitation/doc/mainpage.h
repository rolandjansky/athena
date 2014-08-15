/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage  G4StepLimitation
@author Andrea Dell'Acqua (dellacqu@mail.cern.ch)

@section G4StepLimitation Introduction

This class provides an interface for the Geant4 physics process called step limitation.  Step limitation limits the maximum length of a step allowed.  It can be applied only in specific volumes in order to further divide the Geant4 step.  It was most recently used in the silicon of the pixel and SCT detectors in order to ensure a more uniform distribution of energy through the silicon as a track traverses the sensitive detector.

@section G4StepLimitation Class Overview

The only class included in this package is G4StepLimitProcess , a UserPhysicsProcess.  It defines the step limitation.

@ref used_G4StepLimitation

@ref requirements_G4StepLimitation

*/

/**
@page used_G4StepLimitation Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_G4StepLimitation Requirements
@include requirements
*/
