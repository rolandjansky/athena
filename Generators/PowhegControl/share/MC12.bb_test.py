#Test job options to see how things work.

include("PowhegControl/MC12_POWHEG_hvq.py")
PowhegConfig.ncall1 = 200
PowhegConfig.ncall2 = 200
PowhegConfig.nubound = 500
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
evgenConfig.description = 'POWHEG bb + Pythia 8 test jobs'
evgenConfig.keywords = ["SM","bb"]
evgenConfig.minevents = 10
