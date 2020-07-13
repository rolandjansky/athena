useNewAlgs = True # Use new AthenaMT friendly DQ algorithms instead of DQ tools

if useNewAlgs:
  include("SCT_Monitoring/SCTErrMonAlg_jobOptions.py")
  include("SCT_Monitoring/SCTHitEffMonAlg_jobOptions.py")
  include("SCT_Monitoring/SCTLorentzMonAlg_jobOptions.py")
  include("SCT_Monitoring/SCTTracksMonAlg_jobOptions.py")
  include("SCT_Monitoring/SCTHitsNoiseMonAlg_jobOptions.py")

else:
  from AthenaMonitoring.DQMonFlags import DQMonFlags
  tracksName = (InDetKeys.SCTTracks() if InDetFlags.doTrackSegmentsSCT() else InDetKeys.UnslimmedTracks())

  doTrigger = False
  TrigDecisionTool_InDetSCTHitsTool = ""
  if globalflags.DataSource == "data":
    from RecExConfig.RecFlags import rec
    if rec.doTrigger():
      doTrigger = True
      TrigDecisionTool_InDetSCTHitsTool = DQMonFlags.nameTrigDecTool()
  from SCT_Monitoring.SCT_MonitoringConf import SCTHitsNoiseMonTool
  InDetSCTHitsTool = SCTHitsNoiseMonTool ( name = "InDetSCTHitsNoiseMonTool",
                                           OutputLevel = 4,
                                           doTrigger = doTrigger,
                                           TrigDecisionTool = TrigDecisionTool_InDetSCTHitsTool,
                                           tracksName = tracksName )

  if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetSCTHitsTool)

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

  from SCT_Monitoring.SCT_MonitoringConf import SCTTracksMonTool
  InDetSCTTracksMonTool = SCTTracksMonTool ( name             = "InDetSCTTracksMonTool",
                                             OutputLevel      = 4,
                                             doTrigger        = doTrigger,
                                             tracksName       = tracksName )

  if jobproperties.Beam.beamType()=='collisions':
    from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
    InDetSCTTracksMonTool.FilterTools += [GetFilledBunchFilterTool()]

    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetSCTTracksMonTool)

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
  InDetSCTLorentzMonTool.AssociationTool = InDetPrdAssociationTool
  InDetSCTLorentzMonTool.RejectSharedHits = False

  if jobproperties.Beam.beamType()=='collisions':
    from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
    InDetSCTLorentzMonTool.FilterTools += [GetFilledBunchFilterTool()]
  
  if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetSCTLorentzMonTool)

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
                                           ROTCreator              = SCT_ClusterOnTrackTool,
                                           BunchCrossingTool       = SCT_BunchCrossingTool,
                                           TrackName               = tracksName)

  if jobproperties.Beam.beamType()=='collisions':
    from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
    InDetSCTHitEffMonTool.FilterTools += [GetFilledBunchFilterTool()]
  
  if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetSCTHitEffMonTool)

  from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

  InDetSCTMonMan = AthenaMonManager("InDetSCTMonManager",
                                    FileKey             = DQMonFlags.monManFileKey(),
                                    ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                    ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),
                                    DataType            = DQMonFlags.monManDataType(),
                                    Environment         = DQMonFlags.monManEnvironment(),
                                    Run                 = DQMonFlags.monManRun(),
                                    LumiBlock           = DQMonFlags.monManLumiBlock(),
                                    AthenaMonTools      = [ InDetSCTErrMonTool,
                                                            InDetSCTHitEffMonTool,
                                                            InDetSCTLorentzMonTool,
                                                            InDetSCTTracksMonTool,
                                                            InDetSCTHitsTool ] )

  topSequence += InDetSCTMonMan
  if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetSCTMonMan)
