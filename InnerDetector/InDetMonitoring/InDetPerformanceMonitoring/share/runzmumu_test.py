#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
###############################
import os
import socket # find hostname with socket.gethostname()
#
print (' == runzmumu == START == TestArea = %s ==' %os.getenv("TestArea"))
###############################
# MC
MC_bool = False

# user defined ID alignment constants or tags
inputConstantsFile = "NONE"
userAlignTags = False

# use IDAlignment dynamic folders for Run 2 data
useIDADynamicFolders = True
if (MC_bool): useIDADynamicFolders = False # dynamic folders must be disabled in MC

EvtMax = 10000 # -1 all events
SkipEvents = 0

#fill Alignment monitoring
fillIDAmonitoring = True

# run alignment on Zmumu events
runAlignment = False

# Solneoid Current
solenoidCurrent = 7730 # 7730 is the nominal value

PoolInput = ["/eos/user/m/martis/data/InputFileForGridJobs/data18_13TeV.00352436.physics_Main.merge.DAOD_ZMUMU.f938_m1831_f938_m1982._0027.1"]
if (MC_bool): 
    PoolInput = ["/eos/user/m/martis/data/InputFileForGridJobs/ZmumuMC16_AOD.18379878._000123.pool.root.1"]

conditionsTag = "default"
#conditionsTag = "CONDBR2-BLKPA-RUN2-03"

###########################################################
print (' ========= runzmumu === config == start == ')
print (' == Athena_VERSION = %s' %os.getenv("Athena_VERSION"))
print (' == host name = %s ' % socket.gethostname() )
print (' == input file(s) = %s' %PoolInput) 
print (' == MC_bool = %s' %MC_bool)
print (' == EvtMax = %d' %EvtMax)
print (' == SkipEvents = %d' %SkipEvents)
print (' == fillIDAMonitoring = %s' %fillIDAmonitoring)
print (' == runAlignment = %s' %runAlignment)
print (' == useIDADynamicFolders = %s' %useIDADynamicFolders)
print (' == inputConstantsFile = %s ' % inputConstantsFile)
print (' == userAlignTags = %s ' % userAlignTags)
print (' == conditions tag = %s ' %conditionsTag )
print (' == solenoid current = %d A' %solenoidCurrent) 
print (' ========= runzmumu === config == end == ')

###########################################################
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AthenaCommonFlags import jobproperties
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AlgSequence import AthSequencer
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr      import ServiceMgr 
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

job = AlgSequence()
seq = AthSequencer("AthFilterSeq")

# grid jobs use all events 
if ('lxplus' not in socket.gethostname()):
    EvtMax = -1

# pass user input values to the configuration
jobproperties.AthenaCommonFlags.EvtMax.set_Value_and_Lock( EvtMax )
jobproperties.AthenaCommonFlags.SkipEvents.set_Value_and_Lock( SkipEvents )

'''
# this is the basic way, below there is one more complex that works for grid in release 22.0.24 
from AthenaConfiguration.AllConfigFlags import ConfigFlags 
ConfigFlags.Input.Files = PoolInput # this is also needed in 22.0.24
jobproperties.AthenaCommonFlags.FilesInput = PoolInput 
'''

from AthenaConfiguration.AllConfigFlags import ConfigFlags 
ConfigFlags.Input.Files = PoolInput # this is also needed in 22.0.24
jobproperties.AthenaCommonFlags.FilesInput = PoolInput 

# necessary for running on grid with 22.0.24
from RecExConfig.RecoFunctions import InputFileNames
PoolInput = InputFileNames()
print ('\n == runzmumu == from RecExConfig.RecoFunctions import InputFileNames --> PoolInput= ',PoolInput)

#Check if the input is a file or a list of files.
if isinstance(PoolInput, str):
    PoolInput = [PoolInput]

print ('\n == new input file (after tests) --> PoolInput= %s' %PoolInput) 
ConfigFlags.Input.Files = PoolInput 
jobproperties.AthenaCommonFlags.FilesInput = PoolInput

