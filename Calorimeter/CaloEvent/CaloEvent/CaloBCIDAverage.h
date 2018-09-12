#ifndef CALOEVENT_CALOBCIDAVERAGE_H
#define CALOEVENT_CALOBCIDAVERAGE_H

#include <unordered_map>
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"

#include "LArRawConditions/LArMCSym.h"

class CaloBCIDAverage {

 public:
  CaloBCIDAverage() = delete;
  CaloBCIDAverage(const LArMCSym* mcSym, std::unorderd_map<HWIdentifier, float>&& data);
  
  float average(const Identifier id) const {
    const HWIdentifier hwid=m_mcSym->ZPhiSymOfl(id);
    return m_avg[hwid];
  }

 private:
  const LArMCSym* m_mcSym;
  std::unordered_map<Identifier,float> m_avg;

};


#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(CaloBCIDAverage, 141052237, 1)


#endif
