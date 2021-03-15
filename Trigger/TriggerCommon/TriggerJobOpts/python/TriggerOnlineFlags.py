# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

################################################################################
# @file   TriggerOnlineFlags.py
# @author Frank Winklmeier
# @brief  Trigger flags for online running
################################################################################

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties # noqa: F401

_flags = []

class doDBConfig(JobProperty):
   """ Flag to extract the trigger configuration """
   statusOn = True
   allowedTypes = ['bool']
   StoredValue = False   

_flags += [doDBConfig]

class doDBConfigBaseName(JobProperty):
   """ Base name of the file that holds the trigger configuration """
   statusOn = True
   allowedTypes = ['str']
   StoredValue = ''   

_flags += [doDBConfigBaseName]

class doValidation(JobProperty):
   """ Configure job for validation (i.e. remove time stamps from messages) """
   statusOn = True
   allowedTypes = ['bool']
   StoredValue = True

_flags += [doValidation]

class partitionName(JobProperty):
   """ Name of the partition if running in online environment, otherwise empty """
   statusOn = True
   allowedTypes = ['str']
   import os
   StoredValue = os.getenv('TDAQ_PARTITION') or ''

_flags += [partitionName]

# Create "Online" container
class Online(JobPropertyContainer):
   """ Trigger online flags
   """
   pass

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(Online)

# Add all flags
for f in _flags:
   TriggerFlags.Online.add_JobProperty(f)
