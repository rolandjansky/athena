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

####from MuonRecExample import MuonRecTools


muTrkInSituMonMan = AthenaMonManager(name="MuTrkInSituMonManager",
                                     FileKey             = DQMonFlags.monManFileKey(),
                                     Environment         = DQMonFlags.monManEnvironment(),
                                     OutputLevel         = muonOutputLevel)
muZmuTrkPhysMonMan = AthenaMonManager(name="MuonZmuTrkPhysMonManager",
                                     FileKey             = DQMonFlags.monManFileKey(),
                                     Environment         = DQMonFlags.monManEnvironment(),
                                     OutputLevel         = muonOutputLevel)

#------------------------------------------
myElectronContainer = "ElectronAODCollection"
myMuonContainer = "StacoMuonCollection"
myJetContainer = "AntiKt4TopoEMJets"

# TrigDecisionTool configuration
DoTrigger = DQMonFlags.useTrigger() and hasattr(ToolSvc, DQMonFlags.nameTrigDecTool())
myTrigDecisionTool = ""
if DoTrigger:
        myTrigDecisionTool = getattr(ToolSvc, DQMonFlags.nameTrigDecTool())

#############
useAODContainers = True;  # to set the right containers
#############
if useAODContainers:
    myJetContainer= "AntiKt4TopoEMJets"

BinsEta = [ -2.5, -1.4, 1.4, 2.51 ]
BinsPhi = [ -3.15,3.15 ]
BinsPt  = [ 0., 1000., 2000., 3000., 4000, 6000., 10000. ]

#------------------------------------------
if MuonDQADetFlags.doMuTrkMon:
#---- Track Isolation
    from InDetRecExample.InDetKeys import InDetKeys
    include( "TrackIsolationTools/TrackIsolationTool_jobOptions.py" )
    # needed for Analysis::Muon building
    include( "MuonIsolationTools/MuonIsolationTool_jobOptions.py" )
    ##--------------------------------------------------------
    ## MuTrackInsituPerformance
    ##--------------------------------------------------------
    from MuonPhysicsMonitoring.MuonPhysicsMonitoringConf import MuTrackInsituPerformance
    MuTrackInsituPerformance = MuTrackInsituPerformance(name = "MuTrackInsituPerformance",
                                                        doMCTruth = False, # to monitor real data
                                                        useTrigger = DoTrigger,
                                                        TrigDecisionTool = myTrigDecisionTool, 
                                                        MuonContainer = myMuonContainer,
                                                        MuonSpectrometerContainer = "MuonboyTrackParticles", 
                                                        InnerTrackContainer = "TrackParticleCandidate",
                                                        JetContainer = myJetContainer,
							ElectronContainer = myElectronContainer,
							vxContainerName=InDetKeys.PrimaryVertices(),
                                                        diMuonMassWindow = 10000.,
                                                        binsEta = BinsEta,
                                                        binsPhi = BinsPhi,
                                                        binsPt = BinsPt,
                                                        muonEtaCut = 2.5,
                                                        tagPtCut =2000.,
                                                        probePtCut = 0,
                                                        muonDeltaPhiCut = 0.,
                                                        isolationJet = 0.,
                                                        isolationTrkIso = 0.,
                                                        isolationET = 0,
                                                        isoConeMax = 0.5,
                                                        isoConeMin = 0.05,
                                                        ###Probe track muon association requirements:
                                                        TrkIsoCone = 0., 
                                                        TrkPt = 0.,
                                                        TrkRelTrkIso = 0.,
                                                        TrkNhitsSCT = 1,
                                                        TrkNhitsPIX = 2,
                                                        TrkImpactPar = 0.,
                                                        TrkChi2Ndf = 100.,
                                                        OutputLevel = WARNING)
    MuTrackInsituPerformance.TagmuProbeTrk_deltaRMatchCut = 0.3
    MuTrackInsituPerformance.EnergyIsoConeSize = 0.0
    MuTrackInsituPerformance.TrackEIsoBarrelCut = 0.
    MuTrackInsituPerformance.TrackEIsoPtRatioBarrelCut = 0.
    MuTrackInsituPerformance.TrackEIsoTransitionCut = 0.
    MuTrackInsituPerformance.TrackEIsoPtRatioTransitionCut=0.0
    MuTrackInsituPerformance.TrackEIsoEndCapCut=0.
    MuTrackInsituPerformance.TrackEIsoPtRatioEndCapCut=0.
    MuTrackInsituPerformance.CaloIsol=False
#    MuTrackInsituPerformance.MuonIsolationTool = ToolSvc.MuonIsolationTool
#    MuTrackInsituPerformance.takeSummaryAndPerigeeFrom = "TrackParticle"
#    MuTrackInsituPerformance.IsolationTool = TrackIsolationTool
    ToolSvc += MuTrackInsituPerformance
    muTrkInSituMonMan.AthenaMonTools += [MuTrackInsituPerformance]   
    topSequence += muTrkInSituMonMan
    print muTrkInSituMonMan
    ##--------------------------------------------------------
    ## MuTrackPhysMonTool
    ##--------------------------------------------------------
    from MuonPhysicsMonitoring.MuonPhysicsMonitoringConf import MuTrackPhysMonTool
    MuTrackPhysMonTool = MuTrackPhysMonTool(name = "MuTrackPhysMonTool",
                                            doMCTruth = False, # to monitor real data
                                            useTrigger = DoTrigger,
                                            TrigDecisionTool = myTrigDecisionTool,
                                            MuonContainer = myMuonContainer,
                                            MuonSpectrometerContainer = "MuonboyTrackParticles", 
                                            InnerTrackContainer = "TrackParticleCandidate",
                                            JetContainer = myJetContainer,
                                            ElectronContainer = myElectronContainer,
					    diMuonMassWindow = 10000,
                                            binsEta = BinsEta,
                                            binsPhi = BinsPhi,
                                            binsPt = BinsPt,
                                            muonEtaCut = 2.5,
                                            tagPtCut =3000,
                                            probePtCut = 1000,
                                            muonDeltaPhiCut = 0.,
                                            isolationJet = 0.,
                                            isolationTrkIso = 0.,
                                            isolationET = 0.,
                                            isoConeMax = 0.5,
                                            isoConeMin = 0.05,
                                            ###Probe track muon association requirements:
                                            TrkIsoCone = 0.4, 
                                            TrkPt = 1000,
                                            TrkRelTrkIso = 0.2,
                                            TrkNhitsSCT = 1,
                                            TrkNhitsPIX = 2,
                                            TrkImpactPar = 0.,
                                            TrkChi2Ndf = 100,
                                            OutputLevel = WARNING)

#    MuTrackPhysMonTool.MuonIsolationTool = ToolSvc.MuonIsolationTool
    ToolSvc += MuTrackPhysMonTool
    muZmuTrkPhysMonMan.AthenaMonTools += [MuTrackPhysMonTool]       
    topSequence += muZmuTrkPhysMonMan
    print muZmuTrkPhysMonMan 
