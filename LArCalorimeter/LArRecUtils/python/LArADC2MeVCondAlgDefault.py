# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import Include, IncludeError, include
from IOVDbSvc.CondDB import conddb

if conddb.isMC:
    include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")
else:
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
 
    if conddb.isMC:
        from LArConditionsCommon.LArCondFlags import larCondFlags 
        if not larCondFlags.hasMphys():
            theADC2MeVCondAlg.LArMphysOverMcalKey="" #No MphysOVerMcal
        else:
            theADC2MeVCondAlg.LArMphysOverMcalKey="LArMphysOverMcalSym"

        if not larCondFlags.hasHVCorr():
            theADC2MeVCondAlg.LArHVScaleCorrKey=""
      
        theADC2MeVCondAlg.LAruA2MeVKey="LAruA2MeVSym"
        theADC2MeVCondAlg.LArDAC2uAKey="LArDAC2uASym"
        theADC2MeVCondAlg.LArRampKey="LArRampSym"


        theADC2MeVCondAlg.UseFEBGainTresholds=False
    else: # not MC:
        from LArRecUtils.LArFebConfigCondAlgDefault import LArFebConfigCondAlgDefault
        LArFebConfigCondAlgDefault()


    condSeq+=theADC2MeVCondAlg
    return theADC2MeVCondAlg
