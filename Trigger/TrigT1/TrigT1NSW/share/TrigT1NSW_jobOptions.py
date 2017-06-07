#--------------------------------------------------------------
# NSW L1 simulation algorithm
#--------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TrigT1NSW.TrigT1NSWConfig import NSWL1Simulation
topSequence += NSWL1Simulation("NSWL1Simulation")
