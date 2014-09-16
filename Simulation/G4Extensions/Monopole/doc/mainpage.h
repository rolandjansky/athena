/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage  Monopole
@author Andrea Dell'Acqua andrea.Dellacqua@cern.ch

@section Monopole Introduction

This package allows users to introduce magnetic monopoles into the Geant4 simulation.

@section MonopoleOverview Class Overview
  The Monopole package contains the following classes:

  - G4Monopole : Defines the magnetic monopole particle itself
  - MonopoleProcessDefinition : Defines which processes are to be applied to the magnetic monopoles as they propagate through the detector.
  - G4mplIonisation , G4mplIonisationModel , G4mplTransportation : Defines several specific processes for the magnetic monopoles.
  
@ref used_Monopole

@ref requirements_Monopole

*/

/**
@page used_Monopole Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_Monopole Requirements
@include requirements
*/

doc/mainpage.h
Monopole/G4Monopole.h
Monopole/G4mplTransportation.h
Monopole/MonopoleProcessDefinition.h
