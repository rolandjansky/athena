# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file ISF_Config/python/ISF_jobProperties.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" ISF_jobProperties

"""

__author__ = "E. Ritsch"
__version__= "$Revision: 507036 $"
__doc__    = "ISF_jobProperties"

__all__    = [ "ISF_jobProperties" ]

# kindly stolen from FatrasJobProperties by A. Salzburger

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes
class UseParticleGenerator(JobProperty):
    """Steering of ISF: use particle generator to create initial particles"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class RunVP1(JobProperty):
    """Use Virtual Point 1"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class DoPerfMonStats(JobProperty):
    """Run PerfMon profiling while running"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class DumpMcEvent(JobProperty):
    """Dump McEvent Collection after each event"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class DumpStoreGate(JobProperty):
    """Dump StoreGate contents after each event"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class RunValgrind(JobProperty):
    """Dump StoreGate contents after each event"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class ValidationMode(JobProperty):
    """Steering of ISF: run ISF internal validation checks"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    def _do_action(self):
        jobproperties.ISF_jobProperties.TruthService.set_Value("ISF_ValidationTruthService")
    def _undo_action(self):
        jobproperties.ISF_jobProperties.TruthService.set_Value(jobproperties.ISF_jobProperties.TruthService.__class__.StoredValue)

class VertexPositionFromFile(JobProperty):
    """Steering of ISF: run vertex position from file"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class DoMemoryMonitoring(JobProperty):
    """Steering of ISF: do memory monitoring"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class DoTimeMonitoring(JobProperty):
    """Steering of ISF: run time monitoring"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class Input(JobProperty):
    """Steering of ISF: set input"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'NONE'

class Simulator(JobProperty):
    """Steering of ISF: set simulator"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'ATLFASTII'
    def KernelName(self):
        if self.statusOn:
            return 'ISF_Kernel_' + self.StoredValue
    def configFlagsMethodName(self):
        if self.statusOn:
            return 'configureFlags' + self.StoredValue

class HITSMergingRequired(JobProperty):
    """The configured set of simulators requires merging of HITS collections"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class UsingGeant4(JobProperty):
    """Will this job be using Geant4?"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

# currently unused:
#class ParticleBroker(JobProperty):
#    """Steering of ISF: set the Particle Broker Service"""
#    statusOn     = True
#    allowedTypes = ['str']
#    StoredValue  = 'ISF_ParticleBrokerSvc'

class OverrideInputFiles(JobProperty):
    """Steering of ISF: set input override"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []

class RoutingChainBeamPipe(JobProperty):
    """Steering of ISF: the RoutingChain to be used in the BeamPipe"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []

class RoutingChainID(JobProperty):
    """Steering of ISF: the RoutingChain to be used in the InnerDetector"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []

class RoutingChainCalo(JobProperty):
    """Steering of ISF: the RoutingChain to be used in the Calorimeter"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []

class RoutingChainMS(JobProperty):
    """Steering of ISF: the RoutingChain to be used in the MuonSpectrometer"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []

class RoutingChainCavern(JobProperty):
    """Steering of ISF: the RoutingChain to be used in the Cavern"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []

class GeoIDService(JobProperty):
    """The GeoIDService to bue used by ISF"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'NONE'

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class ISF_jobProperties(JobPropertyContainer):
    """Container for the ISF key flags
    """
    pass


##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(ISF_jobProperties)


##-----------------------------------------------------------------------------
## 4th step
## adding flags to the jobProperties container
jobproperties.ISF_jobProperties.add_JobProperty(UseParticleGenerator)
jobproperties.ISF_jobProperties.add_JobProperty(RunVP1)
jobproperties.ISF_jobProperties.add_JobProperty(DoPerfMonStats)
jobproperties.ISF_jobProperties.add_JobProperty(DumpMcEvent)
jobproperties.ISF_jobProperties.add_JobProperty(DumpStoreGate)
jobproperties.ISF_jobProperties.add_JobProperty(RunValgrind)
jobproperties.ISF_jobProperties.add_JobProperty(ValidationMode)
jobproperties.ISF_jobProperties.add_JobProperty(VertexPositionFromFile)
jobproperties.ISF_jobProperties.add_JobProperty(DoMemoryMonitoring)
jobproperties.ISF_jobProperties.add_JobProperty(DoTimeMonitoring)
jobproperties.ISF_jobProperties.add_JobProperty(Input)
jobproperties.ISF_jobProperties.add_JobProperty(Simulator)
jobproperties.ISF_jobProperties.add_JobProperty(HITSMergingRequired)
jobproperties.ISF_jobProperties.add_JobProperty(UsingGeant4)
#jobproperties.ISF_jobProperties.add_JobProperty(ParticleBroker)
jobproperties.ISF_jobProperties.add_JobProperty(OverrideInputFiles)
jobproperties.ISF_jobProperties.add_JobProperty(RoutingChainBeamPipe)
jobproperties.ISF_jobProperties.add_JobProperty(RoutingChainID)
jobproperties.ISF_jobProperties.add_JobProperty(RoutingChainCalo)
jobproperties.ISF_jobProperties.add_JobProperty(RoutingChainMS)
jobproperties.ISF_jobProperties.add_JobProperty(RoutingChainCavern)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select ISF_jobProperties as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from ISF_Example.ISF_jobProperties import ISF_Flags
ISF_Flags = jobproperties.ISF_jobProperties
