#############################
# Rel 21
###############################
import commands
import os
#
print ' == runzmumu == START == TestArea = ',os.getenv("TestArea")
print ' == runzmumu == CHECK if the TestArea is setup such that the setup.sh is sourced '
#theCommand = "source %s/build/x86_64-slc6-gcc62-opt/setup.sh" %(os.getenv("TestArea"))
theCommand = "source %s/x86_64-slc6-gcc62-opt/setup.sh" %(os.getenv("TestArea"))
theOutput =  commands.getoutput(theCommand)
print ' == runzmumu == Rel21 init command: ',theCommand
print '                output: ', theOutput
#
WorkDir_PLATFORM = "x86_64-slc6-gcc62-opt"
Athena_VERSION = "21.0.69"
try:
    WorkDir_PLATFORM= os.getenv("WorkDir_PLATFORM")
except:
    WorkDir_PLATFORM = "x86_64-slc6-gcc62-opt"
try:
    Athena_VERSION = os.getenv("Athena_VERSION")
except:
    Athena_VERSION = "21.0.69"
###############################
# enable outputs
zmumuval = True
globalAlignmentMon = False #no refitting for extracting the recommendations, only bulk as it comes out
dimuonmon = False
monitoringAllTracks = True

# use GRL
useGRL = False

# MC
MC_bool = True

# do Trigger
DoTrigger = False

# where to run
grid_bool = True

# handle input constants
readPool = False # default True
readLocalDynamicDB = False # default False

#inputConstants = "ReAlign_2018_L6_Step28_L3.root"
inputConstants = "step29_358395_AlignmentConstants_Iter0_Block00.root"
#inputConstants = "MisalignmentSet11_p01.pool.root"
inputdb = "Javi_Test_mycool.db"
#inputdb = "step8_Iter1_mycool.db"

if (readPool):
    print " readPool = True file: %s" %(inputConstants)
    import socket
    print(" Using host: %s" %socket.gethostname())
    if ("lxplus" not in socket.gethostname()):
        inputConstants = "usr/WorkDir/%s/InstallArea/%s/src/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/%s" % (Athena_VERSION, WorkDir_PLATFORM, inputConstants)
        inputdb = "usr/WorkDir/%s/InstallArea/%s/src/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/%s" % (Athena_VERSION, WorkDir_PLATFORM, inputdb)

# weight file for IDAlignment monitoring
useWeightInMonitoring = False

# use configuration for conditions (True) or autoconfigured (False)
useConfigConditions = True

# use IDAlignment dynamic folders for 2016 data
useIDADynamicFolders = True
if (MC_bool): useIDADynamicFolders = False # dynamic folders must be disabled in MC

print ' ========= runzmumu === config == start == '
print ' == MC_bool =', MC_bool
print ' == grid_bool =', grid_bool
print ' == globalAlignmentMon = ', globalAlignmentMon
print ' == useConfigConditions = ', useConfigConditions
print ' == useWeightInMonitoring =', useWeightInMonitoring
print ' == useIDADynamicFolders =', useIDADynamicFolders
print ' == useGRL =', useGRL
print ' == readPool =',readPool
if (readPool): 
    print ' == inputConstants =',inputConstants
print ' == readLocalDynamicDB =',readLocalDynamicDB
if (readLocalDynamicDB):
    print ' == inputdb =',inputdb
print ' ========= runzmumu === config == end == '

#include("InDetSimpleVisual/GetDetectorPositions.py")

if (grid_bool):
    #PoolInput = ["/eos/user/m/martis/data/InputFileForGridJobs/data18_13TeV.00352436.physics_Main.merge.DAOD_ZMUMU.f938_m1831_f938_m1982._0027.1"]
    PoolInput = ["/eos/user/m/martis/data/InputFileForGridJobs/DAOD_HIGG2D1.14773488._000014.pool.root.1"]
