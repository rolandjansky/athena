ServiceMgr.MessageSvc.OutputLevel = DEBUG

from AthenaCommon.DetFlags import DetFlags
DetFlags.ZDC_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput = ['evnt.ZDC.pool.root']
athenaCommonFlags.PoolHitsOutput =  "hits.ZDC.pool.root"
athenaCommonFlags.EvtMax = 10

from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.EventFilter.set_Off()
SimFlags.MagneticField.set_Off()
SimFlags.ForwardDetectors.set_On()

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

include("ForwardTransportSvc/preInclude.ForwardTransportFlags_3.5TeV_0000.55m_nominal_v01.py")
include("ForwardTransportSvc/ForwardTransportSvcConfig.ZDC.py")
include("ForwardTransportSvc/postInclude.ForwardTransportSvcConfig.FillRootTree.py")
include("ForwardTransportFast/ForwardTransportFast.py")

topSeq.ForwardTransportFast.ForwardTransportSvc = forwardTransportSvc