#
print ('\n == runzmumu == final list == ConfigFlags.Input.Files = ',ConfigFlags.Input.Files)
print ('\n == runzmumu == final list == jobproperties.AthenaCommonFlags.FilesInput = ',jobproperties.AthenaCommonFlags.FilesInput)
#

# use the athena logger
from AthenaCommon.Logging import logging 
msg = logging.getLogger('RunZmumu')
msg.info('\n == Input list of files: {0}'.format(PoolInput))

###########################
# user given conditions tag 
if ("default" not in conditionsTag):
    print (' == runzmumu == setting user conditionsTag as == %s' %conditionsTag)
    globalflags.ConditionsTag.set_Value_and_Lock(conditionsTag)
    globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-R2-2016-01-00-01")

# Find out if input is DRAW
inputIsDRAW = False
if any ("DRAW" in infiles for infiles in ConfigFlags.Input.Files):
    inputIsDRAW = True
    print (' == runzmumu == input is DRAW ')

##########################
# athena recognizes if input file is data or mc. 
# However from 22.0.30 when using data and trying to change alignment tags as soon as one tries:
# from IOVDbSvc.CondDB import conddb
# then athena thinks input is MC --> fix the data type
if not MC_bool:
    globalflags.DataSource.set_Value_and_Lock("data")

#
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True

# Rec flags
from RecExConfig.RecFlags import rec
rec.AutoConfiguration=['everything']
rec.doInDet.set_Value_and_Lock(True)
rec.doPerfMon.set_Value_and_Lock(False)
#
rec.doBTagging.set_Value_and_Lock(False)
rec.doZdc.set_Value_and_Lock(False)
rec.doLucid.set_Value_and_Lock(False)
rec.doTile.set_Value_and_Lock(False)
rec.doLArg.set_Value_and_Lock(False)
rec.doCalo.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)

## processing of DRAW
if (inputIsDRAW):
    # These are not needed for our purposes and can cause athena crash when running over DRAW
    print (" == runzmumu == switching off R3LargeD0 and TrackSegmentsDisappearing")
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.doR3LargeD0 = False
    InDetFlags.doTrackSegmentsDisappearing = False


#
# use dynamic alignment folders?
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
InDetGeometryFlags.useDynamicAlignFolders.set_Value_and_Lock(useIDADynamicFolders)

# 
include("RecExCond/RecExCommon_DetFlags.py") # include("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()

#
if ('21.' in os.getenv("Athena_VERSION") ):
    print (' == runzmumu == including InDetRecExample/InDetRecConditionsAccess.py')
    include("InDetRecExample/InDetRecConditionsAccess.py")

