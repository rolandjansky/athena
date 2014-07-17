/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TileRecAlgs Package

This package provides format conversion methods for reconstructed objects.

@author Alexander Solodkov <Sanya.Solodkov@cern.ch>

@section Format conversion tools

  - The TileBeamElemToCell algorithm is used for test beam data to convert additional detector information into a convenient TileCell format.

  - The TileDigitsToTTL1 and TileRawChannelToTTL1 algorithms are used in test beam reconstruction to create a fake trigger output (TTL1) from the samples (Digits) or the reconstructed energy (RawChannel).

  - The TileCellIDCToCell is presently not used. It converts the IdentifiableContainer format (created directly in ByteStream, without passing through TileRawChannel) into the CaloCellContainer format.

  - The TileRawChannelToHit algorithm is used to convert data in RawChannel format to a fake TileHit, to be used as input to the v-atlas event display tool, that needs the input in Hit format.

  - Thie TileCellToTTL1 algorithm is used to build the L1 trigger tower from the Tile Cell energy. This is useful for L1Calo to compare their tower energy to the tower energy from the Tile digitial readout. 

@section Aditional tools

  - The TileCellVerify algorithm is used to compare TileCell objects created with TileCellIDCToCell with those created following the standard way, through TileCellBuilder (see the TileRecUtils package).
  
  - The TileDigitsFilter algorithm is used to make a filtered copy of a TileDigitsContainer. In the output object are put only the channels for which the maximum and minimum value of the samples is above a certain threshold.




*/
