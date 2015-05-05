
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc

job = AlgSequence()

from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors


# add an instance of the TP Algorithm
def AddZmumuTPAlg(name, ProbeCont, MatchCont):

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
        
        from MuonPerformanceAlgs.MuonPerformanceAlgsConf import TrackIsolationDecorAlg

        job += TrackIsolationDecorAlg("MyIDTrackDecorator")
        job.MyIDTrackDecorator.TrackIsolationTool = ToolSvc.MyTrackIsoTool
        job.MyIDTrackDecorator.CaloIsolationTool = ToolSvc.MyCaloIsoTool

        
        #except AttributeError:
            #print "This did not work :("
            #sys.exit(1)
    
# Add the trigger decision tool
def AddTrigDecisionTool():
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
def AddZmumuTPPlotTool(name, EffiFlag, applySF=False):

    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import ZmumuMuonTPPlotTool
    thetool = ZmumuMuonTPPlotTool(name)
    thetool.EfficiencyFlag = EffiFlag
    thetool.doTrkValidPlots = False
    thetool.doBasicKinematicPlots = False
    thetool.doFineEtaPhiPlots = True
    thetool.doDetectorRegionPlots = True
    thetool.DoAsymmErrorGraphs = False
    thetool.doDileptonPlots = False
    thetool.ApplyScaleFactors = applySF
    ToolSvc+= thetool
    return thetool

# Add a tree filling tool
def AddZmumuTPTreeTool (name, EffiFlag):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import ZmumuMuonTPTreeTool
    thetool = ZmumuMuonTPTreeTool(name)
    thetool.EfficiencyFlag = EffiFlag
    ToolSvc+= thetool
    return thetool
    

# Add a TP selection tool
def AddZmumuTPSelectionTool (name, EffiFlag, ProbeType, SameSign=False, AntiIso=False):

    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import ZmumuMuonTPSelectionTool
    thetool = ZmumuMuonTPSelectionTool(name)
    
    # Try to add the isolation updater tools (only works in full athena!)
    if hasattr(ToolSvc, "MyTrackIsoTool"):
        thetool.TrackIsolationTool = ToolSvc.MyTrackIsoTool
        thetool.CaloIsolationTool = ToolSvc.MyCaloIsoTool
    
    # pt cuts
    thetool.TagPtCut = 25000.
    thetool.ProbePtCut = 10000.
    # Z window
    thetool.HighMassWindow = 101000.
    thetool.LowMassWindow = 81000.
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
    thetool.TagTriggerList = ["HLT_mu26_imedium"]

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


    if SameSign:
        thetool.AcceptOppCharge=False
        thetool.AcceptSameCharge=True

    ToolSvc += thetool
    return thetool

# Add an efficiency tool
def AddZmumuTPEfficiencyTool (name, EffiFlag):
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
    thetool.MatchPtCut = 5000.0         # speed up the loop a bit
    thetool.EfficiencyFlag = EffiFlag
    thetool.SelectionTool = ToolSvc.TagProbeMuonSelectionTool
    thetool.TriggerMatchTool = ToolSvc.TrigMuonMatching
    ToolSvc += thetool
    return thetool

# Add an instance of the 'main' TP tool
def AddZmumuTPTool (name, EffiFlag):

    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import MuonTPTool
    thetool = MuonTPTool(name)
    thetool.EfficiencyFlag = EffiFlag
    #thetool.OutputLevel = DEBUG
    thetool.DumpInefficientEvents = True
    ToolSvc += thetool
    return thetool

