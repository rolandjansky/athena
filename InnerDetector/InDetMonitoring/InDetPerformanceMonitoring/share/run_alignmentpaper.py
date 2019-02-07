###############################
# Rel 21
###############################

import commands
import os

# For colored printouts
RED   = "\033[1;31m"
BLUE  = "\033[1;34m"
CYAN  = "\033[1;36m"
GREEN = "\033[0;32m"
RESET = "\033[0;0m"
BOLD    = "\033[;1m"
sys.stdout.write(BLUE)
print ' == ALIGNMENT PAPER == START == TestArea = ',os.getenv("TestArea")
print ' == ALIGNMENT PAPER == CHECK if the TestArea is setup such that the setup.sh is sourced '
theCommand = "source %s/build/x86_64-slc6-gcc62-opt/setup.sh" %(os.getenv("TestArea"))
theOutput =  commands.getoutput(theCommand)
print ' == ALIGNMENT PAPER == Rel21 init command: ',theCommand
print '                output: ', theOutput
###############################
# enable outputs
zmumuval = True
jpsival=False
upsilonval=False

globalAlignmentMon = True #no refitting for extracting the recommendations, only bulk as it comes out
dimuonmon = False

# use GRL
useGRL = True

# MC
#MC_bool = False # Will be set automatically later when reading metadata of the file you are running over

#do Trigger
DoTrigger = False

# where to run
grid = True

# handle input constants
readPool = False
inputConstants = "/afs/cern.ch/work/s/sthenkel/public/forPF/ScriptsToRunWeakModeMaps/runOnBatch/AlignmentConstants/Iter0_AlignmentConstants.root"

# weight file for IDAlignment monitoring
useWeightInMonitoring = False

# use configuration for conditions (True) or autoconfigured (False)
useConfigConditions = False

# use IDAlignment dynamic folders for 2016 data
useIDADynamicFolders = False

print ' ========= ALIGNMENT PAPER === config == start == '
print ' == runzmumu selection =', zmumuval
print ' == run jpsi selection =', jpsival
print ' == run upsilon selection =', upsilonval
print ' == globalAlignmentMon = ', globalAlignmentMon
print ' == useConfigConditions = ', useConfigConditions
print ' == useWeightInMonitoring =', useWeightInMonitoring
print ' == useIDADynamicFolders =', useIDADynamicFolders
print 'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'
print ' \n\n\n== gridMode =', grid, "\n\n\n"
print 'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'
print ' ========= ALIGNMENT PAPER === config == end == '

#include("InDetSimpleVisual/GetDetectorPositions.py")

if grid == True:
    #
    # (SH) - When running on the grid, you need to choose a local file (either MC or data),
    #        please let me know in case you find a better solution
    #PoolInput = ["/data/sthenkel/work/Performance/Alignment/data16_13TeV.00306310.physics_Main.merge.AOD.r9264_p3083/AOD.11138483._002020.pool.root.1"]
     PoolInput = ["/data/sthenkel/work/Performance/Alignment/mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.AOD.e3601_s2576_s2132_r10005_r7676/AOD.12298370._000065.pool.root.1"]

elif 'inputFiles' in dir():
    print inputFiles
    PoolInput = inputFiles
else:
    print 'WARNING: No inputfiles specified'

#    print inputFiles
#PoolInput = inputFiles
# number of event to process

EvtMax= -1 #
SkipEvents = 0


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AlgSequence import AthSequencer

job = AlgSequence()
seq = AthSequencer("AthFilterSeq")




# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
#from PerfMonComps.PerfMonFlags import jobproperties as pmjp
#pmjp.PerfMonFlags.doFastMon=True

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=PoolInput
athenaCommonFlags.EvtMax = EvtMax
athenaCommonFlags.SkipEvents = SkipEvents

#
# Get information about the input file
#(SH)
from PyUtils import AthFile
af = AthFile.fopen( athenaCommonFlags.FilesInput()[0] )
camp = af.fileinfos['tag_info']['project_name']

if "IS_DATA" in af.fileinfos['evt_type'][0]:
    print "-- INFO :: user JO ----------------------------"
    print "- You run on data -"
    print "-----------------------------------------------"
    MC_bool = False
