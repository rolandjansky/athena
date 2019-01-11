## Config for Py8 tune A2 with MSTW2008LO
## This is the version without EvtGen, but the default is to use EvtGen
## The default version is in common/Pythia8/Pythia8_A2_MSTW2008LO_Common.py
## PDF syntax depending on  Pythia8 version

include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
    "Tune:pp = 5",
    "MultipartonInteractions:bProfile = 4",
    "MultipartonInteractions:a1 = 0.03",
    "MultipartonInteractions:pT0Ref = 1.90",
    "MultipartonInteractions:ecmPow = 0.30",
#    "BeamRemnants:reconnectRange = 2.28",
    "SpaceShower:rapidityOrder=0"]

rel = os.popen("echo $AtlasVersion").read()
print "Atlas release found " + rel

if rel[:2].isdigit() and int(rel[:2])<20:
  ver =  os.popen("cmt show versions External/Pythia8").read()
  print "Pythia8 version: " + ver
#  ver = os.popen("acmake.py show_versions Generators/Pythia8_i").read()
  if ('Pythia8-01' in ver[:50]) or ('Pythia8_i-00-11' in ver):	
    genSeq.Pythia8.Commands += [
    "PDF:useLHAPDF = on",
    "PDF:LHAPDFset = MSTW2008lo68cl.LHgrid",
    "BeamRemnants:reconnectRange = 2.28"]
  else:
    genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:MSTW2008lo68cl",
                                "ColourReconnection:range=2.28"]
else:
  genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:MSTW2008lo68cl",
                              "ColourReconnection:range=2.28"]

include("Pythia8_i/Pythia8_RapidityOrderMPI.py")

evgenConfig.tune = "A2 MSTW2008LO"
