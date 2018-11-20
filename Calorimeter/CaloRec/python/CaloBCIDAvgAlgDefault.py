


def CaloBCIDAvgAlgDefault():
    from CaloRec.CaloRecConf import CaloBCIDAvgAlg
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    topSequence = AlgSequence()
    #condSequence = AthSequencer("AthCondSeq")
    if not hasattr(topSequence,"CaloBCIDAvgAlg"):
        from IOVDbSvc.CondDB import conddb
        from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault
        LArMCSymCondAlgDefault()
        if globalflags.DataSource()=='data':
            from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
            theLumiTool = LuminosityToolDefault()
            from AthenaCommon.AppMgr import ToolSvc
            ToolSvc += theLumiTool
            if athenaCommonFlags.isOnline:
                conddb.addFolder("LAR_ONL","/LAR/LArPileup/LArPileupShape<key>LArShape32</key>",className="LArShape32MC")
                conddb.addFolder("LAR_ONL","/LAR/LArPileup/LArPileupAverage",className="LArMinBiasAverageMC")
            else:
                conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/LArPileupShape<key>LArShape32</key>",className="LArShape32MC")
                conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/LArPileupAverage",className="LArMinBiasAverageMC")

            topSequence+=CaloBCIDAvgAlg(isMC=False,LumiTool=theLumiTool,ShapeKey="LArShape32")
        else: #MC case
            from LArRecUtils.LArOFCCondAlgDefault import LArOFCCondAlgDefault
            from LArRecUtils.LArAutoCorrTotalCondAlgDefault import  LArAutoCorrTotalCondAlgDefault
            from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
            LArADC2MeVCondAlgDefault()
            LArAutoCorrTotalCondAlgDefault()
            LArOFCCondAlgDefault()
            from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
            theBunchCrossingTool = BunchCrossingTool()
            conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/Shape",className="LArShape32MC")
            conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/LArPileupAverage",className="LArMinBiasAverageMC")
            topSequence+=CaloBCIDAvgAlg(isMC=True,BunchCrossingTool = theBunchCrossingTool,ShapeKey="LArShape")
            
    return 
