# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc

job = AlgSequence()

from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors


# add an instance of the TP Algorithm
def AddTagProbeAlg(name, ProbeCont, MatchCont):

    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()
    MuonContainerToUse = "Muons"
    if hasattr(job, "MuonQualityUpdater"):
        MuonContainerToUse = "UpdatedMuons"
    from MuonPerformanceAlgs.MuonPerformanceAlgsConf import MuonTPAlg
    thisAlg = MuonTPAlg(name)
    thisAlg.TagContainerName   = MuonContainerToUse
    thisAlg.ProbeContainerName = ProbeCont
    thisAlg.MatchContainerName = MatchCont
    job += thisAlg
    return thisAlg

# add a muon selection tool if there is none yet
def AddMuonSelectionTool():

    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"TagProbeMuonSelectionTool"):
        from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
        ToolSvc += CP__MuonSelectionTool("TagProbeMuonSelectionTool")
        ToolSvc.TagProbeMuonSelectionTool.MaxEta = 99.  # don't cut on eta in this tool

# add a GRL tool if there is none yet
def AddGoodRunsListSelectionTool(GoodRunsListVec=["/afs/cern.ch/user/a/atlasdqm/grlgen/Atlas_Ready/data15_13TeV.periodAllYear_HEAD_DQDefects-00-01-02_PHYS_StandardGRL_Atlas_Ready.xml"]):

    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"MCPAnalysisGRLTool"):
        from AthenaCommon import CfgMgr
        ToolSvc += CfgMgr.GoodRunsListSelectionTool("MCPAnalysisGRLTool")
        ToolSvc.MCPAnalysisGRLTool.GoodRunsListVec=GoodRunsListVec

# Add a track isolation updater tool
def AddIsolationTools():
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    from AthenaCommon.AppMgr import ToolSvc
    
    if not hasattr(ToolSvc, "MyTrackIsoTool"):
        print "Adding it!!"
            
        #try:
        from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
        ToolSvc += xAOD__TrackIsolationTool("MyTrackIsoTool")
        # Cuts suggested by Dongliang to reproduce the default muon ptcone
        ToolSvc.MyTrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
        ToolSvc.MyTrackIsoTool.TrackSelectionTool.minPt= 1000.
        #ToolSvc.MyTrackIsoTool.TrackSelectionTool.minNSiHits= 9
        #ToolSvc.MyTrackIsoTool.TrackSelectionTool.minNPixelHits= 1
        ToolSvc.MyTrackIsoTool.TrackSelectionTool.CutLevel="Loose"
        
        
        from AthenaCommon.GlobalFlags import globalflags
        isMC = not globalflags.DataSource()=='data'
        from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool
        IsoCorrectionTool = CP__IsolationCorrectionTool ("NewLeakageCorrTool",IsMC = isMC)
        ToolSvc += IsoCorrectionTool

        # tool to collect topo clusters in cone
        from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
        ToolSvc += xAOD__CaloClustersInConeTool("MyCaloClustersInConeTool",CaloClusterLocation = "CaloCalTopoClusters")

        from CaloIdentifier import SUBCALO

        from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
        ToolSvc += xAOD__CaloIsolationTool("MyCaloIsoTool")
        ToolSvc.MyCaloIsoTool.IsoLeakCorrectionTool = ToolSvc.NewLeakageCorrTool
        ToolSvc.MyCaloIsoTool.ClustersInConeTool = ToolSvc.MyCaloClustersInConeTool
        ToolSvc.MyCaloIsoTool.EMCaloNums  = [SUBCALO.LAREM]
        ToolSvc.MyCaloIsoTool.HadCaloNums = [SUBCALO.LARHEC, SUBCALO.TILE]
        ToolSvc.MyCaloIsoTool.UseEMScale  = True
        ToolSvc.MyCaloIsoTool.saveOnlyRequestedCorrections = True   
        
        #from MuonPerformanceAlgs.MuonPerformanceAlgsConf import TrackIsolationDecorAlg

        #job += TrackIsolationDecorAlg("MyIDTrackDecorator")
        #job.MyIDTrackDecorator.TrackIsolationTool = ToolSvc.MyTrackIsoTool
        #job.MyIDTrackDecorator.CaloIsolationTool = ToolSvc.MyCaloIsoTool

        
        #except AttributeError:
            #print "This did not work :("
            #sys.exit(1)
    
