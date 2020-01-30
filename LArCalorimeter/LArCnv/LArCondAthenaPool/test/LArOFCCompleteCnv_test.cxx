/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file LArCondAthenaPool/test/LArOFCCompleteCnv_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2020
 * @brief Tests for LArOFCCompleteCnv (partial).
 */


#undef NDEBUG
#include "../src/LArOFCCompleteCnv.cxx"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


typedef LArConditionsSubset<LArOFCP>  OldType;
typedef LArConditionsSubset<LArOFCP1> NewType;
const size_t nfeb = 10;


class TestCnv
  : public LArOFCCompleteCnv
{
public:
  using LArOFCCompleteCnv::LArOFCCompleteCnv;
  using LArOFCCompleteCnv::createTransient;
};


OldType::Payload makePayload (int x)
{
  OldType::Payload payload;
  payload.m_timeOffset = 1.5;
  payload.m_vOFC_a.resize (3);
  payload.m_vOFC_b.resize (3);
  for (size_t i = 0; i < 3; i++) {
    payload.m_vOFC_a[i].resize (5);
    payload.m_vOFC_b[i].resize (5);
    for (size_t j = 0; j < 5; j++) {
      size_t ii = i*5 + j;
      payload.m_vOFC_a[i][j] = ii+1.5+x;
      payload.m_vOFC_b[i][j] = ii+1.5+x + 50;
    }
  }
  return payload;
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
  assert (oldPayload.m_timeOffset == newPayload.timeOffset());
  assert (newPayload.OFC_aSize() == 3);
  assert (newPayload.OFC_bSize() == 3);
  for (size_t i = 0; i < 3; i++) {
    assert (oldPayload.m_vOFC_a[i] == newPayload.OFC_a(i).asVector());
    assert (oldPayload.m_vOFC_b[i] == newPayload.OFC_b(i).asVector());
  }
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
  std::cout << "LArCondAthenaPool/LArOFCCompleteCnv_test\n";
  test1 (svcloc);
  return 0;
}
