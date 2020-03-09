/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

  NSW_TrigRawDataSegment(uint8_t deltaTheta, uint8_t phiIndex, uint8_t rIndex,
			 bool lowRes, bool phiRes);

  ~NSW_TrigRawDataSegment() { };

  uint8_t deltaTheta() {return m_deltaTheta;}
  uint8_t phiIndex() {return m_phiIndex;}
  uint8_t rIndex() {return m_rIndex;}

  bool lowRes()   {return m_lowRes;}
  bool phiRes()   {return m_phiRes;}


 private:

  uint8_t m_deltaTheta;
  uint8_t m_phiIndex;
  uint8_t m_rIndex;

  bool m_lowRes;
  bool m_phiRes;

};
}

CLASS_DEF(Muon::NSW_TrigRawDataSegment,218364457,1)

#endif   ///  NSW_TRIGRAWDATASEGMENT_H




