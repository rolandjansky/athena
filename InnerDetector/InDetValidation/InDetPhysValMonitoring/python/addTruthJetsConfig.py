# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def AddTruthJetsIfNotExistingCfg(flags):
    '''
    Add algorithm to create the truth jets collection unless the
    collection exists already, or a truth jet finder is already running
    '''
    
    # the jet collection name does not exist in the input file
    # add a jet finder algorithm in front of the monitoring if the algorithm
    # does not yet exist.
    if "xAOD::JetContainer#AntiKt4TruthJets" not in flags.Input.TypedCollections:
        from AthenaCommon.Logging import logging
        log = logging.getLogger('InDetPhysValMonitoring/addTruthJets.py')

        log.info('DEBUG addTruthJetsIfNotExising {} not in {} [file_type={}]'.format("AntiKt4TruthJets", flags.Input.TypedCollections, flags.Input.Format))

        from JetRecConfig.StandardSmallRJets import AntiKt4Truth
        from JetRecConfig.JetRecConfig import JetRecCfg

        return JetRecCfg(flags, AntiKt4Truth)
