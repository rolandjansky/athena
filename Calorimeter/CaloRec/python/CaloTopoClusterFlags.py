# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: CaloTopoClusterFlags.py,v 1.5 2009-05-04 16:23:04 lochp Exp $
#
# File: CaloRec/python/CaloTopoClusterFlags.py
# Created: June 2007, S.Menke
# Purpose: Define flags to steer topo cluster algorithm
#
# Modified: May 4, 2009, P.Loch
# Purpose: Added steering for H1-style calibration of TopoClusters

"""  A container of flags for CaloTopoCluster making configuring
"""

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#
class doTopoClusterLocalCalib(JobProperty):
    """ Flag to activate Local Hadron Calibration
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

#
class usePhysicsList(JobProperty):
    """ indicate the Physics List most appropriate for Local Hadron Calibration
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "QGSP_BERT"

#
class doClusterVertexFraction(JobProperty):
    """ Flag to activate calculation of cluster vertex fraction and append it as a cluster moment
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
                           
#
class doCalibHitMoments(JobProperty):
    """ Flag to activate calibration hit based cluster moments
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
#>> PL May 4, 2009
# Defines if cell weight signal is to be calculated
class doCellWeightCalib(JobProperty):
    """ Apply H1-style cell weighting to cluster (3rd signal state)
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
# Defines reference jet finder for cell weights
class cellWeightRefFinder(JobProperty):
    """ Default jet finder for calibration weight extraction
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ["Cone","Kt","SISCone","AntKt"]
    StoredValue   = "Cone"
# Defines reference jet size for cell weights
class cellWeightRefSize(JobProperty):
    """ Default jet size for calibration weight extraction
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.4
# Defines reference jet calorimeter signal
class cellWeightRefSignal(JobProperty):
    """ Default calorimeter signal for cell weights
    """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ["Tower","Topo"]
    StoredValue   = "Topo"

class doTwoGaussianNoise(JobProperty):
    """ Flag to activate two gaussian noise description
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

# Defines the container for the topo cluster making flags
class CaloTopoClusterFlags(JobPropertyContainer):
    """ The CaloTopoCluster making flag property container
    """
    pass
  
#Split clusters according to abs E only   
class doTreatEnergyCutAsAbsolute(JobProperty):    
    """ 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False    
    
#also compute moments from abs E from the Cells only  (has to go together with doTreatEnergyCutAsAbsolute)
class doMomentsfromAbs(JobProperty):    
    """ 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False       
  

class doTimeCut(JobProperty):
    """                                                                                                                    
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class extendTimeCut(JobProperty):
    """                                                                                                                    
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


# add the flags container to the top container 
jobproperties.add_Container(CaloTopoClusterFlags)


# We want always the following flags in the container  
list_jobproperties = [
    doTopoClusterLocalCalib
    ,usePhysicsList
    #,lockTopoClusterSamplingEnergies
    #,lockTopoClusterSamplingVariables
    ,doClusterVertexFraction
    #,printTopoClusters
    ,doCalibHitMoments
    ,doCellWeightCalib
    ,cellWeightRefFinder
    ,cellWeightRefSize
    ,cellWeightRefSignal
    ,doTwoGaussianNoise
    ,doTreatEnergyCutAsAbsolute
    ,doMomentsfromAbs
    ,doTimeCut
    ,extendTimeCut
    ]

for i in list_jobproperties:
    jobproperties.CaloTopoClusterFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties
