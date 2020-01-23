from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

########################################################################
# Conditions access
# These lines were previously in SCT_Monitoring_ConditionsAccess.py
########################################################################

useNewAlgs = True # Use new AthenaMT friendly DQ algorithms instead of DQ tools

tracksName = (InDetKeys.SCTTracks() if InDetFlags.doTrackSegmentsSCT() else InDetKeys.UnslimmedTracks())

doTriggger = False
TrigDecisionTool_InDetSCTHitsTool = ""
if globalflags.DataSource == "data":
  from RecExConfig.RecFlags import rec
  if rec.doTrigger():
    doTriggger = True
    TrigDecisionTool_InDetSCTHitsTool = DQMonFlags.nameTrigDecTool()
from SCT_Monitoring.SCT_MonitoringConf import SCTHitsNoiseMonTool
InDetSCTHitsTool = SCTHitsNoiseMonTool ( name = "InDetSCTHitsNoiseMonTool",
                                         OutputLevel = 4,
                                         doTrigger = doTriggger,
                                         TrigDecisionTool = TrigDecisionTool_InDetSCTHitsTool,
                                         tracksName = tracksName )

if (InDetFlags.doPrintConfigurables()):
  printfunc (InDetSCTHitsTool)

from SCT_Monitoring.SCT_MonitoringConf import SCTTracksMonTool
InDetSCTTracksMonTool = SCTTracksMonTool ( name             = "InDetSCTTracksMonTool",
                                           OutputLevel      = 4,
                                           doTrigger        = doTriggger,
                                           tracksName       = tracksName )

if jobproperties.Beam.beamType()=='collisions':
  from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
  InDetSCTTracksMonTool.FilterTools += [GetFilledBunchFilterTool()]

if (InDetFlags.doPrintConfigurables()):
  printfunc (InDetSCTTracksMonTool)

from SCT_Monitoring.SCT_MonitoringConf import SCTErrMonTool
InDetSCTErrMonTool = SCTErrMonTool ( name             = "InDetSCTErrMonTool",
                                     OutputLevel      = 4,
                                     histoPathBase    = "/stat",
                                     UseDCS           = InDetFlags.useDCS() )
if InDetFlags.useDCS():
  from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
  sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
  sct_DCSConditionsToolSetup.setup()
  InDetSCTErrMonTool.SCT_DCSConditionsTool = sct_DCSConditionsToolSetup.getTool()
else:
  InDetSCTErrMonTool.SCT_DCSConditionsTool = None

if jobproperties.Beam.beamType()=='collisions':
  from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
  InDetSCTErrMonTool.FilterTools += [GetFilledBunchFilterTool()]
  
if (InDetFlags.doPrintConfigurables()):
  printfunc (InDetSCTErrMonTool)
               


if not hasattr(ServiceMgr,"TrigConfigSvc"):
  from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
  trigconfigsvc= SetupTrigConfigSvc()
  trigconfigsvc.SetStates('ds')
  trigconfigsvc.InitialiseSvc()
  pass

from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
SCT_BunchCrossingTool = BunchCrossingTool()


from SCT_Monitoring.SCT_MonitoringConf import SCTHitEffMonTool
InDetSCTHitEffMonTool = SCTHitEffMonTool(name = "InDetSCTHitEffMonTool",
                                         OutputLevel             = 4,
                                         IsCosmic                = InDetFlags.doCosmics(),
                                         HoleSearch              = InDetHoleSearchTool,
                                         IsSim                   = (globalflags.DataSource != "data"),
                                         ROTCreator              = SCT_ClusterOnTrackTool,
                                         BunchCrossingTool       = SCT_BunchCrossingTool,
                                         TrackName               = tracksName)

if jobproperties.Beam.beamType()=='collisions':
  from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
  InDetSCTHitEffMonTool.FilterTools += [GetFilledBunchFilterTool()]
  
if (InDetFlags.doPrintConfigurables()):
  printfunc (InDetSCTHitEffMonTool)

if not useNewAlgs:
  from SCT_Monitoring.SCT_MonitoringConf import SCTLorentzMonTool
  from BTagging.BTaggingConfiguration_CommonTools import toolAtlasExtrapolator
  atlasExtrapolator = toolAtlasExtrapolator('AtlasExtrapolator')
  options = {}
  options.setdefault('Extrapolator', atlasExtrapolator)
  from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
  trackToVertex = Reco__TrackToVertex(**options)
  InDetSCTLorentzMonTool = SCTLorentzMonTool ( name             = "InDetSCTLorentzMonTool",
                                               OutputLevel      = 4,
                                               tracksName       = tracksName )

  if jobproperties.Beam.beamType()=='collisions':
    from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
    InDetSCTLorentzMonTool.FilterTools += [GetFilledBunchFilterTool()]
  
  if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetSCTLorentzMonTool)


from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

InDetSCTMonMan = AthenaMonManager("InDetSCTMonManager",
                                  FileKey             = DQMonFlags.monManFileKey(),
                                  ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                  ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),
                                  DataType            = DQMonFlags.monManDataType(),
                                  Environment         = DQMonFlags.monManEnvironment(),
                                  Run                 = DQMonFlags.monManRun(),
                                  LumiBlock           = DQMonFlags.monManLumiBlock(),
                                  AthenaMonTools      = [ InDetSCTHitEffMonTool,
                                                          InDetSCTHitsTool,
                                                          InDetSCTErrMonTool ] )

if useNewAlgs:
  include("SCT_Monitoring/SCTLorentzMonAlg_jobOptions.py")
  include("SCT_Monitoring/SCTTracksMonAlg_jobOptions.py")
else:
  InDetSCTMonMan.AthenaMonTools += [ InDetSCTLorentzMonTool ]
  InDetSCTMonMan.AthenaMonTools += [ InDetSCTTracksMonTool ]

topSequence += InDetSCTMonMan
if (InDetFlags.doPrintConfigurables()):
  printfunc (InDetSCTMonMan)