if (MC_bool): 
    PoolInput = ["/eos/user/m/martis/data/InputFileForGridJobs/ggH400NW_ZZ4lep_AOD.16564460._000001.pool.root.1"]
    #PoolInput = ["/eos/user/m/martis/data/InputFileForGridJobs/ZmumuMC16_AOD.18379878._000123.pool.root.1"]
    #PoolInput = ["/eos/user/m/martis/data/InputFileForGridJobs/mc16_13TeV_361603.PowhegPy8EG_CT10nloME_AZNLOCTEQ6L1_ZZllll_mll4.deriv.DAOD_HIGG2D1_file11.pool.root.1"]
    #PoolInput = ["/eos/user/m/martis/data/InputFileForGridJobs/mc16_13TeV.361603.PowhegPy8EG_CT10nloME_AZNLOCTEQ6L1_ZZllll_mll4.AOD.file01.pool.root.1"]

EvtMax= -1
EvtMax = 5000 #2000
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
athenaCommonFlags.FilesInput = PoolInput
athenaCommonFlags.EvtMax = EvtMax
athenaCommonFlags.SkipEvents = SkipEvents

from AthenaCommon.GlobalFlags import globalflags

myConditionsTag = "auto-configured" #"CONDBR2-BLKPA-2018-10"
if (useConfigConditions):
    if MC_bool: # -> MC
        #myConditionsTag = "OFLCOND-MC15c-SDR-05" # mc15
        myConditionsTag = "OFLCOND-MC16-SDR-25"
    if not MC_bool: # --> real data 
        myConditionsTag = "CONDBR2-BLKPA-2018-10" # RD

    print " == runzmumu == globalflags.ConditionsTag -> manually configured to ", myConditionsTag
    globalflags.ConditionsTag.set_Value_and_Lock(myConditionsTag)
        
else: 
    print " == runzmumu == globalflags.ConditionsTag -> use default: ", myConditionsTag 

myDetDescr = "auto-configured" #ATLAS-R2-2015-04-00-00  ATLAS-R2-2016-00-01-00
if (useConfigConditions):
    myDetDescr = "ATLAS-R2-2016-01-00-01"
    print " == runzmumu == globalflags.DetDescrVersion -> manually configured to ", myDetDescr
    globalflags.DetDescrVersion.set_Value_and_Lock(myDetDescr)
else:
    print " == runzmumu == globalflags.DetDescrVersion -> no user setting. Using default: ", myDetDescr

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
InDetFlags.useDynamicAlignFolders.set_Value_and_Lock(useIDADynamicFolders)



from AthenaCommon.AppMgr import ServiceMgr as svcMgr

include("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()


print " == runzmumu == user may define his favourite alignment == start == "
from IOVDbSvc.CondDB import conddb
if (useIDADynamicFolders):
    if (False):
        print (" == runzmumu == configuring 2018 ReAlign family ") 
        conddb.addOverride("/Indet/AlignL1/ID" ,"IndetAlignL1ID-R2dynamic_2018_ReAlign_Initial")
        conddb.addOverride("/Indet/AlignL2/PIX" ,"IndetAlignL2PIX-R2dynamic_2018_ReAlign_Initial")
        conddb.addOverride("/Indet/AlignL2/SCT" ,"IndetAlignL2SCT-R2dynamic_2018_ReAlign_Initial")
        conddb.addOverride("/Indet/AlignL3" ,"IndetAlignL3-R2dynamic_2018_ReAlign_Initial")
        conddb.addOverride("/Indet/IBLDist", "IndetAlignL3-R2dynamic_2018_ReAlign_Initial")
        conddb.addOverride("/TRT/AlignL1/TRT", "TRTAlignL1-R2dynamic_2018_ReAlign_Initial")
        conddb.addOverride("/TRT/AlignL2", "TRTAlignL2-R2dynamic_2018_ReAlign_Initial")
    if (False):
        if readPool :
            conddb.blockFolder("/Indet/AlignL3")
            conddb.blockFolder("/TRT/AlignL2")
        if readLocalDynamicDB :
            conddb.blockFolder("/Indet/AlignL1/ID")
            conddb.blockFolder("/Indet/AlignL2/PIX")
            conddb.blockFolder("/Indet/AlignL2/SCT")
            conddb.blockFolder("/TRT/AlignL1/TRT")
            conddb.blockFolder("/Indet/IBLDist")

            conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+inputdb+';dbname=CONDBR2</dbConnection>/Indet/AlignL1/ID','IndetL1Test',True);
            conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+inputdb+';dbname=CONDBR2</dbConnection>/Indet/AlignL2/PIX','IndetL2PIXTest',True);
            conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+inputdb+';dbname=CONDBR2</dbConnection>/Indet/AlignL2/SCT','IndetL2SCTTest',True);
            conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+inputdb+';dbname=CONDBR2</dbConnection>/TRT/AlignL1/TRT','IndetL1TRTTest',True);
            conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+inputdb+';dbname=CONDBR2</dbConnection>/Indet/IBLDist','IndetIBLDist',True); 

    if (False): # --> ReAlign 2018 default True
        print (" == runzmumu == tags + SiL3 + TRTL2.... ") 
        conddb.addOverride("/Indet/AlignL1/ID",  "IndetAlignL1ID-R2dynamic_2018_ReAlign_Initial")
        conddb.addOverride("/Indet/AlignL2/PIX", "IndetAlignL2PIX-R2dynamic_2018_ReAlign_Initial")
        conddb.addOverride("/Indet/AlignL2/SCT", "IndetAlignL2SCT-R2dynamic_2018_ReAlign_Initial")
        conddb.addOverride("/Indet/IBLDist",     "IndetIBLDist-R2dynamic_2018_ReAlign_Initial")
        conddb.addOverride("/TRT/AlignL1/TRT",   "TRTAlignL1-R2dynamic_2018_ReAlign_Initial")
        if readPool :
            conddb.blockFolder("/Indet/AlignL3")
            conddb.blockFolder("/TRT/AlignL2")
        
