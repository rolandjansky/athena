/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloTowerContainer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2013
 * @brief Component test for CaloTowerContainer.
 */

#undef NDEBUG

#include "CaloEvent/CaloTowerContainer.h"
#include "TestTools/expect_exception.h"
#include "TestTools/FLOATassert.h"
#include <iostream>
#include <cstdarg>


typedef CaloTowerContainer::index_t index_t;


bool comp (double x1, double x2, double thresh=1e-6)
{
  return Athena_test::isEqual (x1, x2, thresh);
#if 0
  double n = abs(x1-x2);
  double d = abs(x1) + abs(x2);
  if (d == 0) return true;
  return n/d < thresh;
#endif
}


void test1()
{
  std::cout << "test1\n";
  CaloTowerContainer tc0;

  CaloTowerContainer tc1 (CaloTowerSeg (10, 10, 0, 1, 0, 2));
  assert (tc1.etamin() == 0);
  assert (tc1.phimin() == 0);
  assert (tc1.deta() == 0.1);
  assert (tc1.dphi() == 0.2);
  assert (tc1.neta() == 10);
  assert (tc1.nphi() == 10);

  assert (tc1.towerseg().etamax() == 1);
  assert (tc1.towerseg().phimax() == 2);

  std::vector<CaloCell_ID::SUBCALO> calos;
  assert (tc1.getNumberOfCalos() == 0);
  assert (tc1.getCalos (calos) == 0);
  assert (calos.empty());

  tc1.setCalo (CaloCell_ID::LARHEC);
  tc1.setCalo (CaloCell_ID::TILE);
  assert (tc1.getNumberOfCalos() == 2);
  assert (tc1.getCalos (calos) == 2);
  assert (calos.size() == 2);
  assert (calos[0] == CaloCell_ID::LARHEC);
  assert (calos[1] == CaloCell_ID::TILE);

  for (int ieta=1; ieta <= 10; ieta++)
    for (int iphi=1; iphi <= 10; iphi++)
      tc1.getTower (ieta, iphi)->setE (ieta*iphi);

  assert (tc1.e (0.15, 0.5) == 6);
  assert (tc1.e (2, 3) == 6);
  assert (tc1.e ((size_t)2, (size_t)3) == 6);

  assert (tc1.energy (0.15, 0.5) == 6);
  assert (tc1.energy (2, 3) == 6);
  assert (tc1.energy ((index_t)2, (index_t)3) == 6);

  assert (comp (tc1.et (0.15, 0.5), 6 / cosh (0.15)));
  assert (comp (tc1.et (2, 3), 6 / cosh (0.15)));
  assert (comp (tc1.et ((index_t)2, (index_t)3), 6 / cosh (0.15)));

  CaloTower footower;
  footower.setEta (-1);
  footower.setPhi (-1);
  assert (tc1.getTowerIndex (tc1.getTower (10)) == 10);
  assert (tc1.getTowerIndex (0) == tc1.flagOutOfRange());
  assert (tc1.getTowerIndex (&footower) == tc1.flagOutOfRange());

  assert (tc1.getTowerIndex ((index_t)2, (index_t)3) == 12);
  assert (tc1.getTowerIndex ((index_t)12, (index_t)3) == tc1.flagOutOfRange());
  assert (tc1.getTowerIndex (0.15, 0.5) == 12);
  assert (tc1.getTowerIndex (0.15, -0.5) == tc1.flagOutOfRange());

  assert (tc1.getTowerEtaIndex (tc1.getTower (2, 3)) == 2);
  assert (tc1.getTowerEtaIndex ((CaloTower*)0) == tc1.flagOutOfRange());
  assert (tc1.getTowerEtaIndex (&footower) == tc1.flagOutOfRange());
  assert (tc1.getTowerEtaIndex (0.15) == 2);
  assert (tc1.getTowerEtaIndex (2.0) == tc1.flagOutOfRange());
  assert (tc1.getTowerEtaIndex ((index_t)12) == 2);
  //No out-of-bounds checking for this one.
  //assert (tc1.getTowerEtaIndex ((index_t)200) == tc1.flagOutOfRange());

  assert (tc1.getTowerPhiIndex (tc1.getTower (2, 3)) == 3);
  assert (tc1.getTowerPhiIndex ((CaloTower*)0) == tc1.flagOutOfRange());
  assert (tc1.getTowerPhiIndex (&footower) == tc1.flagOutOfRange());
  assert (tc1.getTowerPhiIndex (0.5) == 3);
  assert (tc1.getTowerPhiIndex (3.0) == tc1.flagOutOfRange());
  assert (tc1.getTowerPhiIndex ((index_t)12) == 3);
  //No out-of-bounds checking for this one.
  //assert (tc1.getTowerPhiIndex ((index_t)200) == tc1.flagOutOfRange());

  index_t index_eta = 0;
  index_t index_phi = 0;
  assert (tc1.getTowerIndices (tc1.getTower (2, 3), index_eta, index_phi));
  assert (index_eta == 2);
  assert (index_phi == 3);
  assert (!tc1.getTowerIndices (0, index_eta, index_phi));
  assert (!tc1.getTowerIndices (&footower, index_eta, index_phi));

  index_eta = 0;
  index_phi = 0;
  assert (tc1.getTowerIndices (0.15, 0.5, index_eta, index_phi));
  assert (index_eta == 2);
  assert (index_phi == 3);
  assert (!tc1.getTowerIndices (-0.5, 0.5, index_eta, index_phi));
  assert (!tc1.getTowerIndices (0.15, 3.0, index_eta, index_phi));

  assert (comp (tc1.getTower (2, 3)->eta(), 0.15));
  assert (comp (tc1.getTower (2, 3)->phi(), 0.5));
  assert (tc1.getTower (0, 3) == 0);
  assert (tc1.getTower (2, 12) == 0);

  assert (comp (tc1.getTower ((index_t)2, (index_t)3)->eta(), 0.15));
  assert (comp (tc1.getTower ((index_t)2, (index_t)3)->phi(), 0.5));
  assert (tc1.getTower ((index_t)0, (index_t)3) == 0);
  assert (tc1.getTower ((index_t)2, (index_t)12) == 0);

  assert (comp (tc1.getTower (0.15, 0.5)->eta(), 0.15));
  assert (comp (tc1.getTower (0.15, 0.5)->phi(), 0.5));
  assert (tc1.getTower (-0.5, 0.5) == 0);
  assert (tc1.getTower (0.15, 3.0) == 0);

  assert (comp (tc1.getTower (12)->eta(), 0.15));
  assert (comp (tc1.getTower (12)->phi(), 0.5));
  assert (tc1.getTower (200) == 0);

  const CaloTowerContainer& ctc1 = tc1;

  assert (comp (ctc1.getTower (2, 3)->eta(), 0.15));
  assert (comp (ctc1.getTower (2, 3)->phi(), 0.5));
  assert (ctc1.getTower (0, 3) == 0);
  assert (ctc1.getTower (2, 12) == 0);

  assert (comp (ctc1.getTower ((index_t)2, (index_t)3)->eta(), 0.15));
  assert (comp (ctc1.getTower ((index_t)2, (index_t)3)->phi(), 0.5));
  assert (ctc1.getTower ((index_t)0, (index_t)3) == 0);
  assert (ctc1.getTower ((index_t)2, (index_t)12) == 0);

  assert (comp (ctc1.getTower (0.15, 0.5)->eta(), 0.15));
  assert (comp (ctc1.getTower (0.15, 0.5)->phi(), 0.5));
  assert (ctc1.getTower (-0.5, 0.5) == 0);
  assert (ctc1.getTower (0.15, 3.0) == 0);

  assert (comp (ctc1.getTower (12)->eta(), 0.15));
  assert (comp (ctc1.getTower (12)->phi(), 0.5));
  assert (ctc1.getTower (200) == 0);

  assert (tc1.flagOutOfRange() == (index_t)-1);
  assert (tc1.isOutOfRange(tc1.flagOutOfRange()));
  assert (!tc1.isOutOfRange(10));
}


void test2()
{
  std::cout << "test2\n";

  CaloTowerContainer tc1 (CaloTowerSeg (10, 10, 0, 1, 0, 2));
  assert (tc1.size() == 100);

  for (int ieta=1; ieta <= 10; ieta++)
    for (int iphi=1; iphi <= 10; iphi++)
      tc1.getTower (ieta, iphi)->setE (ieta*iphi);
  tc1.setCalo (CaloCell_ID::LARHEC);
  tc1.setCalo (CaloCell_ID::TILE);

  CaloTowerContainer tc2 (tc1);
  assert (tc2.deta() == 0.1);
  assert (tc2.dphi() == 0.2);

  std::vector<CaloCell_ID::SUBCALO> calos;
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

  CaloTowerContainer tc3 (CaloTowerSeg (5, 5, 0, 1, 0, 2));
  for (int ieta=1; ieta <= 5; ieta++)
    for (int iphi=1; iphi <= 5; iphi++)
      tc3.getTower (ieta, iphi)->setE (ieta*iphi);
  tc3.setCalo (CaloCell_ID::LAREM);
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

  CaloTowerContainer tc4 (CaloTowerSeg (10, 10, 0, 1, 0, 2), true);
  assert (tc4.size() == 0);
}


template <class T>
void test3_check (T& p)
{
  assert (p[10]->energy() == 2);
  assert (p.at(10)->energy() == 2);
  assert (p.front()->energy() == 1);
  assert (p.back()->energy() == 100);

  assert ((*p.begin())->energy() == 1);
  assert ((*p.rbegin())->energy() == 100);

  double sum = 0;
  for (const CaloTower* i : p) {
    sum += i->energy();
  }
  assert (sum == 3025);

  CaloTowerContainer::const_iterator cit = p.begin();
  assert ((*cit)->energy() == 1);
}


