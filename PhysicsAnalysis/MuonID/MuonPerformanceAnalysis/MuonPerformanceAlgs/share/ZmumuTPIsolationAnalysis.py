

# Add instance of the TP plotting tool
def AddZmumuTPIsolationPlotTool(name, EffiFlag, applySF=False):

    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import ZmumuMuonTPPlotTool
    thetool = ZmumuMuonTPPlotTool(name)
    thetool.EfficiencyFlag = EffiFlag
    thetool.doTrkValidPlots = False
    thetool.doBasicKinematicPlots = True
    thetool.doFineEtaPhiPlots = True
    thetool.DoAsymmErrorGraphs = True
    thetool.doDileptonPlots = True
    thetool.ApplyScaleFactors = applySF
    ToolSvc+= thetool
    return thetool

# Add a tree filling tool
def AddZmumuTPIsolationTreeTool (name, EffiFlag):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import ZmumuMuonTPIsolationTreeTool
    thetool = ZmumuMuonTPIsolationTreeTool(name)
    thetool.EfficiencyFlag = EffiFlag
    ToolSvc+= thetool
    return thetool
    

# Add a TP selection tool
def AddZmumuTPIsolationSelectionTool (name, EffiFlag, ProbeType, SameSign=False, AntiIso=False):

    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import ZmumuMuonTPSelectionTool
    thetool = ZmumuMuonTPSelectionTool(name)
    
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
    
    # Probe isolation: disabled for isolation studies
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

    if ProbeType == "Truth":
        thetool.UseTruthProbes = True
    if ProbeType == "CB":
        thetool.UseCBProbes = True

    if SameSign:
        thetool.AcceptOppCharge=False
        thetool.AcceptSameCharge=True

    ToolSvc += thetool
    return thetool


    
################################################################
# Add a version of the Z TP algorithm and all associated tools
################################################################
def AddConfiguredZmumuTPIsolationAlg(name_suffix="myProbes",
                            ProbeKind = "CB", # options: "CaloTag","MS", "Truth", "ID"
                            MatchContainer="Muons",
                            doEtaSlices=False, writeNtuple=False,
                            doClosure = False, doDRSys = False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = "Muons"

    ProbeContainer=MuonContainerToUse

    theAlg = AddZmumuTPAlg(name="ZmumuTPIsolationAlg_%s"%name_suffix,
                           ProbeCont=ProbeContainer,
                           MatchCont=MatchContainer)

    AddMuonSelectionTool()

    # add two TP selection tools ( OC and SC)
    SelectionTools = []
    SelectionTools += [AddZmumuTPIsolationSelectionTool(name="ZmumuTPIsolationSelectionTool_OC_%s"%name_suffix,EffiFlag="Isolation", ProbeType = ProbeKind)]
    SelectionTools += [AddZmumuTPIsolationSelectionTool(name="ZmumuTPIsolationSelectionTool_SC_%s"%name_suffix,EffiFlag="Isolation_SC", ProbeType = ProbeKind, SameSign=True)]


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
            PlotTool = AddZmumuTPIsolationPlotToo(name="ZmumuTPIsoPlottingTool_%s_%s"%(name_suffix,etaslice),EffiFlag=name_suffix+"_"+etaslice,applySF = False)
            PlotTool.ProbeAbsEtaMin = interval[0]
            PlotTool.ProbeAbsEtaMax = interval[1]
            PlotTools.append(PlotTool)
            if doClosure:
                PlotToolCL = AddZmumuTPIsolationPlotTool(name="ZmumuTPIsoPlottingToolClosure_%s_%s"%(name_suffix,etaslice),EffiFlag=name_suffix+"_"+etaslice+"_Closure",applySF = True)
                PlotToolCL.ProbeAbsEtaMin = interval[0]
                PlotToolCL.ProbeAbsEtaMax = interval[1]
                PlotTools.append(PlotToolCL)
                
    else:
        PlotTools = [AddZmumuTPIsolationPlotTool(name="ZmumuTPIsoPlottingTool_%s_All"%name_suffix,EffiFlag=name_suffix+"_All",applySF = False)]
        if doClosure:
            PlotTools.append(AddZmumuTPIsolationPlotTool(name="ZmumuTPIsoPlottingToolClosure_%s_All"%name_suffix,EffiFlag=name_suffix+"_All_Closure",applySF = True))
        

    # add any desired matching tools

    MATCH_DR = 0.05

    
    MatchingTools = []
    thistool = AddZmumuTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_%s_CB"%name_suffix,EffiFlag="Isolation")
    if ProbeKind == "CB":
        thistool.MatchToCB = True
    if ProbeKind == "Loose":
        thistool.MatchToLoose = True
    if ProbeKind == "Medium":
        thistool.MatchToMedium = True
    thistool.ApplyScaleFactors = False
    thistool.MaximumDrCut = MATCH_DR
    MatchingTools += [thistool]
    if doDRSys:
        thistool_dRDown = AddZmumuTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_%s_CB_dRDown"%name_suffix,EffiFlag="Isolation_dRDown")
        if ProbeKind == "CB":
            thistool_dRDown.MatchToCB = True
        if ProbeKind == "Loose":
            thistool_dRDown.MatchToLoose = True
        if ProbeKind == "Medium":
            thistool_dRDown.MatchToMedium = True
        thistool_dRDown.ApplyScaleFactors = False
        thistool_dRDown.MaximumDrCut = 0.5 * MATCH_DR
        MatchingTools += [thistool_dRDown]
        
        thistool_dRUp = AddZmumuTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_%s_CB_dRUp"%name_suffix,EffiFlag="Isolation_dRUp")
        if ProbeKind == "CB":
            thistool_dRUp.MatchToCB = True
        if ProbeKind == "Loose":
            thistool_dRUp.MatchToLoose = True
        if ProbeKind == "Medium":
            thistool_dRUp.MatchToMedium = True
        thistool_dRUp.ApplyScaleFactors = False
        thistool_dRUp.MaximumDrCut = 2. * MATCH_DR
        MatchingTools += [thistool_dRUp]

    

    ntuples = []
    if writeNtuple:
        ntuples.append(AddZmumuTPIsolationTreeTool(name="ZmumuTPIsoTreeTool_%s"%name_suffix, EffiFlag="Trees"))

    # create the TP tool itself
    TheTPTool = AddZmumuTPTool(name = "ZmumuTPTool_%s"%name_suffix,EffiFlag=name_suffix)
    TheTPTool.MuonTPSelectionTools = SelectionTools
    TheTPTool.PlottingTools = PlotTools
    TheTPTool.MuonTPEfficiencyTools = MatchingTools
    TheTPTool.TreeTools = ntuples
    theAlg.MuonTPTools += [TheTPTool]


def AddZmumuTPIsolationAnalysis(doEtaSlices=False,writeNtuple=False,doClosure=False,doDRSys=False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = "Muons"
    #if hasattr(job, "MuonQualityUpdater"):
        #MuonContainerToUse = "UpdatedMuons"

    ##########################################################################################
    # Add the Zmm TP algorithm using Combined muons as probes and matching to ID tracks
    AddConfiguredZmumuTPIsolationAlg(name_suffix = "CBProbes",
                            ProbeKind = "CB",
                            MatchContainer = MuonContainerToUse,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys)
