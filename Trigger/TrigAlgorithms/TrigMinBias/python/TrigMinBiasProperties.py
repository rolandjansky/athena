# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##
## @file Trigger/TrigAlgorithms/TrigMinBias/python/TrigMinBiasProperties.py
## @brief Python module to hold TrigHisto1D and TrigHisto2D histogram dimensions.  The dimensions are used to initialise the TrigMinBias algorithms and the associated monitoring.
##
## Adapted from AthenaCommonFlags.py
##
##

__author__ = "W. H. Bell"
__version__= "$Revision: 1.1 $"
__doc__    = "TrigMinBiasProperties"
 
__all__    = [ "trigMinBiasProperties" ]


##-----------------------------------------------------------------------------
## Import
 
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes

class trkZ0Bins(JobProperty):
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 20

class trkZ0Min(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 0.

class trkZ0Max(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 400.

class trkPtBins(JobProperty):
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 50

class trkPtMin(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 0.

class trkPtMax(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 10.

class trkEtaBins(JobProperty):
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 25

class trkEtaMin(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 0.

class trkEtaMax(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 2.5

class trkPhiBins(JobProperty):
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 16

class trkPhiMin(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 0.

class trkPhiMax(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 3.2
    
##-----------------------------------------------------------------------------
## 2nd step
## Definition of the TrigMinBiasProperties container
class TrigMinBiasProperties(JobPropertyContainer):
    """Container for the common flags
    """
    pass

trigMinBiasProperties = TrigMinBiasProperties()
##-----------------------------------------------------------------------------
## 3rd step

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigMinBiasProperties.py' )

try:
    from TriggerMenu import useNewTriggerMenu
    useNewTM = useNewTriggerMenu()
    log.info("Using new TriggerMenu: %r", useNewTM)
except Exception:
    useNewTM = False
    log.info("Using old TriggerMenuPython since TriggerMenu.useNewTriggerMenu can't be imported")

if useNewTM:
    from TriggerMenu.minbias.MinBiasSliceFlags import MinBiasSliceFlags
else:
    from TriggerMenuPython.MinBiasSliceFlags import MinBiasSliceFlags

MinBiasSliceFlags.add_Container(TrigMinBiasProperties)

trigMinBiasProperties = MinBiasSliceFlags.TrigMinBiasProperties

##-----------------------------------------------------------------------------
## 4th step
## adding athena common flags to the TrigMinBiasProperties container
trigMinBiasProperties.add_JobProperty(trkZ0Bins)
trigMinBiasProperties.add_JobProperty(trkZ0Min)
trigMinBiasProperties.add_JobProperty(trkZ0Max)
trigMinBiasProperties.add_JobProperty(trkPtBins)
trigMinBiasProperties.add_JobProperty(trkPtMin)
trigMinBiasProperties.add_JobProperty(trkPtMax)
trigMinBiasProperties.add_JobProperty(trkEtaBins)
trigMinBiasProperties.add_JobProperty(trkEtaMin)
trigMinBiasProperties.add_JobProperty(trkEtaMax)
trigMinBiasProperties.add_JobProperty(trkPhiBins)
trigMinBiasProperties.add_JobProperty(trkPhiMin)
trigMinBiasProperties.add_JobProperty(trkPhiMax)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select AthenaCommonFlags as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from TrigMinBias.TrigMinBiasProperties import trigMinBiasProperties
#trigMinBiasProperties = trigMinBiasProperties



# trigMinBiasProperties.trkZ0Bins = 20
# trigMinBiasProperties.trkZ0Min = 0.
# trigMinBiasProperties.trkZ0Max = 400.
# trigMinBiasProperties.trkPtBins = 20
# trigMinBiasProperties.trkPtMin = 0.
# trigMinBiasProperties.trkPtMax = 2.0
# trigMinBiasProperties.trkEtaBins = 10
# trigMinBiasProperties.trkEtaMin = 0.
# trigMinBiasProperties.trkEtaMax = 2.5
# trigMinBiasProperties.trkPhiBins = 16
# trigMinBiasProperties.trkPhiMin = 0.
# trigMinBiasProperties.trkPhiMax = 3.2