void test3()
{
  std::cout << "test3\n";

  CaloTowerContainer tc1 (CaloTowerSeg (10, 10, 0, 1, 0, 2));
  for (int ieta=1; ieta <= 10; ieta++)
    for (int iphi=1; iphi <= 10; iphi++)
      tc1.getTower (ieta, iphi)->setE (ieta*iphi);

  test3_check (tc1);

  const CaloTowerContainer& ctc1 = tc1;

  test3_check (ctc1);

  double sum = 0;
  for (CaloTowerContainer::iterator i = tc1.begin();
       i != tc1.end(); ++i)
    sum += (*i)->energy();
  assert (sum == 3025);

  sum = 0;
  for (CaloTowerContainer::const_iterator i = ctc1.begin();
       i != ctc1.end(); ++i)
    sum += (*i)->energy();
  assert (sum == 3025);

  sum = 0;
  for (CaloTowerContainer::reverse_iterator i = tc1.rbegin();
       i != tc1.rend(); i++)
    sum += (*i)->energy();
  assert (sum == 3025);

  sum = 0;
  for (CaloTowerContainer::const_reverse_iterator i = ctc1.rbegin();
       i != ctc1.rend(); ++i)
    sum += (*i)->energy();
  assert (sum == 3025);
}


void test4()
{
  std::cout << "test4\n";

  CaloTowerContainer tc1;
  tc1.push_back (new CaloTower);
  CaloTowerContainer tc2;
  tc2.insert (tc2.begin(), tc1.begin(), tc1.end());
  tc2.clear (SG::OWN_ELEMENTS);

  EXPECT_EXCEPTION (std::runtime_error, tc2.push_back (new CaloTower));
  EXPECT_EXCEPTION (std::runtime_error,
                    tc2.insert (tc2.begin(), tc1.begin(), tc1.end()));


  CaloTowerContainer tc3 (CaloTowerSeg (10, 10, 0, 1, 0, 2));

  EXPECT_EXCEPTION (std::runtime_error, tc3.push_back (new CaloTower));
  EXPECT_EXCEPTION (std::runtime_error,
                    tc3.insert (tc3.begin(), tc1.begin(), tc1.end()));
  EXPECT_EXCEPTION (std::runtime_error, tc3.clear (SG::VIEW_ELEMENTS));
}


// test segmentation wrapping in phi
void test5()
{
  std::cout << "test5\n";

  double phimin =  M_PI - 5*0.2;
  double phimax = -M_PI + 5*0.2;
  CaloTowerContainer tc (CaloTowerSeg (10, 10, 0, 1, phimin, phimax));
  assert (tc.deta() == 0.1);
  assert (tc.dphi() == 0.2);

  assert (comp (tc.getTower ((index_t)1, (index_t)3)->phi() - M_PI, -0.5));
  assert (comp (tc.getTower ((index_t)1, (index_t)7)->phi() + M_PI, 0.3));
}


int main()
{
  test1();
  test2();
  test3();
  test4();
  test5();
  return 0;
}
