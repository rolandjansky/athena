/*
  Copyright (C) 2021-2020 CERN for the benefit of the ATLAS collaboration
*/  

#ifndef RpcBis78_TRIGRAWDATASEGMENT_H
#define RpcBis78_TRIGRAWDATASEGMENT_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"


/*
  This class is for BIS78 trigger simulation.
  BIS78 trigger goes to TGC sector logic.
  This is different from other BIS chambers of phase 2, in this case the
  trigger is in the barrel logic. A different class may be needed.
*/

namespace Muon {
class RpcBis78_TrigRawDataSegment
{

 public:
  RpcBis78_TrigRawDataSegment()=default;

  RpcBis78_TrigRawDataSegment(uint8_t etaIndex, uint8_t phiIndex, uint8_t deltaEta, uint8_t deltaPhi, 
                              uint8_t flag3over3Eta, uint8_t flag3over3Phi, uint32_t trigword );
  ~RpcBis78_TrigRawDataSegment() = default;

  uint8_t etaIndex() const {return m_etaIndex;}
  uint8_t phiIndex() const {return m_phiIndex;}
  uint8_t deltaEta() const {return m_deltaEta;}
  uint8_t deltaPhi() const {return m_deltaPhi;}
  uint8_t flag3over3Eta() const {return m_flag3over3Eta;}
  uint8_t flag3over3Phi() const {return m_flag3over3Phi;}
  uint32_t trigword() const {return m_trigword;}


 private:

  uint8_t m_etaIndex{0};  // chamber eta index
  uint8_t m_phiIndex{0};  // chamber phi index
  uint8_t m_deltaEta{0};  // eta window for strip coincidence
  uint8_t m_deltaPhi{0};  // phi window for strip coincidence
  uint8_t m_flag3over3Eta{0};  // flag for coincidence in eta of 3 (flag=1) or 2 (flag=0) gaps
  uint8_t m_flag3over3Phi{0};  // flag for coincidence in phi of 3 (flag=1) or 2 (flag=0) gaps
  uint32_t m_trigword{0};  // trigger word

};
}

CLASS_DEF(Muon::RpcBis78_TrigRawDataSegment,126491670,1)

#endif   ///  RpcBis78_TRIGRAWDATASEGMENT_H




