# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import include
from AthenaCommon.Logging import logging

include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")



from LArRecUtils.LArRecUtilsConf import LArAutoCorrTotalCondAlg 
from AthenaCommon.AlgSequence import AthSequencer
from LArCabling.LArCablingAccess import LArOnOffIdMapping
condSeq = AthSequencer("AthCondSeq")


def LArAutoCorrTotalCondAlgDefault():

    mlog = logging.getLogger( 'LArAutoCorrTotalCondAlg::__init__ ' )
    mlog.info("entering LArAutoCorrTotalCondAlgDefault")
    LArOnOffIdMapping()
    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq,"LArAutoCorrTotalCondAlg"):
        return getattr(condSeq,"LArAutoCorrTotalCondAlg")

    theAutoCorrTotalCondAlg=LArAutoCorrTotalCondAlg()


    from AthenaCommon.BeamFlags import jobproperties
    from LArROD.LArRODFlags import larRODFlags

    from AthenaCommon.SystemOfUnits import ns
    theAutoCorrTotalCondAlg.Nsamples = larRODFlags.nSamples()
    theAutoCorrTotalCondAlg.firstSample = larRODFlags.firstSample()
    theAutoCorrTotalCondAlg.deltaBunch = int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)   

    if larRODFlags.doOFCPileupOptimization():
        if larRODFlags.NumberOfCollisions():
            theAutoCorrTotalCondAlg.Nminbias=larRODFlags.NumberOfCollisions()
            mlog.info("  setup for  Ncollisions %f   deltaBunch %f", larRODFlags.NumberOfCollisions(), jobproperties.Beam.bunchSpacing())
        else:
            theAutoCorrTotalCondAlg.Nminbias=jobproperties.Beam.numberOfCollisions()
            mlog.info("  setup for  Ncollisions %f   deltaBunch %f", jobproperties.Beam.numberOfCollisions(), jobproperties.Beam.bunchSpacing())
    else:
        theAutoCorrTotalCondAlg.Nminbias=0.
        mlog.info("  setup for computing total noise autocorrelation without pileup")
    
 
    condSeq+=theAutoCorrTotalCondAlg
    return theAutoCorrTotalCondAlg
