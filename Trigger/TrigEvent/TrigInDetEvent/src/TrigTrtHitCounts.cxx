/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigTrtHitCounts.h"

//---------------------------------------------------------------

TrigTrtHitCounts::TrigTrtHitCounts(): m_endcapC(),
				      m_barrel(),
				      m_endcapA() {
}

//---------------------------------------------------------------

TrigTrtHitCounts::TrigTrtHitCounts(TrigHisto1D endcapC,
				   TrigHisto1D barrel,
				   TrigHisto1D endcapA): m_endcapC(endcapC),
							 m_barrel(barrel),
							 m_endcapA(endcapA) {
}

//---------------------------------------------------------------

TrigTrtHitCounts::~TrigTrtHitCounts() {
}

//---------------------------------------------------------------

TrigTrtHitCounts::TrigTrtHitCounts(const TrigTrtHitCounts& trigTrtHitCounts) {
  m_endcapC = trigTrtHitCounts.m_endcapC;
  m_barrel = trigTrtHitCounts.m_barrel;
  m_endcapA = trigTrtHitCounts.m_endcapA;
}

//---------------------------------------------------------------

TrigHisto1D TrigTrtHitCounts::endcapC(void) const {
  return m_endcapC;
}

TrigHisto1D TrigTrtHitCounts::barrel(void) const {
  return m_barrel;
}

TrigHisto1D TrigTrtHitCounts::endcapA(void) const {
  return m_endcapA;
}
