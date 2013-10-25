#--------------------------------------------------------------
# Powheg_hvq setup with sensible defaults
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_hvq_Common.py')
PowhegConfig.quarkMass        = 172.5
PowhegConfig.tdec_wmass       = 80.399
PowhegConfig.tdec_wwidth      = 2.085
PowhegConfig.tdec_bmass       = 4.95
PowhegConfig.tdec_twidth      = 1.3200
PowhegConfig.tdec_elbranching = 0.108
PowhegConfig.tdec_emass       = 0.00051
PowhegConfig.tdec_mumass      = 0.1057
PowhegConfig.tdec_taumass     = 1.777
PowhegConfig.tdec_dmass       = 0.320
PowhegConfig.tdec_umass       = 0.320
PowhegConfig.tdec_smass       = 0.5
PowhegConfig.tdec_cmass       = 1.55
PowhegConfig.tdec_sin2cabibbo = 0.051
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
