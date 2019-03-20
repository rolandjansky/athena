from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

########################################################################
# Conditions access
# These lines were previously in SCT_Monitoring_ConditionsAccess.py
########################################################################

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
  print InDetSCTHitsTool

from SCT_Monitoring.SCT_MonitoringConf import SCTTracksMonTool
InDetSCTTracksMonTool = SCTTracksMonTool ( name             = "InDetSCTTracksMonTool",
                                           OutputLevel      = 4,
                                           doTrigger        = doTriggger,
                                           tracksName       = tracksName )

if jobproperties.Beam.beamType()=='collisions':
  from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
  InDetSCTTracksMonTool.FilterTools += [GetFilledBunchFilterTool()]

if (InDetFlags.doPrintConfigurables()):
  print InDetSCTTracksMonTool

from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
sct_DCSConditionsToolSetup.setup()

from SCT_Monitoring.SCT_MonitoringConf import SCTErrMonTool
InDetSCTErrMonTool = SCTErrMonTool ( name             = "InDetSCTErrMonTool",
                                     OutputLevel      = 4,
                                     histoPathBase    = "/stat",
                                     SCT_DCSConditionsTool = sct_DCSConditionsToolSetup.getTool())

if jobproperties.Beam.beamType()=='collisions':
  from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
  InDetSCTErrMonTool.FilterTools += [GetFilledBunchFilterTool()]
  
if (InDetFlags.doPrintConfigurables()):
  print InDetSCTErrMonTool
               


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
  print InDetSCTHitEffMonTool

from SCT_Monitoring.SCT_MonitoringConf import SCTLorentzMonTool
from BTagging.BTaggingConfiguration_CommonTools import toolAtlasExtrapolator
atlasExtrapolator = toolAtlasExtrapolator('AtlasExtrapolator')
options = {}
options.setdefault('Extrapolator', atlasExtrapolator)
from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
trackToVertex = Reco__TrackToVertex(**options)
InDetSCTLorentzMonTool = SCTLorentzMonTool ( name             = "InDetSCTLorentzMonTool",
                                             TrackToVertexTool = trackToVertex,
                                             OutputLevel      = 4,
                                             tracksName       = tracksName )

if jobproperties.Beam.beamType()=='collisions':
  from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
  InDetSCTLorentzMonTool.FilterTools += [GetFilledBunchFilterTool()]
  
if (InDetFlags.doPrintConfigurables()):
  print InDetSCTLorentzMonTool


from SCT_Monitoring.SCT_MonitoringConf import SCTRatioNoiseMonTool
InDetSCTRatioNoiseMonTool = SCTRatioNoiseMonTool ( name        = "InDetSCTRatioNoiseMonTool",
                                                   OutputLevel = 4,
                                                   doTrigger   = doTriggger )

if (InDetFlags.doPrintConfigurables()):
  print InDetSCTRatioNoiseMonTool
           

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

InDetSCTMonMan = AthenaMonManager("InDetSCTMonManager",
                                  FileKey             = DQMonFlags.monManFileKey(),
                                  ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                  ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),
                                  DataType            = DQMonFlags.monManDataType(),
                                  Environment         = DQMonFlags.monManEnvironment(),
                                  Run                 = DQMonFlags.monManRun(),
                                  LumiBlock           = DQMonFlags.monManLumiBlock(),
                                  AthenaMonTools      = [ InDetSCTTracksMonTool,
                                                          InDetSCTRatioNoiseMonTool,
                                                          InDetSCTLorentzMonTool,
                                                          InDetSCTHitEffMonTool,
                                                          InDetSCTHitsTool,
                                                          InDetSCTErrMonTool ] )

topSequence += InDetSCTMonMan
if (InDetFlags.doPrintConfigurables()):
  print InDetSCTMonMan
