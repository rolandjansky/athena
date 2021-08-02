# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def AthenaMonitoringAODRecoCfg(flags):
    import logging
    local_logger = logging.getLogger('AthenaMonitoringAODRecoCfg')
    info = local_logger.info
    result = ComponentAccumulator()

    if flags.DQ.Environment == 'AOD':
        info('Running on AOD: Scheduling rebuild of standard jet collections if necessary')
        from JetRecConfig.StandardSmallRJets import AntiKt4EMTopo, AntiKt4EMPFlow, AntiKt4LCTopo
        from JetRecConfig.StandardLargeRJets import AntiKt10LCTopo
        jets_to_schedule = (AntiKt4EMTopo, AntiKt4EMPFlow, AntiKt4LCTopo, AntiKt10LCTopo)

        info(f'Ensuring presence of jet collections for monitoring: {jets_to_schedule}')
        from JetRecConfig.JetRecConfig import JetRecCfg
        for container in jets_to_schedule:
            result.merge(JetRecCfg(flags, container))

        info('Scheduling b-tagging of rebuilt jets')
        from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
        result.merge(AtlasGeometryCfg(flags))
        from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
        result.merge(BeamSpotCondAlgCfg(flags))
        from BTagging.BTagRun3Config import BTagRecoSplitCfg
        # would rather use removesuffix below but need to wait for Python 3.9
        for container in [_.fullname().replace('Jets','') for _ in jets_to_schedule]:
            if container in ('AntiKt4EMTopo', 'AntiKt4EMPFlow'):
                result.merge(BTagRecoSplitCfg(flags, [container]))

        info('Scheduling rebuild of standard MET')
        from METReconstruction.METAssociatorCfg import METAssociatorCfg
        for container in [_.fullname().replace('Jets','') for _ in jets_to_schedule]:
            if container in ('AntiKt4EMTopo', 'AntiKt4EMPFlow', 'AntiKt4LCTopo'):
                result.merge(METAssociatorCfg(flags, container))          
        from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
        result.merge(CaloNoiseCondAlgCfg(flags)) # Prereq for Calo MET
        from METReconstruction.METCalo_Cfg import METCalo_Cfg
        result.merge(METCalo_Cfg(flags))

    return result
