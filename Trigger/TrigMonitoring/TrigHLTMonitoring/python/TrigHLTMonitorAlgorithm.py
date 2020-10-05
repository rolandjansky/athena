#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigHLTMonitorAlgorithm.py
@date 2019-09-10
@date 2020-09-18
@brief TrigHLTMonitoring top-level files
'''

def createHLTDQConfigFlags():
    from AthenaConfiguration.AthConfigFlags import AthConfigFlags
    acf=AthConfigFlags()

    acf.addFlag('DQ.Steering.HLT.doGeneral', True)
    acf.addFlag('DQ.Steering.HLT.doBjet', True)
    acf.addFlag('DQ.Steering.HLT.doBphys', True)
    acf.addFlag('DQ.Steering.HLT.doCalo', True)
    acf.addFlag('DQ.Steering.HLT.doEgamma', True)
    acf.addFlag('DQ.Steering.HLT.doJet', True)
    acf.addFlag('DQ.Steering.HLT.doMET', True)
    acf.addFlag('DQ.Steering.HLT.doMinBias', True)
    acf.addFlag('DQ.Steering.HLT.doMuon', True)
    acf.addFlag('DQ.Steering.HLT.doTau', True)

    return acf


def TrigHLTMonTopConfig(inputFlags):
    '''Configuring the HLT signatures top-level steering in the DQ monitoring system.'''

    ########
    #HLT top-level steering

    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

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

