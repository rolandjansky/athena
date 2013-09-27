/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage TilePulseSimulator package

@author Simon Molander <simon.molander@cern.ch>

@section Intro Introduction

This package contains a set of ROOT-based classes which are used by 
TileSimAlgs package to inject arbitrary pulse into TileCal digitization chain:

   - TilePulseShape: Class for definition of pulse shape. Contains getter for values at specified time.
   
   - TileSampleBuffer: Class that holds output from generator. Getter for both vector and single values.
   
   - TileSampleGenerator: Class that generates a shape with specified paramaters using a TilePulseShape to define the shape of the pulse. Gives output to a TileSampleBuffer object.


*/
