#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigHLTMonitorAlgorithm.py
@date 2019-09-10
@date 2020-09-18
@date 2022-02-21
@brief TrigHLTMonitoring top-level files
'''

def createHLTDQConfigFlags():
    from AthenaConfiguration.AthConfigFlags import AthConfigFlags
    acf=AthConfigFlags()

    # need to (temporarily) block signature monitoring by default when it is
    # running on bytestream. Remove when ATR-23720 is completed
    from AthenaConfiguration.Enums import Format
    from AthenaConfiguration.Enums import BeamType
    acf.addFlag('DQ.Steering.HLT.doGeneral', True)

    # b-jets disabled for cosmics following ATR-25036
    acf.addFlag('DQ.Steering.HLT.doBjet', lambda flags: flags.Input.Format is Format.POOL and flags.Beam.Type is BeamType.Collisions)

    acf.addFlag('DQ.Steering.HLT.doBphys', lambda flags: flags.Input.Format is Format.POOL)
    acf.addFlag('DQ.Steering.HLT.doCalo', lambda flags: flags.Input.Format is Format.POOL)
    acf.addFlag('DQ.Steering.HLT.doEgamma', lambda flags: flags.Input.Format is Format.POOL)
    acf.addFlag('DQ.Steering.HLT.doJet', lambda flags: flags.Input.Format is Format.POOL)
    acf.addFlag('DQ.Steering.HLT.doMET', lambda flags: flags.Input.Format is Format.POOL)
    acf.addFlag('DQ.Steering.HLT.doMinBias', lambda flags: flags.Input.Format is Format.POOL)
    acf.addFlag('DQ.Steering.HLT.doMuon', True) #must be changed back to lambda flags if monGroups are implemented. Safe if ATR-23720 is completed
    acf.addFlag('DQ.Steering.HLT.doTau', lambda flags: flags.Input.Format is Format.POOL)

    return acf


def TrigHLTMonTopConfig(inputFlags):
    '''Configuring the HLT signatures top-level steering in the DQ monitoring system.'''

    ########
    #HLT top-level steering

    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # do not run in RAWtoESD, if we have two-step reco
    if inputFlags.DQ.Environment in ('online', 'tier0', 'tier0ESD', 'AOD'):
        if inputFlags.DQ.Steering.HLT.doGeneral:
            from TrigHLTMonitoring.TrigGeneralMonitorAlgorithm import TrigGeneralMonConfig
            result.merge(TrigGeneralMonConfig(inputFlags))

        if inputFlags.DQ.Steering.HLT.doBjet:
            from TrigBjetMonitoring.TrigBjetMonitorAlgorithm import TrigBjetMonConfig
            result.merge(TrigBjetMonConfig(inputFlags))

        if inputFlags.DQ.Steering.HLT.doBphys:
            from TrigBphysMonitoring.TrigBphysMonitorAlgorithm import TrigBphysMonConfig
            result.merge(TrigBphysMonConfig(inputFlags))

        if inputFlags.DQ.Steering.HLT.doCalo:
            from TrigCaloMonitoring.TrigCaloMonitorAlgorithm import TrigCaloMonConfig
            result.merge(TrigCaloMonConfig(inputFlags))

        if inputFlags.DQ.Steering.HLT.doEgamma:
            from TrigEgammaMonitoring.TrigEgammaMonitorAlgorithm import TrigEgammaMonConfig
            result.merge(TrigEgammaMonConfig(inputFlags))

        if inputFlags.DQ.Steering.HLT.doJet:
            from TrigJetMonitoring.TrigJetMonitorAlgorithm import TrigJetMonConfig
            result.merge(TrigJetMonConfig(inputFlags))

        if inputFlags.DQ.Steering.HLT.doMET:
            from TrigMETMonitoring.TrigMETMonitorAlgorithm import TrigMETMonConfig
            result.merge(TrigMETMonConfig(inputFlags))

        if inputFlags.DQ.Steering.HLT.doMinBias:
            from TrigMinBiasMonitoring.TrigMinBiasMonitoringMT import TrigMinBias
            result.merge(TrigMinBias(inputFlags))

        if inputFlags.DQ.Steering.HLT.doMuon:
            from TrigMuonMonitoringMT.TrigMuonMonitoringMTConfig import TrigMuonMonConfig
            result.merge(TrigMuonMonConfig(inputFlags))

        if inputFlags.DQ.Steering.HLT.doTau:
            from TrigTauMonitoring.TrigTauMonitorAlgorithm import TrigTauMonConfig
            result.merge(TrigTauMonConfig(inputFlags))

    return result


