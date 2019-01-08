# This fragment turns on the FixHepMC algorithm
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence()
print "Running FixHepMC"
if not hasattr(topAlg, "FixHepMC"):
    # TODO: what if it exists but at the wrong point in the sequence? Move to the end?
    from EvgenProdTools.EvgenProdToolsConf import FixHepMC
    topAlg += FixHepMC()
