# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def AthenaMonitoringAODRecoCfg(flags):
    import logging
    local_logger = logging.getLogger('AthenaMonitoringAODRecoCfg')
    info = local_logger.info
    result = ComponentAccumulator()

    if flags.DQ.Environment == 'AOD':
        info('Running on AOD: Scheduling rebuild of standard jet collections if necessary')
        from JetRecConfig.StandardSmallRJets import AntiKt4EMTopo, AntiKt4EMPFlow, AntiKt4LCTopo
        from JetRecConfig.StandardLargeRJets import AntiKt10LCTopo_noVR
        # first, check small R jets
        jets_to_schedule = [_ for _ in (AntiKt4EMTopo, AntiKt4EMPFlow, AntiKt4LCTopo)
                            if _.fullname() not in flags.Input.Collections]
        # if we reschedule small R jets, check if we need to reschedule large R as well
        if jets_to_schedule:
            jets_to_schedule += [_ for _ in (AntiKt10LCTopo_noVR,)
                                 if _.fullname() not in flags.Input.Collections]

        if jets_to_schedule:
            info(f'Ensuring presence of jet collections for monitoring: {jets_to_schedule}')
            from JetRecConfig.JetRecConfig import JetRecCfg
            for container in jets_to_schedule:
                result.merge(JetRecCfg(flags, container))

        jet_collections = set([_.fullname().replace('Jets','') for _ in jets_to_schedule])
        btag_jet_collections = set(['AntiKt4EMTopo', 'AntiKt4EMPFlow'])
        met_jet_collections = set(['AntiKt4EMTopo', 'AntiKt4EMPFlow', 'AntiKt4LCTopo'])

        if jet_collections & btag_jet_collections:
            info('Scheduling b-tagging of rebuilt jets')
            from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
            result.merge(AtlasGeometryCfg(flags))
            from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
            result.merge(BeamSpotCondAlgCfg(flags))
            from BTagging.BTagRun3Config import BTagRecoSplitCfg
            # would rather use removesuffix below but need to wait for Python 3.9
            for container in jet_collections & btag_jet_collections:
                result.merge(BTagRecoSplitCfg(flags, [container]))

        if jet_collections & met_jet_collections:
            info('Scheduling rebuild of standard MET')
            from METReconstruction.METAssociatorCfg import METAssociatorCfg
            for container in jet_collections & met_jet_collections:
                result.merge(METAssociatorCfg(flags, container))          
            from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
            result.merge(CaloNoiseCondAlgCfg(flags)) # Prereq for Calo MET
            from METReconstruction.METCalo_Cfg import METCalo_Cfg
            result.merge(METCalo_Cfg(flags))

    return result
