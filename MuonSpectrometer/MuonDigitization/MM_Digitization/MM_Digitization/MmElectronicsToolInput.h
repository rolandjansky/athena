/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MMELECTRONICSTOOLINPUT_H
#define MM_DIGITIZATION_MMELECTRONICSTOOLINPUT_H
 
/*******************************************************************************/ 
class MmElectronicsToolInput {
 
 public:
 
  MmElectronicsToolInput(std::vector<int> NumberOfStripsPos, std::vector<float> chipCharge, std::vector<float> chipTime ) :
    m_NumberOfStripsPos(NumberOfStripsPos),m_chipCharge(chipCharge),m_chipTime(chipTime) {}
          
    ~MmElectronicsToolInput() {}
  
    std::vector<int> NumberOfStripsPos() const { return m_NumberOfStripsPos; }
    std::vector<float> chipCharge() const { return m_chipCharge; } 
    std::vector<float> chipTime() const { return m_chipTime; }
 
 private:
    std::vector<int> m_NumberOfStripsPos; 
    std::vector<float> m_chipCharge; 
    std::vector<float> m_chipTime;
};
/*******************************************************************************/ 	 
#endif  
