# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg ZZ setup starting from ATLAS defaults
#--------------------------------------------------------------
include("PowhegControl/PowhegControl_ZZ_Common.py")
PowhegConfig.generate()

#--------------------------------------------------------------
# Fortran Herwig showering with Jimmy MPI
#--------------------------------------------------------------
include("MC12JobOptions/PowhegJimmy_AUET2_CT10_Common.py")
# Beware! Only used here because Fortran Herwig does not have a native model for QED FSR off charged leptons
include("MC12JobOptions/Jimmy_Photos.py")

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Herwig ZZ production with AU2 CT10 tune"
evgenConfig.keywords    = [ "SM", "diboson", "ZZ" ]
evgenConfig.contact     = [ "james.robinson@cern.ch" ]
evgenConfig.generators += [ "Powheg", "Herwig", "Jimmy" ]

