# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonPerformanceAlgs import CommonMuonTPConfig 
  
def AddIsolPlots(name_suffix,
                 doEtaSlices=True, 
                 doClosure=False,
                 DoProbeMatchPlots=True,
                 ProduceEfficiencies=False):
    
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
            PlotTool = CommonMuonTPConfig.AddPlottingTool(name="JPsiTPIsoPlottingTool_%s_%s"%(name_suffix,etaslice),EffiFlag=name_suffix+"_"+etaslice,applySF = False,DoProbeMatchPlots=DoProbeMatchPlots,ProduceEfficiencies=ProduceEfficiencies)
            PlotTool.ProbeAbsEtaMin = interval[0]
            PlotTool.ProbeAbsEtaMax = interval[1]
            PlotTools.append(PlotTool)
            if doClosure:
                PlotToolCL = CommonMuonTPConfig.AddPlottingTool(name="JPsiTPIsoPlottingToolClosure_%s_%s"%(name_suffix,etaslice),EffiFlag=name_suffix+"_"+etaslice+"_Closure",applySF = True,DoProbeMatchPlots=DoProbeMatchPlots,ProduceEfficiencies=ProduceEfficiencies)
                PlotToolCL.ProbeAbsEtaMin = interval[0]
                PlotToolCL.ProbeAbsEtaMax = interval[1]
                PlotTools.append(PlotToolCL)
                
    else:
        PlotTools = [CommonMuonTPConfig.AddPlottingTool(name="JPsiTPIsoPlottingTool_%s_All"%name_suffix,EffiFlag=name_suffix+"_All",applySF = False,DoProbeMatchPlots=DoProbeMatchPlots,ProduceEfficiencies=ProduceEfficiencies)]
        if doClosure:
            PlotTools.append(CommonMuonTPConfig.AddPlottingTool(name="JPsiTPIsoPlottingToolClosure_%s_All"%name_suffix,EffiFlag=name_suffix+"_All_Closure",applySF = True,DoProbeMatchPlots=DoProbeMatchPlots,ProduceEfficiencies=ProduceEfficiencies))
    # config the plot tools for Zmm isolation
    for thetool in PlotTools:
        thetool.doJPsiKinematicPlots = True
        thetool.doFineEtaPhiPlots = True
        thetool.DoAsymmErrorGraphs = True
        thetool.doJPsiDileptonPlots = True
        
    return PlotTools
    
def AddIsolMatchTools(name_suffix):
    #MATCH_DR = 0.05
    #MatchingTools = []
    #thistool = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="ZmumuTPIsoEfficiencyTool_%s_CB"%name_suffix,EffiFlag="Isolation")
    #thistool.ApplyScaleFactors = False
    #thistool.MaximumDrCut = MATCH_DR
    #MatchingTools += [thistool]
    #return MatchingTools
    return []
     
     
     