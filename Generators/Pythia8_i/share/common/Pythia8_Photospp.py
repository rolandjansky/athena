## Photos++ QED config for Pythia8

## Disable native QED FSR
assert hasattr(genSeq, "Pythia8")
genSeq.Pythia8.Commands += ["TimeShower:QEDshowerByL = off"]

print ("time shower cut active")
genSeq.Pythia8.Commands += ["TimeShower:QEDshowerByOther = off"]

## Enable Photos++
include("Photospp_i/Photospp_Fragment.py")
