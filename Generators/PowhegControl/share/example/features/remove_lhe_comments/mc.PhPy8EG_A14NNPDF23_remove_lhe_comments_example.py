# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 top pair production with A14 NNPDF2.3 tune."
evgenConfig.keywords = ["SM", "top"]
evgenConfig.contact = ["tpelzer@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg tt process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_tt_Common.py")

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
# the lines such as #rwgt, #pdf, #new weight, or #Random are by default kept in the lhe file
# if they look buggy, they are fixed by PowhegControl
# the argument remove_oldStyle_rwt_comments=True of generate removes them from the lhe file
# remove_oldStyle_rwt_comments is False by default
# see AGENE-1892 for details
# --------------------------------------------------------------
PowhegConfig.generate(remove_oldStyle_rwt_comments=True)

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg.py")
