# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg Wj setup starting from ATLAS defaults
#--------------------------------------------------------------
include("PowhegControl/PowhegControl_Wj_Common.py")
PowhegConfig.NNLO_reweighting_inputs = { "DYNNLO":"Wp_CM8_MMHT14NNLO_11.top" }
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 W+j production with A14 NNPDF2.3 tune"
evgenConfig.keywords    = [ "SM", "W", "1jet" ]
evgenConfig.contact     = [ "james.robinson@cern.ch" ]
