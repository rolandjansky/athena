ServiceMgr.MessageSvc.OutputLevel = DEBUG

from AthenaCommon.DetFlags import DetFlags
DetFlags.bpipe_setOn()
DetFlags.ALFA_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.PoolHitsOutput =  "hits.ALFA.pool.root"
athenaCommonFlags.EvtMax = 10

from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.EventFilter.set_Off()
simFlags.MagneticField.set_Off()
simFlags.ForwardDetectors.set_On()
simFlags.ForwardDetectors = 2

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from Pythia8_i.Pythia8_iConf import Pythia8_i
topSeq += Pythia8_i()
include("ForwardTransport/Pythia8Config.ALFA.py")

include("G4AtlasApps/G4Atlas.flat.configuration.py")

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

include("ForwardTransportSvc/preInclude.ForwardTransportFlags_4.0TeV_0090.00m_nominal_v01.py")
include("ForwardTransportSvc/ForwardTransportSvcConfig.ALFA.py")
include("ForwardTransportSvc/postInclude.ForwardTransportSvcConfig.FillRootTree.py")
