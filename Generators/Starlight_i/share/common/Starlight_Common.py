from Starlight_i.Starlight_iConf import Starlight_i
genSeq += Starlight_i("Starlight")
genSeq.Starlight.McEventKey = "GEN_EVENT"

evgenConfig.generators += ["Starlight"]

## Extra stream persistency
## 2101 == EventInfo, 133273 == MCTruth (HepMC)
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
_evgenstream = AthenaPoolOutputStream("StreamEVGEN")
_evgenstream.ItemList = ["2101#*","133273#GEN_EVENT"]
del _evgenstream

