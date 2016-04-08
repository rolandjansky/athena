theApp.EvtMax = 500
svcMgr.MessageSvc.OutputLevel = WARNING

import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile = "AODout.pool.root"

svcMgr.EventSelector.InputCollections = []
