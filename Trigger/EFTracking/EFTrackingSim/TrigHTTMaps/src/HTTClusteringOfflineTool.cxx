/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "HTTClusteringOfflineTool.h"

HTTClusteringOfflineTool::HTTClusteringOfflineTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
  base_class(algname, name, ifc)
{
}


StatusCode HTTClusteringOfflineTool::DoClustering(HTTLogicalEventInputHeader &header, std::vector<HTTCluster> &clusters) const
{

    clusters = header.optional().getOfflineClusters();
    //fill the multitruth
    for( auto& cluster:clusters){
        HTTHit clusterEquiv = cluster.getClusterEquiv();
        HTTMultiTruth mt;
        HTTMultiTruth::Barcode uniquecode(clusterEquiv.getEventIndex(),clusterEquiv.getBarcode());
        mt.maximize(uniquecode,clusterEquiv.getBarcodePt());
        clusterEquiv.setTruth(mt);
        cluster.setClusterEquiv(clusterEquiv);
    }
    return StatusCode::SUCCESS;
}
