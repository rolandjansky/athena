/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloTopoTowerContainer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2013
 * @brief Component test for CaloTopoTowerContainer.
 */

#undef NDEBUG

#include "CaloEvent/CaloTopoTowerContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/Bootstrap.h"
#include <iostream>
#include <cassert>


void test1 (StoreGateSvc* sg)
{
  std::cout << "test1\n";

  CaloTopoTowerContainer tc1 (CaloTowerSeg (10, 10, 0, 1, 0, 2));

  assert (!tc1.GetUseNoiseTool());
  tc1.SetUseNoiseTool(true);
  assert (tc1.GetUseNoiseTool());

  assert (!tc1.GetUsePileUpNoise());
  tc1.SetUsePileUpNoise(true);
  assert (tc1.GetUsePileUpNoise());

  assert (tc1.GetNoiseSigma() == 0);
  tc1.SetNoiseSigma(5.5);
  assert (tc1.GetNoiseSigma() == 5.5);

  assert (tc1.GetCellESignificanceThreshold() == 0);
  tc1.SetCellESignificanceThreshold(5.5);
  assert (tc1.GetCellESignificanceThreshold() == 5.5);

  assert (tc1.GetMinimumCellEnergy() == 0);
  tc1.SetMinimumCellEnergy(5.5);
  assert (tc1.GetMinimumCellEnergy() == 5.5);

  assert (tc1.GetMinimumClusterEnergy() == 0);
  tc1.SetMinimumClusterEnergy(5.5);
  assert (tc1.GetMinimumClusterEnergy() == 5.5);

  assert (!tc1.GetUseCellWeights());
  tc1.SetUseCellWeights(true);
  assert (tc1.GetUseCellWeights());

  assert (!tc1.GetCaloSelection());
  tc1.SetCaloSelection(true);
  assert (tc1.GetCaloSelection());

  CaloClusterContainer* cc1 = new CaloClusterContainer;
  assert (sg->record (cc1, "cc1").isSuccess());
  assert (tc1.GetClusters().isDefault());
  tc1.SetClusters (cc1);
  assert (!tc1.GetClusters().isDefault());
  assert (tc1.GetClusters().cptr() == cc1);

  CaloTowerContainer* tc2 = new CaloTowerContainer;
  assert (sg->record (tc2, "tc2").isSuccess());
  assert (tc1.GetTowers().isDefault());
  tc1.SetTowers (tc2);
  assert (!tc1.GetTowers().isDefault());
  assert (tc1.GetTowers().cptr() == tc2);

  CaloCellContainer* lc1 = new CaloCellContainer;
  assert (sg->record (lc1, "lc1").isSuccess());
  assert (tc1.GetCells().isDefault());
  tc1.SetCells (lc1);
  assert (!tc1.GetCells().isDefault());
  assert (tc1.GetCells().cptr() == lc1);

  CaloCell2ClusterMap cm1;
  assert (tc1.GetCellToClusterMap() == 0);
  tc1.SetCellToClusterMap (&cm1);
  assert (tc1.GetCellToClusterMap() == &cm1);

  std::vector<CaloCell_ID::SUBCALO> vc1;
  assert (tc1.GetCaloIndices() == vc1);
  vc1.push_back (CaloCell_ID::LAREM);
  vc1.push_back (CaloCell_ID::TILE);
  tc1.SetCaloIndices (vc1);
  assert (tc1.GetCaloIndices() == vc1);
}


