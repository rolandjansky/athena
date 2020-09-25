# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 top pair production with A14 NNPDF2.3 tune."
evgenConfig.keywords = ["SM", "top"]
evgenConfig.contact = ["simone.amoroso@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg tt process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_tt_Common.py")

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg.py")
