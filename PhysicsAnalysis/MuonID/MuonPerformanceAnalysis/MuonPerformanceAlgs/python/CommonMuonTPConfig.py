# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc

job = AlgSequence()

from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors
from MuonPerformanceAlgs import TriggerLists

def AddMuonTPMetaAlg():
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    if not hasattr(job, "MuonTPMetaDataAlg"):
        from MuonPerformanceAlgs.MuonPerformanceAlgsConf import  MuonTPMetaDataAlg
        thisAlg = MuonTPMetaDataAlg("MuonTPMetaDataAlg")
        job+= thisAlg
    return getattr(job, "MuonTPMetaDataAlg")
    
# add an instance of the TP Algorithm
def AddTagProbeAlg(name, ProbeCont):
    AddMuonTPMetaAlg()
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()
    MuonContainerToUse = GetRightMuonContainer()

    from MuonPerformanceAlgs.MuonPerformanceAlgsConf import MuonTPAlg
    thisAlg = MuonTPAlg(name)
    thisAlg.TagContainerName   = MuonContainerToUse
    thisAlg.ProbeContainerName = ProbeCont
    job += thisAlg
    return thisAlg

def AddPromptLeptonDecorators():
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()

    import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as Config
    job += Config.DecoratePromptLepton("Muons",     "AntiKt4PV0TrackJets")

def GetBadMuonEventVetoHelper():
    from MuonTPTools.MuonTPToolsConf import BadMuonVetoHelperTool
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr
    if not hasattr(ToolSvc, "BadMuonVetoHelperTool"):
        tool = CfgMgr.BadMuonVetoHelperTool("BadMuonVetoHelperTool")
        tool.MuonSelectionTool = GetMuonSelectionTool()
        tool.MuonContainer = GetRightMuonContainer()
        ToolSvc += tool
    return getattr(ToolSvc, "BadMuonVetoHelperTool")

def GetVertexHelper():
    from MuonTPTools.MuonTPToolsConf import MuonTPVertexHelper
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr
    if not hasattr(ToolSvc, "MuonTPVertexHelper"):
        tool = CfgMgr.MuonTPVertexHelper("MuonTPVertexHelper")
        tool.VertexContainer = "MUON1JpsiCandidates"
        from AthenaCommon.Constants import INFO
        tool.OutputLevel=INFO
        ToolSvc += tool
    return getattr(ToolSvc, "MuonTPVertexHelper")

def GetJetCleaningTool(WorkingPoint):
    from JetSelectorTools.JetSelectorToolsConf import JetCleaningTool
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr
    if not hasattr(ToolSvc, "MyJetCleaningTool_%s"%WorkingPoint):
        tool = CfgMgr.JetCleaningTool("MyJetCleaningTool_%s"%WorkingPoint)
        tool.CutLevel = WorkingPoint
        tool.DoUgly   = False
        ToolSvc+=tool
    return getattr(ToolSvc, "MyJetCleaningTool_%s"%WorkingPoint)

def AddHighPtTrackPicker(cutval):
    from MuonPerformanceAlgs.MuonPerformanceAlgsConf import HighPtTrackPicker
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    if not hasattr(job,"HighPtTrackPicker_%.0f"%cutval):
        thealg = HighPtTrackPicker("HighPtTrackPicker_%.0f"%cutval)
        thealg.PtCut = cutval
        thealg.Input=("InDetTrackParticles")
        thealg.Output=("InDetTrackParticles_%.0fGeV"%cutval)
        thealg.IsolationDecorator = GetIDTrackIsolationDecoratorTool()
        job += thealg
    return getattr(job, "HighPtTrackPicker_%.0f"%cutval)

# find the muon container to run on
def GetRightMuonContainer():
    MuonContainerToUse = "Muons"
    if hasattr(job, "CalibratedMuonsProvider"):
        MuonContainerToUse = job.CalibratedMuonsProvider.Output
    #print "Running with Muon container"+MuonContainerToUse
    return MuonContainerToUse

