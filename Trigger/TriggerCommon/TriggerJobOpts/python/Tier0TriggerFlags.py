# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

	
__author__  = 'Emily Nurse'
__doc__="Tier0 trigger ntuple information"

class Tier0TriggerFlags(JobPropertyContainer):
  """Container for Tier0 trigger ntuple variables"""

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(Tier0TriggerFlags)

class doTier0TriggerNtuple(JobProperty):
   """ global flag to switch on/off the ntuple making"""
   statusOn=True
   allowedType=['bool']
   StoredValue=False

TriggerFlags.Tier0TriggerFlags.add_JobProperty(doTier0TriggerNtuple)


class ntupleName(JobProperty):
   """ set the name for the Tier0 trigger ntuple """
   statusOn = True
   allowedTypes = ['str']
   StoredValue = "Tier0.root"

TriggerFlags.Tier0TriggerFlags.add_JobProperty(ntupleName)

class doMinBiasTree(JobProperty):
   """ do or not the MinBias tree """
   statusOn=True
   allowedTypes=['bool']
   StoredValue=True

TriggerFlags.Tier0TriggerFlags.add_JobProperty(doMinBiasTree)
                          
