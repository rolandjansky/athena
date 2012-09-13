/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage Package InDetSimData
@author Davide.Costanzo@cern.ch

@section InDetSimData Introduction

This package contains the EDM classes that are used to store the MC truth information 
together with the RDOs. Association between these classes and the RDO classes should be 
made by associating the identifiers

These classes should NOT be changed, unless there is a very specific reason. In particular
events have been simulated and written out with pool since release 8 and a change in these
classes may result in a big loss (like all of DC2)



@section  Simulation data object associated with an InDetRawData object

 An object of this class stores the simulation information associated with 
 a RawData object, in two data members. 

 One is an integer, the "simulation data word", in which is packed 
 information summarizing the digitization. Its interpretation may depend on 
 which of the three subdetectors is concerned, but will typically contain 
 bit-flags for "noise", "lost in readout", etc.

 The other consists of a number (normally only one) of  pair<int, float> 
 specifying the barcode and energy (charge) deposited by a charged particle, 
 or rather that part of the charge attributed to the corresponding RDO.
 Encoding and decoding methods will be supplied outside this class.

 The implementation emphasizes compactness in memory, at the expense of speed,
 because objects of this class will be persistent, but will not be accessed
 in real production running. 
 
@section SimDataHelpers

  helers are provided to decode the information that is normally stored in a single word
in the Simulation Data Objects

*/
