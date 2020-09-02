if (not 'doIdGlobalErrorMon' in dir()):
  doIdGlobalErrorMon = False

####################################################
#                                                  #
# InDetGlobalManager top algorithm                 #
#                                                  #
####################################################
if InDetFlags.doMonitoringGlobal() or InDetFlags.doMonitoringPrimaryVertexingEnhanced():

  from AthenaMonitoring.DQMonFlags import DQMonFlags
  from InDetGlobalMonitoring.InDetGlobalMonitoringConf import InDetGlobalManager
  InDetGlobalManager=InDetGlobalManager( name                = "InDetGlobalManager",
                                        TRT_DriftCircleName = InDetKeys.TRT_DriftCircles(),
                                        FileKey             = DQMonFlags.monManFileKey(),
                                        ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                        ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),
                                        DataType            = DQMonFlags.monManDataType(),
                                        Environment         = DQMonFlags.monManEnvironment(),
                                        Run                 = DQMonFlags.monManRun(),
                                        LumiBlock           = DQMonFlags.monManLumiBlock(),
                                        doTopBottom         =  True,
                                        doPixel             =  DetFlags.haveRDO.pixel_on(),
                                        doSCT               =  DetFlags.haveRDO.SCT_on(),
                                        doTRT               =  DetFlags.haveRDO.TRT_on(),
                                        AthenaMonTools      = []) # these are set at the end

