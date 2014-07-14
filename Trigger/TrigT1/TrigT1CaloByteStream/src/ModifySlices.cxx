/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ModifySlices.h"

namespace LVL1BS {

// Return new triggered slice offset

int ModifySlices::peak(const int oldPeak, const int oldSlices,
                                          const int newSlices)
{
  return oldPeak + (newSlices - oldSlices)/2;
}

// Return modified data vector<int>

void ModifySlices::data(const std::vector<int>& oldVec,
                              std::vector<int>& newVec, const int newSlices)
{
  const int oldSlices = oldVec.size();
  const int offset = (newSlices - oldSlices)/2;
  newVec.resize(newSlices);
  for (int sl = 0; sl < newSlices; ++sl) {
    const int oldSl = sl - offset;
    if      (oldSl < 0)          newVec[sl] = oldVec[0];
    else if (oldSl >= oldSlices) newVec[sl] = oldVec[oldSlices - 1];
    else                         newVec[sl] = oldVec[oldSl];
  }
}

// Return modified data vector<unsigned int>

void ModifySlices::data(const std::vector<unsigned int>& oldVec,
                              std::vector<unsigned int>& newVec,
			      const int newSlices)
{
  const int oldSlices = oldVec.size();
  const int offset = (newSlices - oldSlices)/2;
  newVec.resize(newSlices);
  for (int sl = 0; sl < newSlices; ++sl) {
    const int oldSl = sl - offset;
    if      (oldSl < 0)          newVec[sl] = oldVec[0];
    else if (oldSl >= oldSlices) newVec[sl] = oldVec[oldSlices - 1];
    else                         newVec[sl] = oldVec[oldSl];
  }
}

} // end namespace