########################## 
# user given alignment constants set
if ("NONE" not in inputConstantsFile or userAlignTags):
    inputCollections = []
    if ("NONE" not in inputConstantsFile): 
        print (' == runzmumu == setting user inputConstantsFile: %s' %inputConstantsFile)
        if ('lxplus' not in socket.gethostname()): # grid jobs use another path
            inputConstantsFile = "usr/WorkDir/%s/InstallArea/%s/src/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/%s" % (os.getenv("Athena_VERSION"), os.getenv("WorkDir_PLATFORM"), inputConstantsFile)
            print (' == runzmumu == extended inputConstantsFile for grid jobs:\n --> %s' %inputConstantsFile)

        inputCollections = [inputConstantsFile]


    print (' == runzmumu == going to: from IOVDbSvc.CondDB import conddb')
    from IOVDbSvc.CondDB import conddb

    if (userAlignTags and not MC_bool):
        # different set of alignment tags for Run2 data
        if (False):
            print (' == runzmumu == setting userAlignTags == Run2_Legacy_looser')
            conddb.addOverride("/Indet/AlignL1/ID",  "IndetAlignL1ID_Run2_Legacy_looser")
            conddb.addOverride("/Indet/AlignL2/PIX", "IndetAlignL2PIX_Run2_Legacy_looser")
            conddb.addOverride("/Indet/AlignL2/SCT", "IndetAlignL2SCT_Run2_Legacy_looser")
            conddb.addOverride("/Indet/AlignL3",     "IndetAlignL3_Run2_Legacy_looser")
            conddb.addOverride("/Indet/IBLDist",     "IndetIBLDist_Run2_Legacy_looser")
            conddb.addOverride("/TRT/AlignL1/TRT",   "TRTAlignL1_Run2_Legacy_looser")
            conddb.addOverride("/TRT/AlignL2",       "TRTAlignL2_Run2_Legacy_looser")
        if (True):
            print (' == runzmumu == setting userAlignTags == MDN alignment test')
            conddb.addOverride("/Indet/AlignL1/ID",  "IndetAlignL1ID_Run2_Legacy_looser")
            conddb.addOverride("/Indet/AlignL2/PIX", "IndetAlignL2PIX_Run2_Legacy_looser")
            conddb.addOverride("/Indet/AlignL2/SCT", "IndetAlignL2SCT_Run2_Legacy_looser")
            conddb.addOverride("/Indet/AlignL3",     "IndetAlignL3_MDN_2018_2ndPart_BS+WM-SCTL2-2")
            conddb.addOverride("/Indet/IBLDist",     "IndetIBLDist_Run2_Legacy_looser")
            conddb.addOverride("/TRT/AlignL1/TRT",   "TRTAlignL1_Run2_Legacy_looser")
            conddb.addOverride("/TRT/AlignL2",       "TRTAlignL2_Run2_Legacy_looser")

    if ("NONE" not in inputConstantsFile and not MC_bool):
        # changing alignment and conditions for real data
        print (' == runzmumu == setting user constants file == %s' %inputConstantsFile)
        conddb.addOverride("/Indet/AlignL1/ID",  "IndetAlignL1ID_Run2_Legacy_looser")
        conddb.addOverride("/Indet/AlignL2/PIX", "IndetAlignL2PIX_Run2_Legacy_looser")
        conddb.addOverride("/Indet/AlignL2/SCT", "IndetAlignL2SCT_Run2_Legacy_looser")
        conddb.addOverride("/Indet/IBLDist",     "IndetIBLDist_Run2_Legacy_looser")
        conddb.addOverride("/TRT/AlignL1/TRT",   "TRTAlignL1_Run2_Legacy_looser")

        conddb.blockFolder("/Indet/AlignL3")
        conddb.blockFolder("/TRT/AlignL2")
        
        conddb.addFolder("","<dbConnection>sqlite://X;schema="+inputConstantsFile+";dbname=CONDBR2</dbConnection>/Indet/AlignL3<tag>IndetAlign_test</tag>",force=True,className="AlignableTransformContainer")
        conddb.addFolder("","<dbConnection>sqlite://X;schema="+inputConstantsFile+";dbname=CONDBR2</dbConnection>/TRT/AlignL2<tag>TRTAlign_test</tag>",force=True,className="AlignableTransformContainer")

    if ("NONE" not in inputConstantsFile and MC_bool):
        # user input alignment constants file in MC
        print (' == runzmumu == setting user constants file in MC == file: %s' %inputConstantsFile)
        conddb.blockFolder("/Indet/Align")
        conddb.blockFolder("/TRT/Align")
        conddb.addOverride("/Indet/IBLDist", "IBLDist-NULL")

        conddb.addFolder("","<dbConnection>sqlite://X;schema="+inputConstantsFile+";dbname=OFLP200</dbConnection>/Indet/Align<tag>InDetSi_MisalignmentMode_</tag>",force=True,className="AlignableTransformContainer")
        conddb.addFolder("","<dbConnection>sqlite://X;schema="+inputConstantsFile+";dbname=OFLP200</dbConnection>/TRT/Align<tag>MisalignmentMode__TRT</tag>",force=True,className="AlignableTransformContainer")
        
    if (userAlignTags and MC_bool):
        # user input alignment tags in MC
        print (' == runzmumu == setting user alignment tags in MC == Nominal_RDeltaR_01' )
        conddb.addOverride("/Indet/Align", "InDetAlign_Nominal_RDeltaR_01")
        conddb.addOverride("/TRT/Align",   "TRTAlign_Nominal_RDeltaR_01")
       
    ### carry on
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
    ServiceMgr += CondProxyProvider()
    # set this to the file containing AlignableTransform objects
    if len(inputCollections) > 0:
        ServiceMgr.CondProxyProvider.InputCollections += inputCollections
        ServiceMgr.CondProxyProvider.OutputLevel=INFO
    print (ServiceMgr.CondProxyProvider)
    # this preload causes callbacks for read in objects to be activated,
    # allowing GeoModel to pick up the transforms
    ServiceMgr.IOVSvc.preLoadData=True
    ServiceMgr.IOVSvc.OutputLevel=INFO
    
    print (' == runzmumu == condproxyprovider already set')

    if ("NONE" not in inputConstantsFile):
        print (' == runzmumu == setting user constants: inputConstantsFile = %s  - completed -' % inputConstantsFile)
    if (userAlignTags):
        print (' == runzmumu == setting user align tags ')
    
    # report
    print (ServiceMgr.IOVDbSvc)
    print (ServiceMgr.IOVSvc)

