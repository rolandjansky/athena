# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonPerformanceAlgs import CommonMuonTPConfig

#========================================================================================================================
def AddTrigPlots(name_suffix,
                 doEtaSlices=True, 
                 doClosure=False,
                 DoProbeMatchPlots=True,
                 ProduceEfficiencies=False,
                 IsRunOnDAOD=False):
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
                                                          ProduceEfficiencies=ProduceEfficiencies,
                                                          IsRunOnDAOD=IsRunOnDAOD)
            PlotTool.ProbeAbsEtaMin = interval[0]
            PlotTool.ProbeAbsEtaMax = interval[1]
            PlotTools.append(PlotTool)
            if doClosure:
                PlotToolCL = CommonMuonTPConfig.AddPlottingTool(name="TriggerTPPlottingTool_Closure_%s_%s"%(name_suffix,etaslice),
                                                                EffiFlag=name_suffix+"_"+etaslice+"_Closure",
                                                                applySF = True,
                                                                DoProbeMatchPlots=DoProbeMatchPlots,
                                                                ProduceEfficiencies=ProduceEfficiencies,
                                                                IsRunOnDAOD=IsRunOnDAOD)
                PlotToolCL.ProbeAbsEtaMin = interval[0]
                PlotToolCL.ProbeAbsEtaMax = interval[1]
                PlotTools.append(PlotToolCL)
            pass
                
    else:
        PlotTools = [CommonMuonTPConfig.AddPlottingTool(name="TriggerTPPlottingTool_%s_All"%name_suffix,
                                                        EffiFlag=name_suffix+"_All",
                                                        applySF = False,
                                                        DoProbeMatchPlots=DoProbeMatchPlots,
                                                        ProduceEfficiencies=ProduceEfficiencies,
                                                        IsRunOnDAOD=IsRunOnDAOD)]
        if doClosure:
            PlotTools.append(CommonMuonTPConfig.AddPlottingTool(name="TriggerTPPlottingTool_Closure_%s_All"%name_suffix,
                                                                EffiFlag=name_suffix+"_All_Closure",
                                                                applySF = True,
                                                                DoProbeMatchPlots=DoProbeMatchPlots,
                                                                ProduceEfficiencies=ProduceEfficiencies,
                                                                IsRunOnDAOD=IsRunOnDAOD))
        pass
    
    # configure the plot tools
    for thetool in PlotTools:
        thetool.DoAsymmErrorGraphs    = True
        thetool.doTrigValidPlots    = True
    return PlotTools

#========================================================================================================================
def GetTriggerList():
    TriggerList_HLT = [
                "HLT_noalg_L1MU10",
                "HLT_mu4",
                "HLT_mu6",
                "HLT_mu10",
                "HLT_mu14",
                "HLT_mu18",
                "HLT_mu20_msonly_mu6noL1_msonly_nscan05 ",
                "HLT_mu20_2mu0noL1_JpsimumuFS",
                "HLT_mu18_2mu0noL1_JpsimumuFS",
                "HLT_mu20_2mu4_JpsimumuL2",
                "HLT_mu18_2mu4_JpsimumuL2",
                "HLT_AllTriggers", # this is needed, if you want to have combined (inclusive) efficiency of all HLT triggers
                "HLT_mu18_mu8noL1", # needed to compute multi-muon trigger SFs
                "HLT_mu22_mu8noL1", # needed to compute multi-muon trigger SFs
                "HLT_mu24_mu8noL1"  # needed to compute multi-muon trigger SFs
                ]
    TriggerList_L1 = [
                "L1_MU10",
                "L1_MU15",
                "L1_AllTriggers", # this is needed, if you want to have combined (inclusive) efficiency of all L1 triggers 
                ]

    return {
        "L1": TriggerList_L1,
        "HLT":TriggerList_HLT
    }
     
#========================================================================================================================
# needed to compute multi-muon trigger SFs
def GetTriggerList_RerunMode(): 
    TriggerList_HLT = ["HLT_mu6",
                       "HLT_mu10",
                       "HLT_mu14",
                       "HLT_mu18",
                       "HLT_mu22",
                       "HLT_mu24"]
    TriggerList_L1  = []

    return {
        "L1": TriggerList_L1,
        "HLT":TriggerList_HLT
    }

#========================================================================================================================
def AddTrigMatchTools(name_suffix, doL1=True, doHLT=True, doDRSys=True, doClosure=False, doRerunMode=False):

    if doRerunMode:
        TrigList = GetTriggerList_RerunMode()
    else:
        TrigList = GetTriggerList()

    TriggerList_L1  = []
    TriggerList_HLT = []
    
    if doHLT:
        TriggerList_HLT = TrigList["HLT"]
    if doL1:
        TriggerList_L1 = TrigList["L1"]

    MatchingTools = []

    for Trigitem in TriggerList_L1 + TriggerList_HLT:
        thistool = CommonMuonTPConfig.AddTriggerTPEfficiencyTool(name="TriggerTPEfficiencyTool_%s_%s"%(name_suffix,Trigitem),EffiFlag=Trigitem,doClosure=doClosure,IsNominal = True)
        thistool.TrigItem = Trigitem 
        thistool.RerunMode = doRerunMode

        # Delta R: 0.1 for L2/EF/HLT, 0.2 for L1 - taken from TrigMuonMatching
        MATCH_DR = 0.1
        if "L1" in Trigitem:
            MATCH_DR = 0.2

        thistool.MaximumDrCut = MATCH_DR

        if not Trigitem in ["HLT_mu14","HLT_mu20_iloose_L1MU15", "HLT_mu24_iloose_L1MU15","HLT_mu24_imedium", "HLT_mu26_imedium"]:
            thistool.ApplyScaleFactors = False
        MatchingTools.append(thistool)

        if doDRSys:
            thistool_dRup = CommonMuonTPConfig.AddTriggerTPEfficiencyTool(name="TriggerTPEfficiencyTool_DrUp_%s_%s"%(name_suffix,Trigitem),EffiFlag="%s_dRUp"%Trigitem,doClosure=doClosure,IsNominal = False)
            thistool_dRup.TrigItem = Trigitem
            thistool.RerunMode = doRerunMode
            thistool_dRup.MaximumDrCut = MATCH_DR+0.05
            MatchingTools.append(thistool_dRup)

            thistool_dRDown = CommonMuonTPConfig.AddTriggerTPEfficiencyTool(name="TriggerTPEfficiencyTool_DrDown_%s_%s"%(name_suffix,Trigitem),EffiFlag="%s_dRDown"%Trigitem,doClosure=doClosure,IsNominal = False)
            thistool_dRDown.TrigItem = Trigitem
            thistool.RerunMode = doRerunMode
            thistool_dRDown.MaximumDrCut =  MATCH_DR-0.05
            MatchingTools.append(thistool_dRDown)
        
    return MatchingTools
    
