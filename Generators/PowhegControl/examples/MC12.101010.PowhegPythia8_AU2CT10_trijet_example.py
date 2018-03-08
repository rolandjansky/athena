# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg trijet setup starting from ATLAS defaults
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_trijet_Common.py')
PowhegConfig.generateRunCard()
PowhegConfig.generateEvents()

#--------------------------------------------------------------
# Pythia8 showering with main31 and CTEQ6L1
#--------------------------------------------------------------
include('MC12JobOptions/Pythia8_AU2_CTEQ6L1_Common.py')
include('MC12JobOptions/Pythia8_Powheg_Main31.py')

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Pythia8 trijet production with AU2 CT10 tune'
evgenConfig.keywords    = [ 'QCD', 'trijet', 'jets' ]
evgenConfig.contact     = [ 'james.robinson@cern.ch' ]
evgenConfig.generators += [ 'Powheg', 'Pythia8' ]