# add a muon selection tool if there is none yet
def GetMuonSelectionTool(specialoptions=[]):

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.Constants import WARNING,ERROR,FATAL,INFO
    if not hasattr(ToolSvc,"TagProbeMuonSelectionTool"+"_".join(specialoptions)):
        from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
        tool = CP__MuonSelectionTool("TagProbeMuonSelectionTool"+"_".join(specialoptions))
        for option in specialoptions:
            setattr(tool,option,True)
        tool.OutputLevel=INFO
        # get rid of spam if we turn off certain ID cuts
        if len(specialoptions) > 0:
            tool.OutputLevel = ERROR
        tool.MaxEta = 99.  # don't cut on eta in this tool
        tool.ExpertDevelopMode=True  # Make the tool slightly less hysterical if we turn off cuts
        tool.OutputLevel=INFO
        ToolSvc+=tool
    return  getattr(ToolSvc,"TagProbeMuonSelectionTool"+"_".join(specialoptions))
# add a GRL tool if there is none yet
def AddGoodRunsListSelectionTool(GoodRunsListVec=["/afs/cern.ch/user/a/atlasdqm/grlgen/Atlas_Ready/data15_13TeV.periodAllYear_HEAD_DQDefects-00-01-02_PHYS_StandardGRL_Atlas_Ready.xml"]):

    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"MCPAnalysisGRLTool"):
        from AthenaCommon import CfgMgr
        ToolSvc += CfgMgr.GoodRunsListSelectionTool("MCPAnalysisGRLTool")
        ToolSvc.MCPAnalysisGRLTool.GoodRunsListVec=GoodRunsListVec

# Add an FSR tool
def GetFSRTool():
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"FSRTool"):
        from FsrUtils.FsrUtilsConf  import FSR__FsrPhotonTool
        FSRTool = FSR__FsrPhotonTool("FSRTool")
        FSRTool.IsolationSelectionTool = AddIsolationSelectionTool("Loose", "FixedCutTightCaloOnly")
        ToolSvc += FSRTool
    return ToolSvc.FSRTool


# Add a track deposit in calo tool
def GetTrackDepositInCaloTool():
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"TrkDepositInCaloTool"):
        from CaloTrkMuIdTools.CaloTrkMuIdToolsConf import TrackDepositInCaloTool
        TrkDepositInCaloTool = TrackDepositInCaloTool("TrkDepositInCaloTool")
        TrkDepositInCaloTool.CaloCellContainerName = "AODCellContainer"
        ToolSvc += TrkDepositInCaloTool
    return ToolSvc.TrkDepositInCaloTool

# Add a track isolation updater tool
def GetTrackIsolationTool():
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"TrackIsoTool"):
        from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
        TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
        TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
        TrackIsoTool.TrackSelectionTool.minPt= 1000.
        TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
        ToolSvc += TrackIsoTool
    return ToolSvc.TrackIsoTool
def GetCaloIsolationTool():
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"CaloIsoTool"):


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
        CaloIsoTool = xAOD__CaloIsolationTool("CaloIsoTool")
        CaloIsoTool.IsoLeakCorrectionTool = ToolSvc.NewLeakageCorrTool
        CaloIsoTool.ClustersInConeTool = ToolSvc.MyCaloClustersInConeTool
        CaloIsoTool.EMCaloNums  = [SUBCALO.LAREM]
        CaloIsoTool.HadCaloNums = [SUBCALO.LARHEC, SUBCALO.TILE]
        CaloIsoTool.UseEMScale  = True
        CaloIsoTool.UseCaloExtensionCaching = False
        CaloIsoTool.saveOnlyRequestedCorrections = True
        ToolSvc += CaloIsoTool

    return ToolSvc.CaloIsoTool


def GetIDTrackCaloDepositsDecorator():
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"IDTrackCaloDepositsDecoratorTool"):
        from MuonTPTools.MuonTPToolsConf import IDTrackCaloDepositsDecoratorTool
        DecoTool = IDTrackCaloDepositsDecoratorTool("IDTrackCaloDepositsDecoratorTool")
        if hasattr(DecoTool, "TrackDepositInCaloTool"):
            DecoTool.TrackDepositInCaloTool = GetTrackDepositInCaloTool()
        ToolSvc += DecoTool
    return ToolSvc.IDTrackCaloDepositsDecoratorTool

