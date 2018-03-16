from AthenaCommon.Include import Include, IncludeError, include
include("LArConditionsCommon/LArConditionsCommon_MT_jobOptions.py")
include("LArRecUtils/LArOnOffMappingAlg.py")
from LArRecUtils.LArRecUtilsConf import LArADC2MeVCondAlg 
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

def LArADC2MeVCondAlgDefault():
    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq,"LArADC2MeVCondAlg"):
        return getattr(condSeq,"LArADC2MeVCondAlg")

    theADC2MeVCondAlg=LArADC2MeVCondAlg()
    
    from LArRecUtils.LArFEBConfigReaderDefault import LArFEBConfigReaderDefault
    theADC2MeVCondAlg.FebConfigReader=LArFEBConfigReaderDefault()
    condSeq+=theADC2MeVCondAlg
    return theADC2MeVCondAlg
