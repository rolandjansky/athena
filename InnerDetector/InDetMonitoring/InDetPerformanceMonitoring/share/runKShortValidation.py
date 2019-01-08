#example of personal topOptions
#
# to use it
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
#
# see RecExCommon/share/RecExCommon_flags.py for more available flags
# and https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags
# for more complete documentation.
#
# doESD, DetDescrVersion and other flags
# needs be set before the include, since several
# secondary flags are configured according to that one
#
#   1) Specify the input in here
     # - One file
PoolInput = ["../run/data16_13TeV.00310249.physics_Main.merge.AOD.f755_m1699._lb1328._0003.1"]
# PoolInput = ["../run/data15_13TeV/ESD.08262633._004792.pool.root.1"]


#   2) Feed files when executing the script
# if 'inputFiles' in dir():
#   print inputFiles
# PoolInput = inputFiles

# number of events to process
EvtMax=1000
SkipEvents = 0

# refit for ESD
doRefit=False


NoBeamConstraint=True

# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
from PerfMonComps.PerfMonFlags import jobproperties as pmjp
pmjp.PerfMonFlags.doFastMon=True


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AlgSequence import AthSequencer


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=PoolInput
athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)
athenaCommonFlags.SkipEvents = SkipEvents

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock("CONDBR2-BLKPA-2016-19")
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-R2-2015-03-29-00")

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True

from RecExConfig.RecFlags import rec #set the reconstruction, which reconstruction should run
rec.AutoConfiguration=['everything'] #picks up info from ESD, conditions tag...
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False) # uncomment if rec.do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if rec.do not write ESD
rec.doAOD.set_Value_and_Lock(False) # uncomment if rec.do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if rec.do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if rec.do not write TAG
rec.doCBNT.set_Value_and_Lock(False)
# rec.doPerfMon.set_Value_and_Lock(True)
rec.doPerfMon.set_Value_and_Lock(False)


rec.doInDet.set_Value_and_Lock(True)

rec.doTile.set_Value_and_Lock(False)
rec.doLArg.set_Value_and_Lock(False)
rec.doCalo.set_Value_and_Lock(False)
#rec.doMuon.set_Value_and_Lock(True)
#rec.doMuonCombined.set_Value_and_Lock(True)
#rec.doEgamma.set_Value_and_Lock(True)
#rec.doJetMissingETTag.set_Value_and_Lock(True)
rec.doMuon.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock(False)

#rec.doMonitoring.set_Value_and_Lock(True)
#from AthenaMonitoring.DQMonFlags import DQMonFlags
#DQMonFlags.doInDetPerfMon.set_Value_and_Lock(True)

#with beamconstraint
if NoBeamConstraint:
  from InDetRecExample.InDetJobProperties import InDetFlags
  InDetFlags.useBeamConstraint.set_Value_and_Lock(False)

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

include ("RecExCond/RecExCommon_flags.py") #What raw info should we use - gives matrix in output
# switch off ID, calo, or muons
DetFlags.ID_setOn()
#DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
#DetFlags.Tile_setOff()

DetFlags.makeRIO.Calo_setOff()
DetFlags.detdescr.Calo_setOn()


#USE temporary to DEBUG
#from AthenaCommon.AppMgr import theApp
#theApp.ReflexPluginDebugLevel=1

readPool = False

from IOVDbSvc.CondDB import conddb #Here I can specify eg alignment tag
#conddb.addOverride('/Indet/Align', 'InDetAlign_EoverP_189598_189845v0')
#conddb.addOverride('/TRT/Align',   'TRTAlign_EoverP_189598_189845v0')
#conddb.addOverride('/TRT/Calib/DX','TRTCalibDX_EoverP_189598_189845v0')

#conddb.addOverride('/Indet/Align', 'InDetAlign_Nominal_ZDeltaPhi_01')
#conddb.addOverride('/TRT/Align',   'TRTAlign_Nominal_ZDeltaPhi_01')
#conddb.addOverride('/TRT/Calib/DX','TRTCalibDX_nominal')

#conddb.addOverride('/Indet/TrkErrorScaling', 'IndetTrkErrorScaling_nominal')

if not conddb.folderRequested('PIXEL/PixReco'):
  conddb.addFolder('PIXEL_OFL','/PIXEL/PixReco')

if not conddb.folderRequested('/PIXEL/PixelClustering/PixelClusNNCalib'):
  conddb.addFolder("PIXEL_OFL","/PIXEL/PixelClustering/PixelClusNNCalib")

