## Config for Py8 tune A14 with NNPDF23LO Var3c Down
## The default version of this tune fragment include EvtGen for standardised b fragmentation

# Reference the non-standard version without EvtGen
include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3cDown_Common.py")

# Add EvtGen for b fragmentation as default.  No EvtGen is available in "nonStandard"
include("Pythia8_i/Pythia8_EvtGen.py")

