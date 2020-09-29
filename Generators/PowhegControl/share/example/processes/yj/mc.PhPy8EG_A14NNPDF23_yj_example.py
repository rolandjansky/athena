# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 direct photon plus jet production with A14 NNPDF2.3 tune."
evgenConfig.keywords = ["SM", "QCD", "QED", "photon"]
evgenConfig.contact = ["stefan.richter@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg yj process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_yj_Common.py")

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg.py")
