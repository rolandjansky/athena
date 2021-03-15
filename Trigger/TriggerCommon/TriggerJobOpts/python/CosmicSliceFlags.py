# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Cosmics slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper


__doc__="Cosmic slice specific flags  "


_flags = []

class SliceList(JobProperty):
    """ List of slices provided by the Cosmics slice """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[] 
    
_flags.append(SliceList)

class signatures(JobProperty):
    """ signatures in Cosmic slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue= SliceList.StoredValue
_flags.append(signatures)


# create container

class CosmicSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Cosmic Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(CosmicSlice)

for flag in _flags:
    TriggerFlags.CosmicSlice.add_JobProperty(flag)
