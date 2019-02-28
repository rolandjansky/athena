#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createDQConfigFlags():
    acf=AthConfigFlags()
    acf.addFlag('DQ.doMonitoring', True)
    acf.addFlag('DQ.doGlobalMon', True)
    acf.addFlag('DQ.doStreamAwareMon', True)
    acf.addFlag('DQ.disableAtlasReadyFilter', False)
    acf.addFlag('DQ.FileKey', 'CombinedMonitoring')
    return acf

def createComplexDQConfigFlags():
    acf=AthConfigFlags()
    acf.addFlag('DQ.Environment', getEnvironment )
    acf.addFlag('DQ.DataType', getDataType )
    return acf

def getDataType(flags):
    if flags.Input.isMC:
        return 'monteCarlo'
    elif (False): # this is the HI test, needs HI flags
        return 'heavyioncollisions'
    elif flags.Beam.Type == 'cosmics':
        return 'cosmics'
    elif flags.Beam.Type == 'collisions':
        return 'collisions'
    elif flags.Beam.Type == 'singlebeam':
        # historically, singlebeam treated as collisions
        return 'collisions'
    else:
        import logging
        local_logger = logging.getLogger('DQConfigFlags_getDataType')
        local_logger.warning('Unable to figure out beam type for DQ; using "user"')
        return 'user'

def getEnvironment(flags):
    if flags.Common.isOnline:
        return 'online'
    else:
        # this could use being rethought to properly encode input and output types perhaps ...
        return 'tier0'