else:
    print (' == runzmumu == setting user alignment constants or tags: NO user input')

##################################
if (inputIsDRAW):
    print (' == runzmumu == setting /TRT/Onl/Cond/StatusHT')
    try:
        conddb
    except NameError:
        from IOVDbSvc.CondDB import conddb
    else:
        print (' == runzmumu == conddb already defined')
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT",className='TRTCond::StrawStatusMultChanContainer')
else:
    print (' == runzmumu == no need of /TRT/Onl/Cond/StatusHT')

##################################
# main jobOptions
include("RecExCommon/RecExCommon_topOptions.py")

#
if ('22.' in os.getenv("Athena_VERSION") and inputIsDRAW):
    ToolSvc.InDetAmbiTrackSelectionTool.doEmCaloSeed  = False

if ('22.' in os.getenv("Athena_VERSION") and inputIsDRAW and False):
    print (' == runzmumu == include("MuonConfig.py")')
    include("MuonConfig.py") # test
    #include("MuonRecExample/MuonRec_topOptions.py")

#
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = False

##########################
## magnetic field
if (MC_bool and solenoidCurrent != 7730): 
    # 7730 A is the nominal solenoid current
    import MagFieldServices.SetupField
    condSeq = AthSequencer('AthCondSeq')
    condSeq.AtlasFieldMapCondAlg.LoadMapOnStart = True #True
    condSeq.AtlasFieldMapCondAlg.UseMapsFromCOOL = False
    condSeq.AtlasFieldCacheCondAlg.LockMapCurrents = False
    condSeq.AtlasFieldCacheCondAlg.UseDCS = False
    condSeq.AtlasFieldCacheCondAlg.UseSoleCurrent = solenoidCurrent # nominal 7730 A
    condSeq.AtlasFieldCacheCondAlg.UseToroCurrent = 20400 # nominal 20400 A
    condSeq.AtlasFieldMapCondAlg.OutputLevel = DEBUG
    condSeq.AtlasFieldCacheCondAlg.OutputLevel = DEBUG
    print (' == runzmumu == solenoid current: %d A ' %condSeq.AtlasFieldCacheCondAlg.UseSoleCurrent)

    
########################
# Track Selection Tool
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
m_TrackSelectorTool_TightPrimary = InDet__InDetTrackSelectionTool(name     = "InDetTrackSelectorTightPrimary",
                                                                  CutLevel = "TightPrimary",
                                                                  minPt    = 400)
ToolSvc += m_TrackSelectorTool_TightPrimary
print (m_TrackSelectorTool_TightPrimary)

# track refitters
print ('  == runzmumu == including ElectronEoverPTracking.py for defining track fitters')
include("InDetPerformanceMonitoring/ElectronEoverPTracking.py") 

