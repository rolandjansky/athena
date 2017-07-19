/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Mainpage for doxygen

/**
@page FastChainPileup_page package FastChainPileup

@author Richard Hawkings, Vladimir Lyubushkin

This package provides code related to pileup emulation in the fast simulation
chain.

@section FastChainPileup_MultiPy8Pileup 

This algorithm runs Pythia8 to generate multiple pileup events into a 
single McEventCollection with a specified key. It inherits from Pythia8_i
which does the real work of interfacing to the Pythia8 generator, and just
provides the code to call the generator a number of times, according to
a given pileup profile.

The algorithm has the following parameters:

 - NCollPerEvent: The mean number of pileup collisions per event (bunch 
crossing), known as mu.If this is set to a positive value, the givem fixed mu
is used. If it is set to -1, the pileup profile specified by PileupProfile
is used instead.

 - PileupProfile: An array of integers, giving the histogram for the mu
distribution to be simulated.

 - MultBCID: An array of floats, giving the mu multiplier values R for BCIDs 1, 2, 3.. Events will be generated for the number of BCIDs given (numbered from 1),
and for each BCID, the number of events will be sampled from a Poisson 
distribution with the mean mu chosen from the pileup profile, then multiplied
by the multilplier R. Thus an array of [1.0, 1.5] will produce in-time
pileup with BCID=1 according to mu, and then 1.5* this amount of out-of-time
pileup with BCID=2. The different BCIDs can then be treated differently
using the ISF simulation selectors.

 - HistFile: If this is set to a non-null value, a ROOT file with the 
corresponding name will be produced, with three histograms: pileupProfile giving
the input profile, generatedMu giving the generated mu profile (it should
be compatible with the input profile) and generatedEvents, which shows the
distribution of number of pileup events - equivalent to the mu profile but
with the additional Poisson fluctuations, filled for nevt values for each 
BCID (but before multiplying by the R values).


*/
