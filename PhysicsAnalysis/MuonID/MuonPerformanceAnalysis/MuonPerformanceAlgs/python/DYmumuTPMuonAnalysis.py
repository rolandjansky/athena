# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonPerformanceAlgs import CommonMuonTPConfig, ZmumuTPTrigAnalysis, ZmumuTPIsolAnalysis

#========================================================================================================================
def AddConfiguredDYmumuMuonTPAlg(name_suffix  = "MuonProbe",
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


    MuonContainerToUse = CommonMuonTPConfig.GetRightMuonContainer()
    ProbeContainer = CommonMuonTPConfig.GetRightMuonContainer()
    theAlg = CommonMuonTPConfig.AddTagProbeAlg(name="DYmumuMuProbeAlg_%s"%name_suffix, ProbeCont=ProbeContainer)
    theAlg.TopLevelFolderName = "DYmumuTPMuon"

    SelectionTools = []
    
    if doMinimalCutsTree:
        SelecToolOC_Minimal = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_OC_MinimalCuts_%s"%name_suffix,EffiFlag="%s_MinimalCuts_LooseMuonProbes"%name_suffix, ProbeType="Loose",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
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
        
    if doLooseProbes:
        SelecToolsOC_LooseProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_OC_LooseProbes_%s"%name_suffix,EffiFlag="%s_OC_LooseProbes"%name_suffix, ProbeType="Loose",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_LooseProbes.UseLooseProbes = True
        SelectionTools.append(SelecToolsOC_LooseProbes)
        
        # configure one additional instance without d0 cuts, to measure the efficiency of IP requirements in data and MC
        SelecToolsOC_LooseProbes_noProbeIP = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_OC_LooseProbes_noProbeIP_%s"%name_suffix,EffiFlag="%s_OC_LooseProbes_noProbeIP"%name_suffix, ProbeType="Loose",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_LooseProbes_noProbeIP.UseLooseProbes = True
        SelecToolsOC_LooseProbes_noProbeIP.TagD0Cut = -1
        SelecToolsOC_LooseProbes_noProbeIP.TagD0SignCut = -1
        SelecToolsOC_LooseProbes_noProbeIP.TagZ0Cut = -1
        SelecToolsOC_LooseProbes_noProbeIP.ProbeD0Cut = -1
        SelecToolsOC_LooseProbes_noProbeIP.ProbeD0SignCut = -1
        SelecToolsOC_LooseProbes_noProbeIP.ProbeZ0Cut = -1
        SelectionTools.append(SelecToolsOC_LooseProbes_noProbeIP)
        
        
        SelecToolsSC_LooseProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_SC_LooseProbes_%s"%name_suffix,EffiFlag="%s_SC_LooseProbes"%name_suffix, ProbeType="Loose", SameSign=True,forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_LooseProbes.UseLooseProbes = True
        SelectionTools.append(SelecToolsSC_LooseProbes)
        
        # configure one additional instance without d0 cuts, to measure the efficiency of IP requirements in data and MC
        SelecToolsSC_LooseProbes_noProbeIP = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_SC_LooseProbes_noProbeIP_%s"%name_suffix,EffiFlag="%s_SC_LooseProbes_noProbeIP"%name_suffix, ProbeType="Loose", SameSign=True,forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_LooseProbes_noProbeIP.UseLooseProbes = True
        SelecToolsSC_LooseProbes_noProbeIP.TagD0Cut = -1
        SelecToolsSC_LooseProbes_noProbeIP.TagD0SignCut = -1
        SelecToolsSC_LooseProbes_noProbeIP.TagZ0Cut = -1
        SelecToolsSC_LooseProbes_noProbeIP.ProbeD0Cut = -1
        SelecToolsSC_LooseProbes_noProbeIP.ProbeD0SignCut = -1
        SelecToolsSC_LooseProbes_noProbeIP.ProbeZ0Cut = -1
        SelectionTools.append(SelecToolsSC_LooseProbes_noProbeIP)

    if doMediumProbes:
        SelecToolsOC_MediumProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_OC_MediumProbes_%s"%name_suffix,EffiFlag="%s_OC_MediumProbes"%name_suffix, ProbeType="Medium",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_MediumProbes.UseMediumProbes = True
        SelectionTools.append(SelecToolsOC_MediumProbes)
        
        SelecToolsSC_MediumProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_SC_MediumProbes_%s"%name_suffix,EffiFlag="%s_SC_MediumProbes"%name_suffix, ProbeType="Medium",forTriggerEff=doTrig, SameSign=True,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_MediumProbes.UseMediumProbes = True
        SelectionTools.append(SelecToolsSC_MediumProbes)

    if doTightProbes:
        SelecToolsOC_TightProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_OC_TightProbes_%s"%name_suffix,EffiFlag="%s_OC_TightProbes"%name_suffix, ProbeType="Tight",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_TightProbes.UseTightProbes = True
        SelectionTools.append(SelecToolsOC_TightProbes)
        
        SelecToolsSC_TightProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_SC_TightProbes_%s"%name_suffix,EffiFlag="%s_SC_TightProbes"%name_suffix, ProbeType="Tight",forTriggerEff=doTrig, SameSign=True,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_TightProbes.UseTightProbes = True
        SelectionTools.append(SelecToolsSC_TightProbes)

    if doHighPtProbes:
        SelecToolsOC_HighPtProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_OC_HighPtProbes_%s"%name_suffix,EffiFlag="%s_OC_HighPtProbes"%name_suffix, ProbeType="HighPt",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_HighPtProbes.UseHighPtProbes = True
        SelectionTools.append(SelecToolsOC_HighPtProbes)
        
        SelecToolsSC_HighPtProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_SC_HighPtProbes_%s"%name_suffix,EffiFlag="%s_SC_HighPtProbes"%name_suffix, ProbeType="HighPt",forTriggerEff=doTrig, SameSign=True,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_HighPtProbes.UseHighPtProbes = True
        SelectionTools.append(SelecToolsSC_HighPtProbes)
            
            
    from AthenaCommon.GlobalFlags import globalflags
    if doTruthProbes and not globalflags.DataSource()=='data':
        SelecToolsOC_TruthProbes = CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_OC_TruthProbes_%s"%name_suffix,EffiFlag="%s_TruthProbes"%name_suffix, ProbeType="TruthMatchedMuons",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelectionTools.append(SelecToolsOC_TruthProbes)
        
        
    for thetool in SelectionTools:
        thetool.DoTagIsolation = False
        thetool.DoProbeIsolation=False 
        thetool.ProbeEtaCut = 4.5  
        thetool.LowMassWindow = 81000
        thetool.HighMassWindow = 50e6 # should be safe at LHC as long as energy is conserved... 
        thetool.TagPtCut = 30e3  # accept tags above 30 GeV.
        thetool.ProbePtCut = 30e3  # accept probes above 30 GeV.
        
        
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
        ntuples.append(CommonMuonTPConfig.AddTreeTool(name="DYmumuTPMuonTreeTool_%s"%name_suffix, 
                                                      EffiFlag="Trees",WriteSFInfo=doClosure,
                                                      IsRunOnDAOD=IsRunOnDAOD))
    
    for ntuple in ntuples:
        if doIso:
            ntuple.AddExtendedIsolation = True
        if doTrig:
            ntuple.AddExtendedTrigger = True
        
    # create the TP tool itself
    TheTPTool = CommonMuonTPConfig.AddMuonTPTool(name = "DYMuonProbeTPTool_%s"%name_suffix,EffiFlag=name_suffix)
    TheTPTool.MuonTPSelectionTools  = SelectionTools
    TheTPTool.PlottingTools         = PlotTools
    TheTPTool.MuonTPEfficiencyTools = MatchingTools
    TheTPTool.TreeTools             = ntuples
    theAlg.MuonTPTools += [TheTPTool]
    
#========================================================================================================================
def AddDYmumuTPMuonAnalysis(doIso = True,
                           doTrig = True,
                           doTrigEtaSlices    = True, 
                           doIsolEtaSlices    = False, 
                           doIsolPlots      = False,
                           doTriggerPlots      = True,
                           writeNtuple=False, doClosure=False, 
                           doDRSys=False,
                           doMinimalCutsTree=False,
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
    AddConfiguredDYmumuMuonTPAlg(name_suffix  = "DYMuonProbe",
                            doIso=doIso,
                            doTrig=doTrig,
                            doTrigEtaSlices    = doTrigEtaSlices, 
                            doIsolEtaSlices    = doIsolEtaSlices, 
                            doIsolPlots      = doIsolPlots,
                            doTriggerPlots      = doTriggerPlots,
                            writeNtuple    = writeNtuple,
                            doClosure      = doClosure,
                            doDRSys      = doDRSys,
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
                
