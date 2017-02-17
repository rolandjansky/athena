# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonPerformanceAlgs import CommonMuonTPConfig, JPsiTPTrigAnalysis, JPsiTPIsolAnalysis

#========================================================================================================================
def AddConfiguredMuonTPAlg(name_suffix  = "MuonProbe_JPsi",
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

    ProbeContainer = MuonContainerToUse
    theAlg = CommonMuonTPConfig.AddTagProbeAlg(name="JPsiTPMuonAlg_%s"%name_suffix, ProbeCont=ProbeContainer)
    theAlg.TopLevelFolderName = "JPsiTPMuon"

    SelectionTools = []
    
    if doLooseProbes:
        SelecToolsOC_LooseProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_LooseProbes_%s"%name_suffix,EffiFlag="%s_OC_LooseProbes"%name_suffix, ProbeType="Loose",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_LooseProbes.UseLooseProbes = True
        SelectionTools.append(SelecToolsOC_LooseProbes)
        
        SelecToolsSC_LooseProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_LooseProbes_%s"%name_suffix,EffiFlag="%s_SC_LooseProbes"%name_suffix, ProbeType="Loose", SameSign=True,forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_LooseProbes.UseLooseProbes = True
        SelectionTools.append(SelecToolsSC_LooseProbes)

    
    if doMediumProbes:
        SelecToolsOC_MediumProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_MediumProbes_%s"%name_suffix,EffiFlag="%s_OC_MediumProbes"%name_suffix, ProbeType="Medium",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_MediumProbes.UseMediumProbes = True
        SelectionTools.append(SelecToolsOC_MediumProbes)
        
        SelecToolsSC_MediumProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_MediumProbes_%s"%name_suffix,EffiFlag="%s_SC_MediumProbes"%name_suffix, ProbeType="Medium", SameSign=True,forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_MediumProbes.UseMediumProbes = True
        SelectionTools.append(SelecToolsSC_MediumProbes)


    if doTightProbes:
        SelecToolsOC_TightProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_TightProbes_%s"%name_suffix,EffiFlag="%s_OC_TightProbes"%name_suffix, ProbeType="Tight",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_TightProbes.UseTightProbes = True
        SelectionTools.append(SelecToolsOC_TightProbes)
        
        SelecToolsSC_TightProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_TightProbes_%s"%name_suffix,EffiFlag="%s_SC_TightProbes"%name_suffix, ProbeType="Tight", SameSign=True,forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_TightProbes.UseTightProbes = True
        SelectionTools.append(SelecToolsSC_TightProbes)


    if doHighPtProbes:
        SelecToolsOC_HighPtProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_HighPtProbes_%s"%name_suffix,EffiFlag="%s_OC_HighPtProbes"%name_suffix, ProbeType="HighPt",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsOC_HighPtProbes.UseHighPtProbes = True
        SelectionTools.append(SelecToolsOC_HighPtProbes)
        
        SelecToolsSC_HighPtProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_HighPtProbes_%s"%name_suffix,EffiFlag="%s_SC_HighPtProbes"%name_suffix, ProbeType="HighPt", SameSign=True,forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelecToolsSC_HighPtProbes.UseHighPtProbes = True
        SelectionTools.append(SelecToolsSC_HighPtProbes)


    from AthenaCommon.GlobalFlags import globalflags
    if doTruthProbes and not globalflags.DataSource()=='data':
        SelecToolsOC_TruthProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_TruthProbes_%s"%name_suffix,EffiFlag="%s_TruthProbes"%name_suffix, ProbeType="TruthMatchedMuons",forTriggerEff=doTrig,IsRunOnDAOD=IsRunOnDAOD)
        SelectionTools.append(SelecToolsOC_TruthProbes)
        

    for thetool in SelectionTools:
        thetool.DoTagIsolation = False
        thetool.DoProbeIsolation = False
        # also accept high eta muons
        thetool.ProbeEtaCut = 4.5  
        
        
    PlotTools = []
    
    # first the trigger plots 
    if doTrig and doTriggerPlots:
        PlotTools += JPsiTPTrigAnalysis.AddTrigPlots(name_suffix=name_suffix,doEtaSlices=doTrigEtaSlices,
                                        doClosure=doClosure,
                                        DoProbeMatchPlots=DoProbeMatchPlots,
                                        ProduceEfficiencies=ProduceEfficiencies,
                                        IsRunOnDAOD=IsRunOnDAOD)
    # then the isolation plots
    if doIso and doIsolPlots:
        PlotTools += JPsiTPIsolAnalysis.AddIsolPlots(name_suffix=name_suffix,doEtaSlices=doIsolEtaSlices,
                                        doClosure=doClosure,
                                        DoProbeMatchPlots=DoProbeMatchPlots,
                                        ProduceEfficiencies=ProduceEfficiencies,
                                        IsRunOnDAOD=IsRunOnDAOD)
        
    
    MatchingTools = []
    if doTrig:
        MatchingTools += JPsiTPTrigAnalysis.AddTrigMatchTools(name_suffix=name_suffix,doL1=doL1, doHLT=doHLT,
                                                              doDRSys=doDRSys,doClosure=doClosure,doRerunMode=False)
        MatchingTools += JPsiTPTrigAnalysis.AddTrigMatchTools(name_suffix=name_suffix,doL1=doL1, doHLT=doHLT,
                                                              doDRSys=doDRSys,doClosure=doClosure,doRerunMode=True)
    if doIso:
        MatchingTools += JPsiTPIsolAnalysis.AddIsolMatchTools(name_suffix=name_suffix,
                                                               doLooseTrackOnly=doLooseTrackOnly,doLoose=doLoose,
                                                               doTight=doTight,doGradient=doGradient,
                                                               doGradientLoose=doGradientLoose,doClosure=doClosure)
    
    
    ntuples = []
    if writeNtuple:
        ntuples.append(CommonMuonTPConfig.AddTreeTool(name="JPsiTPMuonTreeTool_%s"%name_suffix, 
                                                      EffiFlag="Trees",WriteSFInfo=doClosure,
                                                      IsRunOnDAOD=IsRunOnDAOD,DoJpsiVertexInfo=True))
    
    for ntuple in ntuples:
        if doIso:
            ntuple.AddExtendedIsolation = True
        if doTrig:
            ntuple.AddExtendedTrigger = True
        
    # create the TP tool itself
    TheTPTool = CommonMuonTPConfig.AddMuonTPTool(name = "JPsiMuonProbeTPTool_%s"%name_suffix,EffiFlag=name_suffix)
    TheTPTool.MuonTPSelectionTools  = SelectionTools
    TheTPTool.PlottingTools         = PlotTools
    TheTPTool.MuonTPEfficiencyTools = MatchingTools
    TheTPTool.TreeTools             = ntuples
    theAlg.MuonTPTools += [TheTPTool]
        
#========================================================================================================================    
def AddJPsiTPMuonAnalysis(doIso = True,
                           doTrig = True,
                           doTrigEtaSlices    = True, 
                           doIsolEtaSlices    = False, 
                           doIsolPlots      = False,
                           doTriggerPlots      = True,
                           writeNtuple=False, doClosure=False, 
                           doDRSys=False, 
                           doLooseProbes=False, doMediumProbes=False, 
                           doTightProbes=False, doTruthProbes=False,
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
    AddConfiguredMuonTPAlg( name_suffix  = "MuonProbe_JPsi",
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
                            doLooseProbes   = doLooseProbes,
                            doMediumProbes  = doMediumProbes,
                            doTightProbes   = doTightProbes,
                            doHighPtProbes   = doHighPtProbes,
                            doTruthProbes   = doTruthProbes,
                            doLooseTrackOnly=doLooseTrackOnly,
                            doLoose=doLoose,
                            doTight=doTight,
                            doGradient=doGradient,
                            doGradientLoose=doGradientLoose,
                            doL1 = doL1, 
                            doHLT=doHLT,
                            DoProbeMatchPlots=DoProbeMatchPlots,
                            ProduceEfficiencies=ProduceEfficiencies,
                            IsRunOnDAOD=IsRunOnDAOD
                )
