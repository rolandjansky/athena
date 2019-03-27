/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// CscSimData.cxx

#include "MuonSimData/CscSimData.h"
#include <iostream>

CscSimData::CscSimData()
: m_word(0) { }

CscSimData::
CscSimData(const std::vector<Deposit>& deposits, int simDataWord)
: m_word((deposits.size()<<29 & 0xe0000000) | (simDataWord & 0x1fffffff)) {
  m_deposits = deposits;
}

CscSimData::
CscSimData(std::vector<Deposit>&& deposits, int simDataWord)
: m_word((deposits.size()<<29 & 0xe0000000) | (simDataWord & 0x1fffffff)) {
  m_deposits = std::move(deposits);
}

