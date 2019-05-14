# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 VBF opposite-sign WW production with A14 NNPDF2.3 tune."
evgenConfig.keywords = ["SM", "diboson", "WW", "VBF"]
evgenConfig.contact = ["stefan.richter@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg VBF_osWW process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_VBF_osWW_Common.py")

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")