if InDetFlags.doMonitoringGlobal():
  #######################################################
  #                                                     #
  # LVL1 & BC ID monitoring - only for real data        #
  #                                                     #
  #######################################################
  from InDetGlobalMonitoring.InDetGlobalMonitoringConf import InDetGlobalSynchMonTool
  InDetGlobalSynchMonTool = InDetGlobalSynchMonTool( name          = "InDetGlobalSynchMonTool",
                                                    histoPathBase = "/GLOBAL",
                                                    checkRate     = 2000)
  
  if jobproperties.Beam.beamType()=='collisions':
    from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
    InDetGlobalSynchMonTool.FilterTools += [GetFilledBunchFilterTool()]
  
  #ToolSvc += InDetGlobalSynchMonTool
  if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetGlobalSynchMonTool)

  #######################################################
  #                                                     #
  # Error monitoring, only online                       #
  #                                                     #
  #######################################################
  if doIdGlobalErrorMon:
      from InDetGlobalMonitoring.InDetGlobalMonitoringConf import InDetGlobalErrorMonTool
      InDetGlobalErrorMonTool=InDetGlobalErrorMonTool( name = "InDetGlobalErrorMonTool" )
  
      #ToolSvc += InDetGlobalErrorMonTool
      if (InDetFlags.doPrintConfigurables()):
          printfunc (InDetGlobalErrorMonTool)

  
  ##################################################
  #                                                #
  # TRACK MONITORING                               #
  #                                                #
  ##################################################
      
  from InDetGlobalMonitoring.InDetGlobalMonitoringConf import InDetGlobalTrackMonTool
  InDetGlobalTrackMonTool=InDetGlobalTrackMonTool( name          = "InDetGlobalTrackMonTool",
                                                   histoPathBase = "/GLOBAL",
                                                   DoIBL         = InDetFlags.doIBL(),
                                                   trackMax      = 75)

  TrackCollection = InDetKeys.UnslimmedTracks()

  from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool 
  InDetTrackSelectionToolGlobalMon_Baseline = InDet__InDetTrackSelectionTool(name = "InDetTrackSelectionToolGlobalMon_Baseline",
                                                                                 UseTrkTrackTools = True,
                                                                                 CutLevel = "TightPrimary",
                                                                                 maxNPixelHoles = 1,
                                                                                 minPt = 5000,
                                                                                 TrackSummaryTool    = InDetTrackSummaryTool,
                                                                                 Extrapolator        = InDetExtrapolator)

  InDetTrackSelectionToolGlobalMon_TightPrimary = InDet__InDetTrackSelectionTool(name = "InDetTrackSelectionToolGlobalMon_TightPrimary",
                                                                                 UseTrkTrackTools = True,
                                                                                 CutLevel = "TightPrimary",
                                                                                 minPt = 5000,
                                                                                 TrackSummaryTool    = InDetTrackSummaryTool,
                                                                                 Extrapolator        = InDetExtrapolator)

  #ToolSvc += InDetTrackSelectionToolGlobalMon_Baseline
  #ToolSvc += InDetTrackSelectionToolGlobalMon_TightPrimary
  
  InDetGlobalTrackMonTool.Baseline_SelTool = InDetTrackSelectionToolGlobalMon_Baseline
  InDetGlobalTrackMonTool.Tight_SelTool = InDetTrackSelectionToolGlobalMon_TightPrimary

  if DQMonFlags.monManDataType == 'heavyioncollisions' or InDetFlags.doHeavyIon() == True:
    InDetGlobalTrackMonTool.trackMax      = 10000
    InDetGlobalTrackMonTool.trackBin      = 1000
    
  if jobproperties.Beam.beamType()=='cosmics':
    InDetGlobalTrackMonTool.trackMax = 10
    InDetGlobalTrackMonTool.trackBin = 10

  if jobproperties.Beam.beamType()=='collisions':
    from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
    InDetGlobalTrackMonTool.FilterTools += [GetFilledBunchFilterTool()]
      
  #ToolSvc += InDetGlobalTrackMonTool
  if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetGlobalTrackMonTool)
  
  ####################################################
  #                                                  #
  # PIXEL TOOL                                       #
  #                                                  #
  ####################################################
  if DetFlags.haveRIO.pixel_on():
      from InDetGlobalMonitoring.InDetGlobalMonitoringConf import InDetGlobalPixelTool
      InDetGlobalPixelTool=InDetGlobalPixelTool( name          = "InDetGlobalPixelTool")

      if jobproperties.Beam.beamType()=='collisions':
        from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
        InDetGlobalPixelTool.FilterTools += [GetFilledBunchFilterTool()]
      #ToolSvc += InDetGlobalPixelTool
      if (InDetFlags.doPrintConfigurables()):
          printfunc (InDetGlobalPixelTool)
  
  ####################################################
  #                                                  #
  # BCM TOOL                                         #
  #                                                  #
  ####################################################
  # from InDetGlobalMonitoring.InDetGlobalMonitoringConf import InDetGlobalBCMTool
  # InDetGlobalBCMTool=InDetGlobalBCMTool( name          = "InDetGlobalBCMTool")

  # if jobproperties.Beam.beamType()=='collisions':
  #   from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
  #   InDetGlobalBCMTool.FilterTools += [GetFilledBunchFilterTool()]
  # ToolSvc += InDetGlobalBCMTool
  # if (InDetFlags.doPrintConfigurables()):
  #     printfunc (InDetGlobalBCMTool)

  ####################################################
  #                                                  #
  # Beam Spot Tool                                   #
  #                                                  #
  ####################################################
  if jobproperties.Beam.beamType() != 'cosmics':
      from InDetGlobalMonitoring.InDetGlobalMonitoringConf import InDetGlobalBeamSpotMonTool
      InDetGlobalBeamSpotMonTool=InDetGlobalBeamSpotMonTool( name = "InDetGlobalBeamSpotMonTool",
                                                             vxContainerName = InDetKeys.xAODVertexContainer(),
                                                             vxContainerWithBeamConstraint = InDetFlags.useBeamConstraint(),
                                                             trackContainerName = InDetKeys.xAODTrackParticleContainer(),
                                                             useBeamspot = InDetFlags.useBeamConstraint()) 
      
      if jobproperties.Beam.beamType()=='collisions':
        from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
        InDetGlobalBeamSpotMonTool.FilterTools += [GetFilledBunchFilterTool()]
      #ToolSvc += InDetGlobalBeamSpotMonTool
      if (InDetFlags.doPrintConfigurables()):
          printfunc (InDetGlobalBeamSpotMonTool)

  ####################################################
  #                                                  #
  # Assign tools to global mon manager               #
  #                                                  #
  ####################################################
  InDetGlobalManager.AthenaMonTools += [ InDetGlobalSynchMonTool,
                                         InDetGlobalTrackMonTool]
  # InDetGlobalBCMTool taken out (AK 2016/12/16)
  if DetFlags.haveRIO.pixel_on():
      InDetGlobalManager.AthenaMonTools += [ InDetGlobalPixelTool ]

  if doIdGlobalErrorMon:
      InDetGlobalManager.AthenaMonTools += [ InDetGlobalErrorMonTool ]

  if jobproperties.Beam.beamType() != 'cosmics':
      InDetGlobalManager.AthenaMonTools += [ InDetGlobalBeamSpotMonTool ]

  InDetGlobalManager.CombinedTrackName   = InDetKeys.UnslimmedTracks()
  InDetGlobalManager.doTRTPhase = (InDetFlags.doTRTPhaseCalculation() and jobproperties.Beam.beamType() == 'cosmics')
  InDetGlobalManager.doTiming = (globalflags.DataSource == 'data')

