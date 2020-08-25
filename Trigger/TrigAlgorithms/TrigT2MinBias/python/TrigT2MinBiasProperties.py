# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##
## @file Trigger/TrigAlgorithms/TrigT2MinBias/python/TrigT2MinBiasProperties.py
## @brief Python module to hold TrigHisto1D and TrigHisto2D histogram dimensions.  The dimensions are used to initialise the TrigT2MinBias algorithms and the associated monitoring.
##
## Adapted from AthenaCommonFlags.py
##
##

__author__ = "W. H. Bell"
__version__= "$Revision: 1.2 $"
__doc__    = "TrigT2MinBiasProperties"
 
__all__    = [ "trigT2MinBiasProperties" ]


##-----------------------------------------------------------------------------
## Import
 
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes

class pixelClusToTCut(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 20.

class pixelClusTotBins(JobProperty):
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 40

class pixelClusTotMin(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 0.5

class pixelClusTotMax(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 40.5

class pixelClusSizeBins(JobProperty):
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 1

class pixelClusSizeMin(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 1.5

class pixelClusSizeMax(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 2.5

class trtHitTotBins(JobProperty):
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 30

class trtHitTotMin(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 0.5

class trtHitTotMax(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 30.5
   
class mbtsThreshold(JobProperty): # Threshold in mV
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 0.18018 # this is C++ default also
    #StoredValue = 0.16 # this is desired value but the it was decided not tou touch it in 2010

class mbtsTimeCut(JobProperty): # Time in ns, +/- from L1A
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 12. 

class mbtsGlobalTimeOffset(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 0.

class zdcVetoThresholdLG(JobProperty): # Threshold for ZDC veto in Low Gain
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 1300. # default
 
##-----------------------------------------------------------------------------
## 2nd step
## Definition of the TrigT2MinBiasProperties container
class TrigT2MinBiasProperties(JobPropertyContainer):
    """Container for the common flags
    """
    pass


##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general min bias slice flags
##
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigT2MinBiasProperties.py' )

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

MinBiasSliceFlags.add_Container(TrigT2MinBiasProperties)
trigT2MinBiasProperties = MinBiasSliceFlags.TrigT2MinBiasProperties

##-----------------------------------------------------------------------------
## 4th step
## adding athena common flags to the trigT2MinBiasProperties container
trigT2MinBiasProperties.add_JobProperty(pixelClusToTCut)
trigT2MinBiasProperties.add_JobProperty(pixelClusTotBins)
trigT2MinBiasProperties.add_JobProperty(pixelClusTotMin)
trigT2MinBiasProperties.add_JobProperty(pixelClusTotMax)
trigT2MinBiasProperties.add_JobProperty(pixelClusSizeBins)
trigT2MinBiasProperties.add_JobProperty(pixelClusSizeMin)
trigT2MinBiasProperties.add_JobProperty(pixelClusSizeMax)
trigT2MinBiasProperties.add_JobProperty(trtHitTotBins)
trigT2MinBiasProperties.add_JobProperty(trtHitTotMin)
trigT2MinBiasProperties.add_JobProperty(trtHitTotMax)
trigT2MinBiasProperties.add_JobProperty(mbtsThreshold)
trigT2MinBiasProperties.add_JobProperty(mbtsTimeCut)
trigT2MinBiasProperties.add_JobProperty(mbtsGlobalTimeOffset)
trigT2MinBiasProperties.add_JobProperty(zdcVetoThresholdLG)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select AthenaCommonFlags as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from TrigT2MinBias.TrigT2MinBiasProperties import trigT2MinBiasProperties
#trigT2MinBiasProperties = jobproperties.TrigT2MinBiasProperties


# # defaults They should actually go to the "StoredValue" but I need to check that with Regina
# trigT2MinBiasProperties.pixelClusTotBins = 40
# trigT2MinBiasProperties.pixelClusTotMin = 0.5
# trigT2MinBiasProperties.pixelClusTotMax = 40.5
# trigT2MinBiasProperties.pixelClusSizeBins = 1

# trigT2MinBiasProperties.pixelClusSizeMin = 1.5
# trigT2MinBiasProperties.pixelClusSizeMax = 2.5
# trigT2MinBiasProperties.trtHitTotBins = 30
# trigT2MinBiasProperties.trtHitTotMin = 0.5

# trigT2MinBiasProperties.trtHitTotMax = 30.5
# trigT2MinBiasProperties.mbtsThreshold = 0.16 # ????



