# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# This is an example joboption to generate events with Powheg
# using ATLAS' interface. Users should optimise and carefully
# validate the settings before making an official sample request.
#--------------------------------------------------------------

#--------------------------------------------------------------
# Powheg ZZ setup starting from ATLAS defaults
#--------------------------------------------------------------
include("PowhegControl/PowhegControl_ZZ_Common.py")
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune, main31 routine
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg_Main31.py")

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 ZZ production with A14 NNPDF2.3 tune"
evgenConfig.keywords    = [ "SM", "diboson", "ZZ" ]
evgenConfig.contact     = [ "james.robinson@cern.ch" ]
