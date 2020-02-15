/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LArRawConditions/test/LArConditionsSubset_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2020
 * @brief Unit test for LArConditionsSubset.
 */

#undef NDEBUG
#include "LArRawConditions/LArConditionsSubset.h"
#include "LArRawConditions/LArOFCP1.h"
#include "LArRawConditions/LArShapeP2.h"
#include <vector>
#include <iostream>
#include <cassert>


template <class T>
struct Copier
{
  void operator() (const T& src, T& dst) const
  {
    dst = src;
  };
};


struct TestPayload
{
  TestPayload (int x=0) : m_x(x), m_y(x+1) {}
  int m_x;
  int m_y;
};


const size_t nfeb = 10;
const float timeOffset = 1.5;
const float timeBinWidth = 0.5;


template <class T> T makePayload (int);

template<> TestPayload makePayload<TestPayload> (int x)
{
  return TestPayload (x);
}


void checkPayload (int x, const TestPayload& p)
{
  assert (p.m_x == x);
  assert (p.m_y == x+1);
}


template<> LArOFCP1 makePayload<LArOFCP1> (int offs)
{
  std::vector<float> v1 (15);
  std::vector<float> v2 (15);
  for (int i = 0; i < 15; i++) {
    v1[i] = i+1+offs;
    v2[i] = i+1+offs + 50;
  }
  return LArOFCP1 (timeOffset, timeBinWidth, 3, 5, v1, v2, 0);
}


void checkPayload (int x, const LArOFCP1& p)
{
  LArOFCP1 w =  makePayload<LArOFCP1> (x);
  assert (p.timeOffset() == timeOffset);
  assert (p.timeBinWidth() == timeBinWidth);
  assert (p.OFC_aSize() == w.waveSize(0));
  assert (p.OFC_bSize() == w.waveSize(1));

  for (size_t bin = 0; bin < p.OFC_aSize(); bin++) {
    assert (p.OFC_a (bin).asVector() == w.wave (0, bin).asVector());
    assert (p.OFC_b (bin).asVector() == w.wave (1, bin).asVector());
  }
}


template<> LArShapeP2 makePayload<LArShapeP2> (int offs)
{
  std::vector<float> v1 (15);
  std::vector<float> v2 (15);
  for (int i = 0; i < 15; i++) {
    v1[i] = i+1+offs;
    v2[i] = i+1+offs + 50;
  }
  return LArShapeP2 (timeOffset, timeBinWidth, 3, 5, v1, v2, 0);
}


void checkPayload (int x, const LArShapeP2& p)
{
  LArShapeP2 w =  makePayload<LArShapeP2> (x);
  assert (p.timeOffset() == timeOffset);
  assert (p.timeBinWidth() == timeBinWidth);
  assert (p.shapeSize() == w.waveSize(0));
  assert (p.shapeDerSize() == w.waveSize(1));

  for (size_t bin = 0; bin < p.shapeSize(); bin++) {
    assert (p.shape (bin).asVector() == w.wave (0, bin).asVector());
    assert (p.shapeDer (bin).asVector() == w.wave (1, bin).asVector());
  }
}


template <class T, class CV>
void checkChannels (size_t feb, const CV& v)
{
  for (size_t ch = 0; ch < v.size(); ++ch) {
    T payload (v[ch]);
    checkPayload (feb*1000 + ch*10, payload);
  }
}


template <class T>
void checkCorr (const LArConditionsSubset<T>& ss)
{
  typedef const LArConditionsSubset<T> SS;

  assert (ss.correctionVecSize() == 5);
  size_t icorr = 0;
  unsigned int cchan[5] = {100, 102, 103, 105, 106};
  for (typename SS::ConstCorrectionVecIt it = ss.correctionVecBegin();
       it != ss.correctionVecEnd();
       ++it, ++icorr)
  {
    assert (it->first == cchan[icorr]);
    checkPayload ((cchan[icorr]-100)*10 + 10000, it->second);
  }

  assert (ss.findConditionsObj(103)->first == 103);
  checkPayload (10030, ss.findConditionsObj(103)->second);
}


template <class T>
void check (LArConditionsSubset<T>& ss)
{
  typedef const LArConditionsSubset<T> SS;

  assert (ss.gain() == 3);
  assert (ss.channel() == 10);
  assert (ss.groupingType() == 7);
  assert (ss.subsetSize() == nfeb);
  for (size_t feb = 0; feb < 10; ++feb) {
    typename SS::FebPairReference p = *ss.findChannelVector (feb);
    assert (p.first == feb);
    checkChannels<T> (feb, p.second);
  }

  for (typename SS::SubsetIt it = ss.subsetBegin();
       it != ss.subsetEnd();
       ++it)
  {
    checkChannels<T> ((*it).first, (*it).second);
  }
}


template <class T>
void check (const LArConditionsSubset<T>& ss)
{
  typedef const LArConditionsSubset<T> SS;

  assert (ss.gain() == 3);
  assert (ss.channel() == 10);
  assert (ss.groupingType() == 7);
  assert (ss.subsetSize() == nfeb);
  for (size_t feb = 0; feb < 10; ++feb) {
    const auto& p = *ss.findChannelVector (feb);
    assert (p.first == feb);
    checkChannels<T> (feb, p.second);
  }

  for (typename SS::ConstSubsetIt it = ss.subsetBegin();
       it != ss.subsetEnd();
       ++it)
  {
    checkChannels<T> ((*it).first, (*it).second);
  }
}


template <class T>
LArConditionsSubset<T> makeSubset()
{
  typedef LArConditionsSubset<T> SS;

  std::vector<typename SS::FebId> febs;
  for (size_t i = 0; i < nfeb; i++) {
    febs.push_back(i);
  }

  SS ss (febs, 3);
  ss.setChannel (10);
  ss.setGroupingType (7);

  for (size_t feb = 0; feb < nfeb; ++feb) {
    typename SS::FebPairReference p = *ss.findChannelVector (feb);
    assert (p.first == feb);
    for (size_t ch = 0; ch < p.second.size(); ++ch) {
      p.second[ch] = makePayload<T> (feb*1000 + ch*10);
    }
  }

  typename SS::CorrectionVec cv;
  cv.emplace_back (100, makePayload<T> (10000));
  cv.emplace_back (103, makePayload<T> (10030));
  cv.emplace_back (106, makePayload<T> (10060));
  ss.insertCorrections (std::move (cv));
  ss.insertCorrection (102, makePayload<T> (10020));

  cv.emplace_back (105, makePayload<T> (10050));
  ss.insertCorrections (std::move (cv));

  return ss;
}


template <class T>
void testone()
{
  typedef LArConditionsSubset<T> SS;
  SS ss = makeSubset<T>();
  const SS& css = ss;
  check (ss);
  check (css);
  checkCorr (ss);

  SS ss2;
  ss2.assign (ss, Copier<T>());
  check (ss2);
  checkCorr (ss2);
}


void test1()
{
  std::cout << "test1\n";

  testone<TestPayload>();
  testone<LArOFCP1>();
  testone<LArShapeP2>();
}


int main()
{
  std::cout << "LArRawConditions/LArConditionsSubset_test\n";
  test1();
  return 0;
}

