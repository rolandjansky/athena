# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#
# The file is obsolete and replaced by TriggerConfigFlags.py. Do not add any new flags!
#

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerJobOpts.TriggerFlags' )

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer

_flags = []

class AODEDMSet(JobProperty):
    """ Define which sets of object go to AOD """
    statusOn=True
    allowedType=['list']
    StoredValue='AODSLIM'
    def _do_action(self):
        log.error("TriggerFlags.AODEDMSet is deprecated. Use ConfigFlags.Trigger.AODEDMSet instead.")
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        ConfigFlags.Trigger.AODEDMSet = self.get_Value()

_flags.append(AODEDMSet)


# the container of all trigger flags

class Trigger(JobPropertyContainer):
    """ Trigger top flags """
    pass

## attach yourself to the RECO flags
from RecExConfig.RecFlags import rec
rec.add_Container(Trigger)


for flag in _flags:
    rec.Trigger.add_JobProperty(flag)
del _flags

## make an alias for trigger flags which looks like old TriggerFlags class
TriggerFlags = rec.Trigger
