# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from LArROD.LArRODFlags import larRODFlags
from LArDigitization.LArDigitizationFlags import jobproperties

larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(False) # no pileup optimisation for OFC
larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(False) 
larRODFlags.firstSample.set_Value_and_Lock(0)  # default
larRODFlags.nSamples.set_Value_and_Lock(4)     # default
larRODFlags.NumberOfCollisions.set_Value_and_Lock(0) #  no pileup
larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(False) #  no pileup

jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(True)  # use high gain in EMEC IW in nopileup case
jobproperties.LArDigitizationFlags.useFcalHighGain.set_Value_and_Lock(True)    # use high gain in Fcal in nopileup case

# check if Beam flag is set properly
if jobproperties.Beam.numberOfCollisions() != 0:
   from AthenaCommon.Logging import logging
   mlog = logging.getLogger ('SetLArConfigRun3')
   mlog.error("Beam.numberOfCollisions %f different from what is expected for Run3 no pileup digi/reco ", jobproperties.Beam.numberOfCollisions) 
