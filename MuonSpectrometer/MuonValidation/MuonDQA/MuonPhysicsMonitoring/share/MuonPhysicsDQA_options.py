#======================================================================
# TOP JobOptions to run on MuonPhysicsMonitoring
#======================================================================
## #--------------------------#
## #   MuonPhysicsMonitoring  # 
## #--------------------------#
#print '\n****************************************************************************************\n'
#print "   ******** Welcome to the Offline MuonPhysicsMonitoring package. ******** "  
#print "   ******** Documentation may be found at:******** "  
#print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MuonOfflineDQA ***** "  
#print '\n****************************************************************************************\n'

import MuonCnvExample.MuonCablingConfig

from MuonRecExample import MuonRecTools

# TrigDecisionTool configuration
DoTrigger = DQMonFlags.useTrigger() and hasattr(ToolSvc, DQMonFlags.nameTrigDecTool())
myTrigDecisionTool = ""
if DoTrigger:
    myTrigDecisionTool = getattr(ToolSvc, DQMonFlags.nameTrigDecTool())


#===============================================================================	
# Check For Container Availibility:
#===============================================================================					
from RecExConfig.InputFilePeeker import inputFileSummary
def isContainerInInputFile(container_type, container_name):
    if container_type in inputFileSummary['eventdata_itemsDic']:
        if container_name in inputFileSummary['eventdata_itemsDic'][container_type]:
            return True
    return False

#===============================================================================	
# Muon Collections
# Flags are set based upon container availability 
staco = 0
muid  = 0
muons = 0

if (rec.readESD() and isContainerInInputFile("Analysis::MuonContainer", "StacoMuonCollection")):
    staco = 1

if (rec.readESD() and isContainerInInputFile("Analysis::MuonContainer", "MuidMuonCollection")):
    muid = 1
 	
myMuonContainer = ["StacoMuonCollection","MuidMuonCollection"]
  
#
# Configrable version of loading the InDetTrackSummaryHelperTool
#

muJpsimumuMonMan = AthenaMonManager(name="MuonJpsimumuMonManager",
                                    FileKey             = DQMonFlags.monManFileKey(),
                                    Environment         = DQMonFlags.monManEnvironment(),
                                    OutputLevel         = muonOutputLevel)

muJpsiInSituMonMan = AthenaMonManager(name="MuonJpsiInSituMonManager",
                                      FileKey             = DQMonFlags.monManFileKey(),
                                      Environment         = DQMonFlags.monManEnvironment(),
                                      OutputLevel         = muonOutputLevel)

muZmumuPhysMonMan = AthenaMonManager(name="MuonZmumuPhysMonManager",
                                     FileKey             = DQMonFlags.monManFileKey(),
                                     Environment         = DQMonFlags.monManEnvironment(),
                                     OutputLevel         = muonOutputLevel)

muTrkInSituMonMan = AthenaMonManager(name="MuTrkInSituMonManager",
                                     FileKey             = DQMonFlags.monManFileKey(),
                                     Environment         = DQMonFlags.monManEnvironment(),
                                     OutputLevel         = muonOutputLevel)
muZmuTrkPhysMonMan = AthenaMonManager(name="MuonZmuTrkPhysMonManager",
                                     FileKey             = DQMonFlags.monManFileKey(),
                                     Environment         = DQMonFlags.monManEnvironment(),
                                     OutputLevel         = muonOutputLevel)

muInSituMonMan = AthenaMonManager(name="MuonInSituMonManager",
                                  FileKey             = DQMonFlags.monManFileKey(),
                                  Environment         = DQMonFlags.monManEnvironment(),
                                  OutputLevel         = muonOutputLevel)
#------------------------------------------
myJetContainer = "AntiKt4TopoEMJets"
myMuonContainer = [ "StacoMuonCollection", "MuidMuonCollection" ] 
#############
useAODContainers = False;  # to set the right containers
#############
if useAODContainers:
    myJetContainer= "AntiKt4TopoJets"