def GetIDTrackIsolationDecoratorTool():
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"IDTrackIsolationDecoratorTool"):
        from MuonTPTools.MuonTPToolsConf import IDTrackIsolationDecoratorTool
        DecoTool = IDTrackIsolationDecoratorTool("IDTrackIsolationDecoratorTool")
        if hasattr(DecoTool,"TrackIsolationTool") and hasattr(DecoTool,"CaloIsolationTool"):
            DecoTool.TrackIsolationTool = GetTrackIsolationTool()
            DecoTool.CaloIsolationTool = GetCaloIsolationTool()
#         from AthenaCommon.Constants import WARNING
#         DecoTool.OutputLevel = DEBUG
        ToolSvc += DecoTool
    return ToolSvc.IDTrackIsolationDecoratorTool

def AddTrackIsolationDecorAlg():
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    from AthenaCommon.AppMgr import ToolSvc
    from MuonPerformanceAlgs.MuonPerformanceAlgsConf import TrackIsolationDecorAlg
    job += TrackIsolationDecorAlg("TrackIsolationDecorator")
    job.TrackIsolationDecorator.TrackIsolationTool = GetTrackIsolationTool()
    job.TrackIsolationDecorator.PtMin = 10.


# Add the trigger decision tool
def GetTrigDecisionTool():
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc, "TrigDecisionTool"):
        from AthenaCommon import CfgMgr
        from AthenaCommon.Constants import WARNING, DEBUG, VERBOSE
        ToolSvc += CfgMgr.Trig__TrigDecisionTool( "TrigDecisionTool",
                                                  OutputLevel = WARNING,
                                                  TrigDecisionKey = "xTrigDecision" )
    return ToolSvc.TrigDecisionTool

def AddCalibratedMuonsProvider(outputCollection="CalibratedMuons"):
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    from AthenaCommon.AppMgr import ToolSvc
    from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__MuonCalibrationAndSmearingTool
    ToolSvc += CP__MuonCalibrationAndSmearingTool("MomentumCorrectionsTool")
    from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__CalibratedMuonsProvider
    from AthenaCommon.Constants import WARNING
    job += CP__CalibratedMuonsProvider("CalibratedMuonsProvider",
                                       Input="Muons",
                                       Output=outputCollection,
                                       Tool=ToolSvc.MomentumCorrectionsTool,OutputLevel=WARNING)


# Add the trigger matching tool
def GetTrigMatchTool():
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"TrigMuonMatching"):
        from TrigMuonMatching.TrigMuonMatchingConf import Trig__TrigMuonMatching
        trigmatch = Trig__TrigMuonMatching("TrigMuonMatching")
        ToolSvc += trigmatch
        #trigmatch.dRmin = 0.1
        #trigmatch.LVL1dRmin = 0.2
        trigmatch.TriggerTool = GetTrigDecisionTool()
    return ToolSvc.TrigMuonMatching
def GetTrigUtils():
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"TrigUtils"):
        from MuonTPTools.MuonTPToolsConf import MuonTPTrigUtils
        utils = MuonTPTrigUtils("TrigUtils")
        utils.TriggerDecisionTool = GetTrigDecisionTool()
        utils.TriggerMatchTool = GetTrigMatchTool()
        ToolSvc += utils
    return ToolSvc.TrigUtils
# Add instance of the TP plotting tool
def AddPlottingTool(name, EffiFlag, applySF=False, DoProbeMatchPlots=False, ProduceEfficiencies=False, IsRunOnDAOD=False):

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
    thetool.IsRunOnDAOD = IsRunOnDAOD
    thetool.ProduceProbeMatchPlots = DoProbeMatchPlots
    thetool.ProduceEfficiencies = ProduceEfficiencies
    thetool.ApplyScaleFactors = applySF
    ToolSvc+= thetool
    return thetool

