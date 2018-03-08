# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg tt setup starting from ATLAS defaults
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_tt_Common.py')
PowhegConfig.PDF = range(10800,10853)
PowhegConfig.mu_F = [ 1.0, 0.5, 0.5, 1.0, 1.0, 2.0, 2.0 ]
PowhegConfig.mu_R = [ 1.0, 0.5, 1.0, 0.5, 2.0, 1.0, 2.0 ]
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia6 showering with the Perugia2011 tune
#--------------------------------------------------------------
include('MC15JobOptions/PowhegPythia_Perugia2011C_Common.py')
include('MC15JobOptions/Pythia_Tauola.py')
include('MC15JobOptions/Pythia_Photos.py')

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Pythia6 ttbar production with Perugia 2011c tune. Additional weights for 52 CT10 PDF uncertainties and 7 scale variations.'
evgenConfig.keywords    = [ 'SM', 'top' ]
evgenConfig.contact     = [ 'james.robinson@cern.ch' ]
