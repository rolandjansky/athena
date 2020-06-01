

def CaloBCIDAvgAlgDefault():
    from CaloRec.CaloRecConf import CaloBCIDAvgAlg
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    if not hasattr(topSequence,"CaloBCIDAvgAlg"):
        from IOVDbSvc.CondDB import conddb
        from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault
        LArMCSymCondAlgDefault()
        from CaloRec.CaloBCIDLumiCondAlgDefault import CaloBCIDLumiCondAlgDefault
        CaloBCIDLumiCondAlgDefault()
        topSequence+=CaloBCIDAvgAlg()
            
    return 
