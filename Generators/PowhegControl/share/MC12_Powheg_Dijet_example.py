#--------------------------------------------------------------
# Powheg_Dijet setup with sensible defaults
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_Dijet_Common.py')
PowhegConfig.bornktmin = 30
PowhegConfig.generateRunCard()
PowhegConfig.generateEvents()

#--------------------------------------------------------------
# Pythia8 showering with new, main31-style shower
#--------------------------------------------------------------
include('MC12JobOptions/Pythia8_AU2_CT10_Common.py')
include('MC12JobOptions/Pythia8_Powheg.py')
topAlg.Pythia8.Commands += [ 'SpaceShower:pTmaxMatch = 2'
                           , 'TimeShower:pTmaxMatch = 2'
                           ]
topAlg.Pythia8.UserHook  = 'Main31'

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Pythia8 dijet production with AU2 CT10 tune'
evgenConfig.keywords    = [ 'QCD', 'dijet', 'jets' ]
evgenConfig.contact     = [ 'james.robinson@cern.ch' ]
evgenConfig.generators += [ 'Pythia8' ]
evgenConfig.minevents   = 10
