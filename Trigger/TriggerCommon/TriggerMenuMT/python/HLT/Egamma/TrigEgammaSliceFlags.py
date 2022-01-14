# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

""" Egamma slice specific flags  """

from AthenaCommon.Logging import logging
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer

__author__  = 'Joao Victor Pinto'
__version__="$$"
__doc__="Egamma slice specific flags  "

log = logging.getLogger( __name__ )
_flags = [] 


class ringerVersion(JobProperty):
    """  """ 
    statusOn=True
    allowedTypes=['str']
    StoredValue='TrigL2_20210702_r4'

_flags.append(ringerVersion)

class pidVersion(JobProperty):
    """  """ 
    statusOn=True
    allowedTypes=['str']
    StoredValue='rel22_20210611'

_flags.append(pidVersion)

class dnnVersion(JobProperty):
    """  """ 
    statusOn=True
    allowedTypes=['str']
    StoredValue='rel21_20210928'

_flags.append(dnnVersion)

class calibMVAVersion(JobProperty):
    """  """ 
    statusOn=True
    allowedTypes=['str']
    StoredValue='egammaMVACalib/online/v6'

_flags.append(calibMVAVersion)



# create container
class TrigEgammaSlice(JobPropertyContainer):
    """ Egamma Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(TrigEgammaSlice)


for flag in _flags:
    TriggerFlags.TrigEgammaSlice.add_JobProperty(flag)

# make an alias
TrigEgammaSliceFlags = TriggerFlags.TrigEgammaSlice

def run3ConfigAction(run3Flag):
    if run3Flag=='2021':
        log.info('TrigEgammaSliceFlags set for %s',run3Flag)
        TrigEgammaSliceFlags.ringerVersion      = 'TrigL2_20210702_r4'
        TrigEgammaSliceFlags.dnnVersion         = 'rel21_20210928'
        TrigEgammaSliceFlags.pidVersion         = 'rel22_20210611'
        TrigEgammaSliceFlags.calibMVAVersion    = 'egammaMVACalib/online/v6'
    else:
        log.info('TrigEgammaSliceFlags not set use defaults',run3Flag)


run3ConfigAction('2021')