# Add a tree filling tool
def AddTreeTool (name, EffiFlag,WriteSFInfo=False,IsRunOnDAOD=False,DoJpsiVertexInfo=False):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import DiMuonTPTreeTool
    thetool = DiMuonTPTreeTool(name)
    thetool.EfficiencyFlag = EffiFlag
    thetool.MuonSelectionTool = GetMuonSelectionTool()
    thetool.RecordScaleFactorInfo = WriteSFInfo
    thetool.JetCleaningTool = GetJetCleaningTool("TightBad")
    thetool.TriggerUtils = GetTrigUtils()
    thetool.FSRPhotonTool = GetFSRTool()
    thetool.IsRunOnDAOD = IsRunOnDAOD
    thetool.DoJpsiVertexInfo = DoJpsiVertexInfo
    thetool.BadMuonVetoHelper = GetBadMuonEventVetoHelper()
    thetool.VertexHelper =  GetVertexHelper()

    from AthenaCommon.GlobalFlags import globalflags
    thetool.AddTruthMatching = not globalflags.DataSource()=='data'
    from AthenaCommon.Constants import DEBUG
#     thetool.OutputLevel = DEBUG
    ToolSvc+= thetool
    return thetool

# note: The photon WP is used for the FSR recovery. Usually it is not needed.
def AddIsolationSelectionTool (WorkingPoint="Loose", PhotonWP = "Cone20"):
    from AthenaCommon.AppMgr import ToolSvc
    from IsolationSelection.IsolationSelectionConf import CP__IsolationSelectionTool
    if not hasattr(ToolSvc, "IsolationSelectionTool_%s_phot%s"%(WorkingPoint,PhotonWP)):
        isotool = CP__IsolationSelectionTool("IsolationSelectionTool_%s_phot%s"%(WorkingPoint,PhotonWP))
        isotool.MuonWP = WorkingPoint
        isotool.PhotonWP = PhotonWP
        ToolSvc += isotool
    return getattr(ToolSvc,  "IsolationSelectionTool_%s_phot%s"%(WorkingPoint,PhotonWP))

def AddMuonEfficiencyScaleFactors (WorkingPoint="Medium", doClosure=False):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors
    if not hasattr(ToolSvc, "EffiScaleFactors_%s"%WorkingPoint):
        sf = CP__MuonEfficiencyScaleFactors("EffiScaleFactors_%s"%WorkingPoint)
        sf.WorkingPoint = WorkingPoint
        import os
        if (doClosure):
            sf.CustomInputFolder = os.environ["TestArea"]+"/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceAlgs/scripts/ScaleFactors"
        ToolSvc += sf
    return getattr(ToolSvc,  "EffiScaleFactors_%s"%WorkingPoint)

def AddMuonTriggerScaleFactors (doClosure=False):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonTriggerScaleFactors
    if not hasattr(ToolSvc, "TriggerScaleFactors"):
        sf = CP__MuonTriggerScaleFactors("TriggerScaleFactors")
        #sf.runNumber = 900000
        import os
        if (doClosure):
            sf.CustomInputFolder = os.environ["TestArea"]+"/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceAlgs/scripts/ScaleFactors"
        ToolSvc += sf
    return getattr(ToolSvc,  "TriggerScaleFactors")

# Add a TP selection tool
def AddZmumuTPSelectionTool (name, EffiFlag, ProbeType, SameSign=False, AntiIso=False, UseTrigger=True, forTriggerEff=False,IsRunOnDAOD=False):

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.Constants import DEBUG
    from MuonTPTools.MuonTPToolsConf import DiMuonTPSelectionTool
    thetool = DiMuonTPSelectionTool(name)
    thetool.IsNotIncludedInNominal = False
    thetool.IsRunOnDAOD=IsRunOnDAOD

    # Try to add the isolation updater tools (only works in full athena!)
    #if hasattr(ToolSvc, "MyTrackIsoTool"):
        #thetool.TrackIsolationTool = ToolSvc.MyTrackIsoTool
        #thetool.CaloIsolationTool = ToolSvc.MyCaloIsoTool
    if hasattr(ToolSvc, "MCPAnalysisGRLTool"):
        thetool.GRLTool = ToolSvc.MCPAnalysisGRLTool
    # pt cuts
    thetool.TagPtCut = 24000.
    thetool.ProbePtCut = 4000.
