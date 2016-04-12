/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloTowerSeg_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2013
 * @brief Component test for CaloTowerSeg.
 */

#undef NDEBUG

#include "CaloEvent/CaloTowerSeg.h"
#include <vector>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <cassert>


using std::printf;


bool comp (double x1, double x2, double thresh = 1e-6)
{
  double den = std::abs(x1) + std::abs(x2);
  if (den == 0) return true;
  double num = std::abs (x1 - x2);
  return num / den < thresh;
}


// Null constructor
void test1()
{
  std::cout << "test1\n";
  CaloTowerSeg seg1;
  assert (seg1.neta() == 0);
  assert (seg1.nphi() == 0);
  assert (seg1.deta() == 0);
  assert (seg1.dphi() == 0);
  assert (seg1.etamin() == 0);
  assert (seg1.etamax() == 0);
  assert (seg1.phimin() == 0);
  assert (seg1.phimax() == 0);
}


// Window with no phi wrapping
void test2()
{
  typedef CaloTowerSeg::index_t index_t;
  index_t outOfRange = CaloTowerSeg::outOfRange;

  std::cout << "test2\n";
  CaloTowerSeg seg (10, 10, 0, 1, -1, 1);
  assert (seg.neta() == 10);
  assert (seg.nphi() == 10);
  assert (seg.deta() == 0.1);
  assert (seg.dphi() == 0.2);
  assert (seg.etamin() == 0);
  assert (seg.etamax() == 1);
  assert (seg.phimin() == -1);
  assert (seg.phimax() ==  1);

  assert ( seg.inbound ((index_t)1,  (index_t)1));
  assert ( seg.inbound ((index_t)10, (index_t)10));
  assert ( seg.inbound ((index_t)6,  (index_t)3));
  assert (!seg.inbound ((index_t)6,  (index_t)0));
  assert (!seg.inbound ((index_t)6,  (index_t)11));
  assert (!seg.inbound ((index_t)0,  (index_t)3));
  assert (!seg.inbound ((index_t)11, (index_t)3));

  assert ( seg.inbound ( 0.0, -1.0));
  assert ( seg.inbound ( 1.0,  1.0));
  assert ( seg.inbound ( 0.3,  0.2));
  assert (!seg.inbound (-0.1,  0.2));
  assert (!seg.inbound ( 0.3, -1.1));
  assert (!seg.inbound ( 0.3,  1.1));

  assert (seg.etaphi (1, 1) == 0);
  assert (seg.etaphi (3, 4) == 23);
  assert (seg.etaphi (3, 11) == outOfRange);
  assert (seg.etaIndex ((index_t)23) == 3);
  assert (seg.etaIndex ((index_t)0) == 1);
  assert (seg.phiIndex ((index_t)23) == 4);
  assert (seg.phiIndex ((index_t)0) == 1);

  assert (seg.eta (3) == 0.25);
  assert (seg.phi (3) == -0.5);
  assert (std::abs (seg.phi (7) - 0.3) < 1e-12);

  assert (seg.etaIndex (0.25) == 3);
  assert (seg.etaIndex (-1.0) == outOfRange);
  assert (seg.etaIndex (1.1) == outOfRange);

  assert (seg.phiIndex (-0.5) == 3);
  assert (seg.phiIndex (0.3) == 7);
  assert (seg.phiIndex (-1.1) == outOfRange);
  assert (seg.phiIndex (1.1) == outOfRange);
}


