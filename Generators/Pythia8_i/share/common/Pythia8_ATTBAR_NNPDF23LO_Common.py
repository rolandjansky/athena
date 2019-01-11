# Config for Py8 tune ATTBAR with NNPDF23LO                                                                                                                  
# if used for the standalone Pythia8 add the two following lines to enable the ISR damping:
# "SpaceShower:pTdampMatch = 1"
# "SpaceShower:pTdampFudge=1.18"
include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
    "Tune:ee = 7",
    "Tune:pp = 14",
#    "PDF:useLHAPDF=on",
#    "PDF:LHAPDFset = NNPDF23_lo_as_0130_qed",
    "TimeShower:alphaSvalue = 0.137",
    "TimeShower:pTmin = 1.26",
    "SpaceShower:alphaSvalue = 0.121",
    "MultipartonInteractions:pT0Ref = 2.16"]

rel = os.popen("echo $AtlasVersion").read()
print "Atlas release " + rel

if rel[:2].isdigit() and int(rel[:2])<20:
  ver =  os.popen("cmt show versions External/Pythia8").read()
  print "Pythia8 version: " + ver
  if ('Pythia8-01' in ver[:50]) or ('Pythia8_i-00-11' in ver):
   genSeq.Pythia8.Commands += [
    "PDF:useLHAPDF = on",
    "PDF:LHAPDFset = NNPDF23_lo_as_0130_qed"]
  else:
   genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:NNPDF23_lo_as_0130_qed"]                            
else:
   genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:NNPDF23_lo_as_0130_qed"]                            

evgenConfig.tune = "ATTBAR NNPDF23LO"

