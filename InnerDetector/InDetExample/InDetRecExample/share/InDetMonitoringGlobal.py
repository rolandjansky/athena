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
  
  if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
    InDetGlobalSynchMonTool.FilterTools.append(monFilledBunchFilterTool)
  
  ToolSvc += InDetGlobalSynchMonTool
  if (InDetFlags.doPrintConfigurables()):
      print InDetGlobalSynchMonTool
  
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

  InDetTrackSelectionToolGlobalMon_LoosePrimary = InDet__InDetTrackSelectionTool(name = "InDetTrackSelectionToolGlobalMon_LoosePrimary",
                                                                                 UseTrkTrackTools = True,
                                                                                 CutLevel = "LoosePrimary",
                                                                                 TrackSummaryTool    = InDetTrackSummaryTool,
                                                                                 Extrapolator        = InDetExtrapolator)

  InDetTrackSelectionToolGlobalMon_TightPrimary = InDet__InDetTrackSelectionTool(name = "InDetTrackSelectionToolGlobalMon_TightPrimary",
                                                                                 UseTrkTrackTools = True,
                                                                                 CutLevel = "TightPrimary",
                                                                                 TrackSummaryTool    = InDetTrackSummaryTool,
                                                                                 Extrapolator        = InDetExtrapolator)

  ToolSvc += InDetTrackSelectionToolGlobalMon_LoosePrimary
  ToolSvc += InDetTrackSelectionToolGlobalMon_TightPrimary
  
  InDetGlobalTrackMonTool.LoosePrimary_SelTool = InDetTrackSelectionToolGlobalMon_LoosePrimary
  InDetGlobalTrackMonTool.Tight_SelTool = InDetTrackSelectionToolGlobalMon_TightPrimary

  if DQMonFlags.monManDataType == 'heavyioncollisions' or InDetFlags.doHeavyIon() == True:
    InDetGlobalTrackMonTool.trackMax      = 10000
    InDetGlobalTrackMonTool.trackBin      = 1000
    
  if jobproperties.Beam.beamType()=='cosmics':
    InDetGlobalTrackMonTool.trackMax = 10
    InDetGlobalTrackMonTool.trackBin = 10

  if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
    InDetGlobalTrackMonTool.FilterTools.append(monFilledBunchFilterTool)
      
  ToolSvc += InDetGlobalTrackMonTool
  if (InDetFlags.doPrintConfigurables()):
      print InDetGlobalTrackMonTool
  
  ####################################################
  #                                                  #
  # PIXEL TOOL                                       #
  #                                                  #
  ####################################################
  if DetFlags.haveRIO.pixel_on():
      from InDetGlobalMonitoring.InDetGlobalMonitoringConf import InDetGlobalPixelTool
      InDetGlobalPixelTool=InDetGlobalPixelTool( name          = "InDetGlobalPixelTool")

      if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
        InDetGlobalPixelTool.FilterTools.append(monFilledBunchFilterTool)
      ToolSvc += InDetGlobalPixelTool
      if (InDetFlags.doPrintConfigurables()):
          print InDetGlobalPixelTool
  
  ####################################################
  #                                                  #
  # BCM TOOL                                         #
  #                                                  #
  ####################################################
  from InDetGlobalMonitoring.InDetGlobalMonitoringConf import InDetGlobalBCMTool
  InDetGlobalBCMTool=InDetGlobalBCMTool( name          = "InDetGlobalBCMTool")

  if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
    InDetGlobalBCMTool.FilterTools.append(monFilledBunchFilterTool)
  ToolSvc += InDetGlobalBCMTool
  if (InDetFlags.doPrintConfigurables()):
      print InDetGlobalBCMTool

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
      
      if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
        InDetGlobalBeamSpotMonTool.FilterTools.append(monFilledBunchFilterTool)
      ToolSvc += InDetGlobalBeamSpotMonTool
      if (InDetFlags.doPrintConfigurables()):
          print InDetGlobalBeamSpotMonTool

  ####################################################
  #                                                  #
  # Assign tools to global mon manager               #
  #                                                  #
  ####################################################
  InDetGlobalManager.AthenaMonTools += [ InDetGlobalSynchMonTool,
                                         InDetGlobalTrackMonTool,
                                         InDetGlobalBCMTool]
  if DetFlags.haveRIO.pixel_on():
      InDetGlobalManager.AthenaMonTools += [ InDetGlobalPixelTool ]

  if jobproperties.Beam.beamType() != 'cosmics':
      InDetGlobalManager.AthenaMonTools += [ InDetGlobalBeamSpotMonTool ]

  InDetGlobalManager.PixelTrackName      = InDetKeys.PixelTracks()
  InDetGlobalManager.SCTTrackName        = InDetKeys.SCTTracks()
  InDetGlobalManager.TRTTrackName        = InDetKeys.TRTTracks()
  InDetGlobalManager.CombinedTrackName   = InDetKeys.UnslimmedTracks()

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
  if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
     InDetGlobalPrimaryVertexMonTool.FilterTools.append(monFilledBunchFilterTool)

  ToolSvc += InDetGlobalPrimaryVertexMonTool
  if (InDetFlags.doPrintConfigurables()):
          print InDetGlobalPrimaryVertexMonTool

  InDetGlobalManager.AthenaMonTools += [ InDetGlobalPrimaryVertexMonTool]


  ####################################################
  #                                                  #
  # Vertex Monitoring Tool                           #
  #                                                  #
  ####################################################
if InDetFlags.doMonitoringGlobal():
  from InDetVertexMonitoring.InDetVertexMonitoringConf import InDetVertexMonitoring
  InDetVertexMonitoring = InDetVertexMonitoring(  name = "InDetVertexMonitoring",
                                                VertexContainer = InDetKeys.PrimaryVertices(),
                                                DoControlPlots = False,
                                                histFolder = "InDetGlobal/PrimaryVertexMultiplicity"
                                                )
  
  if not hasattr(ToolSvc, 'monTrigDecTool') and rec.doTrigger == True:
    print "Trigger decision tool not found: including it now"
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    monTrigDecTool = Trig__TrigDecisionTool(name=DQMonFlags.nameTrigDecTool(),
                                            OutputLevel=ERROR,
                                            PublicChainGroups = {"EFRandom": "EF_rd0_filled_NoAlg"}
                                            )
    ToolSvc += monTrigDecTool
  
  if rec.doTrigger == True:
    InDetVertexMonitoring.TrigDecisionTool = monTrigDecTool
    InDetVertexMonitoring.TriggerChain = "EF_rd0_filled_NoAlg"
  
  ToolSvc += InDetVertexMonitoring
  InDetGlobalManager.AthenaMonTools += [ InDetVertexMonitoring]


  ####################################################
  topSequence+=InDetGlobalManager
  if (InDetFlags.doPrintConfigurables()):
      print InDetGlobalManager
