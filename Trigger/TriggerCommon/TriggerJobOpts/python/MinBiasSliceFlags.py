# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" MinBias slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper


#
# This file should not be modified without consulting 
# the Minimum Bias trigger slice coordinator.
#
# The Minimum Bias trigger slice is documented at:
# https://twiki.cern.ch/twiki/bin/view/Atlas/MinbiasTrigger
#

#===================================================================

_flags = [] 

class signatures(JobProperty):
    """ signatures in MinBias slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []

    
_flags.append(signatures)

# create container
class MinBiasSlice(JobPropertyContainer, CommonSignatureHelper):
    """ MinBias Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(MinBiasSlice)

for flag in _flags:
    TriggerFlags.MinBiasSlice.add_JobProperty(flag)
