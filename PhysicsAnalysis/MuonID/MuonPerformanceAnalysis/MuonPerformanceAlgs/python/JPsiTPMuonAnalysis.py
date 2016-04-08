# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonPerformanceAlgs import CommonMuonTPConfig, JPsiTPTrigAnalysis, JPsiTPIsolAnalysis

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
                            doDPhiTPSys    = False,
                            doProbeCharSys = False,
                            doVeryLooseProbes = False,
                            doLooseProbes   = False,
                            doMediumProbes  = False,
                            doTightProbes   = False,
                            doHighPtProbes   = False,
                            doTruthProbes   = False,
                            DoProbeMatchPlots=True,
                            ProduceEfficiencies=False,
                            doL1 = False,
                            doL2 = False, doEF = False,
                            doHLT = False
                ):
    
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = "Muons"
    if hasattr(job, "MuonQualityUpdater"):
        MuonContainerToUse = "UpdatedMuons"

    ProbeContainer = MuonContainerToUse
    theAlg = CommonMuonTPConfig.AddTagProbeAlg(name="JPsiTPMuonAlg_%s"%name_suffix, ProbeCont=ProbeContainer, MatchCont=MatchContainer)
    theAlg.TopLevelFolderName = "JPsiTPMuon"

    SelectionTools = []
    
    if doLooseProbes:
        SelecToolsOC_LooseProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_LooseProbes_%s"%name_suffix,EffiFlag="%s_OC_LooseProbes"%name_suffix, ProbeType="Loose")
        SelecToolsOC_LooseProbes.UseLooseProbes = True
        SelectionTools.append(SelecToolsOC_LooseProbes)
        
        SelecToolsSC_LooseProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_LooseProbes_%s"%name_suffix,EffiFlag="%s_SC_LooseProbes"%name_suffix, ProbeType="Loose", SameSign=True)
        SelecToolsSC_LooseProbes.UseLooseProbes = True
        SelectionTools.append(SelecToolsSC_LooseProbes)

        
        if doDPhiTPSys:
            SelecToolsOC_LooseProbesDPhiCut = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_LooseProbesDPhiCut_%s"%name_suffix,EffiFlag="%s_OC_LooseProbesDPhiCut"%name_suffix, ProbeType="Loose")
            SelecToolsOC_LooseProbesDPhiCut.DeltaPhiCut  = 3.04
            SelecToolsOC_LooseProbesDPhiCut.IsNominal = False
            SelectionTools.append(SelecToolsOC_LooseProbesDPhiCut)
            
            SelecToolsSC_LooseProbesDPhiCut = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_LooseProbesDPhiCut_%s"%name_suffix,EffiFlag="%s_SC_LooseProbesDPhiCut"%name_suffix, ProbeType="Loose", SameSign=True)
            SelecToolsSC_LooseProbesDPhiCut.DeltaPhiCut  = 3.04
            SelecToolsSC_LooseProbesDPhiCut.IsNominal = False
            SelectionTools.append(SelecToolsSC_LooseProbesDPhiCut)

        if doProbeCharSys:
            SelecToolsOC_LooseProbesPos = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_LooseProbesPos_%s"%name_suffix,EffiFlag="%s_OC_LooseProbesPos"%name_suffix, ProbeType="Loose")
            SelecToolsOC_LooseProbesPos.doProbeChargeSys = True
            SelecToolsOC_LooseProbesPos.ProbeCharge = "positive"
            SelecToolsOC_LooseProbesPos.IsNominal = False
            SelectionTools.append(SelecToolsOC_LooseProbesPos)

            SelecToolsOC_LooseProbesNeg = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_LooseProbesNeg_%s"%name_suffix,EffiFlag="%s_OC_LooseProbesNeg"%name_suffix, ProbeType="Loose")
            SelecToolsOC_LooseProbesNeg.doProbeChargeSys = True
            SelecToolsOC_LooseProbesNeg.IsNominal = False
            SelecToolsOC_LooseProbesNeg.ProbeCharge = "negative"
            SelectionTools.append(SelecToolsOC_LooseProbesNeg)
            
            
            SelecToolsSC_LooseProbesPos = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_LooseProbesPos_%s"%name_suffix,EffiFlag="%s_SC_LooseProbesPos"%name_suffix, ProbeType="Loose", SameSign=True)
            SelecToolsSC_LooseProbesPos.doProbeChargeSys = True
            SelecToolsSC_LooseProbesPos.ProbeCharge = "positive"
            SelecToolsSC_LooseProbesPos.IsNominal = False
            SelectionTools.append(SelecToolsSC_LooseProbesPos)

            SelecToolsSC_LooseProbesNeg = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_LooseProbesNeg_%s"%name_suffix,EffiFlag="%s_SC_LooseProbesNeg"%name_suffix, ProbeType="Loose", SameSign=True)
            SelecToolsSC_LooseProbesNeg.doProbeChargeSys = True
            SelecToolsSC_LooseProbesNeg.IsNominal = False
            SelecToolsSC_LooseProbesNeg.ProbeCharge = "negative"
            SelectionTools.append(SelecToolsSC_LooseProbesNeg)

    
    if doMediumProbes:
        SelecToolsOC_MediumProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_MediumProbes_%s"%name_suffix,EffiFlag="%s_OC_MediumProbes"%name_suffix, ProbeType="Medium")
        SelecToolsOC_MediumProbes.UseMediumProbes = True
        SelectionTools.append(SelecToolsOC_MediumProbes)
        
        SelecToolsSC_MediumProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_MediumProbes_%s"%name_suffix,EffiFlag="%s_SC_MediumProbes"%name_suffix, ProbeType="Medium", SameSign=True)
        SelecToolsSC_MediumProbes.UseMediumProbes = True
        SelectionTools.append(SelecToolsSC_MediumProbes)

        
        if doDPhiTPSys:
            SelecToolsOC_MediumProbesDPhiCut = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_MediumProbesDPhiCut_%s"%name_suffix,EffiFlag="%s_OC_MediumProbesDPhiCut"%name_suffix, ProbeType="Medium")
            SelecToolsOC_MediumProbesDPhiCut.DeltaPhiCut  = 3.04
            SelecToolsOC_MediumProbesDPhiCut.IsNominal = False
            SelectionTools.append(SelecToolsOC_MediumProbesDPhiCut)
            
            SelecToolsSC_MediumProbesDPhiCut = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_MediumProbesDPhiCut_%s"%name_suffix,EffiFlag="%s_SC_MediumProbesDPhiCut"%name_suffix, ProbeType="Medium", SameSign=True)
            SelecToolsSC_MediumProbesDPhiCut.DeltaPhiCut  = 3.04
            SelecToolsSC_MediumProbesDPhiCut.IsNominal = False
            SelectionTools.append(SelecToolsSC_MediumProbesDPhiCut)

        if doProbeCharSys:
            SelecToolsOC_MediumProbesPos = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_MediumProbesPos_%s"%name_suffix,EffiFlag="%s_OC_MediumProbesPos"%name_suffix, ProbeType="Medium")
            SelecToolsOC_MediumProbesPos.doProbeChargeSys = True
            SelecToolsOC_MediumProbesPos.ProbeCharge = "positive"
            SelecToolsOC_MediumProbesPos.IsNominal = False
            SelectionTools.append(SelecToolsOC_MediumProbesPos)

            SelecToolsOC_MediumProbesNeg = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_MediumProbesNeg_%s"%name_suffix,EffiFlag="%s_OC_MediumProbesNeg"%name_suffix, ProbeType="Medium")
            SelecToolsOC_MediumProbesNeg.doProbeChargeSys = True
            SelecToolsOC_MediumProbesNeg.IsNominal = False
            SelecToolsOC_MediumProbesNeg.ProbeCharge = "negative"
            SelectionTools.append(SelecToolsOC_MediumProbesNeg)
            
            
            SelecToolsSC_MediumProbesPos = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_MediumProbesPos_%s"%name_suffix,EffiFlag="%s_SC_MediumProbesPos"%name_suffix, ProbeType="Medium", SameSign=True)
            SelecToolsSC_MediumProbesPos.doProbeChargeSys = True
            SelecToolsSC_MediumProbesPos.ProbeCharge = "positive"
            SelecToolsSC_MediumProbesPos.IsNominal = False
            SelectionTools.append(SelecToolsSC_MediumProbesPos)

            SelecToolsSC_MediumProbesNeg = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_MediumProbesNeg_%s"%name_suffix,EffiFlag="%s_SC_MediumProbesNeg"%name_suffix, ProbeType="Medium", SameSign=True)
            SelecToolsSC_MediumProbesNeg.doProbeChargeSys = True
            SelecToolsSC_MediumProbesNeg.IsNominal = False
            SelecToolsSC_MediumProbesNeg.ProbeCharge = "negative"
            SelectionTools.append(SelecToolsSC_MediumProbesNeg)

    if doTightProbes:
        SelecToolsOC_TightProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_TightProbes_%s"%name_suffix,EffiFlag="%s_OC_TightProbes"%name_suffix, ProbeType="Tight")
        SelecToolsOC_TightProbes.UseTightProbes = True
        SelectionTools.append(SelecToolsOC_TightProbes)
        
        SelecToolsSC_TightProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_TightProbes_%s"%name_suffix,EffiFlag="%s_SC_TightProbes"%name_suffix, ProbeType="Tight", SameSign=True)
        SelecToolsSC_TightProbes.UseTightProbes = True
        SelectionTools.append(SelecToolsSC_TightProbes)

        
        if doDPhiTPSys:
            SelecToolsOC_TightProbesDPhiCut = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_TightProbesDPhiCut_%s"%name_suffix,EffiFlag="%s_OC_TightProbesDPhiCut"%name_suffix, ProbeType="Tight")
            SelecToolsOC_TightProbesDPhiCut.DeltaPhiCut  = 3.04
            SelecToolsOC_TightProbesDPhiCut.IsNominal = False
            SelectionTools.append(SelecToolsOC_TightProbesDPhiCut)
            
            SelecToolsSC_TightProbesDPhiCut = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_TightProbesDPhiCut_%s"%name_suffix,EffiFlag="%s_SC_TightProbesDPhiCut"%name_suffix, ProbeType="Tight", SameSign=True)
            SelecToolsSC_TightProbesDPhiCut.DeltaPhiCut  = 3.04
            SelecToolsSC_TightProbesDPhiCut.IsNominal = False
            SelectionTools.append(SelecToolsSC_TightProbesDPhiCut)

        if doProbeCharSys:
            SelecToolsOC_TightProbesPos = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_TightProbesPos_%s"%name_suffix,EffiFlag="%s_OC_TightProbesPos"%name_suffix, ProbeType="Tight")
            SelecToolsOC_TightProbesPos.doProbeChargeSys = True
            SelecToolsOC_TightProbesPos.ProbeCharge = "positive"
            SelecToolsOC_TightProbesPos.IsNominal = False
            SelectionTools.append(SelecToolsOC_TightProbesPos)

            SelecToolsOC_TightProbesNeg = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_TightProbesNeg_%s"%name_suffix,EffiFlag="%s_OC_TightProbesNeg"%name_suffix, ProbeType="Tight")
            SelecToolsOC_TightProbesNeg.doProbeChargeSys = True
            SelecToolsOC_TightProbesNeg.IsNominal = False
            SelecToolsOC_TightProbesNeg.ProbeCharge = "negative"
            SelectionTools.append(SelecToolsOC_TightProbesNeg)
            
            
            SelecToolsSC_TightProbesPos = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_TightProbesPos_%s"%name_suffix,EffiFlag="%s_SC_TightProbesPos"%name_suffix, ProbeType="Tight", SameSign=True)
            SelecToolsSC_TightProbesPos.doProbeChargeSys = True
            SelecToolsSC_TightProbesPos.ProbeCharge = "positive"
            SelecToolsSC_TightProbesPos.IsNominal = False
            SelectionTools.append(SelecToolsSC_TightProbesPos)

            SelecToolsSC_TightProbesNeg = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_TightProbesNeg_%s"%name_suffix,EffiFlag="%s_SC_TightProbesNeg"%name_suffix, ProbeType="Tight", SameSign=True)
            SelecToolsSC_TightProbesNeg.doProbeChargeSys = True
            SelecToolsSC_TightProbesNeg.IsNominal = False
            SelecToolsSC_TightProbesNeg.ProbeCharge = "negative"
            SelectionTools.append(SelecToolsSC_TightProbesNeg)


    if doHighPtProbes:
        SelecToolsOC_HighPtProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_HighPtProbes_%s"%name_suffix,EffiFlag="%s_OC_HighPtProbes"%name_suffix, ProbeType="HighPt")
        SelecToolsOC_HighPtProbes.UseHighPtProbes = True
        SelectionTools.append(SelecToolsOC_HighPtProbes)
        
        SelecToolsSC_HighPtProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_HighPtProbes_%s"%name_suffix,EffiFlag="%s_SC_HighPtProbes"%name_suffix, ProbeType="HighPt", SameSign=True)
        SelecToolsSC_HighPtProbes.UseHighPtProbes = True
        SelectionTools.append(SelecToolsSC_HighPtProbes)

        
        if doDPhiTPSys:
            SelecToolsOC_HighPtProbesDPhiCut = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_HighPtProbesDPhiCut_%s"%name_suffix,EffiFlag="%s_OC_HighPtProbesDPhiCut"%name_suffix, ProbeType="HighPt")
            SelecToolsOC_HighPtProbesDPhiCut.DeltaPhiCut  = 3.04
            SelecToolsOC_HighPtProbesDPhiCut.IsNominal = False
            SelectionTools.append(SelecToolsOC_HighPtProbesDPhiCut)
            
            SelecToolsSC_HighPtProbesDPhiCut = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_HighPtProbesDPhiCut_%s"%name_suffix,EffiFlag="%s_SC_HighPtProbesDPhiCut"%name_suffix, ProbeType="HighPt", SameSign=True)
            SelecToolsSC_HighPtProbesDPhiCut.DeltaPhiCut  = 3.04
            SelecToolsSC_HighPtProbesDPhiCut.IsNominal = False
            SelectionTools.append(SelecToolsSC_HighPtProbesDPhiCut)

        if doProbeCharSys:
            SelecToolsOC_HighPtProbesPos = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_HighPtProbesPos_%s"%name_suffix,EffiFlag="%s_OC_HighPtProbesPos"%name_suffix, ProbeType="HighPt")
            SelecToolsOC_HighPtProbesPos.doProbeChargeSys = True
            SelecToolsOC_HighPtProbesPos.ProbeCharge = "positive"
            SelecToolsOC_HighPtProbesPos.IsNominal = False
            SelectionTools.append(SelecToolsOC_HighPtProbesPos)

            SelecToolsOC_HighPtProbesNeg = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_HighPtProbesNeg_%s"%name_suffix,EffiFlag="%s_OC_HighPtProbesNeg"%name_suffix, ProbeType="HighPt")
            SelecToolsOC_HighPtProbesNeg.doProbeChargeSys = True
            SelecToolsOC_HighPtProbesNeg.IsNominal = False
            SelecToolsOC_HighPtProbesNeg.ProbeCharge = "negative"
            SelectionTools.append(SelecToolsOC_HighPtProbesNeg)
            
            
            SelecToolsSC_HighPtProbesPos = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_HighPtProbesPos_%s"%name_suffix,EffiFlag="%s_SC_HighPtProbesPos"%name_suffix, ProbeType="HighPt", SameSign=True)
            SelecToolsSC_HighPtProbesPos.doProbeChargeSys = True
            SelecToolsSC_HighPtProbesPos.ProbeCharge = "positive"
            SelecToolsSC_HighPtProbesPos.IsNominal = False
            SelectionTools.append(SelecToolsSC_HighPtProbesPos)

            SelecToolsSC_HighPtProbesNeg = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_HighPtProbesNeg_%s"%name_suffix,EffiFlag="%s_SC_HighPtProbesNeg"%name_suffix, ProbeType="HighPt", SameSign=True)
            SelecToolsSC_HighPtProbesNeg.doProbeChargeSys = True
            SelecToolsSC_HighPtProbesNeg.IsNominal = False
            SelecToolsSC_HighPtProbesNeg.ProbeCharge = "negative"
            SelectionTools.append(SelecToolsSC_HighPtProbesNeg)

    from AthenaCommon.GlobalFlags import globalflags
    if doTruthProbes and not globalflags.DataSource()=='data':
        SelecToolsOC_TruthProbes = CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_TruthProbes_%s"%name_suffix,EffiFlag="%s_TruthProbes"%name_suffix, ProbeType="TruthMatchedMuons")
        SelectionTools.append(SelecToolsOC_TruthProbes)
        

    for thetool in SelectionTools:
        thetool.ProbeTrackIsoCut = -1
        thetool.ProbeTrackAntiIsoCut = -1
        thetool.ProbeCaloIsoCut = -1
        thetool.ProbeCaloAntiIsoCut = -1
        # also accept high eta muons
        thetool.ProbeEtaCut = 4.5  
        
    PlotTools = []
    
    # first the trigger plots 
    if doTrig and doTriggerPlots:
        PlotTools += JPsiTPTrigAnalysis.AddTrigPlots(name_suffix=name_suffix,doEtaSlices=doTrigEtaSlices,
                                        doClosure=doClosure,
                                        DoProbeMatchPlots=DoProbeMatchPlots,
                                        ProduceEfficiencies=ProduceEfficiencies)
    # then the isolation plots
    if doIso and doIsolPlots:
        PlotTools += JPsiTPIsolAnalysis.AddIsolPlots(name_suffix=name_suffix,doEtaSlices=doIsolEtaSlices,
                                        doClosure=doClosure,
                                        DoProbeMatchPlots=DoProbeMatchPlots,
                                        ProduceEfficiencies=ProduceEfficiencies)
        
    
    MatchingTools = []
    if doTrig:
        MatchingTools += JPsiTPTrigAnalysis.AddTrigMatchTools(name_suffix=name_suffix,doL1 = doL1, doL2=doL2, doEF=doEF,doHLT=doHLT,doDRSys=doDRSys)
    if doIso:
        MatchingTools += JPsiTPIsolAnalysis.AddIsolMatchTools(name_suffix)
    
    
    ntuples = []
    if writeNtuple:
        ntuples.append(CommonMuonTPConfig.AddTreeTool(name="JPsiTPMuonTreeTool_%s"%name_suffix, EffiFlag="Trees",WriteSFInfo=doClosure))
    
    for ntuple in ntuples:
        if doIso:
            ntuple.AddExtendedIsolation = True
        
    # create the TP tool itself
    TheTPTool = CommonMuonTPConfig.AddMuonTPTool(name = "MuonProbeTPTool_%s"%name_suffix,EffiFlag=name_suffix)
    TheTPTool.MuonTPSelectionTools  = SelectionTools
    TheTPTool.PlottingTools         = PlotTools
    TheTPTool.MuonTPEfficiencyTools = MatchingTools
    TheTPTool.TreeTools             = ntuples
    theAlg.MuonTPTools += [TheTPTool]
    
    
    
