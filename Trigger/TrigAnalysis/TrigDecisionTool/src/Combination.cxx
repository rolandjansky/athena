/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : Combination
 *
 * @brief simple container to hold trigger chains
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - LAPP Annecy
 * 
***********************************************************************************/

#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"

Trig::Combination::Combination() 
: m_cgm(0) {}

Trig::Combination::Combination(const std::vector<HLT::TriggerElement*>& tes, const Trig::CacheGlobalMemory* cgm)
  : m_cgm(cgm)
{
  m_tes.insert(m_tes.end(), tes.begin(), tes.end());
}


bool Trig::Combination::active() const {
  for (std::vector<const HLT::TriggerElement*>::const_iterator te = m_tes.begin(); te != m_tes.end(); ++te)
    if ( ! (*te)->getActiveState() )
      return false;
  return true;
}

HLT::NavigationCore* Trig::Combination::navigation() const
{
  return const_cast<HLT::NavigationCore*>(m_cgm->navigation());
}


bool Trig::Combination::operator==(const Trig::Combination& other) const {
  return m_tes == other.m_tes;
}

bool Trig::Combination::operator<(const Trig::Combination& other) const {
  return m_tes < other.m_tes;
}

    
MsgStream& operator<< ( MsgStream& m, const Trig::Combination& c ) {
   m << "TEs: " << c.size();
   for ( std::vector<const HLT::TriggerElement*>::const_iterator i = c.tes().begin();
	 i != c.tes().end(); ++i ) {
     m << " " << Trig::getTEName(**i);
   }
   return m;
}
