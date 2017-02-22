# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonPerformanceAlgs import CommonMuonTPConfig, ZmumuTPTrigAnalysis, ZmumuTPIsolAnalysis

#========================================================================================================================
def AddConfiguredMuonTPAlg(name_suffix  = "MuonProbe",
                            MatchContainer = "Muons",
                            doIso = True,
                            doTrig = True,
                            doTrigEtaSlices    = True, 
                            doIsolEtaSlices    = False, 
                            doIsolPlots      = False,
                            doTriggerPlots      = True,
                            writeNtuple    = False,
                            doClosure      = False,
                            doDRSys    = False,
                            doMinimalCutsTree = False,
                            doVeryLooseProbes = False,
                            doLooseProbes   = False,
                            doMediumProbes  = False,
                            doTightProbes   = False,
                            doHighPtProbes   = False,
                            doTruthProbes   = False,
                            doLooseTrackOnly=True,
                            doLoose=True,
                            doTight=True,
                            doGradient=True,
                            doGradientLoose=True,
                            DoProbeMatchPlots=True,
                            ProduceEfficiencies=False,
                            doL1 = False,
                            doHLT = False,
                            IsRunOnDAOD=False
                ):
    
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()


    ProbeContainer = CommonMuonTPConfig.GetRightMuonContainer()
    theAlg = CommonMuonTPConfig.AddTagProbeAlg(name="ZmumuMuProbeAlg_%s"%name_suffix, ProbeCont=ProbeContainer)
    theAlg.TopLevelFolderName = "ZmumuTPMuon"

    SelectionTools = []
    
    if doMinimalCutsTree:
        SelecToolOC_Minimal = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OC_MinimalCuts_%s"%name_suffix,EffiFlag="%s_MinimalCuts_LooseMuonProbes"%name_suffix, ProbeType="Loose",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolOC_Minimal.UseLooseProbes = True
        SelecToolOC_Minimal.TagPtCut=1000
        SelecToolOC_Minimal.ProbePtCut=1000
        SelecToolOC_Minimal.TagD0Cut = -1
        SelecToolOC_Minimal.TagD0SignCut = -1
        SelecToolOC_Minimal.TagZ0Cut = -1
        SelecToolOC_Minimal.ProbeD0Cut = -1
        SelecToolOC_Minimal.ProbeD0SignCut = -1
        SelecToolOC_Minimal.ProbeZ0Cut = -1
        SelecToolOC_Minimal.DoTagIsolation = False
        SelecToolOC_Minimal.DoProbeIsolation = False
        SelecToolOC_Minimal.AcceptOppCharge=True
        SelecToolOC_Minimal.AcceptSameCharge=True
        SelectionTools.append(SelecToolOC_Minimal)
    
    # for high-eta muons
    if doVeryLooseProbes:
        SelecToolsOC_VeryLooseProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OC_VeryLooseProbes_%s"%name_suffix,EffiFlag="%s_OC_VeryLooseProbes"%name_suffix, ProbeType="VeryLoose",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        # no IP cuts
        SelecToolsOC_VeryLooseProbes.UseVeryLooseProbes = True
        SelecToolsOC_VeryLooseProbes.TagD0Cut = -1
        SelecToolsOC_VeryLooseProbes.TagD0SignCut = -1
        SelecToolsOC_VeryLooseProbes.TagZ0Cut = -1
        SelecToolsOC_VeryLooseProbes.ProbeD0Cut = -1
        SelecToolsOC_VeryLooseProbes.ProbeD0SignCut = -1
        SelecToolsOC_VeryLooseProbes.ProbeZ0Cut = -1
        # turn on the pivot plane eta/phi extrapolation - used for CSC inefficiency crossschecks
        SelecToolsOC_VeryLooseProbes.SkipTriggerPivotPlaneDr = False
        SelectionTools.append(SelecToolsOC_VeryLooseProbes)
        
        SelecToolsSC_VeryLooseProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_SC_VeryLooseProbes_%s"%name_suffix,EffiFlag="%s_SC_VeryLooseProbes"%name_suffix, ProbeType="VeryLoose", SameSign=True,forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_VeryLooseProbes.UseVeryLooseProbes = True
        SelecToolsSC_VeryLooseProbes.TagD0Cut = -1
        SelecToolsSC_VeryLooseProbes.TagD0SignCut = -1
        SelecToolsSC_VeryLooseProbes.TagZ0Cut = -1
        SelecToolsSC_VeryLooseProbes.ProbeD0Cut = -1
        SelecToolsSC_VeryLooseProbes.ProbeD0SignCut = -1
        SelecToolsSC_VeryLooseProbes.ProbeZ0Cut = -1
        # turn on the pivot plane eta/phi extrapolation - used for CSC inefficiency crossschecks
        SelecToolsSC_VeryLooseProbes.SkipTriggerPivotPlaneDr = False
        SelectionTools.append(SelecToolsSC_VeryLooseProbes)
    
    if doLooseProbes:
        # SelecToolsOC_LooseProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OC_LooseProbes_%s"%name_suffix,EffiFlag="%s_OC_LooseProbes"%name_suffix, ProbeType="Loose",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        # SelecToolsOC_LooseProbes.UseLooseProbes = True
        # SelectionTools.append(SelecToolsOC_LooseProbes)
        
        # configure one additional instance without d0 cuts, to measure the efficiency of IP requirements in data and MC
        SelecToolsOC_LooseProbes_noProbeIP = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OC_LooseProbes_noProbeIP_%s"%name_suffix,EffiFlag="%s_OC_LooseProbes_noProbeIP"%name_suffix, ProbeType="Loose",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_LooseProbes_noProbeIP.UseLooseProbes = True
        SelecToolsOC_LooseProbes_noProbeIP.TagD0Cut = -1
        SelecToolsOC_LooseProbes_noProbeIP.TagD0SignCut = -1
        SelecToolsOC_LooseProbes_noProbeIP.TagZ0Cut = -1
        SelecToolsOC_LooseProbes_noProbeIP.ProbeD0Cut = -1
        SelecToolsOC_LooseProbes_noProbeIP.ProbeD0SignCut = -1
        SelecToolsOC_LooseProbes_noProbeIP.ProbeZ0Cut = -1
        SelectionTools.append(SelecToolsOC_LooseProbes_noProbeIP)
        
        
        # SelecToolsSC_LooseProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_SC_LooseProbes_%s"%name_suffix,EffiFlag="%s_SC_LooseProbes"%name_suffix, ProbeType="Loose", SameSign=True,forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        # SelecToolsSC_LooseProbes.UseLooseProbes = True
        # SelectionTools.append(SelecToolsSC_LooseProbes)
        
        SelecToolsSC_LooseProbes_noProbeIP = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_SC_LooseProbes_noProbeIP_%s"%name_suffix,EffiFlag="%s_SC_LooseProbes_noProbeIP"%name_suffix, ProbeType="Loose", SameSign=True,forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_LooseProbes_noProbeIP.UseLooseProbes = True
        SelecToolsSC_LooseProbes_noProbeIP.TagD0Cut = -1
        SelecToolsSC_LooseProbes_noProbeIP.TagD0SignCut = -1
        SelecToolsSC_LooseProbes_noProbeIP.TagZ0Cut = -1
        SelecToolsSC_LooseProbes_noProbeIP.ProbeD0Cut = -1
        SelecToolsSC_LooseProbes_noProbeIP.ProbeD0SignCut = -1
        SelecToolsSC_LooseProbes_noProbeIP.ProbeZ0Cut = -1
        SelectionTools.append(SelecToolsSC_LooseProbes_noProbeIP)
        
    if doMediumProbes:
        SelecToolsOC_MediumProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OC_MediumProbes_%s"%name_suffix,EffiFlag="%s_OC_MediumProbes"%name_suffix, ProbeType="Medium",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_MediumProbes.UseMediumProbes = True
        SelectionTools.append(SelecToolsOC_MediumProbes)
        
        SelecToolsSC_MediumProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_SC_MediumProbes_%s"%name_suffix,EffiFlag="%s_SC_MediumProbes"%name_suffix, ProbeType="Medium",forTriggerEff=doTrig, SameSign=True,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_MediumProbes.UseMediumProbes = True
        SelectionTools.append(SelecToolsSC_MediumProbes)

    if doTightProbes:
        SelecToolsOC_TightProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OC_TightProbes_%s"%name_suffix,EffiFlag="%s_OC_TightProbes"%name_suffix, ProbeType="Tight",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_TightProbes.UseTightProbes = True
        SelectionTools.append(SelecToolsOC_TightProbes)
        
        SelecToolsSC_TightProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_SC_TightProbes_%s"%name_suffix,EffiFlag="%s_SC_TightProbes"%name_suffix, ProbeType="Tight",forTriggerEff=doTrig, SameSign=True,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_TightProbes.UseTightProbes = True
        SelectionTools.append(SelecToolsSC_TightProbes)

    if doHighPtProbes:
        SelecToolsOC_HighPtProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OC_HighPtProbes_%s"%name_suffix,EffiFlag="%s_OC_HighPtProbes"%name_suffix, ProbeType="HighPt",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_HighPtProbes.UseHighPtProbes = True
        SelectionTools.append(SelecToolsOC_HighPtProbes)
        
        SelecToolsSC_HighPtProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_SC_HighPtProbes_%s"%name_suffix,EffiFlag="%s_SC_HighPtProbes"%name_suffix, ProbeType="HighPt",forTriggerEff=doTrig, SameSign=True,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_HighPtProbes.UseHighPtProbes = True
        SelectionTools.append(SelecToolsSC_HighPtProbes)
            
            
    from AthenaCommon.GlobalFlags import globalflags
    if doTruthProbes and not globalflags.DataSource()=='data':
        SelecToolsOC_TruthProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OC_TruthProbes_%s"%name_suffix,EffiFlag="%s_TruthProbes"%name_suffix, ProbeType="TruthMatchedMuons",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelectionTools.append(SelecToolsOC_TruthProbes)
        
        
    for thetool in SelectionTools:
        thetool.DoTagIsolation = False
        thetool.DoProbeIsolation=False 
        thetool.ProbeEtaCut = 4.5   
        
    PlotTools = []
    
    # first the trigger plots 
    if doTrig and doTriggerPlots:
        PlotTools += ZmumuTPTrigAnalysis.AddTrigPlots(name_suffix=name_suffix,
                                        doEtaSlices=doTrigEtaSlices,
                                        doClosure=doClosure,
                                        DoProbeMatchPlots=DoProbeMatchPlots,
                                        ProduceEfficiencies=ProduceEfficiencies,
                                        IsRunOnDAOD=IsRunOnDAOD)
    # then the isolation plots
    if doIso and doIsolPlots:
        PlotTools += ZmumuTPIsolAnalysis.AddIsolPlots(name_suffix=name_suffix,
                                        doEtaSlices=doIsolEtaSlices,
                                        doClosure=doClosure,
                                        DoProbeMatchPlots=DoProbeMatchPlots,
                                        ProduceEfficiencies=ProduceEfficiencies,
                                        IsRunOnDAOD=IsRunOnDAOD)
        
    
    MatchingTools = []
    if doTrig:
        MatchingTools += ZmumuTPTrigAnalysis.AddTrigMatchTools(name_suffix=name_suffix,doL1 = doL1, doHLT=doHLT,
                                                               doDRSys=doDRSys,doClosure=doClosure,doRerunMode=False)

        #MatchingTools += ZmumuTPTrigAnalysis.AddTrigMatchTools(name_suffix=name_suffix+'_RM',doL1 = doL1, doHLT=doHLT,
        #                                                       doDRSys=doDRSys,doClosure=doClosure,doRerunMode=True)
    if doIso:
        MatchingTools += ZmumuTPIsolAnalysis.AddIsolMatchTools(name_suffix=name_suffix,
                                                               doLooseTrackOnly=doLooseTrackOnly,doLoose=doLoose,
                                                               doTight=doTight,doGradient=doGradient,
                                                               doGradientLoose=doGradientLoose,doClosure=doClosure)
    
    
    ntuples = []
    if writeNtuple:
        ntuples.append(CommonMuonTPConfig.AddTreeTool(name="ZmumuTPMuonTreeTool_%s"%name_suffix, 
                                                      EffiFlag="Trees",WriteSFInfo=doClosure,
                                                      IsRunOnDAOD=IsRunOnDAOD))
    
    for ntuple in ntuples:
        if doIso:
            ntuple.AddExtendedIsolation = True
        if doTrig:
            ntuple.AddExtendedTrigger = True
        
    # create the TP tool itself
    TheTPTool = CommonMuonTPConfig.AddMuonTPTool(name = "MuonProbeTPTool_%s"%name_suffix,EffiFlag=name_suffix)
    TheTPTool.MuonTPSelectionTools  = SelectionTools
    TheTPTool.PlottingTools         = PlotTools
    TheTPTool.MuonTPEfficiencyTools = MatchingTools
    TheTPTool.TreeTools             = ntuples
    theAlg.MuonTPTools += [TheTPTool]
    