################################################################
# Add a version of the Z TP algorithm and all associated tools
################################################################
def AddConfiguredZmumuTPAlg(name_suffix="myProbes",
                            ProbeKind = "CaloTag", # options: "CaloTag","MS", "Truth", "ID"
                            MatchContainer="Muons",
                            doID=False,
                            doCB = True, doLoose=True,
                            doMedium=True,doTight=True,
                            doStandalone=False, doCaloTag=False,
                            doIndividualAuthors=False,
                            doEtaSlices=False, writeNtuple=False,
                            doClosure = False, doDRSys = False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = "Muons"
    #if hasattr(job, "MuonQualityUpdater"):
        #MuonContainerToUse = "UpdatedMuons"

    ProbeContainer=MuonContainerToUse

    if ProbeKind == "MSTrackPart":
        ProbeContainer = "ExtrapolatedMuonTrackParticles"
        ProbeKind = "MS"
        
    if ProbeKind == "ID":
        ProbeContainer = "InDetTrackParticles"
    if ProbeKind == "Truth":
        ProbeContainer = "MuonTruthParticles"

    theAlg = AddZmumuTPAlg(name="ZmumuTPAlg_%s"%name_suffix,
                           ProbeCont=ProbeContainer,
                           MatchCont=MatchContainer)

    AddMuonSelectionTool()


    # add a plotting tool
    #PlotTool = AddZmumuTPPlotTool(name="ZmumuTPPlottingTool_%s"%name_suffix,EffiFlag=name_suffix)

    # add four TP selection tools (iso and antiiso OC and SC)
    SelectionTools = []
    SelectionTools += [AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OC_%s"%name_suffix,EffiFlag="%s_OC"%name_suffix, ProbeType = ProbeKind)]
    SelectionTools += [AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_SC_%s"%name_suffix,EffiFlag="%s_SC"%name_suffix, ProbeType = ProbeKind, SameSign=True)]
    SelectionTools += [AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OCAntiIso_%s"%name_suffix,EffiFlag="%s_OCAntiIso"%name_suffix, ProbeType = ProbeKind, AntiIso=True)]
    SelectionTools += [AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_SCAntiIso_%s"%name_suffix,EffiFlag="%s_SCAntiIso"%name_suffix, ProbeType = ProbeKind, SameSign=True, AntiIso=True)]


    PlotTools = []
    # add plotting tools in slices
    if doEtaSlices:
        Slices = {
                   "All" : (0.0,10.0),
                   "noCrack" : (0.1,10.0),
                   "Crack" : (0.0,0.1),
                   "Barrel" : (0.1,1.1),
                   "Transition" : (1.1, 1.3),
                   "Endcap" : (1.3,2.0),
                   "CSC" : (2.0,2.7)
                   }

        for etaslice, interval in Slices.iteritems():
            PlotTool = AddZmumuTPPlotTool(name="ZmumuTPPlottingTool_%s_%s"%(name_suffix,etaslice),EffiFlag=name_suffix+"_"+etaslice,applySF = False)
            PlotTool.ProbeAbsEtaMin = interval[0]
            PlotTool.ProbeAbsEtaMax = interval[1]
            PlotTools.append(PlotTool)
            if doClosure:
                PlotToolCL = AddZmumuTPPlotTool(name="ZmumuTPPlottingToolClosure_%s_%s"%(name_suffix,etaslice),EffiFlag=name_suffix+"_"+etaslice+"_Closure",applySF = True)
                PlotToolCL.ProbeAbsEtaMin = interval[0]
                PlotToolCL.ProbeAbsEtaMax = interval[1]
                PlotTools.append(PlotToolCL)
                
    else:
        PlotTools = [AddZmumuTPPlotTool(name="ZmumuTPPlottingTool_%s_All"%name_suffix,EffiFlag=name_suffix+"_All",applySF = False)]
        if doClosure:
            PlotTools.append(AddZmumuTPPlotTool(name="ZmumuTPPlottingToolClosure_%s_All"%name_suffix,EffiFlag=name_suffix+"_All_Closure",applySF = True))
        

    # add any desired matching tools

    MATCH_DR = 0.05

    
    MatchingTools = []
    if doCB:
        thistoolCB = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CB"%name_suffix,EffiFlag="CombinedMuons")
        thistoolCB.MatchToCB = True
        if not hasattr(ToolSvc, "EffiScaleFactors_CBMuons"):
            ToolSvc+= CP__MuonEfficiencyScaleFactors("EffiScaleFactors_CBMuons")
            ToolSvc.EffiScaleFactors_CBMuons.WorkingPoint = "CB"
            ToolSvc.EffiScaleFactors_CBMuons.DataPeriod = "2012"
        thistoolCB.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
        thistoolCB.ApplyScaleFactors = True
        thistoolCB.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolCB]
        if doDRSys:
            thistoolCB_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CB_dRDown"%name_suffix,EffiFlag="CombinedMuons_dRDown")
            thistoolCB_dRDown.MatchToCB = True
            thistoolCB_dRDown.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
            thistoolCB_dRDown.ApplyScaleFactors = True
            thistoolCB_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolCB_dRDown]
            
            thistoolCB_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CB_dRUp"%name_suffix,EffiFlag="CombinedMuons_dRUp")
            thistoolCB_dRUp.MatchToCB = True
            thistoolCB_dRUp.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
            thistoolCB_dRUp.ApplyScaleFactors = True
            thistoolCB_dRUp.MaximumDrCut = 2. * MATCH_DR
            MatchingTools += [thistoolCB_dRUp]

    if doLoose:
        thistoolLoose = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Loose"%name_suffix,EffiFlag="LooseMuons")
        thistoolLoose.MatchToLoose = True
        if not hasattr(ToolSvc, "EffiScaleFactors_CBMuons"):
            ToolSvc+= CP__MuonEfficiencyScaleFactors("EffiScaleFactors_CBMuons")
            ToolSvc.EffiScaleFactors_CBMuons.WorkingPoint = "CB"
            ToolSvc.EffiScaleFactors_CBMuons.DataPeriod = "2012"
        thistoolLoose.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
        thistoolLoose.ApplyScaleFactors = True
        thistoolLoose.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolLoose]
        if doDRSys:
            thistoolLoose_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Loose_dRDown"%name_suffix,EffiFlag="LooseMuons_dRDown")
            thistoolLoose_dRDown.MatchToLoose = True
            thistoolLoose_dRDown.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
            thistoolLoose_dRDown.ApplyScaleFactors = True
            thistoolLoose_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolLoose_dRDown]
            
            thistoolLoose_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Loose_dRUp"%name_suffix,EffiFlag="LooseMuons_dRUp")
            thistoolLoose_dRUp.MatchToLoose = True
            thistoolLoose_dRUp.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
            thistoolLoose_dRUp.ApplyScaleFactors = True
            thistoolLoose_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolLoose_dRUp]
            
        # also run a version without CaloTag
        thistoolLoose_noCaloTag = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Loose_noCaloTag"%name_suffix,EffiFlag="LooseMuons_noCaloTag")
        thistoolLoose_noCaloTag.MatchToLoose_noCaloTag = True
        if not hasattr(ToolSvc, "EffiScaleFactors_CBMuons"):
            ToolSvc+= CP__MuonEfficiencyScaleFactors("EffiScaleFactors_CBMuons")
            ToolSvc.EffiScaleFactors_CBMuons.WorkingPoint = "CB"
            ToolSvc.EffiScaleFactors_CBMuons.DataPeriod = "2012"
        thistoolLoose_noCaloTag.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
        thistoolLoose_noCaloTag.ApplyScaleFactors = True
        thistoolLoose_noCaloTag.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolLoose_noCaloTag]
        if doDRSys:
            thistoolLoose_noCaloTag_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Loose_noCaloTag_dRDown"%name_suffix,EffiFlag="LooseMuons_noCaloTag_dRDown")
            thistoolLoose_noCaloTag_dRDown.MatchToLoose_noCaloTag = True
            thistoolLoose_noCaloTag_dRDown.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
            thistoolLoose_noCaloTag_dRDown.ApplyScaleFactors = True
            thistoolLoose_noCaloTag_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolLoose_noCaloTag_dRDown]
            
            thistoolLoose_noCaloTag_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Loose_noCaloTag_dRUp"%name_suffix,EffiFlag="LooseMuons_noCaloTag_dRUp")
            thistoolLoose_noCaloTag_dRUp.MatchToLoose_noCaloTag = True
            thistoolLoose_noCaloTag_dRUp.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
            thistoolLoose_noCaloTag_dRUp.ApplyScaleFactors = True
            thistoolLoose_noCaloTag_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolLoose_noCaloTag_dRUp]
        
    if doMedium:
        thistoolMed = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Medium"%name_suffix,EffiFlag="MediumMuons")
        thistoolMed.MatchToMedium = True
        if not hasattr(ToolSvc, "EffiScaleFactors_CBMuons"):
            ToolSvc+= CP__MuonEfficiencyScaleFactors("EffiScaleFactors_CBMuons")
            ToolSvc.EffiScaleFactors_CBMuons.WorkingPoint = "CB"
            ToolSvc.EffiScaleFactors_CBMuons.DataPeriod = "2012"
        thistoolMed.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
        thistoolMed.ApplyScaleFactors = True
        thistoolMed.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMed]
        if doDRSys:
            thistoolMed_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Medium_dRDown"%name_suffix,EffiFlag="MediumMuons_dRDown")
            thistoolMed_dRDown.MatchToMedium = True
            thistoolMed_dRDown.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
            thistoolMed_dRDown.ApplyScaleFactors = True
            thistoolMed_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMed_dRDown]

            thistoolMed_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Medium_dRUp"%name_suffix,EffiFlag="MediumMuons_dRUp")
            thistoolMed_dRUp.MatchToMedium = True
            thistoolMed_dRUp.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
            thistoolMed_dRUp.ApplyScaleFactors = True
            thistoolMed_dRUp.MaximumDrCut = 0.010
            MatchingTools += [thistoolMed_dRUp]

        
    if doTight:
        thistoolTight = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Tight"%name_suffix,EffiFlag="TightMuons")
        thistoolTight.MatchToTight = True
        if not hasattr(ToolSvc, "EffiScaleFactors_CBMuons"):
            ToolSvc+= CP__MuonEfficiencyScaleFactors("EffiScaleFactors_CBMuons")
            ToolSvc.EffiScaleFactors_CBMuons.WorkingPoint = "CB"
            ToolSvc.EffiScaleFactors_CBMuons.DataPeriod = "2012"
        thistoolTight.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
        thistoolTight.ApplyScaleFactors = True
        thistoolTight.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolTight]
        if doDRSys:
            thistoolTight_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Tight_dRDown"%name_suffix,EffiFlag="TightMuons_dRDown")
            thistoolTight_dRDown.MatchToTight = True
            thistoolTight_dRDown.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
            thistoolTight_dRDown.ApplyScaleFactors = True
            thistoolTight_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolTight_dRDown]
            
            thistoolTight_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Tight_dRUp"%name_suffix,EffiFlag="TightMuons_dRUp")
            thistoolTight_dRUp.MatchToTight = True
            thistoolTight_dRUp.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
            thistoolTight_dRUp.ApplyScaleFactors = True
            thistoolTight_dRUp.MaximumDrCut = 0.010
            MatchingTools += [thistoolTight_dRUp]
            

    if doStandalone:
        thistoolSA = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_SA"%name_suffix,EffiFlag="StandaloneMuons")
        thistoolSA.MatchToAnyMS = True
        thistoolSA.IDhitCut = False
        thistoolSA.ApplyScaleFactors = False
        thistoolSA.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolSA]
        if doDRSys:
            thistoolSA_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_SA_dRDown"%name_suffix,EffiFlag="StandaloneMuons_dRDown")
            thistoolSA_dRDown.MatchToAnyMS = True
            thistoolSA_dRDown.IDhitCut = False
            thistoolSA_dRDown.ApplyScaleFactors = False
            thistoolSA_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolSA_dRDown]

            thistoolSA_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_SA_dRUp"%name_suffix,EffiFlag="StandaloneMuons_dRUp")
            thistoolSA_dRUp.MatchToAnyMS = True
            thistoolSA_dRUp.IDhitCut = False
            thistoolSA_dRUp.ApplyScaleFactors = False
            thistoolSA_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolSA_dRUp]

    if doCaloTag:
        thistoolCT = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CT"%name_suffix,EffiFlag="CaloTaggedMuons")
        thistoolCT.MatchToCaloTag = True
        if not hasattr(ToolSvc, "EffiScaleFactors_CBMuons"):
            ToolSvc+= CP__MuonEfficiencyScaleFactors("EffiScaleFactors_CBMuons")
            ToolSvc.EffiScaleFactors_CBMuons.WorkingPoint = "CB"
            ToolSvc.EffiScaleFactors_CBMuons.DataPeriod = "2012"
        thistoolCT.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
        thistoolCT.ApplyScaleFactors = True
        thistoolCT.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolCT]
        if doDRSys:
            thistoolCT_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CT_dRDown"%name_suffix,EffiFlag="CaloTaggedMuons_dRDown")
            thistoolCT_dRDown.MatchToCaloTag = True
            thistoolCT_dRDown.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
            thistoolCT_dRDown.ApplyScaleFactors = True
            thistoolCT_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolCT_dRDown]

            thistoolCT_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CT_dRUp"%name_suffix,EffiFlag="CaloTaggedMuons_dRUp")
            thistoolCT_dRUp.MatchToCaloTag = True
            thistoolCT_dRUp.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
            thistoolCT_dRUp.ApplyScaleFactors = True
            thistoolCT_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolCT_dRUp]

    if doID:
        thistoolID = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID"%name_suffix,EffiFlag="IDTracks")
        thistoolID.MatchToID = True
        thistoolID.ApplyScaleFactors = False
        thistoolID.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolID]
        if doDRSys:
            thistoolID_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID_dRUp"%name_suffix,EffiFlag="IDTracks_dRUp")
            thistoolID_dRUp.MatchToID = True
            thistoolID_dRUp.ApplyScaleFactors = False
            thistoolID_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolID_dRUp]

            thistoolID_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID_dRDown"%name_suffix,EffiFlag="IDTracks_dRDown")
            thistoolID_dRDown.MatchToID = True
            thistoolID_dRDown.ApplyScaleFactors = False
            thistoolID_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolID_dRDown]

        thistoolID_noMCPcuts = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID_noMCP"%name_suffix,EffiFlag="IDTracks_noMCP")
        thistoolID_noMCPcuts.MatchToID = True
        thistoolID_noMCPcuts.IDhitCut = False
        thistoolID_noMCPcuts.ApplyScaleFactors = False
        thistoolID_noMCPcuts.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolID_noMCPcuts]
        if doDRSys:
            thistoolID_noMCPcuts_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID_noMCP_dRUp"%name_suffix,EffiFlag="IDTracks_noMCP_dRUp")
            thistoolID_noMCPcuts_dRUp.MatchToID = True
            thistoolID_noMCPcuts_dRUp.ApplyScaleFactors = False
            thistoolID_noMCPcuts_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolID_noMCPcuts_dRUp]

            thistoolID_noMCPcuts_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID_noMCP_dRDown"%name_suffix,EffiFlag="IDTracks_noMCP_dRDown")
            thistoolID_noMCPcuts_dRDown.MatchToID = True
            thistoolID_noMCPcuts_dRDown.ApplyScaleFactors = False
            thistoolID_noMCPcuts_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolID_noMCPcuts_dRDown]
            


    if doIndividualAuthors:
        thistoolMuidCB = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidCB"%name_suffix,EffiFlag="MuidCB")
        thistoolMuidCB.MatchToMuidCB = True
        thistoolMuidCB.ApplyScaleFactors = False
        thistoolMuidCB.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuidCB]
        if doDRSys:
            thistoolMuidCB_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidCB_dRDown"%name_suffix,EffiFlag="MuidCB_dRDown")
            thistoolMuidCB_dRDown.MatchToMuidCB = True
            thistoolMuidCB_dRDown.ApplyScaleFactors = False
            thistoolMuidCB_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuidCB_dRDown]

            thistoolMuidCB_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidCB_dRUp"%name_suffix,EffiFlag="MuidCB_dRUp")
            thistoolMuidCB_dRUp.MatchToMuidCB = True
            thistoolMuidCB_dRUp.ApplyScaleFactors = False
            thistoolMuidCB_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuidCB_dRUp]

        thistoolSTACO = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_STACO"%name_suffix,EffiFlag="STACO")
        thistoolSTACO.MatchToSTACO = True
        thistoolSTACO.ApplyScaleFactors = False
        thistoolSTACO.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolSTACO]
        if doDRSys:
            thistoolSTACO_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_STACO_dRDown"%name_suffix,EffiFlag="STACO_dRDown")
            thistoolSTACO_dRDown.MatchToSTACO = True
            thistoolSTACO_dRDown.ApplyScaleFactors = False
            thistoolSTACO_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolSTACO_dRDown]

            thistoolSTACO_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_STACO_dRUp"%name_suffix,EffiFlag="STACO_dRUp")
            thistoolSTACO_dRUp.MatchToSTACO = True
            thistoolSTACO_dRUp.ApplyScaleFactors = False
            thistoolSTACO_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolSTACO_dRUp]

        thistoolMuTag = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuTag"%name_suffix,EffiFlag="MuTag")
        thistoolMuTag.MatchToMuTag = True
        thistoolMuTag.ApplyScaleFactors = False
        thistoolMuTag.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuTag]
        if doDRSys:
            thistoolMuTag_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuTag_dRDown"%name_suffix,EffiFlag="MuTag_dRDown")
            thistoolMuTag_dRDown.MatchToMuTag = True
            thistoolMuTag_dRDown.ApplyScaleFactors = False
            thistoolMuTag_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuTag_dRDown]

            thistoolMuTag_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuTag_dRUp"%name_suffix,EffiFlag="MuTag_dRUp")
            thistoolMuTag_dRUp.MatchToMuTag = True
            thistoolMuTag_dRUp.ApplyScaleFactors = False
            thistoolMuTag_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuTag_dRUp]

        thistoolMuTagIMO = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuTagIMO"%name_suffix,EffiFlag="MuTagIMO")
        thistoolMuTagIMO.MatchToMuTagIMO = True
        thistoolMuTagIMO.ApplyScaleFactors = False
        thistoolMuTagIMO.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuTagIMO]
        if doDRSys:
            thistoolMuTagIMO_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuTagIMO_dRDown"%name_suffix,EffiFlag="MuTagIMO_dRDown")
            thistoolMuTagIMO_dRDown.MatchToMuTagIMO = True
            thistoolMuTagIMO_dRDown.ApplyScaleFactors = False
            thistoolMuTagIMO_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuTagIMO_dRDown]

            thistoolMuTagIMO_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuTagIMO_dRUp"%name_suffix,EffiFlag="MuTagIMO_dRUp")
            thistoolMuTagIMO_dRUp.MatchToMuTagIMO = True
            thistoolMuTagIMO_dRUp.ApplyScaleFactors = False
            thistoolMuTagIMO_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuTagIMO_dRUp]

        thistoolMuidSA = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidSA"%name_suffix,EffiFlag="MuidSA")
        thistoolMuidSA.MatchToMuidSA = True
        thistoolMuidSA.ApplyScaleFactors = False
        thistoolMuidSA.IDhitCut = False
        thistoolMuidSA.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuidSA]
        if doDRSys:
            thistoolMuidSA_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidSA_dRDown"%name_suffix,EffiFlag="MuidSA_dRDown")
            thistoolMuidSA_dRDown.MatchToMuidSA = True
            thistoolMuidSA_dRDown.ApplyScaleFactors = False
            thistoolMuidSA_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuidSA_dRDown]

            thistoolMuidSA_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidSA_dRUp"%name_suffix,EffiFlag="MuidSA_dRUp")
            thistoolMuidSA_dRUp.MatchToMuidSA = True
            thistoolMuidSA_dRUp.ApplyScaleFactors = False
            thistoolMuidSA_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuidSA_dRUp]

        thistoolMuGirl = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidGirl"%name_suffix,EffiFlag="MuGirl")
        thistoolMuGirl.MatchToMuGirl = True
        thistoolMuGirl.ApplyScaleFactors = False
        thistoolMuGirl.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuGirl]
        if doDRSys:
            thistoolMuGirl_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidGirl_dRDown"%name_suffix,EffiFlag="MuGirl_dRDown")
            thistoolMuGirl_dRDown.MatchToMuGirl = True
            thistoolMuGirl_dRDown.ApplyScaleFactors = False
            thistoolMuGirl_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuGirl_dRDown]

            thistoolMuGirl_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidGirl_dRUp"%name_suffix,EffiFlag="MuGirl_dRUp")
            thistoolMuGirl_dRUp.MatchToMuGirl = True
            thistoolMuGirl_dRUp.ApplyScaleFactors = False
            thistoolMuGirl_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuGirl_dRUp]

        #thistoolMuGirlLowBeta = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidGirlLowBeta"%name_suffix,EffiFlag="MuGirlLowBeta")
        #thistoolMuGirlLowBeta.MatchToMuGirlLowBeta = True
        #thistoolMuGirlLowBeta.ApplyScaleFactors = False
        #thistoolMuGirlLowBeta.MaximumDrCut = MATCH_DR
        #MatchingTools += [thistoolMuGirlLowBeta]
        #if doDRSys:
            #thistoolMuGirlLowBeta_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidGirlLowBeta_dRDown"%name_suffix,EffiFlag="MuGirlLowBeta_dRDown")
            #thistoolMuGirlLowBeta_dRDown.MatchToMuGirlLowBeta = True
            #thistoolMuGirlLowBeta_dRDown.ApplyScaleFactors = False
            #thistoolMuGirlLowBeta_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            #MatchingTools += [thistoolMuGirlLowBeta_dRDown]

            #thistoolMuGirlLowBeta_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidGirlLowBeta_dRUp"%name_suffix,EffiFlag="MuGirlLowBeta_dRUp")
            #thistoolMuGirlLowBeta_dRUp.MatchToMuGirlLowBeta = True
            #thistoolMuGirlLowBeta_dRUp.ApplyScaleFactors = False
            #thistoolMuGirlLowBeta_dRUp.MaximumDrCut = 2 * MATCH_DR
            #MatchingTools += [thistoolMuGirlLowBeta_dRUp]

        #thistoolCaloLikelihood = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CaloLR"%name_suffix,EffiFlag="CaloLikelihood")
        #thistoolCaloLikelihood.MatchToCaloLikelihood = True
        #thistoolCaloLikelihood.ApplyScaleFactors = False
        #thistoolCaloLikelihood.MaximumDrCut = MATCH_DR
        #MatchingTools += [thistoolCaloLikelihood]
        #if doDRSys:
            #thistoolCaloLikelihood_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CaloLR_dRDown"%name_suffix,EffiFlag="CaloLikelihood_dRDown")
            #thistoolCaloLikelihood_dRDown.MatchToCaloLikelihood = True
            #thistoolCaloLikelihood_dRDown.ApplyScaleFactors = False
            #thistoolCaloLikelihood_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            #MatchingTools += [thistoolCaloLikelihood_dRDown]

            #thistoolCaloLikelihood_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CaloLR_dRUp"%name_suffix,EffiFlag="CaloLikelihood_dRUp")
            #thistoolCaloLikelihood_dRUp.MatchToCaloLikelihood = True
            #thistoolCaloLikelihood_dRUp.ApplyScaleFactors = False
            #thistoolCaloLikelihood_dRUp.MaximumDrCut = 2 * MATCH_DR
            #MatchingTools += [thistoolCaloLikelihood_dRUp]

        #thistoolExtrapolateToIP = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ExtrapIP"%name_suffix,EffiFlag="ExtrapolateToIP")
        #thistoolExtrapolateToIP.MatchToExtrapolateToIP = True
        #thistoolExtrapolateToIP.ApplyScaleFactors = False
        #thistoolExtrapolateToIP.IDhitCut = False
        #thistoolExtrapolateToIP.MaximumDrCut = MATCH_DR
        #MatchingTools += [thistoolExtrapolateToIP]
        #if doDRSys:
            #thistoolExtrapolateToIP_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ExtrapIP_dRDown"%name_suffix,EffiFlag="ExtrapolateToIP_dRDown")
            #thistoolExtrapolateToIP_dRDown.MatchToExtrapolateToIP = True
            #thistoolExtrapolateToIP_dRDown.ApplyScaleFactors = False
            #thistoolExtrapolateToIP_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            #MatchingTools += [thistoolExtrapolateToIP_dRDown]

            #thistoolExtrapolateToIP_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ExtrapIP_dRUp"%name_suffix,EffiFlag="ExtrapolateToIP_dRUp")
            #thistoolExtrapolateToIP_dRUp.MatchToExtrapolateToIP = True
            #thistoolExtrapolateToIP_dRUp.ApplyScaleFactors = False
            #thistoolExtrapolateToIP_dRUp.MaximumDrCut = 2 * MATCH_DR
            #MatchingTools += [thistoolExtrapolateToIP_dRUp]
            
            
    ntuples = []
    if writeNtuple:
        ntuples.append(AddZmumuTPTreeTool(name="ZmumuTPTreeTool_%s"%name_suffix, EffiFlag="Trees"))

    # create the TP tool itself
    TheTPTool = AddZmumuTPTool(name = "ZmumuTPTool_%s"%name_suffix,EffiFlag=name_suffix)
    TheTPTool.MuonTPSelectionTools = SelectionTools
    TheTPTool.PlottingTools = PlotTools
    TheTPTool.MuonTPEfficiencyTools = MatchingTools
    TheTPTool.TreeTools = ntuples
    theAlg.MuonTPTools += [TheTPTool]


