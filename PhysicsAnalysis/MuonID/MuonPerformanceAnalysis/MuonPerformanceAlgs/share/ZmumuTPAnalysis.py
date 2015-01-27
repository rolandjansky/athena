
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

# Add instance of the TP plotting tool
def AddZmumuTPPlotTool(name, EffiFlag):

    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import ZmumuMuonTPPlotTool
    thetool = ZmumuMuonTPPlotTool(name)
    thetool.EfficiencyFlag = EffiFlag
    thetool.doTrkValidPlots = False
    thetool.doBasicKinematicPlots = True
    thetool.doDileptonPlots = True
    ToolSvc+= thetool
    return thetool

# Add a TP selection tool
def AddZmumuTPSelectionTool (name, EffiFlag, ProbeType, SameSign=False):

    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import ZmumuMuonTPSelectionTool
    thetool = ZmumuMuonTPSelectionTool(name)
    thetool.TagPtCut = 25000.
    thetool.ProbePtCut = 10000.
    thetool.HighMassWindow = 101000.
    thetool.LowMassWindow = 81000.
    thetool.ProbeIsoCut = -1.
    thetool.EfficiencyFlag = EffiFlag
    thetool.SelectionTool = ToolSvc.TagProbeMuonSelectionTool

    if ProbeType == "ID":
        thetool.UseIDProbes=True
        thetool.ProbeIDHitCut=True
        thetool.ProbeIsoCut = 0.05
    if ProbeType == "MS":
        thetool.UseMSProbes = True
    if ProbeType == "CaloTag":
        thetool.UseCaloProbes = True
        thetool.ProbeIDHitCut=True
        thetool.ProbeIsoCut = 0.05
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
    from MuonTPTools.MuonTPToolsConf import ZmumuMuonTPEfficiencyTool
    thetool = ZmumuMuonTPEfficiencyTool(name)
    thetool.MuonAuthor = 21
    thetool.MatchToAnyMS = False
    thetool.MatchToCB = False
    thetool.MatchToMedium = False
    thetool.MatchToID = False
    thetool.MatchToCaloTag = False
    thetool.MatchPtCut = 1000.0
    thetool.EfficiencyFlag = EffiFlag
    thetool.SelectionTool = ToolSvc.TagProbeMuonSelectionTool
    ToolSvc += thetool
    return thetool

# Add an instance of the 'main' TP tool
def AddZmumuTPTool (name, EffiFlag):

    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import MuonTPTool
    thetool = MuonTPTool(name)
    thetool.EfficiencyFlag = EffiFlag
    ToolSvc += thetool
    return thetool

