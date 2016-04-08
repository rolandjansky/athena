class TauSeeding:
    
    def __init__(self, JetAlgName):
        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        from PanTauAnalysis.PanTauSeedBuilderAlg import createTauSeedBuilder
        panTauSeedBuilder = createTauSeedBuilder(JetAlgName)
        panTauSeedBuilder.OutputLevel=INFO
        
        for iTool in panTauSeedBuilder.Tools_TauFeatureExtractionTools:
            iTool.OutputLevel=INFO
        
        # add RunTimeHistos if required
        from PanTauAnalysis.PanTauSeedBuilderAlg_Flags import PanTauSeedBuilderAlgFlags as PSB_Flags
        if PSB_Flags.Config_DoRunTimeHists():
            from PanTauAnalysis.RunTimeHistConfig import configRunTimeHistos
            configRunTimeHistos(panTauSeedBuilder)
        
        pionEflowObjectMatchTool = None
        
        from RecExConfig.RecFlags import rec
        if (rec.doTruth() ) :
            # pion-efo match tool
            from PanTauAlgs.PanTauAlgsConf import PanTau__PionEflowObjectTruthMatchTool
            pionEflowObjectMatchTool = PanTau__PionEflowObjectTruthMatchTool( 'PionEflowObjectMatchTool',
                                                                            MinPt = 0.5*GeV,
                                                                            MatchMaxDR = 0.02,
                                                                            UseExtrapolatedPositions = False,
                                                                            ExtrapolateToCaloLayer = 1,
                                                                            ExcludeEMNeutrals = False)
            pionEflowObjectMatchTool.OutputLevel=VERBOSE
            ToolSvc += pionEflowObjectMatchTool
            print pionEflowObjectMatchTool
        #### end if(rec.doTruth() )
        
        topSequence+=panTauSeedBuilder
        print panTauSeedBuilder
        
        
    #end def __init__            
#end class
