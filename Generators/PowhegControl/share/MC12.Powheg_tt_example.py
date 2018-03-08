#--------------------------------------------------------------
# Powheg_tt setup with sensible defaults
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_tt_Common.py')
PowhegConfig.bornsuppfact = -1  # if generation of events weighted in pT is desired
PowhegConfig.bornktmin    = 5.0 # the kt cut-off in GeV - this should be set to a non-zero value when using bornsuppfact
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
evgenConfig.generators += [ 'Pythia6' ]
evgenConfig.minevents   = 10
