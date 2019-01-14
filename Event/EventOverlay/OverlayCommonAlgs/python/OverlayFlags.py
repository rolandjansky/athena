# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Flag class for Pileup Overlay building

usage:
from OverlayCommonAlgs.OverlayFlags import overlayFlags
overlayFlags.EventIDTextFile= runArgs.inputTXT_EVENTIDFile[0]

"""

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

from AthenaCommon.Logging import logging

overlayflaglog = logging.getLogger('Overlay_Flags')

class isOverlayMT(JobProperty):
    """Flag to separate the new MT-ready overlay from the old one"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue = False

class isDataOverlay(JobProperty):
    """Flag to separate data vs MC overlay"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue = True

class EventIDTextFile(JobProperty):
    """Name of the Event ID Text file"""
    statusOn=True
    allowedTypes=['str']
    StoredValue = 'events.txt'

class signalOnlyOutput(JobProperty):
    """Output signal-only RDOs"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue = False

class PoolSignalRDOOutput(JobProperty):
    """The name of the output signal POOL RDO file"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "RDO_SGNL.pool.root"     

class evtStore(JobProperty):
    """MC EvtStore name for overlay"""
    statusOn=True
    allowedTypes=['str']
    StoredValue = 'BkgEvent_0_SG'

class dataStore(JobProperty):
    """Data EvtStore name for overlay"""
    statusOn=True
    allowedTypes=['str']
    StoredValue = 'OriginalEvent_SG'
    
class outputStore(JobProperty):
    """Output EvtStore name for overlay"""
    statusOn=True
    allowedTypes=['str']
    StoredValue = 'StoreGateSvc'

class bkgPrefix(JobProperty):
    """Overlay background StoreGate key prefix"""
    statusOn=True
    allowedTypes=['str']
    StoredValue = 'Bkg_'

class sigPrefix(JobProperty):
    """Overlay signal StoreGate key prefix"""
    statusOn=True
    allowedTypes=['str']
    StoredValue = 'Sig_'

class extraTagInfoPairs(JobProperty):
    """ workaround to correctly populate the /TagInfo metadata container
    """
    statusOn=False
    allowedTypes=['dict']
    StoredValue=dict()

## Definition and registration of the simulation flag container

class Overlay_Flags(JobPropertyContainer):
    """ The global Overlay flag/job property container.
    """
## Register and populate the Overlay_Flags container

jobproperties.add_Container(Overlay_Flags)
for jpname in dir():
    jp = eval(jpname)
    import inspect
    if inspect.isclass(jp):
        if issubclass(jp, JobProperty) and jp is not JobProperty:
            overlayflaglog.debug("Adding OverlayFlag '%s' to Overlay_Flags container" % jpname)
            jobproperties.Overlay_Flags.add_JobProperty(jp)

## Short-cut alias (standard naming)
overlayFlags = jobproperties.Overlay_Flags
