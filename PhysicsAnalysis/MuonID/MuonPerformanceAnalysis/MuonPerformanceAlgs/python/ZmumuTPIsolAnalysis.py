# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonPerformanceAlgs import CommonMuonTPConfig 
  
def AddIsolPlots(name_suffix,
                 doEtaSlices=True, 
                 doClosure=False,
                 DoProbeMatchPlots=True,
                 ProduceEfficiencies=False,
                 IsRunOnDAOD=False):
    
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
            PlotTool = CommonMuonTPConfig.AddPlottingTool(name="ZmumuTPIsoPlottingTool_%s_%s"%(name_suffix,etaslice),
                                                        IsRunOnDAOD=IsRunOnDAOD,EffiFlag=name_suffix+"_"+etaslice,
                                                        applySF = False,DoProbeMatchPlots=DoProbeMatchPlots,
                                                        ProduceEfficiencies=ProduceEfficiencies)
            PlotTool.ProbeAbsEtaMin = interval[0]
            PlotTool.ProbeAbsEtaMax = interval[1]
            PlotTools.append(PlotTool)
            if doClosure:
                PlotToolCL = CommonMuonTPConfig.AddPlottingTool(name="ZmumuTPIsoPlottingToolClosure_%s_%s"%(name_suffix,etaslice),
                                                                EffiFlag=name_suffix+"_"+etaslice+"_Closure",
                                                                applySF = True,DoProbeMatchPlots=DoProbeMatchPlots,
                                                                ProduceEfficiencies=ProduceEfficiencies,
                                                                IsRunOnDAOD=IsRunOnDAOD)
                PlotToolCL.ProbeAbsEtaMin = interval[0]
                PlotToolCL.ProbeAbsEtaMax = interval[1]
                PlotTools.append(PlotToolCL)
                
    else:
        PlotTools = [CommonMuonTPConfig.AddPlottingTool(name="ZmumuTPIsoPlottingTool_%s_All"%name_suffix,EffiFlag=name_suffix+"_All",
                                                        applySF = False,DoProbeMatchPlots=DoProbeMatchPlots,
                                                        ProduceEfficiencies=ProduceEfficiencies,
                                                        IsRunOnDAOD=IsRunOnDAOD)]
        if doClosure:
            PlotTools.append(CommonMuonTPConfig.AddPlottingTool(name="ZmumuTPIsoPlottingToolClosure_%s_All"%name_suffix,
                                                                EffiFlag=name_suffix+"_All_Closure",applySF = True,
                                                                DoProbeMatchPlots=DoProbeMatchPlots,
                                                                ProduceEfficiencies=ProduceEfficiencies,
                                                                IsRunOnDAOD=IsRunOnDAOD))
    # config the plot tools for Zmm isolation
    for thetool in PlotTools:
        thetool.doZmumuKinematicPlots = True
        thetool.doFineEtaPhiPlots = True
        thetool.DoAsymmErrorGraphs = True
        thetool.doZmumuDileptonPlots = True
        
    return PlotTools
    
def AddIsolMatchTools(name_suffix,doLooseTrackOnly=True,doLoose=True,doTight=True,doGradient=True,doGradientLoose=True,doFixedCutLoose=True,doFixedCutTightTrackOnly=True,doClosure=True ):
    MatchingTools = []
    if doLooseTrackOnly:
        thistool_LooseTrackOnly = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_%s_LooseTrackOnly"%name_suffix,EffiFlag="IsoLooseTrackOnly",doClosure=doClosure,IsNominal=True)
        thistool_LooseTrackOnly.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("LooseTrackOnly")
        thistool_LooseTrackOnly.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("LooseTrackOnlyIso",doClosure=doClosure)
        MatchingTools += [thistool_LooseTrackOnly]

    # temporary since other isolation SFs still not available
    doClosure=False

    if doLoose:
        thistool_Loose = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_%s_Loose"%name_suffix,EffiFlag="IsoLoose",doClosure=doClosure,IsNominal=True)
        thistool_Loose.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("Loose")
        thistool_Loose.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("LooseIso",doClosure=doClosure)
        MatchingTools += [thistool_Loose]
    if doTight:
        thistool_Tight = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_%s_Tight"%name_suffix,EffiFlag="IsoTight",doClosure=doClosure,IsNominal=True)
        thistool_Tight.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("Tight")
        thistool_Tight.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("TightIso",doClosure=doClosure)
        MatchingTools += [thistool_Tight]
    if doGradient:
        thistool_Gradient = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_%s_Gradient"%name_suffix,EffiFlag="IsoGradient",doClosure=doClosure,IsNominal=True)
        thistool_Gradient.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("Gradient")
        thistool_Gradient.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("GradientIso",doClosure=doClosure)
        MatchingTools += [thistool_Gradient]
    if doGradientLoose:
        thistool_GradientLoose = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_%s_GradientLoose"%name_suffix,EffiFlag="IsoGradientLoose",doClosure=doClosure,IsNominal=True)
        thistool_GradientLoose.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("GradientLoose")
        thistool_GradientLoose.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("GradientLooseIso",doClosure=doClosure)
        MatchingTools += [thistool_GradientLoose]
    if doFixedCutLoose:
        thistool_FixedCutLoose = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_%s_FixedCutLoose"%name_suffix,EffiFlag="IsoFixedCutLoose",doClosure=doClosure,IsNominal=True)
        thistool_FixedCutLoose.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("FixedCutLoose")
        thistool_FixedCutLoose.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("FixedCutLooseIso",doClosure=doClosure)
        MatchingTools += [thistool_FixedCutLoose]
    if doFixedCutTightTrackOnly:
        thistool_FixedCutTightTrackOnly = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_%s_FixedCutTightTrackOnly"%name_suffix,EffiFlag="IsoFixedCutTightTrackOnly",doClosure=doClosure,IsNominal=True)
        thistool_FixedCutTightTrackOnly.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("FixedCutTightTrackOnly")
        thistool_FixedCutTightTrackOnly.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("FixedCutTightTrackOnlyIso",doClosure=doClosure)
        MatchingTools += [thistool_FixedCutTightTrackOnly]
        
    return MatchingTools
     
