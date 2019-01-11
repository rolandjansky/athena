## Config for Py8B tune A14 with CTEQ6L1
include("Pythia8B_i/Pythia8B_Base_Fragment.py")

ver =  os.popen("cmt show versions External/Pythia8").read()
print "Pythia8 version: " + ver
if 'Pythia8-01' in ver[:50]:
  genSeq.Pythia8B.Commands += [
    "Tune:ee = 7", 
    "Tune:pp = 14",
    "PDF:useLHAPDF = on",
    "PDF:LHAPDFset = cteq6ll",
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
    "BeamRemnants:reconnectRange = 2.08"]
else:
  genSeq.Pythia8B.Commands += [
    "Tune:ee = 7", 
    "Tune:pp = 14",
    "PDF:pSet=LHAPDF6:cteq6l1",
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
    "ColourReconnection:range = 2.08"]

evgenConfig.tune = "A14 CTEQ6L1"
