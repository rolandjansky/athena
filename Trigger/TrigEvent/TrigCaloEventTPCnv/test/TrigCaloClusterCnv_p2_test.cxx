/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigCaloEventTPCnv/test/TrigCaloClusterCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2014
 * @brief Regression tests for TrigCaloClusterCnv_p2.
 */

#undef NDEBUG
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p2.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p2.h"
#include "TrigCaloEvent/TrigCaloCluster.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <cassert>


void test1()
{
  MsgStream log (0, "test");
  std::cout << "test1\n";
  TrigCaloCluster cl (10000, 1.1, 2.2, 123);
  cl.setRawEt (9000);
  cl.setNCells (456);
  cl.setquality (789);;
  for (int i=0; i < MAXSIZE; i++)
    cl.setRawEnergy ((CaloSampling::CaloSample)i, i*10000 + 20000);

  TrigCaloCluster_p2 pcl;
  TrigCaloClusterCnv_p2 cnv;
  cnv.transToPers (&cl, &pcl, log);

  TrigCaloCluster cl2 (999, 999, 999, 999);
  cl2.setRawEt (999);
  cl2.setNCells (999);
  cl2.setquality (999);;
  for (int i=0; i < MAXSIZE; i++)
    cl2.setRawEnergy ((CaloSampling::CaloSample)i, 999);

  cnv.persToTrans (&pcl, &cl2, log);
  assert (cl2.rawEnergy() == 10000);
  assert (cl2.rawEt() == 9000);
  assert (fabs(cl2.rawEta() - 1.1) < 1e-6);
  assert (fabs(cl2.rawPhi() - 2.2) < 1e-6);
  assert (cl2.RoIword() == 123);
  assert (cl2.nCells() == 456);
  assert (cl2.quality() == 789);
  for (int i=0; i < TrigCaloCluster_p2::MAXSIZE_P; i++)
    assert (cl2.rawEnergy((CaloSampling::CaloSample)i) == i*10000 + 20000);
  for (int i=TrigCaloCluster_p2::MAXSIZE_P; i < MAXSIZE; i++)
    assert (cl2.rawEnergy((CaloSampling::CaloSample)i) == 0);
}


int main()
{
  test1();
  return 0;
}