#------------------------------------------
    
if MuonDQADetFlags.doJpsimumuMon:
    include ("MuonPhysicsMonitoring/configureServices.py")

    ############################################## Jpsi candidate Selection
    DoTruth = False
    #TruthParticleContainer = "SpclMC"    
    OutputMessageLevel = WARNING
    DimuonMassWindow = 500.
    UpperMassCut = 4000
    LowerMassCut = 2000
    JpsiMass = 3097
    JpsiMassBinning = 80
    ProperTimeCut = 0.
    MuonHigherPtCut = 500.
    MuonLowerPtCut = 500.
    MuonDeltaRCut = 0.
    DoVertexing = True
    VertexChi2Cut = 200.
    BinsEta = [ -2.5, -1.6, -0.8, 0.8, 1.6, 2.51 ]
    BinsPhi = [ -3.15,3.15 ]
    BinsPt  = [ 0., 1000., 2000., 4000., 6000., 10000. ]
    #BinsEta = [ -2.5, -2., -1.6, -1.3, -1.05, -0.9, -0.75, -0.6, -0.45, -0.25, 0., 0.25, 0.45, 0.6, 0.75, 0.9, 1.3, 1.6, 2., 2.51 ]
    #BinsPhi = [ -3.14159, -2.79252, -2.44346, -2.09439, -1.74533, -1.39626, -1.0472, -0.698131, -0.349066, 0, 0.349066, 0.698131, 1.0472, 1.39626, 1.74533, 2.09439, 2.44346, 2.79252, 3.14159 ]
   # BinsEta = [ -2.5, -2.4, -2.3, -2.2, -2.1, -2., -1.8, -1.6, -1.4, -1.2, -1.1, -1.,-0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0., 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1., 1.1, 1.2, 1.4, 1.6, 1.8, 2., 2.1, 2.2, 2.3, 2.4, 2.51 ]
    #BinsPhi = [ -3.14159, -3.03687, -2.93215, -2.82743, -2.72271, -2.61799, -2.51327, -2.40855, -2.30383, -2.19911, -2.0944, -1.98968, -1.88496, -1.78024, -1.67552, -1.5708, -1.46608, -1.36136, -1.25664, -1.15192, -1.0472, -0.942478, -0.837758, -0.733038, -0.628319, -0.523599, -0.418879, -0.314159, -0.20944, -0.10472, 8.60423e-16, 0.10472, 0.20944, 0.314159, 0.418879, 0.523599, 0.628319, 0.733038, 0.837758, 0.942478, 1.0472, 1.15192, 1.25664, 1.36136, 1.46608, 1.5708, 1.67552, 1.78024, 1.88496, 1.98968, 2.0944, 2.19911, 2.30383, 2.40855, 2.51327, 2.61799, 2.72271, 2.82743, 2.93215, 3.03687, 3.14159 ]
    #BinsPt  = [ 0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000., 4500, 5000, 6000., 7000, 8000.,9000, 10000., 16000 ]

    #############################################
    ## --------------------------------------------------
    ## JpsimumuPhysMonTool
    #--------------------------------------------------------
    from MuonPhysicsMonitoring.MuonPhysicsMonitoringConf import JpsimumuPhysMonTool
    JpsimumuPhysMonTool_NoTrig = JpsimumuPhysMonTool (name = "JpsimumuPhysMonTool_NoTrig",
                                                      doMCTruth = DoTruth,
                                                      useTrigger = False,
                                                      TriggerSelection = "",
                                                      MuonContainer = myMuonContainer,
                                                      OutputLevel = OutputMessageLevel,
                                                      diMuonMassWindow = DimuonMassWindow,
                                                      lowerMassCut = LowerMassCut,
                                                      upperMassCut = UpperMassCut,
                                                      jpsiMass = JpsiMass,
                                                      jpsiMassBinning = JpsiMassBinning,
                                                      binsEta = BinsEta,
                                                      binsPhi = BinsPhi,
                                                      binsPt = BinsPt,
                                                      properTimeCut = ProperTimeCut, 
                                                      muonHigherPt= MuonHigherPtCut,
                                                      muonLowerPt = MuonLowerPtCut,
                                                      muonDeltaRcut = MuonDeltaRCut,
                                                      vertexChi2Cut = VertexChi2Cut,
                                                      vertexCut = DoVertexing,
                                                      TrkVertexFitterTool = TrkVKalVrtFitter,
                                                      V0Tools = InDetV0Tools,
                                                      ConversionFinderHelperTool = InDetConversionHelper,
                                                      VertexPointEstimator = VtxPointEstimator)

    ToolSvc += JpsimumuPhysMonTool_NoTrig
    muJpsimumuMonMan.AthenaMonTools += [ JpsimumuPhysMonTool_NoTrig ]
        
    if DoTrigger:
        JpsimumuPhysMonTool_L1_MU0 = JpsimumuPhysMonTool (name = "JpsimumuPhysMonTool_L1_MU0",
                                                       doMCTruth = DoTruth,
                                                       useTrigger = True,
                                                       TrigDecisionTool = myTrigDecisionTool,    
                                                       TriggerSelection = "L1_MU0",
                                                       MuonContainer = myMuonContainer,
                                                       OutputLevel = OutputMessageLevel,
                                                       diMuonMassWindow = DimuonMassWindow,
                                                       lowerMassCut = LowerMassCut,
                                                       upperMassCut = UpperMassCut,
                                                       jpsiMass = JpsiMass,
                                                       jpsiMassBinning = JpsiMassBinning,
                                                       binsEta = BinsEta,
                                                       binsPhi = BinsPhi,
                                                       binsPt = BinsPt,
                                                       properTimeCut = ProperTimeCut, 
                                                       muonHigherPt= MuonHigherPtCut,
                                                       muonLowerPt = MuonLowerPtCut,
                                                       muonDeltaRcut = MuonDeltaRCut,
                                                       vertexChi2Cut = VertexChi2Cut,
                                                       vertexCut = DoVertexing,
                                                       TrkVertexFitterTool = TrkVKalVrtFitter,
                                                       V0Tools = InDetV0Tools,
                                                       ConversionFinderHelperTool = InDetConversionHelper,
                                                       VertexPointEstimator = VtxPointEstimator)
        ToolSvc += JpsimumuPhysMonTool_L1_MU0
        muJpsimumuMonMan.AthenaMonTools += [ JpsimumuPhysMonTool_L1_MU0 ]
        
        JpsimumuPhysMonTool_L1_MU4 = JpsimumuPhysMonTool (name = "JpsimumuPhysMonTool_L1_MU4",
                                                          doMCTruth = DoTruth,
                                                          useTrigger = True,
                                                          TrigDecisionTool = myTrigDecisionTool,    
                                                          TriggerSelection = "L1_MU4",
                                                          MuonContainer = myMuonContainer,
                                                          OutputLevel = OutputMessageLevel,
                                                          diMuonMassWindow = DimuonMassWindow,
                                                          lowerMassCut = LowerMassCut,
                                                          upperMassCut = UpperMassCut,
                                                          jpsiMass = JpsiMass,
                                                          jpsiMassBinning = JpsiMassBinning,
                                                          binsEta = BinsEta,
                                                          binsPhi = BinsPhi,
                                                          binsPt = BinsPt,
                                                          properTimeCut = ProperTimeCut, 
                                                          muonHigherPt= MuonHigherPtCut,
                                                          muonLowerPt = MuonLowerPtCut,
                                                          muonDeltaRcut = MuonDeltaRCut,
                                                          vertexChi2Cut = VertexChi2Cut,
                                                          vertexCut = DoVertexing,
                                                          TrkVertexFitterTool = TrkVKalVrtFitter,
                                                          V0Tools = InDetV0Tools,
                                                          ConversionFinderHelperTool = InDetConversionHelper,
                                                          VertexPointEstimator = VtxPointEstimator)
        ToolSvc += JpsimumuPhysMonTool_L1_MU4
        muJpsimumuMonMan.AthenaMonTools += [ JpsimumuPhysMonTool_L1_MU4 ]

        JpsimumuPhysMonTool_EF_mu4 = JpsimumuPhysMonTool (name = "JpsimumuPhysMonTool_EF_mu4",
                                                          doMCTruth = DoTruth,
                                                          useTrigger = True,
                                                          TrigDecisionTool = myTrigDecisionTool,    
                                                          TriggerSelection = "EF_mu4",
                                                          MuonContainer = myMuonContainer,
                                                          OutputLevel = OutputMessageLevel,
                                                          diMuonMassWindow = DimuonMassWindow,
                                                          lowerMassCut = LowerMassCut,
                                                          upperMassCut = UpperMassCut,
                                                          jpsiMass = JpsiMass,
                                                          jpsiMassBinning = JpsiMassBinning,
                                                          binsEta = BinsEta,
                                                          binsPhi = BinsPhi,
                                                          binsPt = BinsPt,
                                                          properTimeCut = ProperTimeCut, 
                                                          muonHigherPt= MuonHigherPtCut,
                                                          muonLowerPt = MuonLowerPtCut,
                                                          muonDeltaRcut = MuonDeltaRCut,
                                                          vertexChi2Cut = VertexChi2Cut,
                                                          vertexCut = DoVertexing,
                                                          TrkVertexFitterTool = TrkVKalVrtFitter,
                                                          V0Tools = InDetV0Tools,
                                                          ConversionFinderHelperTool = InDetConversionHelper,
                                                          VertexPointEstimator = VtxPointEstimator)
        ToolSvc += JpsimumuPhysMonTool_EF_mu4
        muJpsimumuMonMan.AthenaMonTools += [ JpsimumuPhysMonTool_EF_mu4 ]

    topSequence += muJpsimumuMonMan
    print muJpsimumuMonMan
    ######################################################################################
    ######################################################################################
    ## --------------------------------------------------
    ## JpsiMuonInsituPerformance
    ##--------------------------------------------------------
    from MuonPhysicsMonitoring.MuonPhysicsMonitoringConf import JpsiMuonInsituPerformance
    JpsiMuonInsituPerformance = JpsiMuonInsituPerformance(name = "JpsiMuonInsituPerformance",
                                                          doMCTruth = DoTruth,
                                                          useTrigger = DoTrigger,
                                                          TrigDecisionTool = myTrigDecisionTool,
                                                          TriggerSelection = "EF_mu4",
                                                          TriggerChains = [ "" ],
                                                          doTriggerEffL1 = False,
                                                          doTriggerEffL2 = False,
                                                          doTriggerEffEF = False,
                                                          doEfficiencies = True,
                                                          doBkgExtraction = False,
                                                          doBkgExtractionAfterProbeAssociation = False,
                                                          MuonContainer = "StacoMuonCollection",
                                                          MuonCollection = myMuonContainer,
                                                          TruthParticleContainer = "INav4MomTruthEvent",
                                                          TrackParticleTruthCollection = "TrackParticleTruthCollection",
                                                          McEventCollection = "TruthEvent",
                                                          OutputLevel = OutputMessageLevel,
                                                          diMuonMassWindow = DimuonMassWindow,
                                                          lowerMassCut = LowerMassCut,
                                                          upperMassCut = UpperMassCut,
                                                          jpsiMass = JpsiMass,
                                                          jpsiMassBinning = 50, #JpsiMassBinning,
                                                          jpsiSigma = 80,
                                                          binsEta = BinsEta,
                                                          binsPhi = BinsPhi,
                                                          binsPt = BinsPt,

                                                          ## Track Quality cuts:
                                                          TRTHitsCut = -1,
                                                          TRTOutliersCut = -1,
                                                          SCTHitsCut = 5,
                                                          PixHitsCut = 0,
                                                          d0SignificanceCut = 0,
                                                          chi2Cut = 0,
                                                          pCut = 3000,
                                                          ##

                                                          tagMatchL1 = "L1_MU0",
                                                          tagMatchL2 = "",
                                                          tagMatchEF = "",
                                                          tagCombinedMuonOnly = True,
                                                          tagMuonPtCut = 2500,
                                                          probeMuonPtCut = 2000,
                                                          properTimeCut = 0.2, 
                                                          deltaRMinCut = 0.,
                                                          deltaRMaxCut = 3.5,
                                                          vertexChi2Cut = 10,
                                                          TrkVertexFitterTool = TrkVKalVrtFitter,
                                                          V0Tools = InDetV0Tools,
                                                          ConversionFinderHelperTool = InDetConversionHelper,
                                                          VertexPointEstimator = VtxPointEstimator)


    
    ToolSvc += JpsiMuonInsituPerformance
    muJpsiInSituMonMan.AthenaMonTools += [JpsiMuonInsituPerformance]  
    topSequence += muJpsiInSituMonMan
    print muJpsiInSituMonMan