def AddZmumuTPAnalysis(doEtaSlices=False,writeNtuple=False,doClosure=False,doDRSys=False, doValid=False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = "Muons"
    #if hasattr(job, "MuonQualityUpdater"):
        #MuonContainerToUse = "UpdatedMuons"


    ##########################################################################################
    # Add the Zmm TP algorithm using ID tracks as probes and matching to muons
    AddConfiguredZmumuTPAlg(name_suffix = "IDProbes",
                            ProbeKind = "ID",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doCB = True, doLoose=True, doMedium = True,
                            doTight=False, doStandalone = False, doCaloTag = True, doIndividualAuthors = doValid,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys)


    #########################################################################################
    #Add the Zmm TP algorithm using CaloTag muons as probes and matching to muons
    AddConfiguredZmumuTPAlg(name_suffix = "CaloProbes",
                            ProbeKind = "CaloTag",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doCB = True, doLoose=True, doMedium = True,
                            doTight=False,
                            doStandalone = False, doCaloTag = False, doIndividualAuthors = doValid,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys)


    ##########################################################################################
    # Add the Zmm TP algorithm using MS muons as probes and matching to ID tracks
   # AddConfiguredZmumuTPAlg(name_suffix = "MSTrackPartProbes_ID",
   #                         ProbeKind = "MSTrackPart",
   #                         MatchContainer = "InDetTrackParticles",
   #                         doID = True, doCB = False, doLoose=False, doMedium = False,
   #                         doTight=False,
   #                         doStandalone = False, doCaloTag = False,
   #                         doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys)


    ##########################################################################################
    # Add the Zmm TP algorithm using MS muons as probes and matching to ID tracks
    AddConfiguredZmumuTPAlg(name_suffix = "MSProbes_ID",
                            ProbeKind = "MS",
                            MatchContainer = "InDetTrackParticles",
                            doID = True, doCB = False, doLoose=False, doMedium = False,
                            doTight=False,
                            doStandalone = False, doCaloTag = False, doIndividualAuthors = False,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys)


    ##########################################################################################
    # Add the Zmm TP algorithm using MS muons as probes and matching to CaloTag muons
    AddConfiguredZmumuTPAlg(name_suffix = "MSProbes_Muon",
                            ProbeKind = "MS",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doCB = False, doLoose=False, doMedium = False,
                            doTight=False,
                            doStandalone = False, doCaloTag = True, doIndividualAuthors = False,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys)

    ##########################################################################################
    # Add the Zmm TP algorithm using Truth probes for Muons
    AddConfiguredZmumuTPAlg(name_suffix = "TruthProbes_Muon",
                            ProbeKind = "Truth",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doCB = True, doLoose=True, doMedium = True,
                            doTight=False,
                            doStandalone = False, doCaloTag = True, doIndividualAuthors = doValid,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys)

    ##########################################################################################
    # Add the Zmm TP algorithm using Truth probes for Tracks
    AddConfiguredZmumuTPAlg(name_suffix = "TruthProbes_ID",
                            ProbeKind = "Truth",
                            MatchContainer = "InDetTrackParticles",
                            doID = True, doCB = False, doLoose=False, doMedium = False,
                            doTight=False,
                            doStandalone = False, doCaloTag = False, doIndividualAuthors = False,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys)
