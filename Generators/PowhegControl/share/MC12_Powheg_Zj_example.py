#--------------------------------------------------------------
# Powheg_Zj setup with sensible defaults
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_Zj_Common.py')
PowhegConfig.generateRunCard()
PowhegConfig.generateEvents()

#--------------------------------------------------------------
# Pythia8 showering with new, main31-style shower
#--------------------------------------------------------------
include('MC12JobOptions/Pythia8_AU2_CT10_Common.py')
include('MC12JobOptions/Pythia8_LHEF.py')

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Pythia8 Zj production with AU2 CT10 tune'
evgenConfig.keywords    = [ 'SM', 'zj' ]
evgenConfig.contact     = [ 'stephen.paul.bieniek@cern.ch' ]
evgenConfig.generators += [ 'Pythia8' ]

