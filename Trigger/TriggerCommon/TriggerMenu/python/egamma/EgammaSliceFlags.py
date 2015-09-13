# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Egamma slice specific flags  """

from AthenaCommon.Logging import logging
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'T. Bold, P.Urquijo'
__version__="$Revision: 1.42 $"
__doc__="Egamma slice specific flags  "

_flags = [] 
class doSiTrack(JobProperty):
    """ do or not to do SiTrack algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doSiTrack)

class doIDSCAN(JobProperty):
    """ do or not to do IDSCAN algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doIDSCAN)


class doTrigEgammaRec(JobProperty):
    """ do or not to do TrigEgammaRec (EF algorithm) algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doTrigEgammaRec)

class signatures(JobProperty):
    """ signatures in Egamma slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []

_flags.append(signatures)

# create container
class EgammaSlice(JobPropertyContainer, CommonSliceHelper):
    """ Egamma Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(EgammaSlice)

# add common slice flags
TriggerFlags.EgammaSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.EgammaSlice.add_JobProperty(flag)
del _flags

# make an alias
EgammaSliceFlags = TriggerFlags.EgammaSlice

