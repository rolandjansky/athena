# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# This is an example joboption to generate events with Powheg
# using ATLAS' interface. Users should optimise and carefully
# validate the settings before making an official sample request.
#--------------------------------------------------------------

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 W+t (diagram removal) production with A14 NNPDF2.3 tune. Modified to allow for dynamic scale."
evgenConfig.keywords = ["SM", "W", "top"]
evgenConfig.contact = ["james.robinson@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg Wt_DR_Modified process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_Wt_DR_Modified_Common.py")


# --------------------------------------------------------------
# Additional settings
# --------------------------------------------------------------
PowhegConfig.runningscales = 1

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune, main31 routine
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg_Main31.py")


