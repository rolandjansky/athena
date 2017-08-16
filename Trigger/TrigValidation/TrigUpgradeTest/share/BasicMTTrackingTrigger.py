#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

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
 
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.CheckDependencies( True )
AlgScheduler.OutputLevel( VERBOSE )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.setDataLoaderAlg( 'SGInputLoader' )


## get a handle on the ServiceManager
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from SGComps.SGCompsConf import SGInputLoader
topSequence += SGInputLoader( )
topSequence.SGInputLoader.FailIfNoProxy=False

from TrigConfigSvc.TrigConfigSvcConf import TrigConf__LVL1ConfigSvc
from TrigConfigSvc.TrigConfigSvcConfig import findFileInXMLPATH
from TriggerJobOpts.TriggerFlags import TriggerFlags

l1svc = TrigConf__LVL1ConfigSvc("LVL1ConfigSvc")
l1svc.XMLMenuFile = findFileInXMLPATH(TriggerFlags.inputLVL1configFile())
svcMgr += l1svc


from AthenaCommon.Logging import logging 
log = logging.getLogger("BasicMTTrackingTrigger.py")

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
jobproperties.AthenaCommonFlags.FilesInput=["root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r7572_tid07644622_00/RDO.07644622._000001.pool.root.1"]


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

# once L1 re-simulation works this has to change from emulation to real unpacking
include("TrigUpgradeTest/L1CF.py")

data = {}

data['l1emroi'] = ['1.3,2.9,2704088841,EM3,EM7,EM10,EM20;',
                   '1.2,3.1,2972524297,EM3,EM7,EM10,EM20;']

data['l1muroi'] = [';',
                   ';'] # required by L1CF

data['ctp'] = ['HLT_e20',
               'HLT_e20']
from TrigUpgradeTest.TestUtils import writeEmulationFiles
writeEmulationFiles(data)


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
                                                      ClustersName            = "PixelTrigClusters",
                                                      isRoI_Seeded = True,
                                                      RoIs = "L1EMRoIs",
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
                                                    ClustersName            = "SCT_TrigClusters",
                                                    conditionsService       = InDetSCT_ConditionsSummarySvc,
                                                    FlaggedConditionService = InDetSCT_FlaggedConditionSvc,
                                                    isRoI_Seeded = True,
                                                    RoIs = "L1EMRoIs",
                                                    )
topSequence += InDetSCT_Clusterization

from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")
ToolSvc += InDetSiSpacePointMakerTool

from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder",
                                                                  SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                  PixelsClustersName     = "PixelTrigClusters",
                                                                  SCT_ClustersName       = "SCT_TrigClusters",
                                                                  SpacePointsPixelName   = "PixelTrigSpacePoints",
                                                                  SpacePointsSCTName     = "SCT_TrigSpacePoints",
                                                                  SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                  ProcessPixels          = DetFlags.haveRIO.pixel_on(),
                                                                  ProcessSCTs            = DetFlags.haveRIO.SCT_on(),
                                                                  ProcessOverlaps        = False)
topSequence += InDetSiTrackerSpacePointFinder


from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_eGamma
theFTF = TrigFastTrackFinder_eGamma()
theFTF.RoIs="L1EMRoIs"

topSequence += theFTF
log.info(theFTF)

from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
xAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(name = "InDetxAODParticleCreatorAlg"+InputTrackCollection,
                                                         ConvertTracks = True,
                                                         ConvertTrackParticles = False,
                                                         TrackContainerName = "TrigFastTrackFinder_Tracks",
                                                         xAODContainerName = "xAODTracks",
                                                         xAODTrackParticlesFromTracksContainerName = "xAODTracks2",
                                                         TrackParticleCreator = InDetxAODParticleCreatorTool,
                                                         PrintIDSummaryInfo = True)

topSequence += xAODTrackParticleCnvAlg

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
