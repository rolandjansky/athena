# Job options fragment to enable the copying of event weights during simulation

# Algorithm sequence - get to ensure that we don't rely on this being named
#  topSeq or topAlg or whatever else in the user's job options
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

if not hasattr(topSeq, "EventInfoCnvAlg"):
    from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
    topSeq += xAODMaker__EventInfoCnvAlg(xAODKey="EventInfo")
from EvgenProdTools.EvgenProdToolsConf import CopyEventWeight
topSeq += CopyEventWeight(mcEventWeightsKey="EventInfo.mcEventWeights")

