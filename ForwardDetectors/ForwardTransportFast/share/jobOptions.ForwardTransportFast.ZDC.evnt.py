ServiceMgr.MessageSvc.OutputLevel = DEBUG

from AthenaCommon.DetFlags import DetFlags
DetFlags.ZDC_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput = ['evnt.ZDC.pool.root']
athenaCommonFlags.PoolHitsOutput =  "hits.ZDC.pool.root"
athenaCommonFlags.EvtMax = 10

from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.EventFilter.set_Off()
simFlags.MagneticField.set_Off()
simFlags.ForwardDetectors.set_On()

include("G4AtlasApps/G4Atlas.flat.configuration.py")

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

include("ForwardTransportSvc/preInclude.ForwardTransportFlags_3.5TeV_0000.55m_nominal_v01.py")
include("ForwardTransportSvc/ForwardTransportSvcConfig.ZDC.py")
include("ForwardTransportSvc/postInclude.ForwardTransportSvcConfig.FillRootTree.py")
include("ForwardTransportFast/ForwardTransportFast.py")

topSeq.ForwardTransportFast.ForwardTransportSvc = forwardTransportSvc
