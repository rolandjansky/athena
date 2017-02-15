# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from MuonPerformanceAlgs import CommonMuonTPConfig 
################################################################
# Add a version of the Z TP algorithm and all associated tools
################################################################
def AddConfiguredHighPtSingleMuonWriter(name_suffix="myProbes",
                            ProbeKind="Muon",
                            writeNtuple=False,
                            writeProbeIso=True,
                            IsRunOnDAOD=False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = CommonMuonTPConfig.GetRightMuonContainer()

    ProbeContainer=MuonContainerToUse

    if ProbeKind == "MSTrackPart":
        ProbeContainer = "ExtrapolatedMuonTrackParticles"
        ProbeKind = "MS"
        
    if ProbeKind == "ID":
#         ProbeContainer = CommonMuonTPConfig.AddHighPtTrackPicker(10000.).Output
        ProbeContainer = "InDetTrackParticles"
	#
    if ProbeKind == "Truth":
        ProbeContainer = "MuonTruthParticles"

    theAlg = CommonMuonTPConfig.AddTagProbeAlg(name="HighPtSingeMuonWriterAlg_%s"%name_suffix,
                           ProbeCont=ProbeContainer)
    theAlg.TopLevelFolderName = "HighPtSingleMuon"

    # add a plotting tool
    #PlotTool = AddPlottingTool(name="ZmumuTPPlottingTool_%s"%name_suffix,EffiFlag=name_suffix)

    # add four TP selection tools (iso and antiiso OC and SC)
    SelectionTools = []
    SelectionTools += [CommonMuonTPConfig.AddHighPtSingleMuonSelectionTool(name="HighPtSingleMuonSelectionTool_%s"%name_suffix,
                                                                  EffiFlag="%s"%name_suffix, 
                                                                  ProbeType = ProbeKind,IsRunOnDAOD=IsRunOnDAOD)]

    PlotTools = []
                
    PlotTools = [CommonMuonTPConfig.AddPlottingTool(name="HighPtSingleMuonPlottingTool_%s_All"%name_suffix,
                                                        EffiFlag=name_suffix+"_All",
                                                        applySF = False,
                                                        DoProbeMatchPlots=False,
                                                        ProduceEfficiencies=False,IsRunOnDAOD=IsRunOnDAOD)]

        
    # Config the plot tools
    for thetool in PlotTools:
        thetool.doZmumuKinematicPlots = False
        thetool.doFineEtaPhiPlots = True
        thetool.doDetectorRegionPlots = True
    
    # add any desired matching tools
    MATCH_DR = 0.05

    
    MatchingTools = []
        
    if writeProbeIso:
        isotool_Loose = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_Reco_%s_Loose"%name_suffix,EffiFlag="IsoLoose",doClosure=False,IsNominal=True)
        isotool_Loose.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("Loose")
        isotool_Loose.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("LooseIso",doClosure=False)
        MatchingTools += [isotool_Loose]

        isotool_Tight = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_Reco_%s_Tight"%name_suffix,EffiFlag="IsoTight",doClosure=False,IsNominal=True)
        isotool_Tight.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("Tight")
        isotool_Tight.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("TightIso",doClosure=False)
        MatchingTools += [isotool_Tight]

        isotool_Gradient = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_Reco_%s_Gradient"%name_suffix,EffiFlag="IsoGradient",doClosure=False,IsNominal=True)
        isotool_Gradient.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("Gradient")
        isotool_Gradient.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("GradientIso",doClosure=False)
        MatchingTools += [isotool_Gradient]

        isotool_GradientLoose = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_Reco_%s_GradientLoose"%name_suffix,EffiFlag="IsoGradientLoose",doClosure=False,IsNominal=True)
        isotool_GradientLoose.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("GradientLoose")
        isotool_GradientLoose.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("GradientLooseIso",doClosure=False)
        MatchingTools += [isotool_GradientLoose]

        isotool_FixedCutLoose = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_Reco_%s_FixedCutLoose"%name_suffix,EffiFlag="IsoFixedCutLoose",doClosure=False,IsNominal=True)
        isotool_FixedCutLoose.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("FixedCutLoose")
        isotool_FixedCutLoose.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("FixedCutLooseIso",doClosure=False)
        MatchingTools += [isotool_FixedCutLoose]

        isotool_FixedCutTightTrackOnly = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_Reco_%s_FixedCutTightTrackOnly"%name_suffix,EffiFlag="IsoFixedCutTightTrackOnly",doClosure=False,IsNominal=True)
        isotool_FixedCutTightTrackOnly.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("FixedCutTightTrackOnly")
        isotool_FixedCutTightTrackOnly.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("FixedCutTightTrackOnlyIso",doClosure=False)
        MatchingTools += [isotool_FixedCutTightTrackOnly]
    
    ntuples = []
    if writeNtuple:
        ntuples.append(CommonMuonTPConfig.AddTreeTool(name="HighPtSingleMuonTreeTool_%s"%name_suffix, 
                                                      EffiFlag="Trees",WriteSFInfo=False,IsRunOnDAOD=IsRunOnDAOD))
    
    for ntuple in ntuples:
        ntuple.AddExtendedIsolation = True
    # create the TP tool itself
    TheTPTool = CommonMuonTPConfig.AddMuonTPTool(name = "HighPtSingleMuonTool_%s"%name_suffix,
                                                 EffiFlag=name_suffix)
    TheTPTool.MuonTPSelectionTools = SelectionTools
    TheTPTool.PlottingTools = PlotTools
    TheTPTool.MuonTPEfficiencyTools = MatchingTools
    TheTPTool.TreeTools = ntuples
    theAlg.MuonTPTools += [TheTPTool]
    
    print theAlg
    

def AddHighPtSingleMuonAnalysis(writeNtuple=False,
                           writeProbeIso=True,doIDTracks=False,doMSTracks=False,doMuons=True,IsRunOnDAOD=False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = CommonMuonTPConfig.GetRightMuonContainer()

    ##########################################################################################
    if doIDTracks:
        AddConfiguredHighPtSingleMuonWriter(name_suffix = "IDTrack",
                            ProbeKind = "ID",
                            writeNtuple = writeNtuple,writeProbeIso=writeProbeIso,IsRunOnDAOD=IsRunOnDAOD)


    #########################################################################################
    if doMSTracks:
        AddConfiguredHighPtSingleMuonWriter(name_suffix = "MSTrackParticles",
                            ProbeKind = "MSTrackPart",
                            writeNtuple = writeNtuple,writeProbeIso=writeProbeIso,IsRunOnDAOD=IsRunOnDAOD)


    ##########################################################################################
    if doMuons:
        AddConfiguredHighPtSingleMuonWriter(name_suffix = "Muons",
                            ProbeKind = "Muons",
                            writeNtuple = writeNtuple,writeProbeIso=writeProbeIso,IsRunOnDAOD=IsRunOnDAOD)

