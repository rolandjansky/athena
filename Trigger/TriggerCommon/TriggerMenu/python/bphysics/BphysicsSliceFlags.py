# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Bphysics slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer

__author__  = 'T. Bold'
__version__="$Revision: 1.30 $"
__doc__="Bphysics slice specific flags  "


_flags = []

class doSiTrack(JobProperty):
    """ do or not to do SiTrack algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

_flags.append(doSiTrack)

class doIDSCAN(JobProperty):
    """ do or not to do IDSCAN algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doIDSCAN)



class signatures(JobProperty):
    """ signatures in Bphysics slice """
    statusOn=True
    allowedTypes=['list']

    
    StoredValue   = [
        ]




_flags.append(signatures)



# create container
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

class BphysicsSlice(JobPropertyContainer, CommonSliceHelper):
    """ Bphysics Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(BphysicsSlice)

# add add common slice flags
TriggerFlags.BphysicsSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.BphysicsSlice.add_JobProperty(flag)
del _flags

# make an alias
BphysicsSliceFlags = TriggerFlags.BphysicsSlice

