# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file MuonDQAMonFlags/python/MuonDQAProperFlags.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" MuonDQAProperFlags
    Python module to hold flags for Muon Data Quality monitoring

"""

__author__ = "Nektarios Chr. Benekos"
__version__= "$Revision: 1.3 $"
__doc__    = "MuonDQAProperFlags"
__all__    = [ "MuonDQAProperFlags" ]

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes
list=[]
class doMuonMonitoring(JobProperty):
    """ Global switch for MuonMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMuonMonitoring]

class doMuonEvtMon(JobProperty):
    """ Global switch for MuonEventMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMuonEvtMon]

class doMuonHitMon(JobProperty):
    """ Global switch for MuonHitLevelMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMuonHitMon]

# --- sudetector specific flags 
class doCSCMon(JobProperty):
    """ Global switch for MuonCscMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doCSCMon]

class doCSCClusMon(JobProperty):
    """ Global switch for MuonCscClusterMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doCSCClusMon]

class doMDTMon(JobProperty):
    """ Global switch for MuonMdtMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMDTMon]

class doRPCMon(JobProperty):
    """ Global switch for MuonRpcMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doRPCMon]

class doRPCL1Mon(JobProperty):
    """ Global switch for MuonRpcLv1Monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doRPCL1Mon]

class doMDTRPCMon(JobProperty):
    """ Global switch for MuonMdtVsRpcMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMDTRPCMon]

class doTGCMon(JobProperty):
    """ Global switch for MuonTgcMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doTGCMon]

class doTGCL1Mon(JobProperty):
    """ Global switch for MuonTgcLv1Monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doTGCL1Mon]

class doMDTTGCL1Mon(JobProperty):
    """ Global switch for MuonMdtVsTgcMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMDTTGCL1Mon]
 
## Muon Segment Level Monitoring 
class doMuonSegmMon(JobProperty):
    """ Global switch for MuonSegmentMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doMuonSegmMon]

class doCSCSegmMon(JobProperty):
    """ Global switch for MuonCscSegmentMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doCSCSegmMon]

## Muon Trk Level Monitoring 
class doMuonTrackMon(JobProperty):
    """ Global switch for MuonTrkMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doMuonTrackMon]

#############TriggerAware Trk Monitoring#######################
## set to true in case you would like to use trigger-aware monitoring
class MuonTrkMonDoTrigger(JobProperty):
    """ Global switch for MuonTrkTriggerAwareMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[MuonTrkMonDoTrigger]

## Muon Trk Level Monitoring 
class doMuonCombTrackMon(JobProperty):
    """ Global switch for MuonCombinedTrkMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doMuonCombTrackMon]


## Muon Trk Level Alignment Monitoring 
class doMuonTrkAlignMon(JobProperty):
    """ Global switch for MuonTrk Alignment Monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doMuonTrkAlignMon]

## Muon Physics Level Monitoring 
class doMuonPhysMon(JobProperty):
    """ Global switch for MuonPhysicsMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doMuonPhysMon]

## High Level Muon Monitoring 
class doMuonTrkPhysMon(JobProperty):
    """ Global switch for MuonMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMuonTrkPhysMon]

## Muon CbTrk Level Alignment Monitoring 
class doMuonCbTrkAlignMon(JobProperty):
    """ Global switch for MuonCbTrk Alignment Monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doMuonCbTrkAlignMon]

## Zmumu Level Monitoring 
class doZmumuMon(JobProperty):
    """ Global switch for MuonPhysicsMonitoring/Zmumu Monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doZmumuMon]

class doMuTrkMon(JobProperty):
    """ Global switch for MuonPhysicsMonitoring/MuTrk Monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doMuTrkMon]
    
## JpsimumuLevel Monitoring 
class doJpsimumuMon(JobProperty):
    """ Global switch for MuonPhysicsMonitoring/JpsimumuMonitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doJpsimumuMon]

 
##-----------------------------------------------------------------------------
## 2nd step
## Definition of the MuonDQAProperFlags container
class MuonDQAFlagsCont(JobPropertyContainer):
    """Container for Muon Data Quality Monitoring flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(MuonDQAFlagsCont)

##-----------------------------------------------------------------------------
## 4th step
## adding properties to DQMonFlags container
for j in list:
    jobproperties.MuonDQAFlagsCont.add_JobProperty(j)

##-----------------------------------------------------------------------------
## 5th step
## short-cut 
MuonDQADetFlags = jobproperties.MuonDQAFlagsCont
#MuonDQAProperFlags = jobproperties.MuonDQAFlagsCont
