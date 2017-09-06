/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TRT_CalibTools_page TRT_CalibTools
@author johan.lundquist@nbi.dk

@section TRT_CalibTools_MyPackageIntro Introduction

This package is used to calibrate the %TRT. For calibration purposes The %TRT is divided into 7 sub-levels:<br>

<ul>
<li>Full <b>%TRT</b> - consists 4 <b>Sub-detectors</b>, 2 barrels and 2 end-caps<br>
<li><b>Sub-detector</b> - barrel contains 3 <b>Layers</b>, end-cap contains 14 Layers (wheels)<br>
<li><b>Layer</b> - contains 32 <b>Phi-Modules</b><br>
<li><b>Phi-Module</b> - contains a number of <b>Boards</b> which varies between 1 and 4<br>
<li><b>Board</b> - contains a number of <b>Chips</b> which varies between 9 and 15<br>
<li><b>Chip</b> - contains 16 <b>Straws</b><br>
<li><b>Straw</b><br>
</ul> 

A unit in a sub-level is in this documentation referred to as a sub-module. The calibration always starts on the top 
%TRT level and works it's way down through the successive sub-layers. Both the R-t and t0 calibration works this way, 
and one can configure either one of them to only go to certain depth. The sub-modules downstream from the last level 
that was calibrated will then be assigned the t0 and R-t parameters of the sub-module that are closest above (e.g. a chip 
will be assigned the value of the board it belongs to and a board to the phi module it belongs to).

(<i>In the barrel the two levels "Layer" and "Phi-module" are a bit special in the sense that it not obvious why one is to 
be considered a sub-level of the other due to their geometrical definitions; a layer can be said to consist of 32
phi-modules, but a phi-module could just as well be said to consist of 3 layers. The Layer level has anyhow chosen been 
chosen to be above the Phi-module level. This makes perfect sense for the end-caps where a layer is defined as a wheel.</i>)

It is also possible to make configurations in order to calibrate only a part of the detector. This allows for the 
calibration of various parts to be made in parallel. The output files can then be merged into calibration results for the 
whole detector. 

@section TRT_CalibTools_MyPackageOverview Class Overview
  The TRT_CalibTools package contains of following classes:
  
  FillAlignTRTHits
  TRTCalibrator
  Calibrator



*/
