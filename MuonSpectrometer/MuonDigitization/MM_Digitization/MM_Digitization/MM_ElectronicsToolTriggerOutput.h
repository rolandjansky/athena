/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MM_ELECTRONICSTOOLTRIGGEROUTPUT_H
#define MM_DIGITIZATION_MM_ELECTRONICSTOOLTRIGGEROUTPUT_H
#include "Identifier/Identifier.h"
/*******************************************************************************/
class MM_ElectronicsToolTriggerOutput {
public:
    MM_ElectronicsToolTriggerOutput(const std::vector<int>& NumberOfStripsPos, const std::vector<float>& chipCharge,
                                    const std::vector<float>& chipTime, const std::vector<int>& VMMid, const std::vector<int>& MMFEVMMid) :
        m_chipCharge(chipCharge), m_chipTime(chipTime), m_NumberOfStripsPos(NumberOfStripsPos), m_VMMid(VMMid), m_MMFEVMMid(MMFEVMMid) {}

    ~MM_ElectronicsToolTriggerOutput() = default;

    const std::vector<int>& NumberOfStripsPos() const { return m_NumberOfStripsPos; }
    const std::vector<float>& chipCharge() const { return m_chipCharge; }
    const std::vector<float>& chipTime() const { return m_chipTime; }
    const std::vector<int>& VMMid() const { return m_VMMid; }
    const std::vector<int>& MMFEVMMid() const { return m_MMFEVMMid; }

private:
    std::vector<float> m_chipCharge{};
    std::vector<float> m_chipTime{};
    std::vector<int> m_NumberOfStripsPos{};
    std::vector<int> m_VMMid{};
    std::vector<int> m_MMFEVMMid{};
};
/*******************************************************************************/
#endif
