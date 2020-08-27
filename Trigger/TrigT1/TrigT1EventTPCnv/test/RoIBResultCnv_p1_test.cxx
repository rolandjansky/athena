/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/RoIBResultCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for RoIBResultCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/RoIBResultCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const ROIB::Header& p1,
              const ROIB::Header& p2)
{
  assert (p1.header() == p2.header());
}


void compare (const ROIB::Trailer& p1,
              const ROIB::Trailer& p2)
{
  assert (p1.trailer() == p2.trailer());
}


void compare (const ROIB::MuCTPIRoI& p1,
              const ROIB::MuCTPIRoI& p2)
{
  assert (p1.roIWord() == p2.roIWord());
}


void compare (const ROIB::MuCTPIResult& p1,
              const ROIB::MuCTPIResult& p2)
{
  compare (p1.header(), p2.header());
  compare (p1.trailer(), p2.trailer());
  assert (p1.roIVec().size() == p2.roIVec().size());
  for (size_t i = 0; i < p1.roIVec().size(); i++)
    compare (p1.roIVec()[i], p2.roIVec()[i]);
}


void compare (const ROIB::CTPRoI& p1,
              const ROIB::CTPRoI& p2)
{
  assert (p1.roIWord() == p2.roIWord());
}


void compare (const ROIB::CTPResult& p1,
              const ROIB::CTPResult& p2)
{
  compare (p1.header(), p2.header());
  compare (p1.trailer(), p2.trailer());
  assert (p1.roIVec().size() == p2.roIVec().size());
  for (size_t i = 0; i < p1.roIVec().size(); i++)
    compare (p1.roIVec()[i], p2.roIVec()[i]);
}


void compare (const ROIB::JetEnergyRoI& p1,
              const ROIB::JetEnergyRoI& p2)
{
  assert (p1.roIWord() == p2.roIWord());
}


void compare (const ROIB::JetEnergyResult& p1,
              const ROIB::JetEnergyResult& p2)
{
  compare (p1.header(), p2.header());
  compare (p1.trailer(), p2.trailer());
  assert (p1.roIVec().size() == p2.roIVec().size());
  for (size_t i = 0; i < p1.roIVec().size(); i++)
    compare (p1.roIVec()[i], p2.roIVec()[i]);
}


void compare (const ROIB::EMTauRoI& p1,
              const ROIB::EMTauRoI& p2)
{
  assert (p1.roIWord() == p2.roIWord());
}


void compare (const ROIB::EMTauResult& p1,
              const ROIB::EMTauResult& p2)
{
  compare (p1.header(), p2.header());
  compare (p1.trailer(), p2.trailer());
  assert (p1.roIVec().size() == p2.roIVec().size());
  for (size_t i = 0; i < p1.roIVec().size(); i++)
    compare (p1.roIVec()[i], p2.roIVec()[i]);
}


void compare (const L1TopoRDO& p1,
              const L1TopoRDO& p2)
{
  assert (p1.getSourceID() == p2.getSourceID());
  assert (p1.getErrors() == p2.getErrors());
  assert (p1.getDataWords() == p2.getDataWords());
  assert (p1.getStatusWords() == p2.getStatusWords());
}


void compare (const ROIB::L1TopoResult& p1,
              const ROIB::L1TopoResult& p2)
{
  compare (p1.header(), p2.header());
  compare (p1.trailer(), p2.trailer());
  compare (p1.rdo(), p2.rdo());
}


void compare (const ROIB::RoIBResult& p1,
              const ROIB::RoIBResult& p2)
{
  compare (p1.muCTPIResult(), p2.muCTPIResult());
  compare (p1.cTPResult(), p2.cTPResult());

  assert (p1.jetEnergyResult().size() == p2.jetEnergyResult().size());
  for (size_t i = 0; i < p1.jetEnergyResult().size(); i++)
    compare (p1.jetEnergyResult()[i], p2.jetEnergyResult()[i]);

  assert (p1.eMTauResult().size() == p2.eMTauResult().size());
  for (size_t i = 0; i < p1.eMTauResult().size(); i++)
    compare (p1.eMTauResult()[i], p2.eMTauResult()[i]);

  assert (p1.l1TopoResult().size() == p2.l1TopoResult().size());
  for (size_t i = 0; i < p1.l1TopoResult().size(); i++)
    compare (p1.l1TopoResult()[i], p2.l1TopoResult()[i]);
}


void testit (const ROIB::RoIBResult& trans1)
{
  MsgStream log (0, "test");
  RoIBResultCnv_p1 cnv;
  RoIBResult_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ROIB::RoIBResult trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  using namespace ROIB;
  MuCTPIResult muc (Header (std::vector<unsigned int> {1, 2, 3}),
                    Trailer(std::vector<unsigned int> {4, 5, 6}),
                    std::vector< MuCTPIRoI > {7, 8, 9});
                          
  CTPResult ctp (3,
                 Header (std::vector<unsigned int> {11, 12, 13}),
                 Trailer(std::vector<unsigned int> {14, 15, 16}),
                 std::vector< unsigned int > {17, 18, 19});

  std::vector<JetEnergyResult> jet;
  for (int i=0; i<3; i++) {
    uint32_t o = (i+2)*10;
    jet.emplace_back (Header (std::vector<unsigned int> {o+1, o+2, o+3}),
                      Trailer(std::vector<unsigned int> {o+4, o+5, o+6}),
                      std::vector< JetEnergyRoI > {o+7, o+8, o+9});
  }

  std::vector<EMTauResult> emtau;
  for (int i=0; i<4; i++) {
    uint32_t o = (i+5)*10;
    emtau.emplace_back (Header (std::vector<unsigned int> {o+1, o+2, o+3}),
                        Trailer(std::vector<unsigned int> {o+4, o+5, o+6}),
                        std::vector< EMTauRoI > {o+7, o+8, o+9});
  }

  RoIBResult trans1 (std::move(muc), std::move(ctp), std::move(jet), std::move(emtau));
  
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
