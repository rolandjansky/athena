include.block("LArRecUtils/LArMCSymCond.py")

condSequence = AthSequencer("AthCondSeq")  
from LArRecUtils.LArRecUtilsConf import LArMCSymCondAlg

condSequence+=LArMCSymCondAlg(ReadKey="LArOnOffIdMap")


