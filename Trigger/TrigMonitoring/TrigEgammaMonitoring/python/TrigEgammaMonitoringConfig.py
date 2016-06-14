# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Following method taken from TrigEgammaMonToolConfig found in TrigEgammaAnalysisTools/TrigEgammaMonToolConfig.py
# Configuration should be updated and kept in this package for monitoring
# Provide all triggers to be monitored
# Provide list of categories and shifter plots

def TrigEgammaMonitoringTool():
    from AthenaCommon.AppMgr import ToolSvc
    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavAnalysisTool,TrigEgammaNavTPAnalysisTool,TrigEgammaNavTPJpsieeAnalysisTool
    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import EfficiencyTool, ResolutionTool, DistTool, TrigEgammaPlotTool

    # Move the ProbeList into MonitCategory to keep all configuration in TrigEgammaMonitoring 
    from TrigEgammaMonitoring.TrigEgammaMonitCategory import monitoring_L1Calo, monitoring_electron, monitoringTP_electron, monitoring_ele_idperf 
    from TrigEgammaMonitoring.TrigEgammaMonitCategory import monitoring_photon, monitoring_mam, monitoringTP_Jpsiee 
   
    #Define the base path for all histograms
    basePath = '/HLT/Egamma'
    # Define the outputLevel 
    # WARNING only change for debugging
    debugLevel=0
    #Configure the TrigEgammaPlotTool
    #MaM maps a category name (creates a directory) to a trigger (copies histos from trigger dir to category dir)
    #Efficiency,Distribution,Resolution -- list of plots to monitor for shifter MaM categories
    HLTEgammaPlotTool = TrigEgammaPlotTool(name="HLTEgammaPlotTool",
            DirectoryPath=basePath,
            MaM=monitoring_mam,
            Efficiency=["eff_et","eff_eta","eff_mu"],
            Distribution=["et","eta","Reta","Rphi","Rhad","f1","f3","eratio","deta2","eprobHT","npixhits","nscthits","ptvarcone20"],
            Resolution=["res_et","res_Rphi","res_Reta","res_Rhad","res_ptvarcone20","res_deta2"],
            OutputLevel=debugLevel)

    HLTEgammaEffTool = EfficiencyTool(name="HLTEgammaEffTool",PlotTool=HLTEgammaPlotTool,OutputLevel=debugLevel)
    HLTEgammaResTool = ResolutionTool(name="HLTEgammaResTool",PlotTool=HLTEgammaPlotTool,OutputLevel=debugLevel)
    HLTEgammaDistTool = DistTool(name="HLTEgammaDistTool",PlotTool=HLTEgammaPlotTool,OutputLevel=debugLevel)

    tagItems = ['HLT_e24_lhtight_nod0_ivarloose',
        'HLT_e26_lhtight_nod0_ivarloose']

    JpsitagItems = ['HLT_e5_lhtight_nod0_e4_etcut',
                    'HLT_e9_lhtight_nod0_e4_etcut',
                    'HLT_e14_lhtight_nod0_e4_etcut',
                    'HLT_e5_lhtight_nod0_e4_etcut_Jpsiee',
                    'HLT_e9_lhtight_nod0_e4_etcut_Jpsiee']

    # For MaM, most important is the list of triggers to monitor
    # Currently these are found in TRigEgammProbelist
    # We should use the TrigEgammaMonitCategory to define lists 
    # Use the MaM categories from those lists
    ElectronAnalysis = TrigEgammaNavAnalysisTool(name='HLTEgammaElectronAnalysis',
            Analysis='Electrons',
            PlotTool=HLTEgammaPlotTool,
            Tools=[HLTEgammaEffTool,HLTEgammaResTool,HLTEgammaDistTool],
            TriggerList=monitoring_electron,
            ForceProbeIsolation=True,
            DefaultProbePid="LHMedium",
            File="",
            OutputLevel=debugLevel,DetailedHistograms=False)
    PhotonAnalysis = TrigEgammaNavAnalysisTool(name='HLTEgammaPhotonAnalysis',
            Analysis='Photons',
            PlotTool=HLTEgammaPlotTool,
            Tools=[HLTEgammaEffTool,HLTEgammaResTool,HLTEgammaDistTool],
            TriggerList=monitoring_photon,
            File="",
            OutputLevel=debugLevel,DetailedHistograms=False)
    TPAnalysis = TrigEgammaNavTPAnalysisTool(name='HLTEgammaTPAnalysis',
            Analysis='Zee',
            PlotTool=HLTEgammaPlotTool,
            Tools=[HLTEgammaEffTool,HLTEgammaResTool,HLTEgammaDistTool],
            TriggerList=monitoringTP_electron+monitoring_ele_idperf+monitoring_L1Calo,
            DefaultProbePid="LHMedium",
            File="",
            TagTriggerList=tagItems,
            RemoveCrack=False,
            OutputLevel=debugLevel,DetailedHistograms=False)

    JpsiTPAnalysis = TrigEgammaNavTPJpsieeAnalysisTool(name='HLTEgammaTPJpsieeAnalysis',
                                                        Analysis='Jpsiee',
                                                        PlotTool=HLTEgammaPlotTool,
                                                        Tools=[HLTEgammaEffTool,HLTEgammaResTool,HLTEgammaDistTool],
                                                        TriggerList=monitoringTP_Jpsiee,
                                                        File="",
                                                        TagTriggerList= JpsitagItems)
    
    #ZeeTPAnalysis = TrigEgammaNavTPJpsieeAnalysisTool(name='HLTEgammaTPZeeAnalysis',
    #                                                    Analysis='ZeeTP',
    #                                                    PlotTool=HLTEgammaPlotTool,
    #                                                    Tools=[HLTEgammaEffTool,HLTEgammaResTool,HLTEgammaDistTool],
    #                                                    TriggerList=monitoring_Zee,
    #                                                    File="",
    #                                                    TPTrigger=True,
    #                                                    TagTriggerList= tagItems)

    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaMonTool
    TrigEgammaMonTool = TrigEgammaMonTool( name = "HLTEgammaMon", 
            histoPathBase=basePath,
            IgnoreTruncationCheck=True,
            Tools=["TrigEgammaNavAnalysisTool/HLTEgammaPhotonAnalysis",
                    "TrigEgammaNavAnalysisTool/HLTEgammaElectronAnalysis",
                    "TrigEgammaNavTPAnalysisTool/HLTEgammaTPAnalysis",
                    "TrigEgammaNavTPAnalysisTool/HLTEgammaTPJpsieeAnalysis"])
    ToolSvc += TrigEgammaMonTool
    list = ['TrigEgammaMonTool/HLTEgammaMon'];
    return list
