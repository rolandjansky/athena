# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
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
