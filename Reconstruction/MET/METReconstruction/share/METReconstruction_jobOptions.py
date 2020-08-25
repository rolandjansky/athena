import six

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from METReconstruction.METRecoFlags import metFlags
from AthenaCommon.BeamFlags import jobproperties
from RecExConfig.RecFlags import rec
if jobproperties.Beam.beamType == 'cosmics' or jobproperties.Beam.beamType == 'singlebeam' or not rec.doInDet():
    metFlags.UseTracks.set_Value(False)
    metFlags.DoPFlow.set_Value(False)
    printfunc ("METReconstruction_jobOptions: detected cosmics/single-beam configuration -- switch off track-based MET reco")

import METReconstruction.METConfig_Calo
import METReconstruction.METConfig_Track
if rec.doTruth():
    import METReconstruction.METConfig_Truth

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

# Make sure we add these in a well-defined order.
from METUtilities.METMakerConfig import getMETMakerAlg
metConfigs = list(metFlags.METAssocConfigs().keys())
metConfigs.sort()

for key in metConfigs:
    conf = metFlags.METAssocConfigs()[key]
    if not conf.doTruth:
        makerAlg = getMETMakerAlg(conf.suffix)
        topSequence += makerAlg

svcMgr.StoreGateSvc.Dump=True