print " == runzmumu == user may define his favourite alignment == completed == "
##


blocked_folders = ['/LAR/ElecCalibMC/AutoCorr']
for f in blocked_folders:
    conddb.blockFolder (f)
    print " == runzmumu == folder ",f," --> blocked "


inputCollectons =[]
if 'inputConstants' in dir() and readPool:
    inputCollections = [inputConstants]
    print " == runzmumu == inputConstants in dir() == ", inputConstants

if readPool:
    if (MC_bool):
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
    
include("InDetRecExample/InDetRecConditionsAccess.py")

# main jobOption
include("RecExCommon/RecExCommon_topOptions.py")

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = False

print svcMgr.IOVDbSvc


####  Track Selection Tool
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
m_TrackSelectorTool_Loose = InDet__InDetTrackSelectionTool(name     = "InDetTrackSelectorLoose",
                                                           CutLevel = "Loose",
                                                           minPt    = 400)
ToolSvc += m_TrackSelectorTool_Loose

m_TrackSelectorTool_TightPrimary = InDet__InDetTrackSelectionTool(name     = "InDetTrackSelectorTightPrimary",
                                                                  CutLevel = "TightPrimary",
                                                                  minPt    = 400)
ToolSvc += m_TrackSelectorTool_TightPrimary
print m_TrackSelectorTool_TightPrimary

