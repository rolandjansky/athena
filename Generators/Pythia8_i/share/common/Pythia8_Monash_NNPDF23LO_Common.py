## Config for Py8 Monash tune 
## This is not the default recommended tune, but forms the base for A14
## In principle it could be set with Tune:pp=14, but there are some problems with PDFs

include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
   "PDF:pSet = LHAPDF6:NNPDF23_lo_as_0130_qed"]

rel = os.popen("echo $AtlasVersion").read()
print "Atlas release " + rel

if rel[:2].isdigit() and int(rel[:2])<20:
  ver =  os.popen("cmt show versions External/Pythia8").read()
  print "Pythia8 version: " + ver
  if 'Pythia8-01' in ver[:50]:
   genSeq.Pythia8.Commands += [
    "PDF:useLHAPDF=on",
    "PDF:LHAPDFset = NNPDF23_lo_as_0130_qed"]
  else:
   genSeq.Pythia8.Commands += ["PDF:pSet = LHAPDF6:NNPDF23_lo_as_0130_qed"] 
else:
  genSeq.Pythia8.Commands += ["PDF:pSet = LHAPDF6:NNPDF23_lo_as_0130_qed"]
evgenConfig.tune = "Monash"
