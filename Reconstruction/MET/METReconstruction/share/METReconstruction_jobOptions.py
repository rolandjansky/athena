from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

############################################################################
# Set up muon and egamma topocluster links
muonTCLinkAlg = CfgMgr.ClusterMatching__CaloClusterMatchLinkAlg("MuonTCLinks",
                                               ClustersToDecorate="MuonClusterCollection")
topSequence += muonTCLinkAlg

############################################################################
# SHOULD BE MIGRATED TO MUON ALGORITHMS

from METReconstruction.METRecoFlags import metFlags
from AthenaCommon.BeamFlags import jobproperties
from RecExConfig.RecFlags import rec
if jobproperties.Beam.beamType == 'cosmics' or jobproperties.Beam.beamType == 'singlebeam' or not rec.doInDet():
    metFlags.UseTracks.set_Value(False)
    metFlags.DoPFlow.set_Value(False)
    print "METReconstruction_jobOptions: detected cosmics/single-beam configuration -- switch off track-based MET reco"

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

from METUtilities.METMakerConfig import getMETMakerAlg
for key,conf in metFlags.METAssocConfigs().iteritems():
    if not conf.doTruth:
        makerAlg = getMETMakerAlg(conf.suffix)
        topSequence += makerAlg
