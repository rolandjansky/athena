#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

_steeringFlags = [ 'doGlobalMon', 'LVL1CaloMon', 'doCTPMon', 'doHLTMon',
                   'doPixelMon', 'doSCTMon', 'doTRTMon', 'doInDetMon',
                   'doLArMon', 'doTileMon',
                   'doCaloGlobalMon', 'doMuonMon',
                   'doLucidMon', 'doAFPMon',
                   'doHIMon', 'doEgammaMon', 'doJetMon', 'doMissingEtMon',
                   'doTauMon', 'doJetTagMon' ]

_lowLevelSteeringFlags = [ 'InDet.doGlobalMon', 'InDet.doAlignMon',
                           'InDet.doPerfMon',  'Muon.doRawMon',
                           'Muon.doTrackMon', 'Muon.doAlignMon',
                           'Muon.doSegmentMon',
                           'Muon.doPhysicsMon', 'Muon.doTrkPhysMon',
                           'Muon.doCombinedMon'
                           ]

def createDQConfigFlags():
    acf=AthConfigFlags()
    acf.addFlag('DQ.doMonitoring', True)
    acf.addFlag('DQ.doStreamAwareMon', True)
    acf.addFlag('DQ.disableAtlasReadyFilter', False)
    acf.addFlag('DQ.FileKey', 'CombinedMonitoring')
    acf.addFlag('DQ.useTrigger', True)

    # temp thing for steering from inside old-style ...
    acf.addFlag('DQ.isReallyOldStyle', False)

    # steering ...
    for flag in _steeringFlags + _lowLevelSteeringFlags:
        acf.addFlag('DQ.Steering.' + flag, True)
    # HLT steering ...
    from TrigHLTMonitoring.TrigHLTMonitoringConfig import createHLTDQConfigFlags
    acf.join(createHLTDQConfigFlags())
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

def allSteeringFlagsOff():
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    for flag in _steeringFlags:
        setattr(getattr(ConfigFlags, 'DQ.Steering'), flag, False)
        #print flag
        #getattr(ConfigFlags, 'DQ.Steering.' + flag).set(False)
