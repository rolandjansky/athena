# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file Simulation/FastSimulation/FastChainPileup/python/FastChain_jobProperties.py
## @purpose Python module to hold common flags to configure JobOptions
## this is needed for configuring the bcid for Out-of-Time PU with the FastChain
##

""" FastChain_jobProperties

"""

__author__ = "J. Schaarschmidt"
__version__= "$Revision$"
__doc__    = "FastChain_jobProperties"

__all__    = [ "FastChain_jobProperties" ]

# kindly stolen from ISF_Flags by E. Ritsch

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties


class FastChainBCID(JobProperty):
    """Steering of FastChain: Setting the BCID for Out-of-Time PU events"""
    statusOn     = True
    allowedTypes = ['list']
    #StoredValue  = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23]
    StoredValue  = [1]

class FastChainPUWeights_lar_em(JobProperty):
    """Steering of FastChain: LAr EM weights for Out-of-Time PU events"""
    statusOn     = True
    allowedTypes = ['list']
    #StoredValue  = [0.48,1.0,0.68,0.25,-0.008,-0.12,-0.17,-0.18,-0.19,-0.19,-0.19,-0.19,-0.19,-0.19,-0.19,-0.19,-0.18,-0.13,-0.07,-0.03,-0.01,-0.003,-0.001]
    StoredValue  = [1.0]

class FastChainPUWeights_lar_hec(JobProperty):
    """Steering of FastChain: LAr HEC weights for Out-of-Time PU events"""
    statusOn     = True
    allowedTypes = ['list']
    #StoredValue  = [0.52,1.0,0.7,0.26,-0.01,-0.15,-0.2,-0.22,-0.23,-0.23,-0.23,-0.23,-0.23,-0.23,-0.22,-0.15,-0.08,-0.04,-0.01,-0.005,-0.002,-0.0006,0.0]
    StoredValue  = [1.0]

class FastChainPUWeights_lar_bapre(JobProperty):
    """Steering of FastChain: LAr Barrel presampler weights for Out-of-Time PU events"""
    statusOn     = True
    allowedTypes = ['list']
    #StoredValue  = [0.42,1.0,0.73,0.33,0.07,-0.06,-0.12,-0.15,-0.16,-0.16,-0.16,-0.16,-0.16,-0.16,-0.16,-0.16,-0.16,-0.16,-0.16,-0.16,-0.12,-0.07,-0.03]
    StoredValue  = [1.0]

class FastChainPUWeights_tile(JobProperty):
    """Steering of FastChain: Tile weights for Out-of-Time PU events"""
    statusOn     = True
    allowedTypes = ['list']
    #StoredValue  = [0.5,1.0,0.5,0.06,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0]
    StoredValue  = [1.0]


#class FastChainPUWeights(JobProperty):
#    """Steering of FastChain: global weights for Out-of-Time PU events"""
#    statusOn     = True
#    allowedTypes = ['list']
#    StoredValue  = [0.5,2.0,-0.3]


##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class FastChain_jobProperties(JobPropertyContainer):
    """Container for the FastChain key flags
    """
    pass


##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container

jobproperties.add_Container(FastChain_jobProperties)

##-----------------------------------------------------------------------------
## 4th step
## adding flags to the jobProperties container

jobproperties.FastChain_jobProperties.add_JobProperty(FastChainBCID)
#jobproperties.FastChain_jobProperties.add_JobProperty(FastChainPUWeights)
jobproperties.FastChain_jobProperties.add_JobProperty(FastChainPUWeights_lar_em)
jobproperties.FastChain_jobProperties.add_JobProperty(FastChainPUWeights_lar_hec)
jobproperties.FastChain_jobProperties.add_JobProperty(FastChainPUWeights_lar_bapre)
jobproperties.FastChain_jobProperties.add_JobProperty(FastChainPUWeights_tile)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select ISF_jobProperties as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from ISF_Example.ISF_jobProperties import ISF_Flags

FastChain_Flags = jobproperties.FastChain_jobProperties

