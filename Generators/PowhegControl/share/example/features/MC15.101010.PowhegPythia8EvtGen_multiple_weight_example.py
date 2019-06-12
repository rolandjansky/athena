# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# --------------------------------------------------------------
# EVGEN configuration
# --------------------------------------------------------------
evgenConfig.description = "PowhegControl example of multiple weights"
evgenConfig.keywords    = [ "SM", "QCD", "2jet", "bottom" ]
evgenConfig.contact = ["james.robinson@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg bb process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_bb_Common.py")

# --------------------------------------------------------------
# Set process options and generate
# --------------------------------------------------------------
PowhegConfig.PDF = (10800, 10853)
PowhegConfig.mu_F = [ 1.0, 0.5, 0.5, 1.0, 1.0, 2.0, 2.0 ]
PowhegConfig.mu_R = [ 1.0, 0.5, 1.0, 0.5, 2.0, 1.0, 2.0 ]
PowhegConfig.generate()

# --------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
# --------------------------------------------------------------
include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")
