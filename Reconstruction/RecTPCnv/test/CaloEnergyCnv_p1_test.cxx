/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RecTPCnv/test/CaloEnergyCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "RecTPCnv/CaloEnergyCnv_p1.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "muonEvent/DepositInCalo.h"
#include "muonEvent/CaloEnergy.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Trk::EnergyLoss& p1,
              const Trk::EnergyLoss& p2)
{
  assert (p1.deltaE() == p2.deltaE());
  // Not saved in _p1
  //assert (p1.sigmaDeltaE() == p2.sigmaDeltaE());
  assert (p1.sigmaMinusDeltaE() == p2.sigmaMinusDeltaE());
  assert (p1.sigmaPlusDeltaE() == p2.sigmaPlusDeltaE());
}


void compare (const DepositInCalo& p1,
              const DepositInCalo& p2)
{
  assert (p1.subCaloId() == p2.subCaloId());
  assert (p1.energyDeposited() == p2.energyDeposited());
  assert (p1.muonEnergyLoss() == p2.muonEnergyLoss());
  // Not saved in _p1
  //assert (p1.etDeposited() == p2.etDeposited());
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


void testit (const CaloEnergy& trans1, const CaloEnergy_p1& pers)
{
  MsgStream log (0, "test");
  CaloEnergyCnv_p1 cnv;
  CaloEnergy trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  std::vector<DepositInCalo> deposits;
  deposits.emplace_back (CaloCell_ID::EMB2, 10.5, 11.5, 12.5);
  deposits.emplace_back (CaloCell_ID::EME2, 20.5, 21.5, 22.5);
  CaloEnergy trans1 (1.5, 2.5, 3.5, 4.5, 1, 5.5, 20, deposits);

  CaloEnergy_p1 pers1;
  pers1.m_energyLossType = static_cast<int>(trans1.energyLossType());
  pers1.m_caloLRLikelihood = trans1.caloLRLikelihood();
  pers1.m_caloMuonIdTag = trans1.caloMuonIdTag();
  pers1.m_fsrCandidateEnergy = trans1.fsrCandidateEnergy();
  pers1.m_energyDeposit = trans1.deltaE();
  pers1.m_energySigmaMinus = trans1.sigmaMinusDeltaE();
  pers1.m_energySigmaPlus = trans1.sigmaPlusDeltaE();
  for (const DepositInCalo& d : trans1.depositInCalo()) {
    DepositInCalo_p1 pdep;
    pdep.m_subCaloId = d.subCaloId();
    pdep.m_energyDeposited = d.energyDeposited();
    pdep.m_muonEnergyLoss = d.muonEnergyLoss();
    pers1.m_deposits.push_back (pdep);
  }
  
  testit (trans1, pers1);
}


int main()
{
  std::cout << "RecTPCnv/CaloEnergyCnv_p1\n";
  test1();
  return 0;
}
