# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# This is an example joboption to generate events with Powheg
# using ATLAS' interface. Users should optimise and carefully
# validate the settings before making an official sample request.
#--------------------------------------------------------------

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 tt+jet production with A14 NNPDF2.3 tune."
evgenConfig.keywords = ["SM", "top", "1jet"]
evgenConfig.contact = ["james.robinson@cern.ch", "stefan.richter@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg ttj process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_ttj_Common.py")

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune, main31 routine
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg_Main31.py")
# Setting the appropriate number of final state particles for the main31 routine
genSeq.Pythia8.Commands += [ 'Powheg:NFinal = 3' ]
