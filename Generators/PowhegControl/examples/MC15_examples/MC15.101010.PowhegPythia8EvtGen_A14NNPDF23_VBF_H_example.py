# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg VBF_H setup starting from ATLAS defaults
#--------------------------------------------------------------
include("PowhegControl/PowhegControl_VBF_H_Common.py")
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 VBF_H production with A14 NNPDF2.3 tune"
evgenConfig.keywords    = [ "Higgs", "VBF" ]
evgenConfig.contact     = [ "james.robinson@cern.ch" ]
