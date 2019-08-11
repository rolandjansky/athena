

def CaloBCIDAvgAlgDefault():
    from CaloRec.CaloRecConf import CaloBCIDAvgAlg
    from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    topSequence = AlgSequence()
    condSeq = AthSequencer("AthCondSeq")
    if not hasattr(topSequence,"CaloBCIDAvgAlg"):
        from IOVDbSvc.CondDB import conddb
        from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault
        LArMCSymCondAlgDefault()
        if globalflags.DataSource()=='data':
            from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgDefault
            lumiAlg = LuminosityCondAlgDefault()
            if athenaCommonFlags.isOnline:
                conddb.addFolder("LAR_ONL","/LAR/LArPileup/LArPileupShape<key>LArShape32</key>",className="LArShape32MC")
                conddb.addFolder("LAR_ONL","/LAR/LArPileup/LArPileupAverage",className="LArMinBiasAverageMC")
            else:
                conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/LArPileupShape<key>LArShape32</key>",className="LArShape32MC")
                conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/LArPileupAverage",className="LArMinBiasAverageMC")

            #For data, the regular shape is the 4-sample one used to Q-factor computation by LArRawChannelBuilder
            #Here we need a 32-sample, symmetrized shape. Therfore the re-key'ing and the dedicated LArPileUpShapeSymCondAlg

            from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMinBiasAverageMC_LArMinBiasAverageSym_ as LArMinBiasAverageSymAlg
            condSeq+=LArMinBiasAverageSymAlg("LArPileUpAvgSymCondAlg",ReadKey="LArPileupAverage",WriteKey="LArPileupAverageSym")

            from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArShape32MC_LArShape32Sym_ as LArShapeSymAlg
            condSeq+=LArShapeSymAlg("LArPileUpShapeSymCondAlg",ReadKey="LArShape32",WriteKey="LArShape32Sym")

            topSequence+=CaloBCIDAvgAlg(isMC=False,
                                        LuminosityCondDataKey = lumiAlg.LuminosityOutputKey,
                                        ShapeKey="LArShape32Sym")
        else: #MC case
            from LArRecUtils.LArOFCCondAlgDefault import LArOFCCondAlgDefault
            from LArRecUtils.LArAutoCorrTotalCondAlgDefault import  LArAutoCorrTotalCondAlgDefault
            from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
            LArADC2MeVCondAlgDefault()
            LArAutoCorrTotalCondAlgDefault()
            LArOFCCondAlgDefault()
            from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
            theBunchCrossingTool = BunchCrossingTool()
            conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/LArPileupAverage",className="LArMinBiasAverageMC")
            
            from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMinBiasAverageMC_LArMinBiasAverageSym_ as LArMinBiasAverageSymAlg
            condSeq+=LArMinBiasAverageSymAlg("LArPileUpAvgSymCondAlg",ReadKey="LArPileupAverage",WriteKey="LArPileupAverageSym")

            topSequence+=CaloBCIDAvgAlg(isMC=True,
                                        LuminosityCondDataKey = '',
                                        BunchCrossingTool = theBunchCrossingTool,ShapeKey="LArShapeSym")
            
    return 
