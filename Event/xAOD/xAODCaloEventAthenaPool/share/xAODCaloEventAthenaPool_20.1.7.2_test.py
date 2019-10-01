# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-20.1.7.2/AOD-20.1.7.2-full.pool.root'
keys = [
    #xAOD::CaloCluster
    'CaloCalTopoClusters',
    'ForwardElectronClusters',
    'HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex',
    'InDetTrackParticlesAssociatedClusters',
    'MuonClusterCollection',
    'TauPi0Clusters',
    'egammaClusters',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