# Add the trigger decision tool
def AddTrigDecisionTool():
    from AthenaCommon import CfgMgr
    from AthenaCommon.Constants import WARNING
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc, "TrigDecisionTool"):
        ToolSvc += CfgMgr.Trig__TrigDecisionTool( "TrigDecisionTool",
                                            OutputLevel = WARNING,
                                            TrigDecisionKey = "xTrigDecision" )
    
# Add the trigger matching tool
def AddTrigMatchingTool():
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"TrigMuonMatching"):
        from TrigMuonMatching.TrigMuonMatchingConf import Trig__TrigMuonMatching
        trigmatch = Trig__TrigMuonMatching("TrigMuonMatching")
        ToolSvc += trigmatch
        #trigmatch.dRmin = 0.1
        #trigmatch.LVL1dRmin = 0.2
        trigmatch.TriggerTool = ToolSvc.TrigDecisionTool

        
# Add instance of the TP plotting tool
def AddPlottingTool(name, EffiFlag, applySF=False, DoProbeMatchPlots=False, ProduceEfficiencies=False):

    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import DiMuonTPPlotTool
    thetool = DiMuonTPPlotTool(name)
    thetool.EfficiencyFlag = EffiFlag
    thetool.doTrkValidPlots = False
    thetool.doJPsiKinematicPlots = False
    thetool.doZmumuKinematicPlots = False
    thetool.doFineEtaPhiPlots = False
    thetool.doDetectorRegionPlots = False
    thetool.DoAsymmErrorGraphs = False
    thetool.doJPsiDileptonPlots = False
    thetool.doZmumuDileptonPlots = False
    thetool.doTrigValidPlots = False
    thetool.ProduceProbeMatchPlots = DoProbeMatchPlots
    thetool.ProduceEfficiencies = ProduceEfficiencies
    thetool.ApplyScaleFactors = applySF
    ToolSvc+= thetool
    return thetool

# Add a tree filling tool
def AddTreeTool (name, EffiFlag,WriteSFInfo=False):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import DiMuonTPTreeTool
    thetool = DiMuonTPTreeTool(name)
    thetool.EfficiencyFlag = EffiFlag
    thetool.MuonSelectionTool = ToolSvc.TagProbeMuonSelectionTool
    thetool.RecordScaleFactorInfo = WriteSFInfo
    ToolSvc+= thetool
    return thetool
    
def AddMuonEfficiencyScaleFactors (WorkingPoint="Medium"):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors
    if not hasattr(ToolSvc, "EffiScaleFactors_%s"%WorkingPoint):
        sf = CP__MuonEfficiencyScaleFactors("EffiScaleFactors_%s"%WorkingPoint)
        sf.WorkingPoint = WorkingPoint
        ToolSvc += sf
    return getattr(ToolSvc,  "EffiScaleFactors_%s"%WorkingPoint)

