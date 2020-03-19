# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import include
from IOVDbSvc.CondDB import conddb

if conddb.isMC:
    include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")
else:
    include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

from LArRecUtils.LArRecUtilsConf import LArAutoCorrNoiseCondAlg 
from AthenaCommon.AlgSequence import AthSequencer
from LArCabling.LArCablingAccess import LArOnOffIdMapping
from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault
condSeq = AthSequencer("AthCondSeq")


def LArAutoCorrNoiseCondAlgDefault():

    LArOnOffIdMapping()
    LArMCSymCondAlgDefault()
    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq,"LArAutoCorrNoiseCondAlg"):
        return getattr(condSeq,"LArAutoCorrNoiseCondAlg")

    theAutoCorrNoiseCondAlg=LArAutoCorrNoiseCondAlg("LArAutoCorrNoiseCondAlg")

    from LArROD.LArRODFlags import larRODFlags
    theAutoCorrNoiseCondAlg.nSampl = larRODFlags.nSamples()
 
    condSeq+=theAutoCorrNoiseCondAlg
    return theAutoCorrNoiseCondAlg
