/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigDecSummary.cxx
*/
#include "TriggerMenuNtuple/TrigDecSummary.h"
#include <iostream>
#include <algorithm>

using namespace std;

const int kNL1Words=8;
const int kNHltWords=256;

TrigDecSummary::TrigDecSummary() : 
  mL1_TBP(kNL1Words, 0), mL1_TAP(kNL1Words, 0), mL1_TAV(kNL1Words, 0), 
  mL2_Raw(), mL2_PT(), 
  mEF_Raw(), mEF_PT() {
}

TrigDecSummary::~TrigDecSummary() {
}

void TrigDecSummary::setBitStatus(unsigned int bit, DecType_t type, bool status) {
  int iword = bit/32;
  int ibit = bit%32;
  bool bit_to_large=false;

  if (type == kL1_TBP || type == kL1_TAP || type == kL1_TAV) {
    if (iword >= kNL1Words) bit_to_large = true;
  }
//   if (type == kL2_Raw || type == kL2_PT ||
//       type == kEF_Raw || type == kEF_PT) {
//     if (iword >= kNHltWords) bit_to_large = true;
//   }
  if (bit_to_large) {
    cout << "Bit too large" << endl;
    return;
  }
  if (status) {
    if (type == kL1_TBP) mL1_TBP[iword] |= (1<<ibit);
    if (type == kL1_TAP) mL1_TAP[iword] |= (1<<ibit);
    if (type == kL1_TAV) mL1_TAV[iword] |= (1<<ibit);
    
    if (type == kL2_Raw) add(bit, mL2_Raw);
    if (type == kL2_PT) add(bit, mL2_PT);

    if (type == kEF_Raw) add(bit, mEF_Raw);
    if (type == kEF_PT) add(bit, mEF_PT);
  } else {
    if (type == kL1_TBP) mL1_TBP[iword] &= ~(1<<ibit);
    if (type == kL1_TAP) mL1_TAP[iword] &= ~(1<<ibit);
    if (type == kL1_TAV) mL1_TAV[iword] &= ~(1<<ibit);
    
    if (type == kL2_Raw) remove(bit, mL2_Raw);
    if (type == kL2_PT) remove(bit, mL2_PT);

    if (type == kEF_Raw) remove(bit, mEF_Raw);
    if (type == kEF_PT) remove(bit, mEF_PT);
  }
}

bool TrigDecSummary::bitStatus(unsigned int bit, DecType_t type) const {
  int iword = bit/32;
  int ibit = bit%32;
  bool bit_to_large=false;
  bool status=false;

  if (type == kL1_TBP || type == kL1_TAP || type == kL1_TAV) {
    if (iword >= kNL1Words) bit_to_large = true;
  }
//   if (type == kL2_Raw || type == kL2_PT ||
//       type == kEF_Raw || type == kEF_PT) {
//     if (iword >= kNHltWords) bit_to_large = true;
//   }
  if (bit_to_large) {
    cout << "Bit too large" << endl;
  }

  if (type == kL1_TBP) status = (mL1_TBP[iword] >> ibit) & 0x1;
  if (type == kL1_TAP) status = (mL1_TAP[iword] >> ibit) & 0x1;
  if (type == kL1_TAV) status = (mL1_TAV[iword] >> ibit) & 0x1;

  if (type == kL2_Raw) status = bit_status(bit, mL2_Raw);
  if (type == kL2_PT) status = bit_status(bit, mL2_PT);

  if (type == kEF_Raw) status = bit_status(bit, mEF_Raw);
  if (type == kEF_PT) status = bit_status(bit, mEF_PT);

  return status;
}

void TrigDecSummary::clear() {
  int i;

  for (i=0; i<kNL1Words; ++i) {
    mL1_TBP[i] = 0;
    mL1_TAP[i] = 0;
    mL1_TAV[i] = 0;
  }
//   for (i=0; i<kNHltWords; ++i) {
//     mL2_Raw[i] = 0;
//     mL2_PT[i] = 0;
//     mEF_Raw[i] = 0;
//     mEF_PT[i] = 0;
//   }
  mL2_Raw.clear();
  mL2_PT.clear();
  mEF_Raw.clear();
  mEF_PT.clear();
}

void TrigDecSummary::add(unsigned int n, std::vector<unsigned int>& v) {
  std::vector<unsigned int>::iterator p;
  if ( (p = find(v.begin(), v.end(), n)) == v.end()) v.push_back(n);
}

void TrigDecSummary::remove(unsigned int n, std::vector<unsigned int>& v) {
  std::vector<unsigned int>::iterator p;
  if ( (p = find(v.begin(), v.end(), n)) != v.end()) v.erase(p);
}

bool TrigDecSummary::bit_status(unsigned int n, 
				const std::vector<unsigned int>& v) const {
  std::vector<unsigned int>::const_iterator p;
  if ( (p = find(v.begin(), v.end(), n)) != v.end()) {
    return true;
  } else {
    return false;
  }
}

