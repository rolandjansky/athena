# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import include

include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")
from AthenaCommon.Logging import logging
from LArRecUtils.LArRecUtilsConf import LArOFCCondAlg 
from AthenaCommon.AlgSequence import AthSequencer
from LArCabling.LArCablingAccess import LArOnOffIdMapping
condSeq = AthSequencer("AthCondSeq")


def LArOFCCondAlgDefault():

    mlog = logging.getLogger('LArOFCCondAlg')
    mlog.info("entering LArOFCCondAlgDefault")

    LArOnOffIdMapping()
    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq,"LArOFCCondAlg"):
        return getattr(condSeq,"LArOFCCondAlg")

    theOFCCondAlg=LArOFCCondAlg()
    #theOFCCondAlg.MCSym = True
    theOFCCondAlg.isMC  = True
    from LArROD.LArRODFlags import larRODFlags
    from AthenaCommon.BeamFlags import jobproperties

    theOFCCondAlg.firstSample = larRODFlags.firstSample()
    theOFCCondAlg.useHighestGainAutoCorr = larRODFlags.useHighestGainAutoCorr()

    if larRODFlags.doOFCPileupOptimization():
        if larRODFlags.NumberOfCollisions():
            theOFCCondAlg.Nminbias=larRODFlags.NumberOfCollisions()
            mlog.info("  setup for  Ncollisions %f   ", larRODFlags.NumberOfCollisions())
        else:
            theOFCCondAlg.Nminbias=jobproperties.Beam.numberOfCollisions()
            mlog.info("  setup for  Ncollisions %f   ", jobproperties.Beam.numberOfCollisions())
    else:
        theOFCCondAlg.Nminbias=0
        mlog.info("  setup for no pileup optimization")
      
    pass
    
    condSeq+=theOFCCondAlg
    return theOFCCondAlg
