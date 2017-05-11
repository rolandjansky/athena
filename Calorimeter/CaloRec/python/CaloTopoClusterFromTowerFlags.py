# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class clusterMakerName(JobProperty):
    """ Provides name of ClusterMaker algorithm
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TowerClusterMaker'

class towerConverterName(JobProperty):
    """ Provides name of CaloTopoClusterFromTowerMaker
    """
    statusON     = True
    allowedTypes = ['str']
    StoredValie  = 'TowerClusterConverter'

class inputTowerContainerKey(JobProperty):
    """ Provides input tower container to cluster-from-tower maker
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CmbTower'

class orderByPt(JobProperty):
    """ Orders output cluster container by Pt
    """
    statusOn     = True;
    allowedTypes = ['bool']
    StoredValue  = False


class inputCellContainerKey(JobProperty):
    """ Cell container key
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'AllCell' 

class towerGrid(JobProperty):
    """ Tower grid
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'Coarse' ## alt value "Fine"

class CaloTopoClusterFromTowerFlags(JobPropertyContainer):
    """ Always add container to job properties
    """
    pass

jobproperties.add_Container(CaloTopoClusterFromTowerFlags)

# we want always the following flags in the container  
list_jobproperties = [
    clusterMakerName,
    towerConverterName,
    inputTowerContainerKey,
    orderByPt,
    inputCellContainerKey,
    towerGrid
    ]

for prop in list_jobproperties:
    jobproperties.CaloTopoClusterFromTowerFlags.add_JobProperty(prop)

## module clean-up
del list_jobproperties




