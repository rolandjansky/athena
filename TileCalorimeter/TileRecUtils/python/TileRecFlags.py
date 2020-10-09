# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: TileRecFlags.py
# @purpose: a container of flags for TileCal Reconstruction
# @author: Ximo Poveda <Ximo.Poveda@cern.ch>

"""  A container of flags for TileCal Reconstruction  

"""
#
#
__author__  = 'Ximo Poveda'
__version__ = "$Revision: 1.10 $"
__doc__     = "A container of flags for TileCal Reconstruction"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#
class doTileFlat(JobProperty):
    """ Use Flat filtering for energy reconstruction
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
#
class doTileFit(JobProperty):
    """ Use Fit method for energy reconstruction
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
#
class doTileFitCool(JobProperty):
    """ Use FitCool method for energy reconstruction
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
#
class doTileOpt2(JobProperty):
    """ Use Optimal Filtering (version 2) for energy reconstruction
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

#
class doTileQIE(JobProperty):
    """ Use QIE algorith for energy reconstruction
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

#
class doTileOF1(JobProperty):
    """ Use Optimal Filtering (version 1) for energy reconstruction, using pedestals from the conditions DB
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

#
class doTileOptATLAS(JobProperty):
    """ Use Optimal Filtering (no iterations) for energy reconstruction
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

#
class doTileManyAmps(JobProperty):
    """ Use ManyAmps fit method for energy reconstruction
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

#
class doTileMF(JobProperty):
    """ Use Matched filter method for energy reconstruction
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

#
class doTileWiener(JobProperty):
    """ Use Wiener filter method for energy reconstruction
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

#
class noiseFilter(JobProperty):
    """ appply one or another noise suppression algorithm at the level of raw channels
    """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = -1

#
class TileRawChannelContainer(JobProperty):
    """ name of the container which will be produced by TileRawChannelMaker
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TileRawChannelCnt'

#
class TileDigitsContainer(JobProperty):
    """ name of the container which will be used by TileRawChannelMaker
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TileDigitsCnt'


#
class TileRunType(JobProperty):
    """ declare type of all events in a run (0=unknown, 1=physics, 2=laser, 4=ped, 8=cis)
    """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0

#
class calibrateEnergy(JobProperty):
    """ convert reconstructed raw channel amplitude to pCb
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

#
class correctTime(JobProperty):
    """ apply time correction using info from DB (for physics and unknown trigger types)
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

#
class correctTimeNI(JobProperty):
    """ apply time correction in Optimal filter without iterations
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

#
class correctAmplitude(JobProperty):
    """ apply parabolic amplitude correction in Optimal filter without iterations
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

#
class AmpMinForAmpCorrection(JobProperty):
    """ apply parabolic amplitude correction if amplitude is above threshold (in ADC counts)
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 15.

#
class TimeMinForAmpCorrection(JobProperty):
    """ apply parabolic amplitude correction if time is above min threshold
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1000.0

#
class TimeMaxForAmpCorrection(JobProperty):
    """ apply parabolic amplitude correction if time is below max threshold
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = -999.0

#
class OfcFromCOOL(JobProperty):
    """ read OFCs from COOL - no on the fly calculation
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

#
class BestPhaseFromCOOL(JobProperty):
    """ read best phase from COOL for OF without iterations
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

#
class readDigits(JobProperty):
    """ set to True if digits are available in input file for Optimal Filter
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

#
class doTileOverflowFit(JobProperty):
    """ set to True if apply fit builder for raw channels with overflow
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

#
class simulateTrips(JobProperty):
    """ set to True if drawer trips simulation has to be done
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    

#
class zeroAmplitudeWithoutDigits(JobProperty):
    """ zero amplitude in Tile raw channel container from DSP reconstructed with OF1 method
        if amplitude is above threshold but there are no corresponding digits 
        available in Tile digits container
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

#
class correctPedestalDifference(JobProperty):
    """ correct amplitude in Tile raw channel container from DSP reconstructed with OF1 method
        if there is online pedestal difference 
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True    

#
class correctTimeJumps(JobProperty):
    """ appply time correction for channels with possible +/-25 ns time jumps
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True


# Defines the container for the performance monitoring flags  
class TileRecFlags(JobPropertyContainer):
    """ The global Tile reconstruction flag/job property container.
    """
    pass


# add the perfmon flags container to the top container 
jobproperties.add_Container(TileRecFlags)


# We want always the following flags in the container  
list_jobproperties = [
    doTileFlat,
    doTileFit,
    doTileFitCool,
    doTileOF1,
    doTileOpt2,
    doTileQIE,
    doTileManyAmps,
    doTileMF,
    doTileOptATLAS,
    doTileWiener,
    TileRawChannelContainer,
    TileDigitsContainer,
    TileRunType,
    noiseFilter,
    calibrateEnergy,
    correctTime,
    correctTimeNI,
    correctAmplitude,
    AmpMinForAmpCorrection,
    TimeMinForAmpCorrection,
    TimeMaxForAmpCorrection,
    OfcFromCOOL,
    BestPhaseFromCOOL,
    readDigits,
    doTileOverflowFit,
    simulateTrips,
    zeroAmplitudeWithoutDigits,
    correctPedestalDifference,
    correctTimeJumps
    ]

for i in list_jobproperties:
    jobproperties.TileRecFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties
