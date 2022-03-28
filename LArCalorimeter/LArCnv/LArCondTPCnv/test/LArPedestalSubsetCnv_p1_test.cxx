/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file LArCondTPCnv/test/LArPedestalSubsetCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2020
 * @brief Tests for LArPedestalSubsetCnv_p1.
 */


#undef NDEBUG
#include "LArCondTPCnv/LArPedestalSubsetCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


typedef LArConditionsSubset<LArPedestalP1> TransType;


void compare (const LArPedestalP1& p1,
              const LArPedestalP1& p2)
{
  assert (p1.m_Pedestal == p2.m_Pedestal);
  assert (p1.m_PedestalRMS == p2.m_PedestalRMS);
}


void compare (const TransType::ChannelVector& p1,
              const TransType::ChannelVector& p2)
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


void transToPers (const TransType& trans,
                  LArPedestalSubset_p1& pers)
{
  // Doesn't test sparse channel logic (which was abandoned in _p2).
  
  pers.m_vPedestalSize = 1;
  pers.m_vPedestalRMSSize = 1;

  const auto subsetEnd = trans.subsetEnd();
  for (auto subsetIt = trans.subsetBegin();
       subsetIt != subsetEnd;
       ++subsetIt)
  {
    unsigned int nfebChans = subsetIt->second.size();
    unsigned int febid = subsetIt->first;
    pers.m_subset.m_febIds.push_back(febid);
    for (unsigned int j=0; j<nfebChans;++j) {  //Loop over channels in FEB
      pers.m_vPedestal.push_back(subsetIt->second[j].m_Pedestal);
      pers.m_vPedestalRMS.push_back(subsetIt->second[j].m_PedestalRMS);
    }//End loop over channels in feb
  }//end loop over febs

  // Copy corrections
  const auto corrEnd = trans.correctionVecEnd();
  for (auto corrIt = trans.correctionVecBegin();
       corrIt != corrEnd;
       ++corrIt)
  {
    // Save channel id in febid vector
    pers.m_subset.m_corrChannels.push_back(corrIt->first);
    pers.m_vPedestal.push_back(corrIt->second.m_Pedestal);
    pers.m_vPedestalRMS.push_back(corrIt->second.m_PedestalRMS);
  }//End loop over corrections

  // Copy the rest
  pers.m_subset.m_gain          = trans.gain(); 
  pers.m_subset.m_channel       = trans.channel();
  pers.m_subset.m_groupingType  = trans.groupingType();
}


void testit (const TransType& trans1)
{
  MsgStream log (0, "test");
  LArPedestalSubsetCnv_p1 cnv;
  LArPedestalSubset_p1 pers;
  transToPers (trans1, pers);
  TransType trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


LArPedestalP1 makePayload (int x)
{
  return LArPedestalP1 ( 1.5+x, 2.5+x );
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  const size_t nfeb = 10;
  std::vector<TransType::FebId> febs;
  for (size_t i = 0; i < nfeb; i++) {
    febs.push_back(i);
  }

  TransType trans1 (febs, 3);
  trans1.setChannel (10);
  trans1.setGroupingType (7);

  for (size_t feb = 0; feb < nfeb; ++feb) {
    TransType::FebPairReference p = *trans1.findChannelVector (feb);
    assert (p.first == feb);
    for (size_t ch = 0; ch < p.second.size(); ++ch) {
      p.second[ch] = makePayload (feb*1000 + ch*10);
    }
  }

  trans1.insertCorrection (100, makePayload (10000));
  trans1.insertCorrection (102, makePayload (10020));
  trans1.insertCorrection (103, makePayload (10030));
  trans1.insertCorrection (106, makePayload (10060));

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "LArCondTPCnv/LArPedestalSubsetCnv_p1_test\n";
  test1();
  return 0;
}
