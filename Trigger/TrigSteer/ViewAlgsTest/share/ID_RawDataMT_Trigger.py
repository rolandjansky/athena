#inputfile="MemGrowEvents.data"


from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')
 
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-03-01-00"
 
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()  #Switched off to avoid geometry
DetFlags.ID_setOn()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doCaloSeededBrem = False

#DetFlags.haveRIO.all_off()
#DetFlags.haveRIO.TRT_on()
#DetFlags.haveRIO.pixel_setOff()
#DetFlags.haveRIO.SCT_setOff()
#DetFlags.TRT_setOn()
#include("InDetRecExample/InDetRecConditionsAccess.py")

#DetFlags.Print()
 
theApp.EvtMax=1000

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("CONDBR2-BLKPA-2015-17")
#conddb.addFolder("TRT_ONL","/TRT/Onl/ROD/Compress")
#conddb.addFolder("TRT_OFL","/TRT/Calib/HTCalib")
#conddb.addFolder("TRT_OFL","/TRT/Calib/ToTCalib")
#conddb.addFolder("TRT_OFL","/TRT/Calib/RT")
#conddb.addFolder("TRT_OFL","/TRT/Calib/T0")


 #Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
inputfile="root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data16_13TeV.00309640.physics_EnhancedBias.merge.RAW/data16_13TeV.00309640.physics_EnhancedBias.merge.RAW._lb0628._SFO-1._0001.1"
svcMgr.ByteStreamInputSvc.FullFileName=[inputfile,]
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput=[inputfile,]

from TrigConfigSvc.TrigConfigSvcConf import TrigConf__LVL1ConfigSvc
l1svc = TrigConf__LVL1ConfigSvc("LVL1ConfigSvc")
l1svc.XMLMenuFile = "LVL1config_Physics_pp_v7.xml"
svcMgr += l1svc

if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc 
    svcMgr += ByteStreamAddressProviderSvc()

svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
    "ROIB::RoIBResult/RoIBResult" ]

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if nThreads >= 1:
  #Retrieve input data
  from SGComps.SGCompsConf import SGInputLoader
  topSequence += SGInputLoader( OutputLevel=INFO, ShowEventDump=False )
  topSequence.SGInputLoader.Load = [ ('ROIB::RoIBResult','RoIBResult') ]

from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
ServiceMgr += ROBDataProviderSvc()

#Run calo decoder
from L1Decoder.L1DecoderConf import L1CaloDecoder
caloDecoder = L1CaloDecoder() # by default it is steered towards the RoIBResult of the name above
caloDecoder.OutputLevel=VERBOSE
topSequence += caloDecoder

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.InDet25nsec = True 
InDetFlags.doPrimaryVertex3DFinding = False 
InDetFlags.doPrintConfigurables = False
InDetFlags.doResolveBackTracks = True 
InDetFlags.doSiSPSeededTrackFinder = True
InDetFlags.doTRTPhaseCalculation = True
InDetFlags.doTRTSeededTrackFinder = True
InDetFlags.doTruth = False
InDetFlags.init()
#Determine whether we're running in threaded mode (threads= >=1)
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()

if nThreads >= 1:
  ## get a handle on the ForwardScheduler
  from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
  svcMgr += ForwardSchedulerSvc()
  svcMgr.ForwardSchedulerSvc.CheckDependencies = True

import MagFieldServices.SetupField

# PixelLorentzAngleSvc and SCTLorentzAngleSvc
include("InDetRecExample/InDetRecConditionsAccess.py")

from InDetRecExample.InDetKeys import InDetKeys

include ("InDetRecExample/InDetRecCabling.py")

#Pixel

from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRodDecoder
InDetPixelRodDecoder = PixelRodDecoder(name = "InDetPixelRodDecoder")
ToolSvc += InDetPixelRodDecoder

from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProviderTool
InDetPixelRawDataProviderTool = PixelRawDataProviderTool(name    = "InDetPixelRawDataProviderTool",
                                                         Decoder = InDetPixelRodDecoder)
ToolSvc += InDetPixelRawDataProviderTool
if (InDetFlags.doPrintConfigurables()):
  print      InDetPixelRawDataProviderTool

# load the PixelRawDataProvider
from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProvider
InDetPixelRawDataProvider = PixelRawDataProvider(name         = "InDetPixelRawDataProvider",
                                                 RDOKey       = InDetKeys.PixelRDOs(),
                                                 ProviderTool = InDetPixelRawDataProviderTool)
topSequence += InDetPixelRawDataProvider
topSequence.InDetPixelRawDataProvider.isRoI_Seeded = True
topSequence.InDetPixelRawDataProvider.RoIs = "OutputRoIs"
if (InDetFlags.doPrintConfigurables()):
  print          InDetPixelRawDataProvider


#SCT
from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCT_RodDecoder
InDetSCTRodDecoder = SCT_RodDecoder(name        = "InDetSCTRodDecoder",
                                    TriggerMode = False)
ToolSvc += InDetSCTRodDecoder

from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProviderTool
InDetSCTRawDataProviderTool = SCTRawDataProviderTool(name    = "InDetSCTRawDataProviderTool",
                                                    Decoder = InDetSCTRodDecoder)
ToolSvc += InDetSCTRawDataProviderTool
if (InDetFlags.doPrintConfigurables()):
  print      InDetSCTRawDataProviderTool

# load the SCTRawDataProvider
from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProvider
InDetSCTRawDataProvider = SCTRawDataProvider(name         = "InDetSCTRawDataProvider",
                                            RDOKey       = InDetKeys.SCT_RDOs(),
                                            ProviderTool = InDetSCTRawDataProviderTool)

