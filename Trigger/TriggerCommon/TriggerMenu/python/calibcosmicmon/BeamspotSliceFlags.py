# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Beamspot slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

__author__  = ''
__version__="$Revision: 1.4 $"
__doc__="Beamspot slice specific flags  "

_flags = [] 

class signatures(JobProperty):
    """ signatures in Beamspot slice """
    statusOn=True
    allowedTypes = ['list', 'str']
    StoredValue  = []
_flags.append(signatures)


# create container

class BeamspotSlice(JobPropertyContainer, CommonSliceHelper):
    """ Beamspot Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(BeamspotSlice)

# add add common slice flags
TriggerFlags.BeamspotSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.BeamspotSlice.add_JobProperty(flag)
del _flags

# make an alias
BeamspotSliceFlags = TriggerFlags.BeamspotSlice
