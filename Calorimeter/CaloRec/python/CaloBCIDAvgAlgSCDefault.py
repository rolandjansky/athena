# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def CaloBCIDAvgAlgSCDefault(sequence=None):

    if not sequence:
        from AthenaCommon.AlgSequence import AlgSequence
        sequence = AlgSequence()

    if not hasattr(sequence,"CaloBCIDAvgAlgSC"):
        from CaloRec.CaloBCIDLumiCondAlgSCDefault import CaloBCIDLumiCondAlgSCDefault
        CaloBCIDLumiCondAlgSCDefault()
        from CaloRec.CaloRecConf import CaloBCIDAvgAlg
        sequence+=CaloBCIDAvgAlg(name="CaloBCIDAvgAlgSC",IsSuperCell=True,BCIDLumiKey="CaloBCIDLumiSC",WriteKey="CaloBCIDAverageSC")
            
    return sequence.CaloBCIDAvgAlgSC

