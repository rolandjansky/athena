from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

ServiceMgr.MessageSvc.OutputLevel = DEBUG

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput = ['evgen.pgun.ALFA.pool.root']
athenaCommonFlags.PoolHitsOutput.set_Off()
athenaCommonFlags.EvtMax = -1

from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.EventFilter.set_Off()
SimFlags.MagneticField.set_Off()

from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

include("ALFA_BeamTransport/ALFA_BeamTransportConfig.py")
