# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg Wt_DR setup starting from ATLAS defaults
#--------------------------------------------------------------
include("PowhegControl/PowhegControl_Wt_DR_Common.py")
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 W+t production with A14 NNPDF2.3 tune"
evgenConfig.keywords    = [ "SM", "W", "top" ]
evgenConfig.contact     = [ "james.robinson@cern.ch" ]
