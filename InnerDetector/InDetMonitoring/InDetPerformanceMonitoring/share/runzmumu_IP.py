
# enable outputs
zmumuval = True
globalAlignmentMon = True #no refitting for extracting the recommendations, only bulk as it comes out
dimuonmon = False

# use GRL
useGRL = False

# MC
MC_bool = False

#do Trigger
DoTrigger = False

# where to run
grid = False

# handle input constants
readPool = False
inputConstants = "/afs/cern.ch/work/s/sthenkel/public/forPF/ScriptsToRunWeakModeMaps/runOnBatch/AlignmentConstants/Iter0_AlignmentConstants.root"


#include("InDetSimpleVisual/GetDetectorPositions.py")

if grid == True:
    #PoolInput = ["/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/JpsiNtuplesMC/data16_13TeV.00296939.physics_Main.merge.DESDM_MCP.f689_m1587/data16_13TeV.00296939.physics_Main.merge.DESDM_MCP.f689_m1587._0004.1"]
    PoolInput = ["/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/mc15_13TeV/mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.ESD.e3601_s2832_r8014/ESD.09385287._001000.pool.root.1"]
    #PoolInput = ["/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/data16_13TeV/Run00300800_preTS1_Rel21/DESDM_ZMUMU.10722146._000001.pool.root.1"]
    #PoolInput = ["/afs/cern.ch/user/o/oestrada/eos/atlas/user/o/oestrada/mc15_13TeV.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4.merge.AOD.e3735_s2726_r7326_r6282/AOD.07103156._000849.pool.root.1"]

    # include patch to resolve issue with xrootd in 20.7.7+ releases, Thanks Will diClemente for following up this -- https://its.cern.ch/jira/browse/ATEAM-340
    #include("InDetPerformanceMonitoring/xrootd-patch.py")

elif 'inputFiles' in dir():
    print inputFiles
    PoolInput = inputFiles
else:
    print 'WARNING: No inputfiles specified'

#    print inputFiles
#PoolInput = inputFiles
# number of event to process

EvtMax= 100 # -1
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
if (False):
    if not MC_bool:
        print " -- salva -- globalflags.ConditionsTag -> manually configured "
        globalflags.ConditionsTag.set_Value_and_Lock("CONDBR2-BLKPA-2016-21")
    #else:
    #    globalflags.ConditionsTag.set_Value_and_Lock("OFLCOND-MC15c-SDR-05")
else:
    print " -- salva -- globalflags.ConditionsTag -> auto configured "

myDetDescr = "auto-configured"
if (False):
    myDetDescr = "ATLAS-R2-2015-04-00-00"
    print " -- salva -- globalflags.DetDescrVersion -> manually configured to ", myDetDescr
    globalflags.DetDescrVersion.set_Value_and_Lock(myDetDescr)#ATLAS-R2-2015-04-00-00  ATLAS-R2-2016-00-01-00
else:
    print " -- salva -- globalflags.DetDescrVersion -> no user setting -> ", myDetDescr

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

#use dynamic alignment
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.useDynamicAlignFolders.set_Value_and_Lock(False)

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
  ServiceMgr.CondProxyProvider.OutputLevel=INFO
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
m_TrackSelectorTool_Loose = InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectorLoose",
                                                           CutLevel = "Loose",
                                                           minPt = 400
#                                                                  maxD0overSigmaD0 = 1.5
                                                                  )
ToolSvc += m_TrackSelectorTool_Loose

m_TrackSelectorTool_TightPrimary = InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectorTightPrimary",
                                                                  CutLevel = "TightPrimary",
                                                                  minPt = 400
                                                             #     maxD0overSigmaD0 = 1.5,

                                                                  )
ToolSvc += m_TrackSelectorTool_TightPrimary

#### run zmumu validation

if zmumuval == True:

#    ServiceMgr.THistSvc.Output += ["ZmumuValidation DATAFILE='ZmumuValidationOut.root' OPT='RECREATE'"]
    include ("InDetPerformanceMonitoring/ElectronEoverPTracking.py")

    from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonZmumu

#use to run with qualityselection that needs to be set in MuonSelector
    ServiceMgr.THistSvc.Output += ["ZmumuValidationUserSel DATAFILE='ZmumuValidationUserSel.root' OPT='RECREATE'"]
    iDPerfMonZmumu = IDPerfMonZmumu(name = 'IDPerfMonZmumu',
                                    ReFitterTool1 = MuonRefitterTool,
                                    ReFitterTool2 = MuonRefitterTool2,
                                    OutputTracksName =  "SelectedMuons",
                                    isMC = MC_bool, # If set to True, the truth parameters will be filled, have a look into FillTruthParameters
                                    doRefit = False, #True for evaluation of new alignment constants / False for update of weak mode recommendations -- refits tracks according to provided conditions db and/or external root file containing alignment constants
                                    doIPextrToPV = False, # True for IP resolution studies, extrapolates IP parameters to primary vertex
                                    UseTrackSelectionTool = False, # If set to True, it will use the specified TrackSelectionTool in the next Line
                                    TrackSelectionTool = m_TrackSelectorTool_TightPrimary,
                                    TrackParticleName = 'InnerDetectorTrackParticles', # Currently does not do anything, code fills IndetTrackParticles and CombinedTrackParticles Trees
                                    defaultTreeFolder = "/ZmumuValidationUserSel/default",#always filled with information retrieved from TrackParticle associated to selected muon
                                    truthTreeFolder = "/ZmumuValidationUserSel/truth", # Only filled if isMC is set to True
                                    refit1TreeFolder = "/ZmumuValidationUserSel/refit1",# Only filled if doRefit is set to True
                                    refit2TreeFolder = "/ZmumuValidationUserSel/refit2",# Only filled if doRefit is set to True
                                    combTreeFolder = "/ZmumuValidationUserSel/comb",#always filled with information retrieved from TrackParticle associated to selected muon
                                    doIsoSelection = False,
                                    doIPSelection = True,
                                    #loose selection to have Z and JPsi in the n tuple
                                    MassWindowLow = 60.,
                                    MassWindowHigh = 120.,
                                    PtLeadingMuon = 15.0,
                                    PtSecondMuon = 15.0,
                                    OpeningAngle = 1.00, # in radians. 1 radian ~60 deg
                                    OutputLevel= DEBUG)
    job += iDPerfMonZmumu
    print iDPerfMonZmumu

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
    trackCollections = ["SelectedMuonsRefit1","SelectedMuonsRefit2"]
#    trackCollections = ["SelectedMuonsDefault"]
    StoreGateSvc = Service("StoreGateSvc")
    StoreGateSvc.Dump = False
    include ("InDetPerformanceMonitoring/TrackMonitoring.py")
