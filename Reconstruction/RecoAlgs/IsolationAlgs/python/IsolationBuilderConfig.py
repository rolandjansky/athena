# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the electron or photon isolation
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def egIsolationCfg(flags, name='egIsolation', **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting '+name+' configuration')

    acc = ComponentAccumulator()

    suff = ''
    if name.find('photon') >= 0:
        suff = 'photon'
        pref = 'Ph'
    elif name.find('electron') >= 0:
        suff = 'electron'
        pref = 'El'
    else:
        mlog.error('Name '+name+' should contain electron or photon')
        return acc
        
    from xAODPrimitives.xAODIso import xAODIso as isoPar
    from IsolationAlgs.IsoToolsConfig import (
        TrackIsolationToolCfg, ElectronTrackIsolationToolCfg,
        EGammaCaloIsolationToolCfg)

    if flags.Detector.EnableID or flags.Detector.EnableITk:
        isoType  = [ [ isoPar.ptcone30, isoPar.ptcone20 ] ]
        isoCor   = [ [ isoPar.coreTrackPtr ] ]
        isoExCor = [ [] ]
        if 'TrackIsolationTool' not in kwargs:
            if pref == 'Ph':
                kwargs['TrackIsolationTool'] = acc.popToolsAndMerge(
                    TrackIsolationToolCfg(flags))
            else:
                kwargs['TrackIsolationTool'] = acc.popToolsAndMerge(
                    ElectronTrackIsolationToolCfg(flags))
            
    if flags.Detector.EnableCalo:
        isoType.append(
            [ isoPar.topoetcone20, isoPar.topoetcone30, isoPar.topoetcone40 ])
        isoCor.append(
            [ isoPar.core57cells, isoPar.ptCorrection, isoPar.pileupCorrection ])
        isoExCor.append([])
        if 'CaloTopoIsolationTool' not in kwargs:
            kwargs['CaloTopoIsolationTool'] = acc.popToolsAndMerge(
                EGammaCaloIsolationToolCfg(flags))

    kwargs[f'{pref}IsoTypes'] = isoType
    kwargs[f'{pref}CorTypes'] = isoCor
    kwargs[f'{pref}CorTypesExtra'] = isoExCor

    kwargs['name'] = suff+'IsolationBuilder'
    
    acc.addEventAlgo(CompFactory.IsolationBuilder(**kwargs))

    mlog.info(suff+" isolation configured")

    return acc

def muIsolationCfg(flags, name='muIsolation', **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting '+name+' configuration')

    acc = ComponentAccumulator()

    from xAODPrimitives.xAODIso import xAODIso as isoPar
    from IsolationAlgs.IsoToolsConfig import (
        TrackIsolationToolCfg, MuonCaloIsolationToolCfg)

    if flags.Detector.EnableID or flags.Detector.EnableITk:
        isoType  = [ [ isoPar.ptcone40, isoPar.ptcone30, isoPar.ptcone20 ] ]
        isoCor   = [ [ isoPar.coreTrackPtr ] ]
        isoExCor = [ [] ]
        if 'TrackIsolationTool' not in kwargs:
            kwargs['TrackIsolationTool'] = acc.popToolsAndMerge(
                TrackIsolationToolCfg(flags))
        
    if flags.Detector.EnableCalo:
        isoType.append(
            [ isoPar.topoetcone20, isoPar.topoetcone30, isoPar.topoetcone40 ])
        isoCor.append([ isoPar.coreCone, isoPar.pileupCorrection ])
        isoExCor.append([])
        if ('CaloTopoIsolationTool' not in kwargs) or (
                'PFlowIsolationTool' not in kwargs):
            cisoTool = acc.popToolsAndMerge(MuonCaloIsolationToolCfg(flags))
        if 'CaloTopoIsolationTool' not in kwargs:
            kwargs['CaloTopoIsolationTool'] = cisoTool
        if flags.Reco.EnablePFlow and 'PFlowIsolationTool' not in kwargs:
            isoType.append(
                [ isoPar.neflowisol20, isoPar.neflowisol30, isoPar.neflowisol40 ])
            isoCor.append([ isoPar.coreCone, isoPar.pileupCorrection ])
            isoExCor.append([])
            kwargs['PFlowIsolationTool'] = cisoTool

    kwargs['MuIsoTypes'] = isoType
    kwargs['MuCorTypes'] = isoCor
    kwargs['MuCorTypesExtra'] = isoExCor
    kwargs['name'] = 'muonIsolationBuilder'
    
    acc.addEventAlgo(CompFactory.IsolationBuilder(**kwargs))

    mlog.info("muon isolation configured")

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO_RUN2
    flags.Output.doWriteESD = True  # To test the ESD parts
    flags.Output.doWriteAOD = True  # To test the AOD parts
    flags.lock()

    mlog = logging.getLogger("isolationConfigTest")
    mlog.info("Configuring photon isolation: ")

    acc = MainServicesCfg(flags)
    acc.merge(egIsolationCfg(flags,name = 'photonIsolation'))
    acc.merge(egIsolationCfg(flags,name = 'electronIsolation'))
    acc.merge(muIsolationCfg(flags,name = 'muonIsolation'))
    printProperties(mlog,
                    acc.getEventAlgo("photonIsolationBuilder"),
                    nestLevel=1,
                    printDefaults=True)
    printProperties(mlog,
                    acc.getEventAlgo("electronIsolationBuilder"),
                    nestLevel=1,
                    printDefaults=True)
    printProperties(mlog,
                    acc.getEventAlgo("muonIsolationBuilder"),
                    nestLevel=1,
                    printDefaults=True)

    with open("isolationconfig.pkl", "wb") as f:
        acc.store(f)
