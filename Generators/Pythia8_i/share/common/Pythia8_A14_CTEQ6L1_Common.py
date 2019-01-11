## Config for Py8 tune A14 with CTEQ6L1
include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
    "Tune:ee = 7", 
    "Tune:pp = 14",
#    "PDF:useLHAPDF = on",
#    "PDF:LHAPDFset = cteq6ll",
#    "PDF:pSet=LHAPDF6:cteq6l1",  
    "SpaceShower:rapidityOrder = on",
    "SigmaProcess:alphaSvalue = 0.144",
    "SpaceShower:pT0Ref = 1.30",
    "SpaceShower:pTmaxFudge = 0.95",
    "SpaceShower:pTdampFudge = 1.21",
    "SpaceShower:alphaSvalue = 0.125",
    "TimeShower:alphaSvalue = 0.126",
    "BeamRemnants:primordialKThard = 1.72",
    "MultipartonInteractions:pT0Ref = 1.98",
    "MultipartonInteractions:alphaSvalue = 0.118",
    "ColourReconnection:range = 2.08" ]

rel = os.popen("echo $AtlasVersion").read()
print "Atlas release " + rel

if rel[:2].isdigit() and int(rel[:2])<20:
  ver =  os.popen("cmt show versions External/Pythia8").read()
  print "Pythia8 version: " + ver
  if ('Pythia8-01' in ver[:50]) or ('Pythia8_i-00-11' in ver):
   genSeq.Pythia8.Commands += [
    "PDF:useLHAPDF = on",
    "PDF:LHAPDFset = cteq6ll" ]
  else:
   genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:cteq6l1"]                            
else:
   genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:cteq6l1"]                            

evgenConfig.tune = "A14 CTEQ6L1"
