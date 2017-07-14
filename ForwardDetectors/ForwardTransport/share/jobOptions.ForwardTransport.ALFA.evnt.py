ServiceMgr.MessageSvc.OutputLevel = DEBUG

from AthenaCommon.DetFlags import DetFlags
DetFlags.bpipe_setOn()
DetFlags.ALFA_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput = ['evnt.ALFA.pool.root']
athenaCommonFlags.PoolHitsOutput =  "hits.ALFA.pool.root"
athenaCommonFlags.EvtMax = 10

from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.EventFilter.set_Off()
SimFlags.MagneticField.set_Off()
SimFlags.ForwardDetectors.set_On()
SimFlags.ForwardDetectors = 2

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

include("ForwardTransportSvc/preInclude.ForwardTransportFlags_4.0TeV_0090.00m_nominal_v01.py")
include("ForwardTransportSvc/ForwardTransportSvcConfig.ALFA.py")
include("ForwardTransportSvc/postInclude.ForwardTransportSvcConfig.FillRootTree.py")