#========================================================================================================================
def AddZmumuTPMuonAnalysis(doIso = True,
                           doTrig = True,
                           doTrigEtaSlices    = True, 
                           doIsolEtaSlices    = False, 
                           doIsolPlots      = False,
                           doTriggerPlots      = True,
                           writeNtuple=False, doClosure=False, 
                           doDRSys=False,
                           doMinimalCutsTree=False,
                           doVeryLooseProbes = False,
                           doLooseProbes=False, doMediumProbes=False, 
                           doTightProbes=False,  
                           doTruthProbes=False,
                           doHighPtProbes=False,
                           doLooseTrackOnly=True,
                           doLoose=True,
                           doTight=True,
                           doGradient=True,
                           doGradientLoose=True,
                           doL1=False, 
                           doHLT=False, DoProbeMatchPlots=True, 
                           ProduceEfficiencies=False,
                           IsRunOnDAOD=False):
    
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

        
    ##########################################################################################
    # Add the Zmm TP algorithm for muon probes
    AddConfiguredMuonTPAlg(name_suffix  = "MuonProbe",
                            MatchContainer = CommonMuonTPConfig.GetRightMuonContainer(),
                            doIso=doIso,
                            doTrig=doTrig,
                            doTrigEtaSlices    = doTrigEtaSlices, 
                            doIsolEtaSlices    = doIsolEtaSlices, 
                            doIsolPlots      = doIsolPlots,
                            doTriggerPlots      = doTriggerPlots,
                            writeNtuple    = writeNtuple,
                            doClosure      = doClosure,
                            doDRSys      = doDRSys,
                            doVeryLooseProbes = doVeryLooseProbes,
                            doLooseProbes   = doLooseProbes,
                            doMediumProbes  = doMediumProbes,
                            doTightProbes   = doTightProbes,
                            doHighPtProbes   = doHighPtProbes,
                            doTruthProbes=doTruthProbes,
                            doLooseTrackOnly=doLooseTrackOnly,
                            doLoose=doLoose,
                            doTight=doTight,
                            doMinimalCutsTree=doMinimalCutsTree,
                            doGradient=doGradient,
                            doGradientLoose=doGradientLoose,
                            doL1 = doL1, 
                            doHLT=doHLT,
                            DoProbeMatchPlots=DoProbeMatchPlots,
                            ProduceEfficiencies=ProduceEfficiencies,
                            IsRunOnDAOD=IsRunOnDAOD)
                
