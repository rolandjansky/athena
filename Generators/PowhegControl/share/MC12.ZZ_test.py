#Test job options to see how things work.

include("PowhegControl/MC12_POWHEG_ZZ.py")
PowhegConfig.ncall1 = 10000
PowhegConfig.ncall2 = 10000
PowhegConfig.nubound = 10000
PowhegConfig.generateRunCard()
PowhegConfig.generateEvents()

#--------------------------------------------------------------
# General MC12 configuration
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")
include("MC12JobOptions/PowhegPythia8_AU2_CT10_Common.py")
include("MC12JobOptions/Pythia8_LHEF.py")

#from MC12JobOptions.EvgenConfig import evgenConfig
#evgenConfig.generators += ["lhef", "Pythia8"]
evgenConfig.description = 'POWHEG ZZ + Pythia 8 test jobs'
evgenConfig.keywords = ["SM","ZZ"]
evgenConfig.minevents = 10
