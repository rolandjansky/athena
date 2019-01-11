## Config for Py8 with NNPDF23 QED pdf

include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
   "PDF:pSet= LHAPDF6:NNPDF23_nnlo_as_0118_qed"
]

evgenConfig.tune = "NNPDF23_QED"

#EvtGen for b fragmentation as default.  No EvtGen is available in "nonStandard"
include("Pythia8_i/Pythia8_EvtGen.py")

