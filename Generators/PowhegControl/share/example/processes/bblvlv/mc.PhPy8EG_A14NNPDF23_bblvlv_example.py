# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 bblvlv production with A14 NNPDF2.3 tune."
evgenConfig.keywords = ["SM", "WW", "2jet", "bottom"]
evgenConfig.contact = ["simone.amoroso@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg bblvlv process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_bblvlv_Common.py")

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg.py")