#### run zmumu validation
if zmumuval == True:
    include("InDetPerformanceMonitoring/ElectronEoverPTracking.py")

    from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonZmumu

    #use to run with qualityselection that needs to be set in MuonSelector
    ServiceMgr.THistSvc.Output += ["ZmumuValidationUserSel DATAFILE='ZmumuValidationUserSel.root' OPT='RECREATE'"]
    iDPerfMonZmumu = IDPerfMonZmumu(name = 'IDPerfMonZmumu',
                                    ReFitterTool1 = MuonRefitterTool,
                                    ReFitterTool2 = MuonRefitterTool2,
                                    OutputTracksName =  "SelectedMuons",
                                    isMC = MC_bool, # If set to True, the truth parameters will be filled, have a look into FillTruthParameters
                                    doRefit = True, #True for evaluation of new alignment constants / False for update of weak mode recommendations -- refits tracks according to provided conditions db and/or external root file containing alignment constants
                                    doIPextrToPV = False, # True for IP resolution studies, extrapolates IP parameters to primary vertex
                                    UseTrackSelectionTool = False, # If set to True, it will use the specified TrackSelectionTool in the next Line
                                    TrackSelectionTool = m_TrackSelectorTool_TightPrimary,
                                    TrackParticleName = 'InnerDetectorTrackParticles', # Currently does not do anything, code fills IndetTrackParticles and CombinedTrackParticles Trees
                                    defaultTreeFolder = "/ZmumuValidationUserSel/default",#always filled with information retrieved from TrackParticle associated to selected muon
                                    truthTreeFolder = "/ZmumuValidationUserSel/truth", # Only filled if isMC is set to True
                                    refit1TreeFolder = "/ZmumuValidationUserSel/refit1",# Only filled if doRefit is set to True
                                    refit2TreeFolder = "/ZmumuValidationUserSel/refit2",# Only filled if doRefit is set to True
                                    combTreeFolder = "/ZmumuValidationUserSel/comb",#always filled with information retrieved from TrackParticle associated to selected muon
                                    FourMuTreeFolder = "/ZmumuValidationUserSel/fourmu",# Only filled if doFourMuAnalysis is set to true
                                    doIsoSelection = True,
                                    doIPSelection = True,
                                    doMCPSelection = True, # Medium 
                                    doFourMuAnalysis = True,
                                    StoreZmumuNtuple = False,
                                    #loose selection to keep Z and JPsi events in the ntuple
                                    MassWindowLow = 2.,
                                    MassWindowHigh = 2000.,
                                    PtLeadingMuon = 5., #10 #4 #15.,
                                    PtSecondMuon =  5., #10 #4 #15.,
                                    OpeningAngle = 0.01, # in radians. 1 radian ~60 deg
                                    Z0Gap = 5.0, # in mm
                                    OutputLevel = INFO)
    job += iDPerfMonZmumu
    print iDPerfMonZmumu
    
    ##### run dimuon monitoring
    if (dimuonmon):
        from InDetDiMuonMonitoring.InDetDiMuonMonitoringConf import DiMuMon
        
        
        varsVSmeanZmumu = ["eta","etaAll","etaPos","etaNeg","phi","phiAll","phiPos","phiNeg","pt","ptAll","ptPos","ptNeg","etaDiff","etaSumm","phiDiff","phiSumm","crtDiff"]
        varsVSwidthZmumu = ["etaAll","etaPos","etaNeg","phiAll","phiPos","phiNeg","ptAll","ptPos","ptNeg","etaDiff","phiDiff","crtDiff"]
        varsDistrZmumu = ["etaAll","etaPos","etaNeg","phiAll","phiPos","phiNeg","ptAll","ptPos","ptNeg"]
        
        ZmumuMon = DiMuMon (name = "ZmumuMon_NoTrig",
                            resonName = "Zmumu",
                            minInvmass = 10.,
                            maxInvmass = 180.,
                            nMassBins = 80,
                            triggerChainName = "NoTrig",
                            regions = ["All","BB","EAEA","ECEC"],
                            varsVSmean = varsVSmeanZmumu,
                            varsVSwidth = varsVSwidthZmumu,
                            varsDistr = varsDistrZmumu,
                            doFits = True,
                            doSaveFits = False,
                            #setDebug = True,
                            OutputLevel = DEBUG)
        ToolSvc += ZmumuMon
    
    # continue
        from AthenaMonitoring.DQMonFlags import DQMonFlags
        from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
        IDPerfMonManager = AthenaMonManager(name            = "IDPerfMonManager",
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
    
### add CombinedInDetTracks (can be used for monitoring purposes) ###
if (monitoringAllTracks):
    from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__TrackCollectionProvider
    trackCollectionProvider=Trk__TrackCollectionProvider("TrackCollectionProvider",
                                                         OutputLevel = DEBUG,
                                                         InputTrkCol = "CombinedInDetTracks")
    print " == runzmumu_script == track collection provider -- "
    print trackCollectionProvider
    ToolSvc += trackCollectionProvider

#### run global alignment monitoring
if (zmumuval and globalAlignmentMon):
    trackCollections = ["SelectedMuonsRefit1", "SelectedMuonsRefit2", "Tracks"]
    if (monitoringAllTracks): trackCollections.append("CombinedInDetTracks")
#   trackCollections = ["SelectedMuonsDefault"]
    StoreGateSvc = Service("StoreGateSvc")
    StoreGateSvc.Dump = False # True in case of debug ** WARNING ** very large output
    include("InDetPerformanceMonitoring/TrackMonitoring.py")
    #include("TrackMonitoring.py")
