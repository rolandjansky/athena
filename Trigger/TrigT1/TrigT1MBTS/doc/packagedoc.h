/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigT1MBTS_page TrigT1MBTS Package

@author William H. Bell <w.bell@physics.gla.ac.uk>

@section TrigT1MBTS_TrigT1MBTS_Overview Overview

This package simulates the level 1 MBTS trigger electronics and provides 
input for the CTP simulation.  In hardware the 32 trigger inputs from 
the CTP simulation are fed into a 32 channel discriminator.  The logic 
output result from all 32 channels are then fed into the CTP input board.  
The CTP input board calculates the multiplicity either side and passes 
this into the core module.  This package therefore simply contains the 
code to perform the discrimination on the 32 channels and creates the CTP 
input object MbtsCTP.  The MbtsCTP contains two 16bit words: these bits 
correspond to the 32 MBTS channels.


*/
