#--------------------------------------------------------------
# CTP simulation algorithm
#--------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TrigT1CTP.TrigT1CTPConfig import CTPSimulation
topSequence += CTPSimulation("CTPSimulation")

topSequence.CTPSimulation.UseLegacyInterface = False 
topSequence.CTPSimulation.UseDeadTime        = False
#topSequence.CTPSimulation.OutputLevel        = DEBUG
