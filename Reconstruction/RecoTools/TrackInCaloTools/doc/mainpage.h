/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage The TrackInCaloTools package

@author Konstantinos.Bachas@cern.ch, Samira.Hassani@cern.ch, Bruno.Lenzi@cern.ch

@section introTrackInCalo Introduction

The TrackInCaloTools package provides measurements of the energy deposited by muon tracks in each calorimeter layer. This is achieved 
by extrapolating Inner Detector or Muon Spectrometer tracks to each layer using extrapolation tools of the official ATLAS software 
which take into account multiple scattering and magnetic field effects to the particle's trajectory. The procedure can be summarized 
in the following steps:
- The inputs are Trk::Track(or Trk::TrackParameters) and the cone size to be used for cell energy sum up.
- Using the AtlasExtrapolator interface, tracks are extrapolated to the first calorimeter layer. The decision as to which region, 
barrel or endcap, should the extrapolation target, is taken by a dedicated method which checks the eta of the track with the barrel 
or endcap sample's acceptance within the same calorimeter layer.
- After extrapolation the new eta and phi of the track on the calorimeter layer is used to open a cone around the track. The energy 
of each cell within the cone is compared to the noise rms and if found to be greater than a user defined threshold it is added to the 
total energy deposition. The rms of the noise is retrieved from CaloNoiseTool which accounts for both electronic noise and pile-up.
- Since the calorimeter cells are calibrated to the electron energy scale, a correction is applied to the energy measured in the 
layers of the electromagnetic and hadronic tile calorimeters in order to tranfer the measurement to the muon energy scale.
- The above procedure is repeated in each calorimeter layer.

@section used_packagesTrackInCalo Used Packages
@htmlinclude used_packages.html

@section requirements Requirements
@include requirements
**/

