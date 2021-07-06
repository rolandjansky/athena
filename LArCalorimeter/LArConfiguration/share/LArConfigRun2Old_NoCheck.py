# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from LArROD.LArRODFlags import larRODFlags
from LArDigitization.LArDigitizationFlags import jobproperties

larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True) # default
larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True) # default
larRODFlags.firstSample.set_Value_and_Lock(0)  # default
larRODFlags.nSamples.set_Value_and_Lock(4)     # default
larRODFlags.NumberOfCollisions.set_Value_and_Lock(20) # Run-2 default

# Not available in 21.0-mc16a but keeping here for consistency
# jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False)  # default
jobproperties.LArDigitizationFlags.useFcalHighGain.set_Value_and_Lock(False)    # default

