# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg jet-jet-jet setup starting from ATLAS defaults
#--------------------------------------------------------------
include("PowhegControl/PowhegControl_jjj_Common.py")
# # Add a test of the bornsuppfact mean event weight behaviour
# PowhegConfig.bornktmin = 50.0
# PowhegConfig.bornsuppfact = 500
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 tune and NNPDF 2.3
#--------------------------------------------------------------
include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 jet-jet-jet production with AU2 CT10 tune"
evgenConfig.keywords    = [ "SM", "QCD", "jets", "3jet" ]
evgenConfig.contact     = [ "james.robinson@cern.ch" ]
