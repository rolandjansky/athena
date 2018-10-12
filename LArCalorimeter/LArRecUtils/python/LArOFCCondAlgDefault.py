# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import Include, IncludeError, include

include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")
from AthenaCommon.Logging import logging
from LArRecUtils.LArRecUtilsConf import LArOFCCondAlg 
from AthenaCommon.AlgSequence import AthSequencer
from LArCabling.LArCablingAccess import LArOnOffIdMapping
condSeq = AthSequencer("AthCondSeq")


def LArOFCCondAlgDefault():

    mlog = logging.getLogger('LArOFCCondAlg')
    LArOnOffIdMapping()
    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq,"LArOFCCondAlg"):
        return getattr(condSeq,"LArOFCCondAlg")

    theOFCCondAlg=LArOFCCondAlg()
    #theOFCCondAlg.MCSym = True
    theOFCCondAlg.isMC  = True
    from LArROD.LArRODFlags import larRODFlags

    theOFCCondAlg.firstSample = larRODFlags.firstSample()
    theOFCCondAlg.useHighestGainAutoCorr = larRODFlags.useHighestGainAutoCorr()
      
    if larRODFlags.doOFCMixedOptimization(): # kept for backward compatibility
        theOFCCondAlg.UseDelta = 1 # only EMECIW/HEC/FCAL
        from AthenaCommon.BeamFlags import jobproperties
        theOFCCondAlg.DeltaBunch = int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)
        mlog.info("  OFC *MIXED* optimization")
    else:
        theOFCCondAlg.UseDelta = larRODFlags.UseDelta()
        
        if ( larRODFlags.UseDelta() == 0 ):
            mlog.info("  Standard OFC optimization computation")

        elif ( larRODFlags.UseDelta() == 1 ):
            mlog.info("  OFC optimization asking for no average shift as extra constraint only in EMECIW/HEC/FCAL")
            from AthenaCommon.BeamFlags import jobproperties
            theOFCCondAlg.DeltaBunch = int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)

        elif ( larRODFlags.UseDelta() == 2 ):
            mlog.info("  OFC optimization asking for no average shift as extra constraint everywhere")
            from AthenaCommon.BeamFlags import jobproperties
            theOFCCondAlg.DeltaBunch = int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)

        elif ( larRODFlags.UseDelta() == 3 ):
            mlog.info("  OFC optimization asking for no average shift as extra constraint only in EMECIW/HEC/FCAL1+high eta FCAL2-3")
            from AthenaCommon.BeamFlags import jobproperties
            theOFCCondAlg.DeltaBunch = int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)
            
        else:
            theOFCCondAlg.UseDelta = 0 ### avoid unforseed options

        pass
    
    condSeq+=theOFCCondAlg
    return theOFCCondAlg
