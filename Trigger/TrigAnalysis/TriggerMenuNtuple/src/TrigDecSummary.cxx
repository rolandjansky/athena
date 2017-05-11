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

TrigDecSummary::TrigDecSummary() : 
  m_L1_TBP(kNL1Words, 0), m_L1_TAP(kNL1Words, 0), m_L1_TAV(kNL1Words, 0), 
  m_L2_Raw(), m_L2_PT(), 
  m_EF_Raw(), m_EF_PT() {
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
    if (type == kL1_TBP) m_L1_TBP[iword] |= (1<<ibit);
    if (type == kL1_TAP) m_L1_TAP[iword] |= (1<<ibit);
    if (type == kL1_TAV) m_L1_TAV[iword] |= (1<<ibit);
    
    if (type == kL2_Raw) add(bit, m_L2_Raw);
    if (type == kL2_PT) add(bit, m_L2_PT);

    if (type == kEF_Raw) add(bit, m_EF_Raw);
    if (type == kEF_PT) add(bit, m_EF_PT);
  } else {
    if (type == kL1_TBP) m_L1_TBP[iword] &= ~(1<<ibit);
    if (type == kL1_TAP) m_L1_TAP[iword] &= ~(1<<ibit);
    if (type == kL1_TAV) m_L1_TAV[iword] &= ~(1<<ibit);
    
    if (type == kL2_Raw) remove(bit, m_L2_Raw);
    if (type == kL2_PT) remove(bit, m_L2_PT);

    if (type == kEF_Raw) remove(bit, m_EF_Raw);
    if (type == kEF_PT) remove(bit, m_EF_PT);
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

  if (type == kL1_TBP) status = (m_L1_TBP[iword] >> ibit) & 0x1;
  if (type == kL1_TAP) status = (m_L1_TAP[iword] >> ibit) & 0x1;
  if (type == kL1_TAV) status = (m_L1_TAV[iword] >> ibit) & 0x1;

  if (type == kL2_Raw) status = bit_status(bit, m_L2_Raw);
  if (type == kL2_PT) status = bit_status(bit, m_L2_PT);

  if (type == kEF_Raw) status = bit_status(bit, m_EF_Raw);
  if (type == kEF_PT) status = bit_status(bit, m_EF_PT);

  return status;
}

void TrigDecSummary::clear() {
  int i;

  for (i=0; i<kNL1Words; ++i) {
    m_L1_TBP[i] = 0;
    m_L1_TAP[i] = 0;
    m_L1_TAV[i] = 0;
  }
//   for (i=0; i<kNHltWords; ++i) {
//     m_L2_Raw[i] = 0;
//     m_L2_PT[i] = 0;
//     m_EF_Raw[i] = 0;
//     m_EF_PT[i] = 0;
//   }
  m_L2_Raw.clear();
  m_L2_PT.clear();
  m_EF_Raw.clear();
  m_EF_PT.clear();
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

