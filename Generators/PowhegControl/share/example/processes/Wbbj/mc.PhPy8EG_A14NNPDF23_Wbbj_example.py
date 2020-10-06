# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

s#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 Wbbj production with A14 NNPDF2.3 tune."
evgenConfig.keywords = ["SM", "W", "jets", "3jet", "bottom"]
evgenConfig.contact = ["simone.amoroso@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg Wbbj process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_Wbbj_Common.py")

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg.py")
