/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MMELECTRONICSTOOLINPUT_H
#define MM_DIGITIZATION_MMELECTRONICSTOOLINPUT_H
#include "Identifier/Identifier.h"
/*******************************************************************************/
class MmElectronicsToolInput {

 public:

 MmElectronicsToolInput(const std::vector<int>& NumberOfStripsPos,
                        const std::vector<std::vector<float>>& chipCharge,
                        const std::vector<std::vector<float>>& chipTime,
			const Identifier DigitId,
			const float kineticEnergy
			)
  : m_chipCharge(chipCharge),
    m_chipTime(chipTime),
    m_NumberOfStripsPos(NumberOfStripsPos),
    m_DigitId(DigitId),
    m_kineticEnergy(kineticEnergy) {}

    ~MmElectronicsToolInput() {}

    std::vector<int> NumberOfStripsPos() const { return m_NumberOfStripsPos; }
    std::vector<std::vector<float>> chipCharge() const { return m_chipCharge; }
    std::vector<std::vector<float>> chipTime() const { return m_chipTime; }
    Identifier DigitId() const { return m_DigitId; }
    float kineticEnergy() const { return m_kineticEnergy; }

 private:
    std::vector<std::vector<float>> m_chipCharge;
    std::vector<std::vector<float>> m_chipTime;
    std::vector<int> m_NumberOfStripsPos;
    Identifier m_DigitId;
    float m_kineticEnergy;
};
/*******************************************************************************/
#endif
