# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the pflow isolation for egamma in derivation
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def IsolationSteeringDerivCfg(flags, name = 'IsolationSteeringDeriv', inType = 'EMPFlow'):

    mlog = logging.getLogger(name)
    mlog.info('Starting Isolation steering')

    acc = ComponentAccumulator()

    # Prepare densities
    from IsolationAlgs.IsoDensityConfig import (
        NFlowInputAlgCfg, DensityForIsoAlgCfg)
    acc.merge(NFlowInputAlgCfg(flags,InputType = inType))
    suff = 'CSSK' if inType.find('CSSK') >= 0 else ''
    acc.merge(DensityForIsoAlgCfg(flags,name='CentralDensityFor'+suff+'NFlowIso'))
    acc.merge(DensityForIsoAlgCfg(flags,name='ForwardDensityFor'+suff+'NFlowIso'))

    # Prepare CaloIsolationTool
    kwargs = dict()
    margs = dict()
    margs['FlowElementsInConeTool'] = CompFactory.xAOD.FlowElementsInConeTool(
        name=suff+'FlowElementsInConeTool')
    if len(suff) > 0:
        kwargs['CustomConfigurationNameEl'] = suff
        kwargs['CustomConfigurationNamePh'] = suff
        margs['EFlowEDCentralContainer'] = 'NeutralParticle'+suff+'FlowIsoCentralEventShape'
        margs['EFlowEDForwardContainer'] = 'NeutralParticle'+suff+'FlowIsoForwardEventShape'
        margs['FlowElementsInConeTool'].PFlowKey=suff+'NeutralParticleFlowObjects'
    from IsolationAlgs.IsoToolsConfig import EGammaCaloIsolationToolCfg
    kwargs['PFlowIsolationTool'] = acc.popToolsAndMerge(EGammaCaloIsolationToolCfg(flags,**margs))

    # Prepare IsolationBuilder
    from xAODPrimitives.xAODIso import xAODIso as isoPar
    isoType  = [ [ isoPar.neflowisol20, isoPar.neflowisol30, isoPar.neflowisol40 ] ]
    isoCor   = [ [ isoPar.coreCone, isoPar.pileupCorrection ] ]
    isoExCor = [ [ isoPar.coreConeSC ] ]
    kwargs['ElIsoTypes'] = isoType
    kwargs['ElCorTypes'] = isoCor
    kwargs['ElCorTypesExtra'] = isoExCor
    kwargs['PhIsoTypes'] = isoType
    kwargs['PhCorTypes'] = isoCor
    kwargs['PhCorTypesExtra'] = isoExCor

    kwargs['name'] = suff+'PFlowIsolationBuilder'
    
    acc.addEventAlgo(CompFactory.IsolationBuilder(**kwargs))

    mlog.info("PFlow isolation configured")

    return acc

if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO_RUN2
    flags.Output.doWriteESD = True  # To test the ESD parts
    flags.Output.doWriteAOD = True  # To test the AOD parts
    flags.lock()

    mlog = logging.getLogger("isolationConfigTest")
    mlog.info("Configuring isolation: ")

    acc = MainServicesCfg(flags)
    acc.merge(IsolationSteeringDerivCfg(flags))
    acc.printConfig(withDetails=True,
                    printDefaults=True)
    printProperties(mlog,
                    acc.getEventAlgo('PFlowIsolationBuilder'),
                    nestLevel=1,
                    printDefaults=True)

    with open("isolationconfig.pkl", "wb") as f:
        acc.store(f)
