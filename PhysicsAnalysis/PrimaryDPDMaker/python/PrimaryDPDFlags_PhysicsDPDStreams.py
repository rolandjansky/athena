# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_PhysicsDPDStreams.py
##
## Author:      Karsten Koeneke (DESY)
## Created:     August 2008
##
## Description: Here, all neccessary job flags for the Primary physics DPDs
##              are defined.
##=============================================================================

__doc__ = """Here, all neccessary job flags for the Primary Physics DPDs are defined.
"""

__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_PhysicsDPDStreams(JobPropertyContainer):
    """ The Primary Photon-Jet DPD flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_PhysicsDPDStreams)

#short-cut to get the PrimaryDPDFlags_PhysicsDPDStreams container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_PhysicsDPDStreams import primPhysDPD'
#Note that name has to be different to avoid problems with pickle
primPhysDPD=jobproperties.PrimaryDPDFlags_PhysicsDPDStreams






#=====================================================================
# Now define each flag and add it to the container
#=====================================================================


##--------------------------------
## Sub-options for the Physics DPD output stream
##--------------------------------
class ApplyTriggerSlimming(JobProperty):
    """ Apply the slimming of the trigger (removing parts of the object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_PhysicsDPDStreams.add_JobProperty(ApplyTriggerSlimming)

class ApplyTrackParticleSlimming(JobProperty):
    """ Apply the slimming of the track particles (removing parts of the object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_PhysicsDPDStreams.add_JobProperty(ApplyTrackParticleSlimming)



##--------------------------------
## Other stream content
##--------------------------------
class WriteMCTruth(JobProperty):
    """ Write out the MC Truth."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_PhysicsDPDStreams.add_JobProperty(WriteMCTruth)

class WriteAtlfast(JobProperty):
    """ Write the Atlfast containers."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_PhysicsDPDStreams.add_JobProperty(WriteAtlfast)


##--------------------------------
## Define which jet collections to use
##--------------------------------
class JetFilter1Properties(JobProperty):
    """ Define which jet collections to use for the event selection."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    jetCollectionName  = "Cone4TowerJets"
    cutEtMin           = 20.0*Units.GeV
    cutEtMax           = 14000.0*Units.GeV
    cutMinMultiplicity = 2
    pass
jobproperties.PrimaryDPDFlags_PhysicsDPDStreams.add_JobProperty(JetFilter1Properties)

class JetFilter2Properties(JobProperty):
    """ Define which jet collections to use for the event selection."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    jetCollectionName  = "Cone4TopoJets"
    cutEtMin           = 20.0*Units.GeV
    cutEtMax           = 14000.0*Units.GeV
    cutMinMultiplicity = 2
    pass
jobproperties.PrimaryDPDFlags_PhysicsDPDStreams.add_JobProperty(JetFilter2Properties)
