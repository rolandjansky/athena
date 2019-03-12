#--------------------------------------------------------------
# options
#--------------------------------------------------------------

if not 'sample' in globals():
    sample = 'mc' # 'data' or 'mc'

if not 'do_grl' in globals():
    do_grl = True


#--------------------------------------------------------------
# input samples
#--------------------------------------------------------------

from glob import glob

if sample == 'data':
#    file_list = glob('/local_data0/reece/data/trt_eff/data/data10_7TeV.00167607.physics_Egamma.merge.ESD.r1774_p327_tid203271_00/ESD*.root*')
    file_list = glob('/local_data0/reece/data/trt_eff/data/data10_7TeV.00167607.physics_JetTauEtmiss.merge.ESD.r1774_p327_tid203202_00/ESD*.root*')
    file_list.sort()
#    output_ntuple = 'trt_eff.data10_7TeV.00167607.physics_Egamma.root'
    output_ntuple = 'trt_eff.data10_7TeV.00167607.physics_JetTauEtmiss.root'
elif sample == 'mc':
    file_list = glob('/local_data0/reece/data/trt_eff/mc/mc10_valid.105012.J3_pythia_jetjet.recon.ESD.e574_s934_s946_r1624_tid177734_00/ESD*.root*')
    file_list.sort()
    output_ntuple = 'trt_eff.mc10_valid.105012.J3_pythia_jetjet.root'
else:
    pass

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = file_list

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

theApp.EvtMax = 10

#------------------------------------------------------------------------------
# Good Run List
#------------------------------------------------------------------------------
if do_grl and sample == 'data':
    # add LumiBlockMetaDataTool to ToolSvc and configure
    from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
    theLumiBlockMetaDataTool = LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )
    theLumiBlockMetaDataTool.calcLumi = False # False by default
    theLumiBlockMetaDataTool.storeXMLFiles = True
    theLumiBlockMetaDataTool.applyDQCuts = True
    theLumiBlockMetaDataTool.OutputLevel = INFO
    ToolSvc += theLumiBlockMetaDataTool
    
    # add ToolSvc.LumiBlockMetaDataTool to MetaDataSvc
    from AthenaServices.AthenaServicesConf import MetaDataSvc
    theMetaDataSvc = MetaDataSvc( "MetaDataSvc" )
    theMetaDataSvc.MetaDataTools += [ theLumiBlockMetaDataTool ]
    svcMgr += theMetaDataSvc
    
    # Configure the goodrunslist selector tool
    from GoodRunsLists.GoodRunsListsConf import *
    theGoodRunsListSelectorTool = GoodRunsListSelectorTool()
    theGoodRunsListSelectorTool.GoodRunsListVec = [ 'MyLBCollection.xml' ]
    theGoodRunsListSelectorTool.PassThrough = False
    theGoodRunsListSelectorTool.OutputLevel = INFO
    ToolSvc += theGoodRunsListSelectorTool
    
    ## This Athena job consists of algorithms that loop over events;
    ## here, the (default) top sequence is used:
    from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
    job = AlgSequence()
    seq = AthSequencer("AthMasterSeq")
    
    ## GRL selector, dummy ntuple dumper
    from GoodRunsListsUser.GoodRunsListsUserConf import *
    theGRLTriggerSelectorAlg = GRLTriggerSelectorAlg('GRLTriggerAlg1')
    theGRLTriggerSelectorAlg.GoodRunsListArray = ['MyLBCollection']
    theGRLTriggerSelectorAlg.OutputLevel = INFO
#    theGRLTriggerSelectorAlg.TriggerSelectionRegistration = 'L1_MBTS_1' # 'L1_MBTS_1_1' #'L1_RD0_EMPTY'
    seq += theGRLTriggerSelectorAlg
    
#    theDummyDumperAlg = DummyDumperAlg('DummyDumperAlg1')
#    theDummyDumperAlg.RootFileName = 'selection1.root'
#    theDummyDumperAlg.GRLNameVec = [ 'LumiBlocks_GoodDQ0', 'IncompleteLumiBlocks_GoodDQ0' ]
#    job += theDummyDumperAlg
    

#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = False
doWriteAOD      = False
# --- do auditors ?
doAuditors      = False

import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True 
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False

#--------------------------------------------------------------
# Additional Detector Setup
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
rec.Commissioning=False

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
#DetFlags.makeRIO.pixel_setOff()
#DetFlags.makeRIO.SCT_setOff()
#DetFlags.makeRIO.TRT_setOff()

# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# ---- switch parts of ID off/on as follows (always use both lines)
#DetFlags.pixel_setOff()
#DetFlags.detdescr.pixel_setOn()
#DetFlags.SCT_setOff()
#DetFlags.detdescr.SCT_setOn()
#DetFlags.TRT_setOff()
#DetFlags.detdescr.TRT_setOn()