def AddJPsiTPMuonAnalysis(doIso = True,
                           doTrig = True,
                           doTrigEtaSlices    = True, 
                           doIsolEtaSlices    = False, 
                           doIsolPlots      = False,
                           doTriggerPlots      = True,
                           writeNtuple=False, doClosure=False, 
                           doDRSys=False, doDPhiTPSys=True, 
                           doProbeCharSys=True, 
                           doLooseProbes=False, doMediumProbes=False, 
                           doTightProbes=False, doTruthProbes=False,
                           doHighPtProbes=False,
                           doL1=False, doL2=False, doEF=False, 
                           doHLT=False, DoProbeMatchPlots=True, 
                           ProduceEfficiencies=False):
    
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()


    MuonContainerToUse = "Muons"
    if hasattr(job, "MuonQualityUpdater"):
        MuonContainerToUse = "UpdatedMuons"
        
    ##########################################################################################
    # Add the Zmm TP algorithm for muon probes
    AddConfiguredMuonTPAlg( name_suffix  = "MuonProbe_JPsi",
                            MatchContainer = "Muons",
                            doIso=doIso,
                            doTrig=doTrig,
                            doTrigEtaSlices    = doTrigEtaSlices, 
                            doIsolEtaSlices    = doIsolEtaSlices, 
                            doIsolPlots      = doIsolPlots,
                            doTriggerPlots      = doTriggerPlots,
                            writeNtuple    = writeNtuple,
                            doClosure      = doClosure,
                            doDRSys      = doDRSys,
                            doDPhiTPSys    = doDPhiTPSys,
                            doProbeCharSys = doProbeCharSys,
                            doLooseProbes   = doLooseProbes,
                            doMediumProbes  = doMediumProbes,
                            doTightProbes   = doTightProbes,
                            doHighPtProbes   = doHighPtProbes,
                            doTruthProbes   = doTruthProbes,
                            doL1 = doL1, doL2=doL2, 
                            doEF=doEF,doHLT=doHLT,
                            DoProbeMatchPlots=DoProbeMatchPlots,
                            ProduceEfficiencies=ProduceEfficiencies
                )