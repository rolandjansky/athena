/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CALOEVENT_CALOBCIDAVERAGE_H
#define CALOEVENT_CALOBCIDAVERAGE_H
#ifndef GENERATIONBASE

#include <unordered_map>
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"

#include "LArRawConditions/LArMCSym.h"
#include "CxxUtils/aligned_vector.h"


class CaloBCIDAverage {
 public:
  CaloBCIDAverage() = delete;
  CaloBCIDAverage(const LArMCSym* mcSym, CxxUtils::vec_aligned_vector<float>&& data);
  
  float average(const Identifier& id) const {
    if (!id.is_valid()) return 0;
    const size_t idx = m_mcSym->ZPhiSymOflIndex(id);
    if (idx == LArMCSym::NO_INDEX) return 0; //Catches the tile-case
    return m_avg[idx];
  }


  float average(const HWIdentifier& id) const {
    if (!id.is_valid()) return 0;
    const size_t idx = m_mcSym->ZPhiSymOnlIndex(id);
    if (idx == LArMCSym::NO_INDEX) return 0; //Catches the tile-case
    return m_avg[idx];
  }

  
  float average(size_t idx) const {
    return m_avg.at(idx);
  }

  
 private:
  const LArMCSym* m_mcSym;
  CxxUtils::vec_aligned_vector<float> m_avg;
};


#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(CaloBCIDAverage, 141052237, 1)

#endif // NOT GENERATIONBASE

#endif
