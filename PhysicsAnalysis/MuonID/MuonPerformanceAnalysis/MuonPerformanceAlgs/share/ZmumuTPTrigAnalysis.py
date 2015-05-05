
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc

job = AlgSequence()

from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors

# ============================================================
# Add instance of the TP plotting tool
def AddTriggerTPPlotTool(name, EffiFlag, applySF=False):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import  MuonTrigTPPlotTool
    thetool = MuonTrigTPPlotTool(name)
    thetool.EfficiencyFlag        = EffiFlag
    thetool.doTrkValidPlots       = False
    thetool.doBasicKinematicPlots = True
    thetool.ApplyScaleFactors     = applySF
    thetool.DoAsymmErrorGraphs    = True
    ToolSvc+= thetool
    return thetool

# ============================================================
# Add a tree filling tool
def AddTriggerTPTreeTool (name, EffiFlag):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import MuonTrigTPTreeTool
    thetool = MuonTrigTPTreeTool(name)
    thetool.EfficiencyFlag = EffiFlag
    ToolSvc+= thetool
    return thetool

# ============================================================
def AddTriggerTPSelectionTool (name, EffiFlag, SameSign=False, AntiIso=False):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import ZmumuMuonTPSelectionTool
    thetool = ZmumuMuonTPSelectionTool(name)
    
    if hasattr(ToolSvc, "MyTrackIsoTool"):
        thetool.TrackIsolationTool = ToolSvc.MyTrackIsoTool
        thetool.CaloIsolationTool = ToolSvc.MyCaloIsoTool
        
    # pt cuts
    thetool.TagPtCut       = 25000.
    thetool.ProbePtCut     =  5000.
    # Z window 
    thetool.HighMassWindow = 101000.
    thetool.LowMassWindow  = 81000.
    #thetool.OutputLevel   = DEBUG      # this is a bit spammy
    thetool.ProbeTrackIsoCut = -1.
    
    # Tag isolation 
    thetool.TagTrackIsoCut     = 0.2
    thetool.TagCaloIsoCut      = 0.2
    thetool.TagTrackAntiIsoCut = -1
    thetool.TagCaloAntiIsoCut  = -1
    
    # Probe isolation: disabled by default - turned on below 
    thetool.ProbeTrackIsoCut     = -1
    thetool.ProbeTrackAntiIsoCut = -1
    thetool.ProbeCaloIsoCut      = -1
    thetool.ProbeCaloAntiIsoCut  = -1
    
    # Tag IP cuts
    thetool.TagD0Cut       = -1
    thetool.TagD0SignCut   = 3
    thetool.TagZ0Cut       = 10
    
    # Probe IP cuts
    thetool.ProbeD0Cut     = -1
    thetool.ProbeD0SignCut = 3
    thetool.ProbeZ0Cut     = 10
    
    thetool.EfficiencyFlag = EffiFlag
    # tools  
    thetool.SelectionTool = ToolSvc.TagProbeMuonSelectionTool
    thetool.TriggerMatchTool = ToolSvc.TrigMuonMatching
    thetool.TriggerDecisionTool = ToolSvc.TrigDecisionTool
    # trigger(s)
    thetool.TagTriggerList = [
        "HLT_mu26_imedium"
        ]

    thetool.UseCBProbes      = True
    thetool.ProbeIDHitCut    = True
    thetool.ProbeTrackIsoCut = 0.35
    thetool.ProbeCaloIsoCut  = -1
    thetool.DeltaPhiCut      = 2.0
    if AntiIso:
        thetool.ProbeTrackIsoCut = -1
        thetool.ProbeCaloIsoCut = -1
        thetool.ProbeTrackAntiIsoCut = 1.2
        thetool.ProbeCaloAntiIsoCut = 1.2
        thetool.HighMassWindow = 121000.
        thetool.LowMassWindow = 61000.
        thetool.ProbeZ0Cut = -1
        thetool.ProbeD0SignCut =-1
        thetool.ProbeD0Cut =-1
        
    if SameSign:
        thetool.AcceptOppCharge=False
        thetool.AcceptSameCharge=True

    ToolSvc += thetool
    return thetool

# ============================================================
# Add an efficiency tool
def AddTriggerTPEfficiencyTool (name, EffiFlag):
    from AthenaCommon.AppMgr import ToolSvc
    from MuonTPTools.MuonTPToolsConf import MuonTrigTPEfficiencyTool
    thetool = MuonTrigTPEfficiencyTool(name)
    thetool.MuonAuthor       = 21
    thetool.MatchPtCut       = 5000.0         # speed up the loop a bit
    thetool.EfficiencyFlag   = EffiFlag
    thetool.SelectionTool    = ToolSvc.TagProbeMuonSelectionTool
    thetool.TriggerMatchTool = ToolSvc.TrigMuonMatching
    ToolSvc += thetool
    return thetool


