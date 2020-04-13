#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# This fragment turns on the FixHepMC algorithm
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence()
printfunc ("Running FixHepMC")
if not hasattr(topAlg, "FixHepMC"):
    # TODO: what if it exists but at the wrong point in the sequence? Move to the end?
    from EvgenProdTools.EvgenProdToolsConf import FixHepMC
    topAlg += FixHepMC()
