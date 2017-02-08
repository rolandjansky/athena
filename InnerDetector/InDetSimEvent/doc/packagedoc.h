/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetSimEvent_page Package InDetSimEvent
@author Davide.Costanzo@cern.ch

@section InDetSimEvent_InDetSimEvent Introduction

This package contains the EDM classes for the Inner Detector simulation. 
There are three type of classes in this package:
  - The hit classes. They are persistent classes and are written out as output from the
Geant4 simulation and used as input for Digitization.
Some other non persistent classes 
  - The Simulation hit Id helpers. Which are needed to reinterpret the geometry information into
integers
  - Other non persistent classes used during digitization processing
The first two should NOT be changed, unless there is a very specific reason. In particular 
events have been simulated and written out with pool since release 8 and a change in these
classes may result in a big loss (like all of DC2)

@section InDetSimEvent_The hit classes
The hit classes are:
  - SiHit: hits in the Pixel/SCT and SiHitCollection
  - TRTUncompressedHit: hits in the TRT and TRTUncompressedHitColection
  - TRTHit and TRTHitCollection. Legacy

@section InDetSimEvent_The hit simulation helper classes
There are two of them. These classes should be used to interpret a geometry information and produce an integer:
  - SiHitIdHelper
  - TRTHitIdHelper

@section InDetSimEvent_Other stuff
SiCharge and SiTotalCharge are transient classes used by the Pixel and SCT digitization when charges are created as a result of the 
processing of the Geant hits. 
*/
