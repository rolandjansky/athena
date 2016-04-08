import AthenaCommon.AtlasUnixGeneratorJob

theApp.EvtMax = 10

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


include("ForwardTransportSvc/preInclude.ForwardTransportFlags_3.5TeV_0000.55m_nominal_v01.py")
include("ForwardTransportSvc/ForwardTransportSvcConfig.ZDC.py")
include("ForwardTransportSvc/postInclude.ForwardTransportSvcConfig.FillRootTree.py")

forwardTransportSvc.McCollection = "GEN_EVENT"

include("ForwardTransportFast/ForwardTransportFast.py")

topSeq.ForwardTransportFast.ForwardTransportSvc = forwardTransportSvc

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile = "hits.ZDC.pool.root"
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]