// Window with phi wrapping
void test3()
{
  std::cout << "test3\n";
  typedef CaloTowerSeg::index_t index_t;
  index_t outOfRange = CaloTowerSeg::outOfRange;

  double phimin =  M_PI - 5*0.2;
  double phimax = -M_PI + 5*0.2;
  CaloTowerSeg seg (10, 10, 0, 1, phimin, phimax);
  assert (seg.neta() == 10);
  assert (seg.nphi() == 10);
  assert (seg.deta() == 0.1);
  assert (comp (seg.dphi(), 0.2));
  assert (seg.etamin() == 0);
  assert (seg.etamax() == 1);
  assert (seg.phimin() == phimin);
  assert (seg.phimax() == phimax);

  assert ( seg.inbound ((index_t)1,  (index_t)1));
  assert ( seg.inbound ((index_t)10, (index_t)10));
  assert ( seg.inbound ((index_t)6,  (index_t)3));
  assert (!seg.inbound ((index_t)6,  (index_t)0));
  assert (!seg.inbound ((index_t)6,  (index_t)11));
  assert (!seg.inbound ((index_t)0,  (index_t)3));
  assert (!seg.inbound ((index_t)11, (index_t)3));

  assert ( seg.inbound ( 0.0, phimin));
  assert ( seg.inbound ( 1.0, phimax));
  assert ( seg.inbound ( 0.3,  3.1));
  assert (!seg.inbound (-0.1,  3.1));
  assert (!seg.inbound ( 0.3,  2.0));
  assert (!seg.inbound ( 0.3, -2.0));

  assert (seg.etaphi (1, 1) == 0);
  assert (seg.etaphi (3, 4) == 23);
  assert (seg.etaphi (3, 11) == outOfRange);
  assert (seg.etaIndex ((index_t)23) == 3);
  assert (seg.etaIndex ((index_t)0) == 1);
  assert (seg.phiIndex ((index_t)23) == 4);
  assert (seg.phiIndex ((index_t)0) == 1);

  assert (seg.eta (3) == 0.25);
  assert (std::abs (seg.phi (3) - M_PI + 0.5) < 1e-12);
  assert (std::abs (seg.phi (7) + M_PI - 0.3) < 1e-12);

  assert (seg.etaIndex (0.25) == 3);
  assert (seg.etaIndex (-1.0) == outOfRange);
  assert (seg.etaIndex (1.1) == outOfRange);

  assert (seg.phiIndex (-0.5 + M_PI) == 3);
  assert (seg.phiIndex ( 0.3 - M_PI) == 7);
  assert (seg.phiIndex (-2.0) == outOfRange);
  assert (seg.phiIndex (2.0) == outOfRange);
}


// Subseg tests
void test4()
{
  std::cout << "test4\n";

  CaloTowerSeg seg (50, 64, -2.5, 2.5);

  CaloTowerSeg::SubSeg subseg1 = seg.subseg (0.7, 0.3, -0.2, 0.4);
  assert (subseg1.etamin() == 30);
  assert (subseg1.etamax() == 35);
  assert (subseg1.phimin() == 26);
  assert (subseg1.phimax() == 35);
  assert (subseg1.neta() == 6);
  assert (subseg1.nphi() == 10);
  assert (subseg1.size() == 60);

  CaloTowerSeg seg1 = subseg1.segmentation();
  assert (comp (seg1.etamin(), 0.4));
  assert (comp (seg1.etamax(), 1.0));
  assert (comp (seg1.phimin(), 25*M_PI/32 - M_PI));
  assert (comp (seg1.phimax(), 35*M_PI/32 - M_PI));
  assert (seg1.neta() == 6);
  assert (seg1.nphi() == 10);

  CaloTowerSeg::SubSeg subseg2 = seg.subseg (0.7, 0.3, 3.1, 0.4);
  assert (subseg2.etamin() == 30);
  assert (subseg2.etamax() == 35);
  assert (subseg2.phimin() == 60);
  assert (subseg2.phimax() == 4);
  assert (subseg2.neta() == 6);
  assert (subseg2.nphi() == 9);
  assert (subseg2.size() == 54);

  CaloTowerSeg seg2 = subseg2.segmentation();
  assert (comp (seg2.etamin(), 0.4));
  assert (comp (seg2.etamax(), 1.0));
  assert (comp (seg2.phimin(), 59*M_PI/32 - M_PI));
  assert (comp (seg2.phimax(),  4*M_PI/32 - M_PI));
  assert (seg2.neta() == 6);
  assert (seg2.nphi() == 9);
}


void test5a (const std::vector<int>& tows,
             const CaloTowerSeg::SubSeg& subseg)
{
  typedef CaloTowerSeg::SubSegIterator<std::vector<int>::const_iterator>
    tower_subseg_iterator;
  tower_subseg_iterator it = tower_subseg_iterator::make (tows.begin(),
                                                          subseg);
  size_t sz = subseg.size();
  for (size_t i=0; i < sz; ++i, ++it) {
    printf ("%2d %2d %d %d\n", *it, (int)it.itower(), *it/10, *it%10);
  }
}


// SubSegIterator tests.
void test5()
{
  std::cout << "test5\n";
  std::vector<int> test_towers;
  for (int i=0; i < 100; i++) test_towers.push_back(i);
  CaloTowerSeg seg (10, 10, 0, 10);
  test5a (test_towers, seg.subseg (5, 2, 0, 1));
  printf ("\n");
  test5a (test_towers, seg.subseg (5, 2, 3, 1));
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
