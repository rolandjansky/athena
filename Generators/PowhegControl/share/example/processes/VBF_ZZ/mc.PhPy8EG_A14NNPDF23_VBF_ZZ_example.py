# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 VBF ZZ production with A14 NNPDF2.3 tune."
evgenConfig.keywords = ["SM", "diboson", "ZZ", "VBF"]
evgenConfig.contact = ["tpelzer@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg VBF_Z_Z process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_VBF_ZZ_Common.py")

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
# possible decay modes for each z: e+ e-, mu+ mu-, ve ve~, vm vm~, d d~, s s~, u u~, c- c~, (d d~ / s s~), (u u~ / c c~)
# --------------------------------------------------------------
PowhegConfig.decay_mode = "z z > e+ e- mu+ mu-"

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg.py")
