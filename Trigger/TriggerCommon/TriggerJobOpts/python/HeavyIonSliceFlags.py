# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" HeavyIon slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

__doc__="Minimum Bias slice specific flags  "

#
# This file should not be modified without consulting 
# the Heavy Ion Trigger Menu Forum coordinator.
#
# The HITMF trigger slice is documented at:
# https://twiki.cern.ch/twiki/bin/view/Atlas/HeavyIonTriggerMenuForum
#

#===================================================================

_flags = [] 

class signatures(JobProperty):
    """ signatures in HeavyIon slice """
    statusOn=True
    allowedTypes=['list']

    StoredValue   = [
        ]


_flags.append(signatures)

# create container
class HeavyIonSlice(JobPropertyContainer, CommonSignatureHelper):
    """ HeavyIon Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(HeavyIonSlice)

# add add common slice flags
#TriggerFlags.HeavyIonSlice.import_JobProperties('TriggerJobOpts.CommonSignatureFlags')

for flag in _flags:
    TriggerFlags.HeavyIonSlice.add_JobProperty(flag)
del _flags

# make an alias
HeavyIonSliceFlags = TriggerFlags.HeavyIonSlice
