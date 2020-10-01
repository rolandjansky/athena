# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 vector dark matter production with A14 NNPDF2.3 tune."
evgenConfig.keywords = ["BSM", "WIMP", "invisible"]
evgenConfig.contact = ["simone.amoroso@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg DMV process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_DMV_Common.py")

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg.py")

# id:all = name antiName spinType chargeType colType m0 mWidth mMin mMax tau0
genSeq.Pythia8.Commands += ["1000022:all = X Xbar 2 0 0 {0} 0.0 0.0 0.0 0.0".format(PowhegConfig.DM_mass),
                            "1000022:isVisible = false"]
