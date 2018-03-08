# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg_Z setup with sensible defaults
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_Z_Common.py')
PowhegConfig.generateRunCard()
PowhegConfig.generateEvents()

#--------------------------------------------------------------
# Pythia8 showering with new, main31-style shower
#--------------------------------------------------------------
include('MC12JobOptions/Pythia8_AU2_CT10_Common.py')

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Pythia8 Z production with AU2 CT10 tune'
evgenConfig.keywords    = [ 'SM', 'Z' ]
evgenConfig.contact     = [ 'stephen.paul.bieniek@cern.ch' ]
evgenConfig.generators += [ 'Powheg', 'Pythia8' ]

