# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: AthenaMPFlags.py
# @purpose: a container of flags for AthenaMP settings
# @author: Mous Tatarkhanov <tmmous@berkeley.edu>

__author__  = 'Mous Tatarkhanov'
__version__ = "$Revision: 1.17 $"
__doc__     = "A container of flags for AthenaMP"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class WorkerTopDir(JobProperty):
    """ Flag to set the location for worker's run dir
    """
    statusOn = True
    allowedTypes = ['str']
    StoredValue  = 'athenaMP_workers'

class OutputReportFile(JobProperty):
    """ Flag to set the name of AthenaMP output report file
    """
    statusOn = True
    allowedTypes = ['str']
    StoredValue  = 'AthenaMPOutputs'

class AffinityCPUList(JobProperty):
    """ Flag to set the pining of parallel workers to cpu from the list"""
    statusOn = True
    allowdTypes = ['list']
    StoredValue = []

class Strategy(JobProperty):
    """ Switch to choose event scheduling strategy"""
    statusOn = True
    allowedTypes = ['str']
    allowedValues = ['SharedQueue','FileScheduling','EventService','RoundRobin']    
    StoredValue = 'SharedQueue'

class CollectSubprocessLogs(JobProperty):
    """ Flag for including all sub-process logs in the master log file
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class Version(JobProperty):
    """ To distinguish between AthenaMP 1 and 2 (requested by transforms)
    """
    statusOn = True
    allowedTypes = ['int']
    allowedValues = [2]
    StoredValue  = 2

class PollingInterval(JobProperty):
    """ Time interval in milliseconds between subsequent polling of subproceses
    """
    statusOn = True
    allowedTypes = ['int']
    StoredValue  = 100

class EventsBeforeFork(JobProperty):
    """ Flag to set the nbr of events to process before forking.
    """
    statusOn = True
    allowedTypes = ['int']
    StoredValue  = 0

class EventRangeChannel(JobProperty):
    """ For the Event Service: Channel for communicating Event Ranges with the Pilot
    """
    statusOn = True
    allowedTypes = ['str']
    StoredValue = 'EventService_EventRanges'

class EvtRangeScattererCaching(JobProperty):
    """ For the Event Service: flag for activating extra event caching by the EvtRangeScatterer
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class MemSamplingInterval(JobProperty):
    """ Time interval in seconds between taking memory samples
    """
    statusOn = True
    allowedTypes = ['int']
    StoredValue  = 0

class ChunkSize(JobProperty):
    """ Size of event chunks in the shared queue
    """
    statusOn = True
    allowedTypes = ['int']
    StoredValue  = 1

class ReadEventOrders(JobProperty):
    """ Flag for reading event orders (Shared Queue) from an ASCII file for reproducibility
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class EventOrdersFile(JobProperty):
    """ Name of the file for storing/reading event orders (Shared Queue)
    """
    statusOn = True
    allowedTypes = ['str']
    StoredValue = 'athenamp_eventorders.txt'

class UseSharedReader(JobProperty):
    """ Flag for activating shared reader process
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class UseSharedWriter(JobProperty):
    """ Flag for activating shared writer process
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

# Defines the container for the performance monitoring flags  
class AthenaMPFlags(JobPropertyContainer):
    """ The global AthenaMP job property container.
    """
    pass

jobproperties.add_Container(AthenaMPFlags)


# We want the following flags in the container  
list_jobproperties = [
    WorkerTopDir,
    OutputReportFile,
    AffinityCPUList,
    Strategy,
    CollectSubprocessLogs,
    Version,
    PollingInterval,
    EventsBeforeFork,
    EventRangeChannel,
    MemSamplingInterval,
    EvtRangeScattererCaching,
    ChunkSize,
    ReadEventOrders,
    EventOrdersFile,
    UseSharedReader,
    UseSharedWriter,
    ]

for i in list_jobproperties:
    jobproperties.AthenaMPFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties
