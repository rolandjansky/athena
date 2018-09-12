

def CaloBCIDAvgAlgDefault():
    from CaloRec.CaloRecConf import CaloBCIDAvgAlg
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    if not CaloBCIDAvgAlg in topSequence:
        if globalflags.DataSource()=='data':
            from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
            theLumiTool = LuminosityToolDefault()
            from AthenaCommon.AppMgr import ToolSvc
            ToolSvc += theLumiTool
            from IOVDbSvc.CondDB import conddb
            if athenaCommonFlags.isOnline:
                conddb.addFolder("LAR_ONL","/LAR/LArPileup/LArPileupShape<key>LArShape32</key>")
                conddb.addFolder("LAR_ONL","/LAR/LArPileup/LArPileupAverage")
            else:
                conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/LArPileupShape<key>LArShape32</key>")
                conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/LArPileupAverage")

            topSequence+=CaloBCIDAvgAlg(isMC=False,LumiTool=theLumiTool,ShapeKey="LArShape32")
        else: #MC case
            from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
            theBunchCrossingTool = BunchCrossingTool()
            from IOVDbSvc.CondDB import conddb
            conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/Shape")
            conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/LArPileupAverage")
            topSequence+=CaloBCIDAvgAlg(isMC=True,BunchCrossingTool = theBunchCrossingTool,ShapeKey="LArShape")
            
    return 
