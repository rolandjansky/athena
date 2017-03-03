# an example of minimal jO based on RecExCommon configuration running the FastTrackFinder 
# to find tracks in predefined RoIs
# 20/2/2017 Jiri.Masik@manchester.ac.uk
#
# athena.py -l DEBUG --threads=1 BasicMTTrackingTrigger.py 2>&1 | tee log
# in the environment of
# asetup 20.8.X-VAL,r2017-02-06,here
# /afs/cern.ch/work/m/masik/public/id-data-ftf
# The directory contains 
# LVL1config_Physics_pp_v5.xml   <-the version recommended on the twiki
# caloRoIData.dat     <-different version - the twiki recipe using sed
# does not replace ; by a newline
# source/Trigger/TrigAlgorithms/TrigFastTrackFinder
# with the mods to the configuration of the TrigOnlineSpacePointTool
 
from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# svcMgr += ForwardSchedulerSvc()
svcMgr.ForwardSchedulerSvc.CheckDependencies = True
svcMgr.ForwardSchedulerSvc.OutputLevel=VERBOSE

## get a handle on the ServiceManager
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from SGComps.SGCompsConf import SGInputLoader
#topSequence += SGInputLoader( OutputLevel=INFO, ShowEventDump=False )
topSequence += SGInputLoader( )
topSequence.SGInputLoader.Load = [ ('PixelRDO_Container','PixelRDOs'),
                                   #('TRT_RDO_Container','TRT_RDOs'),
                                   ('SCT_RDO_Container','SCT_RDOs'),
#                                  # ('Trk::SegmentCollection','TRTSegments'),
                                 ]


from TrigConfigSvc.TrigConfigSvcConf import TrigConf__LVL1ConfigSvc
l1svc = TrigConf__LVL1ConfigSvc("LVL1ConfigSvc")
l1svc.XMLMenuFile = "LVL1config_Physics_pp_v5.xml"
svcMgr += l1svc


from AthenaCommon.Logging import logging 
log = logging.getLogger("BasicMTTrackingTrigger.py")

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them

def setOutput(obj, prop, name):
    setattr(obj, prop, name)
    setattr(obj, prop+"Aux", name+"Aux.")

def getOutput(obj, prop):
    try:
        return getattr(obj, prop)
    except:
        return obj.getDefaultProperty(prop)
    raise "Error in reading property " + prop + " from " + obj
 
def genMenuAlg(name, inputHypos, inputChains):
    assert inputHypos != None, 'Alg to take hypo putput from is missing'
    assert inputChains != None, 'Alg to take chain decisions from is missing'
    log.info(inputChains, inputHypos)
    from ViewAlgs.ViewAlgsConf import MenuAlg
    menuAlg = MenuAlg(name)
    menuAlg.HypoDecisions = getOutput(inputHypos, "OutputDecisions")
    menuAlg.InputChainDecisions = getOutput(inputChains,"OutputChainDecisions")
    setOutput(menuAlg, "OutputDecisions", name+"RoIs")
    setOutput(menuAlg, "OutputChainDecisions", name+"Chains")
    return menuAlg



#minimal RecExCommon setup

from RecExConfig.RecFlags import rec
rec.doESD=False
rec.doAOD=False
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doWriteTAG=False

rec.doForwardDet=False
rec.doInDet=True
rec.doMuon=False
rec.doCalo=False
rec.doCaloRinger=False     
rec.doEgamma=False
rec.doMuonCombined=False
rec.doJetMissingETTag=False
rec.doTau=False
rec.doTruth=False

rec.doForwardDet=False
rec.doMuonCombined=False
rec.doJetMissingETTag=False
rec.doTau=False
rec.doTrigger=False

#disable perfmon  (avoids a crash in the finalization)
rec.doPerfMon.set_Value_and_Lock( False )
rec.doDetailedPerfMon.set_Value_and_Lock( False )
rec.doSemiDetailedPerfMon.set_Value_and_Lock( False )
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring.set_Value_and_Lock( False )


from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.Enabled.set_Value_and_Lock(False)

#disable all recAlgs
from RecExConfig.RecAlgsFlags import recAlgs
flags = {}
flags.update(recAlgs.__dict__)
for domethod in flags:
  if domethod.startswith('do'):     # and domethod is not 'doTrigger':
    runlockon = recAlgs.__dict__[domethod]
    runlockon.set_Value_and_Lock(False)


include("RecExCommon/RecExCommon_topOptions.py")

#remove RecExCommon 
topSequence = AlgSequence()
for i in topSequence:
  log.info('removing ', i.getName())
  if i.getName()!="SGInputLoader":
    topSequence.remove(i)


#Run Fake RoI
from L1Decoder.L1DecoderConf import FakeRoI
fakeRoI = FakeRoI("fakeRoI")
fakeRoI.InputFilename="caloRoIData.dat"
setOutput(fakeRoI, "OutputDecisions", "OutputRoIs")
fakeRoI.OutputLevel=DEBUG

topSequence += fakeRoI

theApp.EvtMax = 10
svcMgr.DetectorStore.Dump=True
MessageSvc.debugLimit=1000000000



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
                                                      ClustersName            = InDetKeys.PixelClusters(),
                                                      isRoI_Seeded = True,
                                                      RoIs = "OutputRoIs",
                                                      )
topSequence += InDetPixelClusterization

#
# --- SCT_ClusteringTool (public)
#
from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name              = "InDetSCT_ClusteringTool",
                                                    globalPosAlg      = InDetClusterMakerTool,
                                                    conditionsService = InDetSCT_ConditionsSummarySvc)
ToolSvc += InDetSCT_ClusteringTool
      
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
                                                    FlaggedConditionService = InDetSCT_FlaggedConditionSvc,
                                                    isRoI_Seeded = True,
                                                    RoIs = "OutputRoIs",
                                                    )
topSequence += InDetSCT_Clusterization

from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")
ToolSvc += InDetSiSpacePointMakerTool

from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder",
                                                                  SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                  PixelsClustersName     = InDetKeys.PixelClusters(),
                                                                  SCT_ClustersName       = InDetKeys.SCT_Clusters(),
                                                                  SpacePointsPixelName   = 'PixelTrigSpacePoints',
                                                                  SpacePointsSCTName     = 'SCT_TrigSpacePoints',
                                                                  SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                  ProcessPixels          = DetFlags.haveRIO.pixel_on(),
                                                                  ProcessSCTs            = DetFlags.haveRIO.SCT_on(),
                                                                  ProcessOverlaps        = False)
topSequence += InDetSiTrackerSpacePointFinder


from TrigFastTrackFinder.TrigFastTrackFinderMT_Config import TrigFastTrackFinderMT_eGamma
theFTFMT = TrigFastTrackFinderMT_eGamma()

topSequence += theFTFMT
log.info(theFTFMT)

#probably initialized only in trigger=True?
from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
RegSelSvc = RegSelSvcDefault()
RegSelSvc.enablePixel = True
RegSelSvc.enableSCT   = True

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += RegSelSvc

from AthenaCommon.DetFlags import DetFlags
DetFlags.TRT_setOff()


#to be followed up
conddb.blockFolder('/TRT/Align')
conddb.blockFolder('/Indet/PixelDist')