# ============================================================
################################################################
# Add a version of the Z TP algorithm and all associated tools
################################################################
def AddConfiguredTriggerTPAlg(name_suffix="Trigger",
                            MatchContainer="Muons",
                            TriggerList_L1 = [], TriggerList_L2 = [], TriggerList_EF = [], TriggerList_HLT=[],
                            doEtaSlices = True, 
                            writeNtuple = False,
                            doClosure   = False,
                            doDRSys     = False
			    ):
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = "Muons"
    if hasattr(job, "MuonQualityUpdater"):
        MuonContainerToUse = "UpdatedMuons"

    ProbeContainer = MuonContainerToUse
    theAlg = AddZmumuTPAlg(name="ZmumuTPAlg_%s"%name_suffix,
                           ProbeCont=ProbeContainer,
                           MatchCont=MatchContainer)

    AddMuonSelectionTool()

    # add two TP selection tools (OC and SC)
    SelectionTools = []
    SelectionTools += [AddTriggerTPSelectionTool(name="ZmumuTPSelectionTool_Trigger_OC_%s"%name_suffix,EffiFlag="%s_OC"%name_suffix)]
    SelectionTools += [AddTriggerTPSelectionTool(name="ZmumuTPSelectionTool_Trigger_SC_%s"%name_suffix,EffiFlag="%s_SC"%name_suffix, SameSign=True)]


    PlotTools = []
    # add plotting tools in slices
    if doEtaSlices:
        Slices = {
                   "All"        : (0.0,10.0),
                   "noCrack"    : (0.1,10.0),
                   "Crack"      : (0.0,0.1),
                   "Barrel"     : (0.1,1.1),
                   "Transition" : (1.1, 1.3),
                   "Endcap"     : (1.3,2.0),
                   "CSC"        : (2.0,2.7)
                   }

        for etaslice, interval in Slices.iteritems():
            PlotTool = AddTriggerTPPlotTool(name="TriggerTPPlottingTool_%s_%s"%(name_suffix,etaslice),EffiFlag=name_suffix+"_"+etaslice,applySF = False)
            PlotTool.ProbeAbsEtaMin = interval[0]
            PlotTool.ProbeAbsEtaMax = interval[1]
            PlotTools.append(PlotTool)
            if doClosure:
                PlotToolCL = AddTriggerTPPlotTool(name="TriggerTPPlottingTool_Closure_%s_%s"%(name_suffix,etaslice),EffiFlag=name_suffix+"_"+etaslice+"_Closure",applySF = True)
                PlotToolCL.ProbeAbsEtaMin = interval[0]
                PlotToolCL.ProbeAbsEtaMax = interval[1]
                PlotTools.append(PlotToolCL)
                
    else:
        PlotTools = [AddTriggerTPPlotTool(name="TriggerTPPlottingTool_%s_All"%name_suffix,EffiFlag=name_suffix+"_All",applySF = False)]
        if doClosure:
            PlotTools.append(AddTriggerTPPlotTool(name="TriggerTPPlottingTool_Closure_%s_All"%name_suffix,EffiFlag=name_suffix+"_All_Closure",applySF = True))
        

    # add any desired matching tools

    MATCH_DR = 0.05

    MatchingTools = []

    for Trigitem in TriggerList_L1 + TriggerList_HLT:
        thistool = AddTriggerTPEfficiencyTool(name="TriggerTPEfficiencyTool_%s_%s"%(name_suffix,Trigitem),EffiFlag=Trigitem)
        thistool.TrigItem = Trigitem
        thistool.MaximumDrCut = MATCH_DR
        MatchingTools.append(thistool)
        if doDRSys:
            thistool_dRup = AddTriggerTPEfficiencyTool(name="TriggerTPEfficiencyTool_DrUp_%s_%s"%(name_suffix,Trigitem),EffiFlag="%s_dRUp"%Trigitem)
            thistool_dRup.TrigItem = Trigitem
            thistool_dRup.MaximumDrCut = 2.0 * MATCH_DR
            MatchingTools.append(thistool_dRup)

            thistool_dRDown = AddTriggerTPEfficiencyTool(name="TriggerTPEfficiencyTool_DrDown_%s_%s"%(name_suffix,Trigitem),EffiFlag="%s_dRDown"%Trigitem)
            thistool_dRDown.TrigItem = Trigitem
            thistool_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools.append(thistool_dRDown)

           

    ntuples = []
    if writeNtuple:
        ntuples.append(AddTriggerTPTreeTool(name="TriggerTPTreeTool_%s"%name_suffix, EffiFlag="Trees"))

    # create the TP tool itself
    TheTPTool = AddZmumuTPTool(name = "TriggerTPTool_%s"%name_suffix,EffiFlag=name_suffix)
    TheTPTool.MuonTPSelectionTools  = SelectionTools
    TheTPTool.PlottingTools         = PlotTools
    TheTPTool.MuonTPEfficiencyTools = MatchingTools
    TheTPTool.TreeTools             = ntuples
    theAlg.MuonTPTools += [TheTPTool]


