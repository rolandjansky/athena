# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# --------------------------------------------------------------
# EVGEN configuration
# --------------------------------------------------------------
evgenConfig.description = "PowhegControl example of VBFNLO"
evgenConfig.keywords = ["SM", "VBF", "W"]
evgenConfig.contact = ["james.robinson@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg VBF_W process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_VBF_W_Common.py")

# --------------------------------------------------------------
# Set integration parameters to tiny values for fast testing
# DO NOT USE THESE SETTINGS IN PRODUCTION!
# --------------------------------------------------------------
PowhegConfig.ncall1 = 1000
PowhegConfig.ncall2 = 1000
PowhegConfig.nubound = 1000
PowhegConfig.foldphi = 1
PowhegConfig.foldcsi = 1
PowhegConfig.foldy = 1
PowhegConfig.itmx1 = 1
PowhegConfig.itmx2 = 1

# --------------------------------------------------------------
# Set process options and generate
# --------------------------------------------------------------
PowhegConfig.decay_mode = "w+ > e+ ve"
PowhegConfig.generate()

# --------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
# --------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg.py")