topSequence += InDetSCTRawDataProvider
topSequence.InDetSCTRawDataProvider.isRoI_Seeded = True
topSequence.InDetSCTRawDataProvider.RoIs = "OutputRoIs"

#TRT
from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
InDetTRTCalDbSvc = TRT_CalDbSvc()
ServiceMgr += InDetTRTCalDbSvc

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummarySvc
InDetTRTStrawStatusSummarySvc = TRT_StrawStatusSummarySvc(name = "InDetTRTStrawStatusSummarySvc")
ServiceMgr += InDetTRTStrawStatusSummarySvc

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()


from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRT_RodDecoder
InDetTRTRodDecoder = TRT_RodDecoder(name = "InDetTRTRodDecoder",
                                    LoadCompressTableDB = True)#(globalflags.DataSource() != 'geant4'))  
ToolSvc += InDetTRTRodDecoder
  
from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProviderTool
InDetTRTRawDataProviderTool = TRTRawDataProviderTool(name    = "InDetTRTRawDataProviderTool",
                                                      Decoder = InDetTRTRodDecoder)
ToolSvc += InDetTRTRawDataProviderTool

  
# load the TRTRawDataProvider
from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProvider
InDetTRTRawDataProvider = TRTRawDataProvider(name         = "InDetTRTRawDataProvider",
                                             RDOKey       = "TRT_RDOs",
                                              ProviderTool = InDetTRTRawDataProviderTool)
topSequence += InDetTRTRawDataProvider
topSequence.InDetTRTRawDataProvider.isRoI_Seeded = True
topSequence.InDetTRTRawDataProvider.RoIs = "OutputRoIs"
  


include ("InDetRecExample/ConfiguredInDetPreProcessingTRT.py")
InDetPreProcessingTRT = ConfiguredInDetPreProcessingTRT(True,False)


include("InDetBeamSpotService/BeamCondSvc.py")

from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool",
    PixelCalibSvc        = None,
    PixelOfflineCalibSvc = None,
    UsePixelCalibCondDB  = False)

ToolSvc += InDetClusterMakerTool


from SiClusterizationTool.SiClusterizationToolConf import InDet__MergedPixelsTool
InDetMergedPixelsTool = InDet__MergedPixelsTool(name                    = "InDetMergedPixelsTool", 
                                                globalPosAlg            = InDetClusterMakerTool,
                                                MinimalSplitSize        = 0,
                                                MaximalSplitSize        = 49,
                                                MinimalSplitProbability = 0,
                                                DoIBLSplitting = True,
                                                SplitClusterAmbiguityMap= InDetKeys.SplitClusterAmbiguityMap())
ToolSvc += InDetMergedPixelsTool

from SiClusterizationTool.SiClusterizationToolConf import InDet__PixelGangedAmbiguitiesFinder
InDetPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder(name = "InDetPixelGangedAmbiguitiesFinder")
ToolSvc += InDetPixelGangedAmbiguitiesFinder

from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization
InDetPixelClusterization = InDet__PixelClusterization(name                    = "InDetPixelClusterization",
                                                      clusteringTool          = InDetMergedPixelsTool,
                                                      gangedAmbiguitiesFinder = InDetPixelGangedAmbiguitiesFinder,
                                                      DetectorManagerName     = InDetKeys.PixelManager(), 
                                                      DataObjectName          = InDetKeys.PixelRDOs(),
                                                      ClustersName            = InDetKeys.PixelClusters())
topSequence += InDetPixelClusterization
topSequence.InDetPixelClusterization.isRoI_Seeded = True
topSequence.InDetPixelClusterization.RoIs = "OutputRoIs"

#
# --- SCT_ClusteringTool (public)
#
from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name              = "InDetSCT_ClusteringTool",
                                                    globalPosAlg      = InDetClusterMakerTool,
                                                    conditionsService = InDetSCT_ConditionsSummarySvc)
#
# --- SCT_Clusterization algorithm
#
from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
InDetSCT_Clusterization = InDet__SCT_Clusterization(name                    = "InDetSCT_Clusterization",
                                                    clusteringTool          = InDetSCT_ClusteringTool,
                                                    # ChannelStatus         = InDetSCT_ChannelStatusAlg,
                                                    DetectorManagerName     = InDetKeys.SCT_Manager(), 
                                                    DataObjectName          = InDetKeys.SCT_RDOs(),
                                                    ClustersName            = InDetKeys.SCT_Clusters(),
                                                    conditionsService       = InDetSCT_ConditionsSummarySvc,
                                                    FlaggedConditionService = InDetSCT_FlaggedConditionSvc)
topSequence += InDetSCT_Clusterization
topSequence.InDetSCT_Clusterization.isRoI_Seeded = True
topSequence.InDetSCT_Clusterization.RoIs = "OutputRoIs"

from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")
ToolSvc += InDetSiSpacePointMakerTool

from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder",
                                                                  SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                  PixelsClustersName     = InDetKeys.PixelClusters(),
                                                                  SCT_ClustersName       = InDetKeys.SCT_Clusters(),
                                                                  SpacePointsPixelName   = InDetKeys.PixelSpacePoints(),
                                                                  SpacePointsSCTName     = InDetKeys.SCT_SpacePoints(),
                                                                  SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                  ProcessPixels          = DetFlags.haveRIO.pixel_on(),
                                                                  ProcessSCTs            = DetFlags.haveRIO.SCT_on(),
                                                                  ProcessOverlaps        = DetFlags.haveRIO.SCT_on())
topSequence += InDetSiTrackerSpacePointFinder

from TrigFastTrackFinder.TrigFastTrackFinderMT_Config import TrigFastTrackFinderMT_eGamma
theFTFMT = TrigFastTrackFinderMT_eGamma()
theFTFMT.outputLevel=VERBOSE

topSequence += theFTFMT