# Add a TP selection tool
def AddZmumuTPSelectionTool (name, EffiFlag, ProbeType, SameSign=False, AntiIso=False, UseTrigger=True):

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.Constants import DEBUG
    from MuonTPTools.MuonTPToolsConf import DiMuonTPSelectionTool
    thetool = DiMuonTPSelectionTool(name)
    thetool.IsNotIncludedInNominal = False
    
    # Try to add the isolation updater tools (only works in full athena!)
    if hasattr(ToolSvc, "MyTrackIsoTool"):
        thetool.TrackIsolationTool = ToolSvc.MyTrackIsoTool
        thetool.CaloIsolationTool = ToolSvc.MyCaloIsoTool
    if hasattr(ToolSvc, "MCPAnalysisGRLTool"):
        thetool.GRLTool = ToolSvc.MCPAnalysisGRLTool
    # pt cuts
    thetool.TagPtCut = 28000.
    thetool.ProbePtCut = 10000.
    # Z window
    thetool.HighMassWindow = 121000.
    thetool.LowMassWindow = 61000.
    #thetool.OutputLevel=DEBUG      # this is a bit spammy
    thetool.ProbeTrackIsoCut = -1.
    
    # Tag isolation 
    thetool.TagTrackIsoCut = 0.2
    thetool.TagCaloIsoCut = 0.2
    thetool.TagTrackAntiIsoCut = -1
    thetool.TagCaloAntiIsoCut = -1
    
    # Probe isolation: disabled by default - turned on below 
    thetool.ProbeTrackIsoCut = -1
    thetool.ProbeTrackAntiIsoCut = -1
    thetool.ProbeCaloIsoCut = -1
    thetool.ProbeCaloAntiIsoCut = -1
    
    # Tag IP cuts
    thetool.TagD0Cut = -1
    thetool.TagD0SignCut = 3
    thetool.TagZ0Cut = 10
    
    # Probe IP cuts
    thetool.ProbeD0Cut = -1
    thetool.ProbeD0SignCut = 3
    thetool.ProbeZ0Cut = 10
    
    thetool.EfficiencyFlag = EffiFlag
    # tools  
    thetool.SelectionTool = ToolSvc.TagProbeMuonSelectionTool
    thetool.TriggerMatchTool = ToolSvc.TrigMuonMatching
    thetool.TriggerDecisionTool = ToolSvc.TrigDecisionTool
    # trigger(s)
    if UseTrigger:
        thetool.TagTriggerList = ["HLT_mu14","HLT_mu20_iloose_L1MU15","HLT_mu24_iloose_L1MU15","HLT_mu24_imedium","HLT_mu26_imedium"]
    #thetool.TagTriggerList = []

    if ProbeType == "ID":
        thetool.UseIDProbes=True
        thetool.ProbeIDHitCut=True
        thetool.DeltaPhiCut=2.0
        thetool.ProbeTrackIsoCut = 0.15    # try runing without probe iso!
        thetool.ProbeCaloIsoCut = 0.25
        #thetool.ProbeTrackIsoCut = -1
        if AntiIso:
            thetool.ProbeTrackIsoCut = -1
            thetool.ProbeCaloIsoCut = -1
            thetool.ProbeTrackAntiIsoCut = 1.2
            thetool.ProbeCaloAntiIsoCut = 1.2
            thetool.HighMassWindow = 121000.
            thetool.LowMassWindow = 61000.
            thetool.ProbeZ0Cut = -1
            thetool.ProbeD0SignCut =-1
            thetool.ProbeD0Cut =-1
    if ProbeType == "MS":
        thetool.ProbeIDHitCut=False
        thetool.UseMSProbes = True
        thetool.DeltaPhiCut=2.0
        thetool.ProbeCaloIsoCut = 0.35    # try runing without probe iso!
        thetool.ProbeD0Cut = -1
        thetool.ProbeZ0Cut = -1
        thetool.ProbeD0SignCut =-1
        if AntiIso:
            thetool.ProbeCaloIsoCut = -1
            thetool.ProbeCaloAntiIsoCut = 1.2
            thetool.HighMassWindow = 121000.
            thetool.LowMassWindow = 61000.
            thetool.ProbeZ0Cut = -1
            thetool.ProbeD0SignCut =-1
            thetool.ProbeD0Cut =-1
            #thetool.TagTrackIsoCut = -1
            #thetool.TagTrackAntiIsoCut = 1.4
    if ProbeType == "CaloTag":
        thetool.UseCaloProbes = True
        thetool.ProbeIDHitCut=True
        thetool.ProbeTrackIsoCut = 0.15    # try runing without probe iso!
        #thetool.ProbeCaloIsoCut = 0.25
        thetool.DeltaPhiCut=2.0
        if AntiIso:
            thetool.ProbeTrackIsoCut = -1
            thetool.ProbeCaloIsoCut = -1
            thetool.ProbeTrackAntiIsoCut = 1.2
            thetool.ProbeCaloAntiIsoCut = 1.2
            thetool.HighMassWindow = 121000.
            thetool.LowMassWindow = 61000.
            thetool.ProbeZ0Cut = -1
            thetool.ProbeD0SignCut =-1
            thetool.ProbeD0Cut =-1
    if ProbeType == "Truth":
        thetool.UseTruthProbes = True
    if ProbeType == "Loose":
        thetool.UseLooseProbes = True
    if ProbeType == "Medium":
        thetool.UseMediumProbes = True
    if ProbeType == "Tight":
        thetool.UseTightProbes = True
    if ProbeType == "HighPt":
        thetool.UseHighPtProbes = True


    if SameSign:
        thetool.AcceptOppCharge=False
        thetool.AcceptSameCharge=True

    ToolSvc += thetool
    return thetool

