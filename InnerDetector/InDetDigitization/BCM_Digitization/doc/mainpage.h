/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage BCM_Digitization Package
@author Dominique.Tardif@cern.ch

@section BCMDigitizationOverview Overview

This package is used to simulate the electronics response of the 8 ATLAS Beam Conditions Monitor (BCM) modules. The GEANT4 BCM hit collection(s) is/are first retrieved from the simulation store. For each hit in the collection, a pulse whose height is linearly correlated with the energy deposit is added to the corresponding channel waveform, which has been pre-filled with Gaussian noise. This is then split into two parts, which correspond to high- and low-gain channels. A time-over-threshold is used to digitize the analog pulses, and the positions and widths of the first two digital pulses in each channel are recorded in a raw data object. The 16 RDOs are packed into a container which is then saved to the output store.

@ref used_BCM_Digitization

@ref requirements_BCM_Digitization

*/

/**
@page used_BCM_Digitization Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_BCM_Digitization Requirements
@include requirements
*/

