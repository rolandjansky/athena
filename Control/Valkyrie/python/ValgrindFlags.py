# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: ValgrindFlags.py
# @purpose: a container of flags for customizing valgrind
# @author: Sebastien Binet <binet@cern.ch>

"""  A container of flags for customizing valgrind
"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.1.1.2 $"
__doc__     = "A container of flags for customizing valgrind"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties


# Defines the top-level container for the valgrind flags
class ValgrindFlags(JobPropertyContainer):
    """ The top-level container of flags driving valgrind customization.
    """
    pass

class doMemCheck(JobProperty):
    """
    """
    statusOn      = False
    allowedTypes  = ['bool']
    StoredValue   = False
    def _do_action(self):
        jobproperties.ValgrindFlags.doCallGrind.set_Off()
        jobproperties.ValgrindFlags.doMassif.set_Off()
    pass
#
class doCallGrind(JobProperty):
    """
    """
    statusOn      = False
    allowedTypes  = ['bool']
    StoredValue   = False
    def _do_action(self):
        jobproperties.ValgrindFlags.doMemCheck.set_Off()
        jobproperties.ValgrindFlags.doMassif.set_Off()
    pass
#
class doMassif(JobProperty):
    """
    """
    statusOn      = False
    allowedTypes  = ['bool']
    StoredValue   = False
    def _do_action(self):
        jobproperties.ValgrindFlags.doMemCheck.set_Off()
        jobproperties.ValgrindFlags.doCallGrind.set_Off()
    pass
#
class TraceChildren(JobProperty):
    """
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['yes','no']
    StoredValue   = 'yes'
    pass
#
class NumCallers(JobProperty):
    """
    """
    statusOn      = True
    allowedTypes  = ['int']
#    allowedValues = []
    StoredValue   = 20
    pass
#
class SuppressionFile(JobProperty):
    """Path to suppression file
    """
    statusOn      = True
    allowedTypes  = ['str']
#    allowedValues = []
    StoredValue   = 'atlas.supp'
    pass

# add the valgrind flags container to the top container 
jobproperties.add_Container(ValgrindFlags)
for i in [ doMemCheck, doCallGrind, doMassif,
           TraceChildren, NumCallers, SuppressionFile ]:
    jobproperties.ValgrindFlags.add_JobProperty(i)

## ------- [MemCheck flags and container] --------
class MemCheck(JobPropertyContainer):
    """ Container of flags for the MemCheck tool
    """
    pass
#
class ShowReachable(JobProperty):
    """ show reachable blocks in leak check?
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['yes','no']
    StoredValue   = 'yes'
    pass
#
class LeakCheck(JobProperty):
    """ search for memory leaks at exit?
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['no','summary','full']
    StoredValue   = 'full'
    pass
#
class LeakResolution(JobProperty):
    """ how much bt merging in leak check
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['low','med','high']
    StoredValue   = 'low'
    
jobproperties.ValgrindFlags.add_Container( MemCheck )
for i in [ ShowReachable, LeakCheck, LeakResolution ]:
    jobproperties.ValgrindFlags.MemCheck.add_JobProperty(i)

## ------- [CallGrind flags and container] --------
#
class CallGrind(JobPropertyContainer):
    """ Container of flags for the CallGrind tool
    """
    pass

#
class DumpInstruction(JobProperty):
    """ Dump instruction address of costs?
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['yes','no']
    StoredValue   = 'yes'
    pass
#
class CollectJumps(JobProperty):
    """ Collect jumps?
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['yes','no']
    StoredValue   = 'yes'
    pass
#
class CollectSystemTime(JobProperty):
    """ Collect system call time info?
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['yes','no']
    StoredValue   = 'no'
    pass
#
class InstrumentAtStart(JobProperty):
    """ Do instrumentation at callgrind start
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['yes','no']
    StoredValue   = 'yes'
    pass
#
class CollectAtStart(JobProperty):
    """ Collect at process/thread start
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['yes','no']
    StoredValue   = 'yes'
    pass
#
class ToggleCollect(JobProperty):
    """ Toggle collection on enter/leave function
    """
    statusOn      = False
    allowedTypes  = ['str']
#    allowedValues = []
#    StoredValue   = 'MyAlgorithm::execute'
    StoredValue   = ''
    pass

jobproperties.ValgrindFlags.add_Container( CallGrind )
for i in [ DumpInstruction,
           CollectJumps, CollectSystemTime,
           InstrumentAtStart, CollectAtStart,
           ToggleCollect ]:
    jobproperties.ValgrindFlags.CallGrind.add_JobProperty(i)


## ------- [Massif flags and container] --------
#
class Massif(JobPropertyContainer):
    """ Container of flags for the Massif tool
    """
    pass

#
class Heap(JobProperty):
    """ profile heap blocks
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['yes','no']
    StoredValue   = 'yes'
    pass
#
class Stacks(JobProperty):
    """ profile stack(s)
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['yes','no']
    StoredValue   = 'yes'
    pass
#
class Format(JobProperty):
    """ format of textual output
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['html','text']
    StoredValue   = 'html'
    pass
#
class Depth(JobProperty):
    """ depth of contexts
    """
    statusOn      = True
    allowedTypes  = ['int']
#    allowedValues = []
    StoredValue   = 20

jobproperties.ValgrindFlags.add_Container( Massif )
for i in [ Heap, Stacks, Format, Depth ]:
    jobproperties.ValgrindFlags.Massif.add_JobProperty(i)
