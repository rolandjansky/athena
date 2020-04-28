# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
Lvl1 menu specific flags
"""

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.Logging import logging

from .Limits import Limits

__doc__="Level 1 specific flags for Run 3"

log = logging.getLogger('Menu.L1.Base.L1MenuFlags')

_flags = [] 


class CTPVersion(JobProperty):
    """
    Version of the CTP configuration
    Run 2: version 4 (and 5 for a short period in 2015)
    Run 3: version 4
    """
    statusOn = True
    allowedTypes = [int]
    allowedValues = [0,1,2,3,4,5,6]
    StoredValue = 4
    Limits.setLimits(4)

    def _do_action(self):
        """Load the limits"""
        Limits.setLimits(self.get_Value())

_flags.append(CTPVersion)


class items(JobProperty):
    """Names of items enabled in the selected L1 menu"""
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]

_flags.append(items)



class boards(JobProperty):
    from collections import OrderedDict as odict
    """Names of thresholds enabled in selected L1 menu """
    statusOn=True
    allowedTypes=['OrderDict']
    StoredValue   = odict()

_flags.append(boards)


class legacyBoards(JobProperty):
    from collections import OrderedDict as odict
    """Names of thresholds enabled in selected L1 menu """
    statusOn=True
    allowedTypes=['OrderDict']
    StoredValue   = odict()

_flags.append(legacyBoards)


class prescales(JobProperty):
    """Maps prescales to items that are different from 1"""
    statusOn=True
    allowedTypes=['dict']
    StoredValue   = {}

_flags.append(prescales)

class RemapThresholdsAsListed(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

_flags.append(RemapThresholdsAsListed)

class CtpIdMap(JobProperty):
    statusOn = True
    allowedTypes = ['dict']
    StoredValue = {}

_flags.append(CtpIdMap)

class BunchGroupPartitioning(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = []

_flags.append(BunchGroupPartitioning)

class BunchGroupNames(JobProperty):
    statusOn = False
    allowedTypes = ['list']
    StoredValue = []

_flags.append(BunchGroupNames)

class MenuPartitioning(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = []

_flags.append(MenuPartitioning)






class L1MenuFlags(JobPropertyContainer):
    """ L1 flags used in menu generation """


# execute

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(L1MenuFlags)

for flag in _flags:
    TriggerFlags.L1MenuFlags.add_JobProperty(flag)
del _flags


# make an alias
L1MenuFlags = TriggerFlags.L1MenuFlags



            
