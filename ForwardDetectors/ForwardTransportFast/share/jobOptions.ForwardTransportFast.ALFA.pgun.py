ServiceMgr.MessageSvc.OutputLevel = DEBUG

from AthenaCommon.DetFlags import DetFlags
DetFlags.ALFA_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.PoolHitsOutput = "hits.ALFA.pool.root"
athenaCommonFlags.EvtMax = 10

from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.EventFilter.set_Off()
SimFlags.MagneticField.set_Off()
SimFlags.ForwardDetectors.set_On()

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topSeq += ParticleGenerator()
	
topSeq.ParticleGenerator.orders = [
    'pdgcode: constant 2212',
    'vertX:   constant 0',
    'vertY:   constant 0',
    'vertZ:   constant 0',
    't:       constant 0',
    'eta:     flat     8 15',
    'phi:     flat     0 6.28318',
    'e:       constant 4000000']

from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

include("ForwardTransportSvc/preInclude.ForwardTransportFlags_4.0TeV_0090.00m_nominal_v01.py")
include("ForwardTransportSvc/ForwardTransportSvcConfig.ALFA.py")
include("ForwardTransportSvc/postInclude.ForwardTransportSvcConfig.FillRootTree.py")
include("ForwardTransportFast/ForwardTransportFast.py")

topSeq.ForwardTransportFast.ForwardTransportSvc = forwardTransportSvc