################################################################
# Add a version of the Z TP algorithm and all associated tools
################################################################
def AddConfiguredZmumuTPAlg(name_suffix="myProbes",
                            ProbeKind = "CaloTag", # options: "CaloTag","MS", "Truth", "ID"
                            MatchContainer="Muons",
                            doID=False,
                            doCB = True, doMedium=True,
                            doStandalone=False, doCaloTag=False,
                            doEtaSlices=False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = "Muons"
    if hasattr(job, "MuonQualityUpdater"):
        MuonContainerToUse = "UpdatedMuons"

    ProbeContainer=MuonContainerToUse

    if ProbeKind == "MS":
        ProbeContainer = "ExtrapolatedMuonTrackParticles"
    if ProbeKind == "ID":
        ProbeContainer = "InDetTrackParticles"
    if ProbeKind == "Truth":
        ProbeContainer = "MuonTruthParticles"

    theAlg = AddZmumuTPAlg(name="ZmumuTPAlg_%s"%name_suffix,
                           ProbeCont=ProbeContainer,
                           MatchCont=MatchContainer)

    AddMuonSelectionTool()


    # add a plotting tool
    PlotTool = AddZmumuTPPlotTool(name="ZmumuTPPlottingTool_%s"%name_suffix,EffiFlag=name_suffix)

    # add two TP selection tools (OC and SC)
    SelectionTools = []
    if doEtaSlices:
        Slices = { "All" : (0.0,10.0),
                   "Crack" : (0.0,0.1),
                   "Barrel" : (0.1,1.1),
                   "Transition" : (1.1, 1.3),
                   "Endcap" : (1.3,2.0),
                   "CSC" : (2.0,2.7)}
        for etaslice, interval in Slices.iteritems():

            tool = AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OC_%s_%s"%(name_suffix,etaslice),EffiFlag="%s_%s_OC"%(name_suffix,etaslice), ProbeType = ProbeKind)
            tool.ProbeAbsEtaMin = interval[0]
            tool.ProbeAbsEtaMax = interval[1]
            SelectionTools += [tool]

            tool_SC = AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_SC_%s_%s"%(name_suffix,etaslice),EffiFlag="%s_%s_SC"%(name_suffix,etaslice), ProbeType = ProbeKind, SameSign=True)
            tool_SC.ProbeAbsEtaMin = interval[0]
            tool_SC.ProbeAbsEtaMax = interval[1]
            SelectionTools += [tool_SC]
    else:
        SelectionTools += [AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OC_%s"%name_suffix,EffiFlag="%s_OC"%name_suffix, ProbeType = ProbeKind)]
        SelectionTools += [AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_SC_%s"%name_suffix,EffiFlag="%s_SC"%name_suffix, ProbeType = ProbeKind, SameSign=True)]

    # add any desired matching tools

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
        MatchingTools += [thistoolCB]

    if doMedium:
        thistoolMed = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Medium"%name_suffix,EffiFlag="MediumMuons")
        thistoolMed.MatchToMedium = True
        if not hasattr(ToolSvc, "EffiScaleFactors_CBMuons"):
            ToolSvc+= CP__MuonEfficiencyScaleFactors("EffiScaleFactors_CBMuons")
            ToolSvc.EffiScaleFactors_CBMuons.WorkingPoint = "CB"
            ToolSvc.EffiScaleFactors_CBMuons.DataPeriod = "2012"
        thistoolMed.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
        thistoolMed.ApplyScaleFactors = True
        MatchingTools += [thistoolMed]

    if doStandalone:
        thistoolSA = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_SA"%name_suffix,EffiFlag="StandaloneMuons")
        thistoolSA.MatchToAnyMS = True
        thistoolSA.IDhitCut = False
        thistoolSA.ApplyScaleFactors = False
        MatchingTools += [thistoolSA]

    if doCaloTag:
        thistoolCT = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CT"%name_suffix,EffiFlag="CaloTaggedMuons")
        thistoolCT.MatchToCaloTag = True
        if not hasattr(ToolSvc, "EffiScaleFactors_CBMuons"):
            ToolSvc+= CP__MuonEfficiencyScaleFactors("EffiScaleFactors_CBMuons")
            ToolSvc.EffiScaleFactors_CBMuons.WorkingPoint = "CB"
            ToolSvc.EffiScaleFactors_CBMuons.DataPeriod = "2012"
        thistoolCT.ScaleFactorTool = ToolSvc.EffiScaleFactors_CBMuons
        thistoolCT.ApplyScaleFactors = True
        MatchingTools += [thistoolCT]

    if doID:
        thistoolID = AddZmumuTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID"%name_suffix,EffiFlag="IDTracks")
        thistoolID.MatchToID = True
        thistoolID.ApplyScaleFactors = False
        MatchingTools += [thistoolID]

    # create the TP tool itself
    TheTPTool = AddZmumuTPTool(name = "ZmumuTPTool_%s"%name_suffix,EffiFlag=name_suffix)
    TheTPTool.MuonTPSelectionTools = SelectionTools
    TheTPTool.PlottingTools += [PlotTool]
    TheTPTool.MuonTPEfficiencyTools = MatchingTools

    theAlg.MuonTPTools += [TheTPTool]