def AddTriggerTPAnalysis(doEtaSlices=True,writeNtuple=False,doClosure=False,doDRSys=False,doL1=False,doL2=False,doEF=False,doHLT=False):
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()


    MuonContainerToUse = "Muons"
    if hasattr(job, "MuonQualityUpdater"):
        MuonContainerToUse = "UpdatedMuons"
    TriggerList_L1  = []
    TriggerList_L2  = []
    TriggerList_EF  = []
    TriggerList_HLT = []
    
    if doHLT:
        TriggerList_HLT = [
                 #        Multi-Muon Triggers
#                "HLT_2mu4",
#                "HLT_2mu6",
#                "HLT_2mu10",
#                "HLT_2mu14",
#                "HLT_3mu6",
#                "HLT_3mu6_msonly",
#                "HLT_mu11_2mu4noL1_nscan03_L1MU10_2MU6",
#                "HLT_mu11_2mu4noL1_nscan03_L1MU11_2MU6",
#                "HLT_mu14_iloose_mu14",
#                "HLT_mu18_2mu0noL1_JpsimumuFS",
#                "HLT_mu18_2mu4_JpsimumuL2",
#                "HLT_mu18_2mu4noL1",
#                "HLT_mu18_mu8noL1",
#                "HLT_mu20_2mu0noL1_JpsimumuFS",
#                "HLT_mu20_2mu4_JpsimumuL2",
#                "HLT_mu20_2mu4noL1",
#                "HLT_mu20_iloose_2mu4noL1",
#                "HLT_mu20_iloose_mu8noL1",
#                "HLT_mu20_imedium_mu8noL1",
#                "HLT_mu20_msonly_mu6noL1_msonly_nscan05",
#                "HLT_mu20_mu8noL1",
#                "HLT_mu22_2mu4noL1",
#                "HLT_mu22_mu8noL1",
#                "HLT_mu24_2mu4noL1",
#                "HLT_mu24_mu8noL1",
    		 #        Single-Muon
                "HLT_mu0_perf",
                #"HLT_mu0noL1_fsperf",
                "HLT_mu4",
                "HLT_mu4_msonly",
                "HLT_mu4_r1extr",
                "HLT_mu6",
                "HLT_mu6_msonly",
                "HLT_mu10",
                "HLT_mu10_msonly",
                "HLT_mu10_r1extr",
                "HLT_mu14",
                "HLT_mu18",
                "HLT_mu20",
                "HLT_mu20_L1MU15",
                "HLT_mu20_iloose_L1MU15",
                "HLT_mu20_msonly",
                "HLT_mu20_r1extr",
                "HLT_mu22",
                "HLT_mu24",
                "HLT_mu24_L1MU15",
                "HLT_mu24_iloose_L1MU15",
                "HLT_mu24_imedium",
                "HLT_mu26",
                "HLT_mu26_imedium",
                "HLT_mu50",
                "HLT_mu60_0eta105_msonly",

                "HLT_AllTriggers", # this is needed, if you want to have combined (inclusive) efficiency of all HLT triggers
                    ]
    if doL1:
        TriggerList_L1 = [
                "L1_MU4",
                "L1_MU6",
                "L1_MU10",
                "L1_MU15",
                "L1_MU20",
#                "L1_2MU4",
#                "L1_2MU6",
#                "L1_2MU10",
#                "L1_3MU4",
#                "L1_3MU6",
#                "L1_MU6_2MU4",
#                "L1_MU10_2MU6",
#                "L1_MU11_2MU6",

                "L1_AllTriggers", # this is needed, if you want to have combined (inclusive) efficiency of all L1 triggers
                               ]

    # PLEASE NOTE !!! L2 & EF are so far only placeholders
    if doL2:
        TriggerList_L2 = [
                            "L2_MU20" # a dummy name
                                ]
    if doEF:
        TriggerList_EF = [
                            "EF_MU30" # a dummy name
                                ]


    ##########################################################################################
    # Add the Zmm TP algorithm using MS muons as probes and matching to CaloTag muons
    AddConfiguredTriggerTPAlg(
                            name_suffix     = "MuonTrigValid",
                            MatchContainer  = "Muons",
                            TriggerList_L1  = TriggerList_L1,
                            TriggerList_L2  = TriggerList_L2,
                            TriggerList_EF  = TriggerList_EF,
                            TriggerList_HLT = TriggerList_HLT,
                            doEtaSlices     = doEtaSlices,
                            writeNtuple     = writeNtuple,
                            doClosure       = doClosure,
                            doDRSys         = doDRSys 
			    )
