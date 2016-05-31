# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class METRecoFlags(JobPropertyContainer):
    pass

class DoRegions(JobProperty):
    statusOn = True
    allowedTypes = ['bool'] 
    StoredValue  = False

class DoPFlow(JobProperty):
    statusOn = True
    allowedTypes = ['bool'] 
    StoredValue  = True

class UseTracks(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True

class DecorateSoftConst(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False

class AllowOverwrite(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False

class METConfigs(JobProperty):
    statusOn = True
    allowedTypes = ['dict'] 
    StoredValue  = {}

class METRecoTools(JobProperty):
    statusOn = True
    allowedTypes = ['dict'] 
    StoredValue  = {}

class METOutputList(JobProperty):
    statusOn = True
    allowedTypes = ['list'] 
    StoredValue  = []

class METAssocConfigs(JobProperty):
    statusOn = True
    allowedTypes = ['dict'] 
    StoredValue  = {}

class METAssocTools(JobProperty):
    statusOn = True
    allowedTypes = ['dict'] 
    StoredValue  = {}

class METAssocOutputList(JobProperty):
    statusOn = True
    allowedTypes = ['list'] 
    StoredValue  = []

jobproperties.add_Container(METRecoFlags)

jobproperties.METRecoFlags.add_JobProperty(DoRegions)
jobproperties.METRecoFlags.add_JobProperty(DoPFlow)
jobproperties.METRecoFlags.add_JobProperty(UseTracks)
jobproperties.METRecoFlags.add_JobProperty(DecorateSoftConst)
jobproperties.METRecoFlags.add_JobProperty(AllowOverwrite)
jobproperties.METRecoFlags.add_JobProperty(METConfigs)
jobproperties.METRecoFlags.add_JobProperty(METOutputList)
jobproperties.METRecoFlags.add_JobProperty(METRecoTools)
jobproperties.METRecoFlags.add_JobProperty(METAssocConfigs)
jobproperties.METRecoFlags.add_JobProperty(METAssocOutputList)
jobproperties.METRecoFlags.add_JobProperty(METAssocTools)

metFlags = jobproperties.METRecoFlags
