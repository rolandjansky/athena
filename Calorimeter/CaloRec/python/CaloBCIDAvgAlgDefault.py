# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def CaloBCIDAvgAlgDefault():
    from CaloRec.CaloRecConf import CaloBCIDAvgAlg
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    if not hasattr(topSequence,"CaloBCIDAvgAlg"):
        from IOVDbSvc.CondDB import conddb  # noqa: F401
        from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault
        LArMCSymCondAlgDefault()
        from CaloRec.CaloBCIDLumiCondAlgDefault import CaloBCIDLumiCondAlgDefault
        CaloBCIDLumiCondAlgDefault()
        topSequence+=CaloBCIDAvgAlg()
            
    return 
