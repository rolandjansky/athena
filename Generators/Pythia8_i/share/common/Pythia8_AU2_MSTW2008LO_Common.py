## Config for Py8 tune A2 with CTEQ6L1

include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
    "Tune:pp = 5",
#    "PDF:useLHAPDF = on",
#    "PDF:LHAPDFset = MSTW2008lo68cl.LHgrid",
#    "PDF:pSet=LHAPDF6:MSTW2008lo68cl",
    "MultipartonInteractions:bProfile = 4",
    "MultipartonInteractions:a1 = 0.01",
    "MultipartonInteractions:pT0Ref = 1.87",
    "MultipartonInteractions:ecmPow = 0.28",
#    "BeamRemnants:reconnectRange = 5.32",
    "SpaceShower:rapidityOrder=0"]

rel = os.popen("echo $AtlasVersion").read()
print "Atlas release " + rel

if rel[:2].isdigit() and int(rel[:2])<20:
  ver =  os.popen("cmt show versions External/Pythia8").read()
  print "Pythia8 version: " + ver
  if 'Pythia8-01' in ver[:50]:
   genSeq.Pythia8.Commands += [
    "PDF:useLHAPDF = on",
    "PDF:LHAPDFset = MSTW2008lo68cl.LHgrid",
    "BeamRemnants:reconnectRange = 5.32"]
  else:
   genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:MSTW2008lo68cl",
   "ColourReconnection:range = 5.32"] 
else:
   genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:MSTW2008lo68cl",
   "ColourReconnection:range = 5.32"]

include("Pythia8_i/Pythia8_RapidityOrderMPI.py")

evgenConfig.tune = "AU2 MSTW2008LO"
