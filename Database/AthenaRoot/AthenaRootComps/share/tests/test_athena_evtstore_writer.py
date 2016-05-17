import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle on the ApplicationManager
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 20

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.EventSelector.RunNumber = 1

from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__WriteData
topSequence += AthPoolEx__WriteData( "WriteData" )

#from AthenaRootComps.WriteAthenaRoot import AthenaRootOutputStream
from AthenaRootComps.OutputStreamAthenaRoot import createOutputStream 
Stream1 = createOutputStream( "Stream1" , "SimpleRootFile.root" , True )
Stream1.ItemList += [ "ExampleHitContainer#MyHits" ]

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream2 = AthenaPoolOutputStream( "Stream2" , "SimplePoolFile.root" , True )
Stream2.ItemList += [ "ExampleHitContainer#MyHits" ]

svcMgr.MessageSvc.OutputLevel = 0
