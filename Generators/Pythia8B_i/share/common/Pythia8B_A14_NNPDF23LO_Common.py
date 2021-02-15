## Config for Py8B tune A14 with NNPDF23LO
## This is the version without EvtGen, and as such is not the standard.  
## The default version is available in common/Pythia8B/Pythia8B_A14_NNPDF23LO_EvtGen_Common.py

include("Pythia8B_i/Pythia8B_Base_Fragment.py")

# todo - replace BeamRemnants with new ColourReconnection syntax once Pythia 8.201 is in place
ver =  os.popen("cmt show versions External/Pythia8").read()
print "Pythia8 version: " + ver
if 'Pythia8-01' in ver[:50]:
  genSeq.Pythia8B.Commands += [
    "Tune:ee = 7", 
    "Tune:pp = 14",
    "PDF:useLHAPDF = on",
    "PDF:LHAPDFset = NNPDF23_lo_as_0130_qed",
    "SpaceShower:rapidityOrder = on",
    "SigmaProcess:alphaSvalue = 0.140",
    "SpaceShower:pT0Ref = 1.56",
    "SpaceShower:pTmaxFudge = 0.91",
    "SpaceShower:pTdampFudge = 1.05",
    "SpaceShower:alphaSvalue = 0.127",
    "TimeShower:alphaSvalue = 0.127",
    "BeamRemnants:primordialKThard = 1.88",
    "MultipartonInteractions:pT0Ref = 2.09",
    "MultipartonInteractions:alphaSvalue = 0.126",
    "BeamRemnants:reconnectRange  = 1.71"]
else:
  genSeq.Pythia8B.Commands += [
    "Tune:ee = 7",
    "Tune:pp = 14",
    "PDF:pSet=LHAPDF6:NNPDF23_lo_as_0130_qed",
    "SpaceShower:rapidityOrder = on",
    "SigmaProcess:alphaSvalue = 0.140",
    "SpaceShower:pT0Ref = 1.56",
    "SpaceShower:pTmaxFudge = 0.91",
    "SpaceShower:pTdampFudge = 1.05",
    "SpaceShower:alphaSvalue = 0.127",
    "TimeShower:alphaSvalue = 0.127",
    "BeamRemnants:primordialKThard = 1.88",
    "MultipartonInteractions:pT0Ref = 2.09",
    "MultipartonInteractions:alphaSvalue = 0.126",
    "ColourReconnection:range = 1.71"]

evgenConfig.tune = "A14 NNPDF23LO"
