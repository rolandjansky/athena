# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 Higgs+W+jet production with electroweak effects using the A14 NNPDF2.3 tune."
evgenConfig.keywords = ["Higgs", "SMHiggs", "W", "WHiggs", "1jet"]
evgenConfig.contact = ["simone.amoroso@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg HWj_EW process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_HWj_EW_Common.py")

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg.py")
