# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# From Marcelo Vogel to allow setting the name of the output ROOT file for DAPR0
# See https://its.cern.ch/jira/browse/PRODSYS-601 for more info

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
 
class outputFile(JobProperty):
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'PILEUP.root'
    
## Definition of the flag container
class DataPrepJobProperties(JobPropertyContainer):
    """Container for the DerivationFrameworkDataPrep key flags
    """
    pass

## adding the container to the general top-level container
jobproperties.add_Container(DataPrepJobProperties)
 
## adding the flag to the  container
jobproperties.DataPrepJobProperties.add_JobProperty( outputFile )

## shortcut to access flags
DataPrepFlags = jobproperties.DataPrepJobProperties
