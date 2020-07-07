#--------------------------------------------------------------
#-- Job Options for TRT_Monitoring
#--------------------------------------------------------------
from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags           import globalflags
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.DetFlags import DetFlags
import AthenaCommon.SystemOfUnits as Units

### Set event burst cut (no cut by default but 200 for cosmics)
if jobproperties.Beam.beamType()=='cosmics':
  eventBurstCut = 200  
else:
  eventBurstCut = -1

### Set track quality cuts for collisions
if jobproperties.Beam.beamType() == "collisions":
  minTrkPtCut = 2.0 * Units.GeV
  ### at least one silicon detector must be on to require silicon hits
  if DetFlags.pixel_on() or DetFlags.SCT_on():
    minPixHitCut = 1 if DetFlags.pixel_on() else 0
    minSiHitCut = 3
  else: # both silicon detectors are off, we cannot require hits
    minSiHitCut = 0
    minPixHitCut = 0
else: # no track quality cuts for cosmics or single beams
  minTrkPtCut = 0
  minSiHitCut = 0
  minPixHitCut = 0

#-------------------------------------------------------------
# Barrel Monitoring
#-------------------------------------------------------------
from TRT_DriftFunctionTool.TRT_DriftFunctionToolConf import TRT_DriftFunctionTool
InDetTRT_DriftFunctionTool = TRT_DriftFunctionTool(name                = "InDetTRT_DriftFunctionTool",
                                                   IsMC      = (globalflags.DataSource == 'geant4'))

from TRT_Monitoring.TRT_MonitoringConf import TRT_Monitoring_Tool
InDetTRT_Monitoring_Tool = TRT_Monitoring_Tool (name                         = "TRT_Monitoring_Tool",
                                                TRTRawDataObjectName         = InDetKeys.TRT_RDOs(),
                                                NumberOfEvents               = -1,
                                                TRTTracksObjectName          = InDetKeys.UnslimmedTracks(),
                                                DriftFunctionTool            = InDetTRT_DriftFunctionTool,
                                                TrkSummaryTool               = InDetTrackSummaryTool,
                                                Map_Path                     = "../maps/", # obsolete
                                                LE_TimeWindow_MIN            = 0,   # can be 0,1,or 2
                                                LE_TimeWindow_MAX            = 24,  # can be 1,2 .. 24
                                                LL_TimeWindow_MIN            = 0,   # can be 0,1,or 2
                                                LL_TimeWindow_MAX            = 24,  # can be 1,2 .. 24
                                                HL_TimeWindow_MIN            = 0,   # can be 0,1,or 2 # obsolete
                                                HL_TimeWindow_MAX            = 3,   # can be 1,2,or 3 # obsolete
                                                MIN_N_LL_Hits                = 10,  # obsolete
                                                MIN_TOT_Hits                 = 2,   # obsolete
                                                DoTRT_DCS                    = InDetFlags.useTrtDCS(),
                                                NoiseSuppressionLevel_30pc   = False, # obsolete
                                                NoiseSuppressionMap          = False, # obsolete
                                                Debug                        = False,
                                                maxDistToStraw               = 2.0, # obsolete
                                                DistanceToStraw              = 0.4,
                                                LongToTCut                   = 9.375,
                                                is_TRT_only_tracks           = True, # obsolete
                                                is_zero_mag_field            = True, # obsolete
                                                #Histogram Switches
                                                doAside                      = True, # obsolete
                                                doCside                      = True, # obsolete
                                                doStraws                     = True,
                                                doChips                      = True,
                                                doShift                      = True,
                                                doDiagnostic                 = True, # obsolete
                                                EventBurstCut                = eventBurstCut, # default = 200
                                                DoRDOsMon                    = True,
                                                DoGeoMon                     = False,
                                                DoTracksMon                  = True,
                                                doExpert                     = athenaCommonFlags.isOnline(),
                                                IsCosmics                    = InDetFlags.doCosmics(),
                                                InDetTRT_DAQ_ConditionsSvc   = InDetTRT_DAQ_ConditionsSvc,
                                                doEfficiency                 = False, # ME: turn off TRT hole search, it fails Tier-0
                                                every_xth_track              = 25,
                                                whatdatatype                 = DQMonFlags.monManDataType(),
                                                MinTRTHitCut                 = 10,
                                                max_abs_d0                   = 10  * Units.mm,
                                                max_abs_z0                   = 300 * Units.mm,
                                                max_abs_eta                  = 2.5,
                                                MinTrackP                    = 0.0 * Units.GeV,
                                                min_pT                       = minTrkPtCut, # default = 0.5 GeV
                                                min_si_hits                  = minSiHitCut, # default = 3
                                                min_pixel_hits               = minPixHitCut, # default = 1
                                                min_sct_hits                 = 0,
                                                min_trt_hits                 = 10,
                                                TrigDecisionObjectName       = "xTrigDecision" if DQMonFlags.useTrigger else ""
                                                )

if globalflags.DataSource == 'geant4':
  #No ByteStream error in MC case
  InDetTRT_Monitoring_Tool.ByteStreamErrors=""

if jobproperties.Beam.beamType()=='collisions':
  from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
  InDetTRT_Monitoring_Tool.FilterTools += [GetFilledBunchFilterTool()]
  
#ToolSvc += InDetTRT_Monitoring_Tool
if (InDetFlags.doPrintConfigurables()):
  printfunc (InDetTRT_Monitoring_Tool)


#-------------------------------------------------------------
# add an AthenaMonManager algorithm to the list of algorithms to be ran
#-------------------------------------------------------------
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

InDetTRTMonMan = AthenaMonManager( "TRTMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),             #"GLOBAL" #"stat"
                                   ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(), #True
                                   DataType            = DQMonFlags.monManDataType(),            #"cosmics"
                                   Environment         = DQMonFlags.monManEnvironment(),         #"user"
                                   ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),    #True
                                   Run                 = DQMonFlags.monManRun(),                 #1
                                   LumiBlock           = DQMonFlags.monManLumiBlock(),           #1
                                   AthenaMonTools      = [ InDetTRT_Monitoring_Tool  ])

topSequence += InDetTRTMonMan
if (InDetFlags.doPrintConfigurables()):
  printfunc (InDetTRTMonMan)



