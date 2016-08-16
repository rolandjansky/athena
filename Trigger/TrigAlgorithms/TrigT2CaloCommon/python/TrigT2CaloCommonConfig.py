# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigDataAccess as _TrigDataAccess

class TrigDataAccess(_TrigDataAccess):
    __slots__ = ()

    def __init__(self, name='TrigDataAccess'):
        super(TrigDataAccess, self).__init__(name)

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        from AthenaCommon.GlobalFlags import globalflags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        from AthenaCommon.Logging import logging
        log = logging.getLogger(name)
     
        self.loadFullCollections = ( TriggerFlags.doEF() or TriggerFlags.doHLT() )
        self.loadAllSamplings    = ( TriggerFlags.doLVL2() or TriggerFlags.doHLT() )

        if ( globalflags.DatabaseInstance == "COMP200" and TriggerFlags.doCaloOffsetCorrection() ) :
            self.ApplyOffsetCorrection = False
            log.warning("Not possible to run BCID offset correction with COMP200")
        else:
            if TriggerFlags.doCaloOffsetCorrection():
                log.info('Enable HLT calo offset correction')
                if globalflags.DataSource()=='data' and athenaCommonFlags.isOnline():
                    from IOVDbSvc.CondDB import conddb
                    conddb.addFolder("LAR_ONL","/LAR/ElecCalibFlat/OFC")
                    from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
                    svcMgr += LArFlatConditionSvc()
                    svcMgr.LArFlatConditionSvc.OFCInput="/LAR/ElecCalibFlat/OFC"
                    svcMgr.ProxyProviderSvc.ProviderNames += [ "LArFlatConditionSvc" ]
                    
                from CaloTools.CaloLumiBCIDToolDefault import CaloLumiBCIDToolDefault
                theCaloLumiBCIDTool = CaloLumiBCIDToolDefault()
                svcMgr.ToolSvc += theCaloLumiBCIDTool
                self.CaloLumiBCIDTool = theCaloLumiBCIDTool
                self.ApplyOffsetCorrection = True
            else:
                log.info('Disable HLT calo offset correction')
                self.ApplyOffsetCorrection = False
                
        from RecExConfig.RecFlags import rec
        transientBS = (rec.readRDO() and not globalflags.InputFormat()=='bytestream')
        if ( transientBS or TriggerFlags.writeBS() ):
            if ( not hasattr(svcMgr.ToolSvc,'LArRawDataContByteStreamTool') ):
                from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig
                svcMgr.ToolSvc += LArRawDataContByteStreamToolConfig()
            svcMgr.ToolSvc.LArRawDataContByteStreamTool.DSPRunMode=4
            svcMgr.ToolSvc.LArRawDataContByteStreamTool.RodBlockVersion=10

        return
    
