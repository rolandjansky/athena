# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file TrackD3PDMaker/python/VertexD3PDAnalysisKeys.py
## @brief Python module to hold VertexD3PDAnalysis common Keys
## @author Simone Pagan Griso
## @date Mar, 2012
##



"""
This file is used to set different conventions for the VertexD3PDAnalysis objects:
block names and common labels
"""

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

## Define keys

## Example key
#class ExampleKey(JobProperty):
#    """ Comment
#    """
#    statusOn=True
#    allowedType=['str'] #['int','float',...]
#    StoredValue='ExampleValue'
#

## General keys for Vertex Group D3PD production

class D3PDFileName(JobProperty):
    """ Output D3PD file name
    """
    statusOn=True
    allowedType=['str'] 
    StoredValue='VertexGroupD3PD.root'

class D3PDTreeName(JobProperty):
    """ Name of D3PD maker algorithm
    """
    statusOn=True
    allowedType=['str'] 
    StoredValue='VtxTree'

class LabelDefaultVtx(JobProperty):
    """ Label/Prefix for dumping default collection on AOD/ESD
        Prefix is obtained as StoredValue+'_'
    """
    statusOn=True
    allowedType=['str'] 
    StoredValue='vxbc'

class TruthTrackCutPtMin(JobProperty):
    """ Cut of storing truth information on tracks (pT minimum)
    """
    statusOn=True
    allowedType=['float'] 
    StoredValue=350.

class TruthTrackCutEtaMax(JobProperty):
    """ Cut of storing truth information on tracks (|eta| maximum)
    """
    statusOn=True
    allowedType=['float'] 
    StoredValue=2.6

class TruthAddOnlyFirstVertex(JobProperty):
    """ Store only information of first vertex per event
    """
    statusOn=True
    allowedType=['bool'] 
    StoredValue=True

class MetCollections(JobProperty):
    """ Container name of Met info to use
    """
    statusOn=True
    allowedType=['list'] 
    StoredValue=['MET_RefFinal']
    
class MuonCollections(JobProperty):
    """ Container name of Muon info to use
    """
    statusOn=True
    allowedType=['list'] 
    StoredValue=['MuidMuonCollection']

class JetCollections(JobProperty):
    """ Container name of Jet collections to use
    """
    statusOn=True
    allowedType=['list']
    StoredValue=['AntiKt4TopoEMJets']
    


## Add properties
class VertexD3PDAnalysisKeys(JobPropertyContainer):
    """ VertexD3PDAnalysis D3PD Keys container
    """
    
jobproperties.add_Container(VertexD3PDAnalysisKeys)

#jobproperties.VertexD3PDAnalysisKeys.add_JobPorioerty(ExampleKey)
jobproperties.VertexD3PDAnalysisKeys.add_JobProperty(D3PDFileName)
jobproperties.VertexD3PDAnalysisKeys.add_JobProperty(D3PDTreeName)
jobproperties.VertexD3PDAnalysisKeys.add_JobProperty(LabelDefaultVtx)
jobproperties.VertexD3PDAnalysisKeys.add_JobProperty(TruthTrackCutPtMin)
jobproperties.VertexD3PDAnalysisKeys.add_JobProperty(TruthTrackCutEtaMax)
jobproperties.VertexD3PDAnalysisKeys.add_JobProperty(TruthAddOnlyFirstVertex)
jobproperties.VertexD3PDAnalysisKeys.add_JobProperty(MetCollections)
jobproperties.VertexD3PDAnalysisKeys.add_JobProperty(MuonCollections)
jobproperties.VertexD3PDAnalysisKeys.add_JobProperty(JetCollections)

## Shortcut
VertexD3PDAnalysisKeys=jobproperties.VertexD3PDAnalysisKeys
