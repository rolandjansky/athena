# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TrigEgammaPhysValMonTool():
    from AthenaCommon.AppMgr import ToolSvc
    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaEmulationTool
    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavAnalysisTool,TrigEgammaNavTPAnalysisTool, TrigEgammaNavNtuple, TrigEgammaNavTPNtuple
    from TrigEgammaAnalysisTools.TrigEgammaProbelist import default, probeListLowMidPtPhysicsTriggers # to import probelist
    
    #from TrigHLTMonitoring.HLTMonTriggerList import hltmonList # import MaM trigger list not available!!!!
    import TriggerMenu.menu.Physics_pp_v5 as physics_menu
    import TriggerMenu.menu.MC_pp_v5 as mc_menu
    from TriggerJobOpts.TriggerFlags import TriggerFlags

   
    physics_menu.setupMenu()
    mc_menu.setupMenu()
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
    #probelist = list(set(default+probeListLowMidPtPhysicsTriggers))
    

    basePath = 'Trigger/HLT/Egamma/'
    tagItems = [
        'e24_lhmedium_iloose_L1EM18VH',
        'e24_lhmedium_iloose_L1EM20VH',
        'e24_lhtight_iloose'
        'e26_lhtight_iloose',
        #Primary cut-based electron triggers
        'e24_medium_iloose_L1EM18VH',
        'e24_medium_iloose_L1EM20VH',
        'e24_tight_iloose',
        'e26_tight_iloose'
        ]
    Analysis = TrigEgammaNavAnalysisTool(name='NavAnalysis',
            DirectoryPath=basePath+'Analysis',
            TriggerList=probelist, 
            File="PhysVal")
    
    TPAnalysis = TrigEgammaNavTPAnalysisTool(name='NavTPAnalysis',
            DirectoryPath=basePath+'TPAnalysis',
            TriggerList=probelist, 
            File="PhysVal",
            TagTriggerList=tagItems,
            OutputLevel=0)
 
    Emulation = TrigEgammaEmulationTool("Emulation",TriggerList=probelist)
 

    Ntuple    = TrigEgammaNavNtuple(name="NavNtuple",
            DirectoryPath=basePath+'Ntuple',
            TriggerList=probelist,
            DoOfflineDump=False,
            ForcePidSelection=True,
            ForceProbeIsolation=False,
            ForceEtThreshold=True,
            RemoveCrack=True,
            #ForceFilterSelection=False,
            #ElectronFilterType="Tight",
            File="PhysVal",
            OutputLevel=0)

   
    TPNtuple  = TrigEgammaNavTPNtuple(name="NavTPNtuple",
            DirectoryPath=basePath+'TPNtuple',
            TriggerList=probelist,
            File="PhysVal",
            TagTriggerList=tagItems,
            OutputLevel=0)


    from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaPhysValMonTool
    TrigEgammaPhysValMonTool = TrigEgammaPhysValMonTool( name = "TrigEgammaPhysValMonTool", 
            histoPathBase=basePath,
            Tools=[
                "TrigEgammaNavAnalysisTool/NavAnalysis",
                "TrigEgammaNavTPAnalysisTool/NavTPAnalysis",
                "TrigEgammaEmulationTool/Emulation",
                "TrigEgammaNavNtuple/NavNtuple",
                "TrigEgammaNavTPNtuple/NavTPNtuple"
                ])

    ToolSvc += TrigEgammaPhysValMonTool



