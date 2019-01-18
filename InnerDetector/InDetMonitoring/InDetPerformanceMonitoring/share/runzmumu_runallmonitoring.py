
# enable outputs
zmumuval = True
globalAlignmentMon = True
dimuonmon = False

# use GRL
useGRL = False

# MC
MC_bool = False

#do Trigger
DoTrigger = False

# where to run
grid = True

# handle input constants
readPool = False
inputConstants = "/afs/cern.ch/work/s/sthenkel/public/forPF/ScriptsToRunWeakModeMaps/runOnBatch/AlignmentConstants/Iter0_AlignmentConstants.root"


#include("InDetSimpleVisual/GetDetectorPositions.py")

if grid == True:
    #PoolInput = ["/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/JpsiNtuplesMC/data16_13TeV.00296939.physics_Main.merge.DESDM_MCP.f689_m1587/data16_13TeV.00296939.physics_Main.merge.DESDM_MCP.f689_m1587._0004.1"]#
    PoolInput = ["/afs/cern.ch/work/d/dhaliwal/public/ForOscar/test.pool.root"]

    # xx
    # xx
    
elif 'inputFiles' in dir():
    print inputFiles
    PoolInput = inputFiles
else:
    print 'WARNING: No inputfiles specified'

#    print inputFiles
#PoolInput = inputFiles
# number of event to process

EvtMax=100
SkipEvents = 0


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AlgSequence import AthSequencer

job = AlgSequence()
seq = AthSequencer("AthFilterSeq")

###############
# GRL
###############

if useGRL:
  include("InDetPerformanceMonitoring/jobOptions_useGRL.py")

###############
# END GRL
###############

# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
#from PerfMonComps.PerfMonFlags import jobproperties as pmjp
#pmjp.PerfMonFlags.doFastMon=True

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=PoolInput
athenaCommonFlags.EvtMax = EvtMax
athenaCommonFlags.SkipEvents = SkipEvents

from AthenaCommon.GlobalFlags import globalflags
if not MC_bool:
    globalflags.ConditionsTag.set_Value_and_Lock("CONDBR2-BLKPA-2016-16")
else:
    globalflags.ConditionsTag.set_Value_and_Lock("OFLCOND-MC15c-SDR-05")

globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-R2-2015-04-00-00")


from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True

from RecExConfig.RecFlags import rec
rec.AutoConfiguration=['everything']
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False) # uncomment if rec.do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if rec.do not write ESD
rec.doAOD.set_Value_and_Lock(False) # uncomment if rec.do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if rec.do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if rec.do not write TAG
rec.doCBNT.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doInDet.set_Value_and_Lock(True)
rec.doTile.set_Value_and_Lock(False)
rec.doLArg.set_Value_and_Lock(False)
rec.doCalo.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(DoTrigger)
rec.doTruth.set_Value_and_Lock(False)

#rec.doMonitoring.set_Value_and_Lock(True)
#from AthenaMonitoring.DQMonFlags import DQMonFlags
#DQMonFlags.doInDetPerfMon.set_Value_and_Lock(True)


from AthenaCommon.AppMgr import ServiceMgr as svcMgr

include ("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
DetFlags.ID_setOn()
#DetFlags.Calo_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
#DetFlags.Tile_setOff()

#DetFlags.makeRIO.Calo_setOff()
#DetFlags.detdescr.Calo_setOn()



from IOVDbSvc.CondDB import conddb
inputCollectons =[]
if 'inputConstants' in dir():
    inputCollections = [inputConstants]


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

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = False

print svcMgr.IOVDbSvc


####  Track Selection Tool

from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
m_TrackSelectorTool_LooseMuon = InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectorLooseMuon",
                                                               CutLevel = "LooseMuon",
                                                               minPt = 25000
                                                               
                                                               )
ToolSvc += m_TrackSelectorTool_LooseMuon


#### run zmumu validation