void test2 (StoreGateSvc* sg)
{
  std::cout << "test2\n";

  CaloTopoTowerContainer tc1 (CaloTowerSeg (10, 10, 0, 1, 0, 2));
  assert (tc1.size() == 100);

  for (int ieta=1; ieta <= 10; ieta++)
    for (int iphi=1; iphi <= 10; iphi++)
      tc1.getTower (ieta, iphi)->setE (ieta*iphi);
  tc1.setCalo (CaloCell_ID::LARHEC);
  tc1.setCalo (CaloCell_ID::TILE);

  std::vector<CaloCell_ID::SUBCALO> calos;
  calos.push_back (CaloCell_ID::TILE);
  CaloClusterContainer* x1 = new CaloClusterContainer;
  CaloTowerContainer* x2 = new CaloTowerContainer;
  CaloCellContainer* x3 = new CaloCellContainer;
  assert (sg->record (x1, "x1").isSuccess());
  assert (sg->record (x2, "x2").isSuccess());
  assert (sg->record (x3, "x3").isSuccess());
  CaloCell2ClusterMap x4;
  tc1.SetClusters (x1);
  tc1.SetTowers (x2);
  tc1.SetCells (x3);
  tc1.SetCellToClusterMap (&x4);
  tc1.SetMinimumCellEnergy (10);
  tc1.SetMinimumClusterEnergy (11);
  tc1.SetUseCellWeights (true);
  tc1.SetUseNoiseTool (false);
  tc1.SetUsePileUpNoise (true);
  tc1.SetNoiseSigma (12);
  tc1.SetCellESignificanceThreshold (13);
  tc1.SetCaloSelection (false);
  tc1.SetCaloIndices (calos);

  CaloTopoTowerContainer tc2 (tc1);
  assert (tc2.deta() == 0.1);
  assert (tc2.dphi() == 0.2);

  assert (tc2.getCalos (calos) == 2);
  assert (calos.size() == 2);
  assert (calos[0] == CaloCell_ID::LARHEC);
  assert (calos[1] == CaloCell_ID::TILE);

  assert (tc1.e (2, 3) == 6);
  assert (tc2.e (2, 3) == 6);
#ifdef CALOTOWERCONTAINER_USES_DATAPOOL
  assert (tc1.getTower (2, 3) == tc2.getTower (2, 3));
#else
  assert (tc1.getTower (2, 3) != tc2.getTower (2, 3));
#endif

  assert (tc2.GetClusters() == x1);
  assert (tc2.GetTowers() == x2);
  assert (tc2.GetCells() == x3);
  assert (tc2.GetCellToClusterMap() == &x4);
  assert (tc2.GetMinimumCellEnergy() == 10);
  assert (tc2.GetMinimumClusterEnergy() == 11);
  assert (tc2.GetUseCellWeights() == true);
  assert (tc2.GetUseNoiseTool() == false);
  assert (tc2.GetUsePileUpNoise() == true);
  assert (tc2.GetNoiseSigma() == 12);
  assert (tc2.GetCellESignificanceThreshold() == 13);
  assert (tc2.GetCaloSelection() == false);
  calos.clear();
  calos.push_back (CaloCell_ID::TILE);
  assert (tc2.GetCaloIndices() == calos);

  CaloTopoTowerContainer tc3 (CaloTowerSeg (5, 5, 0, 1, 0, 2));
  for (int ieta=1; ieta <= 5; ieta++)
    for (int iphi=1; iphi <= 5; iphi++)
      tc3.getTower (ieta, iphi)->setE (ieta*iphi);
  tc3.setCalo (CaloCell_ID::LAREM);

  calos.clear();
  calos.push_back (CaloCell_ID::LAREM);
  CaloClusterContainer* y1 = new CaloClusterContainer;
  CaloTowerContainer* y2 = new CaloTowerContainer;
  CaloCellContainer* y3 = new CaloCellContainer;
  assert (sg->record (y1, "y1").isSuccess());
  assert (sg->record (y2, "y2").isSuccess());
  assert (sg->record (y3, "y3").isSuccess());
  CaloCell2ClusterMap y4;
  tc3.SetClusters (y1);
  tc3.SetTowers (y2);
  tc3.SetCells (y3);
  tc3.SetCellToClusterMap (&y4);
  tc3.SetMinimumCellEnergy (20);
  tc3.SetMinimumClusterEnergy (21);
  tc3.SetUseCellWeights (false);
  tc3.SetUseNoiseTool (true);
  tc3.SetUsePileUpNoise (false);
  tc3.SetNoiseSigma (22);
  tc3.SetCellESignificanceThreshold (23);
  tc3.SetCaloSelection (true);
  tc3.SetCaloIndices (calos);

  tc2 = tc3;

  assert (tc2.deta() == 0.2);
  assert (tc2.dphi() == 0.4);

  assert (tc2.getCalos (calos) == 1);
  assert (calos.size() == 1);
  assert (calos[0] == CaloCell_ID::LAREM);

  assert (tc2.e (2, 3) == 6);
  assert (tc3.e (2, 3) == 6);
#ifdef CALOTOWERCONTAINER_USES_DATAPOOL
  assert (tc2.getTower (2, 3) == tc3.getTower (2, 3));
#else
  assert (tc2.getTower (2, 3) != tc3.getTower (2, 3));
#endif

  assert (tc2.GetClusters() == y1);
  assert (tc2.GetTowers() == y2);
  assert (tc2.GetCells() == y3);
  assert (tc2.GetCellToClusterMap() == &y4);
  assert (tc2.GetMinimumCellEnergy() == 20);
  assert (tc2.GetMinimumClusterEnergy() == 21);
  assert (tc2.GetUseCellWeights() == false);
  assert (tc2.GetUseNoiseTool() == true);
  assert (tc2.GetUsePileUpNoise() == false);
  assert (tc2.GetNoiseSigma() == 22);
  assert (tc2.GetCellESignificanceThreshold() == 23);
  assert (tc2.GetCaloSelection() == true);
  calos.clear();
  calos.push_back (CaloCell_ID::LAREM);
  assert (tc2.GetCaloIndices() == calos);

  tc1.swap (tc2);
  assert (tc1.deta() == 0.2);
  assert (tc1.dphi() == 0.4);
  assert (tc2.deta() == 0.1);
  assert (tc2.dphi() == 0.2);

  assert (tc1.getCalos (calos) == 1);
  assert (calos.size() == 1);
  assert (calos[0] == CaloCell_ID::LAREM);
  assert (tc2.getCalos (calos) == 2);
  assert (calos.size() == 2);
  assert (calos[0] == CaloCell_ID::LARHEC);
  assert (calos[1] == CaloCell_ID::TILE);

  assert (tc1.e (2, 3) == 6);
  assert (tc2.e (2, 3) == 6);
  assert (tc1.getTower (2, 3) != tc2.getTower (2, 3));

  assert (tc1.GetClusters() == y1);
  assert (tc1.GetTowers() == y2);
  assert (tc1.GetCells() == y3);
  assert (tc1.GetCellToClusterMap() == &y4);
  assert (tc1.GetMinimumCellEnergy() == 20);
  assert (tc1.GetMinimumClusterEnergy() == 21);
  assert (tc1.GetUseCellWeights() == false);
  assert (tc1.GetUseNoiseTool() == true);
  assert (tc1.GetUsePileUpNoise() == false);
  assert (tc1.GetNoiseSigma() == 22);
  assert (tc1.GetCellESignificanceThreshold() == 23);
  assert (tc1.GetCaloSelection() == true);
  calos.clear();
  calos.push_back (CaloCell_ID::LAREM);
  assert (tc1.GetCaloIndices() == calos);

  assert (tc2.GetClusters() == x1);
  assert (tc2.GetTowers() == x2);
  assert (tc2.GetCells() == x3);
  assert (tc2.GetCellToClusterMap() == &x4);
  assert (tc2.GetMinimumCellEnergy() == 10);
  assert (tc2.GetMinimumClusterEnergy() == 11);
  assert (tc2.GetUseCellWeights() == true);
  assert (tc2.GetUseNoiseTool() == false);
  assert (tc2.GetUsePileUpNoise() == true);
  assert (tc2.GetNoiseSigma() == 12);
  assert (tc2.GetCellESignificanceThreshold() == 13);
  assert (tc2.GetCaloSelection() == false);
  calos.clear();
  calos.push_back (CaloCell_ID::TILE);
  assert (tc2.GetCaloIndices() == calos);

  CaloTopoTowerContainer tc4 (CaloTowerSeg (10, 10, 0, 1, 0, 2), true);
  assert (tc4.size() == 0);
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("CaloEvent/CaloTopoTowerContainer_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  

  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StoreGateSvc* sg = 0;
  assert ( svcLoc->service("StoreGateSvc", sg).isSuccess() );

  test1 (sg);
  test2 (sg);
  return 0;
}
