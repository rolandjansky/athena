# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TrigEgammaMonTool():
    from AthenaCommon.AppMgr import ToolSvc
    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavAnalysisTool,TrigEgammaNavTPAnalysisTool,TrigEgammaNavTPJpsieeAnalysisTool
    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaPlotTool
    from TrigEgammaAnalysisTools.TrigEgammaProbelist import monitoring_mam, monitoring_electron, monitoring_photon 
    from TrigEgammaAnalysisTools.TrigEgammaProbelist import monitoringTP_electron, monitoringTP_electronZee, monitoringTP_electronJpsiee 


    #Define the base path for all histograms
    basePath = '/HLT/Egamma'
    
    tagItems     = ['HLT_e24_lhmedium_iloose_L1EM18VH',
                    'HLT_e24_lhmedium_iloose_L1EM20VH',
                    'HLT_e24_lhtight_iloose',
                    'HLT_e26_lhtight_iloose',
                    # Primary cut-based electron triggers
                    'HLT_e24_medium_iloose_L1EM18VH',
                    'HLT_e24_medium_iloose_L1EM20VH',
                    'HLT_e24_tight_iloose',
                    'HLT_e26_tight_iloose']

    JpsitagItems = ['HLT_e5_tight_e4_etcut',
                    'HLT_e9_tight_e4_etcut',
                    'HLT_e14_tight_e4_etcut',
                    'HLT_e9_etcut_e5_tight',
                    'HLT_e14_etcut_e5_tight',
                    # Primary cut-based electron triggers
                    'HLT_e5_tight_e4_etcut_Jpsiee',
                    'HLT_e9_tight_e4_etcut_Jpsiee',
                    'HLT_e14_tight_e4_etcut_Jpsiee',
                    'HLT_e9_etcut_e5_tight_Jpsiee',
                    'HLT_e14_etcut_e5_tight_Jpsiee']

    #Configure the TrigEgammaPlotTool
    PlotTool = TrigEgammaPlotTool.copy(name="TrigEgammaPlotTool",
            DirectoryPath=basePath,
            MaM=monitoring_mam,
            Efficiency=["eff_et","eff_eta","eff_mu"],
            Distribution=["et","eta","d0","d0sig"],
            Resolution=["res_et","res_eta","res_Rhad","res_Rphi","res_Reta"])

    #Configure emulation tools
    #from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig import TrigEgammaEmulationTool
    #EmulationPhotonTool     = TrigEgammaEmulationTool.copy( TriggerList = monitoring_photon          , name = "EmulationPhotonTool"     )
    #EmulationElectronTool   = TrigEgammaEmulationTool.copy( TriggerList = monitoring_electron        , name = "EmulationElectronTool"   )
    #EmulationTPElectronTool = TrigEgammaEmulationTool.copy( TriggerList = monitoringTP_electron      , name = "EmulationTPElectronTool" )
    #EmulationJpsieeTool     = TrigEgammaEmulationTool.copy( TriggerList = monitoringTP_electronJpsiee, name = "EmulationJpsieeTool"     )


    ElectronAnalysis = TrigEgammaNavAnalysisTool(name='NavElectronAnalysis',
            Analysis='Electrons',
            PlotTool=PlotTool,
            TriggerList=monitoring_electron,
            File="",
            #doEmulation=True,
            #EmulationTool=EmulationElectronTool,
            OutputLevel=0,DetailedHistograms=False)
    PhotonAnalysis = TrigEgammaNavAnalysisTool(name='NavPhotonAnalysis',
            Analysis='Photons',
            PlotTool=PlotTool,
            TriggerList=monitoring_photon,
            File="",
            #doEmulation=True,
            #EmulationTool=EmulationPhotonTool,
            OutputLevel=0,DetailedHistograms=False)
    TPAnalysis = TrigEgammaNavTPAnalysisTool(name='NavTPAnalysis',
            Analysis='Zee',
            PlotTool=PlotTool,
            TriggerList=monitoringTP_electron,
            File="",
            TagTriggerList=tagItems,
            #doEmulation=True,
            #EmulationTool=EmulationTPElectronTool,
            RemoveCrack=False,
            OutputLevel=0,DetailedHistograms=False)
    JpsiTPAnalysis = TrigEgammaNavTPJpsieeAnalysisTool(name='NavTPJpsieeAnalysis',
                                                        Analysis='Jpsiee',
                                                        PlotTool=PlotTool,
                                                        TriggerList=monitoringTP_electronJpsiee,
                                                        File="",
                                                        #doEmulation=True,
                                                        #EmulationTool=EmulationJpsieeTool,
                                                        TagTriggerList= JpsitagItems)

    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaMonTool
    TrigEgammaMonTool = TrigEgammaMonTool( name = "HLTEgammaMon", 
            histoPathBase=basePath,
            Tools=[
                "TrigEgammaNavAnalysisTool/NavPhotonAnalysis",
                "TrigEgammaNavAnalysisTool/NavElectronAnalysis",
                "TrigEgammaNavTPAnalysisTool/NavTPAnalysis",
                "TrigEgammaNavTPAnalysisTool/NavTPJpsieeAnalysis"])
    ToolSvc += TrigEgammaMonTool
