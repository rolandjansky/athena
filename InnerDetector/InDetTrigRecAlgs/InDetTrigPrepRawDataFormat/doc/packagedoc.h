/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetTrigPrepRawDataFormat_page InDetTrigPrepRawDataFormat Package
@author Tommaso.Lari@cern.ch

@section InDetTrigPrepRawDataFormat_InDetTrigPrepRawDataFormatIntro Introduction

This package is an HLT wrapper. Its equivalent is
InDetPrepRawDataFormation in the offline reconstruction.  This package
manages the creation of InDetPrepRawData objects for the Inner
Detector.

@section InDetTrigPrepRawDataFormat_InDetTrigPrepRawDataFormatOverview Overview

There are three classes, all of them Algorithms:

<ul>

<li> Pixel_TrgClusterization

Calls the apropriate AlgorithmTools in SiClusterizationTool and set
their properties, according to the settings from jobOption. These
tools cluster together adjacent pixel cells and computes the best
estimate of position and errors. The resulting clusters, or
InDetPrepRawData, are then registered in StoreGate. Main settings
which can be changed via jobOptions:

ErrorStrategy 
<ul>
  <li>= 0: errors equal to cluster width/sqrt(12)</li>
  <li>= 1: errors equal to the pixel pitch/sqrt(12)</li>
  <li>= 2: parametrized as a function of eta, and cluster width 
                   (DEFAULT)</li>
</ul>

Position Strategy 
<ul>
<li>=0: cluster position is simple average of pixel cell 
                      position</li>
<li>= 1: charge interpolation used to compute cluster position
                        (DEFAULT) </li>
</ul>
</li>

<li> SCT_TrgClusterization

Calls the appropriate AlgorithmTools in SiClusterizationTool and set
their properties, according to the settings from jobOptions. These
tools cluster together adjacent SCT strips and computes the best
estimate of position and errors. The resulting clusters, or
InDetPrepRawData, are then registered in StoreGate.

Main settings which can be changed via jobOptions:

ErrorStrategy
<ul>
              <li>= 0: errors equal to cluster width/sqrt(12)</li>
              <li>= 1: errors equa to the pixel pitch/sqrt(12)</li>
              <li>= 2: parametrized as a function of eta, and cluster width 
                   (DEFAULT)</li>
</ul>
</li>

<li> TRT_TrgRIO_Maker

Trigger-specific parameters are:
<ul>
<li>RegSelToolName = "RegSelTool/RegSelTool_TRT" name of the RegSel tool, </li>
<li>EtaHalfWidth  - RoI half-width in eta </li>
<li>PhiHalfWidth - RoI half-width in phi </li>
</ul>

They can be changed via jobOptions.
</li>
</ul>

*/
