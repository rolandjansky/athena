/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TileTBRec_page TileTBRec Package

This package contains algorithm used for testbeam and calibration runs reconstruction. It contains algorithms to dump ByteStream data, produce run statistics and to create testbeam ntuples used by TileCal offline data quality group (Team5).


@author Alexander Solodkov <Sanya.Solodkov@cern.ch>
@author Luca Fiorini <Luca.Fiorini@cern.ch>
@author Ximo Poveda <Joaquin.Poveda@cern.ch>

@section TileTBRec_TileTBNtuple 
Obsolete for releases > 13.0.10. Use  TileTBAANtuple, instead.

@section TileTBRec_TileTBAANtuple 
TileTBAANtuple algorithm creates ntuples for testbeam and calibration runs. Ntuple variables are allocated dynamically depending on the bytestream data. 
The typical ntuple would contain Tile Digits informations (samples ADC counts, BCID, CRC), RawChannels informations, Trigger and General informations.
TileTBAANtuple provides the same informations as the former TileTBNtuple, but it writes to an THistSvc NTuple instead of a NtupleSvc ntuple. It is fully functional. It doesn't inherit from CBNTAA. It uses arrays instead of vectors to be fully compatible with the ROOT macros used in the past for TileTBNtuple and because in ROOT arrays are easier and faster to handle than vectors. 

@section TileTBRec_TileTBDump 
TileTBDump produces a formatted dump of Tile ByteStream fragments.
In particular DSPReco and Digits fragments are dumped.

@section TileTBRec_TileTBStat 
TileTBStat produce statistics from the Byte Stream data fragment. This algorithm is used to put run statistics in TileCal commissioning database.

@section TileTBRec_TileTBAna 
Obsolete algorithm to produce a testbeam summary ntuple.


*/
