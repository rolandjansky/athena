/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigEFMissingET_page TrigEFMissingET Package

The TrigEFMissingET package implements the missing energy and scalar 
energy sums feature extraction (Fex) algorithms for the EF level ATLAS
trigger level.  More details are provided by the 
<a href="https://twiki.cern.ch/twiki/bin/view/Atlas/AtlasHltMetSlice">AtlasHltMetSlice</a>
twiki page and in the 
<a href="http://cdsweb.cern.ch/record/1129146">ATL-COM-DAQ-2008-008</a>
note.

@authors Ignacio Aracena <Ignacio.Aracena@cern.ch>
         Diego Casadeii <Diego.Casadei@cern.ch>
         Kyle Cranmer <cranmer@cern.ch>
         Rashid Djilkibaev <rashid@physics.nyu.edu>
         Xiaowen Lei  <xiaowen@physics.arizona.edu>
         Allen Mincer <allen.mincer@nyu.edu>

@section TrigEFMissingET_TrigEFMissingETIntro Introduction

The package provides Event Filter level MissingET algorithms.
It returns the following, stored in the TrigMissingET class
(defined in Trigger/TrigEvent/TrigMissingET):
Etx and Ety - the x and y components of missing Et 
Et - calculated as the sqrt(metx^2+mety^2)  
sumEt - the scalar sum of the energy projected onto the 
        transverse plane
Ez - the longitudinal component of the missing energy sum 

It also returns a vector of objects of the TrigMissingEtComponent 
class which includes the above variables and the calibration 
constants used, separately for each layer.

It may eventually also return sigmaEt, an estimate of the 
uncertainty in Et.

The current calorimeter tools return missing Et as calculated 
from calorimeter cells only, that is, there are no crack
corrections.  A simple calibration scheme is included (see the 
description below).

The routine can be run in a chain that includes muon seeding, in 
which case the muon tool uses EF level muons to correct returned 
values.  Muon values are also separately included as one of the
"layers" in the component class. 

@section TrigEFMissingET_TrigEFMissingETOverview Class Overview

The TrigEFMissingET package contains of following classes:

  - EFMissingET : main class, responsible for creating the transient
    class TrigEFMissingEtHelper and the persistent class TrigMissingET, 
    controls monitoring and invokes all tools.

  - EFMissingETHelper.cxx : This class stores transient-only information.
    All tools, apart from EFMissingETFromHelper itself, operate on 
    one instance of this class. EFMissingETFromHelper, is responsible 
    for transferring the information from this transient class to the 
    persistent class TrigMissingET.

  - EFMissingETFlags.cxx : Does several checks on information accumulated
    in the transient class and sets event status flags.

@section TrigEFMissingET_TrigEFMissingETTools Tools for Calculating Missing Et

  - EFMissingETBaseTool.cxx : Base class from which the derived tools 
    inherit. It provides all shared attributes, in addition to the 
    interface definition.

  - EFMissingETFromCells.cxx : Computation of vector and scalar quantities
    for each calorimeter sampling by looping over all cells
    A simplified calibration is included as described below in
    EFMissingETFromHelper.cxx
    Cell noise suppression is currently implemented with a one sided
    (eliminate cells with E<N*sigma_cell_noise) cut.  
 
  - EFMissingETFromFEBHeader.cxx : Calculates all quantities for LARr 
    samplings only by looping over FEB headers.  This is much faster 
    than looping over all cells.
   
  - EFMissingETFromMuons.cxx : Calculates Missing ET from EF Muons.
    (Early versions of the code used Level 2 muons). 

  - EFMissingETFromLvl1Ppr.cxx : Mentioned here for backwards information
    only, this would get trigger towers from the level 1 pre-processor. 
    It is not currently implemented and is no longer part of the package.

  - EFMissingETFromHelper.cxx : Returns accumulated results from the
    EFMissingETHelper class and saves the transient information in
    the persistent object, applying calibration only to global properties

    Layer by layer calibration constants are included in the form
    Ecalibrated = C1*Eraw + C0*sign_sum, where sign_sum is the number 
    of positive energy cells minus number of negative energy cells in a
    layer.

@section TrigEFMissingET_TrigEFMissingETAlgorithm Algorithm properties

The following algorithm properties can be set via athena.  See
Trigger/TrigAlgorithms/TrigEFMissingET/trunk/python/TrigEFMissingETConfig.py
for how these are set.

property                   [default]                    meaning

"MissingETOutputKey"  ["TrigEFMissingET"]  HLT feature label in navigation
"Tools"                                    list of tools to use
"doTimers"                  [true]         switch on/off internal timers
"ComponentFlags"                           set to -1 to switch off a component
"ComponentCalib0"                          additive calibration constants
"ComponentCalib1"                          multiplicative calib. constants

Tool properties:

EFMissingETFromCells::EFMissingETFromCells

"DoCellNoiseSuppression"   [false]         do noise suppression T/F
"useFullCollection"        [true]          use calorimeter load full collection T/F
"CaloNoiseTool"                            Tool Handle for noise tool
"CaloNoiseFactor"          [2.]            Number of sigmas for noise cut

EFMissingETFromFEBHeader::EFMissingETFromFEBHeader
"useFullCollection"        [true]          Use calorimeter load full collection T/F

*/
