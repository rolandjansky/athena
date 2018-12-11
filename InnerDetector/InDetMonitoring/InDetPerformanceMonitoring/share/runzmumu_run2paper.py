##############################
# Rel 21
###############################
import commands
import os
#
print ' == run2paper == START == TestArea = ',os.getenv("TestArea")
print ' == run2paper  == CHECK if the TestArea is setup such that the setup.sh is sourced '
theCommand = "source %s/build/x86_64-slc6-gcc62-opt/setup.sh" %(os.getenv("TestArea"))
theOutput =  commands.getoutput(theCommand)
print ' == run2paper == Rel21 init command: ',theCommand
print '                output: ', theOutput
###############################
# enable outputs
zmumuval = True
globalAlignmentMon = True #no refitting for extracting the recommendations, only bulk as it comes out
dimuonmon = False
monitoringAllTracks = True

# use GRL
useGRL = False

# MC
MC_bool = False

# do Trigger
DoTrigger = False

# where to run
grid = True

# handle input constants
readPool = False
#inputConstants = "MisalignmentSet11_p01.pool.root"
inputConstants = "AlignmentConstants_Galo_2018BaseLine.root"
if (readPool):
    inputConstants = "usr/WorkDir/21.0.69/InstallArea/x86_64-slc6-gcc62-opt/src/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/%s" % inputConstants
    #inputConstants = "%s" % inputConstants

# weight file for IDAlignment monitoring
useWeightInMonitoring = False

# use configuration for conditions (True) or autoconfigured (False)
useConfigConditions = False

# use IDAlignment dynamic folders for 2016 data
useIDADynamicFolders = True
if (MC_bool): useIDADynamicFolders = False # dynamic folders must be disabled in MC

print ' ========= runzmumu === config == start == '
print ' == MC_bool =', MC_bool
print ' == globalAlignmentMon = ', globalAlignmentMon
print ' == useConfigConditions = ', useConfigConditions
print ' == useWeightInMonitoring =', useWeightInMonitoring
print ' == useIDADynamicFolders =', useIDADynamicFolders
print ' == useGRL =', useGRL
print ' == grid =',grid
print ' == readPool =',readPool
if (readPool): print ' == inputConstants =',inputConstants
print ' ========= runzmumu === config == end == '

#include("InDetSimpleVisual/GetDetectorPositions.py")

if grid == True:
    #PoolInput = [ "/afs/cern.ch/work/w/wollrath/private/IDAlignment/inputFiles/data16_13TeV/AOD.11063137._018527.pool.root.1" ]
    #PoolInput = [ "/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/mc15_13TeV/ESD.09385287._000001.pool.root.1"]
    #PoolInput = [ "/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/data15_5TeV/DESDM_ZMUMU.11207033._000001.pool.root.1"]
    PoolInput = [ "/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/data18_13TeV/DESDM_ZMUMU/data18_13TeV.00348354.physics_Main.merge.DESDM_ZMUMU.f920_m1831_f920_m1951._0001.1"]
else:
    #PoolInput = [ "/home/jw1095/storage/IDAlignment/inputFiles/data17_13TeV/data17_13TeV.00327265.physics_Main.merge.DESDM_ZMUMU.f832_m1616_f832_m1816._0518.1" ]
    if (MC_bool): 
        PoolInput = [ "/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/mc15_13TeV/ESD.09385287._000001.pool.root.1"
                      "/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/mc15_13TeV/ESD.09385287._001884.pool.root.1",
                      "/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/mc15_13TeV/ESD.09385287._001883.pool.root.1",
                      "/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/mc15_13TeV/ESD.09385287._001300.pool.root.1",
                      "/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/mc15_13TeV/ESD.09385287._001200.pool.root.1",
                      "/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/mc15_13TeV/ESD.09385287._001100.pool.root.1",
                      "/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/mc15_13TeV/ESD.09385287._001101.pool.root.1",
                      "/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/mc15_13TeV/ESD.09385287._001000.pool.root.1"]
    else:
        PoolInput = [ "/afs/cern.ch/user/m/martis/mywork/ZmumuNtuples/data16_13TeV/data16_13TeV.00303338.physics_Main.merge.DESDM_MCP.f716_m1624._0001.1"]

EvtMax= -1
#EvtMax= 100
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
myConditionsTag = "auto-configured"
if (useConfigConditions):
    if not MC_bool: # --> real data 
        myConditionsTag = "CONDBR2-BLKPA-2016-21" # Hide 21/03/17 
        print " -- run2paper -- globalflags.ConditionsTag -> manually configured to ", myConditionsTag
        globalflags.ConditionsTag.set_Value_and_Lock(myConditionsTag)
    #else:
    #    globalflags.ConditionsTag.set_Value_and_Lock("OFLCOND-MC15c-SDR-05")
