# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg bb setup starting from ATLAS defaults
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_bb_Common.py')
PowhegConfig.generateRunCard()
PowhegConfig.generateEvents()

#--------------------------------------------------------------
# Pythia8 showering with main31 and CTEQ6L1
#--------------------------------------------------------------
include('MC15JobOptions/Pythia8_AU2_CTEQ6L1_Common.py')
include('MC15JobOptions/Pythia8_Powheg.py')

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Pythia8 bb production with AU2 CT10 tune'
evgenConfig.keywords    = [ 'SM', 'QCD', '2jet' ] #'bbbar'
evgenConfig.contact     = [ 'james.robinson@cern.ch' ]
evgenConfig.generators += [ 'Powheg', 'Pythia8' ]
