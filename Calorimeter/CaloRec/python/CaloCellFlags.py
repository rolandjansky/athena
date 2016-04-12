# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: CaloCellFlags.py
# @purpose: a container of flags for CaloCell making configuring
# @author: David Rousseau <rousseau@lal.in2p3.fr>

"""  A container of flags for CaloCell making configuring

"""
#
#
__author__  = 'David Rousseau'
__version__ = "$Revision: 1.11 $"
__doc__     = "A container of flags for CaloCell making configuring"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#
class doLArCellEmMisCalib(JobProperty):
    """ Flag to activate LAr cell random miscalibration (to be applied only on MC)
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
                           
class doFastCaloSim(JobProperty):
    """ Flag to activate the fast calorimeter simulation instead of geant4 calo simulation
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
                           
class doFastCaloSimNoise(JobProperty):
    """ Flag to activate the fast calorimeter electronic noise simulation ("digitization")
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False

class doFastCaloSimAddCells(JobProperty):
    """ Flag to run FastCaloSim on top of a full simulated calorimeter. Used if muons are done with full simulation
    """
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False

class doLArThinnedDigits(JobProperty):
    """ Flag to activate creation of thinned digits (cells above threshold)
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doLArHVCorr(JobProperty):
   """ Flag to activate  HV correction at LAr cell level from DCS cool database undoing the online applied factor
   """  
   statusOn     = True
   allowedTypes = ['bool']
   StoredValue  = True

class doPedestalCorr(JobProperty):
   """ Flag to activate cell level pedestal shift correction
   """  
   statusOn     = True
   allowedTypes = ['bool']
   StoredValue  = True

class doMinBiasAverage(JobProperty):
   """ Flag to activate cell level energy shift correction computed on the fly for MC pileup reco
   """  
   statusOn     = True
   allowedTypes = ['bool']
   StoredValue  = False

class doLArRecalibration(JobProperty):
   """ Flag to activate cell recalibration, undoing online energy computation and applying new calibration
   """
   statusOn = True
   allowedTypes = ['bool']
   StoredValue = False

class doLArNoiseMasking(JobProperty):
   """ Flag to activate masking of high noise / dead  cell for all events
   """
   statusOn     = True
   allowedTypes = ['bool']
   StoredValue   = True

class doLArSporadicMasking(JobProperty):
   """ Flag to activate masking of sporadic noisy cell on event by event basis based on quality
   """
   statusOn     = True
   allowedTypes = ['bool']
   StoredValue   = True

class doLArBadFebMasking(JobProperty):
   """ Flag to activate masking of bad Febs
   """
   statusOn     = True
   allowedTypes = ['bool']
   StoredValue  = True

class doDeadCellCorr(JobProperty):
   """ Flag to activate correction for dead/masked cells based on the neighbors
   """
   statusOn      = True
   allowedTypes  = ['bool']
   StoredValue   = True

class doLArRawChannelMerge(JobProperty):
   """ Flag to activate mergeing of cell container with improved raw channel container containing less cells
       (use case: main container from DSP energies, improved container from sparse digits written by DSP reconstructed offline     
       default is true but in fact will only be activated if reco is configured to have the 2 raw channel containers
   """  
   statusOn      = True
   allowedType   = ['bool']
   StoredValue   = True

class doLArCreateMissingCells(JobProperty):
   """ Flag to activate creation of cells missing in the readout in the calo cell container
   """
   statusOn      = True
   allowedType   = ['bool']
   StoredValue   = True

class doLArDeadOTXCorr(JobProperty):
   """ Flag to activate correction for cells in Feb with deadOTX based on L1 readout
   """
   statusOn      = True
   allowedType   = ['bool']
   StoredValue   = True

class doLArCellGainPathology(JobProperty):
   """ Flag to activate emulation of gain recording pathology affecting some 7 samples 2010 data
   """
   statusOn      = True
   allowedType   = ['bool']
   StoredValue   = False

class doPileupOffsetBCIDCorr(JobProperty):
   """ Flag to activate BCID per BCID average pileup energy offset correction
   """
   statusOn     = True
   allowedType  = ['bool']
   StoredValue  = True


class doCaloCellTimeCorr(JobProperty):
    """switch CaloCell Time correction on/off."""
    statusOn = True
    allowedTypes=['bool']
    StoredValue=True

class doCaloCellEnergyCorr(JobProperty):
    """switch CaloCell Energy correction on/off."""
    statusOn = True
    allowedTypes=['bool']
    StoredValue=True




# Defines the container for the cell making flags
class CaloCellFlags(JobPropertyContainer):
    """ The CaloCell making flag property container
    """
    pass



# add the perfmon flags container to the top container 
jobproperties.add_Container(CaloCellFlags)


# We want always the following flags in the container  
list_jobproperties = [
    doLArCellEmMisCalib,
    doFastCaloSim,
    doFastCaloSimNoise,
    doFastCaloSimAddCells,
    doLArThinnedDigits,
    doLArHVCorr,
    doPedestalCorr,
    doMinBiasAverage,
    doLArRecalibration,
    doLArNoiseMasking,
    doLArSporadicMasking,
    doLArBadFebMasking,
    doDeadCellCorr,
    doLArRawChannelMerge,
    doLArDeadOTXCorr,
    doLArCreateMissingCells,
    doLArCellGainPathology,
    doPileupOffsetBCIDCorr,
    doCaloCellTimeCorr,
    doCaloCellEnergyCorr
    ]

for i in list_jobproperties:
    jobproperties.CaloCellFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties
