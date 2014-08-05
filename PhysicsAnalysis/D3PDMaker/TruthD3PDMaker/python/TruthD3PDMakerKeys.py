# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file TruthD3PDMaker/python/TruthD3PDMakerKeys.py
## @brief Python module to hold TruthD3PDMaker common Keys
## @author Georges Aad
## @date Nov, 2010
##



"""
This file is used to set different conventions for the Gen D3PD objects:
block names and common labels ...
In most cases you do not need to change them.
"""

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties




class GenEventGetterLabel(JobProperty):
    """ label for gen event getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="TruthD3PD_GenEvent"

class GenEventPrefix(JobProperty):
    """ prefix for gen event object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="mcevt_"


class GenVertexGetterLabel(JobProperty):
    """ label for gen vertex getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="TruthD3PD_GenVertex"

class GenVertexPrefix(JobProperty):
    """ prefix for gen vertex object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="mcvtx_"


class GenParticleGetterLabel(JobProperty):
    """ label for gen particle getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="TruthD3PD_GenParticle"

class GenParticlePrefix(JobProperty):
    """ prefix for gen particle object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="mcpart_"


class TruthTrackGetterLabel(JobProperty):
    """ label for truth track getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="TruthD3PD_TruthTrack"

class TruthTrackPrefix(JobProperty):
    """ prefix for truth track object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="truthtrack_"


##-----------------------------------------------------------------------------
## Definition of the TruthD3PDMaker flag container

class TruthD3PDMakerKeys(JobPropertyContainer):
    """ The Truth D3PD keys container
    """
# add the key container to the top container
jobproperties.add_Container(TruthD3PDMakerKeys)

# and now add the properties

jobproperties.TruthD3PDMakerKeys.add_JobProperty(GenEventGetterLabel)
jobproperties.TruthD3PDMakerKeys.add_JobProperty(GenEventPrefix)
jobproperties.TruthD3PDMakerKeys.add_JobProperty(GenVertexGetterLabel)
jobproperties.TruthD3PDMakerKeys.add_JobProperty(GenVertexPrefix)
jobproperties.TruthD3PDMakerKeys.add_JobProperty(GenParticleGetterLabel)
jobproperties.TruthD3PDMakerKeys.add_JobProperty(GenParticlePrefix)
jobproperties.TruthD3PDMakerKeys.add_JobProperty(TruthTrackGetterLabel)
jobproperties.TruthD3PDMakerKeys.add_JobProperty(TruthTrackPrefix)


## use this shortcut
#from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys
##
TruthD3PDKeys=jobproperties.TruthD3PDMakerKeys