if readPool :
  conddb.blockFolder("/Indet/Align")
  conddb.blockFolder("/TRT/Align")
  from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
  from AthenaCommon.AppMgr import ServiceMgr
  ServiceMgr += CondProxyProvider()
  ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
  # set this to the file containing AlignableTransform objects
  ServiceMgr.CondProxyProvider.InputCollections += inputCollections
  ServiceMgr.CondProxyProvider.OutputLevel=DEBUG
  print ServiceMgr.CondProxyProvider
  # this preload causes callbacks for read in objects to be activated,
  # allowing GeoModel to pick up the transforms
  ServiceMgr.IOVSvc.preLoadData=True
  ServiceMgr.IOVSvc.OutputLevel=INFO

include ("InDetRecExample/InDetRecConditionsAccess.py")

# Setup for V0 finder
InDetFlags.doxAOD.set_Value_and_Lock(True)
InDetFlags.postProcessing.set_Value_and_Lock(True)
InDetFlags.loadTools          = True
InDetFlags.doVertexFinding    = False
InDetFlags.doParticleCreation = False
InDetFlags.doConversions      = False
InDetFlags.doV0Finder         = True
InDetFlags.doSimpleV0Finder   = True
InDetFlags.doRefit            = False
InDetFlags.doPrintConfigurables = True
InDetFlags.useV0Fitter        = True

# Modify the name of the output xAOD particles and the V0 container to ensure we are not overwriting anything
from InDetRecExample.InDetKeys import InDetKeys
InDetKeys.xAODV0VertexContainer = "NewContainerV0"


# Setup for refitting of tracks in ESD
if doRefit:
  InDetFlags.preProcessing      = False
  InDetFlags.doPRDFormation        = False                                                                                                            
  InDetFlags.doSpacePointFormation = False                                                                                                                                                                                     
  InDetFlags.doNewTracking      = False
  InDetFlags.doiPatRec          = False
  InDetFlags.doxKalman          = False
  InDetFlags.doLowPt            = False
  InDetFlags.doBackTracking     = False
  InDetFlags.doTRTStandalone    = False
  InDetFlags.doTrtSegments      = False
  InDetFlags.doTrackSegmentsPixel = False
  InDetFlags.doTrackSegmentsSCT = False
  InDetFlags.doTrackSegmentsTRT = False
  InDetFlags.doSlimming         = False
  InDetFlags.doVertexFinding    = True
  InDetFlags.doParticleCreation = True
  InDetFlags.doSecVertexFinder  = False
  InDetFlags.doTrkNtuple        = False
  InDetFlags.doPixelTrkNtuple   = False
  InDetFlags.doSctTrkNtuple     = False
  InDetFlags.doTrtTrkNtuple     = False
  InDetFlags.doPixelClusterNtuple = False
  InDetFlags.doSctClusterNtuple   = False
  InDetFlags.doTrtDriftCircleNtuple = False
  InDetFlags.doVtxNtuple        = False
  InDetFlags.doConvVtxNtuple    = False
  InDetFlags.doV0VtxNtuple      = False
  InDetFlags.doRefit            = True
  InDetFlags.doLowBetaFinder    = False
  InDetFlags.doMinBias          = True

  # Modify the name of the output xAOD particles and the V0 container to ensure we are not overwriting anything
  InDetKeys.xAODTrackParticleContainer = "RefittedTrackParticles"
  InDetKeys.xAODVertexContainer = "NewContainerPrimary"


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonKshort
iDPerfMonKshort = IDPerfMonKshort(name = 'IDPerfMonKshort',
                                  tracksName = InDetKeys.xAODTrackParticleContainer(), # Point to the newly created xAOD track particles
                                  #CheckRate = 1000,
                                  triggerChainName = "NoTriggerSelection",
                                  VxContainerName = InDetKeys.xAODV0VertexContainer(), # Point to the newly created xAOD V0 container
                                  VxPrimContainerName = "PrimaryVertices",
                                  OutputLevel = ERROR)

ToolSvc += iDPerfMonKshort

from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
IDPerfMonManager = AthenaMonManager(name                = "IDPerfMonManager",
                                    FileKey             = DQMonFlags.monManFileKey(),
                                    ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                    DataType            = DQMonFlags.monManDataType(),
                                    Environment         = "user",
                                    ManualRunLBSetup    = True,
                                    Run                 = 1,
                                    LumiBlock           = 1)
IDPerfMonManager.AthenaMonTools += [ iDPerfMonKshort ]

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["IDPerfMon DATAFILE='./IDPerfMon.root' OPT='RECREATE'"]
IDPerfMonManager.FileKey = "IDPerfMon"

topSequence += IDPerfMonManager
