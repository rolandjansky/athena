## Config for Py8 tune A14 FSR 2-loop alpha_s evolution with NNPDF23LO

## Baseline A14 tune with EvtGen
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

genSeq.Pythia8.Commands += ["TimeShower:alphaSorder=2"]
