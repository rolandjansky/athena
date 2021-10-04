# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Egamma slice specific flags  """

from AthenaCommon.Logging import logging
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

log = logging.getLogger( 'TriggerJobOpts.EgammaSliceFlags' )
_flags = []

class signatures(JobProperty):
    """ signatures in Egamma slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []

_flags.append(signatures)

# create container
class EgammaSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Egamma Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(EgammaSlice)


for flag in _flags:
    TriggerFlags.EgammaSlice.add_JobProperty(flag)
