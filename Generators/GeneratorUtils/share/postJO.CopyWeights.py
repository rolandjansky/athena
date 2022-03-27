## Run the CopyEventWeight algorithm to copy HepMC weight(s) to EventInfo/EventType
if not hasattr(topAlg, "EventInfoCnvAlg"):
    from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
    topAlg += xAODMaker__EventInfoCnvAlg(xAODKey="EventInfo")
from EvgenProdTools.EvgenProdToolsConf import CopyEventWeight
topAlg += CopyEventWeight(mcEventWeightsKey="EventInfo.mcEventWeights")
