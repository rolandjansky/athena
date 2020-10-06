# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
# Reweighting to get PDF and QCD scale weight variations
# is not yet supported for this process (it fails for
# currently unknown reasons), so it is disabled here by
# setting single values for the PDF and QCD scale factors
# --------------------------------------------------------------
PowhegConfig.PDF = 260000
PowhegConfig.mu_F = 1.0
PowhegConfig.mu_R = 1.0

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg.py")
