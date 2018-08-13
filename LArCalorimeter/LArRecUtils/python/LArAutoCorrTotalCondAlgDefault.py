# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import Include, IncludeError, include

include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")



from LArRecUtils.LArRecUtilsConf import LArAutoCorrTotalCondAlg 
from AthenaCommon.AlgSequence import AthSequencer
from LArCabling.LArCablingAccess import LArOnOffIdMapping
condSeq = AthSequencer("AthCondSeq")


def LArAutoCorrTotalCondAlgDefault():

    LArOnOffIdMapping()
    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq,"LArAutoCorrTotalCondAlg"):
        return getattr(condSeq,"LArAutoCorrTotalCondAlg")

    theAutoCorrTotalCondAlg=LArAutoCorrTotalCondAlg()

    from LArROD.LArRODFlags import larRODFlags
    theAutoCorrTotalCondAlg.Nsamples = larRODFlags.nSamples()
 
    condSeq+=theAutoCorrTotalCondAlg
    return theAutoCorrTotalCondAlg
