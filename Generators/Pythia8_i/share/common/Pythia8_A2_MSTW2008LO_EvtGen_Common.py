## Config for Py8 tune A2 with MSTW2008LO tune
## The default version of this includes EvtGen for standardised b fragmentation
## This tune is generally only used for pile up samples at the start of run 2 - for high pT physics at the start of run 2 the A14 tune is more appropriate.  
## There are also more recent soft QCD tunes, such as Monash, but A2 was a conservative choice for initial 13 TeV pile up

# Reference the non-standard version without EvtGen
include("Pythia8_i/Pythia8_A2_MSTW2008LO_Common.py")

# Add EvtGen for b fragmentation as default.  No EvtGen is available in "nonStandard"
include("Pythia8_i/Pythia8_EvtGen.py")
