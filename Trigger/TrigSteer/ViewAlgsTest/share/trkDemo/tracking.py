## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from SGComps.SGCompsConf import SGInputLoader
topSequence += SGInputLoader( OutputLevel=INFO, ShowEventDump=False )
topSequence.SGInputLoader.Load = [ ('PixelRDO_Container','PixelRDOs'),
                                   ('TRT_RDO_Container','TRT_RDOs'),
                                   ('SCT_RDO_Container','SCT_RDOs'),
#                                   ('Trk::SegmentCollection','TRTSegments'),
                                 ]
#Turn off some ID stuff
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTRTPhaseCalculation = False
InDetFlags.doTruth = False
InDetFlags.doCaloSeededBrem = False
InDetFlags.doCaloSeededTRTSegments = False


include("RecExRecoTest/RecExRecoTest_RTT_id.py")

from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
RegSelSvc = RegSelSvcDefault()


RegSelSvc.enablePixel = True
RegSelSvc.enableSCT   = True


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += RegSelSvc


# wire it with input and output
from ViewAlgsTest.Menu import *
from ViewAlgsTest.connectAlgorithmsIO import *

topSequence.InDetPixelClusterization.isRoI_Seeded = True
connectAlgorithmsIO(consumer=(topSequence.InDetPixelClusterization, "RoIs"), producer=(topSequence.fakeRoI, "OutputRoIs"))
