# Job options fragment to enable the copying of event weights during simulation

# Algorithm sequence - get to ensure that we don't rely on this being named
#  topSeq or topAlg or whatever else in the user's job options
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

try:
    from GeneratorModules.GeneratorModulesConf import CopyEventWeight
    topSeq += CopyEventWeight(TruthCollKey="GEN_EVENT")
except:
    from EvgenProdTools.EvgenProdToolsConf import CopyEventWeight
    topSeq += CopyEventWeight()

