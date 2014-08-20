# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################################
# @file   TriggerOnlineFlags.py
# @author Frank Winklmeier
# @brief  Trigger flags for online running
# $Id: TriggerOnlineFlags.py 587315 2014-03-12 13:58:52Z krasznaa $
################################################################################

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

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
del _flags

OnlineFlags = TriggerFlags.Online
