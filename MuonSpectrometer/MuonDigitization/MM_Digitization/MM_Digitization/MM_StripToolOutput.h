/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MM_STRIPTOOLOUTPUT_H
#define MM_DIGITIZATION_MM_STRIPTOOLOUTPUT_H
#include "Identifier/Identifier.h"
/*******************************************************************************/
class MM_StripToolOutput {
public:
    MM_StripToolOutput(const std::vector<int>& NumberOfStripsPos, const std::vector<std::vector<float>>& chipCharge,
                       const std::vector<std::vector<float>>& chipTime) :
        m_chipCharge(chipCharge), m_chipTime(chipTime), m_NumberOfStripsPos(NumberOfStripsPos) {}

    ~MM_StripToolOutput() = default;

    const std::vector<int>& NumberOfStripsPos() const { return m_NumberOfStripsPos; }
    const std::vector<std::vector<float>>& chipCharge() const { return m_chipCharge; }
    const std::vector<std::vector<float>>& chipTime() const { return m_chipTime; }

private:
    std::vector<std::vector<float>> m_chipCharge{};
    std::vector<std::vector<float>> m_chipTime{};
    std::vector<int> m_NumberOfStripsPos{};
};
/*******************************************************************************/
#endif
