/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file LArCondTPCnv/test/LArCaliWaveSubsetCnv_p3_test.cxx
 * @based on p2 test by:
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2021
 * @brief Tests for LArCaliWaveSubsetCnv_p3.
 */


#undef NDEBUG
#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p3.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


typedef LArConditionsSubset<LArCaliWaveVec> TransType;


void compare (const LArWave& p1,
              const LArWave& p2)
{
  assert (p1.getDt() == p2.getDt());
  assert (p1.getFlag() == p2.getFlag());
  assert (p1.getWave() == p2.getWave());
}


void compare (const LArWaveCumul& p1,
              const LArWaveCumul& p2)
{
  assert (p1.getErrors() == p2.getErrors());
  assert (p1.getTriggers() == p2.getTriggers());
  compare (static_cast<const LArWave&>(p1),
           static_cast<const LArWave&>(p2));
}


void compare (const LArCaliWave& p1,
              const LArCaliWave& p2)
{
  assert (p1.getDAC() == p2.getDAC());
  assert (p1.getIsPulsedInt() == p2.getIsPulsedInt());
  compare (static_cast<const LArWaveCumul&>(p1),
           static_cast<const LArWaveCumul&>(p2));
}


void compare (const LArCaliWaveVec& p1,
              const LArCaliWaveVec& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (p1[i], p2[i]);
  }
}


void compare (const TransType::ConstChannelVector& p1,
              const TransType::ConstChannelVector& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (p1[i], p2[i]);
  }
}

             
void compare (const TransType& p1, const TransType& p2)
{
  assert (p1.gain() == p2.gain());
  assert (p1.channel() == p2.channel());
  assert (p1.groupingType() == p2.groupingType());
  assert (p1.subsetSize() == p2.subsetSize());

  TransType::ConstSubsetIt it1 = p1.subsetBegin();
  TransType::ConstSubsetIt it2 = p2.subsetBegin();
  for (; it1 != p1.subsetEnd(); ++it1, ++it2)
  {
    assert ((*it1).first == (*it2).first);
    compare ((*it1).second, (*it2).second);
  }

  assert (p1.correctionVecSize() == p2.correctionVecSize());
  TransType::ConstCorrectionVecIt cit1 = p1.correctionVecBegin();
  TransType::ConstCorrectionVecIt cit2 = p2.correctionVecBegin();
  for (; cit1 != p1.correctionVecEnd(); ++cit1, ++cit2)
  {
    assert ((*cit1).first == (*cit2).first);
    compare ((*cit1).second, (*cit2).second);
  }
}


void testit (const TransType& trans1)
{
  MsgStream log (0, "test");
  LArCaliWaveSubsetCnv_p3 cnv;
  LArCaliWaveSubset_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  TransType trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


LArCaliWave makeWave (int x)
{
  return LArCaliWave (std::vector<double> { 10.5+x, 11.5+x, 12.5+x, 13.5+x,
                                             14.5+x },
                      std::vector<double> { 20.5+x, 21.5+x, 22.5+x, 23.5+x,
                                             24.5+x },
                      std::vector<int> { 30+x, 31+x, 32+x, 33+x, 34+x },
                      1.5 + x,
                      0x00005678 + x,
                      0x00001234 + x,
                      x*2);
}


LArCaliWave makeEmpty (int)
{
  return LArCaliWave();
}


typedef LArCaliWave waveMaker_t (int x);
LArCaliWaveVec makePayload (waveMaker_t* maker, int x)
{
  LArCaliWaveVec vec;
  for (int i = 0; i < (x%5); i++) {
    vec.emplace_back (maker (x + 50*i));
  }
  return vec;
}


TransType makeTrans (waveMaker_t* maker)
{
  const size_t nfeb = 10;
  std::vector<TransType::FebId> febs;
  for (size_t i = 0; i < nfeb; i++) {
    febs.push_back(i);
  }

  TransType trans (febs, 3);
  trans.setChannel (10);
  trans.setGroupingType (7);

  for (size_t feb = 0; feb < nfeb; ++feb) {
    TransType::FebPairReference p = *trans.findChannelVector (feb);
    assert (p.first == feb);
    for (size_t ch = 0; ch < p.second.size(); ++ch) {
      p.second[ch] = makePayload (maker, feb*1000 + ch*10);
    }
  }

  trans.insertCorrection (100, makePayload (maker, 10000));
  trans.insertCorrection (102, makePayload (maker, 10020));
  trans.insertCorrection (103, makePayload (maker, 10030));
  trans.insertCorrection (106, makePayload (maker, 10060));

  return trans;
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TransType trans1 = makeTrans (makeWave);
  testit (trans1);
  TransType trans2 = makeTrans (makeEmpty);
  testit (trans2);
}


int main()
{
  std::cout << "LArCondTPCnv/LArCaliWaveSubsetCnv_p3_test\n";
  test1();
  return 0;
}
