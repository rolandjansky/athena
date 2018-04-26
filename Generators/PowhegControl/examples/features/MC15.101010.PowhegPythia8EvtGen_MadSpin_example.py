# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# --------------------------------------------------------------
# EVGEN configuration
# --------------------------------------------------------------
evgenConfig.description = "PowhegControl example of MadSpin"
evgenConfig.keywords = ["SM", "top"]
evgenConfig.contact = ["james.robinson@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg tt process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_tt_Common.py")

# --------------------------------------------------------------
# Set process options and generate
# --------------------------------------------------------------
PowhegConfig.decaymode = "undecayed"
PowhegConfig.MadSpin_decays.append("decay t > w+ b, w+ > l+ vl")
PowhegConfig.MadSpin_decays.append("decay t > w+ b, w+ > j j")
PowhegConfig.MadSpin_process = "\n".join(["generate p p > t t~ [QCD]",
                                          "add process b g > t t~ b [QCD]",
                                          "add process b~ g > t t~ b~ [QCD]",
                                          "add process p p > t t~ j[QCD]",
                                          "add process b b~ > t t~ j[QCD]",
                                          "add process b b~ > t t~[QCD]"])
PowhegConfig.generate()

# --------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
# --------------------------------------------------------------
include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")