else: 
    print " -- run2paper -- globalflags.ConditionsTag -> auto configured "

myDetDescr = "auto-configured"
if (useConfigConditions):
    myDetDescr = "ATLAS-R2-2016-01-00-01"
    print " -- run2paper -- globalflags.DetDescrVersion -> manually configured to ", myDetDescr
    globalflags.DetDescrVersion.set_Value_and_Lock(myDetDescr)#ATLAS-R2-2015-04-00-00  ATLAS-R2-2016-00-01-00
else:
    print " -- run2paper -- globalflags.DetDescrVersion -> no user setting -> ", myDetDescr

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


from IOVDbSvc.CondDB import conddb
if (useIDADynamicFolders):
    if (False):
        conddb.addOverride("/Indet/AlignL1/ID" ,"InDetAlignL1_ID_R2dynamic_data17_2ndBatch_Initial")
        conddb.addOverride("/Indet/AlignL2/PIX" ,"InDetAlignL2PIX_R2dynamic_data17_2ndBatch_Initial")
        conddb.addOverride("/Indet/AlignL2/SCT" ,"InDetAlignL2SCT_R2dynamic_data17_2ndBatch_Initial")
        conddb.addOverride("/Indet/AlignL3" ,"InDetAlignL3_R2dynamic_data17_2ndBatch_Initial")
        conddb.addOverride("/Indet/IBLDist", "InDetAlignIBLDist_R2dynamic_data17_2ndBatch_Initial")
        conddb.addOverride("/TRT/AlignL1/TRT", "TRTAlignL1_R2dynamic_data17_2ndBatch_Initial")
        conddb.addOverride("/TRT/AlignL2", "TRTAlignL2_R2dynamic_data17_2ndBatch_Initial")
    if (False):
        conddb.addOverride("/Indet/AlignL1/ID",  "IndetAlignL1ID-RUN2-BLK-UPD4-07")
        conddb.addOverride("/Indet/AlignL2/PIX", "IndetAlignL2PIX-RUN2-BLK-UPD4-07")
        conddb.addOverride("/Indet/AlignL2/SCT", "IndetAlignL2SCT-RUN2-BLK-UPD4-07")
        conddb.addOverride("/Indet/AlignL3" ,    "IndetAlignL3-RUN2-BLK-UPD4-07")
        conddb.addOverride("/Indet/IBLDist",     "IndetIBLDist-RUN2-BLK-UPD4-07") 
        conddb.addOverride("/TRT/AlignL1/TRT",   "TRTAlignL1-RUN2-BLK-UPD4-07")
        conddb.addOverride("/TRT/AlignL2",       "TRTAlignL2-RUN2-BLK-UPD4-07")


blocked_folders = [
     '/LAR/ElecCalibMC/AutoCorr',
     ]
for f in blocked_folders:
    conddb.blockFolder (f)
    print " == runzmumu == folder ",f," --> blocked "


inputCollectons =[]
if 'inputConstants' in dir():
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
                                    doIPextrToPV = True, # True for IP resolution studies, extrapolates IP parameters to primary vertex
                                    UseTrackSelectionTool = False, # If set to True, it will use the specified TrackSelectionTool in the next Line
                                    TrackSelectionTool = m_TrackSelectorTool_TightPrimary,
                                    TrackParticleName = 'InnerDetectorTrackParticles', # Currently does not do anything, code fills IndetTrackParticles and CombinedTrackParticles Trees
                                    defaultTreeFolder = "/ZmumuValidationUserSel/default",#always filled with information retrieved from TrackParticle associated to selected muon
                                    truthTreeFolder = "/ZmumuValidationUserSel/truth", # Only filled if isMC is set to True
                                    refit1TreeFolder = "/ZmumuValidationUserSel/refit1",# Only filled if doRefit is set to True
                                    refit2TreeFolder = "/ZmumuValidationUserSel/refit2",# Only filled if doRefit is set to True
                                    combTreeFolder = "/ZmumuValidationUserSel/comb",#always filled with information retrieved from TrackParticle associated to selected muon
                                    doIsoSelection = True,
                                    doIPSelection = True,
                                    #loose selection to have Z and JPsi in the n tuple
                                    MassWindowLow = 60.,
                                    MassWindowHigh = 120.,
                                    PtLeadingMuon = 15.,
                                    PtSecondMuon = 15.,
                                    OpeningAngle = 0.01, # in radians. 1 radian ~60 deg
                                    
                                    OutputLevel = DEBUG)
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
                            maxInvmass = 120.,
                            nMassBins = 60,
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