#     thetool.ProbePtCut = 10000.
    # Z window
    thetool.HighMassWindow = 121000.
    thetool.LowMassWindow = 61000.
    #thetool.OutputLevel=DEBUG      # this is a bit spammy


    # Tag isolation
    if not AntiIso:
        thetool.DoTagIsolation = True
        thetool.TagIsolationTool = AddIsolationSelectionTool("Loose")


    # Probe isolation: disabled by default - turned on below
    thetool.DoProbeIsolation = False

    # Tag IP cuts
    thetool.TagD0Cut = -1
    thetool.TagD0SignCut = 3
    thetool.TagZ0Cut = 10

    # Probe IP cuts
    thetool.ProbeD0Cut = -1
    thetool.ProbeD0SignCut = 3
    thetool.ProbeZ0Cut = 10

    # skip very slow part that is only used in jpsi
    thetool.SkipTriggerPivotPlaneDr = True

    thetool.EfficiencyFlag = EffiFlag
    # tools
    thetool.SelectionTool = GetMuonSelectionTool()
    thetool.TriggerUtils = GetTrigUtils()
    thetool.IDTrackIsoDecorator = GetIDTrackIsolationDecoratorTool()
    thetool.IDTrackCaloDepositsDecorator = GetIDTrackCaloDepositsDecorator()
    # trigger(s)
    if UseTrigger:

        thetool.TagTriggerList = [
                                  "HLT_mu20_iloose_L1MU15",
                                  "HLT_mu20_ivarloose_L1MU15",
                                  "HLT_mu24_iloose_L1MU15",
                                  "HLT_mu24_ivarloose_L1MU15",
                                  "HLT_mu24_iloose",
                                  "HLT_mu24_imedium",
                                  "HLT_mu24_ivarloose",
                                  "HLT_mu24_ivarmedium",
                                  "HLT_mu26_imedium",
                                  "HLT_mu26_ivarmedium",
                                  "HLT_mu28_imedium",
                                  "HLT_mu28_ivarmedium",
                                  ]
        thetool.TagTriggerListRerunMode = []

        # Please avoid adding dilepton triggers to the default selection - this biases the reco SF.
        if forTriggerEff:
            for trigs in TriggerLists.GetTriggerList_Zmumu_RerunMode().itervalues():
                thetool.TagTriggerListRerunMode += trigs

            thetool.ObserverTriggerListL1  += TriggerLists.GetTriggerList_Zmumu_Observer()['L1']
            thetool.ObserverTriggerListHLT += TriggerLists.GetTriggerList_Zmumu_Observer()['HLT']

    if ProbeType == "ID":
        thetool.UseIDProbes=True
        thetool.ProbeIDHitCut=True
        thetool.DeltaPhiCut=-1
        #thetool.ProbeTrackIsoCut = -1
        if AntiIso:
            thetool.DoProbeAntiIsolation=True
            thetool.ProbeIsolationTool =  AddIsolationSelectionTool("LooseTrackOnly")
            thetool.HighMassWindow = 121000.
            thetool.LowMassWindow = 61000.
            thetool.ProbeZ0Cut = -1
            thetool.ProbeD0SignCut =-1
            thetool.ProbeD0Cut =-1
        else:
            thetool.DoProbeIsolation=True
            thetool.ProbeIsolationTool =  AddIsolationSelectionTool("Loose")

    if ProbeType == "MS":
        thetool.ProbeIDHitCut=False
        thetool.UseMSProbes = True
        thetool.DeltaPhiCut=-1
        thetool.ProbeZ0Cut = -1
        thetool.ProbeD0SignCut =-1
        thetool.ProbeD0Cut =-1
        if AntiIso:
            thetool.DoProbeAntiIsolation=True
            thetool.ProbeIsolationTool =  AddIsolationSelectionTool("LooseTrackOnly")
            thetool.HighMassWindow = 121000.
            thetool.LowMassWindow = 61000.
        else:
            thetool.DoProbeIsolation=True
            thetool.ProbeIsolationTool =  AddIsolationSelectionTool("Loose")

    if ProbeType == "CaloTag":
        thetool.UseCaloProbes = True
        thetool.ProbeIDHitCut=True
        thetool.DeltaPhiCut=-1
        if AntiIso:
            thetool.DoProbeAntiIsolation=True
            thetool.ProbeIsolationTool =  AddIsolationSelectionTool("LooseTrackOnly")
            thetool.HighMassWindow = 121000.
            thetool.LowMassWindow = 61000.
            thetool.ProbeZ0Cut = -1
            thetool.ProbeD0SignCut =-1
            thetool.ProbeD0Cut =-1
        else:
            thetool.DoProbeIsolation=True
            thetool.ProbeIsolationTool =  AddIsolationSelectionTool("Loose")

    if ProbeType == "Truth":
        thetool.UseTruthProbes = True
    if ProbeType == "VeryLoose":
        thetool.UseVeryLooseProbes = True
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
def AddJPsiTPSelectionTool (name, EffiFlag, ProbeType, SameSign=False, AntiIso=False, UseTrigger=True,forTriggerEff=False, DecorateProbeWithCaloDeposits=False,IsRunOnDAOD=False):

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.Constants import DEBUG
    from MuonTPTools.MuonTPToolsConf import DiMuonTPSelectionTool
    thetool = DiMuonTPSelectionTool(name)
    thetool.IsNotIncludedInNominal = False
    thetool.IsRunOnDAOD = IsRunOnDAOD
    thetool.DecorateProbeWithCaloDeposits = DecorateProbeWithCaloDeposits

    # Try to add the isolation updater tools (only works in full athena!)
    #if hasattr(ToolSvc, "MyTrackIsoTool"):
        #thetool.TrackIsolationTool = ToolSvc.MyTrackIsoTool
        #thetool.CaloIsolationTool = ToolSvc.MyCaloIsoTool
    if hasattr(ToolSvc, "MCPAnalysisGRLTool"):
        thetool.GRLTool = ToolSvc.MCPAnalysisGRLTool
    # pt cuts
    thetool.TagPtCut = 4000.
    thetool.ProbePtCut = 2500.
    # JPsi mass window
    thetool.HighMassWindow = 3500.
    thetool.LowMassWindow = 2700.
    #thetool.OutputLevel=DEBUG      # this is a bit spammy


    # Tag IP cuts
    thetool.TagD0Cut = -1.
    thetool.TagD0SignCut = -1.
    thetool.TagZ0Cut = -1.

    # Probe IP cuts
    thetool.ProbeD0Cut = -1.
    thetool.ProbeD0SignCut = -1.
    thetool.ProbeZ0Cut = -1.

    thetool.EfficiencyFlag = EffiFlag
    # tools
    thetool.SelectionTool = GetMuonSelectionTool()
    thetool.TriggerUtils = GetTrigUtils()
    thetool.IDTrackIsoDecorator = GetIDTrackIsolationDecoratorTool()
    thetool.IDTrackCaloDepositsDecorator = GetIDTrackCaloDepositsDecorator()
    # trigger(s)
    if UseTrigger:
        thetool.TagTriggerList = ["L1_MU6",
                                  "L1_MU4",
                                  "HLT_mu4",
                                  "HLT_mu6",
                                  "HLT_mu8",
                                  "HLT_mu10",
                                  "HLT_mu18",
                                  "HLT_mu6_idperf",
                                  "HLT_mu14",
                                  "HLT_mu20_iloose_L1MU15",
                                  "HLT_mu20_ivarloose_L1MU15",
                                  "HLT_mu24_iloose_L1MU15",
                                  "HLT_mu24_imedium",
                                  "HLT_mu24_ivarloose_L1MU15",
                                  "HLT_mu24_iloose",
                                  "HLT_mu24_ivarloose",
                                  "HLT_mu24_ivarmedium",
                                  "HLT_mu26_imedium",
                                  "HLT_mu26_ivarmedium",
                                  "HLT_mu28_imedium",
                                  "HLT_mu28_ivarmedium",
                                  ]
