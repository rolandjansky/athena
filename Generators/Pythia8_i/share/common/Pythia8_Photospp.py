## Photos++ QED config for Pythia8

## Disable native QED FSR
assert hasattr(genSeq, "Pythia8")
genSeq.Pythia8.Commands += ["TimeShower:QEDshowerByL = off"]

#ver =  os.popen("cmt show versions External/Pythia8").read()
#print "Pythia8 version: " + ver
#if ('Pythia8-23' in ver[:50]):
## disable radioation from "resonances" - AGENE1637
# to switch on when PYTHIA8VER available in the release

if "PYTHIA8VER" in os.environ:
  verph8str = str(os.environ['PYTHIA8VER'])[:3]
  verph8 = int(verph8str)
  print("verph8 ", verph8)
  if (verph8 > 218):
     print "time shower cut active"
     genSeq.Pythia8.Commands += ["TimeShower:QEDshowerByOther = off"]

## Enable Photos++
include("Photospp_i/Photospp_Fragment.py")
