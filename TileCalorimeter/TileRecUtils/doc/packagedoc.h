/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**

This package contains the algorithms and tools used for TileCal reconstruction

@page TileRecUtils_page 

@author Alexander Solodkov <Sanya.Solodkov@cern.ch>
@author Tomas Davidek  <Tomas.Davidek@cern.ch>
@author Ximo Poveda <Ximo.Poveda@cern.ch>

@section TileRecUtils_Algos Algorithms
  
  The following algorithms are present in this package:

    - TileRawChannelMaker: builds TileRawChannels from TileDigits by making use of one or several of the following tools: TileRawChannelBuilderFlatFilter, TileRawChannelBuilderFitFilter, TileRawChannelBuilderFitFilterCool, TileRawChannelBuilderOptFilter, TileRawChannelBuilderOpt2Filter,TileRawChannelBuilderManyAmps.

  - TileRawChannelVerify: this algorithm compares two sets of  TileRawChannels (obtained from different data and/or reconstructed by different methods) dumping differences in the console. 


@section TileRecUtils_ATools AlgTools

  These are the tools in the TileRecutils package:

  - TileRawChannelBuilderFlatFilter: used by TileDigitsMaker. TileRawChannels are built using the Flat Filtering method, based on the integration of the pulse shapes.

  - TileRawChannelBuilderFitFilter: used by TileDigitsMaker. TileRawChannels are built using the Fit method, based on fit of the pulse samples minimizing the noise contribution. Default mehtod used for testbeam and cosmics reconstruction until 2008.

  - TileRawChannelBuilderFitFilterCool: used by TileDigitsMaker. TileRawChannels are built using the Fit method, based on fit of the pulse samples minimizing the noise contribution. Pulse shapes are loaded from COOL DB.

  - TileRawChannelBuilderOptFilter: used by TileDigitsMaker. TileRawChannels are built using the Optimal Filtering method, based on linear combinations of the samples minimizing the noise contribution. Two version of the method are used: OF1 (which computes amplitude and phase) and OF2 (which computes amplitude, phase and pedestal)

  - TileRawChannelBuilderOpt2Filter: used by TileDigitsMaker. TileRawChannels are built using the Optimal Filtering method, based on linear combinations of the samples minimizing the noise contribution. Two version of the method are used: OF1 (which computes amplitude and phase) and OF2 (which computes amplitude, phase and pedestal). The Opt2Filter makes several iterations, so the time doesn't need to be known in advance. Default method for cosmics since 2008.

  - TileRawChannelBuilderManyAmps: used by TileDigitsMaker. TileRawChannels are built using the method developed by Frank Marritt, based on fitting the digital samples to to a series of pulses (for pileup handling). Default method for full ATLAS reconstruction.

  - TileCellBuilder: creates TileCells from TileRawChannels, which are stored in a container. By default uses Opt2 Optimal Filter, and on-th-fly cell masking.

  - TileCellFakeProb: scales down the energy of TileCells due to simulated failure of drawers
  
  - TileCellMaskingTool:  interface to for TileCellBuilder to information on which hardware parts (channel, DMU, digitizer, MB, drawer) to maskout.

  - TileTowerBuilderTool: creates Tower-type cell clusters to be used in jet reconstruction


*/
