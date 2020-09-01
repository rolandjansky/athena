/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file RecTPCnv/test/MuonCaloEnergyContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "RecTPCnv/MuonCaloEnergyContainerCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Trk::EnergyLoss& p1,
              const Trk::EnergyLoss& p2)
{
  assert (p1.deltaE() == p2.deltaE());
  assert (p1.sigmaDeltaE() == p2.sigmaDeltaE());
  assert (p1.sigmaMinusDeltaE() == p2.sigmaMinusDeltaE());
  assert (p1.sigmaPlusDeltaE() == p2.sigmaPlusDeltaE());
}


void compare (const DepositInCalo& p1,
              const DepositInCalo& p2)
{
  assert (p1.subCaloId() == p2.subCaloId());
  assert (p1.energyDeposited() == p2.energyDeposited());
  assert (p1.muonEnergyLoss() == p2.muonEnergyLoss());
  assert (p1.etDeposited() == p2.etDeposited());
}


void compare (const CaloEnergy& p1,
              const CaloEnergy& p2)
{
  compare (static_cast<const Trk::EnergyLoss&> (p1),
           static_cast<const Trk::EnergyLoss&> (p2));
  assert (p1.energyLossType() == p2.energyLossType());
  assert (p1.caloLRLikelihood() == p2.caloLRLikelihood());
  assert (p1.caloMuonIdTag() == p2.caloMuonIdTag());
  assert (p1.fsrCandidateEnergy() == p2.fsrCandidateEnergy());
  assert (p1.etCore() == p2.etCore());
  assert (p1.deltaEParam() == p2.deltaEParam());
  assert (p1.sigmaMinusDeltaEParam() == p2.sigmaMinusDeltaEParam());
  assert (p1.sigmaPlusDeltaEParam() == p2.sigmaPlusDeltaEParam());
  assert (p1.deltaEMeas() == p2.deltaEMeas());
  assert (p1.sigmaDeltaEMeas() == p2.sigmaDeltaEMeas());

  const std::vector<DepositInCalo>& d1 = p1.depositInCalo();
  const std::vector<DepositInCalo>& d2 = p2.depositInCalo();
  assert (d1.size() == d2.size());
  for (size_t i = 0; i < d1.size(); i++)
    compare (d1[i], d2[i]);
}


void compare (const MuonCaloEnergyContainer& p1,
              const MuonCaloEnergyContainer& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const MuonCaloEnergyContainer& trans1)
{
  MsgStream log (0, "test");
  MuonCaloEnergyContainerCnv_p1 cnv;
  MuonCaloEnergyContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  MuonCaloEnergyContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


std::unique_ptr<CaloEnergy> make_one (float offs)
{
  std::vector<DepositInCalo> deposits;
  deposits.emplace_back (CaloCell_ID::EMB2, offs+10.5, offs+11.5, offs+12.5);
  deposits.emplace_back (CaloCell_ID::EME2, offs+20.5, offs+21.5, offs+22.5);
  return std::make_unique<CaloEnergy> (offs+1.5, offs+2.5, offs+3.5, offs+4.5,
                                       2, offs+5.5, offs+20,
                                       deposits);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  MuonCaloEnergyContainer trans1;
  trans1.push_back (make_one (100));
  trans1.push_back (make_one (200));
  trans1.push_back (make_one (300));
  testit (trans1);
}


int main()
{
  std::cout << "RecTPCnv/MuonCaloEnergyContainerCnv_p1\n";
  test1();
  return 0;
}
