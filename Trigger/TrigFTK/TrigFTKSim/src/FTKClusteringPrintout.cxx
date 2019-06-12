/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKClusteringPrintout.h"
#include "TrigFTKSim/FTKClusteringEngine.h"
#include <iostream>
#include <iomanip>

/*!
 * Function printing the hit details
 * \param hit the hit
 * \return void
 */
void FTKClusteringPrintout::printHit(const FTKRawHit &hit)
{
    std::cout << " Hit details: "
              << " isPixel=" << hit.getIsPixel()
              << " barrel_ec=" << hit.getBarrelEC()
              << " layer_disk=" << hit.getLayer()
              << " phi_module=" << hit.getPhiModule()
              << " eta_module=" << hit.getEtaModule()
              << " eta_strip=" << hit.getEtaStrip()
              << " phi_side=" << hit.getPhiSide()
              << " tot=" << hit.getTot()
              << " n_strips=" << hit.getNStrips()
              << " deltaPhi=" << hit.getDeltaPhi()
              << " deltaEta=" << hit.getDeltaEta()
              << " eventindex=" << hit.getEventIndex()
              << " barcode=" << hit.getBarcode()
              << " bar_pt=" << hit.getBarcodePt()
              << " parentage_mask=" << hit.getParentageMask()
              << "\n";
}


bool FTKClusteringPrintout::clusterSort (const FTK_ClusterCoordHit &i, const FTK_ClusterCoordHit &j)
{
    // reverse the hits on the first row if they are above the seed
    if (i.ccol == 0 && j.ccol == 0 && i.crow <= 10 && j.crow <= 10)
        return j.crow <= i.crow;
    else if (i.ccol == j.ccol)
        return i.crow <= j.crow;
    else
        return i.ccol <= j.ccol;
}

void FTKClusteringPrintout::printCluster(const std::unique_ptr<cluster> &clu)
{
  //To be reimplemented
}

void FTKClusteringPrintout::printClusterList(clustersByModuleMap clustersByModule)
{
    for (auto& module: clustersByModule) { // loop over modules
        for (auto &cluster: module.second) {
            printCluster(cluster);
        }
    }
}


void FTKClusteringPrintout::printCentroidList(clustersByModuleMap clustersByModule)
{
  //To be reimplemented
}

bool sortWords(const FTKRawHit* i, const FTKRawHit* j)
{
    int firstCol  = i->getEtaStrip() + 1;
    int secondCol = j->getEtaStrip() + 1;
    int firstRow  = (ftk::clustering::rowsInFEChipPerIblModuleRow - 1) - i->getPhiSide(); //start from 0
    int secondRow = (ftk::clustering::rowsInFEChipPerIblModuleRow - 1) - j->getPhiSide(); //start from 0

    if(firstCol != secondCol) {
	return firstCol < secondCol;
    }
    else {
	return firstRow > secondRow;
    }


}

void FTKClusteringPrintout::printHitlist(const hitVector& hitlist)
{
    printf("0x08%.7x\n", (hitlist.front()->getIdentifierHash()));
    for(const auto& hit: hitlist) {
        printf("0x00%.2x%.2x%.3x", hit->getTot(), hit->getEtaStrip(), hit->getPhiSide());
        printHit(*(hit.get()));
    }
}

