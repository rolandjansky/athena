# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file TrackD3PDMaker/python/VertexD3PDAnalysisFlags.py
## @brief Python module to hold VertexD3PDAnalysis common flags
## @author Simone Pagan Griso
## @date Mar, 2012
##



"""
This file is used to configure the VertexD3PDAnalysis objects:
enabling specific branch writing, ...
"""

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags

## Example flags
#class doExampleFlag(JobProperty):
#    """ Comment
#    """
#    statusOn=True
#    allowedType=['bool'] #['int','float',...]
#    StoredValue=False
#

## General flags

class D3PDMakerExtraDict(JobProperty):
    """ Extra options for D3PD making
    """
    statusOn=True
    allowedType=['dict'] 
    StoredValue={}

class useEventInfo(JobProperty):
    """ Store/use track information
    """
    statusOn=True
    allowedType=['bool'] 
    StoredValue=True

class useTracks(JobProperty):
    """ Store/use track information
    """
    statusOn=True
    allowedType=['bool'] 
    StoredValue=True

class useBeamspot(JobProperty):
    """ Store/use beamspot information
    """
    statusOn=True
    allowedType=['bool'] 
    StoredValue=True

class useTrigger(JobProperty):
    """ Store/use trigger information
    """
    statusOn=True
    allowedType=['bool'] 
    StoredValue=True

class useTriggerRaw(JobProperty):
    """ Store/use trigger RAW information
    """
    statusOn=True
    allowedType=['bool'] 
    StoredValue=False

class useBackgroundWord(JobProperty):
    """ Store/use beam background word information
    """
    statusOn=True
    allowedType=['bool'] 
    StoredValue=True

class useSecondaryVertex(JobProperty):
    """ Store/use V0 information
    """
    statusOn=True
    allowedType=['bool'] 
    StoredValue=False

class useAllVertexCollections(JobProperty):
    """ Store/use split and non BC vertices
    """
    statusOn=True
    allowedType=['bool'] 
    StoredValue=True

class useTruth(JobProperty):
    """ Store/use truth information
    """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

class filterTightTruth(JobProperty):
    """ Only keep charged stable particles from in-time pile-up
    """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

class useMET(JobProperty):
    """ Store/use Missing Energy information
    """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

class useMuons(JobProperty):
    """ Store/use Muons information
    """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

class useElectrons(JobProperty):
    """ Store/use Electrons information
    """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

class usePhotons(JobProperty):
    """ Store/use Photons information
    """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

class useJets(JobProperty):
    """ Store/use Jets information
    """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

class useTaus(JobProperty):
    """ Store/use Jets information
    """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

    

## Add properties
class VertexD3PDAnalysisFlags(JobPropertyContainer):
    """ VertexD3PDAnalysis D3PD Flags container
    """
    
jobproperties.add_Container(VertexD3PDAnalysisFlags)

#jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(doExampleFlag)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(D3PDMakerExtraDict)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useEventInfo)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useTracks)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useBeamspot)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useTrigger)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useTriggerRaw)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useBackgroundWord)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useSecondaryVertex)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useAllVertexCollections)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useTruth)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(filterTightTruth)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useMET)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useElectrons)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useMuons)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(usePhotons)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useJets)
jobproperties.VertexD3PDAnalysisFlags.add_JobProperty(useTaus)


## Create shortcut
VertexD3PDAnalysisFlags=jobproperties.VertexD3PDAnalysisFlags
