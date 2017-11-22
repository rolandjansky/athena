/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MMELECTRONICSTOOLTRIGGEROUTPUT_H
#define MM_DIGITIZATION_MMELECTRONICSTOOLTRIGGEROUTPUT_H
#include "Identifier/Identifier.h"
/*******************************************************************************/
class MmElectronicsToolTriggerOutput {

 public:

 MmElectronicsToolTriggerOutput(
				const std::vector<int>& NumberOfStripsPos,
				const std::vector<float>& chipCharge,
				const std::vector<float>& chipTime,
				const std::vector<int>& VMMid,
				const std::vector<int>& MMFEVMMid)
   : m_chipCharge(chipCharge),
    m_chipTime(chipTime),
    m_NumberOfStripsPos(NumberOfStripsPos),
    m_VMMid(VMMid),
    m_MMFEVMMid(MMFEVMMid) {}

  ~ MmElectronicsToolTriggerOutput() {}

  std::vector<int> NumberOfStripsPos() const { return m_NumberOfStripsPos; }
  std::vector<float> chipCharge() const { return m_chipCharge; }
  std::vector<float> chipTime() const { return m_chipTime; }
  std::vector<int> VMMid() const { return m_VMMid; }
  std::vector<int> MMFEVMMid() const { return m_MMFEVMMid; }

 private:
  std::vector<float> m_chipCharge;
  std::vector<float> m_chipTime;
  std::vector<int> m_NumberOfStripsPos;
  std::vector<int> m_VMMid;
  std::vector<int> m_MMFEVMMid;
};
/*******************************************************************************/
#endif
