## Config for Py8 tune A14 with NNPDF23LO
## This is the version without EvtGen, and as such is not the standard.  
## The default version is available in common/Pythia8/Pythia8_A14_NNPDF23LO_EvtGen_Common.py

include("Pythia8_i/Pythia8_Base_Fragment.py")

# todo - replace BeamRemnants with new ColourReconnection syntax once Pythia 8.201 is in place
genSeq.Pythia8.Commands += [
    "Tune:ee = 7", 
    "Tune:pp = 14",
    "SpaceShower:rapidityOrder = on",
    "SigmaProcess:alphaSvalue = 0.140",
    "SpaceShower:pT0Ref = 1.56",
    "SpaceShower:pTmaxFudge = 0.91",
    "SpaceShower:pTdampFudge = 1.05",
    "SpaceShower:alphaSvalue = 0.127",
    "TimeShower:alphaSvalue = 0.127",
    "BeamRemnants:primordialKThard = 1.88",
    "MultipartonInteractions:pT0Ref = 2.09",
    "MultipartonInteractions:alphaSvalue = 0.126"]

rel = os.popen("echo $AtlasVersion").read()
print "Atlas release " + rel

if rel[:2].isdigit() and int(rel[:2])<20:
  ver =  os.popen("cmt show versions External/Pythia8").read()
  print "Pythia8 version: " + ver
  if ('Pythia8-01' in ver[:50]) or ('Pythia8_i-00-11' in ver):
   genSeq.Pythia8.Commands += [
    "PDF:useLHAPDF = on",
    "PDF:LHAPDFset = NNPDF23_lo_as_0130_qed",
    "BeamRemnants:reconnectRange = 1.71" ]
  else:
   genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:NNPDF23_lo_as_0130_qed",
    "ColourReconnection:range = 1.71"]                            
else:
   genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:NNPDF23_lo_as_0130_qed",
    "ColourReconnection:range = 1.71"]  

include("Pythia8_i/Pythia8_RapidityOrderMPI.py")

evgenConfig.tune = "A14 NNPDF23LO"
