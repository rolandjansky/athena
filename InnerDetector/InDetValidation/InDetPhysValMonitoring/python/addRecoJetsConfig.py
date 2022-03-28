# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def AddRecoJetsIfNotExistingCfg(flags, jetCollection):
    '''
    Add algorithm to create the Reco jets collection unless the
    collection exists already, or a Reco jet finder is already running
    '''
    
    # the jet collection name does not exist in the input file
    # add a jet finder algorithm in front of the monitoring if the algorithm
    # does not yet exist.
    if "xAOD::JetContainer#"+jetCollection not in flags.Input.TypedCollections:
        from AthenaCommon.Logging import logging
        log = logging.getLogger('InDetPhysValMonitoring/addRecoJets.py')

        log.info('DEBUG addRecoJetsIfNotExising {} not in {} [file_type={}]'.format(jetCollection, flags.Input.TypedCollections, flags.Input.Format))

        from JetRecConfig import StandardSmallRJets 
        theCollectionCfg = getattr(StandardSmallRJets, jetCollection.split("Jets")[0])
        from JetRecConfig.JetRecConfig import JetRecCfg

        return JetRecCfg(flags, theCollectionCfg)