#         thetool.TagTriggerList = []
        thetool.TagTriggerListRerunMode = []
        if forTriggerEff:
            thetool.TagTriggerList += ["HLT_mu18_mu8noL1",
                                       "HLT_mu20_mu8noL1",
                                       "HLT_mu22_mu8noL1",
                                       "HLT_mu24_mu8noL1",
                                       "HLT_mu26_mu8noL1",
                                       ]
            thetool.TagTriggerListRerunMode += ["HLT_mu6","HLT_mu10","HLT_mu14","HLT_mu18","HLT_mu22","HLT_mu24"]
        if ProbeType == "ID" or ProbeType == "CaloTag" or ProbeType == "Loose" or ProbeType == "Medium" or ProbeType == "Tight":
            thetool.TagTriggerList += [
                                  "HLT_mu4_bJpsi_Trkloose",
                                  "HLT_mu6_bJpsi_Trkloose",
                                  "HLT_mu10_bJpsi_Trkloose",
                                  "HLT_mu18_bJpsi_Trkloose",]
        if ProbeType == "MS":
            thetool.TagTriggerList += ["HLT_mu6","HLT_mu8","HLT_mu14","HLT_mu4_mu4_idperf_bJpsimumu_noid"]
        if ProbeType == "VeryLoose" or ProbeType == "Loose" or ProbeType == "Medium" or ProbeType == "Tight":
            thetool.TagTriggerList += [
                                  "HLT_mu20_2mu0noL1_JpsimumuFS",
                                  "HLT_mu18_2mu0noL1_JpsimumuFS",
                                  "HLT_mu20_2mu4_JpsimumuL2",
                                  "HLT_mu18_2mu4_JpsimumuL2",
                                  "HLT_2mu6_bJpsimumu",
                                  "HLT_2mu6_bJpsimumu_Lxy0",
                                  "HLT_mu6_mu4_bJpsimumu",
                                  "HLT_mu6_mu4_bJpsimumu_Lxy0",
                                  "HLT_2mu10_bJpsimumu_noL2",
                                  ]

    thetool.DeltaPhiCut=-1.0
    thetool.DeltaEtaCut=-1.0

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
    if ProbeType == "VeryLoose":
        thetool.UseVeryLooseProbes = True
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