# Add a TP selection tool
def AddJPsiTPSelectionTool (name, EffiFlag, ProbeType, SameSign=False, AntiIso=False, UseTrigger=True):

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.Constants import DEBUG
    from MuonTPTools.MuonTPToolsConf import DiMuonTPSelectionTool
    thetool = DiMuonTPSelectionTool(name)
    thetool.IsNotIncludedInNominal = False
    
    # Try to add the isolation updater tools (only works in full athena!)
    if hasattr(ToolSvc, "MyTrackIsoTool"):
        thetool.TrackIsolationTool = ToolSvc.MyTrackIsoTool
        thetool.CaloIsolationTool = ToolSvc.MyCaloIsoTool
    if hasattr(ToolSvc, "MCPAnalysisGRLTool"):
        thetool.GRLTool = ToolSvc.MCPAnalysisGRLTool
    # pt cuts
    thetool.TagPtCut = 4000.
    thetool.ProbePtCut = 2500.
    # JPsi mass window
    thetool.HighMassWindow = 3500.
    thetool.LowMassWindow = 2700.
    #thetool.OutputLevel=DEBUG      # this is a bit spammy
    thetool.ProbeTrackIsoCut = -1.
    
    # Tag isolation 
    thetool.TagTrackIsoCut = -1
    thetool.TagCaloIsoCut = -1
    thetool.TagTrackAntiIsoCut = -1
    thetool.TagCaloAntiIsoCut = -1
    
    # Probe isolation: disabled by default - turned on below 
    thetool.ProbeTrackIsoCut = -1
    thetool.ProbeTrackAntiIsoCut = -1
    thetool.ProbeCaloIsoCut = -1
    thetool.ProbeCaloAntiIsoCut = -1
    
    # Tag IP cuts
    thetool.TagD0Cut = 0.3
    thetool.TagD0SignCut = 3
    thetool.TagZ0Cut = 10
    
    # Probe IP cuts
    thetool.ProbeD0Cut = 0.3
    thetool.ProbeD0SignCut = 3
    thetool.ProbeZ0Cut = 10
    
    thetool.EfficiencyFlag = EffiFlag
    # tools  
    thetool.SelectionTool = ToolSvc.TagProbeMuonSelectionTool
    thetool.TriggerMatchTool = ToolSvc.TrigMuonMatching
    thetool.TriggerDecisionTool = ToolSvc.TrigDecisionTool
    # trigger(s)
    if UseTrigger:
        thetool.TagTriggerList = ["HLT_noalg_L1MU4",
                                  "HLT_noalg_L1MU6",
                                  "HLT_mu4",
                                  "HLT_mu6"]
        if ProbeType == "ID" or ProbeType == "CaloTag" or ProbeType == "Loose" or ProbeType == "Medium" or ProbeType == "Tight":
            thetool.TagTriggerList += [
                                  "HLT_mu4_bJpsi_Trkloose",
                                  "HLT_mu6_bJpsi_Trkloose",
                                  "HLT_mu10_bJpsi_Trkloose",
                                  "HLT_mu18_bJpsi_Trkloose",]
        if ProbeType == "MS":
            thetool.TagTriggerList = ["HLT_mu4_mu4_idperf_bJpsimumu_noid"]
        if ProbeType == "Loose" or ProbeType == "Medium" or ProbeType == "Tight":
            thetool.TagTriggerList += [
                                  "HLT_mu20_2mu0noL1_JpsimumuFS",
                                  "HLT_mu18_2mu0noL1_JpsimumuFS",
                                  "HLT_mu20_2mu4_JpsimumuL2",
                                  "HLT_mu18_2mu4_JpsimumuL2"]
            
    thetool.DeltaPhiCut=0.2
    thetool.DeltaEtaCut=0.2

    if ProbeType == "ID":
        thetool.UseIDProbes=True
        thetool.ProbeIDHitCut=True
    if ProbeType == "MS":
        thetool.ProbeIDHitCut=False
        thetool.UseMSProbes = True
    if ProbeType == "CaloTag":
        thetool.UseCaloProbes = True
        thetool.ProbeIDHitCut=True
    if ProbeType == "Truth":
        thetool.UseTruthProbes = True
    if ProbeType == "TruthMatchedMuons":
        thetool.UseTruthMatchedProbes = True
    if ProbeType == "Loose":
        thetool.UseLooseProbes = True
    if ProbeType == "Medium":
        thetool.UseMediumProbes = True
    if ProbeType == "Tight":
        thetool.UseTightProbes = True
    if ProbeType == "HighPt":
        thetool.UseHighPtProbes = True

    ToolSvc += thetool
    return thetool