if zmumuval == True:

    ServiceMgr.THistSvc.Output += ["ZmumuValidation DATAFILE='ZmumuValidationOut.root' OPT='RECREATE'"]
    include ("InDetPerformanceMonitoring/ElectronEoverPTracking.py")

    from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonZmumu
    if (True): 
        iDPerfMonZmumu = IDPerfMonZmumu(name = 'IDPerfMonZmumu',
                                        ReFitterTool1 = MuonRefitterTool,
                                        ReFitterTool2 = MuonRefitterTool2,
                                        OutputTracksName =  "SelectedMuons",
                                        isMC = MC_bool,
                                        doIsoSelection = True,
                                        doIPSelection  = True,
                                        UseTrigger     = DoTrigger,
                                        OutputLevel= INFO,
                                        UseTrackSelectionTool = True,
                                        MassWindowLow        = 70,
                                        MassWindowHigh       = 110,
                                        PtLeadingMuon        = 25,
                                        PtSecondMuon         = 25,
                                        TrackSelectionTool = m_TrackSelectorTool_LooseMuon,
                                        TrackParticleName= 'CombinedMuonTrackParticles')
        
        job += iDPerfMonZmumu
        print iDPerfMonZmumu

    if (False):
        iDPerfMonJPsi = IDPerfMonZmumu(name = 'IDPerfMonJPsi',
                                       ReFitterTool1 = MuonRefitterTool,
                                       ReFitterTool2 = MuonRefitterTool2,
                                       OutputTracksName =  "SelectedMuons",
                                       isMC = False,
                                       doIsoSelection = False,
                                       doIPSelection = True,
                                       OutputLevel= INFO,
                                       MassWindowLow = 1.0,
                                       MassWindowHigh = 120.,
                                       PtLeadingMuon = 1.0,
                                       OpeningAngle = 0.001,
				       #TrackTruthName ="TrackTruthCollection",
                                       TrackParticleName= 'CombinedMuonTrackParticles')
        job += iDPerfMonJPsi
        print iDPerfMonJPsi



##### run dimuon monitoring
if dimuonmon == True:
    from InDetDiMuonMonitoring.InDetDiMuonMonitoringConf import DiMuMon


    varsVSmeanZmumu = ["eta","etaAll","etaPos","etaNeg","phi","phiAll","phiPos","phiNeg","pt","ptAll","ptPos","ptNeg","etaDiff","etaSumm","phiDiff","phiSumm","crtDiff"]
    varsVSwidthZmumu = ["etaAll","etaPos","etaNeg","phiAll","phiPos","phiNeg","ptAll","ptPos","ptNeg","etaDiff","phiDiff","crtDiff"]
    varsDistrZmumu = ["etaAll","etaPos","etaNeg","phiAll","phiPos","phiNeg","ptAll","ptPos","ptNeg"]

    ZmumuMon = DiMuMon (name = "ZmumuMon_NoTrig",
                        resonName = "Zmumu",
                        minInvmass = 10.,
                        maxInvmass = 120.,
                        nMassBins = 60,
                        triggerChainName = "NoTrig",
                        regions = ["All","BB","EAEA","ECEC"],
                        varsVSmean = varsVSmeanZmumu,
                        varsVSwidth = varsVSwidthZmumu,
                        varsDistr = varsDistrZmumu,
                        doFits = True,
                        doSaveFits = False,
    #                               setDebug = True,
    OutputLevel = INFO)
    ToolSvc += ZmumuMon

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
    IDPerfMonManager.AthenaMonTools += [ ZmumuMon ]


    ServiceMgr.THistSvc.Output += ["DiMuMon DATAFILE='DiMuMon.root' OPT='RECREATE'"]
    IDPerfMonManager.FileKey = "DiMuMon"

    topSequence += IDPerfMonManager

#### run global alignment monitoring

if zmumuval == True and globalAlignmentMon == True:
    trackCollections = ["SelectedMuonsRefit1","SelectedMuonsRefit2","]
#    trackCollections = ["SelectedMuonsDefault"]
    StoreGateSvc = Service("StoreGateSvc")
    StoreGateSvc.Dump = False
    include ("InDetPerformanceMonitoring/TrackMonitoring.py")
