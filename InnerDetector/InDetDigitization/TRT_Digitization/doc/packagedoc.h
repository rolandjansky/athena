/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TRT_Digitization_page TRT_Digitization Package

Digitization of the TRT simulation.

@author Thomas Kittelmann (doxygen comments: Mogens Dam)

@section TRT_Digitization_IntroductionTRT_Digitization Introduction

The purpose of the TRT_Digitization package is to turn @e hits from the Geant4
simulation into @e digits, which is a reflection of the event format
as read out from the real detector.

The @e hits from the Geant4 simulation basically contains:

- positions of beginning and end of a G4 step inside the TRT straw
  (in G4 language, @c PreStepPoint and @c PostStepPoint)
- energy deposition inside the TRT straw


The TRT digitization is divided into several steps:
-# <b>Resimultion of ionisation process</b> @n
   Because the straws constitute very thin
   layers of active gas, the standard G4 ionisation functions are not
   appropriate for the detailed simulation of the ionisation process.
   As a consequence, the first step
   of the digitization is a re-simulation of energy loss by charged particles.
   This is done using the Photon Absorption Ionisation model as
   implemented in the @c TRT_PAI_Process package. The ionisation process
   creates @e clusters distributed along the particle trajectory. @n
   For photons interacting with the Xenon gas, no re-simulation is done.
   The deposited energy forms one @e cluster at the point of interaction.
-# <b>Modelling of the drift chamber</b> @n
   @e Clusters are converted to @e deposits, i.e. signals reaching the
   front end electronics.
   This process includes several effects/steps:
   - The number of primary drift electrons is calculated from the
     cluster energy
   - Electrons are drifted to the wire in the presence of the magnetic field
     (different effects in barrel/end cap).
     Stocastic recapturing of drift electrons in the drift gas.
     Optionally a drift time spread is included
   - Wire propagation delays (direct and reflected signals) can be optinally
     modelled
-# <b>Modelling of the front-end electronics</b> @n
   For each straw:
   - Energy @e deposits are filled (as delta functions) into a vector in the
     appropriate time bins.
   - This vector is convulated with the <em>signal shaping</em> functions
     (separate for low and high threshold signals)
   - The signals are then discriminated against low and high thresholds in
     appropriate time slices and the output digit constructed.

In addition, the package handles detector noise, which is modelled
after the detector performance in the 2004 combined test beam.

@section TRT_Digitization_OverviewTRTDigitization Class Overview
  The TRTDigitization package contains of following classes:

  - TRTDigitization: main class for the TRT digitization.
  - TRTProcessingOfStraw: Handles the complete digitization
                          straw-by-straw.
  - TRTGasGain: Gas gain as function of drift distance.
  - TRTSignalShape:       Provides signal shaping for the electronics signals.
  - TRTElectronicsProcessing: conversion of signals from @e deposits
                              on signal wires to @e digits - the 27 bit
			      output from the front-end electronics
  - TRTDigit: the @e digit for a single straw.
  - TRTTimeCorrection:    Calculates the time correction from wire propagation
                          delays.
  - TRTDigCondBase: communication with CondDB - the conditions data base.
  - TRTDigCondDase::StrawState: The status of an individual straw.
  - TRTDigCondFakeMap: Fake straw conditions map until we know the real
                       detector.
  - TRTDigSettings: containing parameters and settings for TRT digitization.
  - TRTElectronicsNoise: Together with @c TRTNoise, this controls the
                         simulation of electronics noise.
  - TRTNoise: Together with @c TRTElectronicsNoise, this controls the
                         simulation of electronics noise.



*/
