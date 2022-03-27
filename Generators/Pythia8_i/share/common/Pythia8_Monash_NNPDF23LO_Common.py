# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

## Config for Py8 Monash tune
## This is not the default recommended tune, but forms the base for A14
## In principle it could be set with Tune:pp=14, but there are some problems with PDFs

include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
   "Tune:preferLHAPDF = 2",
   "Tune:pp = 14",
   "PDF:pSet = LHAPDF6:NNPDF23_lo_as_0130_qed"] # set this AFTER Tune:pp, to avoid crashed due to missing internal Pythia8 PDF data]

# set PDF set AFTER Tune:pp, to avoid crashes due to missing internal Pythia8 PDF data

rel = os.popen("echo $AtlasVersion").read()

evgenConfig.tune = "Monash"
