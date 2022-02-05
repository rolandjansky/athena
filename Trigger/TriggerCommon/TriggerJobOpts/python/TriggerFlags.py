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
        log.warning("TriggerFlags.AODEDMSet is deprecated. Use ConfigFlags.Trigger.AODEDMSet instead.")
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        ConfigFlags.Trigger.AODEDMSet = self.get_Value()

_flags.append(AODEDMSet)


class ESDEDMSet(JobProperty):
    """ Define which sets of object go to ESD (or derived ESD) """
    statusOn=True
    allowedType=['list']
    StoredValue='ESD'
    def _do_action(self):
        log.error("TriggerFlags.ESDEDMSet is deprecated. Use ConfigFlags.Trigger.ESDEDMSet instead.")
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        ConfigFlags.Trigger.ESDEDMSet = self.get_Value()

_flags.append(ESDEDMSet)


class configForStartup(JobProperty):
    """ A temporary flag to determine the actions to be taken for the different cases of HLT running in the startup phase"""
    statusOn=True
    allowedType=['string']
    StoredValue = None
    def _do_action(self):
        log.error("TriggerFlags.configForStartup is deprecated. Remove it from your configuration.")

_flags.append(configForStartup)


class triggerConfig(JobProperty):
    statusOn=True
    allowedType=['string']
    StoredValue = None

    def _do_action(self):
        log.error("TriggerFlags.triggerConfig is deprecated. Use ConfigFlags.Trigger.triggerConfig instead.")
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        ConfigFlags.Trigger.triggerConfig = self.get_Value()

_flags.append(triggerConfig)


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
