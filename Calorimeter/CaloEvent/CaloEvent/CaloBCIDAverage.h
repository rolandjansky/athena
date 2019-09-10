/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CALOEVENT_CALOBCIDAVERAGE_H
#define CALOEVENT_CALOBCIDAVERAGE_H

#include <unordered_map>
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"

#include "LArRawConditions/LArMCSym.h"


class CaloBCIDAverage {
 public:
  CaloBCIDAverage() = delete;
  CaloBCIDAverage(const LArMCSym* mcSym, std::unordered_map<unsigned, float>&& data);
  
  float average(const Identifier id) const {
    if (!id.is_valid()) return 0;
    const HWIdentifier hwid=m_mcSym->ZPhiSymOfl(id);
    if (!hwid.is_valid()) return 0; //Catches the tile-case
    const unsigned id32=hwid.get_identifier32().get_compact();
    const auto it=m_avg.find(id32);
    if (it==m_avg.end()) {
      std::abort();
    }
    return it->second;
  }

 private:
  const LArMCSym* m_mcSym;
  std::unordered_map<unsigned,float> m_avg;

};


#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(CaloBCIDAverage, 141052237, 1)


#endif
