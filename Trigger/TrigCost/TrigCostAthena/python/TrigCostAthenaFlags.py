# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class TrigCostAthenaFlagsContainer(JobPropertyContainer):
    pass
jobproperties.add_Container(TrigCostAthenaFlagsContainer)

class StoreNtVerticesOutputFile(JobProperty):
    statusOn = False
    allowedTypes = ['str']
    StoredValue = 'vertices.root'
jobproperties.TrigCostAthenaFlagsContainer.add_JobProperty( StoreNtVerticesOutputFile )

class DoStoreNtVertices(JobProperty):
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = False
jobproperties.TrigCostAthenaFlagsContainer.add_JobProperty( DoStoreNtVertices )

TrigCostAthenaFlags = jobproperties.TrigCostAthenaFlagsContainer
