## Config for Py8 tune AZNLO with CTEQ6L1
## The default version of this tune fragment include EvtGen for standardised b fragmentation

# Reference the non-standard version without EvtGen
include("Pythia8_i/Pythia8_AZNLO_CTEQ6L1_Common.py")

# Add EvtGen for b fragmentation as default.  No EvtGen is available in "nonStandard"
include("Pythia8_i/Pythia8_EvtGen.py")

