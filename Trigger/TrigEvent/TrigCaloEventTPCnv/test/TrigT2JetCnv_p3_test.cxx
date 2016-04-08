/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigCaloEventTPCnv/test/TrigT2JetCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TrigT2JetCnv_p3.
 */


#undef NDEBUG
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEventTPCnv/TrigT2JetCnv_p3.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/MsgStream.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <cassert>
#include <iostream>


void compare (const Trig3Momentum& p1,
              const Trig3Momentum& p2)
{
  assert (p1.e() == p2.e());
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
}


void compare (const TrigT2Jet& p1,
              const TrigT2Jet& p2)
{
  assert (p1.e() == p2.e());
  assert (p1.ehad0() == p2.ehad0());
  assert (p1.eem0() == p2.eem0());
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
  assert (p1.RoIword() == p2.RoIword());
  assert (p1.nLeadingCells() == p2.nLeadingCells());
  assert (p1.hecf() == p2.hecf());
  assert (p1.jetQuality() == p2.jetQuality());
  assert (p1.emf() == p2.emf());
  assert (p1.jetTimeCells() == p2.jetTimeCells());

#if 0
  const std::vector<Trig3Momentum>& g1 = *p1.grid();
  const std::vector<Trig3Momentum>& g2 = *p2.grid();
  assert (g1.size() == g2.size());
  for (size_t i = 0; i < g1.size(); i++)
    compare (g1[i], g2[i]);
#endif
  assert (p2.grid() == nullptr);
}


void testit (const TrigT2Jet& trans1)
{
  MsgStream log (0, "test");
  TrigT2JetCnv_p3 cnv;
  TrigT2Jet_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigT2Jet trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigT2Jet trans1;
  trans1.setE (100000);
  trans1.setEhad0 (80000);
  trans1.setEem0 (20000);
  trans1.setEta (2.5);
  trans1.setPhi (0.5);
  trans1.set_RoIword (12345);
  trans1.setNLeadingCells (20);
  trans1.setHecf (0.3);
  trans1.setJetQuality (0.9);
  trans1.setEmf (0.7);
  trans1.setJetTimeCells (0.4);

  auto grid = CxxUtils::make_unique<std::vector<Trig3Momentum> >();
  grid->emplace_back (80000, 3, 1);
  grid->emplace_back (60000, 2, 4);
  trans1.setGrid (grid.release());
  
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
