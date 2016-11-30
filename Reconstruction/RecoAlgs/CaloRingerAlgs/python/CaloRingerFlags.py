# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:  CaloRingerAlgs/python/CaloRingerFlags.py
#=======================================================================
__author__  = 'W. S. Freund'
__version__="$Revision: 785784 $"
__doc__="CaloRinger flags."


#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#=======================================================================
# Defines a sub-container for the algorithm switches
class CaloRingerFlags(JobPropertyContainer):
    """ CaloRinger information """
# add the CaloRingerFlags flags container to the top container
jobproperties.add_Container(CaloRingerFlags)

#=======================================================================
class Enabled(JobProperty):
    """ master switch for CaloRinger Algorithm
    """
    statusOn        = True
    allowedTypes    = ['bool']
    StoredValue     = True
jobproperties.CaloRingerFlags.add_JobProperty(Enabled)

#=======================================================================
class CaloRingerFlagsJobProperty(JobProperty):
    """ special flag classing with job properties.CaloRingerFlags.Enabled
    """
    StoredValue     = True
    allowedTypes    = ['bool']
    def get_Value(self):
      if self.statusOn and self.StoredValue and jobproperties.CaloRingerFlags.Enabled():
        return self.StoredValue
      else:
        return False
jobproperties.CaloRingerFlags.add_JobProperty(CaloRingerFlagsJobProperty)

from AthenaCommon.Logging import logging
class OutputLevel(CaloRingerFlagsJobProperty):
    """ switch for all CaloRinger algorithms and tools
    """
    statusOn        = True
    allowedTypes    = ['int']
    StoredValue     = logging.INFO
jobproperties.CaloRingerFlags.add_JobProperty(OutputLevel)

#=======================================================================
class buildElectronCaloRings(CaloRingerFlagsJobProperty):
    """ switch for building the CaloRings for electron candidates
    """
    statusOn        = True
    allowedTypes    = ['bool']
    StoredValue     = True
jobproperties.CaloRingerFlags.add_JobProperty(buildElectronCaloRings)

#=======================================================================
class minElectronEnergy(CaloRingerFlagsJobProperty):
    """ Minimum transverse energy (GeV) needed to build CaloRings for electrons
    """
    statusOn        = True
    allowedTypes    = ['float', 'NoneType']
    StoredValue     = 14
jobproperties.CaloRingerFlags.add_JobProperty(minElectronEnergy)

#=======================================================================
class doElectronIdentification(CaloRingerFlagsJobProperty):
    """ switch for electron particle identification (PID), that is, whether to
    run or not Ringer selector for Electrons.
    """
    statusOn        = True
    allowedTypes    = ['bool']
    StoredValue     = False
jobproperties.CaloRingerFlags.add_JobProperty(doElectronIdentification)

#=======================================================================
class buildPhotonCaloRings(CaloRingerFlagsJobProperty):
    """ switch for building the CaloRings for electron candidates
    """
    statusOn       = True 
    allowedTypes   = ['bool']
    StoredValue    = False
jobproperties.CaloRingerFlags.add_JobProperty(buildPhotonCaloRings)

#=======================================================================
class useAsymBuilder(CaloRingerFlagsJobProperty):
    """ switch for building the AsymRings 
    """
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
jobproperties.CaloRingerFlags.add_JobProperty(useAsymBuilder)

#=======================================================================
class doPhotonIdentification(CaloRingerFlagsJobProperty):
    """ switch for photon particle identification (PID), that is, whether to
    run or not Ringer selector for Photons.
    """
    statusOn      = False # TODO Change to true when implemented 
    allowedTypes  = ['bool']
    StoredValue   = True
jobproperties.CaloRingerFlags.add_JobProperty(doPhotonIdentification)

#=======================================================================
class minPhotonEnergy(CaloRingerFlagsJobProperty):
    """ Minimum energy needed to build CaloRings for photons
    """
    statusOn        = True
    allowedTypes    = ['float', 'NoneType']
    StoredValue     = None
jobproperties.CaloRingerFlags.add_JobProperty(minPhotonEnergy)

#=======================================================================
from RingerSelectorTools.ElectronRingerSelectorMapping import electronRingerPIDmenu
class electronMenuToUse(CaloRingerFlagsJobProperty):
    """ which menu should be used for electron identification  """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = electronRingerPIDmenu.testMenu
jobproperties.CaloRingerFlags.add_JobProperty(electronMenuToUse)

#=======================================================================
class ignoreRingerExistingDataObject(CaloRingerFlagsJobProperty):
    """ switch to allow overwriting Ringer existing data objects.  """
    statusOn     = True
    allowedType  = ['bool']
    StoredValue  = True
jobproperties.CaloRingerFlags.add_JobProperty(ignoreRingerExistingDataObject)

#=======================================================================
class buildCaloRingsOn(CaloRingerFlagsJobProperty):
    """ special flag to obtain whether it will run CaloRings Reconstruction
    """

    def get_Value(self):
        return jobproperties.CaloRingerFlags.buildElectronCaloRings() or \
            jobproperties.CaloRingerFlags.buildPhotonCaloRings()

    def set_Value(self, val):
        jobproperties.CaloRingerFlags.buildElectronCaloRings = val
        jobproperties.CaloRingerFlags.buildPhotonCaloRings = val
jobproperties.CaloRingerFlags.add_JobProperty(buildCaloRingsOn)

#=======================================================================
class doIdentificationOn(CaloRingerFlagsJobProperty):
    """ special flag to obtain whether it will run particle identification
    """
    def get_Value(self):
        return jobproperties.CaloRingerFlags.doElectronIdentification() or \
            jobproperties.CaloRingerFlags.doPhotonIdentification()
    def set_Value(self, val):
        jobproperties.CaloRingerFlags.doElectronIdentification = val
        jobproperties.CaloRingerFlags.doPhotonIdentification = val
jobproperties.CaloRingerFlags.add_JobProperty(doIdentificationOn)


#=======================================================================
class doWriteRingsToFile(CaloRingerFlagsJobProperty):
    """ Determine if rings container will be written to file """
    statusOn     = True
    allowedType  = ['bool']
    StoredValue  = True
jobproperties.CaloRingerFlags.add_JobProperty(doWriteRingsToFile)

caloRingerFlags = jobproperties.CaloRingerFlags