def AddHighPtSingleMuonSelectionTool (name, EffiFlag, ProbeType,IsRunOnDAOD=False):

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.Constants import DEBUG
    from MuonTPTools.MuonTPToolsConf import HighPtSingleMuonSelectionTool
    thetool = HighPtSingleMuonSelectionTool(name)
    thetool.IsNotIncludedInNominal = False
    thetool.IsRunOnDAOD = IsRunOnDAOD

    if hasattr(ToolSvc, "MCPAnalysisGRLTool"):
        thetool.GRLTool = ToolSvc.MCPAnalysisGRLTool
    # pt cuts
    thetool.ProbePtCut = 500000. # 500 GeV
    thetool.ProbeEtaCut = 2.7

    # Probe isolation: disabled by default - turned on below
    thetool.DoProbeIsolation = False

    # Probe IP cuts
    thetool.ProbeD0Cut = -1
    thetool.ProbeD0SignCut = -1
    thetool.ProbeZ0Cut = -1

    thetool.EfficiencyFlag = EffiFlag
    # tools
    thetool.SelectionTool = GetMuonSelectionTool()
    thetool.TriggerUtils = GetTrigUtils()
    thetool.IDTrackIsoDecorator = GetIDTrackIsolationDecoratorTool()
    thetool.IDTrackCaloDepositsDecorator = GetIDTrackCaloDepositsDecorator()

    if ProbeType == "ID":
        thetool.ProbeIDHitCut=True
        #thetool.ProbeTrackIsoCut = -1

    if ProbeType == "MSTrackPart":
        thetool.ProbeIDHitCut=False

    if ProbeType == "Muons":
        thetool.ProbeIDHitCut=False

    ToolSvc += thetool
    return thetool


