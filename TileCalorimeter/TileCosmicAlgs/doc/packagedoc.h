/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TileCosmicAlgs_page TileCosmicAlgs Package

This package contains the algorithms used for TileCal cosmic 
muons reconstruction, analysis, and trigger simulation.

@author Jose Maneira <maneira@lip.pt>

@section TileCosmicAlgs_Algos Algorithms
  
  The following algorithms are present in this package:

  - TileMuonFitter: reads TileCells, fitting a straight muon track through and calculating horizontal plane crossing time. More details in https://twiki.cern.ch/twiki/bin/view/Atlas/TileMuonFitter. Two different track finding methods are used: a standard chi-square Minuit-based algorithm, and a "pattern-recongnition-style" Hough transform based one (by Luciano Filho). Two types of output are created in StoreGate: a "non-Tile-dependent" Commission/ComTime object for retrieval by LAr and Monitoring packages, and a TileEvent/TileCosmicMuon object with much more detailed information, such as the energy in the cells around the track, the path length travelled, etc.
  Until February 2009, TMF read TileRawChannels and created its own cell vector. This was very slow
  and is now fixed by reading the cells from CaloCellContainer.

  - TileMuonTrackDistance: is an auxiliary class for the Minuit-based TileMuonFitter algorithm. It provides the chi-square function for minimization od the orthogonal distances from the cell centers to the track.
    
  - CBNTAA_TileMuonFitter: creates an AthenaAware CBNT ntuple with TileMuonFitter information.

  - TileCosmicsTrigger: used in simulation to emulate the standalone Tile cosmics trigger. It reads the container of TileTTL1 objects (see TileEvent and TileSimAlgs), apllies the trigger logic according to the board configuration and produces a TileTrigger object, put in the StoreGate. More details on the trigger logic, usage, etc, can be found here: https://twiki.cern.ch/twiki/bin/view/Main/TileCosmicsTrigger  The output can be put in CBNTAA format (see the TileRec package).
*/
