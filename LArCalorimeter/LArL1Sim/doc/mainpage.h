/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

The aim of the main algorithm of the present package (LArTTL1Maker) is the simulation of the LAr analogue trigger tower sums. <br>

   It includes correct allocation of cells to towers, pulse profile as a function of time, saturation, appropriate noise addition, pile-up. <br>
   The resulting signals are an input to the Preprocessor (which in turn performs digitization, filtering, bunch crossing id., noise suppression,...) <br>
   Since it needs hits, the simulation only takes "simul" datasets as input, NOT digitized datasets. <br>

There is a second algorithm, LArTTL1Calib, which is meant to be run in standalone in order to perform the energy calibration of the Trigger Towers.


@htmlinclude used_packages.html

@include requirements

*/
