from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

import METReconstruction.METConfig_Calo
import METReconstruction.METConfig_Track
if rec.doTruth():
    import METReconstruction.METConfig_Truth

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import getMETRecoAlg

metAlg = getMETRecoAlg('METReconstruction')
topSequence += metAlg

# Set up default configurations
import METReconstruction.METConfig_Associator
from METReconstruction.METAssocConfig import getMETAssocAlg

# Get the configuration directly from METRecoFlags
# Can also provide a dict of configurations or list of RecoTools or both
assocAlg = getMETAssocAlg('METAssociation')
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += assocAlg

from METUtilities.METMakerConfig import getMETMakerAlg
for key,conf in metFlags.METAssocConfigs().iteritems():
    makerAlg = getMETMakerAlg(conf.suffix)
    topSequence += makerAlg
