## Config for Py8 tune A3 with NNPDF23LO tune
## The default version of this includes EvtGen for standardised b hadronization
## This tune is generally only used for pile up samples in run 2 - for high pT physics at the start of run 2 the A14 tune is more appropriate.  

# Reference the non-standard version without EvtGen
include("Pythia8_i/Pythia8_A3_NNPDF23LO_Common.py")

# Add EvtGen for b fragmentation as default.  No EvtGen is available in "nonStandard"
include("Pythia8_i/Pythia8_EvtGen.py")

