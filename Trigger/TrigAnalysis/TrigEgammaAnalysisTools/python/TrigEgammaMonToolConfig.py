# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def TrigEgammaMonTool():
    from AthenaCommon.AppMgr import ToolSvc
    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavAnalysisTool,TrigEgammaNavTPAnalysisTool,TrigEgammaNavTPJpsieeAnalysisTool, PlotTool
    from TrigEgammaAnalysisTools.TrigEgammaProbelist import default,defaultJpsi # to import probelist
    #from TrigHLTMonitoring.HLTMonTriggerList import hltmonList # import MaM trigger list, not available!!!!
   
    # Import full trigger menu, requires setup of both MC and Physics
    # Makes available all possible triggers which can be in any dataset
    # Set final list from triggers available in data
    import TriggerMenu.menu.Physics_pp_v5 as physics_menu
    from TriggerJobOpts.TriggerFlags import TriggerFlags

    physics_menu.setupMenu()
    egammaMenu = TriggerFlags.EgammaSlice.signatures()
    egammaChains = []
    l1Items = []
    addItems = []
    for egchain in egammaMenu:
        egammaChains.append(egchain[0])
        l1Items.append(egchain[1])
        addItems.append(egchain[3])
    # Set list to full menu
    #probelist = egammaChains
    probelist = default
    Jpsiprobelist = defaultJpsi
    #probelist=['e5_loose_idperf','e5_lhloose_idperf','e0_perf_L1EM15','g0_perf_L1EM15']

    basePath = '/HLT/Egamma/'
    tagItems = ['e24_lhmedium_iloose_L1EM18VH',
        'e24_lhmedium_iloose_L1EM20VH',
        'e24_lhtight_iloose',
        'e26_lhtight_iloose',
        # Primary cut-based electron triggers
        'e24_medium_iloose_L1EM18VH',
        'e24_medium_iloose_L1EM20VH',
        'e24_tight_iloose',
        'e26_tight_iloose']

    JpsitagItems = ['e5_tight_e4_etcut',
                    'e9_tight_e4_etcut',
                    'e14_tight_e4_etcut',
                    'e9_etcut_e5_tight',
                    'e14_etcut_e5_tight',
                    # Primary cut-based electron triggers
                    'e5_tight_e4_etcut_Jpsiee',
                    'e9_tight_e4_etcut_Jpsiee',
                    'e14_tight_e4_etcut_Jpsiee',
                    'e9_etcut_e5_tight_Jpsiee',
                    'e14_etcut_e5_tight_Jpsiee']



    Analysis = TrigEgammaNavAnalysisTool(name='NavAnalysis',
            DirectoryPath=basePath+'Analysis',
            TriggerList=probelist, #["e26_lhtight_iloose","g20_tight"],
            File="",
            OutputLevel=2,DetailedHistograms=False)
    TPAnalysis = TrigEgammaNavTPAnalysisTool(name='NavTPAnalysis',
            DirectoryPath=basePath+'TPAnalysis',
            TriggerList=probelist,#["e26_lhtight_iloose"],
            File="",
            TagTriggerList=tagItems,
            RemoveCrack=False,
            OutputLevel=2,DetailedHistograms=False)

    JpsiTPAnalysis = TrigEgammaNavTPJpsieeAnalysisTool(name='NavTPJpsieeAnalysis',
                                                        DirectoryPath=basePath+'TPJpsieeAnalysis',
                                                        TriggerList=Jpsiprobelist,#["e5_lhloose"],
                                                        File="",
                                                        TagTriggerList= JpsitagItems,
                                                        RemoveCrack=True,
                                                        ZeeLowerMass=2,
                                                        ZeeUpperMass=5,
                                                        OfflineTagMinEt=5,
                                                        OfflineProbeMinEt=5,
                                                        OutputLevel=0,
                                                        DetailedHistograms=False,
                                                        doJpsiee=True)

    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaMonTool
    PlotTool = PlotTool()
    ToolSvc += PlotTool
    TrigEgammaMonTool = TrigEgammaMonTool( name = "TrigEgammaMonTool", 
            histoPathBase=basePath,
            Tools=["TrigEgammaNavAnalysisTool/NavAnalysis",
                "TrigEgammaNavTPAnalysisTool/NavTPAnalysis",
                   "TrigEgammaNavTPAnalysisTool/NavTPJpsieeAnalysis"])
    ToolSvc += TrigEgammaMonTool
