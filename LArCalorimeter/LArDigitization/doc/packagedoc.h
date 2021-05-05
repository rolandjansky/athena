/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**

@page LArDigitization_page LArDigitization Package

This package contains the LArDigitization algorithm used to create LArDigits objects
from LArHits

@section LArDigitization_LArDigitMaker LArDigitMaker class

This is the digitization algorithm for the LAR.
The input data are the LArHit from the simulation (visible energy in LAr).
The output data are LArDigit representing the output of the front-end electronis (5 ADC time sample
per channel).
Electronic noise and pileup effects can be included.

The default jobOptions fragment to run digitization on G4 sample is
share/LArDigitization_G4_jobOptions.py

The list of the properties of the algorithm with their explanation can be found
in the LArDigitMaker::LArDigitMaker.



@section LArDigitization_LArHitEMap LArHitEMap and LArHitList

These are auxiliary classes used by LArDigitMaker to store the hit energies per channel:
LArHitEMap is a vector (indexed by cell hash identifier) of LArHitList.
LArHitList contains the list of Energy and Time of the hits contributing to a given cell.

*/
