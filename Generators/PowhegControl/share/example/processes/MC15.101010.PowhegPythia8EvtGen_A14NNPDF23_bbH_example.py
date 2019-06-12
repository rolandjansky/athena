# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 bbH production with A14 NNPDF2.3 tune."
evgenConfig.keywords = ["SM", "bottom", "Higgs"]
evgenConfig.contact = ["james.robinson@cern.ch", "riccardo.di.sipio@cern.ch" ]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg bbH process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_bbH_Common.py")

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")
