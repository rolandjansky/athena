# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file DataQualityTools/python/DQMonFlags.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" LArMonFlags
    Python module to hold flags for Data Quality monitoring

"""

__author__ = "I. M. Nugent"
__version__= "$Revision: 1.11 $"
__doc__    = "LArMonFlags"

__all__    = [ "LArMonFlags" ]

# kindly stolen from InDetKeys by A. Wildauer

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes
list=[]

class LArDigitKey(JobProperty):
    """ Global switch for LArDigitKey """
    statusOn=True
    allowedTypes=['str']
    StoredValue='FREE'
list+=[LArDigitKey]

class EventBlockSize(JobProperty):
    """ Global switch for EventBlockSize """
    statusOn=True
    allowedTypes=['int']
    StoredValue=0
list+=[EventBlockSize]


class doLArRawChannelMonEtaPhi(JobProperty):
    """ Global switch for doLArRawChannelMonEtaPhi """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doLArRawChannelMonEtaPhi]


class doLArRawMonitorSignal(JobProperty):
    """ Global switch for monitor_signal """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doLArRawMonitorSignal]
                    

############################################################3
#
# define the flags for each tool
#

class doLArRawChannelMon(JobProperty):
    """ Global switch for doLArRawChannelMon """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArRawChannelMon]


class doLArCollisionTimeMon(JobProperty):
    """ Global switch for doLArCollisionTimeMon """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArCollisionTimeMon]
                                                            

class doLArAffectedRegions(JobProperty):
    """ Global switch for doLArAffectedRegions """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArAffectedRegions]
                    
class doLArFEBMon(JobProperty):
    """ Global switch for doLArFEBMon """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArFEBMon]

class doLArHVCorrectionMonTool(JobProperty):
    """ Global switch for doLArHVCorrectionMonTool """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArHVCorrectionMonTool]

class doLArCosmicsMonTool(JobProperty):
    """ Global switch for doLArCosmicsMonTool """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArCosmicsMonTool]

class doLArCoverage(JobProperty):
    """ Global switch for doLArCoverage """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArCoverage]
                    
class doLArDigitMon(JobProperty):
    """ Global switch for doLArDigitMon """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArDigitMon]

class doLArNoiseCorrelationMon(JobProperty):
    """ Global switch for doLArNoiseCorrelationMon """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArNoiseCorrelationMon]

class doLArNoisyROMon(JobProperty):
    """ Global switch for doLArNoisyROMon """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArNoisyROMon]
                    
class doLArRODMonTool(JobProperty):
    """ Global switch for doLArRODMonTool """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArRODMonTool]

class doLArNoiseCorrelationMon(JobProperty):
    """ Global switch for doLArNoiseCorrelationMon """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArNoiseCorrelationMon]

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the LArMon flag container
class LArMonFlagsCont(JobPropertyContainer):
    """Container for Data Quality Monitoring flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(LArMonFlagsCont)

##-----------------------------------------------------------------------------
## 4th step
## adding properties to LArMonFlags container
for j in list:
   jobproperties.LArMonFlagsCont.add_JobProperty(j)

##-----------------------------------------------------------------------------
## 5th step
## short-cut 
LArMonFlags = jobproperties.LArMonFlagsCont
