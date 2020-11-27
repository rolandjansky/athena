/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/  

#ifndef RpcBis78_TRIGRAWDATASEGMENT_H
#define RpcBis78_TRIGRAWDATASEGMENT_H

#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"


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
  RpcBis78_TrigRawDataSegment();

  RpcBis78_TrigRawDataSegment(uint8_t etaIndex, uint8_t phiIndex, uint8_t deltaEta, uint8_t deltaPhi, 
                              uint8_t flag3over3Eta, uint8_t flag3over3Phi, uint32_t trigword );
  ~RpcBis78_TrigRawDataSegment() = default;

  uint8_t etaIndex() {return m_etaIndex;}
  uint8_t phiIndex() {return m_phiIndex;}
  uint8_t deltaEta() {return m_deltaEta;}
  uint8_t deltaPhi() {return m_deltaPhi;}
  uint8_t flag3over3Eta() {return m_flag3over3Eta;}
  uint8_t flag3over3Phi() {return m_flag3over3Phi;}
  uint32_t trigword() {return m_trigword;}


 private:

  uint8_t m_etaIndex;
  uint8_t m_phiIndex;
  uint8_t m_deltaEta;
  uint8_t m_deltaPhi;
  uint8_t m_flag3over3Eta;
  uint8_t m_flag3over3Phi;
  uint32_t m_trigword;

};
}

CLASS_DEF(Muon::RpcBis78_TrigRawDataSegment,126491670,1)

#endif   ///  RpcBis78_TRIGRAWDATASEGMENT_H




