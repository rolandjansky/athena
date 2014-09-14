from TrigNavTools.TrigNavToolsConfig import HLTTrigNavigationSlimming
slimmerAlg = HLTTrigNavigationSlimming()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += slimmerAlg
