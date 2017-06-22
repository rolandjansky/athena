# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Lvl1 specific flags
"""

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.Logging import logging
from Limits import Limits

__author__  = 'J.Stelzer'
__version__="$Revision: 1.78 $"
__doc__="Level 1 specific flags for Run 2"

log = logging.getLogger('TriggerMenu.L1Flags.py')

_flags = [] 
class CTPVersion(JobProperty):
    """Version of the CTP configuration"""

    statusOn = True
    allowedTypes = [int]
    allowedValues = [0,1,2,3,4,5]
    StoredValue = 4
    
    Limits.setLimits(3)

    def _do_action(self):
        """Load the limits"""
        from Limits import Limits
        Limits.setLimits(self.get_Value())


_flags.append(CTPVersion)

class items(JobProperty):
    """Names of items enabled in the selected L1 menu"""
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]

_flags.append(items)

class thresholds(JobProperty):
    """Names of thresholds enabled in selected L1 menu """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []

_flags.append(thresholds)

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
    statusOn = True
    allowedTypes = ['list']
    StoredValue = []

_flags.append(BunchGroupNames)

class MenuPartitioning(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = []

_flags.append(MenuPartitioning)

class OldStylePrescales(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class ThresholdMap(JobProperty):
    statusOn = True
    allowedTypes = ['dict']
    StoredValue = {}

_flags.append(ThresholdMap)


class ItemMap(JobProperty):
    statusOn = True
    allowedTypes = ['dict']
    StoredValue = {}

_flags.append(ItemMap)



_flags.append(OldStylePrescales)



class Lvl1(JobPropertyContainer):
    """ Lvl1 flags used in menu generation """




# execute

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(Lvl1)

for flag in _flags:
    TriggerFlags.Lvl1.add_JobProperty(flag)
del _flags



# make an alias
Lvl1Flags = TriggerFlags.Lvl1



#  LocalWords:  AllowedList


            
