ServiceMgr.MessageSvc.OutputLevel = DEBUG

from AthenaCommon.DetFlags import DetFlags
DetFlags.bpipe_setOn()
DetFlags.ZDC_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.PoolHitsOutput = "hits.ZDC.pool.root"
athenaCommonFlags.EvtMax = 10

from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.EventFilter.set_Off()
SimFlags.MagneticField.set_Off()
SimFlags.ForwardDetectors.set_On()
SimFlags.ForwardDetectors = 2

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topSeq += ParticleGenerator()
        
topSeq.ParticleGenerator.orders = [
    'pdgcode: constant 2112',
    'vertX:   constant 0',
    'vertY:   constant 0',
    'vertZ:   constant 0',
    't:       constant 0',
    'eta:     flat     8 15',
    'phi:     flat     0 6.28318',
    'e:       constant 3500000']

from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

include("ForwardTransportSvc/preInclude.ForwardTransportFlags_3.5TeV_0000.55m_nominal_v01.py")
include("ForwardTransportSvc/ForwardTransportSvcConfig.ZDC.py") 
include("ForwardTransportSvc/postInclude.ForwardTransportSvcConfig.FillRootTree.py")
