/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

This package contains the algorithms used for TileCal simulation

@page TileSimAlgs_page 

@author Alexander Solodkov <Sanya.Solodkov@cern.ch>
@author Ximo Poveda <Ximo.Poveda@cern.ch>
@author Simon Molander <Simon.Molander@cern.ch>

@section TileSimAlgs_Algos Algorithms
  
  The following algorithms are present in this package:
  
  - TileDigitsFromPulse: Builds TileDigits from Simulates pulses. The pulses have either a fixed amplitude or a random one following a distribution. Noise, channel-to-channel phase variation, pulse shape broadening and pile-up are added according to parameter specifications.

  - TileDigitsMaker: builds TileDigits from TileHits. During its execution TileHits are retrieved from a TileHitContainer and TileDigits are produced after applying digitizer pulse shapes, noise and photostatistic effects. A filtered container is also produced with only the channels above, a threshold on the hit sum energy.

  - TileHitToCell: builds TileCells from TileHits. Hits from the PMTs in the cell are merged and calibration constants applied to obtained cell energy, time and quality.
  
  - TileHitToRawChannel: builds TileRawChannels from TileHits. TileRawChannel amplitude, time and quality are obtianed from the TileHits. Noise is applied to the amplitude.

  - TileHitToTTL1: build TileTTL1 from TileHits. Hits from all PMTs are convolved with the adder pulse shape, combined to form trigger towers and have random noise added. Pulses are digitized for full ATLAS calorimeter trigger and N 25ns samples are stored for each tower. This
is the default mode of running. It is possible to specify the 'cosmic' mode of running which stores only the pulse maximum for each tower (this mode was used for pre-beam commissioning).

  - TileHitVecToCnt: merges TileHits from differentAthenaHitVectors and store result in TileHitContainer. Time shift may be applied to the hits if needed. In PileUp simulation, all pileup hits are added taking into account their time shift (..,-50,-25,0,25,50,...). This way, other algorithms uses TileHitContainer handling pileup trasparently.

  - TileTBHitToBeamElem: builds TileBeamElements from TileHits for testbeam simulation.

*/
