# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def CaloBCIDAvgAlgDefault(sequence=None):

    if not sequence:
        from AthenaCommon.AlgSequence import AlgSequence
        sequence = AlgSequence()

    if not hasattr(sequence,"CaloBCIDAvgAlg"):
        from IOVDbSvc.CondDB import conddb  # noqa: F401
        from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault
        LArMCSymCondAlgDefault()
        from CaloRec.CaloBCIDLumiCondAlgDefault import CaloBCIDLumiCondAlgDefault
        CaloBCIDLumiCondAlgDefault()
        from CaloRec.CaloRecConf import CaloBCIDAvgAlg
        sequence+=CaloBCIDAvgAlg()
            
    return sequence.CaloBCIDAvgAlg

