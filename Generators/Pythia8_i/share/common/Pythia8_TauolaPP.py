## TAUOLA config for PYTHIA

## Disable native tau decays
assert hasattr(genSeq, "Pythia8")
genSeq.Pythia8.Commands += ["15:onMode = off"]

## Enable TAUOLA
include("Tauolapp_i/TauolaPP_Fragment.py")


