## Photos++ QED config for Pythia8

## Disable native QED FSR
assert hasattr(topAlg, "Pythia8")
topAlg.Pythia8.Commands += ["TimeShower:QEDshowerByL = off"]

## Enable Photos++
#include("MC12JobOptions/Photospp_Fragment.py")
include("common/Photospp_Fragment.py")
