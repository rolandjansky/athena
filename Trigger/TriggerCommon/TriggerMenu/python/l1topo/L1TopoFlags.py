# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
L1Topo specific flags
"""

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.Logging import logging

log = logging.getLogger('TriggerMenu.L1TopoFlags.py')

_flags = [] 

class algos(JobProperty):
    """Names of algos enabled in the selected L1Topo menu"""
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]

_flags.append(algos)  

class L1TopoTriggerFlags(JobPropertyContainer):
    """ L1Topo flags used in menu generation """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(L1TopoTriggerFlags)

for flag in _flags:
    TriggerFlags.L1TopoTriggerFlags.add_JobProperty(flag)
del _flags

# make an alias
L1TopoFlags = TriggerFlags.L1TopoTriggerFlags
            
