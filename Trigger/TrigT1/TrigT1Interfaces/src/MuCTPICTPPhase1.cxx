/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1Interfaces/MuCTPICTPPhase1.h"

namespace LVL1 {

  MuCTPICTPPhase1::MuCTPICTPPhase1( const std::vector<unsigned int>& word )
    : m_MuCTPICTPWord( word ) {

  }

  MuCTPICTPPhase1::~MuCTPICTPPhase1() {

  }

  std::vector<unsigned int> MuCTPICTPPhase1::muCTPIWord() const {
    return m_MuCTPICTPWord;
  }

} // namespace LVL1
