infile = 'esd/ESD-16.6.2.1.pool.root'
keys = [
    #InDet::PixelClusterContainer_p3
    'PixelClusters',

    #InDet::SCT_ClusterContainer_p2
    'SCT_Clusters',

    #InDet::TRT_DriftCircleContainer_p2
    'TRT_DriftCircles',

    #InDetLowBetaContainer_tlp1
    'InDetLowBetaCandidates',

    #InDet::PixelGangedClusterAmbiguities_p1
    'PixelClusterAmbiguitiesMap',

    #InDetRawDataContainer_p1
    'PixelRDOs',
    'TRT_RDOs',

    #SCT_RawDataContainer_p2
    'SCT_RDOs',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')

