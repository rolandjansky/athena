## Config for Py8 MonashStar tune (aka CUETP8M1-NNPDF2.3LO aka CMS tune)
## This is not a default recommended tune but is being used for validation studies
## and tune comparisons

include("Pythia8_i/Pythia8_Base_Fragment.py")
genSeq.Pythia8.Commands += ["Tune:ee = 7",
                            "Tune:pp = 18"]

rel = os.popen("echo $AtlasVersion").read()
print "Atlas release " + rel

if rel[:2].isdigit() and int(rel[:2])<20:
  ver =  os.popen("cmt show versions External/Pythia8").read()
  print "Pythia8 version: " + ver
  if 'Pythia8-01' in ver[:50]:
   genSeq.Pythia8.Commands += ["PDF:useLHAPDF = on",
                             "PDF:LHAPDFset = NNPDF23_lo_as_0130_qed"]
  else:
   genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:NNPDF23_lo_as_0130_qed"]
else:
  genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:NNPDF23_lo_as_0130_qed"]

evgenConfig.tune = "MonashStar"