elif "IS_SIMULATION" in af.fileinfos['evt_type'][0]:
    print "-- INFO :: user JO ----------------------------"
    print "- You run on MC -"
    print "-----------------------------------------------"
    if [zmumuval, jpsival, upsilonval].count(True) > 1:
        print '-- > EXIT :: you try to run on MC for more then one mumu resonance... choose one when running on MC!'
        sys.exit()
    MC_bool = True

else:
    print "ERROR :: Define what the evt type is! ... aborting, currently think it is : ", af.fileinfos['evt_type']
    sys.exit()


###############
# GRL
###############
if useGRL and not "IS_SIMULATION" in camp:
    print "-- INFO :: user JO ----------------------------"
    print "- Enabling GRL for "+str(camp)+"-"
    print "-----------------------------------------------"
    if "15" in camp:
        print "INFO (user JO) :: Importing 2015 GRL ------"
        include("InDetPerformanceMonitoring/jobOptions_useGRL_2015.py")

    if "16" in camp:
        print "INFO (user JO) :: Importing 2016 GRL ------"
        include("InDetPerformanceMonitoring/jobOptions_useGRL_2016.py")

###############
# END GRL
###############

from AthenaCommon.GlobalFlags import globalflags
myConditionsTag = "auto-configured"
if (useConfigConditions):
    if not MC_bool: # --> real data
        myConditionsTag = "CONDBR2-BLKPA-2016-21" # Hide 21/03/17
        print " === ALIGNMENT PAPER -- globalflags.ConditionsTag -> manually configured to ", myConditionsTag
        globalflags.ConditionsTag.set_Value_and_Lock(myConditionsTag)
    #else:
    #    globalflags.ConditionsTag.set_Value_and_Lock("OFLCOND-MC15c-SDR-05")
else:
    print " === ALIGNMENT PAPER -- globalflags.ConditionsTag -> auto configured "

myDetDescr = "auto-configured"
if (useConfigConditions):
    myDetDescr = "ATLAS-R2-2015-04-00-00"
    myDetDescr = "ATLAS-R2-2015-03-29-00" # Hide 21/03/17
    print " === ALIGNMENT PAPER -- globalflags.DetDescrVersion -> manually configured to ", myDetDescr
    globalflags.DetDescrVersion.set_Value_and_Lock(myDetDescr)#ATLAS-R2-2015-04-00-00  ATLAS-R2-2016-00-01-00
else:
    print " === ALIGNMENT PAPER -- globalflags.DetDescrVersion -> no user setting -> ", myDetDescr

# Resetting the colored print out
sys.stdout.write(RESET)

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

include ("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
DetFlags.ID_setOn()
#DetFlags.Calo_setOn() # Salva R21.0.8
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
#DetFlags.Tile_setOff()
#DetFlags.Tile_setOn() # Salva R21.0.8
#DetFlags.LAr_setOn() # Salva R21.0.8

#DetFlags.makeRIO.Calo_setOff()
#DetFlags.detdescr.Calo_setOn()



from IOVDbSvc.CondDB import conddb
if (useIDADynamicFolders):
    conddb.addOverride("/Indet/AlignL1/ID" ,"IndetAlignL1ID-Repro-Rel21-nonUPD-00")
    conddb.addOverride("/Indet/AlignL2/PIX" ,"IndetAlignL2PIX-Repro-Rel21-nonUPD-00")
    conddb.addOverride("/Indet/AlignL2/SCT" ,"IndetAlignL2SCT-Repro-Rel21-nonUPD-00")
    conddb.addOverride("/Indet/AlignL3" ,"IndetAlignL3-Repro-Rel21-nonUPD-00")
    conddb.addOverride("/Indet/IBLDist", "IndetIBLDist-Repro-Rel21-nonUPD-00")
    conddb.addOverride("/TRT/AlignL1/TRT", "TRTAlignL1-Repro-Rel21-nonUPD-00")
    conddb.addOverride("/TRT/AlignL2", "TRTAlignL2-Repro-Rel21-nonUPD-00")

    conddb.addOverride("/Indet/Beampos", "IndetBeampos-Dec16-dataXX_13TevRepro2016-v0")


blocked_folders = [
     '/LAR/ElecCalibMC/AutoCorr',
     ]
for f in blocked_folders:
    conddb.blockFolder (f)
    sys.stdout.write(BLUE)
    print " == ALIGNMENT PAPER == folder ",f," --> blocked "
    sys.stdout.write(RESET)

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
                                    MassWindowLow = 60.,
                                    MassWindowHigh = 120.,
                                    PtLeadingMuon = 15.0,
                                    PtSecondMuon = 15.0,
                                    OpeningAngle = .0, # in radians. 1 radian ~60 deg
                                    OutputLevel= DEBUG)
    job += iDPerfMonZmumu
    print iDPerfMonZmumu

