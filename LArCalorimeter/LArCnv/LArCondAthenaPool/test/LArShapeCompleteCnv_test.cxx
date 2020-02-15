/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file LArCondAthenaPool/test/LArShapeCompleteCnv_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2020
 * @brief Tests for LArShapeCompleteCnv (partial).
 */


#undef NDEBUG
#include "../src/LArShapeCompleteCnv.cxx"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


typedef LArConditionsSubset<LArShapeP>  OldType;
typedef LArConditionsSubset<LArShapeP1> NewType;
const size_t nfeb = 10;


class TestCnv
  : public LArShapeCompleteCnv
{
public:
  using LArShapeCompleteCnv::LArShapeCompleteCnv;
  using LArShapeCompleteCnv::createTransient;
};


OldType::Payload makePayload (int x)
{
  OldType::Payload payload;
  payload.m_vShape.assign ({ 1.5f+x, 2.5f+x, 3.5f+x, 4.5f+x, 5.5f+x });
  payload.m_vShapeDer.assign ({ 21.5f+x, 22.5f+x, 23.5f+x, 24.5f+x, 25.5f+x });
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
  assert (oldPayload.m_vShape.size() == newPayload.m_vShape.size());
  for (size_t i = 0; i < oldPayload.m_vShape.size(); ++i) {
    assert (oldPayload.m_vShape[i] == newPayload.m_vShape[i]);
  }
  assert (oldPayload.m_vShapeDer.size() == newPayload.m_vShapeDer.size());
  for (size_t i = 0; i < oldPayload.m_vShapeDer.size(); ++i) {
    assert (oldPayload.m_vShapeDer[i] == newPayload.m_vShapeDer[i]);
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
  std::cout << "LArCondAthenaPool/LArShapeCompleteCnv_test\n";
  test1 (svcloc);
  return 0;
}
