/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/  

#ifndef NSW_TRIGRAWDATASEGMENT_H
#define NSW_TRIGRAWDATASEGMENT_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"


namespace Muon {
class NSW_TrigRawDataSegment
{

 public:
  NSW_TrigRawDataSegment();
  NSW_TrigRawDataSegment(uint8_t deltaTheta, uint8_t phiIndex, uint8_t rIndex, uint8_t spare, bool lowRes, bool phiRes, bool monitor);
  NSW_TrigRawDataSegment(uint8_t deltaTheta, uint8_t phiIndex, uint8_t rIndex, bool lowRes, bool phiRes);
  NSW_TrigRawDataSegment(const Muon::NSW_TrigRawDataSegment &segment);

  ~NSW_TrigRawDataSegment() { };

  uint8_t deltaTheta() const {return m_deltaTheta;}
  uint8_t phiIndex() const {return m_phiIndex;}
  uint8_t rIndex() const {return m_rIndex;}
  uint8_t spare() const {return m_spare;}

  bool lowRes() const  {return m_lowRes;}
  bool phiRes() const  {return m_phiRes;}
  bool monitor() const  {return m_monitor;}

  void setDeltaTheta(uint8_t deltaTheta) { m_deltaTheta=deltaTheta; }
  void setPhiIndex(uint8_t phiIndex)     { m_phiIndex=phiIndex; }
  void setRIndex(uint8_t rIndex) { m_rIndex=rIndex; }
  void setSpare(uint8_t spare) {m_spare=spare; }

  void setLowRes(bool lowRes)   {m_lowRes=lowRes;}
  void setPhiRes(bool phiRes)   {m_phiRes=phiRes;}
  void setMonitor(bool monitor) {m_monitor=monitor;}

 private:

  uint8_t m_deltaTheta;
  uint8_t m_phiIndex;
  uint8_t m_rIndex;
  
  uint8_t m_spare;

  bool m_lowRes;
  bool m_phiRes;
  bool m_monitor;

};
}

CLASS_DEF(Muon::NSW_TrigRawDataSegment,218364457,1)

#endif   ///  NSW_TRIGRAWDATASEGMENT_H
