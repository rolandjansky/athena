/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./SpecialTrigger.h"

LVL1CTP::RandomTrigger::RandomTrigger( size_t rndmNr, 
                                       const CTPdataformatVersion * ctpFormat) :
   m_rndmNr(rndmNr),
   m_pitPos(ctpFormat->getMaxTIPtotal() + ctpFormat->getRandomTrigShift() + rndmNr)
{}


LVL1CTP::BunchGroupTrigger::BunchGroupTrigger( size_t bgrpNr,
                                               const std::vector<unsigned int>& bunches,
                                               const CTPdataformatVersion * ctpFormat) :
   m_bunches(bunches),
   m_bgrpNr(bgrpNr),
   m_pitPos(ctpFormat->getMaxTIPtotal() + ctpFormat->getBunchGroupShift() + bgrpNr)
{}