if jpsival == True:
    ServiceMgr.THistSvc.Output += ["JpsimumuValidationUserSel DATAFILE='JpsimumuValidationUserSel.root' OPT='RECREATE'"]
    iDPerfMonJPsi = IDPerfMonZmumu(name = 'IDPerfMonJPsi',
                                       ReFitterTool1 = MuonRefitterTool,
                                       ReFitterTool2 = MuonRefitterTool2,
                                       OutputTracksName =  "SelectedMuons",
                                       isMC = MC_bool,
                                       doRefit = False,
                                       doIPextrToPV = False,
                                       UseTrackSelectionTool = False,
                                       TrackSelectionTool = m_TrackSelectorTool_TightPrimary,
                                       TrackParticleName = 'InnerDetectorTrackParticles',
#                                       TrackParticleName= 'CombinedMuonTrackParticles'
                                       defaultTreeFolder = "/JpsimumuValidationUserSel/default",
                                       truthTreeFolder = "/JpsimumuValidationUserSel/truth",
                                       refit1TreeFolder = "/JpsimumuValidationUserSel/refit1",
                                       refit2TreeFolder = "/JpsimumuValidationUserSel/refit2",
                                       combTreeFolder = "/JpsimumuValidationUserSel/comb",
                                       doIsoSelection = False,
                                       doIPSelection = True,
                                       MassWindowLow = 1.0,
                                       MassWindowHigh = 6.,
                                       PtLeadingMuon = 1.0,
                                       PtSecondMuon = 1.0,
                                       OpeningAngle = .0,
                                       OutputLevel= DEBUG)
    job += iDPerfMonJPsi
    print iDPerfMonJPsi

if upsilonval == True:
    ServiceMgr.THistSvc.Output += ["UpsilonmumuValidationUserSel DATAFILE='UpsilonmumuValidationUserSel.root' OPT='RECREATE'"]
    iDPerfMonUpsilon = IDPerfMonZmumu(name = 'IDPerfMonUpsilon',
                                       ReFitterTool1 = MuonRefitterTool,
                                       ReFitterTool2 = MuonRefitterTool2,
                                       OutputTracksName =  "SelectedMuons",
                                       isMC = MC_bool,
                                       doRefit = False,
                                       doIPextrToPV = False,
                                       UseTrackSelectionTool = False,
                                       TrackSelectionTool = m_TrackSelectorTool_TightPrimary,
                                       TrackParticleName = 'InnerDetectorTrackParticles',
#                                       TrackParticleName= 'CombinedMuonTrackParticles'
                                       defaultTreeFolder = "/UpsilonmumuValidationUserSel/default",
                                       truthTreeFolder = "/UpsilonmumuValidationUserSel/truth",
                                       refit1TreeFolder = "/UpsilonmumuValidationUserSel/refit1",
                                       refit2TreeFolder = "/UpsilonmumuValidationUserSel/refit2",
                                       combTreeFolder = "/UpsilonmumuValidationUserSel/comb",
                                       doIsoSelection = False,
                                       doIPSelection = True,
                                       MassWindowLow = 6,
                                       MassWindowHigh = 12.,
                                       PtLeadingMuon = 5.0,
                                       PtSecondMuon = 5.0,
                                       OpeningAngle = .0,
                                       OutputLevel= DEBUG)
    job += iDPerfMonUpsilon
    print iDPerfMonUpsilon







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
