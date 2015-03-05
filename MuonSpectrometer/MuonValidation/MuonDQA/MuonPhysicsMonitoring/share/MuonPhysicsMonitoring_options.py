#======================================================================
# TOP JobOptions to run on MuonPhysicsMonitoring
#======================================================================
#print '\n****************************************************************************************\n'
#print "   ******** Welcome to the Offline MuonTrackMonitoring package. ******** "  
#print "   ******** Documentation may be found at:******** "  
#print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MuonOfflineDQA ***** "  
#print '\n****************************************************************************************\n'

from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags
from MuonPhysicsMonitoring.MuonPhysicsMonitoringConf import ZmumuPhysMonTool
## #--------------------------#
## #   MuonPhysicsMonitoring  # 
## #--------------------------#
if MuonDQAFlags.doMuonPhysMon:
    from MuonPhysicsMonitoring.MuonPhysicsMonitoringConf import ZmumuPhysMonTool
    ZmumuPhysMonTool = ZmumuPhysMonTool (name = "ZmumuPhysMonTool",
                                         FromESD = True,
                                         MaximalConeDistance = 0.0,
                                         MinimalMuonPt = 6.*GeV,
                                         MuonContainer = "StacoMuonCollection", # "StacoESDMuonCollection",
                                         MuonSpectrometerContainer = "MuonboyTrackParticles",   #"MboyESDMuonContainer",
                                         InnerTrackContainer = "TrackParticleCandidate",
                                         OutputLevel = DEBUG
                                         )					 
    ToolSvc += ZmumuPhysMonTool
    monMan.AthenaMonTools += [ZmumuPhysMonTool]
    ## --------------------------------------------------
    ## MuonInsituPerformance
    #--------------------------------------------------------
    from MuonPhysicsMonitoring.MuonPhysicsMonitoringConf import MuonInsituPerformance
    MuonInsituPerformance = MuonInsituPerformance(name = "MuonInsituPerformance",
                                                  MuonContainer = "StacoMuonCollection", # "StacoESDMuonCollection",
                                                  MuonSpectrometerTrackContainer = "MuonboyTrackParticles", ##"MboyESDMuonContainer",
                                                  InnerTrackContainer = "TrackParticleCandidate",
                                                  OutputLevel = DEBUG
                                                  )					 
    ToolSvc += MuonInsituPerformance
    monMan.AthenaMonTools += [MuonInsituPerformance]         
    #--------------------------#
    # Importing InsituPerformanceTools for Muon and Inner Track
    #--------------------------#
    ##### Importing muonInsituTrack
    from MuonDQAUtils.MuonDQAUtilsConf import Muon__InsituTrackTools
    muonInsituTrackTools = Muon__InsituTrackTools(name = "muonInsituTrackTools")
    ToolSvc+= muonInsituTrackTools
    ###### Importing idInsituTrackTools
    from MuonDQAUtils.MuonDQAUtilsConf import Muon__InsituTrackTools
    idInsituTrackTools = Muon__InsituTrackTools(name = "idInsituTrackTools")
    ToolSvc+=idInsituTrackTools
    
    ###commented out due to https://savannah.cern.ch/bugs/?40935
    ####### Importing InsituDatabaseTools
    #    from InsituDatabaseTools.InsituDatabaseToolsConf import InsituDatabaseTools
    #    msInsituDatabaseTools = InsituDatabaseTools(name = "msInsituDatabaseToolsName")
    #    ToolSvc+=msInsituDatabaseTools 

    #-----------------------------------------------#    
    ## Importing MuonSpectrometerProbeCollectorTool
    #-----------------------------------------------#
    from MuonDQAUtils.MuonDQAUtilsConf import Muon__MuonSpectrometerProbeCollectorTool
    muonProbeCollectorTool = Muon__MuonSpectrometerProbeCollectorTool(name = "muonProbeCollectorTool",
                                                                      CombinedMuonTracksContainerName = "StacoMuonCollection" )
    ##### Adding InsituTrackTools to MuonSpectrometerProbeCollectorTool
    muonProbeCollectorTool.InsituPerformanceTools = muonInsituTrackTools
    ToolSvc+= muonProbeCollectorTool
    ##### Importing InnerDetProbeCollectorTool
    from MuonDQAUtils.MuonDQAUtilsConf import Muon__InnerDetProbeCollectorTool
    idProbeCollectorTool = Muon__InnerDetProbeCollectorTool(name ="idProbeCollectorTool",
                                                            CombinedMuonTracksContainerName = "StacoMuonCollection" )
    ##### Adding InsituTrackTools to InnerDetProbeCollectorTool
    idProbeCollectorTool.InsituPerformanceTools = idInsituTrackTools
    ##    idProbeCollectorTool.InsituPerformanceTools = muonInsituTrackTools
    ToolSvc+=idProbeCollectorTool
    ###MuonInsituMonitoring##################
    from MuonPhysicsMonitoring.MuonPhysicsMonitoringConf import MuonInsituMonitoring
    MuonInsituMonitoring = MuonInsituMonitoring( name = "MuonInsituMonitoring",
                                                 MuonContainerLocation = [ "StacoMuonCollection", "MuidMuonCollection" ],
                                                 MuonAuthor = ["STACO","MuidComb"],
                                                 doMuonspectrometerAnalysis = [1,1],
                                                 doInnerDetectorAnalysis = [1,0],
                                                 doCombinedAnalysis = [1,1],
                                                 OutputLevel = DEBUG)
    ToolSvc += MuonInsituMonitoring
    MuonInsituMonitoring.MSProbeCollectorTool = muonProbeCollectorTool
    MuonInsituMonitoring.IDProbeCollectorTool = idProbeCollectorTool
    MuonInsituMonitoring.InsituPerformanceTool = muonInsituTrackTools
    monMan.AthenaMonTools += [MuonInsituMonitoring]
    print monMan;
 