# Add a reco efficiency tool
def AddMuonRecoTPEfficiencyTool (name, EffiFlag, MatchCont, doClosure=False, IsNominal = True):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import MuonRecoTPEfficiencyTool
    thetool = MuonRecoTPEfficiencyTool(name)
    thetool.IsNominal = IsNominal
    thetool.MuonAuthor = 21
    thetool.MatchContainerName = MatchCont
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
    thetool.SelectionTool = GetMuonSelectionTool()
    thetool.ApplyScaleFactors = doClosure and IsNominal
    thetool.TriggerUtils = GetTrigUtils()
    thetool.IsoSelectionTool = AddIsolationSelectionTool("LooseTrackOnly")
    thetool.IDTrackIsoDecorator = GetIDTrackIsolationDecoratorTool()
    if thetool.ApplyScaleFactors:
        thetool.TrigScaleFactorTool = AddMuonTriggerScaleFactors(doClosure=doClosure)
        thetool.IsolScaleFactorTool = AddMuonEfficiencyScaleFactors("LooseTrackOnlyIso",doClosure=doClosure)
    ToolSvc += thetool
    return thetool


# Add a trigger efficiency tool
def AddTriggerTPEfficiencyTool (name, EffiFlag,doClosure=False,IsNominal=True):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import MuonTrigTPEfficiencyTool
    thetool = MuonTrigTPEfficiencyTool(name)
    thetool.MuonAuthor       = 21
    thetool.MatchPtCut       = 5000.0         # speed up the loop a bit
    thetool.EfficiencyFlag   = EffiFlag
    thetool.SelectionTool    = GetMuonSelectionTool()
    thetool.IsNominal = IsNominal
    thetool.ApplyScaleFactors = doClosure and IsNominal
    thetool.IsoSelectionTool = AddIsolationSelectionTool("LooseTrackOnly")
    thetool.IDTrackIsoDecorator = GetIDTrackIsolationDecoratorTool()
    if thetool.ApplyScaleFactors:
        thetool.RecoScaleFactorTool = AddMuonEfficiencyScaleFactors("Medium",doClosure=doClosure)
        thetool.IsolScaleFactorTool = AddMuonEfficiencyScaleFactors("LooseTrackOnlyIso",doClosure=doClosure)
    thetool.TrigScaleFactorTool = AddMuonTriggerScaleFactors(doClosure=doClosure)
    thetool.TriggerUtils = GetTrigUtils()
    ToolSvc += thetool
    return thetool


# Add an iso efficiency tool
def AddIsolTPEfficiencyTool (name, EffiFlag,doClosure=False,IsNominal=True):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import MuonIsolTPEfficiencyTool
    thetool = MuonIsolTPEfficiencyTool(name)
    thetool.MuonAuthor       = 21
    thetool.MatchPtCut       = 5000.0         # speed up the loop a bit
    thetool.EfficiencyFlag   = EffiFlag
    thetool.SelectionTool    = GetMuonSelectionTool()
    thetool.IsNominal = IsNominal
    thetool.ApplyScaleFactors = doClosure and IsNominal
    thetool.TriggerUtils = GetTrigUtils()
    thetool.IDTrackIsoDecorator = GetIDTrackIsolationDecoratorTool()
    if thetool.ApplyScaleFactors:
        thetool.RecoScaleFactorTool = AddMuonEfficiencyScaleFactors("Medium",doClosure=doClosure)
        thetool.TrigScaleFactorTool = AddMuonTriggerScaleFactors(doClosure=doClosure)
    ToolSvc += thetool
    return thetool

# Add an instance of the 'main' TP tool
def AddMuonTPTool (name, EffiFlag):

    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import MuonTPTool
    thetool = MuonTPTool(name)
    thetool.EfficiencyFlag = EffiFlag
#     thetool.OutputLevel = DEBUG
    thetool.DumpInefficientEvents = False
    ToolSvc += thetool
    return thetool
