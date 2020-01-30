/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file LArCondAthenaPool/test/LArCaliWaveContainerCnv_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2020
 * @brief Tests for LArCaliWaveContainerCnv (partial).
 */


#undef NDEBUG
#include "../src/LArCaliWaveContainerCnv.cxx"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


typedef LArConditionsSubset<LArCaliWaveVec>  OldType;
typedef LArConditionsSubset<LArCaliWaveVec> NewType;
const size_t nfeb = 10;


class TestCnv
  : public LArCaliWaveContainerCnv
{
public:
  using LArCaliWaveContainerCnv::LArCaliWaveContainerCnv;
  using LArCaliWaveContainerCnv::createTransient;
};


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


LArCaliWave makeWave (int x)
{
  return LArCaliWave (std::vector<double> { 10.5+x, 11.5+x, 12.5+x, 13.5+x,
                                             14.5+x },
                      std::vector<double> { 20.5+x, 21.5+x, 22.5+x, 23.5+x,
                                             24.5+x },
                      std::vector<int> { 30+x, 31+x, 32+x, 33+x, 34+x },
                      1.5 + x,
                      0x12345678 + x,
                      x*2);
}


OldType::Payload makePayload (int x)
{
  OldType::Payload vec;
  for (int i = 0; i < (x%5); i++) {
    vec.emplace_back (makeWave (x + 50*i));
  }
  return vec;
}


typedef OldType::Payload payloadMaker_t (int x);
OldType makeOld (payloadMaker_t* maker)
{
  std::vector<OldType::FebId> febs;
  for (size_t i = 0; i < nfeb; i++) {
    febs.push_back(i);
  }

  OldType trans (febs, 3);
  trans.setChannel (10);
  trans.setGroupingType (7);

  for (size_t feb = 0; feb < nfeb; ++feb) {
    OldType::FebPairReference p = *trans.findChannelVector (feb);
    assert (p.first == feb);
    for (size_t ch = 0; ch < p.second.size(); ++ch) {
      p.second[ch] = maker (feb*1000 + ch*10);
    }
  }

  trans.insertCorrection (100, maker (10000));
  trans.insertCorrection (102, maker (10020));
  trans.insertCorrection (103, maker (10030));
  trans.insertCorrection (106, maker (10060));

  return trans;
}


void checkPayload (int x, const NewType::Payload& newPayload)
{
  OldType::Payload oldPayload = makePayload (x);
  compare (oldPayload, newPayload);
}


void checkChannels (size_t feb, const NewType::ConstChannelVector& v)
{
  for (size_t ch = 0; ch < v.size(); ++ch) {
    NewType::Payload payload (v[ch]);
    checkPayload (feb*1000 + ch*10, payload);
  }
}


void check (const NewType& ss)
{
  assert (ss.gain() == 3);
  assert (ss.channel() == 10);
  assert (ss.groupingType() == 7);
  assert (ss.subsetSize() == nfeb);
  for (size_t feb = 0; nfeb < 10; ++feb) {
    const auto& p = *ss.findChannelVector (feb);
    assert (p.first == feb);
    checkChannels (feb, p.second);
  }

  assert (ss.correctionVecSize() == 4);
  size_t icorr = 0;
  unsigned int cchan[5] = {100, 102, 103, 106};
  for (typename NewType::ConstCorrectionVecIt it = ss.correctionVecBegin();
       it != ss.correctionVecEnd();
       ++it, ++icorr)
  {
    assert (it->first == cchan[icorr]);
    checkPayload ((cchan[icorr]-100)*10 + 10000, it->second);
  }
}


void test1 (ISvcLocator* svcloc)
{
  std::cout << "test1\n";

  OldType oldObj = makeOld (makePayload);
  TestCnv cnv (svcloc);
  std::unique_ptr<NewType> newObj (cnv.createTransient (&oldObj));
  check (*newObj);
}


int main()
{
  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi (svcloc)) {
    return 1;
  }
  std::cout << "LArCondAthenaPool/LArCaliWaveContainerCnv_test\n";
  test1 (svcloc);
  return 0;
}
