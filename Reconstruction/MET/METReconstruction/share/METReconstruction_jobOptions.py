from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

import METReconstruction.METConfig_RefFinal
import METReconstruction.METConfig_CaloReg
#import METReconstruction.METConfig_PFlow
from RecExConfig.RecFlags import rec
if rec.doTruth:
    import METReconstruction.METConfig_Truth

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import getMETRecoAlg

metAlg = getMETRecoAlg('METReconstruction')

topSequence += metAlg
