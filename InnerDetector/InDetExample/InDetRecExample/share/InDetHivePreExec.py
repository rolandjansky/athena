#Early failure warning
from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
svcMgr += ForwardSchedulerSvc()
svcMgr.ForwardSchedulerSvc.CheckDependencies = True

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from SGComps.SGCompsConf import SGInputLoader
topSequence += SGInputLoader( OutputLevel=INFO, ShowEventDump=False )
topSequence.SGInputLoader.Load = [ ('PixelRDO_Container','PixelRDOs'),
                                   ('TRT_RDO_Container','TRT_RDOs'),
                                   ('SCT_RDO_Container','SCT_RDOs'),
                                   #('CaloClusterROI_Collection','InDetHadCaloClusterROIs'),
                                   #('CaloClusterROI_Collection','InDetCaloClusterROIs'),
                                   #('Trk::SegmentCollection','TRTSegments'),
                                 ]

#
#from InDetRecExample.InDetJobProperties import InDetFlags
#InDetFlags.doTRTPhaseCalculation = False
#InDetFlags.doTruth = False
