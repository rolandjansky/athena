from AthenaCommon.Include import Include, IncludeError, include
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

from LArRecUtils.LArRecUtilsConf import LArADC2MeVCondAlg 
from AthenaCommon.AlgSequence import AthSequencer
from LArCabling.LArCablingAccess import LArOnOffIdMapping
condSeq = AthSequencer("AthCondSeq")


def LArADC2MeVCondAlgDefault():

    LArOnOffIdMapping()
    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq,"LArADC2MeVCondAlg"):
        return getattr(condSeq,"LArADC2MeVCondAlg")

    theADC2MeVCondAlg=LArADC2MeVCondAlg()
    
    from LArRecUtils.LArFEBConfigReaderDefault import LArFEBConfigReaderDefault
    theADC2MeVCondAlg.FebConfigReader=LArFEBConfigReaderDefault()
    condSeq+=theADC2MeVCondAlg
    return theADC2MeVCondAlg
