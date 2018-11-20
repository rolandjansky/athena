# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# 
## @file InDetD3PDMaker/python/InDetD3PDMakerSCTFlags.py
## @brief Python module to hold common flags to configure InDetD3PDMaker SCTNtuple JobOptions
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
     StoredValue = 'SctNtuple.root'


class doSCTNtuple(JobProperty):
    """ Turn on filling of SCT Ntuples track and hit on track branches only 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doSCTNtupleLight(JobProperty):
    """ Turn on filling of SCT Ntuples all branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doSCTNtupleExtended(JobProperty):
    """ Turn on filling of SCT Ntuples branches including BCM rdo information 
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
    StoredValue=False ##=# True

class storeHitsOnTracks(JobProperty):
    """ store HitOnTrack details for Combined Inner Detector 'Tracks' branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False ##=#True


class storeSctClusters(JobProperty):
    """ store branches for all SCT Clusters 
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeSctRDOs(JobProperty):
    """ store branches for all SCT RDOs
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeTriggerInfo(JobProperty):
    """ store branches for trigger information
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False ##=# True

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

class includeTruthInfo(JobProperty):
     """ store branches MC truth info
     """
     statusOn=True
     allowedTypes=['bool']
     StoredValue=False
                     
class doTrackToMuonAssociation(JobProperty):
     """ store Muon Information branches
     """
     statusOn=True
     allowedTypes=['bool']
     StoredValue=False ##=# True

class useStacoMuonCollectionForAssociation(JobProperty):
     """ use 'StacoMuonCollection' instead of default 'Muons' in SCTTrackParticleToMuonAssociationTool
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
## Definition of the InDetD3PDMaker flag container

class InDetD3PDMakerSCTFlags(JobPropertyContainer):
    """ The InDet D3PD SCT flag container
    """

        
# add the reconstruction flags container to the top container
jobproperties.add_Container(InDetD3PDMakerSCTFlags)

jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(inputFiles)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(outputFile)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(doSCTNtuple)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(doSCTNtupleLight)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(doSCTNtupleExtended)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(storeEventInfo)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(storeBSerror)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(storeTracks)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(storeHitsOnTracks)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(storeSctClusters)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(storeSctRDOs)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(storeTriggerInfo)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(storePhysicsInfo)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(storeBCMrdo)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(doTrackToClusterAssociation)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(doClusterToTrackAssociation)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(doClusterToRdoAssociation)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(doRdoToClusterAssociation)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(doTrackToMuonAssociation)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(useStacoMuonCollectionForAssociation)
jobproperties.InDetD3PDMakerSCTFlags.add_JobProperty(includeTruthInfo)

InDetD3PDSCTFlags=jobproperties.InDetD3PDMakerSCTFlags
