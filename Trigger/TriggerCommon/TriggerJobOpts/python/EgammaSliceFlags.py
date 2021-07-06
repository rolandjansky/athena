# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Egamma slice specific flags  """

from AthenaCommon.Logging import logging
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

log = logging.getLogger( 'TriggerJobOpts.EgammaSliceFlags' )
_flags = []


class signatures(JobProperty):
    """ signatures in Egamma slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []

_flags.append(signatures)


class doRinger(JobProperty):
    '''apply ringer selection for Electron chains'''
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

_flags.append(doRinger) 

class ringerVersion (JobProperty):
    """ Version ringer tunes """
    statusOn=False
    allowedTypes=['str','None']
    StoreValues=None

_flags.append(ringerVersion)

class pidVersion (JobProperty):
    """ Version of PID tunes
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='ElectronPhotonSelectorTools/trigger/rel21_20170214/'

_flags.append(pidVersion)

class clusterCorrectionVersion (JobProperty):
    """
    Cluster correction version for HLT Calo
    """
    statusOn=True
    allowedTypes=['str']
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
class EgammaSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Egamma Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(EgammaSlice)


for flag in _flags:
    TriggerFlags.EgammaSlice.add_JobProperty(flag)
