# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Following method taken from TrigEgammaMonToolConfig found in TrigEgammaAnalysisTools/TrigEgammaMonToolConfig.py
# Configuration should be updated and kept in this package for monitoring
# Provide all triggers to be monitored
# Provide list of categories and shifter plots

def TrigEgammaMonitoringTool():
    from AthenaCommon.AppMgr import ToolSvc
    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavAnalysisTool,TrigEgammaNavTPAnalysisTool,TrigEgammaNavTPJpsieeAnalysisTool
    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaPlotTool

    # Move the ProbeList into MonitCategory to keep all configuration in TrigEgammaMonitoring 
    from TrigEgammaAnalysisTools.TrigEgammaProbelist import monitoring_mam, monitoring_electron, monitoring_photon 
    from TrigEgammaAnalysisTools.TrigEgammaProbelist import monitoringTP_electron, monitoringTP_electronZee, monitoringTP_electronJpsiee 
   
    #Define the base path for all histograms
    basePath = '/HLT/Egamma'
    
    #Configure the TrigEgammaPlotTool
    #MaM maps a category name (creates a directory) to a trigger (copies histos from trigger dir to category dir)
    #Efficiency,Distribution,Resolution -- list of plots to monitor for shifter MaM categories
    PlotTool = TrigEgammaPlotTool.copy(name="TrigEgammaPlotTool",
            DirectoryPath=basePath,
            MaM=monitoring_mam,
            Efficiency=["eff_et","eff_eta","eff_mu"],
            Distribution=["et","eta","d0","d0sig"],
            Resolution=["res_et","res_eta","res_Rhad","res_Rphi","res_Reta"])

    tagItems = ['HLT_e24_lhmedium_iloose_L1EM18VH',
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

    # For MaM, most important is the list of triggers to monitor
    # Currently these are found in TRigEgammProbelist
    # We should use the TrigEgammaMonitCategory to define lists 
    # Use the MaM categories from those lists
    ElectronAnalysis = TrigEgammaNavAnalysisTool(name='NavElectronAnalysis',
            Analysis='Electrons',
            PlotTool=PlotTool,
            TriggerList=monitoring_electron,
            File="",
            OutputLevel=0,DetailedHistograms=False)
    PhotonAnalysis = TrigEgammaNavAnalysisTool(name='NavPhotonAnalysis',
            Analysis='Photons',
            PlotTool=PlotTool,
            TriggerList=monitoring_photon,
            File="",
            OutputLevel=0,DetailedHistograms=False)
    TPAnalysis = TrigEgammaNavTPAnalysisTool(name='NavTPAnalysis',
            Analysis='Zee',
            PlotTool=PlotTool,
            TriggerList=monitoringTP_electron,
            File="",
            TagTriggerList=tagItems,
            RemoveCrack=False,
            OutputLevel=0,DetailedHistograms=False)

    JpsiTPAnalysis = TrigEgammaNavTPJpsieeAnalysisTool(name='NavTPJpsieeAnalysis',
                                                        Analysis='Jpsiee',
                                                        PlotTool=PlotTool,
                                                        TriggerList=monitoringTP_electronJpsiee,
                                                        File="",
                                                        TagTriggerList= JpsitagItems)

    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaMonTool
    TrigEgammaMonTool = TrigEgammaMonTool( name = "HLTEgammaMon", 
            histoPathBase=basePath,
            Tools=["TrigEgammaNavAnalysisTool/NavPhotonAnalysis",
                    "TrigEgammaNavAnalysisTool/NavElectronAnalysis",
                    "TrigEgammaNavTPAnalysisTool/NavTPAnalysis",
                    "TrigEgammaNavTPAnalysisTool/NavTPJpsieeAnalysis"])
    ToolSvc += TrigEgammaMonTool
    list = ['TrigEgammaMonTool/HLTEgammaMon'];
    return list
