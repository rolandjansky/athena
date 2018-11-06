def LArMCSymCondAlgDefault():
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if not hasattr(condSeq, "LArMCSymCondAlg"):
        from LArRecUtils.LArRecUtilsConf import LArMCSymCondAlg
        condSeq+=LArMCSymCondAlg("LArMCSymCondAlg",ReadKey="LArOnOffIdMap")
    return 

