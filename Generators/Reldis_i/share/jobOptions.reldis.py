from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.OutputLevel = DEBUG

theApp.EvtMax = 10

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from Reldis_i.Reldis_iConf import Reldis_i
topSequence += Reldis_i()
topSequence.Reldis_i.ReldisRootFileName = "/afs/cern.ch/atlas/offline/external/reldis/v0.0/test/inter/root/_cwn.root"
topSequence.Reldis_i.ReldisTreeName     = "h2032"

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream("StreamEVGEN")
Stream1.ForceRead = TRUE
Stream1.OutputFile = "evgen.reldis.pool.root"
Stream1.ItemList += ["2101#*", "133273#*"]

