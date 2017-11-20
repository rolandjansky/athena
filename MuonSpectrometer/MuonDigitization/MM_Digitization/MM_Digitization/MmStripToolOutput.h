/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MMSTRIPTOOLOUTPUT_H
#define MM_DIGITIZATION_MMSTRIPTOOLOUTPUT_H
#include "Identifier/Identifier.h"
/*******************************************************************************/
class MmStripToolOutput {

 public:

 MmStripToolOutput(const std::vector<int>& NumberOfStripsPos,
		   const std::vector<std::vector<float>>& chipCharge,
		   const std::vector<std::vector<float>>& chipTime)
  : m_chipCharge(chipCharge),
    m_chipTime(chipTime),
    m_NumberOfStripsPos(NumberOfStripsPos) {}

    ~MmStripToolOutput() {}

    std::vector<int> NumberOfStripsPos() const { return m_NumberOfStripsPos; }
    std::vector<std::vector<float>> chipCharge() const { return m_chipCharge; }
    std::vector<std::vector<float>> chipTime() const { return m_chipTime; }

 private:
    std::vector<std::vector<float>> m_chipCharge;
    std::vector<std::vector<float>> m_chipTime;
    std::vector<int> m_NumberOfStripsPos;

};
/*******************************************************************************/
#endif
