# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the electron, photon and muon isolation
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def EMTopoInputAlgCfg(flags, name='EMTopoInputAlg', **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting input building for isolation density configuration')

    acc = ComponentAccumulator()

    from JetRecConfig.StandardJetConstits import stdConstitDic as cst
    from JetRecConfig.JetRecConfig import JetInputCfg, getConstitPJGAlg

    # Get the input (from CaloCal to EM scale, no origin correction)
    acc.merge(JetInputCfg(flags, cst.EMTopo))

    # Get the pseudo jets for this input
    acc.addEventAlgo(getConstitPJGAlg(cst.EMTopo))

    return acc

def NFlowInputAlgCfg(flags, name='NFlowInputAlg', **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting input building for neutral eflow isolation density configuration')

    acc = ComponentAccumulator()

    from JetRecConfig.StandardJetConstits import stdConstitDic as cst
    from JetRecConfig.JetRecConfig import JetInputCfg

    # Get the input (FlowObject, with CHS and origin (for neutral) and weight (for charged) correction)
    acc.merge(JetInputCfg(flags, cst.EMPFlow) )

    # Then transform into pseudo-jets for the neutral only
    constitpjalg = CompFactory.PseudoJetAlgorithm(
        name = "PseudoJetAlgForIsoNFlow",
        InputContainer = "CHSNeutralParticleFlowObjects",
        OutputContainer = "PseudoJetNFlow",
        Label = "EMNPFlow",
        SkipNegativeEnergy=True)

    # Add the algs to the sequence in the ComponentAccumulator
    acc.addEventAlgo(constitpjalg)

    return acc


def DensityForIsoAlgCfg(flags, name = "CentralDensityForTopoIso", **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting density alg for isolation configuration')

    acc = ComponentAccumulator()

    # Need to understand how to know that the above has been configured
    #acc.merge(EMTopoInputAlgCfg(flags))
    
    # And then the density tool and algs. By default the central one
    if name.find('Topo') >= 0:
        inputO = 'PseudoJetEMTopo'
        outputS = 'TopoCluster'
    elif name.find('NFlow') >= 0:
        inputO = 'PseudoJetNFlow'
        outputS = 'NeutralParticleFlow'
    kwargs['InputContainer'] = inputO
    kwargs['JetRadius'] = 0.5
    kwargs['UseFourMomArea'] = True
    kwargs['VoronoiRfact'] = 0.9
    kwargs['JetAlgorithm'] = 'Kt'
    if name.find('Central') >= 0:
        kwargs['OutputContainer'] = outputS+'IsoCentralEventShape'
        kwargs['AbsRapidityMin'] = 0.0
        kwargs['AbsRapidityMax'] = 1.5
    elif name.find('Forward') >= 0:
        kwargs['OutputContainer'] = outputS+'IsoForwardEventShape'
        kwargs['AbsRapidityMin'] = 1.5
        kwargs['AbsRapidityMax'] = 3.0
    densityTool = CompFactory.EventDensityTool(
        name = name+'Tool',**kwargs)
    
    densityAlg = CompFactory.EventDensityAthAlg(
        name = name+'Alg',
        EventDensityTool = densityTool)
    acc.addEventAlgo(densityAlg)

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO
    flags.Output.doWriteESD = True  # To test the ESD parts
    flags.Output.doWriteAOD = True  # To test the AOD parts
    flags.lock()

    mlog = logging.getLogger("densityForIsolationConfigTest")
    mlog.info("Configuring density for isolation: ")

    acc = MainServicesCfg(flags)
    acc.merge(EMTopoInputAlgCfg(flags))
    acc.merge(DensityForIsoAlgCfg(flags))
    acc.merge(DensityForIsoAlgCfg(flags, name='ForwardDensityForIso'))
    acc.merge(NFlowInputAlgCfg(flags))
    acc.merge(DensityForIsoAlgCfg(flags,name='CentralDensityForNFlowIso'))
    printProperties(mlog,
                    acc.getEventAlgo('CentralDensityForTopoIsoAlg'),
                    nestLevel=1,
                    printDefaults=True)
    printProperties(mlog,
                    acc.getEventAlgo('JetModAlgForIsoNFlow'),
                    nestLevel=1,
                    printDefaults=True)

    with open("isolationdensityconfig.pkl", "wb") as f:
        acc.store(f)
