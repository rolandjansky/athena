## Enable CalcHep LHEF reading in Pythia8
include("Pythia8_i/Pythia8_LHEF.py")
evgenConfig.generators += ["CalcHep", "Pythia8"]
