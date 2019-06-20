/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKCLUSTERINGPRINTOUT_H
#define FTKCLUSTERINGPRINTOUT_H

#include "TrigFTKSim/FTKRawHit.h"
#include "TrigFTKSim/ftkdefs.h"
#include <map>
#include <memory>
class cluster;

typedef std::vector<std::unique_ptr<FTKRawHit>> hitVector;
typedef std::map<unsigned int, hitVector> hitsByModuleMap;
typedef std::vector<std::unique_ptr<cluster>> cluList;
typedef std::map<unsigned int, cluList> clustersByModuleMap;

/**
 * This class creates a hit whose coordinates are relative to the hit's position
 * in the cluster. It is used to calculate if a cluster is split and for
 * printing out the test vectors.
 */
class FTK_ClusterCoordHit
{
public:
    /**
     * Create an FTK_Hit_FECoords from an FTKRawHit
     * \param h FTKRawHit to be processed
     * \param seed the central hit of the cluster
     */
    FTK_ClusterCoordHit(const FTKRawHit &h, const FTKRawHit& seed) {
        tot = h.getTot();
        crow = h.getPhiSide() - (seed.getPhiSide() - 10);
        if (seed.getEtaStrip() %2 != 0) ccol = h.getEtaStrip() - (seed.getEtaStrip() - 1);
        else  ccol = h.getEtaStrip() - (seed.getEtaStrip());
    }
    int tot; ///<  ToT of the hit in the cluster
    int ccol; ///< Column of the hit in the cluster
    int crow; ///< Row of the hit in the cluster
};



class FTKClusteringPrintout
{

public:
    FTKClusteringPrintout cp();
    void printHitlist(const hitVector& currentHits);
    void printCentroidList(clustersByModuleMap clustersByModule);
    void printClusterList(clustersByModuleMap clustersByModule);
    void printCluster(const std::unique_ptr<cluster> &clu);
    void printHit(const FTKRawHit &hit);
private:
    bool clusterSort (const FTK_ClusterCoordHit &i, const FTK_ClusterCoordHit &j);


};

#endif
