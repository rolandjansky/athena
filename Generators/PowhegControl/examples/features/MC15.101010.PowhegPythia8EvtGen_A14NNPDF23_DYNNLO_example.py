# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 W+j production with A14 NNPDF2.3 tune"
evgenConfig.keywords = ["SM", "W", "1jet"]
evgenConfig.contact = [ "james.robinson@cern.ch" ]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg Wj process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_Wj_Common.py")

# --------------------------------------------------------------
# Set integration parameters to tiny values for fast testing
# DO NOT USE THESE SETTINGS IN PRODUCTION!
# --------------------------------------------------------------
PowhegConfig.ncall1 = 1000
PowhegConfig.ncall1rm = 1000
PowhegConfig.ncall2 = 1000
PowhegConfig.ncall2rm = 1000
PowhegConfig.nubound = 1000
PowhegConfig.foldphi = 1
PowhegConfig.foldcsi = 1
PowhegConfig.foldy = 1
PowhegConfig.itmx1 = 1
PowhegConfig.itmx2 = 1

# --------------------------------------------------------------
# DYNNLO: specify the input reweighting file
# --------------------------------------------------------------
PowhegConfig.NNLO_reweighting_inputs["DYNNLO"] = "Wp_CM8_MMHT14NNLO_11.top"

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

# --------------------------------------------------------------
# Pythia8 showering with the AZNLO tune
# --------------------------------------------------------------
include("MC15JobOptions/Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")
