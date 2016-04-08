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

TrigTrtHitCounts::TrigTrtHitCounts(const TrigHisto1D& endcapC,
				   const TrigHisto1D& barrel,
				   const TrigHisto1D& endcapA)
  : m_endcapC(endcapC),
    m_barrel(barrel),
    m_endcapA(endcapA)
{
}

//---------------------------------------------------------------

TrigTrtHitCounts::TrigTrtHitCounts(TrigHisto1D&& endcapC,
				   TrigHisto1D&& barrel,
				   TrigHisto1D&& endcapA)
  : m_endcapC(std::move(endcapC)),
    m_barrel(std::move(barrel)),
    m_endcapA(std::move(endcapA))
{
}

//---------------------------------------------------------------

TrigTrtHitCounts::~TrigTrtHitCounts() {
}

//---------------------------------------------------------------

const TrigHisto1D& TrigTrtHitCounts::endcapC(void) const {
  return m_endcapC;
}

const TrigHisto1D& TrigTrtHitCounts::barrel(void) const {
  return m_barrel;
}

const TrigHisto1D& TrigTrtHitCounts::endcapA(void) const {
  return m_endcapA;
}
