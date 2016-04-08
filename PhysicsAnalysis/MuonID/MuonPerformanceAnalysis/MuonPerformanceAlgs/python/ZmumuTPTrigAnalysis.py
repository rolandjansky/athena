# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonPerformanceAlgs import CommonMuonTPConfig

def AddTrigPlots(name_suffix,doEtaSlices=True, 
                 doClosure=False,
                 DoProbeMatchPlots=True,
                 ProduceEfficiencies=False):
    PlotTools = []
    # add plotting tools in slices
    if doEtaSlices:
        Slices = {
                   "Barrel"     : (0.0,1.05),
                   "Endcap"     : (1.05,2.5)
                   }

        for etaslice, interval in Slices.iteritems():
            PlotTool = CommonMuonTPConfig.AddPlottingTool(name="TriggerTPPlottingTool_%s_%s"%(name_suffix,etaslice),
                                                          EffiFlag=name_suffix+"_"+etaslice,
                                                          applySF = False,
                                                          DoProbeMatchPlots=DoProbeMatchPlots,
                                                          ProduceEfficiencies=ProduceEfficiencies)
            PlotTool.ProbeAbsEtaMin = interval[0]
            PlotTool.ProbeAbsEtaMax = interval[1]
            PlotTools.append(PlotTool)
            if doClosure:
                PlotToolCL = CommonMuonTPConfig.AddPlottingTool(name="TriggerTPPlottingTool_Closure_%s_%s"%(name_suffix,etaslice),
                                                                EffiFlag=name_suffix+"_"+etaslice+"_Closure",
                                                                applySF = True,
                                                                DoProbeMatchPlots=DoProbeMatchPlots,
                                                                ProduceEfficiencies=ProduceEfficiencies)
                PlotToolCL.ProbeAbsEtaMin = interval[0]
                PlotToolCL.ProbeAbsEtaMax = interval[1]
                PlotTools.append(PlotToolCL)
            pass
                
    else:
        PlotTools = [CommonMuonTPConfig.AddPlottingTool(name="TriggerTPPlottingTool_%s_All"%name_suffix,
                                                        EffiFlag=name_suffix+"_All",
                                                        applySF = False,
                                                        DoProbeMatchPlots=DoProbeMatchPlots,
                                                        ProduceEfficiencies=ProduceEfficiencies)]
        if doClosure:
            PlotTools.append(CommonMuonTPConfig.AddPlottingTool(name="TriggerTPPlottingTool_Closure_%s_All"%name_suffix,
                                                                EffiFlag=name_suffix+"_All_Closure",
                                                                applySF = True,
                                                                DoProbeMatchPlots=DoProbeMatchPlots,
                                                                ProduceEfficiencies=ProduceEfficiencies))
        pass
    
    # configure the plot tools
    for thetool in PlotTools:
        thetool.DoAsymmErrorGraphs    = True
        thetool.doTrigValidPlots    = True
    return PlotTools

def GetTriggerList():
    TriggerList_HLT = [
                "HLT_mu14",
                "HLT_mu20_iloose_L1MU15",
                "HLT_mu24_iloose_L1MU15",
                "HLT_mu24_imedium",
                "HLT_mu26_imedium",
                "HLT_mu50",
                "HLT_mu40",
                "HLT_AllTriggers", # this is needed, if you want to have combined (inclusive) efficiency of all HLT triggers
                    ]
    TriggerList_L1 = [
                "L1_MU10",
                "L1_MU15",
                "L1_MU20",
                "L1_AllTriggers", # this is needed, if you want to have combined (inclusive) efficiency of all L1 triggers 
                ]
    TriggerList_L2 = [
                       "L2_MU20" # a dummy name
                        ]
    TriggerList_EF = [  "EF_MU30" # a dummy name
                                ]
    return {
        "L1": TriggerList_L1,
        "L2": TriggerList_L2,
        "EF": TriggerList_EF,
        "HLT":TriggerList_HLT
    }
     

def AddTrigMatchTools(name_suffix,doL1 = True, doL2=False, doEF=False,doHLT=True,doDRSys=True):
    TrigList = GetTriggerList()
    TriggerList_L1  = []
    TriggerList_L2  = []
    TriggerList_EF  = []
    TriggerList_HLT = []
    
    if doHLT:
        TriggerList_HLT = TrigList["HLT"]
    if doL1:
        TriggerList_L1 = TrigList["L1"]
    if doL2:
        TriggerList_L2 = TrigList["L2"]
    if doEF:
        TriggerList_EF = TrigList["EF"]

    MatchingTools = []

    MATCH_DR = 0.05
    for Trigitem in TriggerList_L1 + TriggerList_L2 + TriggerList_EF + TriggerList_HLT:
        thistool = CommonMuonTPConfig.AddTriggerTPEfficiencyTool(name="TriggerTPEfficiencyTool_%s_%s"%(name_suffix,Trigitem),EffiFlag=Trigitem)
        thistool.TrigItem = Trigitem
        # Delta R: 0.1 for L2/EF/HLT, 0.2 for L1 - taken from TrigMuonMatching
        MATCH_DR = 0.1
        if "L1" in Trigitem:
            MATCH_DR = 0.2
        thistool.MaximumDrCut = MATCH_DR
        MatchingTools.append(thistool)
        if doDRSys:
            thistool_dRup = CommonMuonTPConfig.AddTriggerTPEfficiencyTool(name="TriggerTPEfficiencyTool_DrUp_%s_%s"%(name_suffix,Trigitem),EffiFlag="%s_dRUp"%Trigitem)
            thistool_dRup.TrigItem = Trigitem
            thistool_dRup.IsNominal = False
            thistool_dRup.MaximumDrCut = MATCH_DR+0.05
            MatchingTools.append(thistool_dRup)

            thistool_dRDown = CommonMuonTPConfig.AddTriggerTPEfficiencyTool(name="TriggerTPEfficiencyTool_DrDown_%s_%s"%(name_suffix,Trigitem),EffiFlag="%s_dRDown"%Trigitem)
            thistool_dRDown.TrigItem = Trigitem
            thistool_dRDown.IsNominal = False
            thistool_dRDown.MaximumDrCut =  MATCH_DR-0.05
            MatchingTools.append(thistool_dRDown)

        
    return MatchingTools
    