# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from D3PDMakerCoreComps.resolveSGKey import resolveSGKey

def makeClusterFilter( InputName  = "CaloCalTopoCluster",
                       OutputName = "SelectedClusters",
                       ptCut=5000., etaCut=5 ) :

    from CaloD3PDMaker.CaloD3PDMakerConf import ClusterFilter
    clusterFilter = ClusterFilter(OutputName)

    clusterFilter.InputName = resolveSGKey ('CaloClusterContainer',
                                            InputName)
    clusterFilter.OutputName = OutputName 
    clusterFilter.ptCut = ptCut
    clusterFilter.etaCut = etaCut

    return clusterFilter


    


