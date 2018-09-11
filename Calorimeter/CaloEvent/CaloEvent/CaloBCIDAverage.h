#ifndef CALOEVENT_CALOBCIDAVERAGE_H
#define CALOEVENT_CALOBCIDAVERAGE_H

#include <vector>
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

class CaloBCIDAverage {

 public:
  CaloBCIDAverage() = delete;
  CaloBCIDAverage(std::vector<float>&& data);
  
  float average(const IdentifierHash h) const {
    return m_avg[h];
  }

 private:
  std::vector<float> m_avg;
 
};


#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(CaloBCIDAverage, 141052237, 1)


#endif
