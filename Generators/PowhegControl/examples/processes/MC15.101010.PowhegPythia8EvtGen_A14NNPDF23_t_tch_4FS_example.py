# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 t-channel single top production (4-flavour scheme) with A14 NNPDF2.3 tune."
evgenConfig.keywords = ["SM", "QCD", "2jet", "bottom"]
evgenConfig.contact = ["james.robinson@cern.ch", "disipio@cern.ch"]
evgenConfig.generators  = [ "Powheg", "Pythia8", "EvtGen" ]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg t_tch_4FS process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_t_tch_4FS_Common.py")

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")
