# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Egamma slice specific flags  """

from AthenaCommon.Logging import logging
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

__author__  = 'T. Bold, P.Urquijo, R. White'
__version__="$Revision: 1.42 $"
__doc__="Egamma slice specific flags  "

log = logging.getLogger( 'TriggerMenu.EgammaSliceFlags' )
_flags = [] 
class doSiTrack(JobProperty):
    """ do or not to do SiTrack algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doSiTrack)

class doIDSCAN(JobProperty):
    """ do or not to do IDSCAN algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doIDSCAN)


class doTrigEgammaRec(JobProperty):
    """ do or not to do TrigEgammaRec (EF algorithm) algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doTrigEgammaRec)

class signatures(JobProperty):
    """ signatures in Egamma slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []

_flags.append(signatures)

class doFastElectronFex(JobProperty):
    '''enable L2ElectronFex cuts and sequence'''
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doFastElectronFex)

class doRinger(JobProperty):
    '''apply ringer selection for Electron chains'''
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

_flags.append(doRinger) 

class ringerVersion (JobProperty):
    """ Version ringer tunes """
    statusOn=True
    allowedTypes=['str']
    StoredValue='RingerSelectorTools/TrigL2_20170505_v6'

_flags.append(ringerVersion)

class pidVersion (JobProperty):
    """ Version of PID tunes
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='ElectronPhotonSelectorTools/trigger/rel21_20170217/'

_flags.append(pidVersion)

class clusterCorrectionVersion (JobProperty):
    """
    Cluster correction version for HLT Calo
    """
    statusOn=True
    allowedTypes=['str','None']
    allowedValues=['v12phiflip_noecorrnogap','None']
    StoredValue='v12phiflip_noecorrnogap'

_flags.append(clusterCorrectionVersion)

class calibMVAVersion (JobProperty):
    """
    MVA calibration version
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='egammaMVACalib/online/v6'

_flags.append(calibMVAVersion)

# create container
class EgammaSlice(JobPropertyContainer, CommonSliceHelper):
    """ Egamma Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(EgammaSlice)

# add common slice flags
TriggerFlags.EgammaSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.EgammaSlice.add_JobProperty(flag)
del _flags

# make an alias
EgammaSliceFlags = TriggerFlags.EgammaSlice

def run2ConfigAction(run2Flag):
    if run2Flag=='2016':
        log.info('EgammaSliceFlags set for %s',run2Flag)
        EgammaSliceFlags.ringerVersion = 'RingerSelectorTools/TrigL2_20170505_v6'
        EgammaSliceFlags.pidVersion = 'ElectronPhotonSelectorTools/trigger/rel21_20170217_mc16a/'
        EgammaSliceFlags.clusterCorrectionVersion = 'v12phiflip_noecorrnogap'
        EgammaSliceFlags.calibMVAVersion = 'egammaMVACalib/online/v6'
    elif run2Flag=='2017':
        log.info('EgammaSliceFlags set for %s',run2Flag)
        EgammaSliceFlags.ringerVersion = 'RingerSelectorTools/TrigL2_20170505_v6'
        EgammaSliceFlags.pidVersion = 'ElectronPhotonSelectorTools/trigger/rel21_20170217/'
        EgammaSliceFlags.clusterCorrectionVersion = 'v12phiflip_noecorrnogap'
        EgammaSliceFlags.calibMVAVersion = 'egammaMVACalib/online/v6'
    else:
        log.info('EgammaSliceFlags not set use defaults',run2Flag)

