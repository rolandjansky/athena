# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file TruthD3PDMaker/python/TruthD3PDMakerFlags.py
## @brief Python module to hold TruthD3PDMaker common flags
## @author Georges Aad
## @date Nov, 2010
##



"""
This file is used to configure the Gen objects in the TruthD3PDMaker
"""



##-----------------------------------------------------------------------------
## Import
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys


class GenParticleAssocLabel(JobProperty):
    """ label for the gen particle getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=TruthD3PDKeys.GenParticleGetterLabel()


class GenVertexAssocLabel(JobProperty):
    """ label for the gen vertex getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=TruthD3PDKeys.GenVertexGetterLabel()

class GenEventAssocLabel(JobProperty):
    """ label for the gen event getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=TruthD3PDKeys.GenEventGetterLabel()

class TruthTrackAssocLabel(JobProperty):
    """ label for the truth track getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=TruthD3PDKeys.TruthTrackGetterLabel()


class GenVertexOutPartAssoc(JobProperty):
    """ switch to associate gen vertex to out gen particles
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class GenVertexInPartAssoc(JobProperty):
    """ switch to associate gen vertex to in gen particles
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class GenParticleMother(JobProperty):
    """ switch to associate gen particles to there mothers
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class GenParticleChild(JobProperty):
    """ switch to associate gen particles to there children
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class TruthD3PDOutputFileName(JobProperty):
    """ label for the truth track getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""

class WriteTruthVertices(JobProperty):
    """ Switch to save truth vertices
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class WriteTruthVertexIDs(JobProperty):
    """ Switch to save truth vertices
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class TruthD3PDMakerFlags(JobPropertyContainer):
    """ The Truth D3PD Flags container
    """

# add the flag container to the top container
jobproperties.add_Container(TruthD3PDMakerFlags)

# and now add the properties

jobproperties.TruthD3PDMakerFlags.add_JobProperty(GenParticleAssocLabel)
jobproperties.TruthD3PDMakerFlags.add_JobProperty(GenVertexAssocLabel)
jobproperties.TruthD3PDMakerFlags.add_JobProperty(GenEventAssocLabel)
jobproperties.TruthD3PDMakerFlags.add_JobProperty(TruthTrackAssocLabel)
jobproperties.TruthD3PDMakerFlags.add_JobProperty(GenVertexOutPartAssoc)
jobproperties.TruthD3PDMakerFlags.add_JobProperty(GenVertexInPartAssoc)
jobproperties.TruthD3PDMakerFlags.add_JobProperty(GenParticleMother)
jobproperties.TruthD3PDMakerFlags.add_JobProperty(GenParticleChild)
jobproperties.TruthD3PDMakerFlags.add_JobProperty(TruthD3PDOutputFileName)
jobproperties.TruthD3PDMakerFlags.add_JobProperty(WriteTruthVertices)
jobproperties.TruthD3PDMakerFlags.add_JobProperty(WriteTruthVertexIDs)

## use this shortcut
#from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags
##
TruthD3PDFlags=jobproperties.TruthD3PDMakerFlags
