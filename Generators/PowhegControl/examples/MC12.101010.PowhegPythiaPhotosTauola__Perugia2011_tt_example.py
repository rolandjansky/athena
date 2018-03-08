# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg_tt setup with sensible defaults
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_tt_Common.py')
PowhegConfig.generateRunCard()
PowhegConfig.generateEvents()

#--------------------------------------------------------------
# Pythia6 (Perugia2011) showering
#--------------------------------------------------------------
include('MC12JobOptions/Pythia_Perugia2011C_Common.py')
include('MC12JobOptions/Pythia_Tauola.py')
include('MC12JobOptions/Pythia_Photos.py')

#--------------------------------------------------------------
# Event filter
#--------------------------------------------------------------
include('MC12JobOptions/TTbarWToLeptonFilter.py')

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Pythia6 ttbar production with Perugia 2011c tune'
evgenConfig.keywords    = [ 'top', 'ttbar', 'leptonic' ]
evgenConfig.contact     = [ 'james.robinson@cern.ch' ]
evgenConfig.generators += [ 'Powheg', 'Pythia' ]
evgenConfig.minevents   = 10