###########################################################################
#--------------------------------------------------------------------------
#--------------------------------------------------------------------------
###########################################################################

if MuonDQADetFlags.doZmumuMon:
    ## -------------------------------------------------------
    ## ZmumuPhysMonTool
    ##--------------------------------------------------------
    from MuonPhysicsMonitoring.MuonPhysicsMonitoringConf import ZmumuPhysMonTool
    ZmumuPhysMonTool = ZmumuPhysMonTool (name = "ZmumuPhysMonTool",
                                         doMCTruth = False, # to monitor real data
                                         useTrigger = DoTrigger,
                                         TrigDecisionTool = myTrigDecisionTool,
                                         TriggerSelection = "",
                                         MuonContainer = myMuonContainer,
                                         MuonSpectrometerContainer = "MuonboyTrackParticles", 
                                         InnerTrackContainer = "TrackParticleCandidate",
                                         JetContainer = myJetContainer,
                                         diMuonMassWindow = 10000,
                                         muonEtaCut = 2.5,
                                         muonHigherPt =20000,
                                         muonLowerPt = 20000,
                                         muonDeltaPhiCut = 2.,
                                         isolationJet = 15000,
                                         isolationNtracks = 5,
                                         isolationPtSum = 8000,
                                         isolationET = 6000,
                                         isoConeMax = 0.5,
                                         isoConeMin = 0.05, 
                                         OutputLevel = WARNING)					 
    ToolSvc += ZmumuPhysMonTool
    muZmumuPhysMonMan.AthenaMonTools += [ZmumuPhysMonTool]
    topSequence += muZmumuPhysMonMan
    print muZmumuPhysMonMan
    ##--------------------------------------------------------
    ## MuonInsituPerformance
    ##--------------------------------------------------------
    from MuonPhysicsMonitoring.MuonPhysicsMonitoringConf import MuonInsituPerformance
    MuonInsituPerformance = MuonInsituPerformance(name = "MuonInsituPerformance",
                                                  doMCTruth = False, # to monitor real data
                                                  useTrigger = DoTrigger,                                        
                                                  TrigDecisionTool = myTrigDecisionTool,
                                                  MuonContainer = "StacoMuonCollection",
                                                  MuonSpectrometerContainer = "MuonboyTrackParticles", 
                                                  InnerTrackContainer = "TrackParticleCandidate",
                                                  JetContainer = myJetContainer,
                                                  diMuonMassWindow = 10000,
                                                  muonEtaCut = 2.5,
                                                  tagMuonPtCut =20000,
                                                  probeMuonPtCut = 20000,
                                                  muonDeltaPhiCut = 2.,
                                                  isolationJet = 15000,
                                                  isolationNtracks = 5,
                                                  isolationPtSum = 8000,
                                                  isolationET = 6000,
                                                  isoConeMax = 0.5,
                                                  isoConeMin = 0.05, 
                                                  OutputLevel = WARNING)
    ToolSvc += MuonInsituPerformance
    muInSituMonMan.AthenaMonTools += [MuonInsituPerformance]   
    topSequence += muInSituMonMan
    print muInSituMonMan

 
