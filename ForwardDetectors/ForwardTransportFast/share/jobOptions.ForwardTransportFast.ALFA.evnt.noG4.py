import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = ["evnt.ALFA.pool.root"]

theApp.EvtMax = 10

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

include("ForwardTransportSvc/preInclude.ForwardTransportFlags_4.0TeV_0090.00m_nominal_v01.py")
include("ForwardTransportSvc/ForwardTransportSvcConfig.ALFA.py")
include("ForwardTransportSvc/postInclude.ForwardTransportSvcConfig.FillRootTree.py")

forwardTransportSvc.McCollection = "GEN_EVENT"

include("ForwardTransportFast/ForwardTransportFast.py")

topSeq.ForwardTransportFast.ForwardTransportSvc = forwardTransportSvc

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile = "hits.ALFA.pool.root"
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]