# --- switch off DCS
#DetFlags.dcs.pixel_setOff()
#DetFlags.dcs.SCT_setOff()
#DetFlags.dcs.TRT_setOff()

# --- printout
DetFlags.Print()

# --- output level
#OutputLevel          = DEBUG

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

## 2011-02-11: Sasa's new dead straw list:
#from IOVDbSvc.CondDB import conddb
#conddb.addOverride('/TRT/Cond/Status','TrtStrawStatus-Preliminary-167607-00-00')
#conddb.OutputLevel = INFO

from AthenaCommon.GlobalFlags import globalflags

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
#InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')
InDetFlags.doTruth = False

# --- uncomment to change the default of one of the following options:
InDetFlags.doNewTracking          = True
#InDetFlags.doLowPt                = True
#InDetFlags.doBeamGas              = True
#InDetFlags.doBeamHalo             = True
#InDetFlags.doxKalman              = True
#InDetFlags.doiPatRec              = True
#InDetFlags.doBackTracking         = False
#InDetFlags.doSingleSpBackTracking = True
InDetFlags.doTRTStandalone        = False
InDetFlags.doLowBetaFinder        = False

# --- Turn off track slimming
InDetFlags.doSlimming = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
InDetFlags.doTrackSegmentsPixel = False
InDetFlags.doTrackSegmentsSCT   = False
InDetFlags.doTrackSegmentsTRT   = False

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- activate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon        = True
# --- activate creation of standard plots
#InDetFlags.doStandardPlots  = True
# --- active storegate delection
#InDetFlags.doSGDeletion  = True

# --- produce various ntuples (all in one root file)
InDetFlags.doTrkNtuple      = False
#InDetFlags.doPixelTrkNtuple = True
#InDetFlags.doSctTrkNtuple   = True
#InDetFlags.doTrtTrkNtuple   = True
#InDetFlags.doSctClusterNtuple   = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True

#InDetFlags.doMinBias   = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

#=========================================================
# Ryan addded:
InDetFlags.preProcessing = True
InDetFlags.postProcessing = True
InDetFlags.doSiSPSeededTrackFinder = False
InDetFlags.doVertexFinding = False
InDetFlags.doParticleCreation = False
InDetFlags.doV0Finder = False
InDetFlags.doConversions = False
InDetFlags.doStatistics = False
InDetFlags.doSpacePointFormation = True
InDetFlags.doPRDFormation = True


from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.TRT_BuildStrawLayers = True
#=========================================================




# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")

#from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
#ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
#ToolSvc.TrigDecisionTool.OutputLevel=ERROR
#ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR
#ToolSvc.TrigDecisionTool.PublicChainGroups = {"L1Jets":"L1_J.*" }

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator = AtlasExtrapolator()
ToolSvc += theAtlasExtrapolator

from TRT_TrackHoleSearch.TRT_TrackHoleSearchConf import TRTTrackHoleSearchTool
theTRTTrackHoleSearchTool = TRTTrackHoleSearchTool(
        OutputLevel = WARNING,
        extrapolator = theAtlasExtrapolator,
        conditions_svc = InDetTRTConditionsSummaryService, # defined in InDetRec_all.py
        use_conditions_svc = True,
        do_dump_bad_straw_log = False,
        begin_at_first_trt_hit = False, # if not, extrapolate from last Si hit
        end_at_last_trt_hit = False, # if not, continue hole search to the edge of the TRT
        max_trailing_holes = 1, # only used if end_at_last_trt_hit=False
        locR_cut = -1, # 1.4*mm  # negative means no cut
        locR_sigma_cut = -1)

ToolSvc += theTRTTrackHoleSearchTool

from TRT_TrackHoleSearch.TRT_TrackHoleSearchConf import TRTStrawEfficiency
theTRTStrawEfficiency = TRTStrawEfficiency(
        OutputLevel = INFO,
        trt_hole_finder = theTRTTrackHoleSearchTool,
        max_abs_d0 = 10*mm,
        max_abs_z0 = 300*mm,
        min_pT = 2.0*GeV,
        min_pixel_hits = 1,
        min_sct_hits = 2,
        min_trt_hits = 10,
        track_collection = 'CombinedInDetTracks',
        tree_name = 'trt_eff',
        stream_name = 'TRTEffStream',
#        required_trigger = 'L1_J55',
        )
if sample == 'mc':
    theTRTStrawEfficiency.event_info_key = 'MyEvent'
topSequence += theTRTStrawEfficiency


# --- THist Service
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["TRTEffStream DATAFILE='%s' OPT='RECREATE'" % output_ntuple]
ServiceMgr.THistSvc.OutputLevel = WARNING


#------------------------------------------------------------------------------
# Message Service
#------------------------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = WARNING
#ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
ServiceMgr += AthenaEventLoopMgr()
ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 10

print topSequence