if MuonDQADetFlags.doZmumuMon or MuonDQADetFlags.doJpsimumuMon:
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

####Commented out due to https://savannah.cern.ch/bugs/?40935
######## Importing InsituDatabaseTools
##    from InsituDatabaseTools.InsituDatabaseToolsConf import InsituDatabaseTools
##    msInsituDatabaseTools = InsituDatabaseTools(name = "msInsituDatabaseToolsName")
##    ToolSvc+=msInsituDatabaseTools 
#
##-----------------------------------------------#    
### Importing MuonSpectrometerProbeCollectorTool
##-----------------------------------------------#
#from MuonDQAUtils.MuonDQAUtilsConf import Muon__MuonSpectrometerProbeCollectorTool
#muonProbeCollectorTool = Muon__MuonSpectrometerProbeCollectorTool(name = "muonProbeCollectorTool",
#                                                                  CombinedMuonTracksContainerName = "StacoMuonCollection" )
#
###### Adding InsituTrackTools to MuonSpectrometerProbeCollectorTool
#muonProbeCollectorTool.InsituPerformanceTools = muonInsituTrackTools
#ToolSvc+= muonProbeCollectorTool
###### Importing InnerDetProbeCollectorTool
#from MuonDQAUtils.MuonDQAUtilsConf import Muon__InnerDetProbeCollectorTool
#idProbeCollectorTool = Muon__InnerDetProbeCollectorTool(name ="idProbeCollectorTool",
#                                                        CombinedMuonTracksContainerName = "StacoMuonCollection" )
###### Adding InsituTrackTools to InnerDetProbeCollectorTool
#idProbeCollectorTool.InsituPerformanceTools = idInsituTrackTools
###    idProbeCollectorTool.InsituPerformanceTools = muonInsituTrackTools
#ToolSvc+=idProbeCollectorTool
####MuonInsituMonitoring##################
#from MuonPhysicsMonitoring.MuonPhysicsMonitoringConf import MuonInsituMonitoring
#MuonInsituMonitoring = MuonInsituMonitoring( name = "MuonInsituMonitoring",
#                                             MuonContainerLocation = [ "StacoMuonCollection", "MuidMuonCollection" ],
#                                             MuonAuthor = ["STACO","MuidComb"],
#                                             doMuonspectrometerAnalysis = [1,1],
#                                             doInnerDetectorAnalysis = [1,0],
#                                             doCombinedAnalysis = [1,1],
#                                             OutputLevel = INFO )
#ToolSvc += MuonInsituMonitoring
#MuonInsituMonitoring.MSProbeCollectorTool = muonProbeCollectorTool
#MuonInsituMonitoring.IDProbeCollectorTool = idProbeCollectorTool
#MuonInsituMonitoring.InsituPerformanceTool = muonInsituTrackTools
#muPhysMonMan.AthenaMonTools += [MuonInsituMonitoring]
###MuonInsituResolution##################
#from MuonPhysicsMonitoring.MuonPhysicsMonitoringConf import MuonInsituResolution
#MuonInsituResolution = MuonInsituResolution(name = "MuonInsituResolution",
#                                            MuonTrackContainerName = ["MuonboyTrackParticles", "MooreTrackParticles"],
# 					   MuonTrackContainerFlag = [1,1],
# 					   OutputLevel = INFO )
#ToolSvc += MuonInsituResolution
#
## Adding Tools to Algorithms
#MuonInsituResolution.InsituPerformanceTool = muonInsituTrackTools
####commented out due to https://savannah.cern.ch/bugs/?40935
#MuonInsituResolution.InsituDatabaseTool = msInsituDatabaseTools
#muPhysMonMan.AthenaMonTools += [MuonInsituResolution]
 
