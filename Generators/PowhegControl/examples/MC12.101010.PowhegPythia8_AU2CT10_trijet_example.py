# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg_trijet setup with sensible defaults
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_trijet_Common.py')
PowhegConfig.generateRunCard()
PowhegConfig.generateEvents()

#--------------------------------------------------------------
# Pythia8 showering with new, main31-style shower
#--------------------------------------------------------------
include('MC12JobOptions/Pythia8_AU2_CT10_Common.py')
topAlg.Pythia8.Commands += [ 'SpaceShower:pTmaxMatch = 2'
                           , 'TimeShower:pTmaxMatch = 2'
                           ]
topAlg.Pythia8.UserHook  = 'Main31'

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Pythia8 trijet production with AU2 CT10 tune'
evgenConfig.keywords    = [ 'QCD', 'trijet', 'jets' ]
evgenConfig.contact     = [ 'james.robinson@cern.ch' ]
evgenConfig.generators += [ 'Powheg', 'Pythia8' ]
evgenConfig.minevents   = 10