if InDetFlags.doMonitoringGlobal() or InDetFlags.doMonitoringPrimaryVertexingEnhanced():
  ####################################################
  #                                                  #
  # Primary Vertex Tool                              #
  #                                                  #
  ####################################################
  from InDetGlobalMonitoring.InDetGlobalMonitoringConf import InDetGlobalPrimaryVertexMonTool
  InDetGlobalPrimaryVertexMonTool=InDetGlobalPrimaryVertexMonTool( name          = "InDetGlobalPrimaryVertexMonTool",
                                                                   histFolder="InDetGlobal/PrimaryVertex",
                                                                   vxContainerName = InDetKeys.xAODVertexContainer(),
                                                                   vxContainerNameWithOutBeamConstraint = InDetKeys.PrimaryVerticesWithoutBeamConstraint(),
                                                                   vxContainerNameSplit = InDetKeys.PrimaryVerticesSplitStream(),
                                                                   doEnhancedMonitoring = InDetFlags.doMonitoringPrimaryVertexingEnhanced())
  if jobproperties.Beam.beamType()=='collisions':
    from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
    InDetGlobalPrimaryVertexMonTool.FilterTools += [GetFilledBunchFilterTool()]

  #ToolSvc += InDetGlobalPrimaryVertexMonTool
  if (InDetFlags.doPrintConfigurables()):
          printfunc (InDetGlobalPrimaryVertexMonTool)

  InDetGlobalManager.AthenaMonTools += [ InDetGlobalPrimaryVertexMonTool]


  ####################################################
  #                                                  #
  # Vertex Monitoring Tool                           #
  #                                                  #
  ####################################################
if InDetFlags.doMonitoringGlobal():
  from InDetVertexMonitoring.InDetVertexMonitoringConf import InDetVertexMonitoring
  InDetVertexMonitoring = InDetVertexMonitoring(  name = "InDetVertexMonitoring",
                                                VertexContainer = InDetKeys.xAODVertexContainer(),
                                                DoControlPlots = False,
                                                histFolder = "InDetGlobal/PrimaryVertexMultiplicity"
                                                )
  
  if not hasattr(ToolSvc, 'monTrigDecTool') and rec.doTrigger == True:
    printfunc ("Trigger decision tool not found: including it now")
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    monTrigDecTool = Trig__TrigDecisionTool(name=DQMonFlags.nameTrigDecTool(),
                                            OutputLevel=ERROR,
                                            PublicChainGroups = {"EFRandom": "EF_rd0_filled_NoAlg"}
                                            )
    #ToolSvc += monTrigDecTool
  
  if rec.doTrigger == True:
    InDetVertexMonitoring.TrigDecisionTool = monTrigDecTool
    InDetVertexMonitoring.TriggerChain = "EF_rd0_filled_NoAlg"
  
  #ToolSvc += InDetVertexMonitoring
  InDetGlobalManager.AthenaMonTools += [ InDetVertexMonitoring]


  ####################################################
  topSequence+=InDetGlobalManager
  if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetGlobalManager)
