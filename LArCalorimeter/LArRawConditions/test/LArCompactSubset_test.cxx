/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LArRawConditions/test/LArCompactSubset_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2020
 * @brief Unit test for LArCompactSubset.
 */

#undef NDEBUG
#include "LArRawConditions/LArCompactSubset.h"
#include "LArRawConditions/LAr2DWaveBase.h"
#include <vector>
#include <iostream>
#include <cassert>


const float timeOffset = 1.5;
const float timeBinWidth = 0.5;


LAr2DWaveBase makeWave (int offs)
{
  std::vector<float> v1 (15);
  std::vector<float> v2 (15);
  for (int i = 0; i < 15; i++) {
    v1[i] = i+1+offs;
    v2[i] = i+1+offs + 50;
  }
  return LAr2DWaveBase (timeOffset, timeBinWidth, 3, 5, v1, v2, 0);
}


void checkWave (size_t feb, size_t ch, const LArCompactSubsetChannelProxy& p)
{
  int offs = feb*1000 + 100*ch;
  LAr2DWaveBase w =  makeWave (offs);
  assert (p.isValid());
  assert (p.timeOffset() == timeOffset);
  assert (p.timeBinWidth() == timeBinWidth);
  assert (p.getSize() == w.waveSize(0));
  assert (p.getSize() == w.waveSize(1));

  for (size_t bin = 0; bin < p.getSize(); bin++) {
    assert (p.getData (0, bin).asVector() == w.wave (0, bin).asVector());
    assert (p.getData (1, bin).asVector() == w.wave (1, bin).asVector());
  }
}


void checkWave (size_t feb, size_t ch, LArCompactSubsetConstChannelProxy p)
{
  int offs = feb*1000 + 100*ch;
  LAr2DWaveBase w =  makeWave (offs);
  assert (p.isValid());
  assert (p.timeOffset() == timeOffset);
  assert (p.timeBinWidth() == timeBinWidth);
  assert (p.getSize() == w.waveSize(0));
  assert (p.getSize() == w.waveSize(1));

  for (size_t bin = 0; bin < p.getSize(); bin++) {
    assert (p.getData (0, bin).asVector() == w.wave (0, bin).asVector());
    assert (p.getData (1, bin).asVector() == w.wave (1, bin).asVector());
  }
}


void checkFeb (LArCompactSubsetFebPair p)
{
  assert ((p.first % 1000) == 0);
  size_t feb = p.first / 1000;
  assert (p.second.size() == (feb%3));
  for (size_t ch = 0; ch < p.second.size(); ch++) {
    checkWave (feb, ch, p.second[ch]);
  }

  size_t ch = 0;
  for (const auto& chp : p.second) {
    checkWave (feb, ch, chp);
    ++ch;
  }

  assert (p.second.end() - p.second.begin() == static_cast<long>(p.second.size()));
}


void checkFeb (const LArCompactSubsetConstFebPair& p)
{
  assert ((p.first % 1000) == 0);
  size_t feb = p.first / 1000;
  assert (p.second.size() == (feb%3));
  for (size_t ch = 0; ch < p.second.size(); ch++) {
    checkWave (feb, ch, p.second[ch]);
  }

#if 0
  // No iterator currently defined for const case.
  size_t ch = 0;
  for (const auto& chp : p.second) {
    checkWave (feb, ch, chp);
    ++ch;
  }
#endif
}


void test1()
{
  std::cout << "test1\n";

  const size_t nfeb = 10;
  LArCompactSubsetVector ss (nfeb);
  assert (ss.size() == nfeb);

  for (size_t feb = 0; feb < nfeb; feb++) {
    int offs = feb*1000;
    ss[feb].first = offs;
    size_t nch = feb%3;
    ss[feb].second.resize (nch);
    for (size_t ch = 0; ch < nch; ch++) {
      ss[feb].second[ch] = makeWave (offs + ch*100);
    }
  }

  const LArCompactSubsetVector& css = ss;
  for (size_t feb = 0; feb < nfeb; feb++) {
    checkFeb (ss[feb]);
    checkFeb (css[feb]);
  }

  assert (ss.end() - ss.begin() == static_cast<long>(ss.size()));
  assert (css.end() - css.begin() == static_cast<long>(css.size()));

  for (const auto& pp : ss)
  {
    checkFeb (pp);
  }

  for (const auto& pp : css)
  {
    checkFeb (pp);
  }
}


int main()
{
  std::cout << "LArRawConditions/LArCompactSubset_test\n";
  test1();
  return 0;
}