# track to vertex association
if ('22.0' in os.getenv("Athena_VERSION")): 
    print ("  == runzmumu == track to vertex association tool: defining working point ")
    from TrackVertexAssociationTool.TrackVertexAssociationToolConf import CP__TrackVertexAssociationTool
    t2vatool = CP__TrackVertexAssociationTool(name = "TrackVertexAssociationTool",
                                              WorkingPoint="SV_Reject")

# Output filename with ntuples
ServiceMgr.THistSvc.Output += ["ZmumuValidationUserSel DATAFILE='ZmumuValidationUserSel.root' OPT='RECREATE'"]

trackrefit1 = MuonRefitterTool
if ('22.0' in os.getenv("Athena_VERSION")):
    print ('  == runzmumu == Rel 22 --> trackrefit2 =  MuonRefitterToolIDSiOnly') 
    trackrefit2 = MuonRefitterToolIDSiOnly
else:
    print ('  == runzmumu == Rel 21 --> trackrefit2 =  MuonRefitterTool2') 
    trackrefit2 = MuonRefitterTool2

from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonZmumu
iDPerfMonZmumu = IDPerfMonZmumu(name = 'IDPerfMonZmumu',
                                ReFitterTool1 = trackrefit1,
                                ReFitterTool2 = trackrefit2,
                                doRefit = True, #True for evaluation of new alignment constants / False for update of weak mode recommendations -- refits tracks according to provided conditions db and/or external root file containing alignment constants
                                OutputTracksName =  "SelectedMuons",
                                isMC = MC_bool, # If set to True, the truth parameters will be filled, have a look into FillTruthParameters
                                TrackSelectionTool = m_TrackSelectorTool_TightPrimary,
                                UseTrackSelectionTool = True, # If set to True, it will use the specified TrackSelectionTool in the next Line
                                doIPextrToPV = False, # True for IP resolution studies, extrapolates IP parameters to primary vertex
                                TrackParticleName = 'CombinedTrackParticle', # CombinedTrackParticle or InnerDetectorTrackParticles
                                IDTreeFolder = "/ZmumuValidationUserSel/id",
                                combTreeFolder = "/ZmumuValidationUserSel/comb",
                                MSTreeFolder = "/ZmumuValidationUserSel/ms",
                                refit1TreeFolder = "/ZmumuValidationUserSel/refit1",# Only filled if doRefit is set to True
                                refit2TreeFolder = "/ZmumuValidationUserSel/refit2",# Only filled if doRefit is set to True
                                truthTreeFolder = "/ZmumuValidationUserSel/truth",  # Only filled if isMC is set to True
                                FourMuTreeFolder = "/ZmumuValidationUserSel/fourmu",# Only filled if doFourMuAnalysis is set to true
                                doIsoSelection = True,
                                doIPSelection = True,
                                # Muons MCP selection and muon quality
                                doMCPSelection = True, 
                                MuonQuality = "Medium", # default is Medium
                                doFourMuAnalysis = False,
                                StoreZmumuNtuple = True,
                                #loose selection to keep Z and JPsi events in the ntuple
                                MassWindowLow = 2., # in GeV
                                MassWindowHigh = 2000.,
                                PtLeadingMuon = 5., 
                                PtSecondMuon =  5., 
                                OpeningAngle = 0.01, # in radians. 1 radian ~60 deg
                                Z0Gap = 5.0, # in mm
                                doZmumuEventDebug = False, # default False
                                OutputLevel = INFO)

job += iDPerfMonZmumu
print (iDPerfMonZmumu)

##-------- Load Alignment --------------------
if (runAlignment):
    include("InDetPerformanceMonitoring/InDetAlign_Setup.py")

##-------- Load monitoring --------------------
## run global ID alignment monitoring
if (fillIDAmonitoring):
    trackCollections = ["SelectedMuonsRefit1","SelectedMuonsRefit2"]
    if (runAlignment):
        trackCollections.append("AlignTracksNormalRefitted")
        trackCollections.append("AlignTracksBeamspotConstrained")
    include("InDetPerformanceMonitoring/TrackMonitoring.py")

print (' == runzmumu == COMPLETED == ')
# end of file
