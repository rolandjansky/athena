/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TileRec Package

This package provides tools that send containers of different formats into
a ntuple format.

@author Alexander Solodkov <Sanya.Solodkov@cern.ch>
@author Jose Maneira <maneira@lip.pt>
@author Luca Fiorini <Luca.Fiorini@cern.ch>

@section TileCal commissioning ntuple

TileAANtuple class creates so-called h2000 ntuple which 
contains all ditigts, all rawChannels (both results of 
optimal filter in Athena and DSP results), DQ status words etc
most of the arrays have dimensions 4x64x48 which correspond
to 4 partitions, 64 modules, 48 channels.

@section standalone ntuples

The remaining classes implement the output into standalone
ntuples for the following containers:
TileCell, TileRawChannel, TileTTL1, TileCellIDC (see
the TileEvent package), TileHit, TileHitVector (see the
TileSimEvent package).
The TileTBHitToNtuple outputs to ntuple specific instances of
TileHitVector, dedicated to ancillary detectors used in
test-beam, as well as the MBTS hits.

These classes are presently used for standalone Tile reconstruction
only.
*/
