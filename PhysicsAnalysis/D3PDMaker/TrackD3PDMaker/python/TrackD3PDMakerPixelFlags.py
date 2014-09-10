# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# 
## @file TrackD3PDMaker/python/TrackD3PDMakerPixelFlags.py
## @purpose Python module to hold common flags to configure TrackD3PDMaker PixelNtuple JobOptions
##



##-----------------------------------------------------------------------------
## Import
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

# General flags

class inputFiles(JobProperty):
     statusOn = True
     allowTypes = ['list']
     StoredValue = ['ESD.pool.root']
 
class outputFile(JobProperty):
     statusOn = True
     allowTypes = ['str']
     StoredValue = 'PixelNtuple.root'


class doPixelNtuple(JobProperty):
    """ Turn on filling of Pixel Ntuples track and hit on track branches only 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doPixelNtupleLight(JobProperty):
    """ Turn on filling of Pixel Ntuples all branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doPixelNtupleExtended(JobProperty):
    """ Turn on filling of Pixel Ntuples branches including BCM rdo information 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class storeEventInfo(JobProperty):
    """ store general EventInformaton 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class storeBSerror(JobProperty):
    """ store general ByteStream error branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class storeTracks(JobProperty):
    """ store Combined Inner Detector 'Tracks' branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeHitsOnTracks(JobProperty):
    """ store HitOnTrack details for Combined Inner Detector 'Tracks' branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class storePixelClusters(JobProperty):
    """ store branches for all Pixel Clusters 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storePixelRDOs(JobProperty):
    """ store branches for all Pixel RDOs
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeTriggerInfo(JobProperty):
    """ store branches for trigger information
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storePhysicsInfo(JobProperty):
    """ store branches for MET etc. 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class storeBCMrdo(JobProperty):
    """ store branches for BCM RDOs 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False



class doTrackToClusterAssociation(JobProperty):
    """ store HitOnTrack details and do index association from tracks to clusters 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
class doClusterToTrackAssociation(JobProperty):
    """ perform index association from clusters to tracks 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doClusterToRdoAssociation(JobProperty):
    """ perform index association from clusters to rdos
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doRdoToClusterAssociation(JobProperty):
    """ store perform index association from rdos to clusters
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


##-----------------------------------------------------------------------------
## Definition of the TrackD3PDMaker flag container

class TrackD3PDMakerPixelFlags(JobPropertyContainer):
    """ The Track D3PD Pixel flag container
    """

        
# add the reconstruction flags container to the top container
jobproperties.add_Container(TrackD3PDMakerPixelFlags)

jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(inputFiles)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(outputFile)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(doPixelNtuple)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(doPixelNtupleLight)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(doPixelNtupleExtended)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(storeEventInfo)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(storeBSerror)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(storeTracks)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(storeHitsOnTracks)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(storePixelClusters)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(storePixelRDOs)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(storeTriggerInfo)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(storePhysicsInfo)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(storeBCMrdo)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(doTrackToClusterAssociation)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(doClusterToTrackAssociation)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(doClusterToRdoAssociation)
jobproperties.TrackD3PDMakerPixelFlags.add_JobProperty(doRdoToClusterAssociation)


TrackD3PDPixelFlags=jobproperties.TrackD3PDMakerPixelFlags