# Add a reco efficiency tool
def AddMuonRecoTPEfficiencyTool (name, EffiFlag):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import MuonRecoTPEfficiencyTool
    thetool = MuonRecoTPEfficiencyTool(name)
    thetool.MuonAuthor = 21
    thetool.MatchToAnyMS = False
    thetool.MatchToCB = False
    thetool.MatchToLoose = False
    thetool.MatchToMedium = False
    thetool.MatchToTight = False
    thetool.MatchToID = False
    thetool.MatchToCaloTag = False
    thetool.MatchToHighPt = False
    thetool.MatchPtCut = 2000.0         # speed up the loop a bit
    thetool.EfficiencyFlag = EffiFlag
    thetool.SelectionTool = ToolSvc.TagProbeMuonSelectionTool
    thetool.TriggerMatchTool = ToolSvc.TrigMuonMatching
    ToolSvc += thetool
    return thetool


# Add a trigger efficiency tool
def AddTriggerTPEfficiencyTool (name, EffiFlag):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import MuonTrigTPEfficiencyTool
    thetool = MuonTrigTPEfficiencyTool(name)
    thetool.MuonAuthor       = 21
    thetool.MatchPtCut       = 5000.0         # speed up the loop a bit
    thetool.EfficiencyFlag   = EffiFlag
    thetool.SelectionTool    = ToolSvc.TagProbeMuonSelectionTool
    thetool.TriggerMatchTool = ToolSvc.TrigMuonMatching
    ToolSvc += thetool
    return thetool


# Add an iso efficiency tool
def AddIsolTPEfficiencyTool (name, EffiFlag):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import MuonIsolTPEfficiencyTool
    thetool = MuonIsolTPEfficiencyTool(name)
    thetool.MuonAuthor       = 21
    thetool.MatchPtCut       = 5000.0         # speed up the loop a bit
    thetool.EfficiencyFlag   = EffiFlag
    thetool.SelectionTool    = ToolSvc.TagProbeMuonSelectionTool
    thetool.TriggerMatchTool = ToolSvc.TrigMuonMatching
    ToolSvc += thetool
    return thetool

# Add an instance of the 'main' TP tool
def AddMuonTPTool (name, EffiFlag):

    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import MuonTPTool
    thetool = MuonTPTool(name)
    thetool.EfficiencyFlag = EffiFlag
    #thetool.OutputLevel = DEBUG
    thetool.DumpInefficientEvents = False
    ToolSvc += thetool
    return thetool

